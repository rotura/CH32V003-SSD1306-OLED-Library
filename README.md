# CH32V003 SSD1306 OLED Library
 Small library to draw from a CH32V003 RISC-V chip to a SSD1306 Oled display.
 
 It works with 128x64 I2C displays. 
 
 # Usage
 Select if you are going to use the normal or unbuffered version and delete the "oled_xxx.h" and "oled_xxx.cpp" files for the version you are not going to use.
 
 Copy the "ssd1306" folder to your proyect's source code and import the library with:
 ```
 #include <oled_segment.h>
 ```
 
 Then you can use it like (Use Delay_Ms to wait beetween changes):
 ```
	// Setup CH32V003 I2C pins
	I2C_init();
	
	// Init the OLED display
	OLED_init();

	// Set cursor to position 0,0
	OLED_setpos(0, 0); 

	// Write text
	OLED_clear();
	OLED_print("Hello world!");

	// Draw WCH Logo
	OLED_clear();
	OLED_DrawLogo();

	// Draw single pixels
	OLED_clear();
	OLED_DrawPixel(0,30,true);
	OLED_DrawPixel(1,31,true);
	OLED_DrawPixel(2,32,true);
	OLED_DrawPixel(3,33,true);
	OLED_DrawPixel(4,34,true);
	OLED_DrawPixel(5,35,true);
	OLED_DrawPixel(6,36,true);
	OLED_DrawPixel(7,37,true);
	OLED_DrawPixel(8,38,true);
	OLED_DrawPixel(9,39,true);
	OLED_DisplayBuffer();
	
	// Draw small bitmap
	OLED_clear();
	OLED_DrawBitmap(128-38, 64-56+1, 38, 56, dust_bmp);
	
	// Fill screen with a pattern (255 for white screen, 0 for black screen)
	OLED_fill(255);
 ```
 
 # Unbuffered version
 The standar library version use a 1024 bytes buffer for the DrawPixel to work with the rest of functions.
 
 If you dont this, you can use the "oled_small" version instead: 
 ```
 #include <oled_small.h>
 ```
 Buffered: 
 
 ![Buffered](https://github.com/rotura/CH32V003-SSD1306-OLED-Library/blob/main/images/buffered.jpg)
 
 Unbuffered:
 
 ![Unbuffered](https://github.com/rotura/CH32V003-SSD1306-OLED-Library/blob/main/images/unbuffered.jpg)
 
 
 # Changelog
 
 * v1.0.0 - Initial release 
 