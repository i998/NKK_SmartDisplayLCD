/*********************************************************************
This file is a part of a library for NKK LCD 64x32 SmartDisplay

Copyright (c) 2021, IFH
All rights reserved.

GNU General Public License,  check license.txt for more information
All text above must be included in any redistribution
*********************************************************************/
/*
NKK Smart Display LCD 64*32 test code using library  NKK_SmartDisplayLCD 
    
 example 02- NKK as a library,
 using Maple Mini hardware (http://docs.leaflabs.com/static.leaflabs.com/pub/leaflabs/maple-docs/0.0.12/hardware/maple-mini.html) 
 and Arduino STM32 core (https://github.com/rogerclarkmelbourne/Arduino_STM32) 

 Status: works ok


// hardware setup for Maple Mini  
    //Use SPI_2 as it is 5v tolerant
    // Maple Mini SS   (NCS)  <-->  PB12 D31 pin no 31 -> NKK SS  (Signal is managed by NKK library to allow running several NKK devices with their own SS pins)
    // Maple Mini SCK  (CLK)  <-->  PB13 D30 pin no 30 -> NKK SCK (Clock for serial communication, maximum 8 MHZ)
    // Maple Mini MISO (SPI)  <-->  PB14 D29 pin no 29 -> NKK SDO (This is not required for NKK devices - no data output back from the NKK device) 
    // Maple Mini MOSI (SDO)  <-->  PB15 D28 pin no 15 -> NKK SDI 
    
    //Please note that STM32F103CBT6 is 3.3v and NKK LCD 64x32 SmartDisplay is 5v.  While direct connection without a level shifter works with that example,
    //such hardware setup is at your own risk.   
*/ 
	
#include <SPI.h>
#include <NKKSmartDisplayLCD.h>

//Setup SPI
#define SPIDEVICE_CS 31 //note this is for the SPI setup only. Actual SS signal is managed by NKK library to allow running several NKK devices with their own SS pins.  
SPIClass SPI_2(2); //Create an instance of the SPI Class called SPI_2 that uses the 2nd SPI Port

//Setup images 
//Note: for simplisity we do not use PROGMEM  
	 //"TEST 1" text with a corner white triangle *
	byte imgTest1[] = 
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6,0,127,131,131,241,252,0,5,0,8,4,64,16,32,0,4,128,8,0,64,16,32,0,4,64,8,0,64,16,32,0,4,0,8,0,64,16,32,0,4,0,8,0,128,16,32,0,4,0,8,1,0,240,32,0,4,0,8,2,0,16,32,0,4,0,8,4,0,16,32,0,4,0,8,4,0,16,32,128,4,0,8,4,0,16,32,192,4,0,8,4,64,16,32,160,63,128,8,3,131,240,32,144,0,0,0,0,0,0,0,136,0,0,0,0,0,0,0,132,0,0,0,0,0,0,0,130,0,0,0,0,0,0,0,129,0,0,0,0,0,0,0,128,128,0,0,0,0,0,0,128,64,0,0,0,0,0,0,128,32,0,0,0,0,0,0,128,16,0,0,0,0,0,0,128,8,0,0,0,0,0,0,128,4,0,0,0,0,0,0,128,2,0,0,0,0,0,0,128,1,0,0,0,0,0,0,255,255,128,0,0,0,0,0
	};

	 // Test GFX - landscape
	byte imgGFX1[] = 
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,252,241,131,131,127,0,0,0,32,16,64,4,8,0,0,0,32,16,64,0,8,224,125,17,32,16,64,0,8,16,4,10,32,16,64,0,8,16,4,4,32,16,128,0,8,144,61,4,32,240,0,1,8,16,5,4,32,16,0,2,8,16,5,10,32,16,0,4,8,224,4,17,32,16,0,4,8,0,0,0,32,16,0,4,8,0,0,128,32,16,64,4,8,0,0,192,32,240,131,3,8,0,0,160,0,0,0,0,0,0,0,144,0,0,0,0,0,0,0,136,0,0,0,0,0,0,0,132,0,0,0,0,0,0,0,130,0,0,0,0,0,0,0,129,0,0,0,0,0,0,128,128,0,0,0,0,0,0,64,129,0,0,0,0,0,0,32,130,0,0,0,0,0,0,16,132,0,0,0,0,0,0,8,136,0,0,0,0,0,0,4,144,0,0,0,0,0,0,2,160,0,0,0,0,0,0,1,192,0,0,0,0,0,128,255,255
	};

	// Test GFX - portrait
	byte imgGFX2[] = 
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,248,253,152,31,248,253,164,31,96,12,6,6,96,12,6,6,96,12,6,6,96,12,4,6,96,60,8,6,96,12,16,6,96,12,32,6,96,12,96,6,96,12,96,6,96,12,96,6,96,252,36,6,96,252,24,6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,128,247,69,0,64,16,40,0,64,16,16,0,64,246,16,0,64,20,16,0,64,20,40,0,128,19,68,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,128,0,0,0,64,0,0,0,32,0,0,0,16,0,0,0,8,32,0,0,4,112,0,0,2,168,0,0,1,36,1,128,0,32,0,64,0,32,0,32,0,32,0,16,0,32,0,8,0,32,0,20,0,32,0,34,0,32,0,65,0,32,128,128,0,32,64,0,1,32,32,0,2,0,16,0,4,0,8,0,8,0,4,0,16,0,2,0,32,0,1,0,64,128,0,0,128
	};


// Initialise NKK device
	  //Landscape
	NKK_SmartDisplayLCD NKK = NKK_SmartDisplayLCD(64,32,0,SPIDEVICE_CS,1000000,&SPI_2); 
	  //Portrait,with 180 rotation
    //NKK_SmartDisplayLCD NKK = NKK_SmartDisplayLCD(32,64,1,SPIDEVICE_CS,1000000);


    
void setup() {
  pinMode(SPIDEVICE_CS, OUTPUT);

  //==============================
   Serial.begin(9600);
   //Serial.begin(115200);
   //The program will wait for serial to be ready up to 10 sec then it will continue anyway  
     for (int i=1; i<=10; i++){
          delay(1000);
     if (Serial){
         break;
       }
     }
    Serial.println("Setup() started ");
  //===============================


//start SPI interface
  SPI_2.begin();
	  
// start NKK device
  NKK.begin(); 
  
}


void loop() {


  NKK.reset();
  
//test NKK colours   
  NKK.setColourNKK(255);  //White
  NKK.setBrightness(255);
  delay (1000);
  NKK.setColourNKK(195);  //Red
  delay (1000);
  NKK.setColourNKK(51);   //Green 
  delay (1000);
  NKK.setColourNKK(15);  //Blue 
  delay (1000); 
  NKK.setColourNKK(255);  //White
  delay (1000);

//test NKK image transfer 
  for(uint16_t i=0; i<256; i++)
	   {
		   NKK.imageBufferNKK[i] = imgTest1[i];
	   }
   NKK.display_NKK();
   delay (2000);

 
//test RGB colour 
  NKK.setColourRGB(255,255,0);  // set Yellow
  //delay (1000);  

// test GFX image transfer  
  for(uint16_t i=0; i<256; i++)
    {
        NKK.imageBufferGFX[i] = imgGFX1[i];
 
    }
  NKK.display();
  delay (2000);
 
 
  
}// End of the Loop


