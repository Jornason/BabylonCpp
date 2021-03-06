#include <babylon/extensions/treegenerators/quick_tree_generator.h>

#include <babylon/core/random.h>
#include <babylon/mesh/mesh.h>
#include <babylon/mesh/vertex_buffer.h>
#include <babylon/mesh/vertex_data.h>
#include <babylon/mesh/vertex_data_options.h>
#include <babylon/tools/tools.h>

namespace BABYLON {
namespace Extensions {

Mesh* QuickTreeGenerator::CreateTree(float sizeBranch, float sizeTrunk,
                                     float radius, Material* trunkMaterial,
                                     Material* leafMaterial, Scene* scene)
{
  auto tree       = Mesh::New("tree", scene);
  tree->isVisible = false;

  auto leaves = Mesh::New("leaves", scene);

  SphereOptions options(sizeBranch);
  options.segments = 2;
  auto vertexData  = VertexData::CreateSphere(options);

  vertexData->applyToMesh(leaves, false);

  auto positions      = leaves->getVerticesData(VertexBuffer::PositionKind);
  auto indices        = leaves->getIndices();
  auto numberOfPoints = positions.size() / 3;

  std::vector<Vector3> vects;
  std::vector<std::vector<size_t>> sizes;

  // The higher point in the sphere
  std::vector<Vector3> max;

  for (size_t i = 0; i < numberOfPoints; ++i) {
    Vector3 p(positions[i * 3], positions[i * 3 + 1], positions[i * 3 + 2]);

    if (p.y >= sizeBranch / 2.f) {
      max.emplace_back(p);
    }

    bool found = false;
    for (size_t index = 0; index < vects.size() && !found; ++index) {
      Vector3 p0 = vects[0];
      if (p0.equals(p) || (p0.subtract(p)).lengthSquared() < 0.01f) {
        sizes[index].emplace_back(i * 3);
        found = true;
      }
    }
    if (!found) {
      std::vector<size_t> array;
      vects.emplace_back(p);
      array.emplace_back(i * 3);
      sizes.emplace_back(array);
    }
  }

  auto randomNumber = [](float min, float max) {
    if (std_util::almost_equal(min, max)) {
      return min;
    }
    auto random = Math::random();
    return ((random * (max - min)) + min);
  };

  for (size_t v = 0; v < vects.size(); ++v) {
    auto _min = -sizeBranch / 10.f, _max = sizeBranch / 10.f;
    auto rx = randomNumber(_min, _max);
    auto ry = randomNumber(_min, _max);
    auto rz = randomNumber(_min, _max);

    for (unsigned int index = 0; index < sizes[v].size(); ++index) {
      size_t i = sizes[v][index];
      positions[i] += rx;
      positions[i + 1] += ry;
      positions[i + 2] += rz;
    }
  };

  leaves->setVerticesData(VertexBuffer::PositionKind, positions);
  Float32Array normals(positions.size());
  VertexData::ComputeNormals(positions, indices, normals);
  leaves->setVerticesData(VertexBuffer::NormalKind, normals);
  leaves->convertToFlatShadedMesh();

  leaves->setMaterial(leafMaterial);
  leaves->position().y = sizeTrunk + sizeBranch / 2.f - 2.f;

  auto trunk = Mesh::CreateCylinder("trunk", sizeTrunk,
                                    radius - 2.f < 1.f ? 1.f : radius - 2.f,
                                    radius, 10, 1, scene);

  trunk->position().y = (sizeBranch / 2.f + 2.f) - sizeTrunk / 2.f;

  trunk->setMaterial(trunkMaterial);
  trunk->convertToFlatShadedMesh();

  leaves->setParent(tree);
  trunk->setParent(tree);
  return tree;
}

} // end of namespace Extensions
} // end of namespace BABYLON
