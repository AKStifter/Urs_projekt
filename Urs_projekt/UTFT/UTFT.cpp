/*
  UTFT.cpp - Arduino/chipKit library support for Color TFT LCD Boards
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
*/

#include <avr/pgmspace.h>

#include "UTFT.h"
#include "ssd1289.h"

#include <stdlib.h>

#define swap(type, i, j) {type t = i; i = j; j = t;}
#define fontbyte(x) pgm_read_byte(&cfont.font[x])  


UTFT::UTFT()
{ 
disp_x_size=239;
disp_y_size=319;
SSD1289_init();	
}

void UTFT::InitLCD(byte orientation)
{
	orient=orientation;
	setColor(255, 255, 255);
	setBackColor(0, 0, 0);
	cfont.font=0;	
}

void UTFT::setXY(word x1, word y1, word x2, word y2)
{
	if (orient==LANDSCAPE)
	{
		swap(word, x1, y1);
		swap(word, x2, y2)
		y1=disp_y_size-y1;
		y2=disp_y_size-y2;
		swap(word, y1, y2)
	}

	SSD1289_setXY(x1,y1,x2,y2,x1,y1);
	SSD1289_dataFollows();
}

void UTFT::clrXY()
{
	if (orient==PORTRAIT)
		setXY(0,0,disp_x_size,disp_y_size);
	else
		setXY(0,0,disp_y_size,disp_x_size);
}

void UTFT::drawRect(int x1, int y1, int x2, int y2)
{
	

	if (x1>x2)
	{
		swap(int, x1, x2);
	}
	if (y1>y2)
	{
		swap(int, y1, y2);
	}

	drawHLine(x1, y1, x2-x1);
	drawHLine(x1, y2, x2-x1);
	drawVLine(x1, y1, y2-y1);
	drawVLine(x2, y1, y2-y1);
}

void UTFT::drawRoundRect(int x1, int y1, int x2, int y2)
{
	if (x1>x2)
	{
		swap(int, x1, x2);
	}
	if (y1>y2)
	{
		swap(int, y1, y2);
	}
	if ((x2-x1)>4 && (y2-y1)>4)
	{
		drawPixel(x1+1,y1+1);
		drawPixel(x2-1,y1+1);
		drawPixel(x1+1,y2-1);
		drawPixel(x2-1,y2-1);
		drawHLine(x1+2, y1, x2-x1-4);
		drawHLine(x1+2, y2, x2-x1-4);
		drawVLine(x1, y1+2, y2-y1-4);
		drawVLine(x2, y1+2, y2-y1-4);
	}
}

void UTFT::fillRect(int x1, int y1, int x2, int y2)
{
	if (x1>x2)
	{
		swap(int, x1, x2);
	}
	if (y1>y2)
	{
		swap(int, y1, y2);
	}

	cbi(CS_PORT, CS_PIN);
	setXY(x1, y1, x2, y2);

	_fast_fill_16(fch,fcl,((long(x2-x1)+1)*(long(y2-y1)+1)));

	cbi(CS_PORT, CS_PIN);
	
}

void UTFT::fillRoundRect(int x1, int y1, int x2, int y2)
{
	if (x1>x2)
	{
		swap(int, x1, x2);
	}
	if (y1>y2)
	{
		swap(int, y1, y2);
	}

	if ((x2-x1)>4 && (y2-y1)>4)
	{
		for (int i=0; i<((y2-y1)/2)+1; i++)
		{
			switch(i)
			{
			case 0:
				drawHLine(x1+2, y1+i, x2-x1-4);
				drawHLine(x1+2, y2-i, x2-x1-4);
				break;
			case 1:
				drawHLine(x1+1, y1+i, x2-x1-2);
				drawHLine(x1+1, y2-i, x2-x1-2);
				break;
			default:
				drawHLine(x1, y1+i, x2-x1);
				drawHLine(x1, y2-i, x2-x1);
			}
		}
	}
}

void UTFT::drawCircle(int x, int y, int radius)
{
	int f = 1 - radius;
	int ddF_x = 1;
	int ddF_y = -2 * radius;
	int x1 = 0;
	int y1 = radius;
 	
	cbi(CS_PORT, CS_PIN);
	setXY(x, y + radius, x, y + radius);
	LCD_Write_DATA(fch,fcl);
	setXY(x, y - radius, x, y - radius);
	LCD_Write_DATA(fch,fcl);
	setXY(x + radius, y, x + radius, y);
	LCD_Write_DATA(fch,fcl);
	setXY(x - radius, y, x - radius, y);
	LCD_Write_DATA(fch,fcl);
 
	while(x1 < y1)
	{
		if(f >= 0) 
		{
			y1--;
			ddF_y += 2;
			f += ddF_y;
		}
		x1++;
		ddF_x += 2;
		f += ddF_x;    
		setXY(x + x1, y + y1, x + x1, y + y1);
		LCD_Write_DATA(fch,fcl);
		setXY(x - x1, y + y1, x - x1, y + y1);
		LCD_Write_DATA(fch,fcl);
		setXY(x + x1, y - y1, x + x1, y - y1);
		LCD_Write_DATA(fch,fcl);
		setXY(x - x1, y - y1, x - x1, y - y1);
		LCD_Write_DATA(fch,fcl);
		setXY(x + y1, y + x1, x + y1, y + x1);
		LCD_Write_DATA(fch,fcl);
		setXY(x - y1, y + x1, x - y1, y + x1);
		LCD_Write_DATA(fch,fcl);
		setXY(x + y1, y - x1, x + y1, y - x1);
		LCD_Write_DATA(fch,fcl);
		setXY(x - y1, y - x1, x - y1, y - x1);
		LCD_Write_DATA(fch,fcl);
	}
	sbi(CS_PORT, CS_PIN);
	clrXY();
}

void UTFT::fillCircle(int x, int y, int radius)
{
	for(int y1=-radius; y1<=0; y1++) 
		for(int x1=-radius; x1<=0; x1++)
			if(x1*x1+y1*y1 <= radius*radius) 
			{
				drawHLine(x+x1, y+y1, 2*(-x1));
				drawHLine(x+x1, y-y1, 2*(-x1));
				break;
			}
}

void UTFT::clrScr()
{
		
	cbi(CS_PORT, CS_PIN);


	SSD1289_setXY(0,0, 239, 319, 0, 0);
	SSD1289_reg11(0x6040 |ID1|ID0|AM);
	SSD1289_dataFollows();

	DATA_PORT_LOW = 0x00;
	DATA_PORT_HIGH = 0;
	fastfill((uint32_t)76800);
	DATA_PORT_LOW = 0xff;
	DATA_PORT_HIGH = 0xff;

	cbi(CS_PORT, CS_PIN);
}

void UTFT::fillScr(byte r, byte g, byte b)
{
	word color = ((r&248)<<8 | (g&252)<<3 | (b&248)>>3);
	fillScr(color);
}


int UTFT::setPixels( word color, uint16_t x, uint16_t y, uint32_t pixels )
{

	char ch, cl;
	
	ch=color>>8;
	cl=color & 0xFF;

	cbi(CS_PORT, CS_PIN);
	clrXY();
	_fast_fill_16(ch,cl,pixels);
	sbi(CS_PORT, CS_PIN);
	return 0;
}

void UTFT::fillScr(word color)
{

	unsigned char ch, cl;
	
	ch=color>>8;
	cl=color & 0xFF;
	
	
	cbi(CS_PORT, CS_PIN);
	
	SSD1289_setXY(0,0, 239, 319, 0, 0);
	SSD1289_reg11(0x6040 |ID1|ID0|AM);
	SSD1289_dataFollows();
	
	DATA_PORT_LOW = cl;
	DATA_PORT_HIGH = ch;
	fastfill((uint32_t)320*240);
	DATA_PORT_LOW = 0xff;
	DATA_PORT_HIGH = 0xff;	
	
	sbi(CS_PORT, CS_PIN);
}

void UTFT::setColor(byte r, byte g, byte b)
{
	fch=((r&248)|g>>5);
	fcl=((g&28)<<3|b>>3);
}

void UTFT::setColor(word color)
{
	fch=color>>8;
	fcl=color & 0xFF;
}

word UTFT::getColor()
{
	return (fch<<8) | fcl;
}

void UTFT::setBackColor(byte r, byte g, byte b)
{
	bch=((r&248)|g>>5);
	bcl=((g&28)<<3|b>>3);
}

void UTFT::setBackColor(word color)
{
	bch=color>>8;
	bcl=color & 0xFF;
}

word UTFT::getBackColor()
{
	return (bch<<8) | bcl;
}

void UTFT::setPixel(word color)
{
	LCD_Write_DATA((color>>8),(color&0xFF));	// rrrrrggggggbbbbb
}

void UTFT::drawPixel(int x, int y)
{
	cbi(CS_PORT, CS_PIN);
	setXY(x, y, x, y);
	setPixel((fch<<8)|fcl);	
	sbi(CS_PORT, CS_PIN);
}
		
void UTFT::drawLine(int xa, int ya, int xb, int yb)
{
	if (ya==yb)
	{
		drawHLine(xa, ya, xb-xa);
		return;
	}			
	if (xa==xb)
	{
		drawVLine(xa, ya, yb-ya);
		return;
	}
	
	if (orient==LANDSCAPE)
	{
		swap(uint16_t, xa, ya);
		swap(uint16_t, xb, yb);
		ya=disp_y_size-ya;
		yb=disp_y_size-yb;
	}		


	uint16_t dx = abs(xb - xa);
	uint16_t dy = abs(yb - ya);
	
	cbi(CS_PORT, CS_PIN);	
	SSD1289_setXY(0,0, 239, 319, xa, ya);
	
	if ( dx < dy ) 
	{
		if ( ya < yb )
		{			
			if ( xa < xb ) 
			{
				SSD1289_reg11(0x6040 |ID1|ID0|AM);
				SSD1289_dataFollows();

				fastlineAPP(fcl | fch << 8, xa, xb, ya, yb, dx, dy);
			}			
			else
			{
				SSD1289_reg11(0x6040 |ID1|ID0|AM);
				SSD1289_dataFollows();

				fastlineANP(fcl | fch << 8, xa, xb, ya, yb, dx, dy);				
			}
		}
		else
		{
			if ( xa < xb )
			{
				SSD1289_reg11(0x6040|ID0|AM);
				SSD1289_dataFollows();
											
				fastlineAPN(fcl | fch << 8, xa, xb, ya, yb, dx, dy);
			}
			else
			{
				SSD1289_reg11(0x6040|ID0|AM);
				SSD1289_dataFollows();
								
				fastlineANN(fcl | fch << 8, xa, xb, ya, yb, dx, dy);							
			}			
		}			
	}
	else
	{
		
		if ( ya < yb )
		{
			if ( xa < xb )
			{
				SSD1289_reg11(0x6040 |ID1|ID0);
				SSD1289_dataFollows();
		
				fastlineBPP(fcl | fch << 8, xa, xb, ya, yb, dx, dy);
			}
			else
			{
				SSD1289_reg11(0x6040 |ID1);
				SSD1289_dataFollows();
				
				fastlineBNP(fcl | fch << 8, xa, xb, ya, yb, dx, dy);							
			}				
		}	
		else
		{
			if ( xa < xb )
			{
				SSD1289_reg11(0x6040 |ID0);
				SSD1289_dataFollows();
				
				fastlineBPN(fcl | fch << 8, xa, xb, ya, yb, dx, dy);
			}
			else
			{
				
				SSD1289_reg11(0x6040 |ID1);
				SSD1289_dataFollows();
				
				fastlineBNN(fcl | fch << 8, xa, xb, ya, yb, dx, dy);
			}			
		}				
		
	}
	
	sbi(CS_PORT, CS_PIN);
	
}	


void UTFT::drawHLine(int x, int y, int l)
{
	if (l<0)
	{
		l = -l;
		x -= l;
	}
	
	cbi(CS_PORT, CS_PIN);
	
	setXY(x, y, x+l, y);
	_fast_fill_16(fch,fcl,l);
	sbi(CS_PORT, CS_PIN);
}

void UTFT::drawVLine(int x, int y, int l)
{
	if (l<0)
	{
		l = -l;
		y -= l;
	}
	cbi(CS_PORT, CS_PIN);
	setXY(x, y, x, y+l);
	_fast_fill_16(fch,fcl,l);
	sbi(CS_PORT, CS_PIN);	

}


void UTFT::printChar(byte c, int x, int y)
{

	word temp;

	cbi(CS_PORT, CS_PIN);
	
	if (orient==PORTRAIT)
	{
		
		temp=((c-cfont.offset)*((cfont.x_size/8)*cfont.y_size))+4;

		uint16_t x1 = x;
		uint16_t x2 = x + cfont.x_size -1;
		uint16_t y1 = y;
		uint16_t y2 = y+cfont.y_size-1;
		

		SSD1289_setXY(x1,y1, x2, y2, x1, y1);
		SSD1289_reg11(0x6040 |ID0|ID1);
		SSD1289_dataFollows();		
		
		fastbitmap_1bit(cfont.y_size * cfont.x_size/8, (uint16_t)cfont.font + temp,(fch<<8)|fcl,(bch<<8)|bcl);
	}
	else
	{
		temp=((c-cfont.offset)*((cfont.x_size/8)*cfont.y_size))+4;

		uint16_t x1 = x;
		uint16_t x2 = x + cfont.x_size -1;			
		uint16_t y1 = y;
		uint16_t y2 = y+cfont.y_size-1;
			
			
		swap(uint16_t, x1, y1);
		swap(uint16_t, x2, y2)
		y1=disp_y_size-y1;
		y2=disp_y_size-y2;
		swap(uint16_t, y1, y2)			
			
		SSD1289_setXY(x1,y1, x2, y2, x1, y2);
		SSD1289_reg11(0x6040 |ID0|AM);
		SSD1289_dataFollows();			
					
		fastbitmap_1bit(cfont.y_size * cfont.x_size/8, (uint16_t)cfont.font + temp,(fch<<8)|fcl,(bch<<8)|bcl);

		
	}
	sbi(CS_PORT, CS_PIN);
	
}

void UTFT::rotateChar(byte c, int x, int y, int pos, int deg)
{
	byte i,j,ch;
	word temp; 
	int newx,newy;
	double radian;
	radian=deg*0.0175;  

	cbi(CS_PORT, CS_PIN);

	temp=((c-cfont.offset)*((cfont.x_size/8)*cfont.y_size))+4;
	for(j=0;j<cfont.y_size;j++) 
	{
		for (int zz=0; zz<(cfont.x_size/8); zz++)
		{
			ch=pgm_read_byte(&cfont.font[temp+zz]); 
			for(i=0;i<8;i++)
			{   
				newx=x+(((i+(zz*8)+(pos*cfont.x_size))*cos(radian))-((j)*sin(radian)));
				newy=y+(((j)*cos(radian))+((i+(zz*8)+(pos*cfont.x_size))*sin(radian)));

				setXY(newx,newy,newx+1,newy+1);
				
				if((ch&(1<<(7-i)))!=0)   
				{
					setPixel((fch<<8)|fcl);
				} 
				else  
				{
					setPixel((bch<<8)|bcl);
				}   
			}
		}
		temp+=(cfont.x_size/8);
	}
	sbi(CS_PORT, CS_PIN);
}

void UTFT::print(const char *st, int x, int y, int deg)
{
	int stl, i;

	stl = strlen(st);

	if (orient==PORTRAIT)
	{
	if (x==RIGHT)
		x=(disp_x_size+1)-(stl*cfont.x_size);
	if (x==CENTER)
		x=((disp_x_size+1)-(stl*cfont.x_size))/2;
	}
	else
	{
	if (x==RIGHT)
		x=(disp_y_size+1)-(stl*cfont.x_size);
	if (x==CENTER)
		x=((disp_y_size+1)-(stl*cfont.x_size))/2;
	}

	for (i=0; i<stl; i++)
		if (deg==0)
			printChar(*st++, x + (i*(cfont.x_size)), y);
		else
			rotateChar(*st++, x, y, i, deg);
}


void UTFT::printNumI(long num, int x, int y, int length, char filler)
{
	char buf[25];
	char st[27];
	bool neg=false;
	int c=0, f=0;
  
	if (num==0)
	{
		if (length!=0)
		{
			for (c=0; c<(length-1); c++)
				st[c]=filler;
			st[c]=48;
			st[c+1]=0;
		}
		else
		{
			st[0]=48;
			st[1]=0;
		}
	}
	else
	{
		if (num<0)
		{
			neg=true;
			num=-num;
		}
	  
		while (num>0)
		{
			buf[c]=48+(num % 10);
			c++;
			num=(num-(num % 10))/10;
		}
		buf[c]=0;
	  
		if (neg)
		{
			st[0]=45;
		}
	  
		if (length>(c+neg))
		{
			for (int i=0; i<(length-c-neg); i++)
			{
				st[i+neg]=filler;
				f++;
			}
		}

		for (int i=0; i<c; i++)
		{
			st[i+neg+f]=buf[c-i-1];
		}
		st[c+neg+f]=0;

	}

	print(st,x,y);
}

void UTFT::printNumF(double num, byte dec, int x, int y, char divider, int length, char filler)
{
	char buf[25];
	char st[27];
	bool neg=false;
	int c=0, f=0;
	int c2;
	long mult;
	unsigned long inum;
  
	if (dec<1)
	dec=1;
	if (dec>5)
	dec=5;
	  
	if (num==0)
	{
		if (length!=0)
		{
			for (c=0; c<(length-2-dec); c++)
				st[c]=filler;
			st[c]=48;
			st[c+1]=divider;
			for (int i=0; i<dec; i++)
				st[c+2+i]=48;
			st[c+2+dec]=0;
		}
		else
		{
			st[0]=48;
			st[1]=divider;
			for (int i=0; i<dec; i++)
				st[2+i]=48;
			st[2+dec]=0;
		}
	}
	else
	{
		if (num<0)
		{
			neg=true;
			num=-num;
		}
	  
		mult = 1;
		for (int j = 0; j < dec; j++)
			mult = mult * 10;
		inum=long(num*mult + 0.5);
	  
		while (inum>0)
		{
			buf[c]=48+(inum % 10);
			c++;
			inum=(inum-(inum % 10))/10;
		}
		if ((num<1) and (num>0))
		{
			buf[c]=48;
			c++;
		}
		while (c<(dec+1))
		{
			buf[c]=48;
			c++;
		}
		buf[c]=0;
	  
		if (neg)
		{
			st[0]=45;
		}
	  
		if (length>(c+neg-1))
		{
			for (int i=0; i<(length-c-neg-1); i++)
			{
				st[i+neg]=filler;
				f++;
			}
		}

		c2=neg;
		for (int i=0; i<c; i++)
		{
			st[c2+f]=buf[c-i-1];
			c2++;
			if ((c-(c2-neg))==dec)
			{
				st[c2+f]=divider;
				c2++;
			}
		}
		st[c2+f]=0;
	}

	print(st,x,y);
}

void UTFT::setFont(const uint8_t* font)
{
	cfont.font=font;
	cfont.x_size=fontbyte(0);
	cfont.y_size=fontbyte(1);
	cfont.offset=fontbyte(2);
	cfont.numchars=fontbyte(3);
}

const uint8_t* UTFT::getFont()
{
	return cfont.font;
}

uint8_t UTFT::getFontXsize()
{
	return cfont.x_size;
}

uint8_t UTFT::getFontYsize()
{
	return cfont.y_size;
}

void UTFT::drawBitmap(int x, int y, int sx, int sy, const bitmapdatatype data, int scale)
{
	unsigned int col;
	int tx, ty, tsx, tsy;

	if (scale==1)
	{
		if (orient==PORTRAIT)
		{
			cbi(CS_PORT, CS_PIN);
			
			SSD1289_setXY(x,y, x+sx-1, y+sy-1, x, y);
			SSD1289_reg11(0x6040 |ID0|ID1);
			SSD1289_dataFollows();

			fastbitmap_16bit(sx*sy, (const uint16_t)data);
			
			sbi(CS_PORT, CS_PIN);
		}
		else
		{
			cbi(CS_PORT, CS_PIN);
			
			swap(uint16_t, x, y);
			swap(uint16_t, sx,sy);			
			y=disp_y_size-y;			
						
			SSD1289_setXY( x, y - (sy -1),  ( x + sx - 1 ),y,x, y - (sy-1) );
			SSD1289_reg11(0x6040 |ID0|AM);
			SSD1289_dataFollows();

			fastbitmap_16bit(sx*sy, (const uint16_t)data);
			
			sbi(CS_PORT, CS_PIN);
		}
	}
	else
	{
		if (orient==PORTRAIT)
		{
			cbi(CS_PORT, CS_PIN);
			for (ty=0; ty<sy; ty++)
			{
				setXY(x, y+(ty*scale), x+((sx*scale)-1), y+(ty*scale)+scale);
				for (tsy=0; tsy<scale; tsy++)
					for (tx=0; tx<sx; tx++)
					{
						col=pgm_read_word(&data[(ty*sx)+tx]);
						for (tsx=0; tsx<scale; tsx++)
							LCD_Write_DATA(col>>8,col & 0xff);
					}
			}
			sbi(CS_PORT, CS_PIN);
		}
		else
		{
			cbi(CS_PORT, CS_PIN);
			for (ty=0; ty<sy; ty++)
			{
				for (tsy=0; tsy<scale; tsy++)
				{
					setXY(x, y+(ty*scale)+tsy, x+((sx*scale)-1), y+(ty*scale)+tsy);
					for (tx=sx; tx>=0; tx--)
					{
						col=pgm_read_word(&data[(ty*sx)+tx]);
						for (tsx=0; tsx<scale; tsx++)
							LCD_Write_DATA(col>>8,col & 0xff);
					}
				}
			}
			sbi(CS_PORT, CS_PIN);
		}
	}
}

void UTFT::drawBitmapPB(int x, int y, int sx, int sy, const bitmapdatatype data)
{
	//unsigned int col;
	//int tx, ty, tc, tsx, tsy;
	//byte r, g, b;


	if (orient==PORTRAIT)
	{
		cbi(CS_PORT, CS_PIN);
		
		SSD1289_setXY(x,y, x+sx-1, y+sy-1, x, y);
		SSD1289_reg11(0x6040 |ID0|ID1);
		SSD1289_dataFollows();

		fastbitmap_pb565((const uint16_t)data);
		
		sbi(CS_PORT, CS_PIN);
	}
	else
	{
		cbi(CS_PORT, CS_PIN);
		
		swap(uint16_t, x, y);
		swap(uint16_t, sx,sy);
		y=disp_y_size-y;
		
		SSD1289_setXY( x, y - (sy -1),  ( x + sx - 1 ),y,x, y - (sy-1) );
		SSD1289_reg11(0x6040 |ID0|AM);
		SSD1289_dataFollows();

		fastbitmap_pb565((const uint16_t)data);
		
		sbi(CS_PORT, CS_PIN);
	}

	
}

void UTFT::drawBitmap(int x, int y, int sx, int sy, const bitmapdatatype data, int deg, int rox, int roy)
{
	unsigned int col;
	int tx, ty, newx, newy;
	//byte r, g, b;
	double radian;
	radian=deg*0.0175;  

	if (deg==0)
		drawBitmap(x, y, sx, sy, data);
	else
	{
		cbi(CS_PORT, CS_PIN);
		for (ty=0; ty<sy; ty++)
			for (tx=0; tx<sx; tx++)
			{
				col=pgm_read_word(&data[(ty*sx)+tx]);

				newx=x+rox+(((tx-rox)*cos(radian))-((ty-roy)*sin(radian)));
				newy=y+roy+(((ty-roy)*cos(radian))+((tx-rox)*sin(radian)));

				setXY(newx, newy, newx, newy);
				LCD_Write_DATA(col>>8,col & 0xff);
			}
		sbi(CS_PORT, CS_PIN);
	}
}

void UTFT::lcdOff()
{
}

void UTFT::lcdOn()
{
}

void UTFT::setContrast(char c)
{
}

int UTFT::getDisplayXSize()
{
	if (orient==PORTRAIT)
		return disp_x_size+1;
	else
		return disp_y_size+1;
}

int UTFT::getDisplayYSize()
{
	if (orient==PORTRAIT)
		return disp_y_size+1;
	else
		return disp_x_size+1;
}