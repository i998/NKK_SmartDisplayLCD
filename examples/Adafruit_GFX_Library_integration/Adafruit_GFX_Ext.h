/*********************************************************************
This file is a part of a library for NKK LCD 64x32 SmartDisplay

Copyright (c) 2021, IFH
All rights reserved.

GNU General Public License,  check license.txt for more information
All text above must be included in any redistribution
*********************************************************************/
#ifndef _Adafruit_GFX_Ext_H_
#define _Adafruit_GFX_Ext_H_

#include "src\Adafruit-GFX-Library\Adafruit_GFX.h"
#include <NKKSmartDisplayLCD.h>

/**************************************************************************/
/*! 
    @brief  Class that extends Adafruit_GFX and allows operations with NKK_SmartDisplayLCD object.
*/
/**************************************************************************/
class Adafruit_GFX_Ext : public Adafruit_GFX {

public:
Adafruit_GFX_Ext(int16_t w, int16_t h, NKK_SmartDisplayLCD *NKK_A);   
~Adafruit_GFX_Ext(void);
  //Draw a pixel to the imageBufferGFX[] of the NKK_SmartDisplayLCD object
  void drawPixel(int16_t x, int16_t y, uint16_t color);
  //Upload an image to the NKK device from imageBufferGFX[], set background colour and brightness
  void display();
	
private:
NKK_SmartDisplayLCD *_NKK; //pointer to the NKK_SmartDisplayLCD object object to communicate with the NKK device
};
#endif // _Adafruit_GFX_Ext_H_
