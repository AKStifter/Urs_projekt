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


#include <avr/io.h>
#include "config.h"
#include "as_macros.h"


.global LCD_Write_COM
LCD_Write_COM:
	BUS_START_REG
	clr r1
	out DPLIO, r24
	out DPHIO, r1
	TOGGLE_WR
	ret

.global LCD_Write_DATA
LCD_Write_DATA:
	BUS_START_DATA	
	out DPLIO, r22
	out DPHIO, r24
	TOGGLE_WR
	ret

.global LCD_Write_DATA2
LCD_Write_DATA2:
	BUS_START_DATA	
	clr r1
	out DPLIO, r24
	out DPHIO, r1
	TOGGLE_WR
	ret


.global LCD_Write_COM_DATA
LCD_Write_COM_DATA:
	
	BUS_START_REG
	clr r1
	out DPLIO, r24
	out DPHIO, r1
	TOGGLE_WR

	BUS_START_DATA	
	out DPLIO, r22
	out DPHIO, r23
	TOGGLE_WR

	ret


.global LCD_Writ_Bus
	LCD_Writ_Bus:

	out DPHIO, r24
	out DPLIO, r22
	TOGGLE_WR

	ret