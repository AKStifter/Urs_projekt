#include <avr/io.h>

#include "UTFT/UTFT.h"
#include "UTFT/color.h"
#include "UTFT/had.c"
#include "UTFT/DefaultFonts.h"

int main(void) {
	
	UTFT display;
	display.InitLCD(LANDSCAPE);
	display.setFont(BigFont);
	display.clrScr();
	
	//krug
	display.setColor(VGA_PURPLE);
	display.fillCircle(100,70,50);
	
	//pravokutnik
	display.setColor(255, 125, 0);
	display.fillRect(110, 140, 220, 200);
	
	display.setBackColor(255, 125, 0);
	display.setColor(0, 0, 0);
	display.print("Radi!", 125, 160,0);
	
	while (1) {
		
	}
}
