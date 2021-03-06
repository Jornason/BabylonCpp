#ifndef BABYLON_MESH_PRIMITIVES_PLANE_H
#define BABYLON_MESH_PRIMITIVES_PLANE_H

#include <babylon/babylon_global.h>

#include <babylon/mesh/geometryprimitives/_primitive.h>
#include <babylon/mesh/mesh.h>

namespace BABYLON {
namespace GeometryPrimitives {

/**
 * @brief
 */
class BABYLON_SHARED_EXPORT Plane : public _Primitive {

public:
  template <typename... Ts>
  static Plane* New(Ts&&... args)
  {
    auto plane = new Plane(std::forward<Ts>(args)...);
    plane->addToScene(static_cast<std::unique_ptr<Geometry>>(plane));

    return plane;
  }
  ~Plane();

  std::unique_ptr<VertexData> _regenerateVertexData() override;
  Geometry* copy(const std::string& id) override;

  // Statics
  static Plane* Parse(const Json::value& parsedPlane, Scene* scene);

protected:
  Plane(const std::string& id, Scene* scene, float size, bool canBeRegenerated,
        Mesh* mesh, unsigned int side = Mesh::DEFAULTSIDE);

public:
  float size;
  unsigned int side;

}; // end of class Plane

} // end of namespace GeometryPrimitives
} // end of namespace BABYLON

#endif // end of BABYLON_MESH_PRIMITIVES_PLANE_H
