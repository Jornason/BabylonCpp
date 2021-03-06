#ifndef BABYLON_MESH_GEOMETRY_PRIMITIVES_PRIMITIVE_H
#define BABYLON_MESH_GEOMETRY_PRIMITIVES_PRIMITIVE_H

#include <babylon/babylon_global.h>

#include <babylon/mesh/geometry.h>

namespace BABYLON {
namespace GeometryPrimitives {

/**
 * @brief
 */
class BABYLON_SHARED_EXPORT _Primitive : public Geometry {

public:
  _Primitive(const std::string& id, Scene* scene, bool canBeRegenerated,
             Mesh* mesh);
  ~_Primitive();

  bool canBeRegenerated() const;
  void regenerate();
  Geometry* asNewGeometry(const std::string& id);

  void setAllVerticesData(VertexData* vertexData, bool updatable);
  Mesh* setVerticesData(unsigned int kind, const Float32Array& data,
                        bool updatable = false, int stride = -1) override;

  virtual std::unique_ptr<VertexData> _regenerateVertexData() = 0;
  virtual Geometry* copy(const std::string& id)               = 0;

private:
  bool _beingRegenerated;
  bool _canBeRegenerated;

}; // end of class _Primitive

} // end of namespace GeometryPrimitives
} // end of namespace BABYLON

#endif // end of BABYLON_MESH_GEOMETRY_PRIMITIVES_PRIMITIVE_H
