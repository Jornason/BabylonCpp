#ifndef BABYLON_MESH_PRIMITIVES_DISC_H
#define BABYLON_MESH_PRIMITIVES_DISC_H

#include <babylon/babylon_global.h>

#include <babylon/mesh/geometryprimitives/_primitive.h>
#include <babylon/mesh/mesh.h>

namespace BABYLON {
namespace GeometryPrimitives {

/**
 * @brief
 */
class BABYLON_SHARED_EXPORT Disc : public _Primitive {

public:
  template <typename... Ts>
  static Disc* New(Ts&&... args)
  {
    auto disc = new Disc(std::forward<Ts>(args)...);
    disc->addToScene(static_cast<std::unique_ptr<Geometry>>(disc));

    return disc;
  }
  ~Disc();

  std::unique_ptr<VertexData> _regenerateVertexData() override;
  Geometry* copy(const std::string& id) override;

protected:
  Disc(const std::string& id, Scene* scene, float radius,
       unsigned int tessellation, bool canBeRegenerated, Mesh* mesh,
       unsigned int side = Mesh::DEFAULTSIDE);

public:
  float radius;
  unsigned int tessellation;
  unsigned int side;

}; // end of class Disc

} // end of namespace GeometryPrimitives
} // end of namespace BABYLON

#endif // end of BABYLON_MESH_PRIMITIVES_GROUND_H
