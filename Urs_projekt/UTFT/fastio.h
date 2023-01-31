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



#ifndef FASTIO_H_
#define FASTIO_H_

#include <stdint.h>

void LCD_Writ_Bus(char VH,char VL, uint8_t mode) asm("LCD_Writ_Bus");
void LCD_Write_COM(uint8_t vl) asm("LCD_Write_COM");
void LCD_Write_DATA(uint8_t vh, uint8_t vl) asm("LCD_Write_DATA");
void LCD_Write_COM_DATA(uint8_t com, uint16_t v) asm("LCD_Write_COM_DATA");


#endif /* FASTIO_H_ */