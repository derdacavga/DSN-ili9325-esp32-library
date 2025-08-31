#ifndef _ILI9325SPRITE_H_
#define _ILI9325SPRITE_H_

#include "ILI9325Driver.h" 

class ILI9325Sprite : public Print {

 private:
  ILI9325Driver* _tft;    
  uint16_t*      _buffer; 
  uint16_t       _width; 
  uint16_t       _height;

  int16_t  _cursorX;
  int16_t  _cursorY;
  uint16_t _textColor;
  uint16_t _textBGColor;
  uint8_t  _textSize;

 public:
  ILI9325Sprite(ILI9325Driver* tft); 
  ~ILI9325Sprite();           

  void*  createSprite(int16_t w, int16_t h);
  void   deleteSprite(void);
  void   pushSprite(int16_t x, int16_t y);
  void   pushSprite(int16_t x, int16_t y, uint16_t transparent_color);
  void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
  void pushSprite(int16_t x, int16_t y, ILI9325Sprite* sprite, uint16_t transparent_color);
  void pushImage(int16_t x, int16_t y, int16_t w, int16_t h, const uint16_t* data);
  uint16_t* getPointer(void); 

  uint16_t width(void) const;
  uint16_t height(void) const;

  void fillSprite(uint16_t color);
  void fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
  void drawPixel(int16_t x, int16_t y, uint16_t color);
  uint16_t readPixel(int16_t x, int16_t y); 
  void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
  void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
  void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
  void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);

  void setCursor(int16_t x, int16_t y);
  void setTextColor(uint16_t color);
  void setTextColor(uint16_t fg_color, uint16_t bg_color);
  void setTextSize(uint8_t size);
  void drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size);
  
  virtual size_t write(uint8_t c) override;
};

#endif
