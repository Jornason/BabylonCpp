#ifndef BABYLON_POSTPROCESS_PASS_POST_PROCESS_H
#define BABYLON_POSTPROCESS_PASS_POST_PROCESS_H

#include <babylon/babylon_global.h>
#include <babylon/engine/engine.h>
#include <babylon/postprocess/post_process.h>

namespace BABYLON {

/**
 * @brief
 */
class BABYLON_SHARED_EXPORT PassPostProcess : public PostProcess {

public:
  PassPostProcess(const std::string& name, float ratio, Camera* camera,
                  unsigned int samplingMode, Engine* engine,
                  bool reusable            = false,
                  unsigned int textureType = Engine::TEXTURETYPE_UNSIGNED_INT);
  ~PassPostProcess();

}; // end of class PassPostProcess

} // end of namespace BABYLON

#endif // end of BABYLON_POSTPROCESS_PASS_POST_PROCESS_H
