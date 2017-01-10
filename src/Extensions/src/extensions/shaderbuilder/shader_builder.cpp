#include <babylon/extensions/shaderbuilder/shader_builder.h>

#include <babylon/core/string.h>
#include <babylon/extensions/shaderbuilder/helper.h>
#include <babylon/extensions/shaderbuilder/icolor.h>
#include <babylon/extensions/shaderbuilder/ieffect.h>
#include <babylon/extensions/shaderbuilder/ilight.h>
#include <babylon/extensions/shaderbuilder/imap.h>
#include <babylon/extensions/shaderbuilder/inut.h>
#include <babylon/extensions/shaderbuilder/irange.h>
#include <babylon/extensions/shaderbuilder/ireflect_map.h>
#include <babylon/extensions/shaderbuilder/ireplace_color.h>
#include <babylon/extensions/shaderbuilder/shader.h>
#include <babylon/extensions/shaderbuilder/shader_material_helper_statics.h>

namespace BABYLON {
namespace Extensions {

ShaderBuilder::ShaderBuilder()
{
}

ShaderBuilder::~ShaderBuilder()
{
}

void ShaderBuilder::PrepareBeforeMaterialBuild()
{

  Attributes.emplace_back(ShaderMaterialHelperStatics::AttrPosition);
  Attributes.emplace_back(ShaderMaterialHelperStatics::AttrNormal);
  if (Setting.Uv) {
    Attributes.emplace_back(ShaderMaterialHelperStatics::AttrUv);
  }
  if (Setting.Uv2) {
    Attributes.emplace_back(ShaderMaterialHelperStatics::AttrUv2);
  }

  std_util::concat(Uniforms,
                   {ShaderMaterialHelperStatics::uniformView,
                    ShaderMaterialHelperStatics::uniformWorld,
                    ShaderMaterialHelperStatics::uniformWorldView,
                    ShaderMaterialHelperStatics::uniformViewProjection,
                    ShaderMaterialHelperStatics::uniformWorldViewProjection});

  /** Start Build Vertex Frame **/
  Vertex.emplace_back("precision " + Setting.PrecisionMode + " float;");
  Vertex.emplace_back("attribute "
                      + ShaderMaterialHelperStatics::AttrTypeForPosition + " "
                      + ShaderMaterialHelperStatics::AttrPosition + ";");
  Vertex.emplace_back("attribute "
                      + ShaderMaterialHelperStatics::AttrTypeForNormal + " "
                      + ShaderMaterialHelperStatics::AttrNormal + ";");

  if (Setting.Uv) {
    Vertex.emplace_back("attribute "
                        + ShaderMaterialHelperStatics::AttrTypeForUv + " "
                        + ShaderMaterialHelperStatics::AttrUv + ";");

    Vertex.emplace_back("varying vec2 " + ShaderMaterialHelperStatics::Uv
                        + ";");
  }
  if (Setting.Uv2) {
    Vertex.emplace_back("attribute "
                        + ShaderMaterialHelperStatics::AttrTypeForUv2 + " "
                        + ShaderMaterialHelperStatics::AttrUv2 + ";");

    Vertex.emplace_back("varying vec2 " + ShaderMaterialHelperStatics::Uv2
                        + ";");
  }

  Vertex.emplace_back("varying vec3 " + ShaderMaterialHelperStatics::Position
                      + ";");
  Vertex.emplace_back("varying vec3 " + ShaderMaterialHelperStatics::Normal
                      + ";");

  Vertex.emplace_back(
    "uniform   " + ShaderMaterialHelperStatics::uniformStandardType + " "
    + ShaderMaterialHelperStatics::uniformWorldViewProjection + ";");
  if (Setting.VertexView) {
    Vertex.emplace_back("uniform   "
                        + ShaderMaterialHelperStatics::uniformStandardType + " "
                        + ShaderMaterialHelperStatics::uniformView + ";");
  }

  if (Setting.VertexWorld) {
    Vertex.emplace_back("uniform   "
                        + ShaderMaterialHelperStatics::uniformStandardType + " "
                        + ShaderMaterialHelperStatics::uniformWorld + ";");
  }

  if (Setting.VertexViewProjection) {
    Vertex.emplace_back(
      "uniform   " + ShaderMaterialHelperStatics::uniformStandardType + " "
      + ShaderMaterialHelperStatics::uniformViewProjection + ";");
  }

  if (Setting.Flags) {
    Uniforms.emplace_back(ShaderMaterialHelperStatics::uniformFlags);

    Vertex.emplace_back("uniform  float "
                        + ShaderMaterialHelperStatics::uniformFlags + ";");
  }

  if (Setting.VertexWorldView) {
    Vertex.emplace_back("uniform   "
                        + ShaderMaterialHelperStatics::uniformStandardType + " "
                        + ShaderMaterialHelperStatics::uniformWorldView + ";");
  }

  if (!VertexUniforms.empty()) {
    Vertex.emplace_back(VertexUniforms);
  }

  /*#extension GL_OES_standard_derivatives : enable*/
  Fragment.emplace_back(
    "precision " + Setting.PrecisionMode
    + " float;\n#extension GL_OES_standard_derivatives : enable\n\n\n");

  if (Setting.Uv) {
    Fragment.emplace_back("varying vec2 " + ShaderMaterialHelperStatics::Uv
                          + ";");
  }
  if (Setting.Uv2) {
    Fragment.emplace_back("varying vec2 " + ShaderMaterialHelperStatics::Uv2
                          + ";");
  }

  if (Setting.FragmentView) {
    Fragment.emplace_back(
      "uniform   " + ShaderMaterialHelperStatics::uniformStandardType + " "
      + ShaderMaterialHelperStatics::uniformView + ";");
  }

  if (Setting.FragmentWorld) {
    Fragment.emplace_back(
      "uniform   " + ShaderMaterialHelperStatics::uniformStandardType + " "
      + ShaderMaterialHelperStatics::uniformWorld + ";");
  }

  if (Setting.FragmentViewProjection) {
    Fragment.emplace_back(
      "uniform   " + ShaderMaterialHelperStatics::uniformStandardType + " "
      + ShaderMaterialHelperStatics::uniformViewProjection + ";");
  }

  if (Setting.FragmentWorldView) {
    Fragment.emplace_back(
      "uniform   " + ShaderMaterialHelperStatics::uniformStandardType + " "
      + ShaderMaterialHelperStatics::uniformWorldView + ";");
  }

  if (Setting.Flags) {
    Fragment.emplace_back("uniform  float "
                          + ShaderMaterialHelperStatics::uniformFlags + ";");
  }

  if (!FragmentUniforms.empty()) {
    Fragment.emplace_back(FragmentUniforms);
  }
  Fragment.emplace_back("varying vec3 " + ShaderMaterialHelperStatics::Position
                        + ";");
  Fragment.emplace_back("varying vec3 " + ShaderMaterialHelperStatics::Normal
                        + ";");

  if (Setting.WorldPosition) {
    Vertex.emplace_back("varying vec3 "
                        + ShaderMaterialHelperStatics::WorldPosition + ";");
    Vertex.emplace_back("varying vec3 "
                        + ShaderMaterialHelperStatics::WorldNormal + ";");

    Fragment.emplace_back("varying vec3 "
                          + ShaderMaterialHelperStatics::WorldPosition + ";");
    Fragment.emplace_back("varying vec3 "
                          + ShaderMaterialHelperStatics::WorldNormal + ";");
  }

  if (!Setting.Texture2Ds.empty()) {
    unsigned int cnt = 0;
    for (auto& texture2D : Setting.Texture2Ds) {
      std::string s = std::to_string(cnt);
      if (texture2D.inVertex) {
        Vertex.emplace_back("uniform  sampler2D "
                            + ShaderMaterialHelperStatics::Texture2D + s + ";");
      }
      if (texture2D.inFragment) {
        Fragment.emplace_back("uniform  sampler2D  "
                              + ShaderMaterialHelperStatics::Texture2D + s
                              + ";");
      }
      ++cnt;
    }
  }

  if (Setting.CameraShot) {
    Fragment.emplace_back("uniform  sampler2D  textureSampler;");
  }

  if (!Setting.TextureCubes.empty()) {
    unsigned int cnt = 0;
    for (auto& textureCube : Setting.TextureCubes) {
      std::string s = std::to_string(cnt);
      if (textureCube.inVertex) {
        Vertex.emplace_back("uniform  samplerCube  "
                            + ShaderMaterialHelperStatics::TextureCube + s
                            + ";");
      }
      if (textureCube.inFragment) {
        Fragment.emplace_back("uniform  samplerCube   "
                              + ShaderMaterialHelperStatics::TextureCube + s
                              + ";");
      }
      ++cnt;
    }
  }

  if (Setting.Center) {
    Vertex.emplace_back("uniform  vec3 " + ShaderMaterialHelperStatics::Center
                        + ";");
    Fragment.emplace_back("uniform  vec3 " + ShaderMaterialHelperStatics::Center
                          + ";");
  }
  if (Setting.Mouse) {
    Vertex.emplace_back("uniform  vec2 " + ShaderMaterialHelperStatics::Mouse
                        + ";");
    Fragment.emplace_back("uniform  vec2 " + ShaderMaterialHelperStatics::Mouse
                          + ";");
  }
  if (Setting.Screen) {
    Vertex.emplace_back("uniform  vec2 " + ShaderMaterialHelperStatics::Screen
                        + ";");
    Fragment.emplace_back("uniform  vec2 " + ShaderMaterialHelperStatics::Screen
                          + ";");
  }
  if (Setting.Camera) {
    Vertex.emplace_back("uniform  vec3 " + ShaderMaterialHelperStatics::Camera
                        + ";");
    Fragment.emplace_back("uniform  vec3 " + ShaderMaterialHelperStatics::Camera
                          + ";");
  }
  if (Setting.Look) {
    Vertex.emplace_back("uniform  vec3 " + ShaderMaterialHelperStatics::Look
                        + ";");
    Fragment.emplace_back("uniform  vec3 " + ShaderMaterialHelperStatics::Look
                          + ";");
  }
  if (Setting.Time) {
    Vertex.emplace_back("uniform  float " + ShaderMaterialHelperStatics::Time
                        + ";");
    Fragment.emplace_back("uniform  float " + ShaderMaterialHelperStatics::Time
                          + ";");
  }
  if (Setting.GlobalTime) {
    Vertex.emplace_back("uniform  vec4 "
                        + ShaderMaterialHelperStatics::GlobalTime + ";");
    Fragment.emplace_back("uniform  vec4 "
                          + ShaderMaterialHelperStatics::GlobalTime + ";");
  }
  if (Setting.ReflectMatrix) {
    Vertex.emplace_back("uniform  mat4 "
                        + ShaderMaterialHelperStatics::ReflectMatrix + ";");
    Fragment.emplace_back("uniform  mat4 "
                          + ShaderMaterialHelperStatics::ReflectMatrix + ";");
  }
  if (Setting.Helpers) {
    constexpr const char* helpers
      = "vec3 random3(vec3 c) {\n"
        "  float j = 4096.0 * sin(dot(c, vec3(17.0, 59.4, 15.0)));\n"
        "  vec3 r;\n"
        "  r.z = fract(512.0 * j);\n"
        "  j *= .125;\n"
        "  r.x = fract(512.0 * j);\n"
        "  j *= .125;\n"
        "  r.y = fract(512.0 * j);\n"
        "  return r - 0.5;\n"
        "}\n"
        "float rand(vec2 co) {\n"
        "  return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);\n"
        "}\n"
        "const float F3 = 0.3333333;\n"
        "const float G3 = 0.1666667;\n"
        "float simplex3d(vec3 p) {\n"
        "  vec3 s = floor(p + dot(p, vec3(F3)));\n"
        "  vec3 x = p - s + dot(s, vec3(G3));\n"
        "  vec3 e = step(vec3(0.0), x - x.yzx);\n"
        "  vec3 i1 = e * (1.0 - e.zxy);\n"
        "  vec3 i2 = 1.0 - e.zxy * (1.0 - e);\n"
        "  vec3 x1 = x - i1 + G3;\n"
        "  vec3 x2 = x - i2 + 2.0 * G3;\n"
        "  vec3 x3 = x - 1.0 + 3.0 * G3;\n"
        "  vec4 w, d;\n"
        "  w.x = dot(x, x);\n"
        "  w.y = dot(x1, x1);\n"
        "  w.z = dot(x2, x2);\n"
        "  w.w = dot(x3, x3);\n"
        "  w = max(0.6 - w, 0.0);\n"
        "  d.x = dot(random3(s), x);\n"
        "  d.y = dot(random3(s + i1), x1);\n"
        "  d.z = dot(random3(s + i2), x2);\n"
        "  d.w = dot(random3(s + 1.0), x3);\n"
        "  w *= w;\n"
        "  w *= w;\n"
        "  d *= w;\n"
        "  return dot(d, vec4(52.0));\n"
        "}\n"
        "float noise(vec3 m) {\n"
        "  return 0.5333333 * simplex3d(m) +\n"
        "         0.2666667 * simplex3d(2.0 * m) +\n"
        "         0.1333333 * simplex3d(4.0 * m) +\n"
        "         0.0666667 * simplex3d(8.0 * m);\n"
        "}\n"
        "float dim(vec3 p1, vec3 p2) {\n"
        "  return sqrt((p2.x - p1.x) * (p2.x - p1.x) +\n"
        "              (p2.y - p1.y) * (p2.y - p1.y) +\n"
        "              (p2.z - p1.z) * (p2.z - p1.z));\n"
        "}\n"
        "vec2 rotate_xy(vec2 pr1, vec2 pr2, float alpha) {\n"
        "  vec2 pp2 = vec2(pr2.x - pr1.x, pr2.y - pr1.y);\n"
        "  return vec2(pr1.x + pp2.x * cos(alpha * 3.14159265 / 180.) -\n"
        "                  pp2.y * sin(alpha * 3.14159265 / 180.),\n"
        "              pr1.y + pp2.x * sin(alpha * 3.14159265 / 180.) +\n"
        "                  pp2.y * cos(alpha * 3.14159265 / 180.));\n"
        "}\n"
        "vec3 r_y(vec3 n, float a, vec3 c) {\n"
        "  vec3 c1 = vec3(c.x, c.y, c.z);\n"
        "  c1.x = c1.x;\n"
        "  c1.y = c1.z;\n"
        "  vec2 p = rotate_xy(vec2(c1.x, c1.y), vec2(n.x, n.z), a);\n"
        "  n.x = p.x;\n"
        "  n.z = p.y;\n"
        "  return n;\n"
        "}\n"
        "vec3 r_x(vec3 n, float a, vec3 c) {\n"
        "  vec3 c1 = vec3(c.x, c.y, c.z);\n"
        "  c1.x = c1.y;\n"
        "  c1.y = c1.z;\n"
        "  vec2 p = rotate_xy(vec2(c1.x, c1.y), vec2(n.y, n.z), a);\n"
        "  n.y = p.x;\n"
        "  n.z = p.y;\n"
        "  return n;\n"
        "}\n"
        "vec3 r_z(vec3 n, float a, vec3 c) {\n"
        "  vec3 c1 = vec3(c.x, c.y, c.z);\n"
        "  vec2 p = rotate_xy(vec2(c1.x, c1.y), vec2(n.x, n.y), a);\n"
        "  n.x = p.x;\n"
        "  n.y = p.y;\n"
        "  return n;\n"
        "}\n";
    Vertex.emplace_back(helpers);
    Fragment.emplace_back(helpers);
  }

  std::ostringstream oss;
  oss << "void main(void) {\n";
  oss << ShaderMaterialHelperStatics::Position << " = ";
  oss << ShaderMaterialHelperStatics::AttrPosition << ";\n";
  oss << ShaderMaterialHelperStatics::Normal << " = ";
  oss << ShaderMaterialHelperStatics::AttrNormal << ";\n";
  oss << "vec4 result = vec4(";
  oss << ShaderMaterialHelperStatics::Position + ",1.);\n";
  oss << "vuv = uv;\n";
  oss << "#[Source]\n";
  oss << "gl_Position = worldViewProjection * result;\n";
  oss << "#[AfterFinishVertex]\n";
  oss << "}";
  Vertex.emplace_back(oss.str());

  /** start Build Fragment Frame **/
  if (!Setting.NormalMap.empty()) {
    oss.clear();
    oss << "vec3 normalMap() { vec4 result = vec4(0.); ";
    oss << Setting.NormalMap << ";\n";
    oss << "result = vec4( normalize( " << Setting.Normal;
    oss << " -(normalize(result.xyz)*2.0-vec3(1.))*(max(-0.5,min(0.5,";
    oss << Shader::Print(Setting.NormalOpacity);
    oss << ")) )),1.0); return result.xyz;}";
    Fragment.emplace_back(oss.str());
  }

  if (!Setting.SpecularMap.empty()) {
    oss.clear();
    oss << "float specularMap() { ";
    oss << "vec4 result = vec4(0.);float float_result = 0.; ";
    oss << Setting.SpecularMap << ";";
    oss << "return float_result ;}";
    Fragment.emplace_back(oss.str());
  }

  Fragment.emplace_back(FragmentBeforeMain);

  oss.clear();
  oss << "void main(void) { \n";
  oss << "   int discardState = 0;\n";
  oss << "   vec4 result = vec4(0.);\n";
  oss << "   #[Source] \n";
  oss << "   if(discardState == 0) gl_FragColor = result; \n";
  oss << "}";
  Fragment.emplace_back(oss.str());
}

void ShaderBuilder::PrepareBeforePostProcessBuild()
{
  Attributes.emplace_back(ShaderMaterialHelperStatics::AttrPosition);

  /** Start Build Vertex Frame **/
  /*#extension GL_OES_standard_derivatives : enable*/
  Fragment.emplace_back("precision " + Setting.PrecisionMode + " float;\n");

  if (Setting.Uv) {
    Fragment.emplace_back("varying vec2 vUV;");
  }

  if (Setting.Flags) {
    Fragment.emplace_back("uniform float "
                          + ShaderMaterialHelperStatics::uniformFlags + ";");
  }

  if (!Setting.Texture2Ds.empty()) {
    unsigned int cnt = 0;
    for (auto& texture2D : Setting.Texture2Ds) {
      std::string s = std::to_string(cnt);
      if (texture2D.inFragment) {
        Fragment.emplace_back("uniform sampler2D "
                              + ShaderMaterialHelperStatics::Texture2D + s
                              + ";");
      }
      ++cnt;
    }
  }

  if (!PPSSamplers.empty()) {
    for (auto& PPSSampler : PPSSamplers) {
      if (!PPSSampler.empty()) {
        Fragment.emplace_back("uniform sampler2D  " + PPSSampler + ";");
      }
    }
  }

  if (Setting.CameraShot) {
    Fragment.emplace_back("uniform sampler2D textureSampler;");
  }

  if (Setting.Mouse) {
    Fragment.emplace_back("uniform vec2 " + ShaderMaterialHelperStatics::Mouse
                          + ";");
  }
  if (Setting.Screen) {
    Fragment.emplace_back("uniform vec2 " + ShaderMaterialHelperStatics::Screen
                          + ";");
  }
  if (Setting.Camera) {
    Fragment.emplace_back("uniform vec3 " + ShaderMaterialHelperStatics::Camera
                          + ";");
  }
  if (Setting.Look) {
    Fragment.emplace_back("uniform vec3 " + ShaderMaterialHelperStatics::Look
                          + ";");
  }
  if (Setting.Time) {
    Fragment.emplace_back("uniform float " + ShaderMaterialHelperStatics::Time
                          + ";");
  }
  if (Setting.GlobalTime) {
    Fragment.emplace_back("uniform vec4 "
                          + ShaderMaterialHelperStatics::GlobalTime + ";");
  }

  if (Setting.Helpers) {
    constexpr const char* helpers
      = "vec3 random3(vec3 c) {\n"
        "  float j = 4096.0 * sin(dot(c, vec3(17.0, 59.4, 15.0)));\n"
        "  vec3 r;\n"
        "  r.z = fract(512.0 * j);\n"
        "  j *= .125;\n"
        "  r.x = fract(512.0 * j);\n"
        "  j *= .125;\n"
        "  r.y = fract(512.0 * j);\n"
        "  return r - 0.5;\n"
        "}\n"
        "float rand(vec2 co) {\n"
        "  return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);\n"
        "}\n"
        "const float F3 = 0.3333333;\n"
        "const float G3 = 0.1666667;\n"
        "float simplex3d(vec3 p) {\n"
        "  vec3 s = floor(p + dot(p, vec3(F3)));\n"
        "  vec3 x = p - s + dot(s, vec3(G3));\n"
        "  vec3 e = step(vec3(0.0), x - x.yzx);\n"
        "  vec3 i1 = e * (1.0 - e.zxy);\n"
        "  vec3 i2 = 1.0 - e.zxy * (1.0 - e);\n"
        "  vec3 x1 = x - i1 + G3;\n"
        "  vec3 x2 = x - i2 + 2.0 * G3;\n"
        "  vec3 x3 = x - 1.0 + 3.0 * G3;\n"
        "  vec4 w, d;\n"
        "  w.x = dot(x, x);\n"
        "  w.y = dot(x1, x1);\n"
        "  w.z = dot(x2, x2);\n"
        "  w.w = dot(x3, x3);\n"
        "  w = max(0.6 - w, 0.0);\n"
        "  d.x = dot(random3(s), x);\n"
        "  d.y = dot(random3(s + i1), x1);\n"
        "  d.z = dot(random3(s + i2), x2);\n"
        "  d.w = dot(random3(s + 1.0), x3);\n"
        "  w *= w;\n"
        "  w *= w;\n"
        "  d *= w;\n"
        "  return dot(d, vec4(52.0));\n"
        "}\n"
        "float noise(vec3 m) {\n"
        "  return 0.5333333 * simplex3d(m) +\n"
        "         0.2666667 * simplex3d(2.0 * m) +\n"
        "         0.1333333 * simplex3d(4.0 * m) +\n"
        "         0.0666667 * simplex3d(8.0 * m);\n"
        "}\n"
        "vec2 rotate_xy(vec2 pr1, vec2 pr2, float alpha) {\n"
        "  vec2 pp2 = vec2(pr2.x - pr1.x, pr2.y - pr1.y);\n"
        "  return vec2(pr1.x + pp2.x * cos(alpha * 3.14159265 / 180.) -\n"
        "                  pp2.y * sin(alpha * 3.14159265 / 180.),\n"
        "              pr1.y + pp2.x * sin(alpha * 3.14159265 / 180.) +\n"
        "                  pp2.y * cos(alpha * 3.14159265 / 180.));\n"
        "}\n"
        "vec3 r_y(vec3 n, float a, vec3 c) {\n"
        "  vec3 c1 = vec3(c.x, c.y, c.z);\n"
        "  c1.x = c1.x;\n"
        "  c1.y = c1.z;\n"
        "  vec2 p = rotate_xy(vec2(c1.x, c1.y), vec2(n.x, n.z), a);\n"
        "  n.x = p.x;\n"
        "  n.z = p.y;\n"
        "  return n;\n"
        "}\n"
        "vec3 r_x(vec3 n, float a, vec3 c) {\n"
        "  vec3 c1 = vec3(c.x, c.y, c.z);\n"
        "  c1.x = c1.y;\n"
        "  c1.y = c1.z;\n"
        "  vec2 p = rotate_xy(vec2(c1.x, c1.y), vec2(n.y, n.z), a);\n"
        "  n.y = p.x;\n"
        "  n.z = p.y;\n"
        "  return n;\n"
        "}\n"
        "vec3 r_z(vec3 n, float a, vec3 c) {\n"
        "  vec3 c1 = vec3(c.x, c.y, c.z);\n"
        "  vec2 p = rotate_xy(vec2(c1.x, c1.y), vec2(n.x, n.y), a);\n"
        "  n.x = p.x;\n"
        "  n.y = p.y;\n"
        "  return n;\n"
        "}\n"
        "float getIdColor(vec4 a) {\n"
        "  float b = 255.;\n"
        "  float c = 255. / b;\n"
        "  float x = floor(a.x * 256. / c);\n"
        "  float y = floor(a.y * 256. / c);\n"
        "  float z = floor(a.z * 256. / c);\n"
        "  return z * b * b + y * b + x;\n"
        "}\n";
    Fragment.emplace_back(helpers);
  }

  std::ostringstream oss;
  if (!Setting.NormalMap.empty()) {
    oss.clear();
    oss << "vec3 normalMap() { ";
    oss << "vec4 result = vec4(0.);";
    oss << "return result.xyz;}";
    Fragment.emplace_back(oss.str());
  }

  /** start Build Fragment Frame **/
  Fragment.emplace_back(FragmentBeforeMain);

  oss.clear();
  oss << "void main(void) { \n";
  oss << "   int discardState = 0;\n";
  oss << "   vec2 vuv = vUV;\n";
  oss << "   vec3 center = vec3(0.);\n";
  oss << "   vec4 result = vec4(0.);\n";
  oss << "   #[Source] \n";
  oss << "   if(discardState == 0) gl_FragColor = result; \n";
  oss << "}";
  Fragment.emplace_back(oss.str());
}

ShaderMaterial* ShaderBuilder::PrepareMaterial(ShaderMaterial* /*material*/,
                                               Scene* /*scene*/)
{
  return nullptr;
}

std::string ShaderBuilder::Build()
{
  Shader::Me->Parent->Setting = Shader::Me->Setting;
  Shader::Me                  = Shader::Me->Parent;

  return Body;
}

std::string ShaderBuilder::BuildVertex()
{
  Shader::Me->Parent->Setting = Shader::Me->Setting;
  Shader::Me                  = Shader::Me->Parent;

  return VertexBody;
}

ShaderBuilder& ShaderBuilder::SetUniform(const std::string& name,
                                         const std::string& type)
{
  if (Shader::Me->VertexUniforms.empty()) {
    Shader::Me->VertexUniforms = "";
  }
  if (Shader::Me->FragmentUniforms.empty()) {
    Shader::Me->FragmentUniforms = "";
  }

  VertexUniforms += "uniform " + type + " " + name + ";\n";
  FragmentUniforms += "uniform " + type + " " + name + ";\n";

  return *this;
}

ShaderMaterial* ShaderBuilder::BuildMaterial(Scene* /*scene*/)
{
  return nullptr;
}

PostProcess* ShaderBuilder::BuildPostProcess(Camera* /*camera*/,
                                             Scene* /*scene*/, float /*scale*/,
                                             const std::string& /*option*/)
{
  return nullptr;
}

ShaderBuilder& ShaderBuilder::Event(int index, const std::string& mat)
{
  Shader::Me->Setting.Flags = true;

  ++Shader::Indexer;

  std::ostringstream oss;
  oss << Shader::Def(Body, "");
  oss << "  if ( floor(mod( ";
  oss << ShaderMaterialHelperStatics::uniformFlags;
  oss << "/pow(2.,";
  oss << Shader::Print(index);
  oss << "),2.)) == 1.) { ";
  oss << mat;
  oss << " } ";

  Body = oss.str();

  return *this;
}

ShaderBuilder& ShaderBuilder::EventVertex(int index, const std::string& mat)
{
  Shader::Me->Setting.Flags  = true;
  Shader::Me->Setting.Vertex = true;
  Shader::Indexer++;

  std::ostringstream oss;
  oss << Shader::Def(VertexBody, "");
  oss << " if( floor(mod( ";
  oss << ShaderMaterialHelperStatics::uniformFlags;
  oss << "/pow(2.,";
  oss << Shader::Print(index);
  oss << "),2.)) == 1. ){ ";
  oss << mat;
  oss << "}";

  VertexBody = oss.str();

  return *this;
}

ShaderBuilder& ShaderBuilder::Transparency()
{
  Shader::Me->Setting.Transparency = true;

  return *this;
}

ShaderBuilder& ShaderBuilder::PostEffect1(size_t id, const std::string& effect)
{
  if (id >= Shader::Me->PostEffect1Effects.size()) {
    Shader::Me->PostEffect1Effects.resize(id + 1);
  }
  Shader::Me->PostEffect1Effects[id] = effect;

  return *this;
}

ShaderBuilder& ShaderBuilder::PostEffect2(size_t id, const std::string& effect)
{
  if (id >= Shader::Me->PostEffect2Effects.size()) {
    Shader::Me->PostEffect2Effects.resize(id + 1);
  }
  Shader::Me->PostEffect2Effects[id] = effect;

  return *this;
}

ShaderBuilder&
ShaderBuilder::ImportSamplers(const std::vector<std::string>& txts)
{
  std_util::concat(Shader::Me->PPSSamplers, txts);

  return *this;
}

ShaderBuilder& ShaderBuilder::Wired()
{
  Shader::Me->Setting.Wire = true;

  return *this;
}

ShaderBuilder& ShaderBuilder::VertexShader(const std::string& mat)
{
  VertexBody = Shader::Def(VertexBody, "");
  VertexBody += mat;

  return *this;
}

ShaderBuilder& ShaderBuilder::Solid(const IColor& iColor)
{
  IColor color;
  color.a = Shader::Def(iColor.a, 1.f);
  color.r = Shader::Def(iColor.r, 0.f);
  color.g = Shader::Def(iColor.g, 0.f);
  color.b = Shader::Def(iColor.b, 0.f);

  std::ostringstream oss;
  oss << Shader::Def(Body, "");
  oss << " result = vec4(";
  oss << Shader::Print(color.r) << ",";
  oss << Shader::Print(color.g) << ",";
  oss << Shader::Print(color.b) << ",";
  oss << Shader::Print(color.a) << ");";

  Body = oss.str();

  return *this;
}

int ShaderBuilder::GetMapIndex(const std::string key)
{
  auto it = std::find_if(
    Shader::Me->Setting.Texture2Ds.begin(),
    Shader::Me->Setting.Texture2Ds.end(),
    [&key](const ITexture& texture) { return texture.key == key; });

  return (it == Shader::Me->Setting.Texture2Ds.end()) ?
           -1 :
           static_cast<int>(it - Shader::Me->Setting.Texture2Ds.begin());
}

int ShaderBuilder::GetCubeMapIndex(const std::string key)
{
  auto it = std::find_if(
    Shader::Me->Setting.TextureCubes.begin(),
    Shader::Me->Setting.TextureCubes.end(),
    [&key](const ITexture& texture) { return texture.key == key; });

  return (it == Shader::Me->Setting.TextureCubes.end()) ?
           -1 :
           static_cast<int>(it - Shader::Me->Setting.TextureCubes.begin());
}

std::string Nut(std::string& /*value*/, const INut& /*option*/)
{
  return "";
}

ShaderBuilder& ShaderBuilder::Map(const IMap& /*option*/)
{
  return *this;
}

ShaderBuilder& ShaderBuilder::Multi(const std::vector<Material*>& /*mats*/,
                                    bool /*combine*/)
{
  return *this;
}

ShaderBuilder& ShaderBuilder::Back(const std::string& mat)
{
  Shader::Me->Setting.Back = true;

  std::ostringstream oss;
  oss << Shader::Def(Body, "");
  oss << "if(";
  oss << ShaderMaterialHelperStatics::face_back;
  oss << "){";
  oss << mat;
  oss << ";}";

  Body = oss.str();

  return *this;
}

ShaderBuilder& ShaderBuilder::InLine(const std::string& mat)
{
  std::ostringstream oss;
  oss << Shader::Def(Body, "") << mat;

  Body = oss.str();

  return *this;
}

ShaderBuilder& ShaderBuilder::Front(const std::string& mat)
{
  std::ostringstream oss;
  oss << Shader::Def(Body, "");
  oss << "if(";
  oss << ShaderMaterialHelperStatics::face_front;
  oss << "){";
  oss << mat;
  oss << ";}";

  Body = oss.str();

  return *this;
}

ShaderBuilder& ShaderBuilder::Range(const std::string& /*mat1*/,
                                    const std::string& /*mat2*/,
                                    const IRange& /*option*/)
{
  return *this;
}

ShaderBuilder& ShaderBuilder::Reference(size_t /*index*/,
                                        const std::string& /*mat*/)
{
  return *this;
}

ShaderBuilder& ShaderBuilder::ReplaceColor(int /*index*/, int /*color*/,
                                           const std::string& /*mat*/,
                                           const IReplaceColor& /*option*/)
{
  return *this;
}

ShaderBuilder& ShaderBuilder::Blue(int index, const std::string& mat,
                                   const IReplaceColor& option)
{
  return ReplaceColor(index, Helper::Blue, mat, option);
}

ShaderBuilder& ShaderBuilder::Cyan(int index, const std::string& mat,
                                   const IReplaceColor& option)
{
  return ReplaceColor(index, Helper::Cyan, mat, option);
}

ShaderBuilder& ShaderBuilder::Red(int index, const std::string& mat,
                                  const IReplaceColor& option)
{
  return ReplaceColor(index, Helper::Red, mat, option);
}

ShaderBuilder& ShaderBuilder::Yellow(int index, const std::string& mat,
                                     const IReplaceColor& option)
{
  return ReplaceColor(index, Helper::Yellow, mat, option);
}

ShaderBuilder& ShaderBuilder::Green(int index, const std::string& mat,
                                    const IReplaceColor& option)
{
  return ReplaceColor(index, Helper::Green, mat, option);
}

ShaderBuilder& ShaderBuilder::Pink(int index, const std::string& mat,
                                   const IReplaceColor& option)
{
  return ReplaceColor(index, Helper::Pink, mat, option);
}

ShaderBuilder& ShaderBuilder::White(int index, const std::string& mat,
                                    const IReplaceColor& option)
{
  return ReplaceColor(index, Helper::White, mat, option);
}

ShaderBuilder& ShaderBuilder::Black(int index, const std::string& mat,
                                    const IReplaceColor& option)
{
  return ReplaceColor(index, Helper::Black, mat, option);
}

ShaderBuilder& ShaderBuilder::ReflectCube(const IReflectMap& /*option*/)
{
  return *this;
}

ShaderBuilder& ShaderBuilder::NormalMap(const std::string& val,
                                        const std::string& mat)
{
  Shader::Me->Setting.NormalOpacity = val;
  Shader::Me->Setting.NormalMap     = mat;

  return *this;
}

ShaderBuilder& ShaderBuilder::SpecularMap(const std::string& mat)
{
  Shader::Me->Setting.SpecularMap = mat;

  return *this;
}

std::unique_ptr<ShaderBuilder> ShaderBuilder::Instance()
{
  return nullptr;
}

ShaderBuilder& ShaderBuilder::Reflect(const IReflectMap& /*option*/,
                                      float /*opacity*/)
{
  return *this;
}

ShaderBuilder& ShaderBuilder::Light(const ILight& /*option*/)
{
  return *this;
}

ShaderBuilder& ShaderBuilder::Effect(const IEffect& /*option*/)
{
  return *this;
}

ShaderBuilder& ShaderBuilder::IdColor(float /*id*/, float /*w*/)
{
  return *this;
}

ShaderBuilder& ShaderBuilder::Discard()
{
  return *this;
}

} // end of namespace Extensions
} // end of namespace BABYLON