#ifndef BABYLON_TOOLS_TOOLS_H
#define BABYLON_TOOLS_TOOLS_H

#include <babylon/babylon_global.h>
#include <babylon/core/structs.h>

namespace BABYLON {

/**
 * @brief Represents the tools class.
 */
struct BABYLON_SHARED_EXPORT Tools {

  /** Statics **/
  static bool IsExponentOfTwo(int value);
  static int GetExponentOfTwo(int value, int max);
  static float ToDegrees(float angle);
  static float ToRadians(float angle);
  static MinMax ExtractMinAndMaxIndexed(const Float32Array& positions,
                                        const Uint32Array& indices,
                                        size_t indexStart, size_t indexCount);
  static MinMax ExtractMinAndMaxIndexed(const Float32Array& positions,
                                        const Uint32Array& indices,
                                        size_t indexStart, size_t indexCount,
                                        const Vector2& bias);
  static MinMax ExtractMinAndMax(const Float32Array& positions, size_t start,
                                 size_t count, unsigned int stride = 3);
  static MinMax ExtractMinAndMax(const Float32Array& positions, size_t start,
                                 size_t count, const Vector2& bias,
                                 unsigned int stride = 3);
  static void
  LoadImage(const std::string& url,
            const std::function<void(const Image& img)>& onLoad,
            const std::function<void(const std::string& msg)>& onError,
            bool flipVertically = true);
  static void
  LoadFile(const std::string& url,
           const std::function<void(const std::string& text)>& callback,
           const std::function<void()>& progressCallBack = nullptr,
           bool useArrayBuffer                           = false);
  static void CheckExtends(Vector3& v, Vector3& min, Vector3& max);
  static std::string RandomId();
  static void SetImmediate(const std::function<void()>& immediate);
  static void DumpFramebuffer(int width, int height, Engine* engine);
  static void StartPerformanceCounter(const std::string&)
  {
  }
  static void StartPerformanceCounter(const std::string&, bool)
  {
  }
  static void EndPerformanceCounter(const std::string&)
  {
  }
  static void EndPerformanceCounter(const std::string&, bool)
  {
  }
  static void ExitFullscreen()
  {
  }
  static void RequestFullscreen(ICanvas*)
  {
  }

}; // end of struct Tools

} // end of namespace BABYLON

#endif // end of BABYLON_TOOLS_TOOLS_H
