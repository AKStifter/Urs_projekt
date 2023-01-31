/*
  Copyright 2013(c) Ron Bessems All right reserved
  Latest version can be found at http://gizmogarage.net/fast-avr-utft/
 
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

#ifndef __SSD1289_H__
#define __SSD1289_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "config.h"
#include "fastio.h"	
#include "fastline.h"
#include "fastbitmap.h"
#include "fastfill.h"

void SSD1289_init();

#define sbi( port, pin ) ( port |= _BV(pin) )
#define cbi( port, pin ) ( port &= ~_BV(pin) )
#define pulse_high( port, pin ) ( sbi(port,pin); cbi(port,pin)) 
#define pulse_low( port, pin ) ( cbi(port,pin); sbi(port,pin))

#define _fast_fill_16( ch, cl, data )\
	DATA_PORT_HIGH = ch;\
	DATA_PORT_LOW = cl;\
	fastfill(data);	\
	DATA_PORT_LOW = 0xff;\
	DATA_PORT_HIGH = 0xff;\

#define SSD1289_setXY( x1, y1, x2, y2, x, y )\
	LCD_Write_COM_DATA(0x44,((x2)<<8)+x1);\
	LCD_Write_COM_DATA(0x45,y1);\
	LCD_Write_COM_DATA(0x46,y2);\
	LCD_Write_COM_DATA(0x4e,x);\
	LCD_Write_COM_DATA(0x4f,y);
	
#define SSD1289_dataFollows()\
	LCD_Write_COM(0x22);\
	sbi(RS_PORT, RS_PIN);
	
	
// Register 11 bits
#define ID0 16 
#define ID1 32
#define AM 8

#define SSD1289_reg11( v )\
	LCD_Write_COM_DATA(0x11, v);	
	
#ifdef __cplusplus
}
#endif

#endif //__SSD1289_H__