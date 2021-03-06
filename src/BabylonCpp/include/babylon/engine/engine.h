#ifndef BABYLON_ENGINE_ENGINE_H
#define BABYLON_ENGINE_ENGINE_H

#include <babylon/babylon_global.h>
#include <babylon/core/structs.h>
#include <babylon/engine/engine_capabilities.h>
#include <babylon/engine/engine_options.h>
#include <babylon/interfaces/idisposable.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/math/viewport.h>
#include <babylon/mesh/buffer_pointer.h>
#include <babylon/tools/perf_counter.h>

namespace BABYLON {

struct RenderTargetOptions;

/**
 * @brief The engine class is responsible for interfacing with all lower-level
 * APIs such as OpenGL and Audio.
 */
class BABYLON_SHARED_EXPORT Engine : public IDisposable {

public:
  using GLBufferPtr            = std::unique_ptr<GL::IGLBuffer>;
  using GLFrameBufferPtr       = std::unique_ptr<GL::IGLFramebuffer>;
  using GLFrameProgramPtr      = std::unique_ptr<GL::IGLProgram>;
  using GLRenderBufferPtr      = std::unique_ptr<GL::IGLRenderbuffer>;
  using GLShaderPtr            = std::unique_ptr<GL::IGLShader>;
  using GLTexturePtr           = std::unique_ptr<GL::IGLTexture>;
  using GLUniformLocationPtr   = std::unique_ptr<GL::IGLUniformLocation>;
  using GLVertexArrayObjectPtr = std::unique_ptr<GL::IGLVertexArrayObject>;

public:
  // Const statics
  static constexpr unsigned int ALPHA_DISABLE       = 0;
  static constexpr unsigned int ALPHA_ADD           = 1;
  static constexpr unsigned int ALPHA_COMBINE       = 2;
  static constexpr unsigned int ALPHA_SUBTRACT      = 3;
  static constexpr unsigned int ALPHA_MULTIPLY      = 4;
  static constexpr unsigned int ALPHA_MAXIMIZED     = 5;
  static constexpr unsigned int ALPHA_ONEONE        = 6;
  static constexpr unsigned int ALPHA_PREMULTIPLIED = 7;

  static constexpr unsigned int DELAYLOADSTATE_NONE      = 0;
  static constexpr unsigned int DELAYLOADSTATE_LOADED    = 1;
  static constexpr unsigned int DELAYLOADSTATE_LOADING   = 2;
  static constexpr unsigned int DELAYLOADSTATE_NOTLOADED = 4;

  static constexpr unsigned int TEXTUREFORMAT_ALPHA           = 0;
  static constexpr unsigned int TEXTUREFORMAT_LUMINANCE       = 1;
  static constexpr unsigned int TEXTUREFORMAT_LUMINANCE_ALPHA = 2;
  static constexpr unsigned int TEXTUREFORMAT_RGB             = 4;
  static constexpr unsigned int TEXTUREFORMAT_RGBA            = 5;

  static constexpr unsigned int TEXTURETYPE_UNSIGNED_INT = 0;
  static constexpr unsigned int TEXTURETYPE_FLOAT        = 1;
  static constexpr unsigned int TEXTURETYPE_HALF_FLOAT   = 2;

  // Depht or Stencil test Constants.
  // Passed to depthFunction or stencilFunction to specify depth or stencil
  // tests will never pass. i.e. Nothing will be drawn.
  static constexpr unsigned int NEVER = 0x0200;
  // Passed to depthFunction or stencilFunction to specify depth or stencil
  // tests will always pass. i.e. Pixels will be drawn in the order they are
  // drawn.
  static constexpr unsigned int ALWAYS = 0x0207;
  // Passed to depthFunction or stencilFunction to specify depth or stencil
  // tests will pass if the new depth value is less than the stored value.
  static constexpr unsigned int LESS = 0x0201;
  // Passed to depthFunction or stencilFunction to specify depth or stencil
  // tests will pass if the new depth value is equals to the stored value.
  static constexpr unsigned int EQUAL = 0x0202;
  // Passed to depthFunction or stencilFunction to specify depth or stencil
  // tests will pass if the new depth value is less than or equal to the stored
  // value.
  static constexpr unsigned int LEQUAL = 0x0203;
  // Passed to depthFunction or stencilFunction to specify depth or stencil
  // tests will pass if the new depth value is greater than the stored value.
  static constexpr unsigned int GREATER = 0x0204;
  // Passed to depthFunction or stencilFunction to specify depth or stencil
  // tests will pass if the new depth value is greater than or equal to the
  // stored value.
  static constexpr unsigned int GEQUAL = 0x0206;
  // Passed to depthFunction or stencilFunction to specify depth or stencil
  // tests will pass if the new depth value is not equal to the stored value.
  static constexpr unsigned int NOTEQUAL = 0x0205;

  // Half floating-point type (16-bit).
  static constexpr unsigned int HALF_FLOAT_OES = 0x8D61;
  // RGBA 16-bit floating-point color-renderable internal sized format.
  static constexpr unsigned int RGBA16F = 0x881A;
  // RGBA 32-bit floating-point color-renderable internal sized format.
  static constexpr unsigned int RGBA32F = 0x8814;

  // Stencil Actions Constants.
  static constexpr unsigned int KEEP      = 0x1E00;
  static constexpr unsigned int REPLACE   = 0x1E01;
  static constexpr unsigned int INCR      = 0x1E02;
  static constexpr unsigned int DECR      = 0x1E03;
  static constexpr unsigned int INVERT    = 0x150A;
  static constexpr unsigned int INCR_WRAP = 0x8507;
  static constexpr unsigned int DECR_WRAP = 0x8508;

  static std::string Version();

  // Updatable statics so stick with vars here
  static float CollisionsEpsilon;
  static std::string CodeRepository;
  static std::string ShadersRepository;

  // Engine instances
  static std::vector<Engine*> Instances;

public:
  template <typename... Ts>
  static std::unique_ptr<Engine> New(Ts&&... args)
  {
    std::unique_ptr<Engine> engine(new Engine(std::forward<Ts>(args)...));
    return engine;
  }
  virtual ~Engine();

  std::vector<std::string>& texturesSupported();
  std::string textureFormatInUse() const;
  float webGLVersion() const;

  /**
   * @brief Returns true if the stencil buffer has been enabled through the
   * creation option of the context.
   */
  bool isStencilEnable() const;
  void resetTextureCache();
  GL::GLInfo getGlInfo();
  float getAspectRatio(Camera* camera, bool useScreen = false);
  int getRenderWidth(bool useScreen = false);
  int getRenderHeight(bool useScreen = false);
  ICanvas* getRenderingCanvas();
  ClientRect getRenderingCanvasClientRect();
  void setHardwareScalingLevel(int level);
  int getHardwareScalingLevel() const;
  std::vector<GLTexturePtr>& getLoadedTexturesCache();
  EngineCapabilities& getCaps();
  size_t drawCalls() const;
  PerfCounter& drawCallsPerfCounter();

  /** Methods **/
  void backupGLState();
  void restoreGLState();
  int getDepthFunction() const;
  void setDepthFunction(int depthFunc);
  void setDepthFunctionToGreater();
  void setDepthFunctionToGreaterOrEqual();
  void setDepthFunctionToLess();
  void setDepthFunctionToLessOrEqual();
  bool getStencilBuffer() const;
  void setStencilBuffer(bool enable);
  unsigned int getStencilMask() const;
  void setStencilMask(unsigned int mask);
  unsigned int getStencilFunction() const;
  int getStencilFunctionReference() const;
  unsigned int getStencilFunctionMask() const;
  void setStencilFunction(unsigned int stencilFunc);
  void setStencilFunctionReference(int reference);
  void setStencilFunctionMask(unsigned int mask);
  unsigned int getStencilOperationFail() const;
  unsigned int getStencilOperationDepthFail() const;
  unsigned int getStencilOperationPass() const;
  void setStencilOperationFail(unsigned int operation);
  void setStencilOperationDepthFail(unsigned int operation);
  void setStencilOperationPass(unsigned int operation);

  /**
   * @brief Stop executing a render loop function and remove it from the
   * execution array.
   * @param {Function} [renderFunction] the function to be removed. If not
   * provided all functions will be removed.
   */
  void stopRenderLoop(const FastFunc<void()>& renderFunction = nullptr);
  void _renderLoop();

  /**
   * @brief Register and execute a render loop. The engine can have more than
   * one render function.
   * @param {Function} renderFunction - the function to continuesly execute
   * starting the next render loop.
   * Example:
   * engine.runRenderLoop(function () {
   *      scene.render()
   * })
   */
  void runRenderLoop(const FastFunc<void()>& renderFunction);
  void renderFunction(const std::function<void()>& renderFunction);

  /**
   * @brief Toggle full screen mode.
   * @param {boolean} requestPointerLock - should a pointer lock be requested
   * from the user
   */
  void switchFullscreen(bool requestPointerLock);
  void clear(bool depth, bool stencil = false);
  void clear(const Color4& color, bool backBuffer, bool depth,
             bool stencil = false);
  void scissorClear(int x, int y, int width, int height,
                    const Color4& clearColor);

  /**
   * @brief Set the WebGL's viewport.
   * @param {BABYLON.Viewport} viewport - the viewport element to be used.
   * @param {number} [requiredWidth] - the width required for rendering. If not
   * provided the rendering canvas' width is used.
   * @param {number} [requiredHeight] - the height required for rendering. If
   * not provided the rendering canvas' height is used.
   */
  void setViewport(Viewport& viewport, int requiredWidth = 0,
                   int requiredHeight = 0);
  Viewport& setDirectViewport(int x, int y, int width, int height);
  void beginFrame();
  void endFrame();

  /**
   * @brief resize the view according to the canvas' size.
   */
  void resize();

  /**
   * @brief Force a specific size of the canvas.
   * @param {number} width - the new canvas' width
   * @param {number} height - the new canvas' height
   */
  void setSize(int width, int height);

  /** WebVR functions **/
  // void initWebVR();
  // void enableVR(VRDevice vrDevice);
  // void disableVR();

  void bindFramebuffer(GL::IGLTexture* texture, unsigned int faceIndex = 0,
                       int requiredWidth = 0, int requiredHeight = 0);
  void unBindFramebuffer(GL::IGLTexture* texture,
                         bool disableGenerateMipMaps                 = false,
                         const std::function<void()>& onBeforeUnbind = nullptr);
  void generateMipMapsForCubemap(GL::IGLTexture* texture);
  void flushFramebuffer();
  void restoreDefaultFramebuffer();

  /** UBOs **/
  GLBufferPtr createUniformBuffer(const Float32Array& elements);
  GLBufferPtr createDynamicUniformBuffer(const Float32Array& elements);
  void updateUniformBuffer(GL::IGLBuffer* uniformBuffer,
                           const Float32Array& elements, int offset = -1,
                           int count = -1);

  /** VBOs **/
  GLBufferPtr createVertexBuffer(const Float32Array& vertices);
  GLBufferPtr createDynamicVertexBuffer(const Float32Array& vertices);
  void updateDynamicVertexBuffer(const GLBufferPtr& vertexBuffer,
                                 const Float32Array& vertices, int offset = -1,
                                 int count = -1);
  GLBufferPtr createIndexBuffer(const IndicesArray& indices);
  void bindArrayBuffer(GL::IGLBuffer* buffer);
  void bindUniformBuffer(GL::IGLBuffer* buffer);
  void bindUniformBufferBase(GL::IGLBuffer* buffer, unsigned int location);
  void bindUniformBlock(GL::IGLProgram* shaderProgram,
                        const std::string blockName, unsigned int index);
  void updateArrayBuffer(const Float32Array& data);
  GLVertexArrayObjectPtr recordVertexArrayObject(
    const std::unordered_map<std::string, VertexBuffer*>& vertexBuffers,
    GL::IGLBuffer* indexBuffer, Effect* effect);
  void bindVertexArrayObject(GL::IGLVertexArrayObject* vertexArrayObject,
                             GL::IGLBuffer* indexBuffer);
  void bindBuffersDirectly(GL::IGLBuffer* vertexBuffer,
                           GL::IGLBuffer* indexBuffer,
                           const std::vector<float>& vertexDeclaration,
                           int vertexStrideSize = 3, Effect* effect = nullptr);
  void bindBuffers(
    const std::unordered_map<std::string, VertexBuffer*>& vertexBuffers,
    GL::IGLBuffer* indexBuffer, Effect* effect);
  void unbindInstanceAttributes();
  void releaseVertexArrayObject(GL::IGLVertexArrayObject* vao);
  bool _releaseBuffer(GL::IGLBuffer* buffer);
  GLBufferPtr createInstancesBuffer(unsigned int capacity);
  void deleteInstancesBuffer(GL::IGLBuffer* buffer);
  void updateAndBindInstancesBuffer(GL::IGLBuffer* instancesBuffer,
                                    const Float32Array& data,
                                    const Uint32Array& offsetLocations);
  void updateAndBindInstancesBuffer(
    GL::IGLBuffer* instancesBuffer, const Float32Array& data,
    const std::vector<InstancingAttributeInfo>& offsetLocations);
  void applyStates();
  void draw(bool useTriangles, unsigned int indexStart, int indexCount,
            int instancesCount = 0);
  void drawPointClouds(int verticesStart, int verticesCount,
                       int instancesCount);
  void drawUnIndexed(bool useTriangles, int verticesStart, int verticesCount,
                     int instancesCount = 0);

  /** Shaders **/
  void _releaseEffect(Effect* effect);
  Effect* createEffect(const std::string& baseName,
                       EffectCreationOptions& options, Engine* engine);
  Effect* createEffect(std::unordered_map<std::string, std::string>& baseName,
                       EffectCreationOptions& options, Engine* engine);
  Effect* createEffectForParticles(
    const std::string& fragmentName,
    const std::vector<std::string>& uniformsNames,
    const std::vector<std::string>& samplers, const std::string& defines = "",
    EffectFallbacks* fallbacks                                  = nullptr,
    const std::function<void(const Effect* effect)>& onCompiled = nullptr,
    const std::function<void(const Effect* effect, const std::string& errors)>&
      onError
    = nullptr);
  GLFrameProgramPtr createShaderProgram(const std::string& vertexCode,
                                        const std::string& fragmentCode,
                                        const std::string& defines,
                                        GL::IGLRenderingContext* gl = nullptr);
  std::unordered_map<std::string, GLUniformLocationPtr>
  getUniforms(GL::IGLProgram* shaderProgram,
              const std::vector<std::string>& uniformsNames);
  Int32Array getAttributes(GL::IGLProgram* shaderProgram,
                           const std::vector<std::string>& attributesNames);
  void enableEffect(Effect* effect);
  void setIntArray(GL::IGLUniformLocation* uniform, const Int32Array& array);
  void setIntArray2(GL::IGLUniformLocation* uniform, const Int32Array& array);
  void setIntArray3(GL::IGLUniformLocation* uniform, const Int32Array& array);
  void setIntArray4(GL::IGLUniformLocation* uniform, const Int32Array& array);
  void setFloatArray(GL::IGLUniformLocation* uniform,
                     const Float32Array& array);
  void setFloatArray2(GL::IGLUniformLocation* uniform,
                      const Float32Array& array);
  void setFloatArray3(GL::IGLUniformLocation* uniform,
                      const Float32Array& array);
  void setFloatArray4(GL::IGLUniformLocation* uniform,
                      const Float32Array& array);
  void setArray(GL::IGLUniformLocation* uniform, const Float32Array& array);
  void setArray2(GL::IGLUniformLocation* uniform, const Float32Array& array);
  void setArray3(GL::IGLUniformLocation* uniform, const Float32Array& array);
  void setArray4(GL::IGLUniformLocation* uniform, const Float32Array& array);
  void setMatrices(GL::IGLUniformLocation* uniform,
                   const Float32Array& matrices);
  void setMatrix(GL::IGLUniformLocation* uniform, const Matrix& matrix);
  void setMatrix3x3(GL::IGLUniformLocation* uniform,
                    const Float32Array& matrix);
  void setMatrix2x2(GL::IGLUniformLocation* uniform,
                    const Float32Array& matrix);
  void setFloat(GL::IGLUniformLocation* uniform, float value);
  void setFloat2(GL::IGLUniformLocation* uniform, float x, float y);
  void setFloat3(GL::IGLUniformLocation* uniform, float x, float y, float z);
  void setBool(GL::IGLUniformLocation* uniform, int value);
  void setFloat4(GL::IGLUniformLocation* uniform, float x, float y, float z,
                 float w);
  void setColor3(GL::IGLUniformLocation* uniform, const Color3& color3);
  void setColor4(GL::IGLUniformLocation* uniform, const Color3& color3,
                 float alpha);

  /** States **/
  void setState(bool culling, float zOffset = 0.f, bool force = false,
                bool reverseSide = false);
  void setDepthBuffer(bool enable);
  bool getDepthWrite() const;
  void setDepthWrite(bool enable);
  void setColorWrite(bool enable);
  void setAlphaMode(int mode, bool noDepthWriteChange = false);
  int getAlphaMode() const;
  void setAlphaTesting(bool enable);
  bool getAlphaTesting() const;

  /** Textures **/
  void wipeCaches();

  /**
   * @brief Set the compressed texture format to use, based on the formats you
   * have, and the formats supported by the hardware / browser.
   *
   * Khronos Texture Container (.ktx) files are used to support this.  This
   * format has the advantage of being specifically designed for OpenGL.  Header
   * elements directly correspond to API arguments needed to compressed
   * textures.  This puts the burden on the container generator to house the
   * arcane code for determining these for current & future formats.
   *
   * for description see https://www.khronos.org/opengles/sdk/tools/KTX/
   * for file layout see
   * https://www.khronos.org/opengles/sdk/tools/KTX/file_format_spec/
   *
   * Note: The result of this call is not taken into account when a texture is
   * base64.
   *
   * @param {Array<string>} formatsAvailable- The list of those format families
   * you have created on your server. Syntax: '-' + format family + '.ktx'.
   * (Case and order do not matter.)
   *
   * Current families are astc, dxt, pvrtc, etc2, atc, & etc1.
   * @returns The extension selected.
   */
  std::string&
  setTextureFormatToUse(const std::vector<std::string>& formatsAvailable);
  GL::IGLTexture* createTexture(const std::vector<std::string>& list,
                                bool noMipmap, bool invertY, Scene* scene,
                                unsigned int samplingMode
                                = Texture::TRILINEAR_SAMPLINGMODE,
                                const std::function<void()>& onLoad  = nullptr,
                                const std::function<void()>& onError = nullptr,
                                Buffer* buffer                       = nullptr);
  GL::IGLTexture*
  createTexture(const std::string& urlArg, bool noMipmap, bool invertY,
                Scene* scene,
                unsigned int samplingMode = Texture::TRILINEAR_SAMPLINGMODE,
                const std::function<void()>& onLoad  = nullptr,
                const std::function<void()>& onError = nullptr,
                Buffer* buffer = nullptr, GL::IGLTexture* fallBack = nullptr,
                unsigned int format = Engine::TEXTUREFORMAT_RGBA);
  void updateRawTexture(GL::IGLTexture* texture, const Uint8Array& data,
                        unsigned int format, bool invertY = true,
                        const std::string& compression = "");
  GL::IGLTexture* createRawTexture(const Uint8Array& data, int width,
                                   int height, int format, bool generateMipMaps,
                                   bool invertY, unsigned int samplingMode,
                                   const std::string& compression = "");
  GL::IGLTexture* createDynamicTexture(int width, int height,
                                       bool generateMipMaps,
                                       unsigned int samplingMode);
  void updateTextureSamplingMode(unsigned int samplingMode,
                                 GL::IGLTexture* texture);
  void updateDynamicTexture(GL::IGLTexture* texture, ICanvas* canvas,
                            bool invertY, bool premulAlpha = false,
                            unsigned int format = Engine::TEXTUREFORMAT_RGBA);
  GL::IGLTexture* createRenderTargetTexture(ISize size,
                                            const RenderTargetOptions& options);
  unsigned int updateRenderTargetTextureSampleCount(GL::IGLTexture* texture,
                                                    unsigned int samples);
  GL::IGLTexture*
  createRenderTargetCubeTexture(const ISize& size,
                                const RenderTargetOptions& options);
  GL::IGLTexture*
  createCubeTexture(const std::string& rootUrl, Scene* scene,
                    const std::vector<std::string>& extensions, bool noMipmap,
                    const std::function<void()>& onLoad  = nullptr,
                    const std::function<void()>& onError = nullptr,
                    unsigned int format                  = 0);
  void updateTextureSize(GL::IGLTexture* texture, int width, int height);
  void _releaseTexture(GL::IGLTexture* texture);
  void bindSamplers(Effect* effect);
  void _bindTextureDirectly(unsigned int target, GL::IGLTexture* texture);
  void _bindTexture(int channel, GL::IGLTexture* texture);
  void setTextureFromPostProcess(int channel, PostProcess* postProcess);
  void unbindAllTextures();
  void setTexture(int channel, GL::IGLUniformLocation* uniform,
                  BaseTexture* texture);
  void setTextureArray(int channel, GL::IGLUniformLocation* uniform,
                       const std::vector<BaseTexture*>& textures);
  void _setAnisotropicLevel(unsigned int key, BaseTexture* texture);
  Uint8Array readPixels(int x, int y, int width, int height);
  void releaseInternalTexture(GL::IGLTexture* texture);
  void unbindAllAttributes();
  void releaseEffects();

  /** Dispose **/
  void dispose(bool doNotRecurse = false) override;

  /** Loading screen **/
  void displayLoadingUI();
  void hideLoadingUI();
  ILoadingScreen* loadingScreen();
  void setLoadingScreen(ILoadingScreen* loadingScreen);
  void setLoadingUIText(const std::string& text);
  void loadingUIBackgroundColor(const std::string& color);
  std::string getVertexShaderSource(GL::IGLProgram* program);
  std::string getFragmentShaderSource(GL::IGLProgram* program);

  /** FPS **/
  float getFps() const;
  microseconds_t getDeltaTime() const;

  /** Statics **/

  static Engine* LastCreatedEngine();
  static Scene* LastCreatedScene();

  /**
   * @brief Will flag all materials in all scenes in all engines as dirty to
   * trigger new shader compilation
   */
  static void
  MarkAllMaterialsAsDirty(unsigned int flag,
                          const std::function<bool(Material* mat)>& predicate
                          = nullptr);
  static bool isSupported();
  static GLShaderPtr CompileShader(GL::IGLRenderingContext* gl,
                                   const std::string& source,
                                   const std::string& type,
                                   const std::string& defines,
                                   const std::string& shaderVersion);
  static SamplingParameters GetSamplingParameters(unsigned int samplingMode,
                                                  bool generateMipMaps);
  static void PrepareGLTexture(
    GL::IGLTexture* texture, GL::IGLRenderingContext* gl, Scene* scene,
    int width, int height, bool noMipmap, bool isCompressed,
    const std::function<void(int width, int height)>& processFunction,
    bool invertY              = true,
    unsigned int samplingMode = Texture::TRILINEAR_SAMPLINGMODE);

protected:
  /**
   * @brief Constructor.
   * @param {HTMLCanvasElement} canvas - the canvas to be used for rendering
   * @param options - further options to be sent to the getContext function
   */
  Engine(ICanvas* canvas, const EngineOptions& options = EngineOptions());

private:
  void _onVRFullScreenTriggered();
  void _getVRDisplays();
  void _setTexture(unsigned int channel, BaseTexture* texture);
  void bindUnboundFramebuffer(GL::IGLFramebuffer* framebuffer);
  void bindIndexBuffer(GL::IGLBuffer* buffer);
  void bindBuffer(GL::IGLBuffer* buffer, int target);
  void vertexAttribPointer(GL::IGLBuffer* buffer, unsigned int indx, int size,
                           unsigned int type, bool normalized, int stride,
                           int offset);
  void _bindIndexBufferWithCache(GL::IGLBuffer* indexBuffer);
  void _bindVertexBuffersAttributes(
    const std::unordered_map<std::string, VertexBuffer*>& vertexBuffers,
    Effect* effect);
  void _unBindVertexArrayObject();
  void setProgram(GL::IGLProgram* program);
  void activateTexture(unsigned int texture);
  GL::GLenum _getInternalFormat(unsigned int format) const;
  GLRenderBufferPtr
  _setupFramebufferDepthAttachments(bool generateStencilBuffer,
                                    bool generateDepthBuffer, int width,
                                    int height, int samples = 1);
  ArrayBufferView _convertRGBtoRGBATextureData(const ArrayBufferView& rgbData,
                                               int width, int height,
                                               unsigned int textureType);
  /** VBOs **/
  void _resetVertexBufferBinding();
  void _resetIndexBufferBinding();
  /** FPS **/
  void _measureFps();

  bool _canRenderToFloatTexture();
  bool _canRenderToHalfFloatTexture();
  bool _canRenderToFramebuffer(unsigned int type);
  GL::GLenum _getWebGLTextureType(unsigned int type) const;
  GL::GLenum _getRGBABufferInternalSizedFormat(unsigned int type) const;

public:
  // Public members
  bool isFullscreen;
  bool isPointerLock;
  bool cullBackFaces;
  bool renderEvenInBackground;
  bool preventCacheWipeBetweenFrames;
  // To enable/disable IDB support and avoid XHR on .manifest
  bool enableOfflineSupport;
  std::vector<Scene*> scenes;
  // WebVR
  // The new WebVR uses promises.
  // this promise resolves with the current devices available.
  int vrDisplaysPromise;
  int _vrDisplays;
  bool _vrDisplayEnabled;
  Size _oldSize;
  int _oldHardwareScaleFactor;
  int _vrAnimationFrameHandler;
  // OpenGL context
  GL::IGLRenderingContext* _gl;

  static AudioEngine* audioEngine;

  PerfCounter _drawCalls;

private:
  // Private Members
  ICanvas* _renderingCanvas;
  bool _windowIsBackground;
  float _webGLVersion;

  bool _badOS;

  std::function<void()> _onBlur;
  std::function<void()> _onFocus;
  std::function<void()> _onFullscreenChange;
  std::function<void()> _onPointerLockChange;

  int _hardwareScalingLevel;
  EngineCapabilities _caps;
  bool _pointerLockRequested;
  bool _alphaTest;
  bool _isStencilEnable;

  ILoadingScreen* _loadingScreen;

  std::string _glVersion;
  std::string _glRenderer;
  std::string _glVendor;

  bool _videoTextureSupported;

  bool _renderingQueueLaunched;
  std::vector<FastFunc<void()>> _activeRenderLoops;

  // FPS
  unsigned int fpsRange;
  std::vector<high_res_time_point_t> previousFramesDuration;
  float fps;
  microseconds_t deltaTime;

  // States
  std::unique_ptr<Internals::_DepthCullingState> _depthCullingState;
  std::unique_ptr<Internals::_StencilState> _stencilState;
  std::unique_ptr<Internals::_AlphaState> _alphaState;
  int _alphaMode;

  // Cache
  std::vector<GLTexturePtr> _loadedTexturesCache;
  unsigned int _maxTextureChannels;
  unsigned int _activeTexture;
  std::unordered_map<unsigned int, GL::IGLTexture*> _activeTexturesCache;
  Effect* _currentEffect;
  GL::IGLProgram* _currentProgram;
  std::unordered_map<std::string, std::unique_ptr<Effect>> _compiledEffects;
  std::vector<bool> _vertexAttribArraysEnabled;
  Viewport* _cachedViewport;
  GL::IGLVertexArrayObject* _cachedVertexArrayObject;
  std::unordered_map<std::string, VertexBuffer*> _cachedVertexBuffersMap;
  GL::IGLBuffer* _cachedVertexBuffers;
  GL::IGLBuffer* _cachedIndexBuffer;
  Effect* _cachedEffectForVertexBuffers;
  GL::IGLTexture* _currentRenderTarget;
  bool _uintIndicesCurrentlySet;
  std::unordered_map<int, GL::IGLBuffer*> _currentBoundBuffer;
  GL::IGLFramebuffer* _currentFramebuffer;
  std::unordered_map<unsigned int, BufferPointer> _currentBufferPointers;
  Int32Array _currentInstanceLocations;
  std::vector<GL::IGLBuffer*> _currentInstanceBuffers;
  Int32Array _textureUnits;
  bool _vaoRecordInProgress;
  bool _mustWipeVertexAttributes;
  // Hardware supported Compressed Textures
  std::vector<std::string> _texturesSupported;
  std::string _textureFormatInUse;

}; // end of class Engine

struct RenderTargetOptions {
  bool generateMipMaps       = false;
  bool generateDepthBuffer   = true;
  bool generateStencilBuffer = false;
  unsigned int type          = Engine::TEXTURETYPE_UNSIGNED_INT;
  unsigned int samplingMode  = Texture::TRILINEAR_SAMPLINGMODE;
}; // end of struct RenderTargetOptions

} // end of namespace BABYLON

#endif // end of BABYLON_ENGINE_ENGINE_H
