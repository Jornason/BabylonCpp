﻿#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_SHADOWS_FRAGMENT_FUNCTIONS_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_SHADOWS_FRAGMENT_FUNCTIONS_FX_H

namespace BABYLON {

extern const char* shadowsFragmentFunctions;

const char* shadowsFragmentFunctions
  = "#ifdef SHADOWS\n"
    "  #ifndef SHADOWFULLFLOAT\n"
    "  float unpack(vec4 color)\n"
    "  {\n"
    "  const vec4 bit_shift = vec4(1.0 / (255.0 * 255.0 * 255.0), 1.0 / (255.0 * 255.0), 1.0 / 255.0, 1.0);\n"
    "  return dot(color, bit_shift);\n"
    "  }\n"
    "  #endif\n"
    "\n"
    "  uniform vec2 depthValues;\n"
    "\n"
    "  float computeShadowCube(vec3 lightPosition, samplerCube shadowSampler, float darkness, float bias)\n"
    "  {\n"
    "  vec3 directionToLight = vPositionW - lightPosition;\n"
    "  float depth = length(directionToLight);\n"
    "  depth = (depth - depthValues.x) / (depthValues.y - depthValues.x);\n"
    "  depth = clamp(depth, 0., 1.0);\n"
    "\n"
    "  directionToLight = normalize(directionToLight);\n"
    "  directionToLight.y = -directionToLight.y;\n"
    "  \n"
    "  #ifndef SHADOWFULLFLOAT\n"
    "  float shadow = unpack(textureCube(shadowSampler, directionToLight)) + bias;\n"
    "  #else\n"
    "  float shadow = textureCube(shadowSampler, directionToLight).x + bias;\n"
    "  #endif\n"
    "\n"
    "  if (depth > shadow)\n"
    "  {\n"
    "  return darkness;\n"
    "  }\n"
    "  return 1.0;\n"
    "  }\n"
    "\n"
    "  float computeShadowWithPCFCube(vec3 lightPosition, samplerCube shadowSampler, float mapSize, float bias, float darkness)\n"
    "  {\n"
    "  vec3 directionToLight = vPositionW - lightPosition;\n"
    "  float depth = length(directionToLight);\n"
    "\n"
    "  depth = (depth - depthValues.x) / (depthValues.y - depthValues.x);\n"
    "  depth = clamp(depth, 0., 1.0);\n"
    "\n"
    "  directionToLight = normalize(directionToLight);\n"
    "  directionToLight.y = -directionToLight.y;\n"
    "\n"
    "  float visibility = 1.;\n"
    "\n"
    "  vec3 poissonDisk[4];\n"
    "  poissonDisk[0] = vec3(-1.0, 1.0, -1.0);\n"
    "  poissonDisk[1] = vec3(1.0, -1.0, -1.0);\n"
    "  poissonDisk[2] = vec3(-1.0, -1.0, -1.0);\n"
    "  poissonDisk[3] = vec3(1.0, -1.0, 1.0);\n"
    "\n"
    "  // Poisson Sampling\n"
    "  float biasedDepth = depth - bias;\n"
    "\n"
    "  #ifndef SHADOWFULLFLOAT\n"
    "  if (unpack(textureCube(shadowSampler, directionToLight + poissonDisk[0] * mapSize)) < biasedDepth) visibility -= 0.25;\n"
    "  if (unpack(textureCube(shadowSampler, directionToLight + poissonDisk[1] * mapSize)) < biasedDepth) visibility -= 0.25;\n"
    "  if (unpack(textureCube(shadowSampler, directionToLight + poissonDisk[2] * mapSize)) < biasedDepth) visibility -= 0.25;\n"
    "  if (unpack(textureCube(shadowSampler, directionToLight + poissonDisk[3] * mapSize)) < biasedDepth) visibility -= 0.25;\n"
    "  #else\n"
    "  if (textureCube(shadowSampler, directionToLight + poissonDisk[0] * mapSize).x < biasedDepth) visibility -= 0.25;\n"
    "  if (textureCube(shadowSampler, directionToLight + poissonDisk[1] * mapSize).x < biasedDepth) visibility -= 0.25;\n"
    "  if (textureCube(shadowSampler, directionToLight + poissonDisk[2] * mapSize).x < biasedDepth) visibility -= 0.25;\n"
    "  if (textureCube(shadowSampler, directionToLight + poissonDisk[3] * mapSize).x < biasedDepth) visibility -= 0.25;\n"
    "  #endif\n"
    "\n"
    "  return  min(1.0, visibility + darkness);\n"
    "  }\n"
    "\n"
    "  float computeShadow(vec4 vPositionFromLight, sampler2D shadowSampler, float darkness, float bias)\n"
    "  {\n"
    "  vec3 depth = vPositionFromLight.xyz / vPositionFromLight.w;\n"
    "  depth = 0.5 * depth + vec3(0.5);\n"
    "  vec2 uv = depth.xy;\n"
    "\n"
    "  if (uv.x < 0. || uv.x > 1.0 || uv.y < 0. || uv.y > 1.0)\n"
    "  {\n"
    "  return 1.0;\n"
    "  }\n"
    "\n"
    "  #ifndef SHADOWFULLFLOAT\n"
    "  float shadow = unpack(texture2D(shadowSampler, uv)) + bias;\n"
    "  #else\n"
    "  float shadow = texture2D(shadowSampler, uv).x + bias;\n"
    "  #endif\n"
    "\n"
    "  if (depth.z > shadow)\n"
    "  {\n"
    "  return darkness;\n"
    "  }\n"
    "  return 1.;\n"
    "  }\n"
    "\n"
    "  float computeShadowWithPCF(vec4 vPositionFromLight, sampler2D shadowSampler, float mapSize, float bias, float darkness)\n"
    "  {\n"
    "  vec3 depth = vPositionFromLight.xyz / vPositionFromLight.w;\n"
    "  depth = 0.5 * depth + vec3(0.5);\n"
    "  vec2 uv = depth.xy;\n"
    "\n"
    "  if (uv.x < 0. || uv.x > 1.0 || uv.y < 0. || uv.y > 1.0)\n"
    "  {\n"
    "  return 1.0;\n"
    "  }\n"
    "\n"
    "  float visibility = 1.;\n"
    "\n"
    "  vec2 poissonDisk[4];\n"
    "  poissonDisk[0] = vec2(-0.94201624, -0.39906216);\n"
    "  poissonDisk[1] = vec2(0.94558609, -0.76890725);\n"
    "  poissonDisk[2] = vec2(-0.094184101, -0.92938870);\n"
    "  poissonDisk[3] = vec2(0.34495938, 0.29387760);\n"
    "\n"
    "  // Poisson Sampling\n"
    "  float biasedDepth = depth.z - bias;\n"
    "\n"
    "  #ifndef SHADOWFULLFLOAT\n"
    "  if (unpack(texture2D(shadowSampler, uv + poissonDisk[0] * mapSize)) < biasedDepth) visibility -= 0.25;\n"
    "  if (unpack(texture2D(shadowSampler, uv + poissonDisk[1] * mapSize)) < biasedDepth) visibility -= 0.25;\n"
    "  if (unpack(texture2D(shadowSampler, uv + poissonDisk[2] * mapSize)) < biasedDepth) visibility -= 0.25;\n"
    "  if (unpack(texture2D(shadowSampler, uv + poissonDisk[3] * mapSize)) < biasedDepth) visibility -= 0.25;\n"
    "  #else\n"
    "  if (texture2D(shadowSampler, uv + poissonDisk[0] * mapSize).x < biasedDepth) visibility -= 0.25;\n"
    "  if (texture2D(shadowSampler, uv + poissonDisk[1] * mapSize).x < biasedDepth) visibility -= 0.25;\n"
    "  if (texture2D(shadowSampler, uv + poissonDisk[2] * mapSize).x < biasedDepth) visibility -= 0.25;\n"
    "  if (texture2D(shadowSampler, uv + poissonDisk[3] * mapSize).x < biasedDepth) visibility -= 0.25;\n"
    "  #endif\n"
    "\n"
    "  return  min(1.0, visibility + darkness);\n"
    "  }\n"
    "\n"
    "  #ifndef SHADOWFULLFLOAT\n"
    "  // Thanks to http://devmaster.net/\n"
    "  float unpackHalf(vec2 color)\n"
    "  {\n"
    "  return color.x + (color.y / 255.0);\n"
    "  }\n"
    "  #endif\n"
    "\n"
    "  float linstep(float low, float high, float v) {\n"
    "  return clamp((v - low) / (high - low), 0.0, 1.0);\n"
    "  }\n"
    "\n"
    "  float ChebychevInequality(vec2 moments, float compare, float bias)\n"
    "  {\n"
    "  float p = smoothstep(compare - bias, compare, moments.x);\n"
    "  float variance = max(moments.y - moments.x * moments.x, 0.02);\n"
    "  float d = compare - moments.x;\n"
    "  float p_max = linstep(0.2, 1.0, variance / (variance + d * d));\n"
    "\n"
    "  return clamp(max(p, p_max), 0.0, 1.0);\n"
    "  }\n"
    "\n"
    "  float computeShadowWithVSM(vec4 vPositionFromLight, sampler2D shadowSampler, float bias, float darkness)\n"
    "  {\n"
    "  vec3 depth = vPositionFromLight.xyz / vPositionFromLight.w;\n"
    "  depth = 0.5 * depth + vec3(0.5);\n"
    "  vec2 uv = depth.xy;\n"
    "\n"
    "  if (uv.x < 0. || uv.x > 1.0 || uv.y < 0. || uv.y > 1.0 || depth.z >= 1.0)\n"
    "  {\n"
    "  return 1.0;\n"
    "  }\n"
    "\n"
    "  vec4 texel = texture2D(shadowSampler, uv);\n"
    "\n"
    "  #ifndef SHADOWFULLFLOAT\n"
    "  vec2 moments = vec2(unpackHalf(texel.xy), unpackHalf(texel.zw));\n"
    "  #else\n"
    "  vec2 moments = texel.xy;\n"
    "  #endif\n"
    "\n"
    "  return min(1.0, 1.0 - ChebychevInequality(moments, depth.z, bias) + darkness);\n"
    "  }\n"
    "#endif\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_SHADOWS_FRAGMENT_FUNCTIONS_FX_H
