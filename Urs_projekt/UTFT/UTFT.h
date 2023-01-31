/*
  UTFT.h - Arduino/chipKit library support for Color TFT LCD Boards
  Copyright (C)2010-2012 Henning Karlsen. All right reserved
  
  This library is the continuation of my ITDB02_Graph, ITDB02_Graph16
  and RGB_GLCD libraries for Arduino and chipKit. As the number of 
  supported display modules and controllers started to increase I felt 
  it was time to make a single, universal library as it will be much 
  easier to maintain in the future.
  Basic functionality of this library was origianlly based on the 
  demo-code provided by ITead studio (for the ITDB02 modules) and 
  NKC Electronics (for the RGB GLCD module/shield).
  This library supports a number of 8bit, 16bit and serial graphic 
  displays, and will work with both Arduino and chipKit boards. For a 
  full list of tested display modules and controllers, see the 
  document UTFT_Supported_display_modules_&_controllers.pdf.
  When using 8bit and 16bit display modules there are some 
  requirements you must adhere to. These requirements can be found 
  in the document UTFT_Requirements.pdf.
  There are no special requirements when using serial displays.
  You can always find the latest version of the library at 
  http://electronics.henningkarlsen.com/
  If you make any modifications or improvements to the code, I would 
  appreciate that you share the code with me so that I might include 
  it in the next release. I can be contacted through 
  http://electronics.henningkarlsen.com/contact.php.
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
  
  2/25/2013: Library heavily modified and tuned for speed by GizmoGarage.net
  http://gizmogarage.net/fast-avr-utft/
    
*/

#ifndef UTFT_h
#define UTFT_h

#include "config.h"

#define LEFT 0
#define RIGHT 9999
#define CENTER 9998

#define PORTRAIT 0
#define LANDSCAPE 1



//*********************************
// COLORS
//*********************************
// VGA color palette
#define VGA_BLACK		0x0000
#define VGA_WHITE		0xFFFF
#define VGA_RED			0xF800
#define VGA_GREEN		0x0400
#define VGA_BLUE		0x001F
#define VGA_SILVER		0xC618
#define VGA_GRAY		0x8410
#define VGA_MAROON		0x8000
#define VGA_YELLOW		0xFFE0
#define VGA_OLIVE		0x8400
#define VGA_LIME		0x07E0
#define VGA_AQUA		0x07FF
#define VGA_TEAL		0x0410
#define VGA_NAVY		0x0010
#define VGA_FUCHSIA		0xF81F
#define VGA_PURPLE		0x8010


	
#include <inttypes.h>
#include <math.h>
#include <util/delay.h>
#include <string.h>
	
#ifndef byte 
#define byte uint8_t
#endif
#ifndef word
#define word uint16_t
#endif
#define bitmapdatatype uint16_t*
	
#define delay _delay_ms
	

struct _current_font
{
	const uint8_t* font;
	uint8_t x_size;
	uint8_t y_size;
	uint8_t offset;
	uint8_t numchars;
};

class UTFT
{
	public:
		UTFT();
		
		void InitLCD(byte orientation=LANDSCAPE);
		
		void clrScr();
		void drawPixel(int x, int y);
		void drawLine(int x1, int y1, int x2, int y2);

		void fillScr(byte r, byte g, byte b);
		void fillScr(word color);
		void drawRect(int x1, int y1, int x2, int y2);
		void drawRoundRect(int x1, int y1, int x2, int y2);
		void fillRect(int x1, int y1, int x2, int y2);
		void fillRoundRect(int x1, int y1, int x2, int y2);
		void drawCircle(int x, int y, int radius);
		void fillCircle(int x, int y, int radius);
		void setColor(byte r, byte g, byte b);
		void setColor(word color);
		word getColor();
		void setBackColor(byte r, byte g, byte b);
		void setBackColor(word color);
		word getBackColor();
		void print(const char *st, int x, int y, int deg=0);
		//void print(String st, int x, int y, int deg=0);
		void printNumI(long num, int x, int y, int length=0, char filler=' ');
		void printNumF(double num, byte dec, int x, int y, char divider='.', int length=0, char filler=' ');
		void setFont(const uint8_t* font);
		const uint8_t* getFont();
		uint8_t getFontXsize();
		uint8_t getFontYsize();
		void drawBitmap(int x, int y, int sx, int sy, const bitmapdatatype data, int scale=1);
		void drawBitmapPB(int x, int y, int sx, int sy, const bitmapdatatype data);
		void drawBitmap(int x, int y, int sx, int sy, const bitmapdatatype data, int deg, int rox, int roy);
		void lcdOff();
		void lcdOn();
		void setContrast(char c);
		int  getDisplayXSize();
		int	 getDisplayYSize();
		int setPixels( word color, uint16_t x, uint16_t y, uint32_t pixels );
		
	protected:
		byte fch, fcl, bch, bcl;
		byte orient;
		long disp_x_size, disp_y_size;
		_current_font	cfont;
		
		void _hw_special_init();
		void setPixel(word color);
		void drawHLine(int x, int y, int l);
		void drawVLine(int x, int y, int l);
		void printChar(byte c, int x, int y);
		void printChar2(byte c, int x, int y);
		void rotateChar(byte c, int x, int y, int pos, int deg);
		void setXY(word x1, word y1, word x2, word y2);
		void clrXY();

};

#endif