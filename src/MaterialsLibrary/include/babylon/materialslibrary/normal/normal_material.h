#ifndef BABYLON_MATERIALS_LIBRARY_NORMAL_NORMAL_MATERIAL_H
#define BABYLON_MATERIALS_LIBRARY_NORMAL_NORMAL_MATERIAL_H

#include <babylon/babylon_global.h>
#include <babylon/materials/push_material.h>
#include <babylon/materialslibrary/normal/normal_material_defines.h>
#include <babylon/math/color3.h>
#include <babylon/math/matrix.h>

namespace BABYLON {
namespace MaterialsLibrary {

class BABYLON_SHARED_EXPORT NormalMaterial : public PushMaterial {

public:
  using NMD = NormalMaterialDefines;

public:
  NormalMaterial(const std::string& name, Scene* scene);
  ~NormalMaterial();

  IReflect::Type type() const override;
  void setDiffuseColor(const Color3& color) override;

  bool needAlphaBlending() override;
  bool needAlphaTesting() override;
  BaseTexture* getAlphaTestTexture() override;
  bool isReadyForSubMesh(AbstractMesh* mesh, SubMesh* subMesh,
                         bool useInstances = false) override;
  void bindForSubMesh(Matrix* world, Mesh* mesh, SubMesh* subMesh) override;
  std::vector<IAnimatable*> getAnimatables();
  virtual void dispose(bool forceDisposeEffect   = false,
                       bool forceDisposeTextures = false) override;
  Material* clone(const std::string& name,
                  bool cloneChildren = false) const override;
  Json::object serialize() const;

  /** Statics **/
  static NormalMaterial* Parse(const Json::value& source, Scene* scene,
                               const std::string& rootUrl);

public:
  Color3 diffuseColor;

private:
  BaseTexture* _diffuseTexture;
  bool _disableLighting;
  unsigned int _maxSimultaneousLights;
  Matrix _worldViewProjectionMatrix;
  Color3 _scaledDiffuse;
  int _renderId;

}; // end of class NormalMaterial

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_LIBRARY_NORMAL_NORMAL_MATERIAL_H
