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


#ifndef AS_MACROS_H_
#define AS_MACROS_H_


/* Toggles WR line using CBI/SBI -> 4 clock cycles */
.macro TOGGLE_WR
	cbi _SFR_IO_ADDR(WR_PORT), WR_PIN
	sbi _SFR_IO_ADDR(WR_PORT), WR_PIN
.endm


/* 
	Toggles WR line using OUT -> 2 clock cycles 
	Note this only works when all other pins on this 
	port are constant during the line output. Otherwise use TOGGLE_WR
*/
.macro TOGGLE_WR_FAST value1, value2
	out _SFR_IO_ADDR(WR_PORT), \value1
	out _SFR_IO_ADDR(WR_PORT), \value2
.endm

.macro BUS_START_DATA
	sbi _SFR_IO_ADDR(RS_PORT), RS_PIN
.endm

.macro BUS_START_REG
	cbi _SFR_IO_ADDR(RS_PORT), RS_PIN
.endm

.macro BUS_REG a,b, tempreg
	BUS_START_REG
	
	ldi \tempreg,\a
	out _SFR_IO_ADDR(DATA_PORT_HIGH), \tempreg
	ldi \tempreg, \b
	out _SFR_IO_ADDR(DATA_PORT_LOW), \tempreg
	
	TOGGLE_WR
.endm

.macro BUS_REG2 reg
BUS_START_REG



TOGGLE_WR
.endm

.macro BUS_DATA a,b
	BUS_START_DATA

	out _SFR_IO_ADDR(DATA_PORT_HIGH), \a
	out _SFR_IO_ADDR(DATA_PORT_LOW), \b

	TOGGLE_WR
.endm

.macro ADD16 lowreg, hireg val
	subi \lowreg, lo8(-\val)
	sbci \hireg, hi8(-\val)
.endm

.macro SUB16 lowreg, hireg, val
	subi \lowreg, lo8(\val)
	sbci \hireg, hi8(\val)
.endm

.macro SUB32 r1,r2,r3,r4,val
	subi \r1, lo8(\val)
	sbci \r2, hi8(\val)
	sbci \r3, hlo8(\val)
	sbci \r4, hhi8(\val)
.endm



#endif /* AS_MACROS_H_ */