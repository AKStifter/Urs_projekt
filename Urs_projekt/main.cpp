#define  F_CPU 7372800UL
#include <avr/io.h>
#include <stdbool.h>
#include <util/delay.h>
#include "UTFT/UTFT.h"
#include "UTFT/color.h"
#include "UTFT/had.c"
#include "UTFT/DefaultFonts.h"
#include "SPI_Master_H_file.h"

bool Touched() {
	// PINB3(T-IRQ) nizak pri dodiru
	if (bit_is_clear(PINB, T_IRQ)){
		return true;
		} else {
		return false;
	}
}

int getX() {
	SPI_Write(0X90);
	float x = SPI_Read() / 120.0 * 320 - 15;
	SPI_Write(0);
	_delay_ms(10);
	return (int) x;
}

int getY() {
	float y = 0;
	while(y <= 0 ){
		SPI_Write(0XD0);
		y = SPI_Read() / 120.0 * 240;
		SPI_Write(0);
	}
	_delay_ms(10);
	
	return (int) y;
}


int main(void) {	
	UTFT display;
	display.InitLCD(LANDSCAPE);
	display.setFont(BigFont);
	display.clrScr();
	
	//T-IRQ spojen na PINB3 kao ulazni te je nizak samo pri dodiru, inace visok
	DDRB &= ~_BV(T_IRQ);

	SPI_Init();
	SS_Enable;
	
	/*//krug
	display.setColor(VGA_PURPLE);
	display.fillCircle(100,70,50);
	
	//pravokutnik
	display.setColor(255, 125, 0);
	display.fillRect(110, 140, 220, 200);
	
	display.setBackColor(255, 125, 0);
	display.setColor(0, 0, 0);
	display.print("Radi!", 125, 160,0);*/
	display.setColor(255, 125, 0);
	display.setBackColor(255, 125, 0);
	uint16_t x,y = 0;
	
	while (1) {
		if(Touched())
		{
			x = getX();
			y = getY();
			display.drawPixel(x, y);
		}
	}
}
