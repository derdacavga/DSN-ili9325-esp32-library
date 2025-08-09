#include "TFT_Sprite.h"

TFT_Sprite::TFT_Sprite(ILI9325Driver* tft) {
  _tft = tft;   
  _buffer = nullptr; 
  _width = 0;
  _height = 0;
}

TFT_Sprite::~TFT_Sprite() {
  deleteSprite();
}

void TFT_Sprite::fillSprite(uint16_t color) {
  if (!_buffer) return;

  uint32_t numPixels = (uint32_t)_width * _height;
  for (uint32_t i = 0; i < numPixels; i++) {
    _buffer[i] = color;
  }
}

void* TFT_Sprite::createSprite(int16_t w, int16_t h) {
  if (_buffer) {
    deleteSprite();
  }

  if (w <= 0 || h <= 0) {
    return nullptr;
  }

  _buffer = (uint16_t *)malloc(w * h * sizeof(uint16_t));

  if (!_buffer) {
    _width = 0;
    _height = 0;
    return nullptr;
  }
  
  _width = w;
  _height = h;
  
  return _buffer;
}

void TFT_Sprite::deleteSprite(void) {
  if (_buffer) {
    free(_buffer);   
    _buffer = nullptr; 
    _width = 0;
    _height = 0;
  }
}

void TFT_Sprite::pushSprite(int16_t x, int16_t y) {
  if (!_buffer) {
    return;
  }
  _tft->drawImage(x, y, _width, _height, _buffer);
}
