#include <babylon/math/color4.h>

#include <babylon/core/string.h>
#include <babylon/math/math_tools.h>
#include <babylon/tools/tools.h>

namespace BABYLON {

Color4::Color4(float red, float green, float blue, float alpha)
    : r{red}, g{green}, b{blue}, a{alpha}
{
}

Color4::Color4(const Color4& otherColor)
    : r{otherColor.r}, g{otherColor.g}, b{otherColor.b}, a{otherColor.a}
{
}

Color4::Color4(Color4&& otherColor)
    : r{std::move(otherColor.r)}
    , g{std::move(otherColor.g)}
    , b{std::move(otherColor.b)}
    , a{std::move(otherColor.a)}
{
}

Color4& Color4::operator=(const Color4& otherColor)
{
  if (&otherColor != this) {
    r = otherColor.r;
    g = otherColor.g;
    b = otherColor.b;
    a = otherColor.a;
  }

  return *this;
}

Color4& Color4::operator=(Color4&& otherColor)
{
  if (&otherColor != this) {
    r = std::move(otherColor.r);
    g = std::move(otherColor.g);
    b = std::move(otherColor.b);
    a = std::move(otherColor.a);
  }

  return *this;
}

Color4::~Color4()
{
}

Color4 Color4::copy() const
{
  return Color4(*this);
}

std::unique_ptr<Color4> Color4::clone() const
{
  return std_util::make_unique<Color4>(*this);
}

const char* Color4::getClassName() const
{
  return "Color4";
}

int Color4::getHashCode() const
{
  float hash = r;
  hash       = std::pow((hash * 397), g);
  hash       = std::pow((hash * 397), b);
  hash       = std::pow((hash * 397), a);
  return static_cast<int>(hash);
}

/** Operators **/
Color4& Color4::addInPlace(const Color4& right)
{
  r += right.r;
  g += right.g;
  b += right.b;
  a += right.a;

  return *this;
}

const Float32Array Color4::asArray() const
{
  Float32Array result;
  toArray(result, 0);

  return result;
}

const Color4& Color4::toArray(Float32Array& array, unsigned int index) const
{
  if (array.size() < index + 4) {
    array.resize(index + 4);
  }

  array[index]     = r;
  array[index + 1] = g;
  array[index + 2] = b;
  array[index + 3] = a;

  return *this;
}

Color4 Color4::add(const Color4& right) const
{
  return Color4(r + right.r, g + right.g, b + right.b, a + right.a);
}

Color4 Color4::subtract(const Color4& right) const
{
  return Color4(r - right.r, g - right.g, b - right.b, a - right.a);
}

const Color4& Color4::subtractToRef(const Color4& right, Color4& result) const
{
  result.r = r - right.r;
  result.g = g - right.g;
  result.b = b - right.b;
  result.a = a - right.a;

  return *this;
}

Color4 Color4::scale(float iscale) const
{
  return Color4(r * iscale, g * iscale, b * iscale, a * iscale);
}

const Color4& Color4::scaleToRef(int iscale, Color4& result) const
{
  result.r = r * static_cast<float>(iscale);
  result.g = g * static_cast<float>(iscale);
  result.b = b * static_cast<float>(iscale);
  result.a = a * static_cast<float>(iscale);

  return *this;
}

const Color4& Color4::scaleToRef(float iscale, Color4& result) const
{
  result.r = r * iscale;
  result.g = g * iscale;
  result.b = b * iscale;
  result.a = a * iscale;

  return *this;
}

Color4 Color4::multiply(const Color4& color) const
{
  return Color4(r * color.r, g * color.g, b * color.b, a * color.a);
}

const Color4 Color4::multiplyToRef(const Color4& color, Color4& result) const
{
  result.r = r * color.r;
  result.g = g * color.g;
  result.b = b * color.b;
  result.a = a * color.a;

  return result;
}

bool Color4::equals(const Color4& otherColor) const
{
  return std_util::almost_equal(r, otherColor.r)
         && std_util::almost_equal(g, otherColor.g)
         && std_util::almost_equal(b, otherColor.b)
         && std_util::almost_equal(a, otherColor.a);
}

std::string Color4::toString() const
{
  std::ostringstream oss;
  oss << *this;

  return oss.str();
}

Color4& Color4::copyFrom(const Color4& source)
{
  r = source.r;
  g = source.g;
  b = source.b;
  a = source.a;

  return *this;
}

Color4& Color4::copyFromFloats(float red, float green, float blue, float alpha)
{
  r = red;
  g = green;
  b = blue;
  a = alpha;

  return *this;
}

Color4& Color4::set(float red, float green, float blue, float alpha)
{
  return copyFromFloats(red, green, blue, alpha);
}

std::string Color4::toHexString() const
{
  const int intR = static_cast<int>(r * 255) | 0;
  const int intG = static_cast<int>(g * 255) | 0;
  const int intB = static_cast<int>(b * 255) | 0;
  const int intA = static_cast<int>(a * 255) | 0;

  std::ostringstream ostream;
  ostream << "#" << String::toUpperCase(MathTools::ToHex(intR))
          << String::toUpperCase(MathTools::ToHex(intG))
          << String::toUpperCase(MathTools::ToHex(intB))
          << String::toUpperCase(MathTools::ToHex(intA));
  return ostream.str();
}

/** Operator overloading **/
std::ostream& operator<<(std::ostream& os, const Color4& color)
{
  os << "{\"R\":" << color.r << ",\"G\":" << color.g << ",\"B\":" << color.b
     << ",\"A\":" << color.a << "}";
  return os;
}

bool operator==(const Color4& lhs, const Color4& rhs)
{
  return lhs.equals(rhs);
}

bool operator!=(const Color4& lhs, const Color4& rhs)
{
  return !(lhs == rhs);
}

Color4 Color4::operator+(const Color4& right) const
{
  return add(right);
}

Color4& Color4::operator+=(const Color4& right)
{
  return addInPlace(right);
}

Color4 Color4::operator-(const Color4& right) const
{
  return subtract(right);
}

Color4 Color4::operator*(float iscale) const
{
  return scale(iscale);
}

/** Statics **/
Color4 Color4::FromHexString(const std::string& hex)
{
  if (hex.substr(0, 1) != "#" || hex.size() != 9) {
    return Color4(0, 0, 0, 0);
  }

  return Color4::FromInts(
    static_cast<int>(strtol(hex.substr(1, 2).c_str(), NULL, 16)),
    static_cast<int>(strtol(hex.substr(3, 2).c_str(), NULL, 16)),
    static_cast<int>(strtol(hex.substr(5, 2).c_str(), NULL, 16)),
    static_cast<int>(strtol(hex.substr(7, 2).c_str(), NULL, 16)));
}

Color4 Color4::Lerp(const Color4& left, const Color4& right, float amount)
{
  Color4 result(0.f, 0.f, 0.f, 0.f);
  Color4::LerpToRef(left, right, amount, result);

  return result;
}

void Color4::LerpToRef(const Color4& left, const Color4& right, float amount,
                       Color4& result)
{
  result.r = left.r + (right.r - left.r) * amount;
  result.g = left.g + (right.g - left.g) * amount;
  result.b = left.b + (right.b - left.b) * amount;
  result.a = left.a + (right.a - left.a) * amount;
}

Color4 Color4::FromArray(const Float32Array& array, unsigned int offset)
{
  return Color4(array[offset], array[offset + 1], array[offset + 2],
                array[offset + 3]);
}

Color4 Color4::FromInts(int r, int g, int b, int a)
{
  return Color4(static_cast<float>(r) / 255.0f, static_cast<float>(g) / 255.0f,
                static_cast<float>(b) / 255.0f, static_cast<float>(a) / 255.0f);
}

Float32Array Color4::CheckColors4(const Float32Array& colors, size_t count)
{
  // Check if color3 was used
  if (colors.size() == count * 3) {
    Float32Array colors4;
    colors4.resize(count * 4);
    for (size_t index = 0; index < colors.size(); index += 3) {
      size_t newIndex       = (index / 3) * 4;
      colors4[newIndex]     = colors[index];
      colors4[newIndex + 1] = colors[index + 1];
      colors4[newIndex + 2] = colors[index + 2];
      colors4[newIndex + 3] = 1.f;
    }

    return colors4;
  }

  return colors;
}

} // end of namespace BABYLON
