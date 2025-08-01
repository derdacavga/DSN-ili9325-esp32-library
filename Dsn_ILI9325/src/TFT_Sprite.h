#ifndef _TFT_SPRITE_H_
#define _TFT_SPRITE_H_

#include "ILI9325Driver.h" 

class TFT_Sprite {

 private:
  ILI9325Driver* _tft;    
  uint16_t*      _buffer; 
  uint16_t       _width; 
  uint16_t       _height; 

 public:
  TFT_Sprite(ILI9325Driver* tft); 
  ~TFT_Sprite();           

  void*  createSprite(int16_t w, int16_t h);
  void   deleteSprite(void);
  void   pushSprite(int16_t x, int16_t y);
  void fillSprite(uint16_t color);

};

#endif 