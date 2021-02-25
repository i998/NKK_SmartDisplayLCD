/*********************************************************************
This is a library for NKK LCD 64x32 SmartDisplay
  https://www.nkkswitches.com/smartdisplay/

Copyright (c) 2021, IFH
All rights reserved.

GNU General Public License,  check license.txt for more information
All text above must be included in any redistribution
*********************************************************************/
/*********************************************************************
2021-02-25
Version: v01 , initial version
Status: Works OK 

Key Features:
- Requres an native SPI object (like Arduino one) which handles SPI communications 
- Supports two image formats:
     1)Normal GFX  (first pixel is top left corner, represented by bit0 in the first byte)
     2)Native NKK  (first pixel is top right corner, represented by bit7 in the first byte, as per NKK specs)
- Supports Landscape (64*32) and Portrait (32*64) display configurations 	 
- Configured by default for LCD 64x32 SmartDisplay but settings could be easily changed 
  for other resolutions
- Supports two formats for background colour - three bytes RGB and Native NKK (RRGGBBxx)
- Supports a rotation of an image by 180 degrees to accommodate different possible footprints
  of an NKK device on your pcb.
- Generates SPI Slave Select(Chip Select) signal to allow management of multiple NKK devices 
- Can be used with Adafruit GFX Graphics core which does all the circles, text and fonts, 
  rectangles, etc. You can get it from https://github.com/adafruit/Adafruit-GFX-Library. 
  See /examples folder for implementation examples. 
- Tested with hardware SPI for Arduino Pro Mini board   
- Tested with NKK IS15EBFP4RGB-09YN LCD 64 x 32 Pushbutton, shall work with these models as well:
    S15DBFP4RGB
    IS15DBFP4RGB-09YN
    S15DSBFP4RGB
    IS01DBFRGB
    IS15EBFP4RGB
    IS15ESBFP4RGB
    IS01EBFRGB
*********************************************************************/
#ifndef _NKK_SmartDisplayLCD_H_
#define _NKK_SmartDisplayLCD_H_


#include <SPI.h> 
 
 /**************************************************************************/
/*! 
    @brief  Class that stores state and functions for interacting with NKK SmartDisplay LCD device.
*/
/**************************************************************************/
class NKK_SmartDisplayLCD { 
  
#define NKK_SmartDisplayLCD_Img_Upload 0x55  /** int 85**/
#define NKK_SmartDisplayLCD_Set_RGB 0x40  /**int 64 **/
#define NKK_SmartDisplayLCD_Set_Bright 0x41  /**int 65**/
#define NKK_SmartDisplayLCD_Reset 0x5e  /**int 94**/
#define NKK_SmartDisplayLCD_Reset_data 0x03  /**int 3**/
  
public:
NKK_SmartDisplayLCD(          uint8_t w=64,
                               uint8_t h=32,
                               uint8_t isRotate180=0,   //0- no rotation* 1 - 180 degree rotation  
							   uint8_t cspin = SS,
                               uint32_t freqSPI=1000000, // in Hz									  
  							   SPIClass *SPI_A=&SPI);								   
								   

~NKK_SmartDisplayLCD(void);								   

  
  
//Setups the SPI interface and hardware
void begin(void);

//Current image
// Note  - Arduino compiler (C++ v.11) require the size of the array declared as we use independent arrays per a class instance. 
// NKK Smart Display is 64bit*32bit so array size is 256 bytes. Cannot use undefined length like imageBufferGFX[] and initialise 
// with values forvided. The library code supports array size up to 65535. 
//current image (GFX format)
byte imageBufferGFX[256] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; 
 // current image (NKK native format)
byte imageBufferNKK[256] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; 
  
//Colour and Brightness  settings for NKK device, in NKK format as per NKK specs
byte bkgColour=255;  //background colour, WHITE
byte bkgBrightnes= 31;  //background colour brightness, 0FF 
 
//Get current NKK device and Image Buffer settings
  uint8_t getWidth(void);
  uint8_t getHeigth(void); 
  uint16_t getImageBufferLength(void);

//NKK commands   
  //Set background colour 
  void setColourNKK(byte data);  // set as per NKK specs 
  void setColourRGB(byte R, byte G, byte B);  // RGB get converted to the closest colour as per NKK specs (64 colours available)
  //Set background colour brightness level
  void setBrightness(byte data); //set as per NKK specs 
  //Reset NKK device 
  void reset(void);
  //Upload an image to the NKK device from imageBufferGFX[], set background colour and brightness
  void display(void); // display the GFX format 
  //Upload an image to the NKK device from imageBufferNKK[], set background colour and brightness
  void display_NKK(void);  // display the native NKK format
   
 
//Image Buffer commands
// Note - these commands need a separate call to  display() methods to make the results visible in the display device.
  //Draw a pixel in the imageBufferGFX[]
  void drawPixel( uint8_t x, uint8_t y, uint8_t color);
  //Clear imageBufferGFX[]
  void clearImageBufferGFX(void);
  //Clear imageBufferNKK[]
  void clearImageBufferNKK(void);
  //Invert imageBufferGFX[]
  void invertImageBufferGFX(void);
  //Invert imageBufferNKK[]
  void invertImageBufferNKK(void);   
  //Convert current image buffers from GFX format to NKK native format and vice versa 
  void convertGFX2NKK(void);
   
   
private:
SPIClass *_SPI;
SPISettings *_spiSetting;
uint32_t _freqSPI=1000000;	

uint8_t _w=64; //Max w is  256 and  Max w*h/8  = 65535 for this library code.
uint8_t _h=32; //Max h is  256 and  Max w*h/8  = 65535 for this library code.
uint16_t _imageBufferLength =256; // in bytes, _w*_h/8 , 65535 max
uint8_t _isRotate180 = 0; // no rotation 
uint8_t _cs = SS; // SPI Slave Select(Chip Select) pin 

 
//Image Buffer commands and helpers
   void convertGFX2NKK(byte imageBufferGFX[], byte imageBufferNKK[]); 
   void rotate180_NKK(byte imageBufferNKK[]); 
   byte reverseByte(byte b);
   
//SPI operations & Slave Select(Chip Select) pin handling per NKK_SmartDisplayLCD instance (thus allows management of multiple NKK devices)
   void sendArrayToSPI(byte buffer[], uint16_t length);
   void sendImageToSPI(byte buffer[], uint16_t length);
   void sendCommandAndDataToSPI(byte command, byte data);
   void beginTransaction(void);
   void endTransaction(void);
};  
#endif // _NKK_SmartDisplayLCD_H_