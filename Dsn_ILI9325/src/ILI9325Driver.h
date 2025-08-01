#ifndef ILI9325DRIVER_H
#define ILI9325DRIVER_H

#include <Arduino.h>
#include <Print.h>
#include "Colors.h" 

#define TFT_D0 12
#define TFT_D1 13
#define TFT_D2 26
#define TFT_D3 25
#define TFT_D4 17
#define TFT_D5 16
#define TFT_D6 27
#define TFT_D7 14

#define TFT_RS 15
#define TFT_WR 4
#define TFT_CS 33
#define TFT_RST 32
#define TFT_RD 2

#define ILI9325_ENTRY_MODE    0x0003
#define EM_AM   (1<<6)  
#define EM_ID1  (1<<5) 
#define EM_ID0  (1<<4)
// #define EM_DFM  (1<<2)  // Data Format for Frame Memory
// #define EM_TRI  (1<<1)  // Interface for Triangle Area Filling
#define ILI9325_GATE_SCAN_CTRL 0x0060

class ILI9325Driver : public Print {

private:
  uint16_t _textColor;
  uint16_t _textBGColor;
  uint8_t  _textSize;
  int16_t  _cursorX; 
  int16_t  _cursorY; 
  struct MaskPair {
     uint32_t mask_set;
     uint32_t mask_clr;
  };
  static MaskPair masks[256];
  static bool masks_initialized;

  inline void pulseWR();
  void writeCommand(uint16_t cmd);
  void writeData(uint16_t data);
  void setData(uint8_t data);
  void writeNcolors(uint16_t color, uint32_t len);

  void _swap_int16_t(int16_t &a, int16_t &b);
  void _swap_uint16_t(uint16_t &a, uint16_t &b);
  void fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta, uint16_t color);
  void drawCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t color);
  
public:
  ILI9325Driver();
  
  uint16_t _width;
  uint16_t _height;
  uint8_t  _rotation;
  uint16_t _rawW;
  uint16_t _rawH;

  void begin();
  void setRotation(uint8_t rotation);
  uint16_t width() const { return _width; }
  uint16_t height() const { return _height; }
  uint8_t getRotation() const { return _rotation; }
  
  static void init_masks();
  void write8(uint8_t val);

  void mapXY(uint16_t &x, uint16_t &y) const;
  void setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);

  void setHardwareGRAMAddress(uint16_t x, uint16_t y);

  void pushColor(uint16_t color);
  void pushColor(uint16_t color, uint16_t count);

  void fillScreen(uint16_t color);

  void drawPixel(uint16_t x, uint16_t y, uint16_t color);

  void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
  void drawFastHLine(uint16_t x, uint16_t y, uint16_t w, uint16_t color);
  void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);

  void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
  void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
  void drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color);
  void fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color);

  void drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
  void fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);

  void drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2,  uint16_t color);
  void fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);

  void setTextColor(uint16_t fgColor);
  void setTextColor(uint16_t fgColor, uint16_t bgColor);

  void setTextSize(uint8_t size);
  uint8_t getTextSize() const { return _textSize; }
  void setCursor(int16_t x, int16_t y);

  int16_t getCursorX() const;
  int16_t getCursorY() const;

  virtual size_t write(uint8_t c) override;
  virtual size_t write(const char *str);
  virtual size_t write(const uint8_t *buffer, size_t size);

  void drawChar(int16_t x, int16_t y, char c);
  void drawChar(int16_t x, int16_t y, char c, uint16_t color, uint16_t bgcolor, uint8_t size);

  void drawText(int16_t x, int16_t y, const char* text);
  void drawText(int16_t x, int16_t y, const char* text, uint16_t color, uint16_t bgcolor, uint8_t size);

  void drawImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t* data);
  void drawImagePGM(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t* data);
};

#endif