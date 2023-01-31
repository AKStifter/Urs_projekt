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





.altmacro 
.macro FAST_LINE_A xop, yop

/*
  
  Parameters:

  Color  = r24:r25
  xa     = r22:r23
  xb     = r20:r21
  ya     = r18:r19
  yb     = r16:r17
  dx     = r14:r15
  dy     = r12:r13
  
  Internal:
  xnum   = r26:r27
  x = xa = r22:r23
  WR_PORT_WR_PIN_SET = r30
  WR_PORT_WR_PIN_CLR = r31
  short temp used by macros r28
*/

local FAST_LINE_A_LOOP
local FAST_LINE_A_LOOP_INCREMENT
local FAST_LINE_A_DONE
push r28


/* this block sets up the TOGGLE_WR_FAST registers r30:r31 */
in r30, _SFR_IO_ADDR(WR_PORT)
mov r31, r30
set
bld r30,WR_PIN
clt
bld r31,WR_PIN

out DPLIO, r24			// output the color to data register
out DPHIO, r25			// output the color to data register

movw r26, r12			// xnum = dy

lsr r27					// xnum >> 1
ror r26					// 16bit op.

\yop r16,r17,1			// add 1 to yb to we draw a line to and including yb
rjmp FAST_LINE_A_LOOP	// skip initial increment and start loop.

FAST_LINE_A_LOOP_INCREMENT:
\yop r18,r19,1			// y inc or dec

FAST_LINE_A_LOOP:
cp r18, r16				// compare y == yb ? set Z flag
cpc r19, r17			//  16bit op.
breq FAST_LINE_A_DONE	// if Z == 0 jump to done.

TOGGLE_WR_FAST r31,r30	// place a pixel!

add r26, r14			// xnum += dx
adc r27, r15			// 16bit op

cp r26, r12				// if xnum > dy
cpc r27, r13			// 16bit op

brlo FAST_LINE_A_LOOP_INCREMENT	// if xnum < dy jump to FAST_LINE_A_LOOP_INCREMENT

sub r26, r12			// xnum -= dy
sbc r27, r13			// 16bit op

\xop r22,23,1			// x++
\yop r18,r19,1			// y++

BUS_REG 0, 0x4e, r28	// SET LCD horizontal pos to X (write reg)
BUS_DATA r23,r22		// (write x pos)



// unfortunately the LCD forgets it's autoincremented Ypos if the Xpos is written so we have to do this here as well.
BUS_REG 0,0x4f, r28		// SET LCD vertical pos to new Y (write reg)
BUS_DATA r19,r18		// (write y pos)

BUS_REG 0, 0x22, r28	// SET LCD to WRITE mode
BUS_START_DATA			// set LCD into data mode

out DPLIO, r24			// set the color
out DPHIO, r25			// set the color

rjmp FAST_LINE_A_LOOP	// jump to top of loop, note we skip increment as we already did that!

FAST_LINE_A_DONE:

pop r28


ret

.endm


.global fastlineAPP
.global fastlineAPN
.global fastlineANP
.global fastlineANN

fastlineAPP:
FAST_LINE_A ADD16 ADD16

fastlineAPN:
FAST_LINE_A ADD16 SUB16

fastlineANP:
FAST_LINE_A SUB16 ADD16

fastlineANN:
FAST_LINE_A SUB16 SUB16




.altmacro 
.macro FAST_LINE_B xop, yop
local FAST_LINE_B_LOOP
local FAST_LINE_B_LOOP_INCREMENT
local FAST_LINE_B_DONE
/*
  
  Parameters:

  Color  = r24:r25
  xa     = r22:r23
  xb     = r20:r21
  ya     = r18:r19
  yb     = r16:r17
  dx     = r14:r15
  dy     = r12:r13
  
  Internal:
  ynum   = r26:r27
  y = ya = r18:r19 
  WR_PORT_WR_PIN_SET = r30
  WR_PORT_WR_PIN_CLR = r31
  short temp used by macros r28
*/

push r28


/* this block sets up the TOGGLE_WR_FAST registers r30:r31 */
in r30, _SFR_IO_ADDR(WR_PORT)
mov r31, r30
set
bld r30,WR_PIN
clt
bld r31,WR_PIN

out DPLIO, r24			// output the color to data register
out DPHIO, r25			// output the color to data register

movw r26, r14			// ynum = dx

lsr r27					// ynum >> 1
ror r26					// 16bit op.

\xop r20,r21,1			// add 1 to xb to we draw a line to and including yb
rjmp FAST_LINE_B_LOOP	// skip initial increment and start loop.

FAST_LINE_B_LOOP_INCREMENT:
\xop r22,r23,1			// x++

FAST_LINE_B_LOOP:
cp r22, r20				// compare x == xb ? set Z flag
cpc r23, r21			//  16bit op.
breq FAST_LINE_B_DONE	// if Z == 0 jump to done.

TOGGLE_WR_FAST r31,r30	// place a pixel!

add r26, r12			// ynum += dy
adc r27, r13			// 16bit op

cp r26, r14				// if ynum > dx
cpc r27, r15			// 16bit op

brlo FAST_LINE_B_LOOP_INCREMENT	// if xnum < dy jump to FAST_LINE_A_LOOP_INCREMENT

sub r26, r14			// ynum -= dx
sbc r27, r15			// 16bit op

\xop r22,r23,1			// x++
\yop r18,r19,1			// y++

BUS_REG 0, 0x4e, r28	// SET LCD horizontal pos to X (write reg)
BUS_DATA r23,r22		// (write x pos)

// unfortunately the LCD forgets it's autoincremented Ypos if the Xpos is written so we have to do this here as well.
BUS_REG 0,0x4f, r28		// SET LCD vertical pos to new Y (write reg)
BUS_DATA r19,r18		// (write y pos)

BUS_REG 0, 0x22, r28	// SET LCD to WRITE mode
BUS_START_DATA			// set LCD into data mode

out DPLIO, r24			// set the color
out DPHIO, r25			// set the color

rjmp FAST_LINE_B_LOOP	// jump to top of loop, note we skip increment as we already did that!

FAST_LINE_B_DONE:

pop r28


ret

.endm

.global fastlineBPP
.global fastlineBPN
.global fastlineBNP
.global fastlineBNN

fastlineBPP:
FAST_LINE_B ADD16 ADD16

fastlineBPN:
FAST_LINE_B ADD16 SUB16

fastlineBNP:
FAST_LINE_B SUB16 ADD16

fastlineBNN:
FAST_LINE_B SUB16 SUB16