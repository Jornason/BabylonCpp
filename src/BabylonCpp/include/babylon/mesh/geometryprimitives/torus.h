#ifndef BABYLON_MESH_PRIMITIVES_TORUS_H
#define BABYLON_MESH_PRIMITIVES_TORUS_H

#include <babylon/babylon_global.h>

#include <babylon/mesh/geometryprimitives/_primitive.h>
#include <babylon/mesh/mesh.h>

namespace BABYLON {
namespace GeometryPrimitives {

/**
 * @brief
 */
class BABYLON_SHARED_EXPORT Torus : public _Primitive {

public:
  template <typename... Ts>
  static Torus* New(Ts&&... args)
  {
    auto torus = new Torus(std::forward<Ts>(args)...);
    torus->addToScene(static_cast<std::unique_ptr<Geometry>>(torus));

    return torus;
  }
  ~Torus();

  std::unique_ptr<VertexData> _regenerateVertexData() override;
  Geometry* copy(const std::string& id) override;

  // Statics
  static Torus* Parse(const Json::value& parsedTorus, Scene* scene);

protected:
  Torus(const std::string& id, Scene* scene, float diameter, float thickness,
        unsigned int tessellation, bool canBeRegenerated, Mesh* mesh,
        unsigned int side = Mesh::DEFAULTSIDE);

private:
  float diameter;
  float thickness;
  unsigned int tessellation;
  unsigned int side;

}; // end of class Torus

} // end of namespace GeometryPrimitives
} // end of namespace BABYLON

#endif // end of BABYLON_MESH_PRIMITIVES_TORUS_H
