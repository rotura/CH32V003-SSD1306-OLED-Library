// ===================================================================================
// SSD1306 128x64 Pixels OLED Terminal Functions                              * v1.5 *
// ===================================================================================
//
// Collection of the most necessary functions for controlling an SSD1306 128x64 pixels
// I2C OLED for the display of text in the context of emulating a terminal output.
//
// References:
// -----------
// - Neven Boyanov: https://github.com/tinusaur/ssd1306xled
//
// 2024 by Enol Matilla: https://github.com/rotura

#include "oled_small.h"

// OLED initialisation sequence
const uint8_t OLED_INIT_CMD[] = {
  OLED_MULTIPLEX,   0x3F,                 // set multiplex ratio  
  OLED_CHARGEPUMP,  0x14,                 // set DC-DC enable  
  OLED_MEMORYMODE,  0x02,                 // set page addressing mode
  OLED_COMPINS,     0x12,                 // set com pins
  OLED_XFLIP, OLED_YFLIP,                 // flip screen
  OLED_DISPLAY_ON                         // display on
};

const uint8_t ssd1306_init_sequence [] = {	// Initialization Sequence
	0xAE,			// Set Display ON/OFF - AE=OFF, AF=ON
	0xD5, 0xF0,		// Set display clock divide ratio/oscillator frequency, set divide ratio
	0xA8, 0x3F,		// Set multiplex ratio (1 to 64) ... (height - 1)
	0xD3, 0x00,		// Set display offset. 00 = no offset
	0x40 | 0x00,	// Set start line address, at 0.
	0x8D, 0x14,		// Charge Pump Setting, 14h = Enable Charge Pump
	0x20, 0x00,		// Set Memory Addressing Mode - 00=Horizontal, 01=Vertical, 10=Page, 11=Invalid
	0xA0 | 0x01,	// Set Segment Re-map
	0xC8,			// Set COM Output Scan Direction
	0xDA, 0x12,		// Set COM Pins Hardware Configuration - 128x32:0x02, 128x64:0x12
	0x81, 0x3F,		// Set contrast control register - 0x01 to 0xFF - Default: 0x3F
	0xD9, 0x22,		// Set pre-charge period (0x22 or 0xF1)
	0xDB, 0x20,		// Set Vcomh Deselect Level - 0x00: 0.65 x VCC, 0x20: 0.77 x VCC (RESET), 0x30: 0.83 x VCC
	0xA4,			// Entire Display ON (resume) - output RAM to display
	0xA6,			// Set Normal/Inverse Display mode. A6=Normal; A7=Inverse
	0x2E,			// Deactivate Scroll command
	0xAF,			// Set Display ON/OFF - AE=OFF, AF=ON
	0x22, 0x00, 0x3f,	// Set Page Address (start,end) 0 - 63
	0x21, 0x00,	0x7f,	// Set Column Address (start,end) 0 - 127
};

// OLED global variables
uint8_t line, column, scroll;

// OLED set cursor to line start
void OLED_setline(uint8_t line) {
  I2C_start(OLED_ADDR);                   // start transmission to OLED
  I2C_write(OLED_CMD_MODE);               // set command mode
  I2C_write(OLED_PAGE + line);            // set line
  I2C_write(0x00); I2C_write(0x10);       // set column to "0"
  I2C_stop();                             // stop transmission
}

// OLED clear line
void OLED_clearline(uint8_t line) {
  uint8_t i;
  OLED_setline(line);                     // set cursor to line start
  I2C_start(OLED_ADDR);                   // start transmission to OLED
  I2C_write(OLED_DAT_MODE);               // set data mode
  for(i=128; i; i--) {                    // clear the line
    I2C_write(0x00);
  }     
  I2C_stop();                             // stop transmission
}

// OLED clear screen and buffer
void OLED_clear(void) {
  uint16_t i;
		OLED_setpos(0, 0);                            // set cursor to first digit
  I2C_start(OLED_ADDR);                           // start transmission to OLED
  I2C_write(OLED_DAT_MODE);                       // set data mode
  for(i=128*8; i; i--) {                          // clear screen and buffer
    I2C_write(0x00);
    }           
  I2C_stop();                                     // stop transmission
}

// OLED clear the top line, then scroll the display up by one line
void OLED_scrollDisplay(void) {
  OLED_clearline(scroll);                 // clear line
  scroll = (scroll + 1) & 0x07;           // set next line
  I2C_start(OLED_ADDR);                   // start transmission to OLED
  I2C_write(OLED_CMD_MODE);               // set command mode
  I2C_write(OLED_OFFSET);                 // set display offset:
  I2C_write(scroll << 3);                 // scroll up
  I2C_stop();                             // stop transmission
}

// OLED init function
void OLED_init(void) {
  uint8_t i;
  I2C_init();                             // initialize I2C first
  I2C_start(OLED_ADDR);                   // start transmission to OLED
  I2C_write(OLED_CMD_MODE);               // set command mode
  for(i = 0; i < sizeof(ssd1306_init_sequence); i++)
    I2C_write(ssd1306_init_sequence[i]);          // send the command bytes
  I2C_stop();                             // stop transmission
  scroll = 0;                             // start with zero scroll
  OLED_clear();                           // clear screen
  OLED_setpos(0,0);
}

// OLED plot a single character
void OLED_plotChar(char c, bool inverted) {
  I2C_start(OLED_ADDR);                   // start transmission to OLED
  I2C_write(OLED_DAT_MODE);               // set data mode
  for(short i=0 ; i!=5; i++){
    I2C_write(inverted? ~(font5x8[c-32][i]) : font5x8[c-32][i] );
  }
  I2C_write(inverted? ~0x00 : 0x00);                        // write space between characters
  I2C_stop();                             // stop transmission
}

// OLED write a character or handle control characters
void OLED_write(char c, bool inverted) {
  c = c & 0x7F;                           // ignore top bit
  // normal character
  if(c >= 32) {
    OLED_plotChar(c, inverted);
  }
  // new line
  else if(c == '\n') {
    column = 0;
    if(line != 7) line++;
    OLED_setline((line + scroll) & 0x07);
  }
  // carriage return
  else if(c == '\r') {
    column = 0;
    OLED_setline((line + scroll) & 0x07);
  }
}

// OLED print string
void OLED_print(char* str) {
  while(*str) {
    OLED_write(*str++, false);
  }
}

// OLED print string
void OLED_printS(char* str, bool inverted) {
  while(*str) {
    OLED_write(*str++, inverted);
  }
}

// OLED print string with newline
void OLED_println(char* str, bool inverted) {
  OLED_printS(str, inverted);
  OLED_write('\n', inverted);
}

// For BCD conversion
const uint32_t DIVIDER[] = {1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000};

// Print decimal value (BCD conversion by substraction method)
void OLED_printD(uint32_t value, bool inverted) {
  uint8_t digits   = 10;                          // print 10 digits
  uint8_t leadflag = 0;                           // flag for leading spaces
  while(digits--) {                               // for all digits
    uint8_t digitval = 0;                         // start with digit value 0
    uint32_t divider = DIVIDER[digits];           // read current divider
    while(value >= divider) {                     // if current divider fits into the value
      leadflag = 1;                               // end of leading spaces
      digitval++;                                 // increase digit value
      value -= divider;                           // decrease value by divider
    }
    if(!digits)  leadflag++;                      // least digit has to be printed
    if(leadflag) OLED_write(digitval + '0', inverted);      // print the digit
  }
}

// Convert byte nibble into hex character and print it
void OLED_printN(uint8_t nibble, bool inverted) {
  OLED_write((nibble <= 9) ? ('0' + nibble) : ('A' - 10 + nibble), inverted);
}

// Convert byte into hex characters and print it
void OLED_printB(uint8_t value, bool inverted) {
  OLED_printN(value >> 4, inverted);
  OLED_printN(value & 0x0f, inverted);
}

// Convert word into hex characters and print it
void OLED_printW(uint16_t value, bool inverted) {
  OLED_printB(value >> 8, inverted);
  OLED_printB(value, inverted);
}

// Convert long into hex characters and print it
void OLED_printL(uint32_t value, bool inverted) {
  OLED_printW(value >> 16, inverted);
  OLED_printW(value, inverted);
}

// OLED set cursor position
void OLED_setpos(uint8_t x, uint8_t y) {
  I2C_start(OLED_ADDR);                   // start transmission to OLED
  I2C_write(OLED_CMD_MODE);               // set command mode
  I2C_write(OLED_PAGE | y);	              // set page start address
  I2C_write(x & 0x0F);			              // set lower nibble of start column
  I2C_write(OLED_COLUMN_HIGH | (x >> 4)); // set higher nibble of start column
  I2C_stop();                             // stop transmission
}

void ssd1306_start_data(void) {
  I2C_start(OLED_ADDR);   
	I2C_write(0x40);			// Control byte: D/C=1 - write data
}

// OLED fill screen
void OLED_fill(uint8_t p) {
  OLED_setpos(0, 0);                      // set cursor to display start
  I2C_start(OLED_ADDR);                   // start transmission to OLED
  I2C_write(OLED_DAT_MODE);               // set data mode
  for(uint16_t i=128*8; i; i--){          // send pattern
    I2C_write(p);
  } 
  I2C_stop();                             // stop transmission
}

// OLED draw bitmap
void OLED_DrawBitmap(uint8_t x0, uint8_t y0, uint8_t w, uint8_t h, const uint8_t* bmp, bool inverted) {
	int z=0;
  for(uint8_t y = y0; y < y0+(h/8); y++) {
    OLED_setpos(x0, y);
    I2C_start(OLED_ADDR);
    I2C_write(OLED_DAT_MODE);
    for(uint8_t x = x0; x < x0+w; x++)
		{
      I2C_write(inverted? ~(bmp[z]) : bmp[z] );
			z++;
		}
    I2C_stop();
  }
}