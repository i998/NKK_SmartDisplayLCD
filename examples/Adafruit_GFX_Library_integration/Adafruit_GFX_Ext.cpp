/*********************************************************************
This file is a part of a library for NKK LCD 64x32 SmartDisplay

Copyright (c) 2021, IFH
All rights reserved.

GNU General Public License,  check license.txt for more information
All text above must be included in any redistribution
*********************************************************************/

  #include "Adafruit_GFX_Ext.h"
/**************************************************************************/
/*!
    @brief  Constructor for Adafruit_GFX_Ext object which allows Adafruit_GFX library functions for NKK LCD 64x32 SmartDisplay device.
    @param  w The NKK LCD screen width in pixels.  
	  @param  h The NKK LCD screen height in pixels.  
	  @param  A reference (pointer) to NKK_SmartDisplayLCD object. 
	  @return Adafruit_GFX_Ext object.
    @note   Extension of  Adafruit_GFX class.
*/
/**************************************************************************/
  Adafruit_GFX_Ext::Adafruit_GFX_Ext(int16_t w, int16_t h, NKK_SmartDisplayLCD *NKK_A): Adafruit_GFX(w, h)
    {
	   _NKK = NKK_A; //pointer to the NKK_SmartDisplayLCD object object to communicate with the NKK device
    }
	
/**************************************************************************/
/*!
    @brief  Destructor for Adafruit_GFX_Ext object.
*/
/**************************************************************************/	
Adafruit_GFX_Ext::~Adafruit_GFX_Ext(void) {
}

/**************************************************************************/
/*!
    @brief  Draw a pixel to the imageBufferGFX[] of the NKK_SmartDisplayLCD object
    @param  x   x coordinate, starts with 0
    @param  y   y coordinate, starts with 0
    @param  color 
	  @note   NKK LCD 64x32 SmartDisplay is monochrome, NKK_SmartDisplayLCD object will handle color value 
*/
/**************************************************************************/
void Adafruit_GFX_Ext::drawPixel( int16_t x, int16_t y, uint16_t color)
    {
      _NKK->drawPixel((uint8_t) x, (uint8_t) y, (uint8_t) color);
    }

/**************************************************************************/
/*! 
    @brief  Displays a picture in GFX format i.e. converts imageBufferGFX to NKK format, uploads the NKK device, sets colour and brightness as per the NKK_SmartDisplayLCD object variables.     
*/
/**************************************************************************/ 
void Adafruit_GFX_Ext::display()
    {
	   _NKK->display();
    }
