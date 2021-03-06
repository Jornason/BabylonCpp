#ifndef BABYLON_POSTPROCESS_POST_PROCESS_H
#define BABYLON_POSTPROCESS_POST_PROCESS_H

#include <babylon/babylon_global.h>
#include <babylon/core/structs.h>
#include <babylon/engine/engine.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/math/color4.h>
#include <babylon/tools/observable.h>
#include <babylon/tools/observer.h>

namespace BABYLON {

/**
 * @brief
 */
class BABYLON_SHARED_EXPORT PostProcess {

public:
  PostProcess(const std::string& name, const std::string& fragmentUrl,
              const std::vector<std::string>& parameters,
              const std::vector<std::string>& samplers, float renderRatio,
              Camera* camera,
              unsigned int samplingMode = Texture::NEAREST_SAMPLINGMODE,
              Engine* engine = nullptr, bool reusable = false,
              const std::string& defines = "",
              unsigned int textureType   = Engine::TEXTURETYPE_UNSIGNED_INT);
  PostProcess(const std::string& name, const std::string& fragmentUrl,
              const std::vector<std::string>& parameters,
              const std::vector<std::string>& samplers,
              const PostProcessOptions& options, Camera* camera,
              unsigned int samplingMode = Texture::NEAREST_SAMPLINGMODE,
              Engine* engine = nullptr, bool reusable = false,
              const std::string& defines = "",
              unsigned int textureType   = Engine::TEXTURETYPE_UNSIGNED_INT);
  virtual ~PostProcess();

  // Events
  void setOnActivate(const std::function<void(Camera* camera)>& callback);
  void setOnSizeChanged(
    const std::function<void(PostProcess* postProcess)>& callback);
  void setOnApply(const std::function<void(Effect* effect)>& callback);
  void setOnBeforeRender(const std::function<void(Effect* effect)>& callback);
  void setOnAfterRender(const std::function<void(Effect* effect)>& callback);

  void updateEffect(const std::string& defines = "");
  bool isReusable() const;
  /**
   * Invalidate frameBuffer to hint the postprocess to create a depth buffer
   */
  void markTextureDirty();
  void activate(Camera* camera, GL::IGLTexture* sourceTexture = nullptr);
  bool isSupported() const;
  Effect* apply();
  virtual void dispose(Camera* camera = nullptr);

public:
  std::string name;
  int width;
  int height;
  unsigned int renderTargetSamplingMode;
  std::unique_ptr<Color4> clearColor;
  // Enable Pixel Perfect mode where texture is not scaled to be power of 2.
  // Can only be used on a single postprocess or on the last one of a chain.
  bool enablePixelPerfectMode;
  unsigned int samples;
  std::vector<GL::IGLTexture*> _textures;
  unsigned int _currentRenderTextureInd;
  // Events
  /**
   * An event triggered when the postprocess is activated.
   */
  Observable<Camera> onActivateObservable;
  /**
   * An event triggered when the postprocess changes its size.
   */
  Observable<PostProcess> onSizeChangedObservable;
  /**
   * An event triggered when the postprocess applies its effect.
   */
  Observable<Effect> onApplyObservable;
  /**
   * An event triggered before rendering the postprocess
   */
  Observable<Effect> onBeforeRenderObservable;
  /**
   * An event triggered after rendering the postprocess
   */
  Observable<Effect> onAfterRenderObservable;

private:
  Camera* _camera;
  Scene* _scene;
  Engine* _engine;
  float _renderRatio;
  PostProcessOptions _options;
  bool _reusable;
  unsigned int _textureType;
  Effect* _effect;
  std::vector<std::string> _samplers;
  std::string _fragmentUrl;
  std::vector<std::string> _parameters;
  Vector2 _scaleRatio;
  // Events
  Observer<Camera>::Ptr _onActivateObserver;
  Observer<PostProcess>::Ptr _onSizeChangedObserver;
  Observer<Effect>::Ptr _onApplyObserver;
  Observer<Effect>::Ptr _onBeforeRenderObserver;
  Observer<Effect>::Ptr _onAfterRenderObserver;

}; // end of class PostProcess

} // end of namespace BABYLON

#endif // end of BABYLON_POSTPROCESS_POST_PROCESS_H
