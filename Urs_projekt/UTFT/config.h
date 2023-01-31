/*
 * config.h
 *
 * Created: 2/19/2013 10:41:04 AM
 */ 


#ifndef __CONFIG_H__
#define __CONFIG_H__


#include <avr/io.h>


/* Configure your hardware pins here */
#define WR_PORT PORTA
#define WR_PIN	4
#define WR_PORT_DDR DDRA

#define DATA_PORT_LOW PORTD
#define DATA_PORT_HIGH PORTC
#define DATA_PORT_LOW_DDR DDRD
#define DATA_PORT_HIGH_DDR DDRC

#define RS_PORT PORTA
#define RS_PIN 2
#define RS_PORT_DDR DDRA

#define CS_PORT PORTA
#define CS_PIN 5
#define CS_PORT_DDR DDRA

#define RD_PORT PORTA
#define RD_PIN 3
#define RD_PORT_DDR DDRA

#define RESET_PORT PORTB
#define RESET_PIN 1
#define RESET_PORT_DDR DDRB

#define BACK_LIGHT_PORT PORTB
#define BACK_LIGHT_PIN 3
#define BACK_LIGHT_DDR DDRB

#define F_CPU 12000000

#define HAVE_STATUS_LED 1	
#define STATUS_LED_PORT PORTB
#define STATUS_LED_PIN PORTB0
#define STATUS_LED_DDR DDRB



/* No touch zone follows */	
#define DPLIO _SFR_IO_ADDR(DATA_PORT_LOW)
#define DPHIO _SFR_IO_ADDR(DATA_PORT_HIGH)

#ifdef HAVE_STATUS_LED

#define status_led_on()\
STATUS_LED_PORT&=~_BV(STATUS_LED_PIN);

#define status_led_off()\
STATUS_LED_PORT|=_BV(STATUS_LED_PIN);

#define status_led_toggle()\
STATUS_LED_PORT^=_BV(STATUS_LED_PIN);

#define status_led_init()\
STATUS_LED_DDR|=_BV(STATUS_LED_PIN);

#else

#define status_led_on() {}
#define status_led_off() {}
#define status_led_toggle() {}
#define status_led_init() {}

#endif


#endif /* __CONFIG_H__ */