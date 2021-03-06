#include <babylon/rendering/rendering_manager.h>

#include <babylon/cameras/camera.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/rendering_group_info.h>
#include <babylon/mesh/sub_mesh.h>
#include <babylon/particles/particle_system.h>
#include <babylon/rendering/rendering_group.h>
#include <babylon/sprites/sprite_manager.h>

namespace BABYLON {

RenderingManager::RenderingManager(Scene* scene)
    : _scene{scene}, _renderinGroupInfo{nullptr}
{
  _autoClearDepthStencil.resize(MAX_RENDERINGGROUPS);
  _customOpaqueSortCompareFn.resize(MAX_RENDERINGGROUPS);
  _customAlphaTestSortCompareFn.resize(MAX_RENDERINGGROUPS);
  _customTransparentSortCompareFn.resize(MAX_RENDERINGGROUPS);

  for (unsigned int i = RenderingManager::MIN_RENDERINGGROUPS;
       i < RenderingManager::MAX_RENDERINGGROUPS; ++i) {
    _autoClearDepthStencil[i]          = {true, true, true};
    _customOpaqueSortCompareFn[i]      = nullptr;
    _customAlphaTestSortCompareFn[i]   = nullptr;
    _customTransparentSortCompareFn[i] = nullptr;
  }
}

RenderingManager::~RenderingManager()
{
}

void RenderingManager::_clearDepthStencilBuffer(bool depth, bool stencil)
{
  if (_depthStencilBufferAlreadyCleaned) {
    return;
  }

  _scene->getEngine()->clear(depth, stencil);
  _depthStencilBufferAlreadyCleaned = true;
}

void RenderingManager::render(
  std::function<void(const std::vector<SubMesh*>& opaqueSubMeshes,
                     const std::vector<SubMesh*>& transparentSubMeshes,
                     const std::vector<SubMesh*>& alphaTestSubMeshes)>
    customRenderFunction,
  const std::vector<AbstractMesh*>& activeMeshes, bool renderParticles,
  bool renderSprites)
{
  // Check if there's at least on observer on the onRenderingGroupObservable and
  // initialize things to fire it
  bool hasObservable = _scene->onRenderingGroupObservable.hasObservers();
  if (hasObservable) {
    if (!_renderinGroupInfo) {
      _renderinGroupInfo = std_util::make_unique<RenderingGroupInfo>();
    }
    _renderinGroupInfo->scene  = _scene;
    _renderinGroupInfo->camera = _scene->activeCamera;
  }

  // Dispatch sprites
  if (renderSprites) {
    for (auto& manager : _scene->spriteManagers) {
      dispatchSprites(manager.get());
    }
  }

  // Render
  auto info = _renderinGroupInfo.get();
  for (unsigned int index = RenderingManager::MIN_RENDERINGGROUPS;
       index < RenderingManager::MAX_RENDERINGGROUPS; ++index) {
    _depthStencilBufferAlreadyCleaned
      = (index == RenderingManager::MIN_RENDERINGGROUPS);
    RenderingGroup* renderingGroup = (index < _renderingGroups.size()) ?
                                       _renderingGroups[index].get() :
                                       nullptr;
    if (!renderingGroup && !hasObservable) {
      continue;
    }

    _currentIndex = index;

    int renderingGroupMask = 0;

    // Fire PRECLEAR stage
    if (hasObservable) {
      renderingGroupMask     = static_cast<int>(std::pow(2, index));
      info->renderStage      = RenderingGroupInfo::STAGE_PRECLEAR;
      info->renderingGroupId = index;
      _scene->onRenderingGroupObservable.notifyObservers(info,
                                                         renderingGroupMask);
    }

    // Clear depth/stencil if needed
    if (index < _autoClearDepthStencil.size()) {
      auto& autoClear = _autoClearDepthStencil[index];
      if (autoClear.autoClear) {
        _clearDepthStencilBuffer(autoClear.depth, autoClear.stencil);
      }
    }

    if (hasObservable) {
      // Fire PREOPAQUE stage
      info->renderStage = RenderingGroupInfo::STAGE_PREOPAQUE;
      _scene->onRenderingGroupObservable.notifyObservers(info,
                                                         renderingGroupMask);
      // Fire PRETRANSPARENT stage
      info->renderStage = RenderingGroupInfo::STAGE_PRETRANSPARENT;
      _scene->onRenderingGroupObservable.notifyObservers(info,
                                                         renderingGroupMask);
    }

    if (renderingGroup) {
      renderingGroup->render(customRenderFunction, renderSprites,
                             renderParticles, activeMeshes);
    }

    // Fire POSTTRANSPARENT stage
    if (hasObservable) {
      info->renderStage = RenderingGroupInfo::STAGE_POSTTRANSPARENT;
      _scene->onRenderingGroupObservable.notifyObservers(info,
                                                         renderingGroupMask);
    }
  }
}

void RenderingManager::reset()
{
  for (unsigned index = RenderingManager::MIN_RENDERINGGROUPS;
       index < RenderingManager::MAX_RENDERINGGROUPS; ++index) {
    if (index < _renderingGroups.size()) {
      auto& renderingGroup = _renderingGroups[index];
      if (renderingGroup) {
        renderingGroup->prepare();
      }
    }
  }
}

void RenderingManager::dispose()
{
  for (unsigned index = RenderingManager::MIN_RENDERINGGROUPS;
       index < RenderingManager::MAX_RENDERINGGROUPS; ++index) {
    if (index < _renderingGroups.size()) {
      auto& renderingGroup = _renderingGroups[index];
      if (renderingGroup) {
        renderingGroup->dispose();
      }
    }
  }
  _renderingGroups.clear();
}

void RenderingManager::_prepareRenderingGroup(unsigned int renderingGroupId)
{
  // Resize render groups vector if needed
  if (renderingGroupId >= _renderingGroups.size()) {
    for (size_t i = _renderingGroups.size(); i <= renderingGroupId; ++i) {
      _renderingGroups.emplace_back(nullptr);
    }
  }

  if (!_renderingGroups[renderingGroupId]) {
    _renderingGroups[renderingGroupId]
      = std::unique_ptr<RenderingGroup>(new RenderingGroup(
        renderingGroupId, _scene, _customOpaqueSortCompareFn[renderingGroupId],
        _customAlphaTestSortCompareFn[renderingGroupId],
        _customTransparentSortCompareFn[renderingGroupId]));
  }
}

void RenderingManager::dispatchSprites(SpriteManager* spriteManager)
{
  const auto& renderingGroupId = spriteManager->renderingGroupId;

  _prepareRenderingGroup(renderingGroupId);

  _renderingGroups[renderingGroupId]->dispatchSprites(spriteManager);
}

void RenderingManager::dispatchParticles(ParticleSystem* particleSystem)
{
  const auto& renderingGroupId = particleSystem->renderingGroupId;

  _prepareRenderingGroup(renderingGroupId);

  _renderingGroups[renderingGroupId]->dispatchParticles(particleSystem);
}

void RenderingManager::dispatch(SubMesh* subMesh)
{
  auto mesh                    = subMesh->getMesh();
  const auto& renderingGroupId = mesh->renderingGroupId;

  _prepareRenderingGroup(renderingGroupId);

  _renderingGroups[renderingGroupId]->dispatch(subMesh);
}

void RenderingManager::setRenderingOrder(
  unsigned int renderingGroupId,
  const std::function<int(SubMesh* a, SubMesh* b)>& opaqueSortCompareFn,
  const std::function<int(SubMesh* a, SubMesh* b)>& alphaTestSortCompareFn,
  const std::function<int(SubMesh* a, SubMesh* b)>& transparentSortCompareFn)
{
  _customOpaqueSortCompareFn[renderingGroupId]      = opaqueSortCompareFn;
  _customAlphaTestSortCompareFn[renderingGroupId]   = alphaTestSortCompareFn;
  _customTransparentSortCompareFn[renderingGroupId] = transparentSortCompareFn;

  if (_renderingGroups[renderingGroupId]) {
    auto& group = _renderingGroups[renderingGroupId];
    group->setOpaqueSortCompareFn(_customOpaqueSortCompareFn[renderingGroupId]);
    group->setAlphaTestSortCompareFn(
      _customAlphaTestSortCompareFn[renderingGroupId]);
    group->setTransparentSortCompareFn(
      _customTransparentSortCompareFn[renderingGroupId]);
  }
}

void RenderingManager::setRenderingAutoClearDepthStencil(
  unsigned int renderingGroupId, bool autoClearDepthStencil, bool depth,
  bool stencil)
{
  _autoClearDepthStencil[renderingGroupId]
    = {autoClearDepthStencil, depth, stencil};
}

} // end of namespace BABYLON
