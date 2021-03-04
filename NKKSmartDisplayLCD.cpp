/*********************************************************************
This is a library for NKK LCD 64x32 SmartDisplay
  https://www.nkkswitches.com/smartdisplay/

Copyright (c) 2021, IFH
All rights reserved.

GNU General Public License,  check license.txt for more information
All text above must be included in any redistribution
*********************************************************************/
/*********************************************************************
2021-03-04
Version: v01.0.1 , tested with STM32
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
- Tested with Maple Mini board and Arduino STM32 core (https://github.com/rogerclarkmelbourne/Arduino_STM32)    
- Tested with NKK IS15EBFP4RGB-09YN LCD 64 x 32 Pushbutton, shall work with these models as well:
    S15DBFP4RGB
    IS15DBFP4RGB-09YN
    S15DSBFP4RGB
    IS01DBFRGB
    IS15EBFP4RGB
    IS15ESBFP4RGB
    IS01EBFRGB
*********************************************************************/

#include <NKKSmartDisplayLCD.h>

/**************************************************************************/
/*!
    @brief  Constructor for NKK_SmartDisplayLCD object, using SPI built-in SPI SPIClass.
    @param  w The NKK LCD screen width in pixels.  Max w is  256 and  Max w*h/8  = 65535 for this library code. 
	@param  h The NKK LCD screen height in pixels.  Max h is  256 and  Max w*h/8  = 65535 for this library code.  
    @param  isRotate180  A flag to indicate that the picture need to be rotated by 180 degrees to accommodate different possible footprints of an NKK device on your pcb. Applied just before the picture is uploaded to the NKK device by the display() and display_NKK() functions.   
    @param  cspin Slave Select(Chip Select) signal  (to allow use more than one NKK device with their own SS signals).
	@param  freqSPI SPI frequency, Hz. 
	@param  A reference (pointer) to native SPI object which handles SPI communications. 
	@return NKK_SmartDisplayLCD object.
    @note   Call the object's begin() function before use SPI begin() etc. is performed there!. CSPIN is included to handle multiple NKK devices with own SS signals. 
*/
/**************************************************************************/
NKK_SmartDisplayLCD::NKK_SmartDisplayLCD(uint8_t w,
                                   uint8_t h,
                                   uint8_t isRotate180, //0- no rotation* 1 - 180 degree rotation    
								   uint8_t cspin,
								   uint32_t freqSPI,	
								   SPIClass *SPI_A)
								   
								   
								   
{
   //set SPI object 
		//=====================
		// NKK definitions (Application Notes for LCD 64x32 SmartDisplayLCD Revision C)
		// The data is taken on the falling edge of the clock.
		// For a PIC microcontroller the following setup will work:
		//  CKP=1, CKE=1, SMP=0/1
		//  CKP=0, CKE=0, SMP=0/1

		// note that NCPHA is the inversion of CPHA"
		// so it shall be as below:
		// CPOL/CKP=1, CKE/NCPHA=1 i.e. failing edge is red, CPHA=0,  -> MODE2
		// CPOL/CKP=0, CKE/NCPHA=0 i.e. failing edge is blue, CPHA=1, -> MODE1

		// https://en.wikipedia.org/wiki/Serial_Peripheral_Interface#Clock_polarity_and_phase
		//====================

  _SPI = SPI_A; // link to the SPI object to communicate with the NKK device
  _freqSPI=freqSPI;
  _spiSetting = new SPISettings(_freqSPI, MSBFIRST, SPI_MODE2); //NKK uses dataOrder=MSBFIRST, dataMode=SPI_MODE2
  
  //set variables, some very basic validation applied just in case to fit the NKK devices as expected  - 64*32, Max w*h/8  = 65535 for this library. 
  //NKK Smart Display is 64bit*32bit, feel free to adjust if you adopt this library for other displays
  if (w>64) {_w=64;}  else {_w = w;}
  if (h>64) {_h=64;}  else {_h = h;}
  if (w<32) {_w=32;}  else {_w = w;}
  if (h<32) {_h=32;}  else {_h = h;}
 

 _imageBufferLength =_w*_h/8; //  image array length in bytes, max allowed in this library is uint16_t which is 65535.
 
 _isRotate180 = isRotate180;
 
 //Set Slave Select(Chip Select) signal  (to allow use more than one NKK device with their own SS signals)
 _cs = cspin;
 pinMode(_cs, OUTPUT);
 digitalWrite(_cs, HIGH);
 
 }

/**************************************************************************/
/*!
    @brief  Destructor for NKK_SmartDisplayLCD object.
*/
/**************************************************************************/
NKK_SmartDisplayLCD::~NKK_SmartDisplayLCD(void) {
}

/**************************************************************************/
/*! 
    @brief  Starts the SPI interface and resets NKK hardware.
*/
/**************************************************************************/
void NKK_SmartDisplayLCD::begin(void) {

  //Start of SPI interface
  _SPI->begin();  

  //NKK reset
  reset();
}

/**************************************************************************/
/*! 
    @brief  Sends a reset command to the NKK chip over SPI.
*/
/**************************************************************************/
void NKK_SmartDisplayLCD::reset(void) {

     sendCommandAndDataToSPI(NKK_SmartDisplayLCD_Reset, NKK_SmartDisplayLCD_Reset_data);
  }
  
/**************************************************************************/
/*! 
    @brief  Converts current image buffer from GFX format to NKK native format. 
*/
/**************************************************************************/
void NKK_SmartDisplayLCD::convertGFX2NKK(void){
		
	convertGFX2NKK(imageBufferGFX, imageBufferNKK);
}

/**************************************************************************/
/*! 
    @brief  Converts current image buffer from GFX format to NKK native format. 
	@param  imageBufferGFX[] An array with an image arranged as per the GFX format.  
	@param  imageBufferNKK[] An array with an image arranged as per the NKK native format.  
*/
/**************************************************************************/
void NKK_SmartDisplayLCD::convertGFX2NKK(byte imageBufferGFX[], byte imageBufferNKK[]){
//Serial.println("NKK_SmartDisplayLCD::convertGFX2NKK:started");	uint32_t startTime = millis();	


    uint16_t HeightInRows = _h;
	uint16_t WidthInBytes = _w/8;

	
 if (_w>=_h) {
  //this is Landscape		
			//swap bytes in every row      
			//for each row 
			 for (uint16_t i = 0; i<HeightInRows; i++) 
			 {
						 // for each column  - swap bytes 
						  for (uint16_t j = 0; j<WidthInBytes; j++) {
							imageBufferNKK[i*WidthInBytes+WidthInBytes-j-1] = imageBufferGFX[i*WidthInBytes+j];
							//[i*WidthInBytes+WidthInBytes-j-1] : number of bytes in all full rows plus another full row minus current position in the row 
							//[i*WidthInBytes+j]   :             number of bytes in all full rows plus current position in the row   
							//Serial.print("converter1: i="); Serial.print(i);  Serial.print(" j="); Serial.println(j);
							//Serial.print("converter1: source="); Serial.print(i*WidthInBytes+j);  Serial.print(" destination="); Serial.println(i*WidthInBytes+WidthInBytes-j-1);
						  }
		  
			 } 
			 
			 
	
   }
   else {
   //this is Portrait
     
	 //Divide picture to 8 bytes (8*8 bit block) and convert them using bitwise operations
	 
		uint16_t numOfLayers=_h/8; //number of layers of blocks
		uint16_t blocksPerLayer=_w/8; //number of 8bit*8bit blocks per layer 

            //Array  - collection of "vertical" layers
            for (uint16_t l = 0; l<numOfLayers; l++) {
                
                        //Layer - "horisontal" line of blocks 
						uint16_t layerStartGFX=8*blocksPerLayer*l; //8 bytes per block 
                        uint16_t layerStartNKK=1*l;  // just 1 byte vertical shift
						//Serial.print("l:"); Serial.print(l); Serial.print(" layerStartGFX="); Serial.print(layerStartGFX); Serial.print(" layerStartNKK="); Serial.println(layerStartNKK);
						
                        for (uint16_t b = 0; b<blocksPerLayer; b++) {
						
						        //Block  - 64 bits, 8 "horisontal" GFX bytes and 8 "vertical" NKK bytes 
                                uint16_t blockStartGFX=layerStartGFX + 1*b ; // just 1 byte horisontal shift 
                                uint16_t blockStartNKK=layerStartNKK + 8*numOfLayers*b ;//  8 bit shift per layer 
                                //Serial.print("b:"); Serial.print(b); Serial.print(" blockStartGFX="); Serial.print(blockStartGFX); Serial.print(" blockStartNKK="); Serial.println(blockStartNKK);
						   
                                for (uint8_t s = 0; s<8; s++) {
                                               
										//Sub block - conversion of each bit in the block bytes
										uint16_t subBlockStartGFX=blockStartGFX;
										uint16_t subBlockStartNKK=blockStartNKK+s*numOfLayers;
										//Serial.print("s:"); Serial.print(s); Serial.print(" subBlockStartGFX="); Serial.print(subBlockStartGFX); Serial.print(" subBlockStartNKK="); Serial.println(subBlockStartNKK);
                                        
																				
										//setting an NKK byte 
										  //Variable S is a number of NKK byte in the block from  left(0) to right (7) 
										  // subBlockStartNKK is NKK adress in the NKK array  for the target NKK byte 
										  // subBlockStartGFX is GFX adress  in the GFX array  for the first GFX byte in the block.  
										  // Other GFX bytes adresses are subBlockStartGFX+ +blocksPerLayer*ByteNumberInTheBlock   ( 0-top, 7 - bottom )    
										  // Variable i is a number of bit in the NKK byte from 0 (bottom bit) to 7 (top bit) 
										byte targetByte=imageBufferNKK[subBlockStartNKK];
										byte sourceByte=0;
										byte mask0=0;
										byte mask1=0;
										
										   //Setting bits in each NKK byte 
											for (uint8_t i = 0; i<8; i++) {
											
											//Source Byte for each NKK bit: 
											// for NKK bit 0: last GFX byte in the block : i=0,  GFXIndex=subBlockStartGFX+blocksPerLayer*7 = subBlockStartGFX+28
											// for NKK bit 7: first GfX byte in the block: i=7,  GFXIndex=subBlockStartGFX        										
											sourceByte= imageBufferGFX[subBlockStartGFX+blocksPerLayer*(7-i)];
											
											//Serial.print("i:"); Serial.print(i); Serial.print("step1       "); Serial.print(" GFXindex="); Serial.print(subBlockStartGFX+blocksPerLayer*(7-i)); Serial.print(" NKKindex="); Serial.print(subBlockStartNKK);  Serial.print(" GFX[]=");  Serial.print(sourceByte); Serial.print(" NKK[]=");  Serial.println(targetByte);
											
											//Set target bit i to 1 in NKK  - moving target bit=1, other bits are as they are.  So do OR with mask like 000010000 to get the target as 111x1111
											mask0=1<<i; // orig math expression:2^i or bit shift as 1<<i
											targetByte=targetByte | mask0;
											
											//Serial.print("i:"); Serial.print(i); Serial.print("step2-mask0 "); Serial.print(" GFXindex="); Serial.print(subBlockStartGFX+blocksPerLayer*(7-i)); Serial.print(" NKKindex="); Serial.print(subBlockStartNKK);  Serial.print(" GFX[]=");  Serial.print(sourceByte); Serial.print(" NKK[]=");  Serial.print(targetByte);Serial.print(" mask0=");  Serial.println(mask0);
											
											//Shift source byte accordingly  - for first byte in NKK block use bit 0 from GFX,  for last NKK byte use bit 7 as "bit in use" which will be copied to target
											sourceByte=sourceByte>>s;
											
											
											//Serial.print("i:"); Serial.print(i); Serial.print("step3-  >>s "); Serial.print(" GFXindex="); Serial.print(subBlockStartGFX+blocksPerLayer*(7-i)); Serial.print(" NKKindex="); Serial.print(subBlockStartNKK);  Serial.print(" GFX[]=");  Serial.print(sourceByte); Serial.print(" NKK[]=");  Serial.println(targetByte);
											
											
											//Place bit in use of the source to bit i  in the target (at this step we "rotating" the source byte)
											   //a)shift bit in use of the source to bit i  in the source 
												 sourceByte=sourceByte<<i;
											
											      //Serial.print("i:"); Serial.print(i); Serial.print("step4-  <<i "); Serial.print(" GFXindex="); Serial.print(subBlockStartGFX+blocksPerLayer*(7-i)); Serial.print(" NKKindex="); Serial.print(subBlockStartNKK);  Serial.print(" GFX[]=");  Serial.print(sourceByte); Serial.print(" NKK[]=");  Serial.println(targetByte);
											
												//b)make unused source bits as 1 so we can do AND later.  So do OR with mask like 11101111  to get the source as 111x1111
												  mask1=~(1<<i);//255 - (1<<i); //inverted mask0
												  sourceByte=sourceByte | mask1;

											      //Serial.print("i:"); Serial.print(i); Serial.print("step5-mask1 "); Serial.print(" GFXindex="); Serial.print(subBlockStartGFX+blocksPerLayer*(7-i)); Serial.print(" NKKindex="); Serial.print(subBlockStartNKK);  Serial.print(" GFX[]=");  Serial.print(sourceByte); Serial.print(" NKK[]=");  Serial.print(targetByte); Serial.print(" mask1=");  Serial.println(mask1);

								
												//c)bits are aligned, do AND with source and target  and get the bit i set in the target, other target bits are intact 
												 targetByte=targetByte & sourceByte;
												 
											     //Serial.print("i:"); Serial.print(i); Serial.print("step6- tORs "); Serial.print(" GFXindex="); Serial.print(subBlockStartGFX+blocksPerLayer*(7-i)); Serial.print(" NKKindex="); Serial.print(subBlockStartNKK);  Serial.print(" GFX[]=");  Serial.print(sourceByte); Serial.print(" NKK[]=");  Serial.println(targetByte);											 
											
											//Write the target byte back to NKK array
											imageBufferNKK[subBlockStartNKK]=targetByte;
										
										}
										
										
										
										
                                }     
                        }
                }

 }	

//Serial.print("NKK_SmartDisplayLCD::convertGFX2NKK:finished, taken millis:"); Serial.println(millis() -	startTime);			 
}

  /**************************************************************************/
/*! 
    @brief  Converts data in NKK image buffer so the image is rotated 180 degrees.
	@param  imageBufferNKK[] An array with an image arranged as per the NKK native format.  
*/
/**************************************************************************/
void NKK_SmartDisplayLCD::rotate180_NKK(byte imageBufferNKK[]) { 
 	 //Divide picture to layers of bytes and convert them using bitwise operations
		 //last byte in array become first byte
		 //layers become mirrored 
		 //byte bits get reversed 

	//Serial.println("NKK_SmartDisplayLCD::rotate180_NKK:started");	 
		 
 
		uint8_t numOfLayers=_h; //number of layers 
	    uint8_t blocksPerLayer=_w/8; //number of bytes per layer 
		
		
		    //Array  - collection of "vertical" layers. Cycle through the first half of them and swap the bytes 
            for (uint8_t l = 0; l<(numOfLayers/2); l++) {
                
                        //Layer - "horisontal" line of bytes
                        uint8_t layer1StartNKK=blocksPerLayer*l;  // just 1 byte vertical shift down
						uint8_t layer2StartNKK=_imageBufferLength - blocksPerLayer - blocksPerLayer*l;  // just 1 byte vertical shift up
						//Serial.print("l:"); Serial.print(l); Serial.print(" layer1StartNKK="); Serial.print(layer1StartNKK); Serial.print(" layer2StartNKK="); Serial.println(layer2StartNKK);
						
				
						// bytes in the layer  
                        for (uint8_t b = 0; b<blocksPerLayer; b++) {
						     uint8_t index1 =layer1StartNKK + b;  //array index for the original byte 
							 uint8_t index2 = layer2StartNKK + blocksPerLayer - b-1; //array index for the mirrored byte 
                            //Serial.print("b:"); Serial.print(b); Serial.print(" index1="); Serial.print(index1); Serial.print(" index2="); Serial.println(index2);
		                      
							//swap the bytes and reverse them 
							uint8_t tmpByte=imageBufferNKK[index2];
							imageBufferNKK[index2]=reverseByte(imageBufferNKK[index1]);
							imageBufferNKK[index1]=reverseByte(tmpByte);

                        }
						
			}			

						
	//Serial.println("NKK_SmartDisplayLCD::rotate180_NKK:started:finished"); 					
 }
 
/**************************************************************************/
/*! 
    @brief  Reverses bits in a byte (7->0, 6->1,..., 0->7). 
	@param  b A source byte.  
	@return A source byte with its bits reversed.   
*/
/**************************************************************************/ 
byte NKK_SmartDisplayLCD::reverseByte(byte b){
//Serial.println("NKK_SmartDisplayLCD::reverseByte started");	 byte b1=b;
  	

   //First the left four bits are swapped with the right four bits. 
   //Then all adjacent pairs are swapped and then all adjacent single bits. 
   //This results in a reversed order.	
   b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
   b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
   b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
 	 
//Serial.print("NKK_SmartDisplayLCD::reverseByte: "); Serial.print(b1);  Serial.print(" -> "); Serial.println(b);
//Serial.println("NKK_SmartDisplayLCD::reverseByte finished");	
 	
	  return b;
    }	
	
 /**************************************************************************/
/*! 
    @brief  Sets background colour for the NKK device.    
	@param  A byte to define colour in NKK format (RRGGBBxx). 
*/
/**************************************************************************/ 
void NKK_SmartDisplayLCD::setColourNKK(byte data) {
		
		data=data | 0x03; // apply mask 
		bkgColour=data;   //save settings into the class variable
		sendCommandAndDataToSPI( NKK_SmartDisplayLCD_Set_RGB, data);
	}
	
 /**************************************************************************/
/*! 
    @brief  Sets a closest available background colour for the NKK device to the colour specified as RGB.  
	@param  A byte to define Red component.  
	@param  A byte to define Green component.  
	@param  A byte to define Blue component.  
*/
/**************************************************************************/ 	  
void NKK_SmartDisplayLCD::setColourRGB(byte R, byte G, byte B) {
	 //map and constrain to 2 bit each 
	 R = map (R, 0,255, 0, 3);
     G = map (G,0,255, 0, 3);
	 B = map (B,0,255, 0, 3);
	 
	 //shift bits to fit NKK format
	 R=R<<6;
	 G=G<<4;
	 B=B<<2;
	 
	//combine all together and set the colour  
    setColourNKK(R | G | B);
	 }  
	 
 /**************************************************************************/
/*! 
    @brief  Sets background brightness for the NKK device.    
	@param  A byte to define background brightness in NKK format (BBBxxxxx). 
*/
/**************************************************************************/ 	  
void NKK_SmartDisplayLCD::setBrightness(byte data) {
		
		data=data | 0x1F; // apply mask 
	    bkgBrightnes=data; //save settings into the class variable
	    sendCommandAndDataToSPI(NKK_SmartDisplayLCD_Set_Bright, data);
	}
	
 /**************************************************************************/
/*! 
    @brief  Displays a picture in GFX format i.e. converts imageBufferGFX to NKK format, uploads the NKK device, sets colour and brightness as per the NKK_SmartDisplayLCD object variables.     
*/
/**************************************************************************/ 
 void NKK_SmartDisplayLCD::display(void) {
        //Serial.println("NKK_SmartDisplayLCD::display started");
		
		 //convert GFX image to native NKK one 
		 convertGFX2NKK(imageBufferGFX, imageBufferNKK);
		 
		//rotation and send to SPI
		if (_isRotate180) {
			rotate180_NKK(imageBufferNKK);
			sendImageToSPI(imageBufferNKK, _imageBufferLength);
			} 
		else {
			sendImageToSPI(imageBufferNKK, _imageBufferLength);
			}
		 
		//set colour and brightness
		setColourNKK(bkgColour);
		setBrightness(bkgBrightnes);
		 
        //Serial.println("NKK_SmartDisplayLCD::display finished");		 
	}
	
 /**************************************************************************/
/*! 
    @brief  Displays a picture in NKK format i.e. uploads imageBufferNKK to the NKK device, sets colour and brightness as per the NKK_SmartDisplayLCD object variables.     
*/
/**************************************************************************/	
void NKK_SmartDisplayLCD::display_NKK(void) {
		//Serial.println("NKK_SmartDisplayLCD::display_NKK started");
		 
		//rotation and send to SPI 
		if (_isRotate180) {
			rotate180_NKK(imageBufferNKK);
			sendImageToSPI(imageBufferNKK, _imageBufferLength);
			} 
		else {
			sendImageToSPI(imageBufferNKK, _imageBufferLength);
			}

			 //Serial.println("NKK_SmartDisplayLCD::display_NKK finished");
		
		
        //set colour and brightness
        setColourNKK(bkgColour);
		setBrightness(bkgBrightnes);
		}	
		
 /**************************************************************************/
/*! 
    @brief  Returns image width as configured for the NKK_SmartDisplayLCD object 
	@return Image width in pixels 
*/
/**************************************************************************/
uint8_t NKK_SmartDisplayLCD::getWidth(void) {
return _w;
}

 /**************************************************************************/
/*! 
    @brief  Returns image height as configured for the NKK_SmartDisplayLCD object 
	@return Image height in pixels 
*/
/**************************************************************************/
uint8_t NKK_SmartDisplayLCD::getHeigth(void) { 
return _h;
}

 /**************************************************************************/
/*! 
    @brief  Returns Image Buffer length
	@return Number of elements (bytes) in imageBufferGFX[] and imageBufferNKK[] arrays  
*/
/**************************************************************************/
uint16_t NKK_SmartDisplayLCD::getImageBufferLength(void) {
return _imageBufferLength;
}

/**************************************************************************/
/*!
    @brief  Draw a pixel to the imageBufferGFX[]
    @param  x   x coordinate, starts with 0
    @param  y   y coordinate, starts with 0
    @param  color NKK LCD 64x32 SmartDisplay is monochrome, color will be converted to 0 or 1 only
*/
/**************************************************************************/
void NKK_SmartDisplayLCD::drawPixel( uint8_t x, uint8_t y, uint8_t color)
    {
      // input data validation 
          if (x > _w -1) {x=_w-1; }   // _w is 1:64 , x is 0:63
          if (y > _h -1) {y=_h-1; }   // _h is 1:32 , y is 0:31
          if (color > 0) {color=1;}  
      
      //calculate image buffer index 
          uint16_t pixelNumber = y*_w+ (x+1);               
          uint16_t fullBytesCount = (uint16_t)  pixelNumber / 8;     // number of full bytes in pixel count 
	      uint8_t  bitNumber=0;	  
          uint16_t  arrayIndex=0;
		  
		  if (pixelNumber - fullBytesCount * 8 ==0) { //we have the last bit in a byte and fullBytesCount includes the byte where our pixel is located 
	             bitNumber=7;	
                 arrayIndex=fullBytesCount-1;
		  }
          else { //normal scenario,  our pixel is located in the byte after the fullBytesCount
		       bitNumber = pixelNumber - (fullBytesCount) * 8 -1;   // bit number is 0:7. 
			                                                         
		       arrayIndex=fullBytesCount;  // arrayIndex is the current byte - 1  i.e. equal to fullBytesCount
		 
		  }
		  
		  //Serial.print("x:"); Serial.print(x); Serial.print(" y:"); Serial.print(y);Serial.print(" pixelNumber=");Serial.print(pixelNumber);Serial.print(" fullBytesCount=");Serial.print(fullBytesCount);Serial.print(" arrayIndex=");Serial.print(arrayIndex);Serial.print(" bitNumber=");Serial.println(bitNumber);
		  

          //set the pixel
          color=color<<bitNumber;  
          byte data = imageBufferGFX[arrayIndex];
          byte mask = ~(1 <<bitNumber);  
          data= data & mask;  
          imageBufferGFX[arrayIndex] = data | color;        
    }   

/******************************************************************************/
/* Image Buffer helpers                                                       */
/******************************************************************************/	
void NKK_SmartDisplayLCD::clearImageBufferGFX(void) {
for(uint16_t i=0; i<_imageBufferLength; i++)
	   {
		   imageBufferGFX[i] = 0;
	   }
}
void NKK_SmartDisplayLCD::clearImageBufferNKK(void) {
for(uint16_t i=0; i<_imageBufferLength; i++)
	   {
		   imageBufferNKK[i] = 0;
	   }
}
void NKK_SmartDisplayLCD::invertImageBufferGFX(void) {
for(uint16_t i=0; i<_imageBufferLength; i++)
	   {
		   imageBufferGFX[i] = ~imageBufferGFX[i];
	   }
}
void NKK_SmartDisplayLCD::invertImageBufferNKK(void) {
for(uint16_t i=0; i<_imageBufferLength; i++)
	   {
		   imageBufferNKK[i] = ~imageBufferNKK[i];
	   }
}

/******************************************************************************/
/* actual read/write functions for SPI interface                              */
/******************************************************************************/

//Function to write a NKK Image Upload command and an array to SPI
void NKK_SmartDisplayLCD::sendImageToSPI(byte buffer[], uint16_t length)
{
//Serial.println("NKK_SmartDisplayLCD::sendImageToSPI: array will be transferred");
	 
 digitalWrite(_cs, LOW); // enable Slave Select
 beginTransaction();

 _SPI->transfer((byte) NKK_SmartDisplayLCD_Img_Upload) ; 
  //Serial.println((byte) NKK_SmartDisplayLCD_Img_Upload);

  for (int i = 0; i < length; i++) {
   _SPI->transfer((byte) buffer[i] ); //Send the array element  over SPI
 //Serial.print(i);  Serial.print(":"); Serial.println((byte) buffer[i]);
 }  


  endTransaction();
  digitalWrite(_cs, HIGH); // disable Slave Select


//Serial.println("NKK_SmartDisplayLCD::sendImageToSPI: array transferred");
} 


//Function to transfer an array to an SPI port
void NKK_SmartDisplayLCD::sendArrayToSPI(byte buffer[], uint16_t length)
{
//Serial.println("NKK_SmartDisplayLCD::sendArrayToSPI: array will be transferred");
	 
 digitalWrite(_cs, LOW); // enable Slave Select
 beginTransaction();

 for (int i = 0; i < length; i++) {
   _SPI->transfer((byte) buffer[i] ); //Send the array element  over SPI
 //Serial.print(i);  Serial.print(":"); Serial.println((byte) buffer[i]);
 }  


  endTransaction();
  digitalWrite(_cs, HIGH); // disable Slave Select


//Serial.println("NKK_SmartDisplayLCD::sendArrayToSPI: array transferred");
}


//Function to write a command and an array to SPI
void NKK_SmartDisplayLCD::sendCommandAndDataToSPI(byte command, byte data)
{
  
 digitalWrite(_cs, LOW); // enable Slave Select
 beginTransaction();
   
  _SPI->transfer((byte) command); 
  _SPI->transfer((byte) data);  
 
  endTransaction();
  digitalWrite(_cs, HIGH); // disable Slave Select

} 


//Manually begin a transaction (calls beginTransaction if hardware SPI)
void NKK_SmartDisplayLCD::beginTransaction(void) {
 if (_SPI) {
    _SPI->beginTransaction(*_spiSetting);
  }
}


//Manually end a transaction (calls endTransaction if hardware SPI)
void NKK_SmartDisplayLCD::endTransaction(void) {
_SPI->endTransaction();
  if (_SPI) {
    _SPI->endTransaction();
  }
}
