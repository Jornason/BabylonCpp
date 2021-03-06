#ifndef BABYLON_MESH_INSTANCED_MESH_H
#define BABYLON_MESH_INSTANCED_MESH_H

#include <babylon/babylon_global.h>
#include <babylon/mesh/abstract_mesh.h>

namespace BABYLON {

/**
 * @brief Represents an instance based on a source mesh.
 */
class BABYLON_SHARED_EXPORT InstancedMesh : public AbstractMesh {

public:
  template <typename... Ts>
  static InstancedMesh* New(Ts&&... args)
  {
    auto mesh = new InstancedMesh(std::forward<Ts>(args)...);
    mesh->addToScene(static_cast<std::unique_ptr<AbstractMesh>>(mesh));

    return mesh;
  }
  ~InstancedMesh();

  /**
   * @brief Returns the string "InstancedMesh"
   */
  const char* getClassName() const override;

  IReflect::Type type() const override;

  /** Methods **/
  bool receiveShadows() const;
  Material* material() const;
  float visibility() const;
  Skeleton* skeleton() override;
  unsigned int renderingGroupId() const;

  /**
   * @brief Returns the total number of vertices (integer).
   */
  size_t getTotalVertices() const override;

  Mesh* sourceMesh() const;

  /**
   * @brief Returns Float32Array of the requested kind of data : positons,
   * normals, uvs, etc.
   */
  Float32Array getVerticesData(unsigned int kind,
                               bool copyWhenShared = false) override;

  /**
   * @brief Sets the vertex data of the mesh geometry for the requested `kind`.
   * If the mesh has no geometry, a new Geometry object is set to the mesh and
   * then passed this vertex data.
   * The `data` are either a numeric array either a Float32Array.
   * The parameter `updatable` is passed as is to the underlying Geometry object
   * constructor (if initianilly none) or updater.
   * The parameter `stride` is an optional positive integer, it is usually
   * automatically deducted from the `kind` (3 for positions or normals, 2 for
   * UV, etc).
   * Note that a new underlying VertexBuffer object is created each call.
   * If the `kind` is the `PositionKind`, the mesh BoundingInfo is renewed, so
   * the bounding box and sphere, and the mesh World Matrix is recomputed.
   *
   * Possible `kind` values :
   * - BABYLON.VertexBuffer.PositionKind
   * - BABYLON.VertexBuffer.UVKind
   * - BABYLON.VertexBuffer.UV2Kind
   * - BABYLON.VertexBuffer.UV3Kind
   * - BABYLON.VertexBuffer.UV4Kind
   * - BABYLON.VertexBuffer.UV5Kind
   * - BABYLON.VertexBuffer.UV6Kind
   * - BABYLON.VertexBuffer.ColorKind
   * - BABYLON.VertexBuffer.MatricesIndicesKind
   * - BABYLON.VertexBuffer.MatricesIndicesExtraKind
   * - BABYLON.VertexBuffer.MatricesWeightsKind
   * - BABYLON.VertexBuffer.MatricesWeightsExtraKind
   *
   * @returns The Mesh.
   */
  Mesh* setVerticesData(unsigned int kind, const Float32Array& data,
                        bool updatable = false, int stride = -1) override;

  /**
   * @brief Updates the existing vertex data of the mesh geometry for the
   * requested `kind`.
   * If the mesh has no geometry, it is simply returned as it is.
   * The `data` are either a numeric array either a Float32Array.
   * No new underlying VertexBuffer object is created.
   * If the `kind` is the `PositionKind` and if `updateExtends` is true, the
   * mesh BoundingInfo is renewed, so the bounding box and sphere, and the mesh
   * World Matrix is recomputed.
   * If the parameter `makeItUnique` is true, a new global geometry is created
   * from this positions and is set to the mesh.
   *
   * Possible `kind` values :
   * - BABYLON.VertexBuffer.PositionKind
   * - BABYLON.VertexBuffer.UVKind
   * - BABYLON.VertexBuffer.UV2Kind
   * - BABYLON.VertexBuffer.UV3Kind
   * - BABYLON.VertexBuffer.UV4Kind
   * - BABYLON.VertexBuffer.UV5Kind
   * - BABYLON.VertexBuffer.UV6Kind
   * - BABYLON.VertexBuffer.ColorKind
   * - BABYLON.VertexBuffer.MatricesIndicesKind
   * - BABYLON.VertexBuffer.MatricesIndicesExtraKind
   * - BABYLON.VertexBuffer.MatricesWeightsKind
   * - BABYLON.VertexBuffer.MatricesWeightsExtraKind
   *
   * @returns The Mesh.
   */
  Mesh* updateVerticesData(unsigned int kind, const Float32Array& data,
                           bool updateExtends = false,
                           bool makeItUnique  = false) override;

  /**
   * @brief Sets the mesh indices.
   * Expects an array populated with integers or a typed array (Int32Array,
   * Uint32Array, Uint16Array).
   * If the mesh has no geometry, a new Geometry object is created and set to
   * the mesh.
   * This method creates a new index buffer each call.
   *
   * @returns The Mesh.
   */
  Mesh* setIndices(const IndicesArray& indices,
                   size_t totalVertices = 0) override;

  /**
   * @brief Returns if the mesh owns the requested kind of data.
   */
  bool isVerticesDataPresent(unsigned int kind) override;

  /**
   * @brief Returns an array of indices (IndicesArray).
   */
  IndicesArray getIndices(bool copyWhenShared = false) override;

  std::vector<Vector3> _positions();

  /**
   * @brief Sets a new updated BoundingInfo to the mesh.
   * @returns The mesh.
   */
  InstancedMesh& refreshBoundingInfo();

  void _preActivate() override;

  void _activate(int renderId) override;

  /**
   * @brief Returns the current associated LOD AbstractMesh.
   */
  AbstractMesh* getLOD(Camera* camera,
                       BoundingSphere* boundingSphere = nullptr) override;

  InstancedMesh& _syncSubMeshes();

  bool _generatePointsArray() override;

  /**
   * @brief Creates a new InstancedMesh from the current mesh.
   * @param name (string) : the cloned mesh name
   * @param  newParent (optional Node) : the optional Node to parent the clone
   * to.
   * @param  doNotCloneChildren (optional boolean, default `false`) : if `true`
   * the model children aren't cloned.
   * @returns The clone.
   */
  InstancedMesh* clone(const std::string& name, Node* newParent,
                       bool doNotCloneChildren = false);

  /**
   * @brief Disposes the InstancedMesh.
   */
  void dispose(bool doNotRecurse = false) override;

protected:
  InstancedMesh(const std::string& name, Mesh* source);

private:
  Mesh* _sourceMesh;
  Mesh* _currentLOD;

}; // end of class InstancedMesh

} // end of namespace BABYLON

#endif // end of BABYLON_MESH_INSTANCED_MESH_H
