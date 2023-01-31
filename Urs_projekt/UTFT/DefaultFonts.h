/*
 * DefaultFonts.h
 *
 * Created: 2/19/2013 2:44:41 PM
 *  Author: ron
 */ 


#ifndef DEFAULTFONTS_H_
#define DEFAULTFONTS_H_



#if defined(__AVR__)
#include <avr/pgmspace.h>
#define fontdatatype uint8_t
#elif defined(__PIC32MX__)
#define PROGMEM
#define fontdatatype const unsigned char
#elif defined(__arm__)
#define PROGMEM
#define fontdatatype const unsigned char
#endif


// SmallFont.c
// Font Size	: 8x12
// Memory usage	: 1144 bytes
// # characters	: 95
extern const fontdatatype SmallFont[1144] PROGMEM;

// BigFont.c (C)2010 by Henning Karlsen
// Font Size	: 16x16
// Memory usage	: 3044 bytes
// # characters	: 95

extern const fontdatatype BigFont[3044] PROGMEM;


// SevenSegNumFont.c
// Font Size	: 32x50
// Memory usage	: 2004 bytes
// # characters	: 10

extern const fontdatatype SevenSegNumFont[2004] PROGMEM;
#endif /* DEFAULTFONTS_H_ */