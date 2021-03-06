#ifndef BABYLON_PARTICLES_PARTICLE_SYSTEM_H
#define BABYLON_PARTICLES_PARTICLE_SYSTEM_H

#include <babylon/animations/ianimatable.h>
#include <babylon/babylon_global.h>
#include <babylon/core/fast_func.h>
#include <babylon/interfaces/idisposable.h>
#include <babylon/math/color4.h>
#include <babylon/math/vector3.h>
#include <babylon/tools/observable.h>
#include <babylon/tools/observer.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT ParticleSystem : public IDisposable,
                                             public IAnimatable {

public:
  /** Statics **/
  static constexpr int BLENDMODE_ONEONE   = 0;
  static constexpr int BLENDMODE_STANDARD = 1;

public:
  ParticleSystem(const std::string& name, size_t capacity, Scene* scene,
                 Effect* customEffect = nullptr);
  virtual ~ParticleSystem();

  virtual IReflect::Type type() const override;

  void setOnDispose(const FastFunc<void()>& callback);
  void recycleParticle(Particle* particle);
  size_t getCapacity() const;
  bool isAlive() const;
  bool isStarted() const;
  void start();
  void stop();
  void _appendParticleVertex(unsigned int index, Particle* particle,
                             int offsetX, int offsetY);
  void animate();
  size_t render();
  void dispose(bool doNotRecurse = false) override;
  std::vector<Animation*> getAnimations() override;
  ParticleSystem* clone(const std::string& name, Mesh* newEmitter);

  // Statics
  static ParticleSystem* Parse(const Json::value& parsedParticleSystem,
                               Scene* scene, const std::string& url);

private:
  void _update(int newParticles);
  Effect* _getEffect();

public:
  // Members
  std::vector<Animation*> animations;
  std::string id;
  std::string name;
  unsigned int renderingGroupId;
  AbstractMesh* emitter;
  Vector3 emitterVec;
  int emitRate;
  int manualEmitCount;
  float updateSpeed;
  int targetStopDuration;
  bool disposeOnStop;
  float minEmitPower;
  float maxEmitPower;
  float minLifeTime;
  float maxLifeTime;
  float minSize;
  float maxSize;
  float minAngularSpeed;
  float maxAngularSpeed;
  Texture* particleTexture;
  unsigned int layerMask;
  std::string customShader;
  bool preventAutoStart;
  Observable<ParticleSystem> onDisposeObservable;
  std::function<void(std::vector<Particle*>& particles)> updateFunction;
  int blendMode;
  bool forceDepthWrite;
  Vector3 gravity;
  Vector3 direction1;
  Vector3 direction2;
  Vector3 minEmitBox;
  Vector3 maxEmitBox;
  Color4 color1;
  Color4 color2;
  Color4 colorDead;
  Color4 textureMask;
  std::function<void(float emitPower, const Matrix& worldMatrix,
                     Vector3& directionToUpdate, Particle* particle)>
    startDirectionFunction;
  std::function<void(const Matrix& worldMatrix, Vector3& positionToUpdate,
                     Particle* particle)>
    startPositionFunction;

private:
  Observer<ParticleSystem>::Ptr _onDisposeObserver;
  std::vector<Particle*> particles;
  size_t _capacity;
  Scene* _scene;
  std::vector<Particle*> _stockParticles;
  int _newPartsExcess;
  Float32Array _vertexData;
  std::unique_ptr<Buffer> _vertexBuffer;
  std::unordered_map<std::string, std::unique_ptr<VertexBuffer>> _vertexBuffers;
  std::unique_ptr<GL::IGLBuffer> _indexBuffer;
  Effect* _effect;
  Effect* _customEffect;
  std::string _cachedDefines;

  Color4 _scaledColorStep;
  Color4 _colorDiff;
  Vector3 _scaledDirection;
  Vector3 _scaledGravity;
  int _currentRenderId;

  bool _alive;
  bool _started;
  bool _stopped;
  int _actualFrame;
  int _scaledUpdateSpeed;

}; // end of class ParticleSystem

} // end of namespace BABYLON

#endif // end of BABYLON_PARTICLES_PARTICLE_SYSTEM_H
