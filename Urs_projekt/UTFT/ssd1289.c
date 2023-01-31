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

#include "config.h"
#include <util/delay.h>
#include "ssd1289.h"
#include "fastio.h"



void SSD1289_init()
{	
	DATA_PORT_LOW_DDR = 0xff;
	DATA_PORT_HIGH_DDR = 0xff;
			
	RS_PORT_DDR |= _BV( RS_PIN);
				
	WR_PORT_DDR |= _BV(WR_PIN);

	CS_PORT_DDR |= _BV(CS_PIN);
		
	RD_PORT_DDR |= _BV(RD_PIN);
	RD_PORT |= _BV(RD_PIN);
		
	RESET_PORT_DDR |= _BV(RESET_PIN);
		
	BACK_LIGHT_PORT |= _BV(BACK_LIGHT_PIN);
	
	BACK_LIGHT_DDR |= _BV(BACK_LIGHT_PIN);			
	
	sbi(RESET_PORT, RESET_PIN);
	_delay_ms(5);
	cbi(RESET_PORT, RESET_PIN);
	_delay_ms(15);
	sbi(RESET_PORT, RESET_PIN);
	_delay_ms(15);

	cbi(CS_PORT, CS_PIN);

	LCD_Write_COM_DATA(0x00,0x0001);
	LCD_Write_COM_DATA(0x03,0xA8A4);
	LCD_Write_COM_DATA(0x0C,0x0000);
	LCD_Write_COM_DATA(0x0D,0x080C);
	LCD_Write_COM_DATA(0x0E,0x2B00);
	LCD_Write_COM_DATA(0x1E,0x00B7);
	LCD_Write_COM_DATA(0x01,0x2B3F);
	LCD_Write_COM_DATA(0x02,0x0600);
	LCD_Write_COM_DATA(0x10,0x0000);
	LCD_Write_COM_DATA(0x11,0x6070);
	LCD_Write_COM_DATA(0x05,0x0000);
	LCD_Write_COM_DATA(0x06,0x0000);
	LCD_Write_COM_DATA(0x16,0xEF1C);
	LCD_Write_COM_DATA(0x17,0x0003);
	LCD_Write_COM_DATA(0x07,0x0233);
	LCD_Write_COM_DATA(0x0B,0x0000);
	LCD_Write_COM_DATA(0x0F,0x0000);
	LCD_Write_COM_DATA(0x41,0x0000);
	LCD_Write_COM_DATA(0x42,0x0000);
	LCD_Write_COM_DATA(0x48,0x0000);
	LCD_Write_COM_DATA(0x49,0x013F);
	LCD_Write_COM_DATA(0x4A,0x0000);
	LCD_Write_COM_DATA(0x4B,0x0000);
	LCD_Write_COM_DATA(0x44,0xEF00);
	LCD_Write_COM_DATA(0x45,0x0000);
	LCD_Write_COM_DATA(0x46,0x013F);
	LCD_Write_COM_DATA(0x30,0x0707);
	LCD_Write_COM_DATA(0x31,0x0204);
	LCD_Write_COM_DATA(0x32,0x0204);
	LCD_Write_COM_DATA(0x33,0x0502);
	LCD_Write_COM_DATA(0x34,0x0507);
	LCD_Write_COM_DATA(0x35,0x0204);
	LCD_Write_COM_DATA(0x36,0x0204);
	LCD_Write_COM_DATA(0x37,0x0502);
	LCD_Write_COM_DATA(0x3A,0x0302);
	LCD_Write_COM_DATA(0x3B,0x0302);
	LCD_Write_COM_DATA(0x23,0x0000);
	LCD_Write_COM_DATA(0x24,0x0000);
	LCD_Write_COM_DATA(0x25,0x8000);
	LCD_Write_COM_DATA(0x4f,0x0000);
	LCD_Write_COM_DATA(0x4e,0x0000);
	LCD_Write_COM(0x22);


	sbi(CS_PORT, CS_PIN);
	
}