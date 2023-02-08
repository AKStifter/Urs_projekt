#define  F_CPU 7372800UL
#include <avr/io.h>
#include <stdbool.h>
#include <util/delay.h>
#include <stdlib.h>
#include "UTFT/UTFT.h"
#include "UTFT/color.h"
#include "UTFT/had.c"
#include "UTFT/DefaultFonts.h"
#include "SPI_Master_H_file.h"

//NOTE - lijeva strana ploce rezervirana za dodatne dijelove
// Memory ploca
#define BOARD_X1 39
#define BOARD_Y1 0
#define BOARD_X2 278
#define BOARD_Y2 239

#define BORDER_L 96       // lijevi rub lijeve vertikalne linije
#define BORDER_C 157       // lijevi rub srednje (center) vertikalne linije
#define BORDER_R 218       // lijevi desne vertikalne linije
#define BORDER_T 56        // gornji rub gornje horizontalne linije
#define BORDER_M 117	   // gornji rub srednje (middle) horizontalne linije
#define BORDER_B 178       // gornji rub donje horizontalne linije

//#define FIELD_WIDTH 56
#define BORDER_WIDTH 5

// "Koordinate" redova i stupaca
//lijeva strana u svakom stupcu
#define X1_1 39
#define X1_2 100
#define X1_3 161
#define X1_4 222
//desna strana u svakom stupcu
#define X2_1 95
#define X2_2 156
#define X2_3 217
#define X2_4 278
//vrh u svakom retku
#define Y1_1 0
#define Y1_2 61
#define Y1_3 122
#define Y1_4 183
//dno u svakom retku
#define Y2_1 56
#define Y2_2 117
#define Y2_3 178
#define Y2_4 239

UTFT display;

bool Touched() {
	// PINB3(T-IRQ) nizak pri dodiru
	if (bit_is_clear(PINB, T_IRQ)){
		return true;
		} else {
		return false;
	}
}

//TEST

uint16_t getX() {
	SPI_Write(0X90);
	float x = SPI_Read() / 120.0 * 320 - 15;
	SPI_Write(0);
	_delay_ms(10);
	return (uint16_t) x;
}

uint16_t getY() {
	float y = 0;
	while(y <= 0 ){
		SPI_Write(0XD0);
		y = SPI_Read() / 120.0 * 240;
		SPI_Write(0);
	}
	_delay_ms(10);
	return (uint16_t) y;
}

// Memory varijable
uint8_t board[16] = {0}; //ploca za igru
//bool control[16] = {0}; // TODO kontrolno polje, jesu li karte okrenute
//uint8_t c1, c2;  //TODO - provjerava ako su okrenute dvije iste karte
//uint8_t state = 0;  //TODO stanje igre 0 - nema okrenutih karti, 1 - jedna okrenuta karta, 2 - dvije okrenute karte - provjera jesu li iste
// uint8_t gameFinished = 0; // 0 - igra jos traje, 1 - pobjeda, 2 - vrijeme je isteklo, TODO ideja je da imamo timer

// Popuni plocu nasumicno simbolima
void fillBoard() {
	uint8_t index, i;
	for (i = 0; i < 16; i++) {
		index = rand() % 16;
		while(board[index] != 0) { //ako se izabere polje koje je vec popunjeno, biraj sljedece
			index++;
			if (index > 15) {
				index = 0;
			}
		}
		board[index] = (i % 8) + 1;
	}
}

// vraca indeks + 1 polja na koje smo kliknuli
uint8_t memoryGetInput() {
	while(!Touched());
	uint16_t x = getX();
	uint16_t y = getY();
	
	if ((x > X1_1) && (x < X2_1) && (y > Y1_1) && (y < Y2_1)) return 1;
	else if ((x > X1_2) && (x < X2_2) && (y > Y1_1) && (y < Y2_1)) return 2;
	else if ((x > X1_3) && (x < X2_3) && (y > Y1_1) && (y < Y2_1)) return 3;
	else if ((x > X1_4) && (x < X2_4) && (y > Y1_1) && (y < Y2_1)) return 4;
	
	else if ((x > X1_1) && (x < X2_1) && (y > Y1_2) && (y < Y2_2)) return 5;
	else if ((x > X1_2) && (x < X2_2) && (y > Y1_2) && (y < Y2_2)) return 6;
	else if ((x > X1_3) && (x < X2_3) && (y > Y1_2) && (y < Y2_2)) return 7;
	else if ((x > X1_4) && (x < X2_4) && (y > Y1_2) && (y < Y2_2)) return 8;
	
	else if ((x > X1_1) && (x < X2_1) && (y > Y1_3) && (y < Y2_3)) return 9;
	else if ((x > X1_2) && (x < X2_2) && (y > Y1_3) && (y < Y2_3)) return 10;
	else if ((x > X1_3) && (x < X2_3) && (y > Y1_3) && (y < Y2_3)) return 11;
	else if ((x > X1_4) && (x < X2_4) && (y > Y1_3) && (y < Y2_3)) return 12;
	
	else if ((x > X1_1) && (x < X2_1) && (y > Y1_4) && (y < Y2_4)) return 13;
	else if ((x > X1_2) && (x < X2_2) && (y > Y1_4) && (y < Y2_4)) return 14;
	else if ((x > X1_3) && (x < X2_3) && (y > Y1_4) && (y < Y2_4)) return 15;
	else if ((x > X1_4) && (x < X2_4) && (y > Y1_4) && (y < Y2_4)) return 16;
	else return 0;
}

//TODO provjeri ako je memory karta vec okrenuta
//bool memoryCheckOpen() {
//return 0;
//}

// otkrivanje karte - crtanje simbol iz polja na plocu
void memoryRevealCard(uint8_t input) {
	display.setColor(255, 255, 255);
	display.setFont(BigFont);
	
	uint16_t x = 0, y = 0;
	
	if (input % 4 == 1) {             // prvi stupac
		x = X1_1;
		} else if (input % 4 == 2) {      // drugi stupac
		x = X1_2;
		} else if (input % 4 == 3) {      // treci stupac
		x = X1_3;
		} else if (input % 4 == 0) {      // cetvrti stupac
		x = X1_4;
	}
	
	if ((input > 0) && (input < 5)) { // prvi red
		y = Y1_1;
		} else if ((input > 4) && (input < 9)) {      // drugi stupac
		y = Y1_2;
		} else if ((input > 8) && (input < 13)) {      // treci stupac
		y = Y1_3;
		} else if ((input > 12) && (input < 17)) {      // cetvrti stupac
		y = Y1_4;
	}
	
	uint8_t openSymbol = board[input - 1];
	
	display.printNumI(openSymbol, x + 20, y + 20);
	
	_delay_ms(200);     // Debounce cekanjem
}

// inicijalno stanje memory igre - generiranje sadrzaja polja i crtanje ploce
void memoryInit() {
	display.clrScr();
	fillBoard();
	
	//crtanje ploce za memory
	display.fillRect(BORDER_L, BOARD_Y1, BORDER_L + BORDER_WIDTH, BOARD_Y2);
	display.fillRect(BORDER_C, BOARD_Y1, BORDER_C + BORDER_WIDTH, BOARD_Y2);
	display.fillRect(BORDER_R, BOARD_Y1, BORDER_R + BORDER_WIDTH, BOARD_Y2);
	display.fillRect(BOARD_X1, BORDER_T, BOARD_X2, BORDER_T + BORDER_WIDTH);
	display.fillRect(BOARD_X1, BORDER_M, BOARD_X2, BORDER_M + BORDER_WIDTH);
	display.fillRect(BOARD_X1, BORDER_B, BOARD_X2, BORDER_B + BORDER_WIDTH);
}

//TODO - zavrsi igru kad su svi brojevi u kontrolnom polju 1
//void memoryEndGame() {}

// glavni game loop
void memoryGame() {
	uint16_t input;
	bool started = 0;
	
	while(1) {
		input = memoryGetInput(); //svakih pola sekunde provjeri input
		if (!started) { //inicijalizira stanje igre pri prvom pokretanju
			// sluzi kao workaroun za vrijeme - generira random seed na temelju gdje smo dodirnuli
			uint16_t x = getX();
			uint16_t y = getY();
			srand(x+y);
			memoryInit();
			started = 1;
			} else if (input > 0 && started) { //ako igra vec traje i pritisnut je ekran, otvori kartu
			memoryRevealCard(input);
		}
		_delay_ms(500);
	}
}

int main(void) {
	
	//T-IRQ spojen na PINB3 kao ulazni te je nizak samo pri dodiru, inace visok
	DDRB &= ~_BV(T_IRQ);

	SPI_Init();
	SS_Enable;
	
	display.InitLCD(LANDSCAPE);
	display.setFont(BigFont);
	display.clrScr();
	
	while (1) {
		memoryGame();
	}
}
