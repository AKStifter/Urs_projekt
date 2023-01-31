/*
 * color.h
 *
 * Created: 2/26/2013 11:28:59 AM
 *  Author: ron
 */ 

#ifndef __COLOR_H__
#define __COLOR_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef struct col32bit_s {
	uint8_t b;
	uint8_t g;
	uint8_t r;
	uint8_t a;
} col32bit;

typedef struct col64bit_s {
	uint16_t v;
	uint16_t s;
	uint16_t h;
	uint16_t a;
} col64bit;


col32bit hsva2rgba(const col64bit c2);
col64bit rgba2hsva(const col32bit c);

#ifdef __cplusplus
}
#endif


#endif // __COLOR_H__