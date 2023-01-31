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


.global fastfill

// r22-r25 unsigned 32 bit number of pixels to set.
// note that only 17 bits of this are used, so max pixels
// is 131071, which is fine since we only have 320 * 240 pixels or 76800 loops
fastfill:

	clr r18

	// devide by 2
	// no need for register 25 it has no bits we care about.
	lsr r24
	ror r23
	ror r22
	ror r18 // take carry onto r18 (modulus)

	// devide by 4
	// no need for register 24, it had a max of 1 bit we care about.
	lsr r23
	ror r22
	ror r18 // take carry onto r18 (modulus)

	// devide by 8
	//lsr r24
	lsr r23
	ror r22
	ror r18 // take carry onto r18 (modulus)

	// devide by 16		
	// lsr r24
	lsr r23
	ror r22
	ror r18 // take carry onto r18 (modulus)

	// finish rolling a full 8 bits aka swap nibbles
	swap r18

	movw r24, r22
	


// r24 + r25 hold 16 bit unsigned number of 16xloops.
// r18 holds 8 bit unsigned numner of single loops
fastfill2:

	// prepare the out port registers
	in r30, _SFR_IO_ADDR(WR_PORT)
	mov r31, r30
	set
	bld r30,WR_PIN
	clt
	bld r31,WR_PIN

	sbiw r24,0 // subtract zero and test if zero	
	breq exitloop16
loop16:
		
	TOGGLE_WR_FAST r31,r30
	TOGGLE_WR_FAST r31,r30
	TOGGLE_WR_FAST r31,r30
	TOGGLE_WR_FAST r31,r30
		
	TOGGLE_WR_FAST r31,r30
	TOGGLE_WR_FAST r31,r30
	TOGGLE_WR_FAST r31,r30
	TOGGLE_WR_FAST r31,r30
		
	TOGGLE_WR_FAST r31,r30
	TOGGLE_WR_FAST r31,r30
	TOGGLE_WR_FAST r31,r30
	TOGGLE_WR_FAST r31,r30
		
	TOGGLE_WR_FAST r31,r30
	TOGGLE_WR_FAST r31,r30
	TOGGLE_WR_FAST r31,r30
	TOGGLE_WR_FAST r31,r30

	sbiw r24,1	
	brne loop16

exitloop16:

	cpi r18,0
	breq exitsingleloop
singleloop:			
	TOGGLE_WR_FAST r31,r30
	dec r18
	brne singleloop

exitsingleloop:
	ret 

	
.global fastfill3

// r22-r25 unsigned 32 bit number of pixels to set.
// note that only 17 bits of this are used, so max pixels
// is 131071, which is fine since we only have 320 * 240 pixels or 76800 loops
fastfill3:
	TOGGLE_WR
	SUB32 r22,r23,r24,r25,1
	brne fastfill3

	ret