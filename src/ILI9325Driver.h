#ifndef ILI9325DRIVER_H
#define ILI9325DRIVER_H

#include <Arduino.h>
#include <Print.h>
#include <SPIFFS.h>
#include "Colors.h" 

#define TFT_D0 22
#define TFT_D1 18
#define TFT_D2 26
#define TFT_D3 25
#define TFT_D4 17
#define TFT_D5 16
#define TFT_D6 27
#define TFT_D7 5

#define TFT_RS 19
#define TFT_WR 4
#define TFT_CS 21
#define TFT_RST 14
#define TFT_RD 2

#define CS_MASK (1UL << TFT_CS)
#define RS_MASK (1UL << TFT_RS)
#define WR_MASK (1UL << TFT_WR)

#define ILI9325_ENTRY_MODE    0x0003
#define EM_AM   (1<<6)  
#define EM_ID1  (1<<5) 
#define EM_ID0  (1<<4)
#define ILI9325_GATE_SCAN_CTRL 0x0060

#define TL_DATUM 0
#define TC_DATUM 1 
#define TR_DATUM 2 
#define ML_DATUM 3
#define MC_DATUM 4 
#define MR_DATUM 5 
#define BL_DATUM 6 
#define BC_DATUM 7 
#define BR_DATUM 8 

class ILI9325Driver;

class TFT_Button {
 public:
  TFT_Button(void);
  void initButton(ILI9325Driver *gfx, int16_t x, int16_t y, uint16_t w, uint16_t h,
                  uint16_t outline, uint16_t fill, uint16_t text,
                  const char *label, uint8_t textsize);

  void drawButton(bool inverted = false);
  bool contains(int16_t x, int16_t y);
  bool isPressed(void);
  bool justPressed(void);
  bool justReleased(void);

 private:
  ILI9325Driver* _tft;
  int16_t  _x, _y;
  uint16_t _w, _h;
  uint16_t _outlinecolor, _fillcolor, _textcolor;
  char     _label[20];
  uint8_t  _textsize;
  bool     _is_pressed;
  bool     _just_pressed;
  bool     _just_released;
};

class ILI9325Driver : public Print {

private:
  uint16_t _width;
  uint16_t _height;
  uint8_t  _rotation;
  uint16_t _rawW;
  uint16_t _rawH;
  uint16_t _textColor;
  uint16_t _textBGColor;
  uint8_t  _textSize;
  int16_t  _cursorX; 
  int16_t  _cursorY; 
  uint8_t  _textDatum;

  int _dataPins[8];

  const uint8_t *_font;

  int32_t  _vp_x;    
  int32_t  _vp_y;      
  int32_t  _vp_w;      
  int32_t  _vp_h;      
  bool     _vp_enabled;

  void drawCrosshair(int16_t x, int16_t y, uint16_t color);

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

  void fillRect_nodcs(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
  void drawPixel_nodcs(uint16_t x, uint16_t y, uint16_t color);

public:
  ILI9325Driver();

  uint16_t color565(uint8_t r, uint8_t g, uint8_t b);

  void begin();
  void init();
  void setRotation(uint8_t rotation);
  void setVerticalScroll(uint16_t y_scroll);
  uint16_t width() const { return _width; }
  uint16_t height() const { return _height; }
  uint8_t getRotation() const { return _rotation; }
  
  void startWrite(void);
  void endWrite(void);

  void pushBlock(uint16_t color, uint32_t len);

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
  void setTextDatum(uint8_t datum);
  void setTextFont(uint8_t font_number);
  int16_t drawString(const char *string, int16_t x, int16_t y);
  int16_t drawString(const String &string, int16_t x, int16_t y);
  int16_t textWidth(const char *string);
  int16_t textWidth(const String &string);
  int16_t getCursorX() const;
  int16_t getCursorY() const;

  virtual size_t write(uint8_t c) override;
  virtual size_t write(const char *str);
  virtual size_t write(const uint8_t *buffer, size_t size);

  void drawChar(int16_t x, int16_t y, char c);
  void drawChar(int16_t x, int16_t y, char c, uint16_t color, uint16_t bgcolor, uint8_t size);

  void drawText(int16_t x, int16_t y, const char* text);
  void drawText(int16_t x, int16_t y, const char* text, uint16_t color, uint16_t bgcolor, uint8_t size);

  void pushImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t* data);

  void drawImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t* data);
  void drawImagePGM(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t* data);

  void setViewport(int32_t x, int32_t y, int32_t w, int32_t h);
  void resetViewport(void);
  bool isViewport(void);

};

#endif
