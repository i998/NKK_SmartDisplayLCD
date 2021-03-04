
## Overview:
This is a library for **NKK LCD 64x32 SmartDisplay** (https://www.nkkswitches.com/smartdisplay/) 
It has been designed and tested for NKK IS15EBFP4RGB-09YN LCD 64 x 32 Pushbutton and shall work with these models as well:

    S15DBFP4RGB
    IS15DBFP4RGB-09YN
    S15DSBFP4RGB
    IS01DBFRGB
    IS15EBFP4RGB
    IS15ESBFP4RGB
	IS15EBFP4RGB-09YN
    IS01EBFRGB

With minimal changes in configuration settings it shall support other LCD resolutions if required.  
    
The library supports: 
 1. Two image formats:    
    - Normal GFX  (first pixel is top left corner, represented by bit0 in the first byte)
    - Native NKK  (first pixel is top right corner, represented by bit7 in the first byte, as per NKK specs)
2. Landscape (64x32) and Portrait (32x64) display configurations 	 
3. Two formats for background colour - three bytes RGB format and native NKK format (RRGGBBxx)
4. A rotation of an image by 180 degrees to accommodate different possible footprints of an NKK device on your pcb.	
	
## Library usage:
 1. Create a NKK_SmartDisplayLCD object.  At this step specify: 
    - w: The NKK LCD screen width in pixels.  Maximum w is  256 and  Maximum w x h/8  = 65535 for this library code. 
    - h: The NKK LCD screen height in pixels  Maximum h is  256 and  Maximum w x h/8  = 65535 for this library code.  
    - isRotate180:  A flag to indicate that the picture need to be rotated by 180 degrees to accommodate different possible footprints of an NKK device on your pcb. Applied just before the picture is uploaded to the NKK device by the *display()* or *display_NKK()* functions. 
    - cspin: Slave Select(Chip Select) signal  (to allow use more than one NKK device with their own SS signals).
    - freqSPI: SPI frequency, Hz. 
    - pointer: A reference (pointer) to native SPI object which handles SPI communications. 
 
 2. Execute *begin()* method. That would start SPI interface and reset the NKK device.

 3. Set required background colour and brightness:    
    - By using library methods that would communicate directly to the NKK device:
        ```C++
       setColourNKK(byte data);  // set as per NKK specs (RRGGBBxx)
       setColourRGB(byte R, byte G, byte B);  // RGB get converted to the closest colour as per NKK specs (64 colours available)
       setBrightness(byte data); //set as per NKK specs  (BBBxxxxx)
       ```	   
    - By setting library variables *bkgColour* and *bkgBrightnes*  which will be used when the *display()* and *display_NKK()* methods are called 	 

 4. Set an image in a GFX or NKK format to library variables *imageBufferGFX[]* and *imageBufferNKK[]*.  No need to set both.
  You can use an  NKK Bitmap bilder (MS Excel file) in the */documentation* folder to build an image in GFX or NKK formats, landscape or portrait.
 
 5. Use *drawPixel(x,y,color)* to set a pixel in the *imageBufferGFX[]*.   X and Y are pixel coordunates, starting from 0. For this monochrome 
  LCD display *color* can be any value, it will be converted either 0 or 1 in the library. This method is also used for integration with Adafruit_GFX library (https://github.com/adafruit/Adafruit-GFX-Library).
  
  6. Execute *display()* or *display_NKK()* methods which will do the following:  
     - upload an image to the NKK device from *imageBufferGFX[]* or *imageBufferNKK[]* and make the image visible.  
        *display()* will use *imageBufferGFX[]* as a source and will overwrite *imageBufferNKK[]*.  
         *display_NKK()* use *imageBufferNKK[]* as a source and does NOT change *imageBufferGFX[]*.  
	AND	
	 - set NKK device background colour and brightness as per library's variables *bkgColour* and *bkgBrightnes*.
   
   *display()* method is also used for integration with Adafruit_GFX library.	 
	 
 7. Use other NKK_SmartDisplayLCD library methods like *clearImageBufferGFX()*, *invertImageBufferGFX()* etc to manage content of the image buffer you use.

 8. Use Adafruit_GFX_Ext object to access to Adafruit_GFX library methods like *setCursor()*, *print()*, *drawPixel()*, *fillRect()* etc to build 
   or adjust your image in the *imageBufferGFX[]* image buffer.  Do not forget to call *display()* method to transfer your image to the NKK device 
   and make it visible.  

See the examples and descriptions of the library functions provided in the code for more details.  
  
      
## Known Limitations:
Requires a native SPI object (like Arduino one) which handles SPI communications. With a mimimal changes to the library (an update to the class constructor) it can use a separate SPI handler such as https://github.com/adafruit/Adafruit_BusIO

## Hardware:
Tested with
    - Arduino Pro Mini 
	- Maple Mini board (STM32F103CBT6) and Arduino STM32 core (https://github.com/rogerclarkmelbourne/Arduino_STM32)  

## Repository Contents:
    /documentation - Image Builder (MS Excel file), NKK LCD 64x32 SmartDisplay application notes 
    /examples - examples on how the libraries can be used 

   
## License:
Copyright (c) 2021, IFH
All rights reserved.

NKK_SmartDisplayLCD is free software: you can redistribute it 
and/or modify it under the terms of the GNU General Public 
License as published by the Free Software Foundation, either 
version 3 of the License, or (at your option) any later version 
provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution.
3. Neither the name of the copyright holders nor the
names of its contributors may be used to endorse or promote products
derived from this software without specific prior written permission.


NKK_SmartDisplayLCD is provided ''AS IS'' in the hope that it will
be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR ANY PARTICULAR PURPOSE. 

IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE 
GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

See the GNU General Public License for more details.

## Donation:
If this project help you reduce your time to develop, you can give me a cup of coffee :) 

[![paypal](https://www.paypalobjects.com/en_US/i/btn/btn_donateCC_LG.gif)](https://www.paypal.com/donate/?business=ifhone777-hub%40yahoo.com&currency_code=USD)
