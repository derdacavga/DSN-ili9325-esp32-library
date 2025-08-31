#include <Arduino.h>
#include "ILI9325Driver.h"
#include "soc/soc.h"
#include "soc/gpio_reg.h"
#include "Colors.h"
#include "font5x7.h"

uint16_t ILI9325Driver::color565(uint8_t r, uint8_t g, uint8_t b) {
  return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

#define ILI9325_RAW_WIDTH 240
#define ILI9325_RAW_HEIGHT 320

ILI9325Driver::ILI9325Driver() {
    _rawW = ILI9325_RAW_WIDTH;
    _rawH = ILI9325_RAW_HEIGHT;
    _width = _rawW;
    _height = _rawH;
    _rotation = 0;
    _cursorX = 0;
    _cursorY = 0;
    _textSize = 1;
    _textColor = TFT_WHITE; 
    _textBGColor = TFT_BLACK;
    _textDatum = TL_DATUM;
    _vp_enabled = false;
    _font = font5x7[0];

    int pins[] = { TFT_D0, TFT_D1, TFT_D2, TFT_D3, TFT_D4, TFT_D5, TFT_D6, TFT_D7 };
    memcpy(_dataPins, pins, sizeof(pins));
}

void ILI9325Driver::init(){
    begin();
}

void ILI9325Driver::begin() {
  for (int i = 0; i < 8; i++) pinMode(_dataPins[i], OUTPUT);
  
  pinMode(TFT_RS, OUTPUT);
  pinMode(TFT_WR, OUTPUT);
  pinMode(TFT_CS, OUTPUT);
  pinMode(TFT_RST, OUTPUT);
  pinMode(TFT_RD, OUTPUT);
  
  digitalWrite(TFT_RD, HIGH);
  digitalWrite(TFT_CS, HIGH);
  digitalWrite(TFT_WR, HIGH);
  digitalWrite(TFT_RS, HIGH);
  digitalWrite(TFT_RST, HIGH);

  delay(50); 
  digitalWrite(TFT_RST, LOW);
  delay(100); 
  digitalWrite(TFT_RST, HIGH);
  delay(150); 

    startWrite();

  writeCommand(0x00E5);  writeData(0x78F0);  writeCommand(0x0001);  writeData(0x0100);
  writeCommand(0x0002);  writeData(0x0200);  writeCommand(0x0003);  writeData(0x1030);
  writeCommand(0x0004);  writeData(0x0000);  writeCommand(0x0008);  writeData(0x0207);
  writeCommand(0x0009);  writeData(0x0000);  writeCommand(0x000A);  writeData(0x0000);
  writeCommand(0x000C);  writeData(0x0000);  writeCommand(0x000D);  writeData(0x0000);
  writeCommand(0x000F);  writeData(0x0000);  writeCommand(0x0010);  writeData(0x0000);
  writeCommand(0x0011);  writeData(0x0007);  writeCommand(0x0012);  writeData(0x0000);
  writeCommand(0x0013);  writeData(0x0000);

  endWrite(); 
  delay(50);   
  startWrite(); 

  writeCommand(0x0010);  writeData(0x1690);  writeCommand(0x0011);  writeData(0x0227);
  endWrite();  
  delay(50);   
  startWrite(); 
  writeCommand(0x0012);  writeData(0x000C);
  endWrite();   
  delay(50);   
  startWrite(); 
  writeCommand(0x0013);  writeData(0x1200);  writeCommand(0x0029);  writeData(0x000A);
  writeCommand(0x002B);  writeData(0x000D);
  endWrite();  
  delay(50);  
  startWrite(); 
  writeCommand(0x0020);  writeData(0x0000);  writeCommand(0x0021);  writeData(0x0000);

  writeCommand(0x0030);  writeData(0x0000);  writeCommand(0x0031);  writeData(0x0506);
  writeCommand(0x0032);  writeData(0x0104);  writeCommand(0x0035);  writeData(0x0207);
  writeCommand(0x0036);  writeData(0x000F);  writeCommand(0x0037);  writeData(0x0306);
  writeCommand(0x0038);  writeData(0x0102);  writeCommand(0x0039);  writeData(0x0707);
  writeCommand(0x003C);  writeData(0x0702);  writeCommand(0x003D);  writeData(0x1604);

  writeCommand(0x0050); writeData(0x0000);   writeCommand(0x0051); writeData(_rawW - 1); 
  writeCommand(0x0052); writeData(0x0000);   writeCommand(0x0053); writeData(_rawH - 1); 

  writeCommand(0x0060); writeData(0xA700);   writeCommand(0x0061); writeData(0x0001); 
  writeCommand(0x006A); writeData(0x0000);
  
  writeCommand(0x0080);  writeData(0x0000);  writeCommand(0x0081);  writeData(0x0000);
  writeCommand(0x0082);  writeData(0x0000);  writeCommand(0x0083);  writeData(0x0000);
  writeCommand(0x0084);  writeData(0x0000);  writeCommand(0x0085);  writeData(0x0000);

  writeCommand(0x0090);  writeData(0x0010);  writeCommand(0x0092);  writeData(0x0600);
  
  writeCommand(0x0007);  writeData(0x0133);
  endWrite();
  delay(50);

  fillScreen(TFT_BLACK);
  setRotation(_rotation);  
}

void ILI9325Driver::setRotation(uint8_t rotation) {
    _rotation = rotation % 4;
    switch(_rotation) {
        case 0:
        _width = _rawW; _height = _rawH;
        writeCommand(0x0003);  writeData(0x1030);
            break;
        case 1: 
        _width = _rawH; _height = _rawW; 
        writeCommand(0x0003);  writeData(0x1028);
            break;
        case 2: 
        _width = _rawW; _height = _rawH;
        writeCommand(0x0003);  writeData(0x1000);
            break;
        case 3: 
        _width = _rawH; _height = _rawW;
        writeCommand(0x0003);  writeData(0x1018);
            break;
    }
}

void ILI9325Driver::mapXY(uint16_t &x, uint16_t &y) const {
    uint16_t temp;
    switch (_rotation) {
        case 0: 
            break;
        case 1:
            temp = x;
            x = _rawW - 1 - y;
            y = temp;
            break;
        case 2:
            x = _rawW - 1 - x;
            y = _rawH - 1 - y;
            break;
        case 3:
            temp = x;
            x = y;
            y = _rawH - 1 - temp;
            break;
    }
}

void ILI9325Driver::setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {

    if (_vp_enabled) {
        x0 += _vp_x;
        y0 += _vp_y;
        x1 += _vp_x;
        y1 += _vp_y;
    }

    uint16_t phy_x0 = x0, phy_y0 = y0;
    uint16_t phy_x1 = x1, phy_y1 = y1;

    mapXY(phy_x0, phy_y0);
    mapXY(phy_x1, phy_y1);
     
    if (phy_x0 > phy_x1) _swap_uint16_t(phy_x0, phy_x1);
    if (phy_y0 > phy_y1) _swap_uint16_t(phy_y0, phy_y1);

    writeCommand(0x0050); writeData(phy_x0);  writeCommand(0x0051); writeData(phy_x1); 
    writeCommand(0x0052); writeData(phy_y0);  writeCommand(0x0053); writeData(phy_y1); 

    uint16_t original_mapped_x0 = x0, original_mapped_y0 = y0;
    mapXY(original_mapped_x0, original_mapped_y0);
    
    writeCommand(0x0020); writeData(original_mapped_x0);  writeCommand(0x0021); writeData(original_mapped_y0);
    writeCommand(0x0022); 
}

void ILI9325Driver::setHardwareGRAMAddress(uint16_t x, uint16_t y) { 
    mapXY(x, y); 
    writeCommand(0x0020); writeData(x);   writeCommand(0x0021); writeData(y); 
    writeCommand(0x0022);   
}

inline void ILI9325Driver::pulseWR() {
  REG_WRITE(GPIO_OUT_W1TC_REG, WR_MASK);
  REG_WRITE(GPIO_OUT_W1TS_REG, WR_MASK); 
}

ILI9325Driver::MaskPair ILI9325Driver::masks[256];
bool ILI9325Driver::masks_initialized = false;

void ILI9325Driver::init_masks() {
    if (masks_initialized) return;
    for (int val = 0; val < 256; ++val) {
        uint32_t mask_set = 0;
        uint32_t mask_clr = 0;

        if (val & 0x01) mask_set |= (1UL << TFT_D0); else mask_clr |= (1UL << TFT_D0);
        if (val & 0x02) mask_set |= (1UL << TFT_D1); else mask_clr |= (1UL << TFT_D1);
        if (val & 0x04) mask_set |= (1UL << TFT_D2); else mask_clr |= (1UL << TFT_D2);
        if (val & 0x08) mask_set |= (1UL << TFT_D3); else mask_clr |= (1UL << TFT_D3);
        if (val & 0x10) mask_set |= (1UL << TFT_D4); else mask_clr |= (1UL << TFT_D4);
        if (val & 0x20) mask_set |= (1UL << TFT_D5); else mask_clr |= (1UL << TFT_D5);
        if (val & 0x40) mask_set |= (1UL << TFT_D6); else mask_clr |= (1UL << TFT_D6);
        if (val & 0x80) mask_set |= (1UL << TFT_D7); else mask_clr |= (1UL << TFT_D7);

        masks[val].mask_set = mask_set;
        masks[val].mask_clr = mask_clr;
    }
    masks_initialized = true;
}

void ILI9325Driver::write8(uint8_t val) {
    if (!masks_initialized) {
        init_masks();
    }
    const MaskPair& m = masks[val];
    REG_WRITE(GPIO_OUT_W1TS_REG, m.mask_set);
    REG_WRITE(GPIO_OUT_W1TC_REG, m.mask_clr);
}

void ILI9325Driver::setData(uint8_t data) {
  write8(data);
}

void ILI9325Driver::writeCommand(uint16_t cmd) {
  REG_WRITE(GPIO_OUT_W1TC_REG, RS_MASK);
  write8(cmd >> 8);
  pulseWR();
  write8(cmd & 0xFF);
  pulseWR();
}

void ILI9325Driver::writeData(uint16_t data) {
  REG_WRITE(GPIO_OUT_W1TS_REG, RS_MASK);

  write8(data >> 8); 
  pulseWR();
  write8(data & 0xFF);
  pulseWR();
}

void ILI9325Driver::pushColor(uint16_t color) {
    startWrite();
    writeData(color);
    endWrite();
}

void ILI9325Driver::pushColor(uint16_t color, uint16_t count) {
  startWrite();
  pushBlock(color, count);
  endWrite();
}

void ILI9325Driver::setVerticalScroll(uint16_t y_scroll) {
    y_scroll %= _rawH; 
    writeCommand(0x006A);
    writeData(y_scroll);
}

void ILI9325Driver::fillScreen(uint16_t color) {
    fillRect(0, 0, _width, _height, color);
}

void ILI9325Driver::drawPixel(uint16_t x, uint16_t y, uint16_t color) {
    startWrite();
    drawPixel_nodcs(x, y, color);
    endWrite();
}

void ILI9325Driver::drawFastHLine(uint16_t x, uint16_t y, uint16_t w, uint16_t color) {
  if (y >= _height || x >=_width || w == 0) return;
  if ((x + w - 1) >= _width) w = _width - x;

  startWrite();
  setAddrWindow(x, y, x + w - 1, y);
  REG_WRITE(GPIO_OUT_W1TS_REG, RS_MASK);
  uint8_t hi = color >> 8;
  uint8_t lo = color & 0xFF;
  while (w--) {
    setData(hi); pulseWR();
    setData(lo); pulseWR();
  }
  endWrite();
}

void ILI9325Driver::drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) {
    if (x >= _width || y >= _height || h == 0) return;
    if ((y + h - 1) >= _height) h = _height - y;

    startWrite();
    setAddrWindow(x, y, x, y + h - 1);
    REG_WRITE(GPIO_OUT_W1TS_REG, RS_MASK);
    uint8_t hi = color >> 8;
    uint8_t lo = color & 0xFF;
    while (h--) {
      setData(hi); pulseWR();
      setData(lo); pulseWR();
    }
    endWrite();
}

void ILI9325Driver::fillRect_nodcs(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
  if (x > _width || y > _height) return; 
  if (x + w > _width) w = _width - x;
  if (y + h > _height) h = _height - y;
  
  setAddrWindow(x, y, x + w - 1, y + h - 1);
  pushBlock(color, (uint32_t)w * h); 
}

void ILI9325Driver::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    startWrite();
    fillRect_nodcs(x, y, w, h, color);
    endWrite();
}

void ILI9325Driver::_swap_int16_t(int16_t &a, int16_t &b) { 
    int16_t t = a;
    a = b;
    b = t;
}

void ILI9325Driver::_swap_uint16_t(uint16_t &a, uint16_t &b) {
    uint16_t t = a;
    a = b;
    b = t;
}

void ILI9325Driver::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) {

    int16_t steep = abs(y1 - y0) > abs(x1 - x0);
    if (steep) {
        _swap_int16_t(x0, y0);
        _swap_int16_t(x1, y1);
    }
    if (x0 > x1) {
        _swap_int16_t(x0, x1);
        _swap_int16_t(y0, y1);
    }
    int16_t dx, dy;
    dx = x1 - x0;
    dy = abs(y1 - y0);
    int16_t err = dx / 2;
    int16_t ystep;
    if (y0 < y1) {
        ystep = 1;
    } else {
        ystep = -1;
    }
    startWrite(); 
    for (; x0 <= x1; x0++) {
        if (steep) {
            drawPixel_nodcs(y0, x0, color); 
        } else {
            drawPixel_nodcs(x0, y0, color); 
        }
        err -= dy;
        if (err < 0) {
            y0 += ystep;
            err += dx;
        }
    }
    endWrite();
}

void ILI9325Driver::drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    drawFastHLine(x, y, w, color);
    drawFastHLine(x, y + h - 1, w, color);
    drawFastVLine(x, y, h, color);
    drawFastVLine(x + w - 1, y, h, color);
}

void ILI9325Driver::drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color) {

    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;

    drawPixel(x0, y0 + r, color);
    drawPixel(x0, y0 - r, color);
    drawPixel(x0 + r, y0, color);
    drawPixel(x0 - r, y0, color);

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;
        drawPixel(x0 + x, y0 + y, color);
        drawPixel(x0 - x, y0 + y, color);
        drawPixel(x0 + x, y0 - y, color);
        drawPixel(x0 - x, y0 - y, color);
        drawPixel(x0 + y, y0 + x, color);
        drawPixel(x0 - y, y0 + x, color);
        drawPixel(x0 + y, y0 - x, color);
        drawPixel(x0 - y, y0 - x, color);
    }
}

void ILI9325Driver::drawCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t color) {

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
        if (cornername & 0x4) { 
            drawPixel(x0 + x, y0 - y, color);
            drawPixel(x0 + y, y0 - x, color);
        }
        if (cornername & 0x2) { 
            drawPixel(x0 - x, y0 - y, color);
            drawPixel(x0 - y, y0 - x, color);
        }
        if (cornername & 0x8) {
            drawPixel(x0 - x, y0 + y, color);
            drawPixel(x0 - y, y0 + x, color);
        }
        if (cornername & 0x1) { 
            drawPixel(x0 + x, y0 + y, color);
            drawPixel(x0 + y, y0 + x, color);
        }
    }
     if (cornername & 0x1) drawPixel(x0 + r, y0, color);
     if (cornername & 0x2) drawPixel(x0 - r, y0, color);
     if (cornername & 0x4) drawPixel(x0, y0 - r, color);
     if (cornername & 0x8) drawPixel(x0, y0 + r, color);
}

void ILI9325Driver::fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color) {
    drawFastVLine(x0, y0 - r, 2 * r + 1, color);
    fillCircleHelper(x0, y0, r, 3, 0, color); 
}

void ILI9325Driver::fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta, uint16_t color) {
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

        if (cornername & 0x1) { 
            drawFastVLine(x0 + x, y0 - y, 2 * y + 1 + delta, color);
            drawFastVLine(x0 + y, y0 - x, 2 * x + 1 + delta, color);
        }
        if (cornername & 0x2) { 
            drawFastVLine(x0 - x, y0 - y, 2 * y + 1 + delta, color);
            drawFastVLine(x0 - y, y0 - x, 2 * x + 1 + delta, color);
        }
    }
}

void ILI9325Driver::drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color) {
    drawLine(x0, y0, x1, y1, color);
    drawLine(x1, y1, x2, y2, color);
    drawLine(x2, y2, x0, y0, color);
}

void ILI9325Driver::fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color) {
    int16_t a, b, y, last;

    if (y0 > y1) { _swap_int16_t(y0, y1); _swap_int16_t(x0, x1); }
    if (y1 > y2) { _swap_int16_t(y2, y1); _swap_int16_t(x2, x1); }
    if (y0 > y1) { _swap_int16_t(y0, y1); _swap_int16_t(x0, x1); }

    if (y0 == y2) { 
        a = b = x0;
        if (x1 < a) a = x1;
        else if (x1 > b) b = x1;
        if (x2 < a) a = x2;
        else if (x2 > b) b = x2;
        drawFastHLine(a, y0, b - a + 1, color);
        return;
    }

    int16_t dx01 = x1 - x0, dy01 = y1 - y0,
            dx02 = x2 - x0, dy02 = y2 - y0,
            dx12 = x2 - x1, dy12 = y2 - y1;
    int32_t sa = 0, sb = 0;

    if (y1 == y2) last = y1;  
    else last = y1 - 1; 

    for (y = y0; y <= last; y++) {
        a = x0 + sa / dy01;
        b = x0 + sb / dy02;
        sa += dx01;
        sb += dx02;
        if (a > b) _swap_int16_t(a, b);
        drawFastHLine(a, y, b - a + 1, color);
    }

    sa = dx12 * (y - y1);
    sb = dx02 * (y - y0);
    for (; y <= y2; y++) {
        a = x1 + sa / dy12;
        b = x0 + sb / dy02;
        sa += dx12;
        sb += dx02;
        if (a > b) _swap_int16_t(a, b);
        drawFastHLine(a, y, b - a + 1, color);
    }
}

void ILI9325Driver::drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color) {

    drawFastHLine(x + r    , y    , w - 2 * r, color); 
    drawFastHLine(x + r    , y + h - 1, w - 2 * r, color); 
    drawFastVLine(x    , y + r    , h - 2 * r, color);
    drawFastVLine(x + w - 1, y + r    , h - 2 * r, color); 

    drawCircleHelper(x + r        , y + r        , r, 1, color);
    drawCircleHelper(x + w - r - 1, y + r        , r, 2, color); 
    drawCircleHelper(x + w - r - 1, y + h - r - 1, r, 4, color); 
    drawCircleHelper(x + r        , y + h - r - 1, r, 8, color); 
}

void ILI9325Driver::fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color) {

    fillRect(x + r, y, w - 2 * r, h, color);

    fillCircleHelper(x + w - r - 1, y + r, r, 1, h - 2 * r - 1, color);
    fillCircleHelper(x + r        , y + r, r, 2, h - 2 * r - 1, color);
}

void ILI9325Driver::setTextColor(uint16_t fgColor) {
    _textColor = fgColor;
}

void ILI9325Driver::setTextColor(uint16_t fgColor, uint16_t bgColor) {
  _textColor = fgColor;
  _textBGColor = bgColor;
}

void ILI9325Driver::setTextSize(uint8_t size) {
  if (size < 1) size = 1;
  _textSize = size;
}

void ILI9325Driver::setCursor(int16_t x, int16_t y) {
  _cursorX = x;
  _cursorY = y;
}

int16_t ILI9325Driver::getCursorX() const {
    return _cursorX;
}

int16_t ILI9325Driver::getCursorY() const {
    return _cursorY;
}

void ILI9325Driver::drawChar(int16_t x, int16_t y, char c, uint16_t color, uint16_t bg, uint8_t size) {
  if (c < ' ' || c > '~') { 
      if (bg != color) { 
        fillRect(x, y, 6 * size, 8 * size, bg); 
      }
      return;
  }

  startWrite(); 

  for (int8_t i = 0; i < 5; i++) { 
    uint8_t line = _font[((c - ' ') * 5) + i];
    for (int8_t j = 0; j < 8; j++, line >>= 1) { 
      if (line & 1) { 
        if (size == 1) {
          drawPixel_nodcs(x + i, y + j, color); 
        } else {
          fillRect_nodcs(x + i * size, y + j * size, size, size, color); 
        }
      } else if (bg != color) { 
        if (size == 1) {
          drawPixel_nodcs(x + i, y + j, bg); 
        } else {
          fillRect_nodcs(x + i * size, y + j * size, size, size, bg); 
        }
      }
    }
  }
  
  if (bg != color) { 
      if (size == 1) {
          fillRect_nodcs(x + 5, y, 1, 8, bg);
      } else {
          fillRect_nodcs(x + 5 * size, y, size, 8 * size, bg);
      }
  }

  endWrite(); 
}

void ILI9325Driver::drawChar(int16_t x, int16_t y, char c) {
    drawChar(x, y, c, _textColor, _textBGColor, _textSize);
}

size_t ILI9325Driver::write(uint8_t c) {
    if (c == '\n') {
        _cursorY += _textSize * 8;
        _cursorX = 0;      
    } else if (c == '\r') {
    } else {
        if (_cursorX >= _width - _textSize * 5) { 
            _cursorY += _textSize * 8; 
            _cursorX = 0;     
        }
        drawChar(_cursorX, _cursorY, c, _textColor, _textBGColor, _textSize);
        _cursorX += _textSize * 6; 
    }
    return 1; 
}

size_t ILI9325Driver::write(const char *str) {
    if (str == nullptr) return 0;
    size_t count = 0;
    while (*str) {
        if (write(*str++)) count++;
        else break; 
    }
    return count;
}

size_t ILI9325Driver::write(const uint8_t *buffer, size_t size) {
    size_t count = 0;
    for (size_t i = 0; i < size; i++) {
        if (write(buffer[i])) count++;
        else break;
    }
    return count;
}

void ILI9325Driver::drawText(int16_t x, int16_t y, const char* text, uint16_t color, uint16_t bgcolor, uint8_t size) {
    uint16_t currentX = x;
    uint16_t currentY = y;
    uint16_t oldTextColor = _textColor;
    uint16_t oldTextBGColor = _textBGColor;
    uint8_t oldTextSize = _textSize;
    int16_t oldCursorX = _cursorX;
    int16_t oldCursorY = _cursorY;

    setTextColor(color, bgcolor);
    setTextSize(size);
    setCursor(x, y);

    print(text); 

    setTextColor(oldTextColor, oldTextBGColor);
    setTextSize(oldTextSize);
    setCursor(oldCursorX, oldCursorY);
}

void ILI9325Driver::drawText(int16_t x, int16_t y, const char* text) {
    drawText(x, y, text, _textColor, _textBGColor, _textSize);
}

void ILI9325Driver::drawImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t* data) {

  if ((x >= _width) || (y >= _height) || w == 0 || h == 0) return;
  if ((x + w - 1) >= _width)  w = _width  - x;
  if ((y + h - 1) >= _height) h = _height - y;

  startWrite();
  
  setAddrWindow(x, y, x + w - 1, y + h - 1);

  REG_WRITE(GPIO_OUT_W1TS_REG, RS_MASK);

  uint32_t totalPixels = (uint32_t)w * h;
  for (uint32_t i = 0; i < totalPixels; i++) {
    uint16_t color = data[i];
    
    write8(color >> 8);
    pulseWR();
     
    write8(color & 0xFF);
    pulseWR();
  }
  endWrite();
}

void ILI9325Driver::drawImagePGM(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t* data) {
  if (x >= _width || y >= _height || w == 0 || h == 0) return;
  if (x + w - 1 >= _width) w = _width - x;
  if (y + h - 1 >= _height) h = _height - y;

  startWrite();

  setAddrWindow(x, y, x + w - 1, y + h - 1);

  REG_WRITE(GPIO_OUT_W1TS_REG, RS_MASK); 

  uint32_t totalPixels = (uint32_t)w * h;
  for (uint32_t i = 0; i < totalPixels; i++) {
    uint16_t color = pgm_read_word(&data[i]);
    write8(color >> 8);
    pulseWR();
    write8(color & 0xFF);
    pulseWR();
  }

  endWrite(); 
}

void ILI9325Driver::startWrite(void) {
    REG_WRITE(GPIO_OUT_W1TC_REG, CS_MASK);
}

void ILI9325Driver::endWrite(void) {
    REG_WRITE(GPIO_OUT_W1TS_REG, CS_MASK);
}

void ILI9325Driver::drawPixel_nodcs(uint16_t x, uint16_t y, uint16_t color) {
    if (x >= _width || y >= _height || x < 0 || y < 0) return; 
    setAddrWindow(x, y, x, y);
    writeData(color);
}

void ILI9325Driver::pushBlock(uint16_t color, uint32_t len) {
    uint8_t hi = color >> 8;
    uint8_t lo = color & 0xFF;

    const MaskPair& m_hi = masks[hi];
    const MaskPair& m_lo = masks[lo];

    REG_WRITE(GPIO_OUT_W1TS_REG, RS_MASK);

    while(len--) {
        REG_WRITE(GPIO_OUT_W1TS_REG, m_hi.mask_set);
        REG_WRITE(GPIO_OUT_W1TC_REG, m_hi.mask_clr);
        REG_WRITE(GPIO_OUT_W1TC_REG, WR_MASK);
        REG_WRITE(GPIO_OUT_W1TS_REG, WR_MASK);

        REG_WRITE(GPIO_OUT_W1TS_REG, m_lo.mask_set);
        REG_WRITE(GPIO_OUT_W1TC_REG, m_lo.mask_clr);
        REG_WRITE(GPIO_OUT_W1TC_REG, WR_MASK);
        REG_WRITE(GPIO_OUT_W1TS_REG, WR_MASK);
    }
}

void ILI9325Driver::pushImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t* data) {
  uint32_t len = (uint32_t)w * h;
  
  REG_WRITE(GPIO_OUT_W1TS_REG, RS_MASK);
  
  while (len--) {
    uint16_t color = *data++;
    write8(color >> 8);
    pulseWR();
    write8(color & 0xFF);
    pulseWR();
  }
}

void ILI9325Driver::setTextDatum(uint8_t datum) {
  _textDatum = datum;
}

int16_t ILI9325Driver::drawString(const char *string, int16_t x, int16_t y) {
  int16_t x_start = x;
  int16_t y_start = y;
  uint16_t text_w = 0;
  uint16_t text_h = 0;

  text_w = strlen(string) * 6 * _textSize;
  text_h = 8 * _textSize;

  switch (_textDatum) {
    case TC_DATUM:
      x_start -= text_w / 2;
      break;
    case TR_DATUM:
      x_start -= text_w;
      break;
    case ML_DATUM:
      x_start = x;
      y_start -= text_h / 2;
      break;
    case MC_DATUM:
      x_start -= text_w / 2;
      y_start -= text_h / 2;
      break;
    case MR_DATUM:
      x_start -= text_w;
      y_start -= text_h / 2;
      break;
    case BL_DATUM:
      x_start = x;
      y_start -= text_h;
      break;
    case BC_DATUM:
      x_start -= text_w / 2;
      y_start -= text_h;
      break;
    case BR_DATUM:
      x_start -= text_w;
      y_start -= text_h;
      break;
    case TL_DATUM:
    default:
      break;
  }

  drawText(x_start, y_start, string, _textColor, _textBGColor, _textSize);

  return text_w; 
}

int16_t ILI9325Driver::drawString(const String &string, int16_t x, int16_t y) {
  return drawString(string.c_str(), x, y);
}

TFT_Button::TFT_Button(void) {
  _tft = nullptr;
  _is_pressed = false;
  _just_pressed = false;
  _just_released = false;
}

void TFT_Button::initButton(ILI9325Driver *gfx, int16_t x, int16_t y, uint16_t w, uint16_t h,
                            uint16_t outline, uint16_t fill, uint16_t text,
                            const char *label, uint8_t textsize) {
  _tft = gfx;
  _x = x; _y = y; _w = w; _h = h;
  _outlinecolor = outline; _fillcolor = fill; _textcolor = text;
  _textsize = textsize;
  strncpy(_label, label, 19);
  _label[19] = '\0';
}

void TFT_Button::drawButton(bool inverted) {
  uint16_t fill, outline, text;
  if (!inverted) { fill = _fillcolor; outline = _outlinecolor; text = _textcolor; }
  else { fill = _textcolor; outline = _outlinecolor; text = _fillcolor; }

  _tft->fillRoundRect(_x - (_w / 2), _y - (_h / 2), _w, _h, 5, fill);
  _tft->drawRoundRect(_x - (_w / 2), _y - (_h / 2), _w, _h, 5, outline);
  _tft->setTextColor(text);
  _tft->setTextDatum(MC_DATUM);
  _tft->setTextSize(_textsize);
  _tft->drawString(_label, _x, _y);
}

bool TFT_Button::isPressed(void) {
  uint16_t touch_x, touch_y;
  return false;
}

bool TFT_Button::justPressed(void) {
    bool now_pressed = isPressed();
    _just_pressed = now_pressed && !_is_pressed;
    _is_pressed = now_pressed;
    return _just_pressed;
}

bool TFT_Button::justReleased(void) {
    bool now_pressed = isPressed();
    _just_released = !now_pressed && _is_pressed;
    _is_pressed = now_pressed;
    return _just_released;
}

void ILI9325Driver::setViewport(int32_t x, int32_t y, int32_t w, int32_t h) {
  _vp_x = x;
  _vp_y = y;
  _vp_w = w;
  _vp_h = h;
  _vp_enabled = true;
  
  _width = w;
  _height = h;
}

void ILI9325Driver::resetViewport(void) {
  _vp_enabled = false;
  
  setRotation(getRotation()); 
}

bool ILI9325Driver::isViewport(void) {
  return _vp_enabled;
}

int16_t ILI9325Driver::textWidth(const char *string) {
  if (string == nullptr) return 0;
  return strlen(string) * 6 * _textSize;
}

int16_t ILI9325Driver::textWidth(const String &string) {
  return textWidth(string.c_str());
}

void ILI9325Driver::setTextFont(uint8_t font_number) {
  switch (font_number) {
    case 1: 
      _font = font5x7[0];
      break;
    // case 2:
    //   #ifdef FONT2_AVAILABLE
    //     _font = font2[0];
    //   #endif
    //   break;
    // case 4:
    //   #ifdef FONT4_AVAILABLE
    //     _font = font4[0];
    //   #endif
    //   break;
    default:
      _font = font5x7[0]; 
      break;
  }
}

bool TFT_Button::contains(int16_t x, int16_t y) {
  return (x > (_x - _w / 2) && x < (_x + _w / 2) &&
          y > (_y - _h / 2) && y < (_y + _h / 2));
}

void ILI9325Driver::drawCrosshair(int16_t x, int16_t y, uint16_t color) {
  drawFastHLine(0, y, width(), color);
  drawFastVLine(x, 0, height(), color);
}
