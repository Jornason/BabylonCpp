#ifndef BABYLON_PHYSICS_IWORLD_H
#define BABYLON_PHYSICS_IWORLD_H

#include <babylon/babylon_global.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT IWorld {

  virtual void create(float timeStep, unsigned int broadPhaseType,
                      unsigned int iterations, bool noStat)
    = 0;
  virtual void clear()                                 = 0;
  virtual void setNoStat(bool)                         = 0;
  virtual void setGravity(const Vector3& gravity)      = 0;
  virtual void setTimeStep(float timeStep)             = 0;
  virtual void step()                                  = 0;
  virtual void removeJoint(PhysicsJoint* joint)        = 0;
  virtual void removeRigidBody(IPhysicsBody* impostor) = 0;

}; // end of struct IWorld

} // end of namespace BABYLON

#endif // end of BABYLON_PHYSICS_IWORLD_H
