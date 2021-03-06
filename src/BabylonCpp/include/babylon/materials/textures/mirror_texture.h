#ifndef BABYLON_MATERIALS_TEXTURES_MIRROR_TEXTURE_H
#define BABYLON_MATERIALS_TEXTURES_MIRROR_TEXTURE_H

#include <babylon/babylon_global.h>
#include <babylon/engine/engine.h>
#include <babylon/materials/textures/render_target_texture.h>
#include <babylon/math/matrix.h>
#include <babylon/math/plane.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT MirrorTexture : public RenderTargetTexture {

public:
  MirrorTexture(const std::string& name, const ISize& size, Scene* scene,
                bool generateMipMaps      = false,
                unsigned int type         = Engine::TEXTURETYPE_UNSIGNED_INT,
                unsigned int samplingMode = Texture::BILINEAR_SAMPLINGMODE,
                bool generateDepthBuffer  = true);
  ~MirrorTexture();

  std::unique_ptr<MirrorTexture> clone() const;
  Json::object serialize() const;

public:
  Plane mirrorPlane;

private:
  Matrix _transformMatrix;
  Matrix _mirrorMatrix;
  Matrix _savedViewMatrix;

}; // end of class MirrorTexture

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_TEXTURES_MIRROR_TEXTURE_H
