#define  F_CPU 7372800UL
#include <avr/io.h>
#include <stdbool.h>
#include <util/delay.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include "UTFT/UTFT.h"
#include "UTFT/color.h"
#include "UTFT/had.c"
#include "UTFT/DefaultFonts.h"
#include "SPI_Master_H_file.h"

// NOTE - lijeva strana ploce rezervirana za dodatne dijelove - povratak, br. poteza i vrijeme
// Memory ploca
#define BOARD_X1 79
#define BOARD_Y1 0
#define BOARD_X2 318
#define BOARD_Y2 239

#define BORDER_L 136       // lijevi rub lijeve vertikalne linije
#define BORDER_C 197       // lijevi rub srednje (center) vertikalne linije
#define BORDER_R 258       // lijevi desne vertikalne linije
#define BORDER_T 56        // gornji rub gornje horizontalne linije
#define BORDER_M 117	   // gornji rub srednje (middle) horizontalne linije
#define BORDER_B 178       // gornji rub donje horizontalne linije

//#define FIELD_WIDTH 56   // ne koristimo, ali je tu da se na sirina polja
#define BORDER_WIDTH 5

// koordinate karata
// lijeva strana u svakom stupcu
#define X1_1 79
#define X1_2 140
#define X1_3 201
#define X1_4 262
// desna strana u svakom stupcu
#define X2_1 135
#define X2_2 196
#define X2_3 257
#define X2_4 318
// vrh u svakom retku
#define Y1_1 0
#define Y1_2 61
#define Y1_3 122
#define Y1_4 183
// dno u svakom retku
#define Y2_1 56
#define Y2_2 117
#define Y2_3 178
#define Y2_4 239

// gumbi za end game
// gumb za povratak
#define BACK_X1 0
#define BACK_Y1 0
#define BACK_X2 40
#define BACK_Y2 40
#define BACK_TEXT_X 12
#define BACK_TEXT_Y 12

// vertokalne koordinte za gumbe (svi osim BACK imaju iste)
#define BUTTONS_Y1 150
#define BUTTONS_Y2 230
#define BUTTONS_TEXT_Y 180

// gumb za izlazak iz igre nakon 1 runde
#define EXIT_X1 10
#define EXIT_X2 160
#define EXIT_TEXT_X 60

// gumb za nastavak igre
#define NEXT_X1 161
#define NEXT_X2 310
#define NEXT_TEXT_X 200

// gumb za "opusteni" nacin igre
#define CASUAL_X1 10
#define CASUAL_X2 100
#define CASUAL_TEXT_X 35

// gumb za "izazovni" nacin igre
#define CHALLENGE_X1 110
#define CHALLENGE_X2 200
#define CHALLENGE_TEXT_X 120

// gumb za resetiranje highscores
#define RESET_X1 220
#define RESET_X2 310
#define RESET_TEXT_X 245

UTFT display;

bool Touched() {
	// PINB3(T-IRQ) nizak pri dodiru
	if (bit_is_clear(PINB, T_IRQ)){
		return true;
		} else {
		return false;
	}
}

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

// ===== Memory varijable =====
uint8_t board[16] = {0};                  // ploca za igru
bool control[16] = {0};                   // kontrolno polje, jesu li karte okrenute
uint8_t c1 = 0, c2 = 0;                   // varijable koje pamte indeks okrenutih karti
uint8_t state = 0;                        // stanje igre 0 - nema okrenutih karti, 1 - jedna okrenuta karta, 2 - dvije okrenute karte - provjera jesu li iste
uint8_t matched = 0;                      // zavrsava se igra kad dostigne 8	
uint8_t moveCounter = 0;                  // brojac poteza
uint8_t bestMoves = 255;				  // najbolji rezultat poteza - najmanje pokusaja, postavljamo ga u najvecu mogucu vrijednost
uint8_t currentTime[3] = {0};             // ima polja za [0] minute, [1] sekunde i [2] stotinke, igra od 16 polja nece trajati vise od 1h
uint8_t bestTime[3] = {255, 255, 255};    // najbolje (najbrze) vrijeme, pocetno se postavlja u najvecu vrijednost
bool started = 0;                         // ako nismo u igri koristi se za inicijalizaciju, ali i pauzira timer
uint8_t roundCounter = 0;                 // brojac koliko krugova igre smo odigrali                         TODO - resetira se kad se ide na glavni ekran (back button?)        
uint8_t roundStreak = 0;                  // najduze odigrana igra u rundama, povecava se na kraju igre
bool menu = 0;                         // 0 - main menu, 1 - in game


void printTime() {
	display.printNumI(currentTime[0], 0, 160);
	display.print("m", 30, 160);
	display.printNumI(currentTime[1], 0, 180);
	display.print("s", 30, 180);
}

// broji stotinke, sekunde i minute provedene u igri
ISR(TIMER0_COMP_vect) {
	if (started) {  //osigurava da se ne broji dok ne igramo
		currentTime[2]++;

		if (currentTime[2] == 100) {
			currentTime[2] = 0;

			currentTime[1]++; 
			if (currentTime[1] == 60) {
				currentTime[1] = 0;
				currentTime[0]++;
			}
			if (currentTime[0] == 60) {
				currentTime[0] = 0;
			}
			printTime();
		}
	}
}

// popuni plocu nasumicno simbolima, koristimo obicne brojeve zbog jednostavnosti implementacije
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

// vraca indeks + 1 karte na koju smo kliknuli, 0 je za kad kliknemo izvan ploce
uint8_t memoryGetInput() {
	while(!Touched());
	uint16_t x = getX();
	uint16_t y = getY();
	
	if ((x > BACK_X1) && (x < BACK_X2) && (y > BACK_Y1) && (y < BACK_Y2)) return 20;
	else if ((x > X1_1) && (x < X2_1) && (y > Y1_1) && (y < Y2_1)) return 1;
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

// "pokriva" kartu crtanjem crnog kvadrata preko simbola
void closeCard(uint8_t index) {
	display.setColor(0, 0, 0);
	switch(index) {
		case 1:
			display.fillRect(X1_1 + 2, Y1_1 + 1, X2_1 - 1, Y2_1 - 1);
			break;
		case 2:
			display.fillRect(X1_2 + 2, Y1_1 + 1, X2_2 - 1, Y2_1 - 1);
			break;
		case 3:
			display.fillRect(X1_3 + 2, Y1_1 + 1, X2_3 - 1, Y2_1 - 1);
			break;
		case 4:
			display.fillRect(X1_4 + 2, Y1_1 + 1, X2_4 - 1, Y2_1 - 1);
			break;
		case 5:
			display.fillRect(X1_1 + 2, Y1_2 + 1, X2_1 - 1, Y2_2 - 1);
			break;
		case 6:
			display.fillRect(X1_2 + 2, Y1_2 + 1, X2_2 - 1, Y2_2 - 1);
			break;
		case 7:
			display.fillRect(X1_3 + 2, Y1_2 + 1, X2_3 - 1, Y2_2 - 1);
			break;
		case 8:
			display.fillRect(X1_4 + 2, Y1_2 + 1, X2_4 - 1, Y2_2 - 1);
			break;	
		case 9:
			display.fillRect(X1_1 + 2, Y1_3 + 1, X2_1 - 1, Y2_3 - 1);
			break;
		case 10:
			display.fillRect(X1_2 + 2, Y1_3 + 1, X2_2 - 1, Y2_3 - 1);
			break;
		case 11:
			display.fillRect(X1_3 + 2, Y1_3 + 1, X2_3 - 1, Y2_3 - 1);
			break;
		case 12:
			display.fillRect(X1_4 + 2, Y1_3 + 1, X2_4 - 1, Y2_3 - 1);
			break;
		case 13:
			display.fillRect(X1_1 + 2, Y1_4 + 1, X2_1 - 1, Y2_4 - 1);
			break;
		case 14:
			display.fillRect(X1_2 + 2, Y1_4 + 1, X2_2 - 1, Y2_4 - 1);
			break;
		case 15:
			display.fillRect(X1_3 + 2, Y1_4 + 1, X2_3 - 1, Y2_4 - 1);
			break;
		case 16:
			display.fillRect(X1_4 + 2, Y1_4 + 1, X2_4 - 1, Y2_4 - 1);
			break;
	}
}

// provjera ako je memory karta vec okrenuta
void checkOpen() {
	if (board[c1 - 1] != board[c2 - 1] && control[c1 - 1] == 0 && control[c2 - 1] == 0) {  //ako su karte razlicite i kontrolno polje je 0 za obje
		closeCard(c1);
		closeCard(c2);  //TEMP
		display.setColor(255, 255, 255);
	} else { //ako su isti okrenuti, postavi kontrolno polje u 1 i povecaj broj pogodjenih parova
		control[c1-1] = 1;
		control[c2-1] = 1;
		matched++;
	}
	state = c1 = c2 = 0;
	moveCounter++;
}

// otkrivanje karte - crtanje simbola iz polja na plocu
void revealCard(uint8_t input) {
	uint16_t x = 0, y = 0;
	
	if (input % 4 == 1) {                 // prvi stupac
		x = X1_1;
	} else if (input % 4 == 2) {      // drugi stupac
		x = X1_2;
	} else if (input % 4 == 3) {      // treci stupac
		x = X1_3;
	} else if (input % 4 == 0) {      // cetvrti stupac
		x = X1_4;
	}
	
	if ((input > 0) && (input < 5)) {                   // prvi red
		y = Y1_1;
	} else if ((input > 4) && (input < 9)) {        // drugi stupac
		y = Y1_2;
	} else if ((input > 8) && (input < 13)) {       // treci stupac
		y = Y1_3;
	} else if ((input > 12) && (input < 17)) {      // cetvrti stupac
		y = Y1_4;
	}
	
	uint8_t openSymbol = board[input - 1];
	
	display.printNumI(openSymbol, x + 20, y + 20);
	
	_delay_ms(200);                                     // Debounce cekanjem
}

// inicijalno stanje memory igre - generiranje sadrzaja polja i crtanje ploce
void memoryInit() {
	fillBoard();
	
	display.clrScr();
	
	// gumb za povratak
	display.drawRect(BACK_X1, BACK_Y1, BACK_X2, BACK_Y2);
	display.print("<", BACK_TEXT_X, BACK_TEXT_Y);
	
	display.print("Moves", 0, 60); // labela za broj poteza
	display.print("Time", 0, 140); // labela za vrijeme
	
	//crtanje ploce
	display.fillRect(BORDER_L, BOARD_Y1, BORDER_L + BORDER_WIDTH, BOARD_Y2);
	display.fillRect(BORDER_C, BOARD_Y1, BORDER_C + BORDER_WIDTH, BOARD_Y2);
	display.fillRect(BORDER_R, BOARD_Y1, BORDER_R + BORDER_WIDTH, BOARD_Y2);
	display.fillRect(BOARD_X1, BORDER_T, BOARD_X2, BORDER_T + BORDER_WIDTH);
	display.fillRect(BOARD_X1, BORDER_M, BOARD_X2, BORDER_M + BORDER_WIDTH);
	display.fillRect(BOARD_X1, BORDER_B, BOARD_X2, BORDER_B + BORDER_WIDTH);
}

// postavlja varijable u pocetne vrijednosti (ne highscores, ne broj rundi)
void memoryResetVariables() {
	  for (uint8_t i = 0; i < 16; i++) {
		  board[i] = 0;
		  control[i] = 0;
	  }
	  c1 = c2 = 0;
	  state = 0;
	  matched = 0;
	  moveCounter = 0;
	  currentTime[0] = currentTime[1] = currentTime[2] = 0;
}

/* TODO - CALL FROM MAIN MENU
void resetHighscores() {
	bestMoves = 255;
	bestTime[0] = bestTime[1] = bestTime[2] = 255;
	roundStreak = 0;
}
*/

// iscrtava ekran za kraj igre, povecava broj runde, stavlja igru u not-started stanje, resetira varijable
void memoryEndGame() {
	roundCounter++;
	started = 0;
		
	if (moveCounter < bestMoves) bestMoves = moveCounter;
	
	if (currentTime[0] <= bestTime[0]) {
		if (currentTime[1] <= bestTime[1]) {
			if (currentTime[0] < bestTime[0]) {
				bestTime[0] = currentTime[0];
				bestTime[1] = currentTime[1];
				bestTime[2] = currentTime[2];
			}
		}
	}
	
	if (roundCounter > roundStreak) roundStreak = roundCounter;
	
	display.clrScr();
	
	display.print("Round:", 100, 10);
	display.printNumI(roundCounter, 200, 10);
	
	display.print("Moves:", 90, 40);
	display.printNumI(moveCounter, 190, 40);
	display.print("Best moves:", 40, 60);
	display.printNumI(bestMoves, 240, 60);
		
	display.print("Time:", 60, 90);
	display.printNumI(currentTime[0], 140, 90);
	display.print(":", 170, 90);
	display.printNumI(currentTime[1], 180, 90);
	display.print(":", 210, 90);
	display.printNumI(currentTime[2], 220, 90);
		
	display.print("Best time:", 20, 110);
	display.printNumI(bestTime[0], 180, 110);
	display.print(":", 210, 110);
	display.printNumI(bestTime[1], 220, 110);
	display.print(":", 250, 110);
	display.printNumI(bestTime[2], 260, 110);
;
	display.drawRect(EXIT_X1, BUTTONS_Y1, EXIT_X2, BUTTONS_Y2);
	display.print("EXIT", EXIT_TEXT_X, BUTTONS_TEXT_Y);
	display.drawRect(NEXT_X1, BUTTONS_Y1, NEXT_X2, BUTTONS_Y2);
	display.print("NEXT", NEXT_TEXT_X, BUTTONS_TEXT_Y);
		
	memoryResetVariables();	  
}

uint8_t endGameGetInput() {
	while(!Touched());
	uint16_t x = getX();
	uint16_t y = getY();
	
	if ((x > EXIT_X1) && (x < EXIT_X2) && (y > BUTTONS_Y1) && (y < BUTTONS_Y2)) return 1;
	else if ((x > NEXT_X1) && (x < NEXT_X2) && (y > BUTTONS_Y1) && (y < BUTTONS_Y2)) return 2;
	else return 0;
}

void startGame() {
	display.clrScr();
	display.print("Tap to start", CENTER, 120);	
	
	_delay_ms(200); //debounce cekanjem
	while(!Touched());
	uint16_t x = getX();
	uint16_t y = getY();
	srand(x + y);          // sluzi kao workaroun za vrijeme - generira random seed na temelju gdje smo dodirnuli
}

// glavni game loop
void memoryGame() {
	uint16_t input;
	
	while(1) {
		if (state == 2) {
			checkOpen();
			display.printNumI(moveCounter, 0, 80);
		}
		if (matched == 8) {
			memoryEndGame();
			
			do {
				input = endGameGetInput();
				if (input == 1) {
					menu = 0;
					return;
				}
			} while (input == 0);
		}
		
		input = memoryGetInput();
		
		if (!started) { //inicijalizira stanje igre pri prvom pokretanju			
			startGame();
			memoryInit();
			started = 1;
		} else if (input == 20) {
			menu = 0;
			started = 0;
			memoryResetVariables();
			return;
		} else if (input > 0 && started && !(control[input-1])) { // ako igra vec traje, pritisnut je ekran i karta nije vec pogodjena, otvori kartu
			revealCard(input);
			
			if (state == 0 && control[input-1] == 0) { // nisu trenutno otvorene karte, ne smijemo otvarati vec otvorenu kartu
				c1 = input;
			    if (control[c1-1] == 0) state = 1;  // promjena stanja jedino ako je u c1 spremljena karta koja nije pogodjena
			} else if (state == 1 && control[c1-1] == 0) { // otvorena jedna karta, ne smijemo otvarati vec otvorenu kartu
				c2 = input;
				if (c1 != c2 && control[c2-1] == 0) state = 2; // promjena stanja jedino ako je u c1 spremljena karta koja nije pogodjena i c1 i c2 su razliciti
			}
		}			 
		_delay_ms(500);
	}
}

void printMenu() {
	display.clrScr();
	
	display.print("Memory", CENTER, 10);
	
	display.print("Best moves: ", 40, 40); 
	if (bestMoves < 255) display.printNumI(bestMoves, 240, 40); // prikazi samo ako postoji high score
					
	display.print("Best time:", 20, 60);
	if (bestTime[0] < 255 && bestTime[1] < 255 && bestTime[2] < 255) {
		display.printNumI(bestTime[0], 180, 60);
		display.print(":", 210, 60);
		display.printNumI(bestTime[1], 220, 60);
		display.print(":", 250, 60);
		display.printNumI(bestTime[2], 260, 60);
	}		
	
	display.print("Longest streak:", 10 , 80);
	display.printNumI(roundStreak, 250, 80);
	
	display.setFont(SmallFont);
	
    display.drawRect(CASUAL_X1, BUTTONS_Y1, CASUAL_X2, BUTTONS_Y2);
	display.print("CASUAL", CASUAL_TEXT_X, BUTTONS_TEXT_Y);
	
	display.drawRect(CHALLENGE_X1, BUTTONS_Y1, CHALLENGE_X2, BUTTONS_Y2);
	display.print("CHALLENGE", CHALLENGE_TEXT_X, BUTTONS_TEXT_Y);
	
	display.drawRect(RESET_X1, BUTTONS_Y1, RESET_X2, BUTTONS_Y2);
	display.print("RESET", RESET_TEXT_X, BUTTONS_TEXT_Y);	
	
	display.setFont(BigFont);
}

/*
uint8_t menuGetInput() {	
	while(!Touched()); 
	uint16_t x = getX();      //TODO ADD OPTIONS
	uint16_t y = getY();      //TODO ADD OPTIONS
	
	if ((x > CASUAL_X1) && (x < CASUAL_X2) && (y > BUTTONS_Y1) && (y < BUTTONS_Y2)) return 1;
	else if ((x > CHALLENGE_X2) && (x < CHALLENGE_X2) && (y > BUTTONS_Y1) && (y < BUTTONS_Y2)) return 2;
	else if ((x > RESET_X2) && (x < RESET_X2) && (y > BUTTONS_Y1) && (y < BUTTONS_Y2)) return 3;
	else return 0;
} 
*/

int main(void) {
	
	//T-IRQ spojen na PINB3 kao ulazni te je nizak samo pri dodiru, inace visok
	DDRB &= ~_BV(T_IRQ);
	
	// timer za brojanje vremena
	TCCR0 = _BV(WGM01) | _BV(CS02) | _BV(CS00);
	OCR0 = 72;
	TIMSK = _BV(OCIE0);
	sei();

	SPI_Init();
	SS_Enable;
	
	display.InitLCD(LANDSCAPE);
	display.clrScr();
	display.setFont(BigFont);
	
	while (1) {		
		if (menu == 0) {
			roundCounter = 0;                                     //resetira broj rundi ako smo na main menu
			printMenu();
			_delay_ms(100);
			while(!Touched());                      // TODO remove when game modes are implemented
			//menuGetInput();                       // TODO normal game mode, challenge game mode or reset highscores
			menu = 1;
		} else if (menu == 1) {
			memoryGame();
		}
	}
}
