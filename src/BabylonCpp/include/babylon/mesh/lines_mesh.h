#ifndef BABYLON_MESH_LINE_MESH_H
#define BABYLON_MESH_LINE_MESH_H

#include <babylon/babylon_global.h>

#include <babylon/math/color3.h>
#include <babylon/mesh/mesh.h>

namespace BABYLON {

/**
 * @brief
 */
class BABYLON_SHARED_EXPORT LinesMesh : public Mesh {

public:
  template <typename... Ts>
  static LinesMesh* New(Ts&&... args)
  {
    auto mesh = new LinesMesh(std::forward<Ts>(args)...);
    mesh->addToScene(static_cast<std::unique_ptr<AbstractMesh>>(mesh));

    return mesh;
  }
  ~LinesMesh();

  /**
   * @brief Returns the string "LineMesh".
   */
  const char* getClassName() const override;

  IReflect::Type type() const override;

  /**
   * @brief The intersection Threshold is the margin applied when intersection a
   * segment of the LinesMesh with a Ray.
   * This margin is expressed in world space coordinates, so its value may vary.
   * Default value is 0.1
   * @returns the intersection Threshold value.
   */
  float intersectionThreshold() const;

  /**
   * @brief The intersection Threshold is the margin applied when intersection a
   * segment of the LinesMesh with a Ray.
   * This margin is expressed in world space coordinates, so its value may vary.
   * @param value the new threshold to apply
   */
  void setIntersectionThreshold(float value);

  Material* getMaterial() override;
  bool checkCollisions();
  InstancedMesh* createInstance(const std::string& name);
  void _bind(SubMesh* subMesh, Effect* effect, unsigned int fillMode) override;
  void _draw(SubMesh* subMesh, int fillMode,
             size_t instancesCount = 0) override;
  PickingInfo intersects(const Ray& ray, bool fastCheck = true) override;
  void dispose(bool doNotRecurse = false) override;

  /**
   * @brief Returns a new LineMesh object cloned from the current one.
   */
  LinesMesh* clone(const std::string& name, Node* newParent,
                   bool doNotCloneChildren);

protected:
  LinesMesh(const std::string& name, Scene* scene, Node* parent = nullptr,
            LinesMesh* source = nullptr, bool doNotCloneChildren = true);

public:
  float dashSize;
  float gapSize;
  Color3 color;
  float alpha;

private:
  float _intersectionThreshold;
  ShaderMaterial* _colorShader;
  std::unordered_map<std::string, VertexBuffer*> _positionBuffer;

}; // end of class LinesMesh

} // end of namespace BABYLON

#endif // end of BABYLON_MESH_LINE_MESH_H
