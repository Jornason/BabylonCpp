#include <babylon/animations/animation.h>

#include <babylon/animations/easing/ieasing_function.h>
#include <babylon/core/json.h>
#include <babylon/core/json.h>
#include <babylon/core/string.h>
#include <babylon/engine/scene.h>
#include <babylon/math/color3.h>
#include <babylon/math/matrix.h>
#include <babylon/math/quaternion.h>
#include <babylon/math/size.h>
#include <babylon/math/vector2.h>
#include <babylon/math/vector3.h>
#include <babylon/mesh/abstract_mesh.h>
#include <babylon/mesh/mesh.h>
#include <babylon/tools/tools.h>

namespace BABYLON {

Animation* Animation::_PrepareAnimation(const std::string& name,
                                        const std::string& targetProperty,
                                        size_t framePerSecond, int totalFrame,
                                        const AnimationValue& from,
                                        const AnimationValue& to,
                                        unsigned int loopMode,
                                        IEasingFunction* easingFunction)
{
  auto dataType = from.dataType;

  if (dataType == -1) {
    return nullptr;
  }

  auto animation
    = new Animation(name, targetProperty, framePerSecond, dataType, loopMode);

  animation->setKeys({
    AnimationKey(0, from), AnimationKey(totalFrame, to),
  });

  if (easingFunction != nullptr) {
    animation->setEasingFunction(easingFunction);
  }

  return animation;
}

Animatable* Animation::CreateAndStartAnimation(
  const std::string& name, Node* node, const std::string& targetProperty,
  size_t framePerSecond, int totalFrame, const AnimationValue& from,
  const AnimationValue& to, unsigned int loopMode,
  IEasingFunction* easingFunction, const std::function<void()>& onAnimationEnd)
{

  auto animation = Animation::_PrepareAnimation(
    name, targetProperty, framePerSecond, totalFrame, from, to, loopMode,
    easingFunction);

  return node->getScene()->beginDirectAnimation(
    node, {animation}, 0, totalFrame, (animation->loopMode == 1), 1.f,
    onAnimationEnd);
}

Animatable* Animation::CreateMergeAndStartAnimation(
  const std::string& name, Node* node, const std::string& targetProperty,
  size_t framePerSecond, int totalFrame, const AnimationValue& from,
  const AnimationValue& to, unsigned int loopMode,
  IEasingFunction* easingFunction, const std::function<void()>& onAnimationEnd)
{
  auto animation = Animation::_PrepareAnimation(
    name, targetProperty, framePerSecond, totalFrame, from, to, loopMode,
    easingFunction);

  node->animations.emplace_back(animation);

  return node->getScene()->beginAnimation(
    node, 0, totalFrame, (animation->loopMode == 1), 1.f, onAnimationEnd);
}

Animation::Animation(const std::string& iName,
                     const std::string& iTargetProperty, size_t iFramePerSecond,
                     int iDataType, unsigned int iLoopMode)

    : name{iName}
    , targetProperty{iTargetProperty}
    , targetPropertyPath{String::split(targetProperty, '.')}
    , framePerSecond{iFramePerSecond}
    , dataType{iDataType}
    , loopMode{iLoopMode}
    , allowMatricesInterpolation{false}
    , blendingSpeed{0.01f}
    , enableBlending{false}
    , _stopped{false}
    , _blendingFactor{0.f}
    , _easingFunction{nullptr}
{
}

Animation::~Animation()
{
}

std::string Animation::toString(bool fullDetails) const
{
  std::ostringstream oss;
  oss << "Name: " << name << ", property: " << targetProperty;
  if (dataType >= 0
      && static_cast<unsigned int>(dataType) <= ANIMATIONTYPE_BOOL) {
    size_t _dataType = static_cast<size_t>(dataType);
    oss << ", datatype: "
        << std::vector<std::string>{"Float",  "Vector3", "Quaternion",
                                    "Matrix", "Color3",  "Vector2",
                                    "Size",   "Boolean"}[_dataType];
  }
  oss << ", nKeys: "
      << (!_keys.empty() ? std::to_string(_keys.size()) : "none");
  oss << ", nRanges: "
      << (!_ranges.empty() ? std::to_string(_ranges.size()) : "none");
  if (fullDetails) {
    oss << ", Ranges: {";
    bool first = true;
    for (auto& range : _ranges) {
      if (first) {
        oss << ", ";
        first = false;
      }
      oss << range.first;
    }
    oss << "}";
  }
  return oss.str();
}

void Animation::addEvent(const AnimationEvent& event)
{
  _events.emplace_back(event);
}

void Animation::removeEvents(int frame)
{
  _events.erase(std::remove_if(_events.begin(), _events.end(),
                               [frame](const AnimationEvent& event) {
                                 return event.frame == frame;
                               }),
                _events.end());
}

void Animation::createRange(const std::string& _name, float from, float to)
{
  // check name not already in use; could happen for bones after serialized
  if (!std_util::contains(_ranges, _name)) {
    _ranges[name] = AnimationRange(_name, from, to);
  }
}

void Animation::deleteRange(const std::string& iName, bool deleteFrames)
{
  if (std_util::contains(_ranges, iName)) {
    if (deleteFrames) {
      auto from = _ranges[iName].from;
      auto to   = _ranges[iName].to;

      _keys.erase(std::remove_if(_keys.begin(), _keys.end(),
                                 [from, to](const AnimationKey& key) {
                                   return key.frame >= from && key.frame <= to;
                                 }),
                  _keys.end());
    }
    _ranges.erase(iName);
  }
}

AnimationRange& Animation::getRange(const std::string& iName)
{
  return _ranges[iName];
}

void Animation::reset()
{
  _offsetsCache.clear();
  _highLimitsCache.clear();
  currentFrame        = 0;
  _blendingFactor     = 0;
  _originalBlendValue = 0.f;
}

bool Animation::isStopped() const
{
  return _stopped;
}

std::vector<AnimationKey>& Animation::getKeys()
{
  return _keys;
}

int Animation::getHighestFrame() const
{
  int ret = 0;
  for (auto& key : _keys) {
    if (ret < key.frame) {
      ret = key.frame;
    }
  }
  return ret;
}

IEasingFunction* Animation::getEasingFunction()
{
  return _easingFunction;
}

void Animation::setEasingFunction(IEasingFunction* easingFunction)
{
  _easingFunction = easingFunction;
}

float Animation::floatInterpolateFunction(float startValue, float endValue,
                                          float gradient) const
{
  return startValue + (endValue - startValue) * gradient;
}

Quaternion
Animation::quaternionInterpolateFunction(const Quaternion& startValue,
                                         const Quaternion& endValue,
                                         float gradient) const
{
  return Quaternion::Slerp(startValue, endValue, gradient);
}

Vector3 Animation::vector3InterpolateFunction(const Vector3& startValue,
                                              const Vector3& endValue,
                                              float gradient) const
{
  return Vector3::Lerp(startValue, endValue, gradient);
}

Vector2 Animation::vector2InterpolateFunction(const Vector2& startValue,
                                              const Vector2& endValue,
                                              float gradient) const
{
  return Vector2::Lerp(startValue, endValue, gradient);
}

Size Animation::sizeInterpolateFunction(const Size& startValue,
                                        const Size& endValue,
                                        float gradient) const
{
  return Size::Lerp(startValue, endValue, gradient);
}

Color3 Animation::color3InterpolateFunction(const Color3& startValue,
                                            const Color3& endValue,
                                            float gradient) const
{
  return Color3::Lerp(startValue, endValue, gradient);
}

Matrix Animation::matrixInterpolateFunction(const Matrix& startValue,
                                            const Matrix& endValue,
                                            float gradient) const
{
  return Matrix::Lerp(startValue, endValue, gradient);
}

std::unique_ptr<Animation> Animation::clone() const
{
  auto clonedAnimation = std_util::make_unique<Animation>(
    name, String::join(targetPropertyPath, '.'), framePerSecond, dataType,
    loopMode);

  clonedAnimation->enableBlending = enableBlending;
  clonedAnimation->blendingSpeed  = blendingSpeed;

  if (!_keys.empty()) {
    clonedAnimation->setKeys(_keys);
  }

  if (!_ranges.empty()) {
    for (auto& range : _ranges) {
      clonedAnimation->_ranges[range.first] = range.second.copy();
    }
  }

  return clonedAnimation;
}

void Animation::setKeys(const std::vector<AnimationKey>& values)
{
  _keys = values;
  _offsetsCache.clear();
  _highLimitsCache.clear();
}

AnimationValue Animation::_getKeyValue(const AnimationValue& value) const
{
  return value;
}

AnimationValue Animation::_interpolate(int iCurrentFrame, int repeatCount,
                                       unsigned int iLoopMode,
                                       const AnimationValue& offsetValue,
                                       const AnimationValue& highLimitValue)
{
  if (iLoopMode == Animation::ANIMATIONLOOPMODE_CONSTANT && repeatCount > 0) {
    return highLimitValue.copy();
  }

  currentFrame       = iCurrentFrame;
  float _repeatCount = static_cast<float>(repeatCount);

  // Try to get a hash to find the right key
  int _keysLength = static_cast<int>(_keys.size());
  int startKey    = std::max(
    0, std::min(_keysLength - 1,
                static_cast<int>(
                  std::floor(_keysLength * (currentFrame - _keys[0].frame)
                             / (_keys.back().frame - _keys[0].frame))
                  - 1)));

  if (_keys[static_cast<unsigned int>(startKey)].frame >= currentFrame) {
    while (startKey - 1 >= 0
           && _keys[static_cast<unsigned int>(startKey)].frame
                >= currentFrame) {
      --startKey;
    }
  }

  for (size_t key = static_cast<unsigned int>(startKey); key < _keys.size();
       ++key) {
    if (_keys[key + 1].frame >= currentFrame) {

      const auto startValue = _getKeyValue(_keys[key].value);
      const auto endValue   = _getKeyValue(_keys[key + 1].value);

      // gradient : percent of currentFrame between the frame inf and the frame
      // sup
      float gradient
        = static_cast<float>(currentFrame - _keys[key].frame)
          / static_cast<float>(_keys[key + 1].frame - _keys[key].frame);

      // check for easingFunction and correction of gradient
      if (_easingFunction != nullptr) {
        gradient = _easingFunction->ease(gradient);
      }

      auto newVale = _keys[key].value.copy();

      switch (dataType) {
        // Float
        case Animation::ANIMATIONTYPE_FLOAT:
          switch (loopMode) {
            case Animation::ANIMATIONLOOPMODE_CYCLE:
            case Animation::ANIMATIONLOOPMODE_CONSTANT:
              newVale.floatData = floatInterpolateFunction(
                startValue.floatData, endValue.floatData, gradient);
              return newVale;
            case Animation::ANIMATIONLOOPMODE_RELATIVE:
              newVale.floatData
                = offsetValue.floatData * _repeatCount
                  + floatInterpolateFunction(startValue.floatData,
                                             endValue.floatData, gradient);
              return newVale;
            default:
              break;
          }
          break;
        // Quaternion
        case Animation::ANIMATIONTYPE_QUATERNION:
          switch (loopMode) {
            case Animation::ANIMATIONLOOPMODE_CYCLE:
            case Animation::ANIMATIONLOOPMODE_CONSTANT:
              newVale.quaternionData = quaternionInterpolateFunction(
                startValue.quaternionData, endValue.quaternionData, gradient);
              return newVale;
            case Animation::ANIMATIONLOOPMODE_RELATIVE:
              newVale.quaternionData
                = quaternionInterpolateFunction(startValue.quaternionData,
                                                endValue.quaternionData,
                                                gradient)
                    .add(offsetValue.quaternionData.scale(_repeatCount));
              return newVale;
            default:
              break;
          }
          break;
        // Vector3
        case Animation::ANIMATIONTYPE_VECTOR3:
          switch (loopMode) {
            case Animation::ANIMATIONLOOPMODE_CYCLE:
            case Animation::ANIMATIONLOOPMODE_CONSTANT:
              newVale.vector3Data = vector3InterpolateFunction(
                startValue.vector3Data, endValue.vector3Data, gradient);
              return newVale;
            case Animation::ANIMATIONLOOPMODE_RELATIVE:
              newVale.vector3Data
                = vector3InterpolateFunction(startValue.vector3Data,
                                             endValue.vector3Data, gradient)
                    .add(offsetValue.vector3Data.scale(_repeatCount));
              return newVale;
            default:
              break;
          }
          break;
        // Vector2
        case Animation::ANIMATIONTYPE_VECTOR2:
          switch (loopMode) {
            case Animation::ANIMATIONLOOPMODE_CYCLE:
            case Animation::ANIMATIONLOOPMODE_CONSTANT:
              newVale.vector2Data = vector2InterpolateFunction(
                startValue.vector2Data, endValue.vector2Data, gradient);
              return newVale;
            case Animation::ANIMATIONLOOPMODE_RELATIVE:
              newVale.vector2Data
                = vector2InterpolateFunction(startValue.vector2Data,
                                             endValue.vector2Data, gradient)
                    .add(offsetValue.vector2Data.scale(_repeatCount));
              return newVale;
            default:
              break;
          }
          break;
        // Size
        case Animation::ANIMATIONTYPE_SIZE:
          switch (loopMode) {
            case Animation::ANIMATIONLOOPMODE_CYCLE:
            case Animation::ANIMATIONLOOPMODE_CONSTANT:
              newVale.sizeData = sizeInterpolateFunction(
                startValue.sizeData, endValue.sizeData, gradient);
              return newVale;
            case Animation::ANIMATIONLOOPMODE_RELATIVE:
              newVale.sizeData
                = sizeInterpolateFunction(startValue.sizeData,
                                          endValue.sizeData, gradient)
                    .add(offsetValue.sizeData.scale(_repeatCount));
              return newVale;
            default:
              break;
          }
          break;
        // Color3
        case Animation::ANIMATIONTYPE_COLOR3:
          switch (loopMode) {
            case Animation::ANIMATIONLOOPMODE_CYCLE:
            case Animation::ANIMATIONLOOPMODE_CONSTANT:
              newVale.color3Data = color3InterpolateFunction(
                startValue.color3Data, endValue.color3Data, gradient);
              return newVale;
            case Animation::ANIMATIONLOOPMODE_RELATIVE:
              newVale.color3Data
                = color3InterpolateFunction(startValue.color3Data,
                                            endValue.color3Data, gradient)
                    .add(offsetValue.color3Data.scale(_repeatCount));
              return newVale;
            default:
              break;
          }
          break;
        // Matrix
        case Animation::ANIMATIONTYPE_MATRIX:
          switch (loopMode) {
            case Animation::ANIMATIONLOOPMODE_CYCLE:
            case Animation::ANIMATIONLOOPMODE_CONSTANT:
              if (allowMatricesInterpolation) {
                newVale.matrixData = matrixInterpolateFunction(
                  startValue.matrixData, endValue.matrixData, gradient);
                return newVale;
              }
              newVale.matrixData = startValue.matrixData;
              return newVale;
            case Animation::ANIMATIONLOOPMODE_RELATIVE:
              newVale.matrixData = startValue.matrixData;
              return newVale;
            default:
              break;
          }
          break;
        default:
          break;
      }
      break;
    }
  }
  return _getKeyValue(_keys.back().value);
}

void Animation::setValue(const AnimationValue& currentValue, bool /*blend*/)
{
  // Set value
  std::string path;
  any destination;

  if (targetPropertyPath.size() > 1) {
    auto property = _target->getProperty(targetPropertyPath[0]);

    for (size_t index = 1; index < targetPropertyPath.size() - 1; ++index) {
      property = _target->getProperty(property, targetPropertyPath[index]);
    }

    path        = targetPropertyPath.back();
    destination = property;
  }
  else {
    path        = targetPropertyPath[0];
    destination = _target;
  }

  // Blending
  if (enableBlending && _blendingFactor <= 1.f) {
  }
  else {
    any newValue = currentValue.getValue();
    _target->setProperty(destination, path, newValue);
  }
}

void Animation::goToFrame(int frame)
{
  int _frame = frame;
  if (_frame < _keys[0].frame) {
    _frame = _keys[0].frame;
  }
  else if (_frame > _keys.back().frame) {
    _frame = _keys.back().frame;
  }

  auto currentValue = _interpolate(_frame, 0, loopMode);

  setValue(currentValue);
}

bool Animation::animate(millisecond_t delay, float from, float to, bool loop,
                        float speedRatio)
{
  if (this->targetProperty.empty()) {
    _stopped = true;
    return false;
  }

  bool returnValue = true;

  // Adding a start key at frame 0 if missing
  if (_keys[0].frame != 0) {
    _keys.clear();
    _keys.insert(_keys.begin(), AnimationKey(0, _keys[0].value));
  }

  // Check limits
  if (from < _keys[0].frame || from > _keys.back().frame) {
    from = _keys[0].frame;
  }
  if (to < _keys[0].frame || to > _keys.back().frame) {
    to = _keys.back().frame;
  }

  // Compute ratio
  float range = to - from;
  AnimationValue offsetValue;
  // ratio represents the frame delta between from and to
  float ratio
    = (static_cast<float>(delay.count() * framePerSecond) * speedRatio)
      / 1000.f;
  AnimationValue highLimitValue;

  if (ratio > range && !loop) {
    // If we are out of range and not looping get back to caller
    returnValue    = false;
    highLimitValue = _getKeyValue(_keys.back().value);
  }
  else {
    // Get max value if required
    if (loopMode != Animation::ANIMATIONLOOPMODE_CYCLE) {
      std::string keyOffset = std::to_string(to) + std::to_string(from);
      if (!_offsetsCache.count(keyOffset)) {
        AnimationValue fromValue = _interpolate(
          static_cast<int>(from), 0, Animation::ANIMATIONLOOPMODE_CYCLE);
        AnimationValue toValue = _interpolate(
          static_cast<int>(to), 0, Animation::ANIMATIONLOOPMODE_CYCLE);
        switch (dataType) {
          // Float
          case Animation::ANIMATIONTYPE_FLOAT:
            _offsetsCache[keyOffset] = toValue - fromValue;
            break;
          // Quaternion
          case Animation::ANIMATIONTYPE_QUATERNION:
            _offsetsCache[keyOffset] = toValue.subtract(fromValue);
            break;
          // Vector3
          case Animation::ANIMATIONTYPE_VECTOR3:
            _offsetsCache[keyOffset] = toValue.subtract(fromValue);
            break;
          // Vector2
          case Animation::ANIMATIONTYPE_VECTOR2:
            _offsetsCache[keyOffset] = toValue.subtract(fromValue);
            break;
          // Size
          case Animation::ANIMATIONTYPE_SIZE:
            _offsetsCache[keyOffset] = toValue.subtract(fromValue);
            break;
          // Color3
          case Animation::ANIMATIONTYPE_COLOR3:
            _offsetsCache[keyOffset] = toValue.subtract(fromValue);
            break;
          default:
            break;
        }

        _highLimitsCache[keyOffset] = toValue;
      }

      highLimitValue = _highLimitsCache[keyOffset];
      offsetValue    = _offsetsCache[keyOffset];
    }
  }

  if (offsetValue.dataType == -1) {
    switch (dataType) {
      // Float
      case Animation::ANIMATIONTYPE_FLOAT:
        offsetValue = AnimationValue(0.f);
        break;
      // Quaternion
      case Animation::ANIMATIONTYPE_QUATERNION:
        offsetValue = AnimationValue(Quaternion(0.f, 0.f, 0.f, 0.f));
        break;
      // Vector3
      case Animation::ANIMATIONTYPE_VECTOR3:
        offsetValue = AnimationValue(Vector3::Zero());
        break;
      // Vector2
      case Animation::ANIMATIONTYPE_VECTOR2:
        offsetValue = AnimationValue(Vector2::Zero());
        break;
      // Size
      case Animation::ANIMATIONTYPE_SIZE:
        offsetValue = Size::Zero();
        break;
      // Color3
      case Animation::ANIMATIONTYPE_COLOR3:
        offsetValue = AnimationValue(Color3::Black());
        break;
      default:
        break;
    }
  }

  // Compute value
  int repeatCount = static_cast<int>(ratio / range);
  int _currentFrame
    = returnValue ? static_cast<int>(from + ratio) % static_cast<int>(range) :
                    static_cast<int>(to);
  AnimationValue currentValue = _interpolate(
    _currentFrame, repeatCount, loopMode, offsetValue, highLimitValue);

  // Set value
  setValue(currentValue);
  // Check events
  for (unsigned int index = 0; index < _events.size(); ++index) {
    if (currentFrame >= _events[index].frame) {
      AnimationEvent& event = _events[index];
      if (!event.isDone) {
        // If event should be done only once, remove it.
        if (event.onlyOnce) {
          _events.erase(_events.begin() + index);
          --index;
        }
        event.isDone = true;
        event.action();
      } // Don't do anything if the event has already be done.
    }
    else if (_events[index].isDone && !_events[index].onlyOnce) {
      // reset event, the animation is looping
      _events[index].isDone = false;
    }
  }
  if (!returnValue) {
    _stopped = true;
  }

  return returnValue;
}

Json::object Animation::serialize() const
{
  auto serializationObject
    = Json::object({Json::Pair("name", name),                             //
                    Json::Pair("property", targetProperty),               //
                    Json::Pair<size_t>("framePerSecond", framePerSecond), //
                    Json::Pair<int>("dataType", dataType),                //
                    Json::Pair<unsigned>("loopBehavior", loopMode)});

  // Animation keys
  std::vector<Json::value> keys;
  for (auto& animationKey : _keys) {
    const AnimationValue& value = animationKey.value;
    Float32Array keyValues;
    switch (dataType) {
      case Animation::ANIMATIONTYPE_FLOAT:
        keyValues.emplace_back(value.floatData);
        break;
      case Animation::ANIMATIONTYPE_QUATERNION:
        std_util::concat(keyValues, value.quaternionData.asArray());
        break;
      case Animation::ANIMATIONTYPE_MATRIX:
        std_util::concat(keyValues, value.matrixData.asArray());
        break;
      case Animation::ANIMATIONTYPE_VECTOR3:
        std_util::concat(keyValues, value.vector3Data.asArray());
        break;
      case Animation::ANIMATIONTYPE_COLOR3:
        std_util::concat(keyValues, value.color3Data.asArray());
        break;
    }

    keys.emplace_back(
      Json::value(Json::object({Json::Pair<int>("frame", animationKey.frame), //
                                /*Json::Pair("values", keyValues)*/})));
  }
  serializationObject["keys"] = Json::value(keys);

  // Animation ranges
  std::vector<Json::value> ranges;
  for (auto& range : _ranges) {
    ranges.emplace_back(Json::value(Json::object(
      {Json::Pair("name", range.first),                              //
       Json::Pair<int>("from", static_cast<int>(range.second.from)), //
       Json::Pair<int>("to", static_cast<int>(range.second.to))})));
  }
  serializationObject["ranges"] = Json::value(ranges);

  return serializationObject;
}

Animation* Animation::Parse(const Json::value& parsedAnimation)
{
  auto animation
    = new Animation(Json::GetString(parsedAnimation, "name"),
                    Json::GetString(parsedAnimation, "property"),
                    Json::GetNumber(parsedAnimation, "framePerSecond", 30ull),
                    Json::GetNumber(parsedAnimation, "dataType", 0),
                    Json::GetNumber(parsedAnimation, "loopBehavior",
                                    Animation::ANIMATIONLOOPMODE_CYCLE));

  auto dataType = Json::GetNumber(parsedAnimation, "dataType", 0);
  std::vector<AnimationKey> keys;

  if (parsedAnimation.contains("enableBlending")) {
    animation->enableBlending
      = Json::GetBool(parsedAnimation, "enableBlending");
  }

  if (parsedAnimation.contains("blendingSpeed")) {
    animation->blendingSpeed
      = Json::GetNumber(parsedAnimation, "blendingSpeed", 0.01f);
  }

  for (auto& key : Json::GetArray(parsedAnimation, "keys")) {
    AnimationValue data;

    switch (dataType) {
      case Animation::ANIMATIONTYPE_FLOAT:
        data = AnimationValue(Json::ToArray<float>(key, "values")[0]);
        break;
      case Animation::ANIMATIONTYPE_QUATERNION:
        data = AnimationValue(
          Quaternion::FromArray(Json::ToArray<float>(key, "values")));
        break;
      case Animation::ANIMATIONTYPE_MATRIX:
        data = AnimationValue(
          Matrix::FromArray(Json::ToArray<float>(key, "values")));
        break;
      case Animation::ANIMATIONTYPE_COLOR3:
        data = AnimationValue(
          Color3::FromArray(Json::ToArray<float>(key, "values")));
        break;
      case Animation::ANIMATIONTYPE_VECTOR3:
      default:
        data = AnimationValue(
          Vector3::FromArray(Json::ToArray<float>(key, "values")));
        break;
    }

    keys.emplace_back(AnimationKey(Json::GetNumber(key, "frame", 0), data));
  }

  animation->setKeys(keys);

  if (parsedAnimation.contains("ranges")) {
    for (auto& range : Json::GetArray(parsedAnimation, "ranges")) {
      animation->createRange(Json::GetString(range, "name"),
                             Json::GetNumber(range, "from", 0),
                             Json::GetNumber(range, "to", 0));
    }
  }

  return animation;
}

} // end of namespace BABYLON
