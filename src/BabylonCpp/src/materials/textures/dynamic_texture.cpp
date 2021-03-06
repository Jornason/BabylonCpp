#include <babylon/materials/textures/dynamic_texture.h>

#include <babylon/core/string.h>
#include <babylon/engine/scene.h>
#include <babylon/interfaces/icanvas.h>
#include <babylon/interfaces/icanvas_rendering_context2D.h>

namespace BABYLON {

DynamicTexture::DynamicTexture(const std::string& iName,
                               const DynamicTextureOptions& options,
                               Scene* scene, bool generateMipMaps,
                               unsigned int samplingMode, unsigned int format)
    : Texture{nullptr, scene,   !generateMipMaps, true,  samplingMode,
              nullptr, nullptr, nullptr,          false, format}
    , _samplingMode{samplingMode}
    , _generateMipMaps{generateMipMaps}
{
  name = iName;

  wrapU = Texture::CLAMP_ADDRESSMODE;
  wrapV = Texture::CLAMP_ADDRESSMODE;

  if (options.canvas) {
    _canvas  = options.canvas;
    _texture = scene->getEngine()->createDynamicTexture(
      options.width, options.height, generateMipMaps, samplingMode);
  }
  else {
    _texture = scene->getEngine()->createDynamicTexture(
      options.width, options.height, generateMipMaps, samplingMode);
  }

  if (_canvas) {
    auto textureSize = getSize();
    _canvas->width   = textureSize.width;
    _canvas->height  = textureSize.height;
    _context         = _canvas->getContext2d();
  }
}

DynamicTexture::~DynamicTexture()
{
}

bool DynamicTexture::canRescale()
{
  return true;
}

void DynamicTexture::scale(float ratio)
{
  auto textureSize = getSize();

  textureSize.width  = static_cast<int>(textureSize.width * ratio);
  textureSize.height = static_cast<int>(textureSize.height * ratio);

  _canvas->width  = textureSize.width;
  _canvas->height = textureSize.height;

  releaseInternalTexture();

  _texture = getScene()->getEngine()->createDynamicTexture(
    textureSize.width, textureSize.height, _generateMipMaps, _samplingMode);
}

ICanvasRenderingContext2D* DynamicTexture::getContext()
{
  return _context;
}

void DynamicTexture::clear()
{
  auto size = getSize();
  _context->fillRect(0, 0, size.width, size.height);
}

void DynamicTexture::update(bool invertY)
{
  getScene()->getEngine()->updateDynamicTexture(_texture, _canvas, invertY,
                                                false, _format);
}

void DynamicTexture::drawText(const std::string& text, int x, int y,
                              const std::string& font, const std::string& color,
                              const std::string& clearColor, bool invertY,
                              bool _update)
{
  auto size = getSize();
  if (!clearColor.empty()) {
    _context->fillStyle = clearColor;
    _context->fillRect(0, 0, size.width, size.height);
  }

  _context->font = font;

  if (x == 0) {
    auto textSize = _context->measureText(text);
    x             = (size.width - textSize.width) / 2;
  }

  if (y == 0) {
    auto _font = font;
    String::replaceInPlace(_font, "/\\D/g", "");
    float fontSize = static_cast<float>(std::stoi(_font));
    y              = static_cast<int>((static_cast<float>(size.height) / 2.f)
                         + (fontSize / 3.65f));
  }

  _context->fillStyle = color;
  _context->fillText(text, x, y);

  if (_update) {
    update(invertY);
  }
}

std::unique_ptr<DynamicTexture> DynamicTexture::clone() const
{
  auto textureSize = getSize();
  DynamicTextureOptions options;
  options.canvas  = _canvas;
  options.width   = textureSize.width;
  options.height  = textureSize.height;
  auto newTexture = std_util::make_unique<DynamicTexture>(
    name, options, getScene(), _generateMipMaps);

  // Base texture
  newTexture->setHasAlpha(hasAlpha());
  newTexture->level = level;

  // Dynamic Texture
  newTexture->wrapU = wrapU;
  newTexture->wrapV = wrapV;

  return newTexture;
}

} // end of namespace BABYLON
