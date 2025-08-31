#include "ILI9325Sprite.h"
#include "font5x7.h" 
#include <utility> 

ILI9325Sprite::ILI9325Sprite(ILI9325Driver* tft) {
  _tft = tft;   
  _buffer = nullptr; 
  _width = 0;
  _height = 0;

  _cursorX = 0;
  _cursorY = 0;
  _textSize = 1;
  _textColor = TFT_WHITE;
  _textBGColor = TFT_BLACK;
}

ILI9325Sprite::~ILI9325Sprite() {
  deleteSprite();
}

void* ILI9325Sprite::createSprite(int16_t w, int16_t h) {
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

void ILI9325Sprite::deleteSprite(void) {
  if (_buffer) {
    free(_buffer);   
    _buffer = nullptr; 
    _width = 0;
    _height = 0;
  }
}

void ILI9325Sprite::pushSprite(int16_t x, int16_t y) {
  if (!_buffer) {
    return;
  }
  _tft->drawImage(x, y, _width, _height, _buffer);
}

void ILI9325Sprite::fillSprite(uint16_t color) {
  if (!_buffer) return;
  uint32_t numPixels = (uint32_t)_width * _height;
  for (uint32_t i = 0; i < numPixels; i++) {
    _buffer[i] = color;
  }
}

void ILI9325Sprite::pushSprite(int16_t x, int16_t y, uint16_t transparent_color) {
    if (!_buffer) return;

    for (int j = 0; j < _height; j++) {
        int i = 0;
        while (i < _width) {
            
            while (i < _width && _buffer[i + j * _width] == transparent_color) {
                i++;
            }
            
            if (i >= _width) break;

            int block_start_x = i;
            
            while (i < _width && _buffer[i + j * _width] != transparent_color) {
                i++;
            }
            int block_end_x = i;

            uint16_t block_width = block_end_x - block_start_x;
            uint16_t* block_data_pointer = _buffer + (j * _width + block_start_x);

            _tft->drawImage(
                x + block_start_x, 
                y + j,          
                block_width,     
                1,               
                block_data_pointer 
            );
        }
    }
}

void ILI9325Sprite::drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    drawFastHLine(x, y, w, color);
    drawFastHLine(x, y + h - 1, w, color);
    drawFastVLine(x, y, h, color);
    drawFastVLine(x + w - 1, y, h, color);
}

void ILI9325Sprite::pushSprite(int16_t x, int16_t y, ILI9325Sprite* sprite, uint16_t transparent_color) {
    if (!sprite || !sprite->getPointer()) return;

    for(int j=0; j < sprite->height(); j++){
        for(int i=0; i < sprite->width(); i++){
            uint16_t color = sprite->readPixel(i, j);
            if (color != transparent_color) {
                drawPixel(x + i, y + j, color);
            }
        }
    }
}

void ILI9325Sprite::pushImage(int16_t x, int16_t y, int16_t w, int16_t h, const uint16_t* data) {
    for(int j=0; j < h; j++){
        for(int i=0; i < w; i++){
            drawPixel(x + i, y + j, pgm_read_word(&data[i + j * w]));
        }
    }
}

void ILI9325Sprite::fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color) {
    drawFastVLine(x0, y0 - r, 2 * r + 1, color);
    int16_t f     = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x     = 0;
    int16_t y     = r;

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f     += ddF_y;
        }
        x++;
        ddF_x += 2;
        f     += ddF_x;

        drawFastVLine(x0 + x, y0 - y, 2 * y + 1, color);
        drawFastVLine(x0 + y, y0 - x, 2 * x + 1, color);
        drawFastVLine(x0 - x, y0 - y, 2 * y + 1, color);
        drawFastVLine(x0 - y, y0 - x, 2 * x + 1, color);
    }
}

uint16_t* ILI9325Sprite::getPointer(void) {
    return _buffer;
}

uint16_t ILI9325Sprite::width(void) const { return _width; }
uint16_t ILI9325Sprite::height(void) const { return _height; }

void ILI9325Sprite::drawPixel(int16_t x, int16_t y, uint16_t color) {
    if (x < 0 || x >= _width || y < 0 || y >= _height || !_buffer) return;
    
    _buffer[x + y * _width] = color;
}

uint16_t ILI9325Sprite::readPixel(int16_t x, int16_t y) {
    if (x < 0 || x >= _width || y < 0 || y >= _height || !_buffer) return 0;
    return _buffer[x + y * _width];
}

void ILI9325Sprite::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    if (!_buffer) return;
    if (x < 0) { w += x; x = 0; }
    if (y < 0) { h += y; y = 0; }
    if (x + w > _width)  w = _width  - x;
    if (y + h > _height) h = _height - y;
    if (w <= 0 || h <= 0) return;

    for (int j = y; j < y + h; j++) {
        for (int i = x; i < x + w; i++) {
            _buffer[i + j * _width] = color;
        }
    }
}

void ILI9325Sprite::drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) {
    fillRect(x, y, 1, h, color);
}

void ILI9325Sprite::drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {
    fillRect(x, y, w, 1, color);
}

void ILI9325Sprite::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) {
    int16_t steep = abs(y1 - y0) > abs(x1 - x0);
    if (steep) { std::swap(x0, y0); std::swap(x1, y1); }
    if (x0 > x1) { std::swap(x0, x1); std::swap(y0, y1); }
    int16_t dx = x1 - x0, dy = abs(y1 - y0);
    int16_t err = dx / 2, ystep = (y0 < y1) ? 1 : -1;
    for (; x0 <= x1; x0++) {
        if (steep) drawPixel(y0, x0, color);
        else drawPixel(x0, y0, color);
        err -= dy;
        if (err < 0) { y0 += ystep; err += dx; }
    }
}

void ILI9325Sprite::setTextColor(uint16_t color) {
    _textColor = color;
}

void ILI9325Sprite::setTextColor(uint16_t fg_color, uint16_t bg_color) {
    _textColor = fg_color;
    _textBGColor = bg_color;
}

void ILI9325Sprite::setTextSize(uint8_t size) {
    _textSize = (size > 0) ? size : 1;
}

void ILI9325Sprite::setCursor(int16_t x, int16_t y) {
    _cursorX = x;
    _cursorY = y;
}

size_t ILI9325Sprite::write(uint8_t c) {
    if (c == '\n') {
        _cursorY += _textSize * 8;
        _cursorX = 0;
    } else if (c == '\r') {
    } else {
        drawChar(_cursorX, _cursorY, c, _textColor, _textBGColor, _textSize);
        _cursorX += _textSize * 6;
    }
    return 1;
}

void ILI9325Sprite::drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size) {
    if ((c < ' ') || (c > '~')) return;

    for (int8_t i = 0; i < 5; i++) {
        uint8_t line = font5x7[c - ' '][i];
        for (int8_t j = 0; j < 8; j++, line >>= 1) {
            if (line & 1) {
                if (size == 1) drawPixel(x + i, y + j, color);
                else fillRect(x + i * size, y + j * size, size, size, color);
            } else if (bg != color) {
                if (size == 1) drawPixel(x + i, y + j, bg);
                else fillRect(x + i * size, y + j * size, size, size, bg);
            }
        }
    }
}
