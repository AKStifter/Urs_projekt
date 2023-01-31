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
.macro PLOT_MONO_PIXEL reg, bit

local PMP_FG_COLOR
local PMP_BG_COLOR
local PMP_NEXT
	bst \reg, \bit
	brtc PMP_BG_COLOR
PMP_FG_COLOR:
	out DPLIO, r20
	out DPHIO, r21
	rjmp PMP_NEXT
PMP_BG_COLOR:
	out DPLIO, r18
	out DPHIO, r19
PMP_NEXT:
	TOGGLE_WR_FAST r27,r26	// place a pixel!
	
.endm


.global fastbitmap_1bit
fastbitmap_1bit:

	/*

		r24:r25 lenght
		r22:r23 pointer to data
		r20:r21 fg color 
		r18:r19 bg color
	*/

	/* this block sets up the TOGGLE_WR_FAST registers r30:r31 */
	in r26, _SFR_IO_ADDR(WR_PORT)
	mov r27, r26
	set
	bld r26,WR_PIN
	clt
	bld r27,WR_PIN
	
	movw r30, r22

FB1BIT_LOOP:

	LPM r0, Z+

	PLOT_MONO_PIXEL r0,7
	PLOT_MONO_PIXEL r0,6
	PLOT_MONO_PIXEL r0,5
	PLOT_MONO_PIXEL r0,4
	PLOT_MONO_PIXEL r0,3
	PLOT_MONO_PIXEL r0,2
	PLOT_MONO_PIXEL r0,1
	PLOT_MONO_PIXEL r0,0

	SUB16 r24,r25,1

	cpi r24,0
	cpc r25,r1
	breq FB1BIT_DONE
	jmp FB1BIT_LOOP


FB1BIT_DONE:

	ret;




.global fastbitmap_16bit
fastbitmap_16bit:

	/*
		r24:r25 lenght in pixels/words
		r22:r23 pointer to data
	*/

	/* this block sets up the TOGGLE_WR_FAST registers r30:r31 */
	in r26, _SFR_IO_ADDR(WR_PORT)
	mov r27, r26
	set
	bld r26,WR_PIN
	clt
	bld r27,WR_PIN
	
	movw r30, r22

FB16BIT_LOOP:

	
	LPM r0, Z+	
	out DPHIO, r0	
	LPM r0, Z+	
	out DPLIO, r0

	TOGGLE_WR_FAST r27,r26	// place a pixel!

	SUB16 r24,r25,1

	cpi r24,0
	cpc r25,r1
	breq FB16BIT_DONE
	jmp FB16BIT_LOOP


FB16BIT_DONE:

	ret;



.global fastbitmap_pb565
fastbitmap_pb565:

	/*
		r24:r25 data
		
	*/

	/* this block sets up the TOGGLE_WR_FAST registers r30:r31 */
	in r26, _SFR_IO_ADDR(WR_PORT)
	mov r27, r26
	set
	bld r26,WR_PIN
	clt
	bld r27,WR_PIN
	
	movw r30, r24

	clr r1

PB565BIT_LOOP:
	
	LPM r18, Z+	
	cpi r18,0
	breq PB565BIT_DONE
	
	bst r18,7
	brtc PB565PLAIN
	
	// compressed loop.
	andi r18,0x7F
	LPM r0, Z+
	out DPHIO, r0
	LPM r0, Z+
	out DPLIO, r0
	

PB565COMPRESSED:
	TOGGLE_WR_FAST r27,r26
	dec r18
	brne PB565COMPRESSED
	rjmp PB565BIT_LOOP


PB565PLAIN:

	LPM r0, Z+
	out DPHIO, r0
	LPM r0, Z+
	out DPLIO, r0
	TOGGLE_WR_FAST r27,r26
	dec r18
	brne PB565PLAIN
	rjmp PB565BIT_LOOP




PB565BIT_DONE:
	clr r0
	ret;