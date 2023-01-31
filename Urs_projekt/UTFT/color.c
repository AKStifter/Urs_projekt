/*
 * color.c
 *
 * GNU GPL v3
 * 
 */ 

#include "color.h"
#include <stdlib.h>
#define ABITS 4
#define HSCALE 256

#define max(a,b) \
({ __typeof__ (a) _a = (a); \
__typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })
	 #define min(a,b) \
	 ({ __typeof__ (a) _a = (a); \
	 __typeof__ (b) _b = (b); \
 _a < _b ? _a : _b; })


col64bit rgba2hsva(const col32bit c){
	col64bit r;
	uint32_t iMin,iMax,chroma;
	const uint32_t k1=255 << ABITS;
	const uint32_t k2=HSCALE << ABITS;
	
	// there's no need to touch alpha, it remains 8-bit
	r.a=c.a;

	if (c.r > c.g) {
		iMax = max (c.r, c.b);
		iMin = min (c.g, c.b);
	} else {
		iMax = max (c.g, c.b);
		iMin = min (c.r, c.b);
	}

	chroma = iMax - iMin;
	// set value
	r.v = iMax << ABITS;

	// set saturation
	if (r.v == 0)
	r.s = 0;
	else
	r.s = (k1*chroma)/iMax;

	// set hue
	if (r.s == 0)
	r.h = 0;
	else {
		if ( c.r == iMax ) {
			r.h  =  (k2*(6*chroma+c.g - c.b))/(6*chroma);
			if (r.h >= k2) r.h -= k2;
		} else if (c.g  == iMax)
		r.h  =  (k2*(2*chroma+c.b - c.r )) / (6*chroma);
		else // (c.b == iMax )
		r.h  =  (k2*(4*chroma+c.r - c.g )) / (6*chroma);
	}
	return r;
}

col32bit hsva2rgba(const col64bit c2){
	col32bit r;
	uint32_t m;
	int32_t H,X,ih,is,iv;
	const uint32_t k1=255<<ABITS;
	const int32_t k2=HSCALE<<ABITS;
	const int32_t k3=1<<(ABITS-1);
	
	col64bit c = c2;

	// there's no need to touch alpha, it remains 8-bit
	r.a=c.a;

	// set chroma and min component value m
	//chroma = ( c.v * c.s )/k1;
	//m = c.v - chroma;
	m = ((uint32_t)c.v*(k1 - (uint32_t )c.s ))/k1;

	// chroma  == 0 <-> c.s == 0 --> m=c.v
	if (c.s == 0) {
		r.b = ( r.g = ( r.r = c.v >> ABITS ));
	} else {
		ih=(int)c.h;
		is=(int)c.s;
		iv=(int)c.v;

		H = (6*ih)/k2;
		X = ((iv*is)/k2)*(k2-abs(6*ih- 2*(H>>1)*k2 - k2)) ;

		// removing additional bits --> unit8
		X=( (X+iv*(k1 - is ))/k1 + k3 ) >> ABITS;
		m=m >> ABITS;

		// ( chroma + m ) --> c.v ;
		c.v=c.v >> ABITS;
		switch (H) {
			case 0:
			r.r = c.v;
			r.g = X;
			r.b = m;
			break;
			case 1:
			r.r = X;
			r.g = c.v;
			r.b = m;
			break;
			case 2:
			r.r = m;
			r.g = c.v;
			r.b = X;
			break;
			case 3:
			r.r = m;
			r.g = X;
			r.b = c.v;
			break;
			case 4:
			r.r = X;
			r.g = m;
			r.b = c.v;
			break;
			case 5:
			r.r = c.v;
			r.g = m ;
			r.b = X;
			break;
		}
	}
	return r;
}