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

// ===== VARIJABLE ZA GLAVNU FUNKCIJU IGRE =====
// memory ploca
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

//#define FIELD_WIDTH 56   // ne koristimo, ali je tu da se zna sirina polja
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

// ===== GUMBI ZA KRAJ IGRE =====
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

// ===== GUMBI ZA MAIN MENU =====
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

// ===== VARIJABLE ZA IZAZOVNI NACIN IGRE =====
#define CHALLENGE_LENGTH 30               // koliko rundi do pobjede, default 30
#define CHALLENGE_MOVES 45                // koliko poteza mozemo napraviti do gubljenja igre u prvoj rundi, default 45
#define CHALLENGE_TIME 120                // koliko vremena imamo za pobijediti level u prvoj rundi, default 120
#define CHALLENGE_STEP 3                  // za koliko sekundi se smanji vrijeme po rundi

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
bool menu = 0;                            // 0 - main menu, 1 - in game
bool challengeMode = 0;                   // 0 - normalni nacin igre, 1 - izazovni nacin igre
uint8_t challengeMoves = CHALLENGE_MOVES; // koliko poteza imamo po rundi, pocinjemo s max postavljenim u CHALLENGE_MOVES pa smanjujemo za 1
uint8_t challengeTime = CHALLENGE_TIME;   // koliko vremena imamo po rundi, pocinjemo s max postavljenim u CHALLENGE_TIME i smanjujemo za CHALLENGE_STEP
uint8_t challengeEndFlag = 0;             // ako smo u izazovnoj igri, provjerava kako je zavrsila: 0 - jos traje, 1 - pobjeda, 2 - ostali smo bez poteza, 3 - isteklo vrijeme
bool gameBeaten = 0;                      // jesmo li pobijedili igru u izazovnom nacinu


// ispisi vrijeme na ekran za vrijeme igre, u opustenom naciju se ispisuje proteklo vrijeme a u izazovnom preostalo
void printTime() {
	//ovaj dio sluzi za brisanje prijasnjeg ispisa
	display.setColor(0, 0, 0);
	display.fillRect(0,159,45,200);
	display.setColor(255, 255, 255);
	
	if (challengeMode) {
		display.printNumI((challengeTime - currentTime[1]) / 60, 0, 160);
		display.print("m", 30, 160);
		display.printNumI((challengeTime - currentTime[1]) % 60, 0, 180);
		display.print("s", 30, 180);
	} else {
		display.printNumI(currentTime[0], 0, 160);
		display.print("m", 30, 160);
		display.printNumI(currentTime[1], 0, 180);
		display.print("s", 30, 180);
	}
}

// broji stotinke, sekunde i minute provedene u igri, aktivira zastavicu za isteklo vrijeme
ISR(TIMER0_COMP_vect) {
	 //osigurava da se ne broji dok ne igramo i ako je isteklo vrijeme
	if (started && challengeEndFlag != 3) {                                     
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
		
		if (challengeMode) {
			if ((challengeTime - (currentTime[0] * 60) - currentTime[1]) <= 0) {
				challengeEndFlag = 3;
			}
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

// "pokriva" kartu crtanjem kvadrata preko simbola
void closeCard(uint8_t index) {
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
		display.setColor(0, 0, 0); //postavlja boju u crnu da de obrise broj
		closeCard(c1);
		closeCard(c2);  
		display.setColor(255, 255, 255);  // vraca boju u bijelu da se ostali dijelovi mogu ispisati
	} else { //ako su isti okrenuti, postavi kontrolno polje u 1 i povecaj broj pogodjenih parova
		control[c1-1] = 1;
		control[c2-1] = 1;
		matched++;
	}
}

// otkrivanje karte - crtanje simbola iz polja na plocu
void revealCard(uint8_t input) {
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
	
	if ((input > 0) && (input < 5)) {               // prvi red
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

// postavlja varijable u pocetne vrijednosti (ne highscores, ne broj rundi), odnosi se na varijable za trenutnu rundu
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

// resetira rekorde
void resetHighscores() {
	bestMoves = 255;
	bestTime[0] = bestTime[1] = bestTime[2] = 255;
	roundStreak = 0;
}

// crta ekran za kraj igre
void memoryEndGame() {	
	// provjerava ako su trenutne varijable bolje od rekorda pa iscrtava ekran za kraj igre
	if (moveCounter < bestMoves) bestMoves = moveCounter;
	
	if (currentTime[0] * 60 + currentTime[1] <= bestTime[0] * 60 + bestTime[1]) {
		bestTime[0] = currentTime[0];
		bestTime[1] = currentTime[1];
		bestTime[2] = currentTime[2];
	}
	
	if (roundCounter > roundStreak) roundStreak = roundCounter;
	
	display.clrScr();
	
	display.print("Round:", 100, 10);
	display.printNumI(roundCounter, 200, 10);
	
	display.print("Moves:", 90, 40);
	display.printNumI(moveCounter, 190, 40);
	display.print("Best moves:", 40, 60);
	display.printNumI(bestMoves, 240, 60);
		
	display.print("Time:", 70, 90);
	display.printNumI(currentTime[0], 150, 90);
	display.print(":", 180, 90);
	display.printNumI(currentTime[1], 190, 90);
		
	display.print("Best time:", 30, 110);
	display.printNumI(bestTime[0], 190, 110);
	display.print(":", 220, 110);
	display.printNumI(bestTime[1], 230, 110);
;
	display.drawRect(EXIT_X1, BUTTONS_Y1, EXIT_X2, BUTTONS_Y2);
	display.print("EXIT", EXIT_TEXT_X, BUTTONS_TEXT_Y);
	display.drawRect(NEXT_X1, BUTTONS_Y1, NEXT_X2, BUTTONS_Y2);
	display.print("NEXT", NEXT_TEXT_X, BUTTONS_TEXT_Y);	  
}

// odabir ako ce se nastaviti s igrom ili se vratiti na glavni izbornik
uint8_t endGameGetInput() {
	while(!Touched());
	uint16_t x = getX();
	uint16_t y = getY();
	
	if ((x > EXIT_X1) && (x < EXIT_X2) && (y > BUTTONS_Y1) && (y < BUTTONS_Y2)) return 1;
	else if ((x > NEXT_X1) && (x < NEXT_X2) && (y > BUTTONS_Y1) && (y < BUTTONS_Y2)) return 2;
	else return 0;
}


// start ekran, sluzi kao tranizicija i nasumicno generiranje ploce na temelju gdje dodirnemo
void startGame() {
	display.clrScr();
	display.print("Tap to start", CENTER, 120);	
	
	_delay_ms(200); //debounce cekanjem
	while(!Touched());
	uint16_t x = getX();
	uint16_t y = getY();
	srand(x + y);       
}

// ekran za kraj igre u izazovnom nacinu rada (pobjeda ili izgubljena igra), vraca se na glavni izbornik
void challengeGameOver() {
	started = 0; 
	
	display.clrScr();
	if (challengeEndFlag == 1) {
		display.print("VICTORY", CENTER, 80);
		gameBeaten = 1;
	}
	else if (challengeEndFlag == 2) {
		display.print("GAME OVER", CENTER, 80);
		display.print("out of moves", CENTER, 100);
	} else if (challengeEndFlag == 3) {
		display.print("GAME OVER", CENTER, 80);
		display.print("time ran out", CENTER, 100);
	}
	display.print("tap to return", CENTER, 160);
	display.print("to main menu", CENTER, 180);
	
	_delay_ms(100);
	while(!Touched());
	memoryResetVariables();
	challengeEndFlag = 0;
	menu = 0;
	return;
}

// glavni game loop
void memoryGame() {
	uint8_t input;
	
	while(1) {	
		// ako su otvorene dvije karte, provjeri ako su iste i povecaj broj poteza 	
		if (state == 2) {
			checkOpen();
			state = c1 = c2 = 0;
			moveCounter++;
			
			// ako smo u izazovnom nacinu, provjeri ako smo ostali bez poteza i zavrsi igru
			if (challengeMode) {
				if (challengeMoves - moveCounter <= 0) {
					challengeEndFlag = 2;
					challengeGameOver();
					return;
				}
			}
		}
		
		// ocisti prijasnji zapis poteza i ovisno o nacinu rada ispisi broj napravljenih ili preostalih poteza
		display.setColor(0, 0, 0);
		display.fillRect(0,75,45,100);
		display.setColor(255, 255, 255);
		if (challengeMode) display.printNumI(challengeMoves - moveCounter, 0, 80);
		else display.printNumI(moveCounter, 0, 80);
		
		// ako smo pogodili svih osam parova, povecaj broj rundi i iskljuci igru, ispisi ekran za kraj igre i resetiraj varijable za rundu
		if (matched == 8) {
			started = 0;
			roundCounter++;
			
			memoryEndGame();
			memoryResetVariables();	  
			
			// ako smo u izazovnom nacinu rada, smanji broj dozvoljenih poteza za 1 i dozvoljeno vrijeme za CHALLENGE_STEP, ako je broj runde jednak tajanju igre, izadji iz igre
			if (challengeMode) {
				challengeMoves--;
				challengeTime -= CHALLENGE_STEP;
				if (roundCounter == CHALLENGE_LENGTH) {
					challengeEndFlag = 1;
					challengeGameOver();
					return;
				}
			}
			
			// cekanje na ekranu za kraj igre, moze se vratiti na glavni izbornik ili ici u sljedecu rundu
			do {
				input = endGameGetInput();
				if (input == 1) {
					menu = 0;
					return;
				}
			} while (input == 0);
		}
		input = memoryGetInput();
		
		if (!started) {                                           //inicijalizira stanje igre pri prvom pokretanju			
			startGame();
			memoryInit();
			started = 1;
		} else if (input == 20) {                                 // povratak na glavni izbornik i resetiranje varijabli runde
			menu = 0;
			started = 0;
			memoryResetVariables();
			return;
		} else if (input > 0 && started && !(control[input-1])) { // ako igra vec traje, pritisnut je ekran i karta nije vec pogodjena, otvori kartu
			if (challengeMode && challengeEndFlag == 3) {         // ako smo u challenge mode i vrijeme je isteklo, izadji iz igre
				challengeGameOver();
				return;
			}
			
			revealCard(input);
			
			if (state == 0 && control[input-1] == 0) {            // nisu trenutno otvorene karte, ne smijemo otvarati vec pogodjenu kartu
				c1 = input;
			    if (control[c1-1] == 0) state = 1;                // promjena stanja jedino ako je u c1 spremljena karta koja nije pogodjena
			} else if (state == 1 && control[c1-1] == 0) {        // otvorena jedna karta, ne smijemo otvarati vec pogodjenu kartu
				c2 = input;
				if (c1 != c2 && control[c2-1] == 0) state = 2;    // promjena stanja jedino ako je u c1 spremljena karta koja nije pogodjena i c1 i c2 su razliciti
			}
		}	 
		_delay_ms(50);  //debounce cekanjem
	}
}

// crtanje glavnog izbornika
void printMenu() {
	display.clrScr();
	
	display.print("Memory", CENTER, 10);
	
	display.print("Best moves: ", 40, 40); 
	if (bestMoves < 255) display.printNumI(bestMoves, 220, 40);   // prikazi samo ako postoji high score
					
	display.print("Best time:", 30, 60);
	if (bestTime[0] < 255 && bestTime[1] < 255 && bestTime[2] < 255) {
		display.printNumI(bestTime[0], 190, 60);
		display.print(":", 220, 60);
		display.printNumI(bestTime[1], 230, 60);
	}		
	
	display.print("Longest streak:", 20 , 80);
	display.printNumI(roundStreak, 260, 80); 
	
	display.setFont(SmallFont);
	
	if (gameBeaten) display.print("CHALLENGE MODE BEATEN", 80, 120);
	
    display.drawRect(CASUAL_X1, BUTTONS_Y1, CASUAL_X2, BUTTONS_Y2);
	display.print("CASUAL", CASUAL_TEXT_X, BUTTONS_TEXT_Y);
	
	display.drawRect(CHALLENGE_X1, BUTTONS_Y1, CHALLENGE_X2, BUTTONS_Y2);
	display.print("CHALLENGE", CHALLENGE_TEXT_X, BUTTONS_TEXT_Y);
	
	display.drawRect(RESET_X1, BUTTONS_Y1, RESET_X2, BUTTONS_Y2);
	display.print("RESET", RESET_TEXT_X, BUTTONS_TEXT_Y);	
	
	display.setFont(BigFont);
}


uint8_t menuGetInput() {	
	while(!Touched()); 
	uint16_t x = getX();
	uint16_t y = getY();      
	
	if ((x > CASUAL_X1) && (x < CASUAL_X2) && (y > BUTTONS_Y1) && (y < BUTTONS_Y2)) return 1;
	else if ((x > CHALLENGE_X1) && (x < CHALLENGE_X2) && (y > BUTTONS_Y1) && (y < BUTTONS_Y2)) return 2;
	else if ((x > RESET_X1) && (x < RESET_X2) && (y > BUTTONS_Y1) && (y < BUTTONS_Y2)) return 3;
	else return 0;
} 


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
			roundCounter = 0;                                     // resetira broj rundi ako smo na main menu
			challengeMoves = CHALLENGE_MOVES;                     // resetira broj dozvoljenih poteza u izazovnom nacinu
			challengeTime = CHALLENGE_TIME;                       // resetira dozvoljeno vrijeme u izazovnom nacinu
			printMenu();
			_delay_ms(100);
			uint8_t input;                            
			do {
				input = menuGetInput();
				/*
				* 1 - "opusteni" nacin igre
				* 2 - "izazovni" nacin igre - ograniceno vrijeme i broj poteza
				* 3 - obrisi rekorde i ponovno ispisi glavni izbornik da se obrisu ispisani rekordi
				* 0 - ako ne dodirnemo niti jedan gumb
				*/
				if (input == 1) {
					challengeMode = 0;
				} else if (input == 2) {
					challengeMode = 1;
				} else if (input == 3) {
					resetHighscores();
					_delay_ms(100); //debounce cekanjem da nema efekta treperenja
					input = 0;
					printMenu();
				}
			} while (input == 0);  // uvijek naprije otvori glavni izbornik i cekaj dok ne pokrenemo igru
			menu = 1;
		} else if (menu == 1) {
			memoryGame();  //pokreni igru
		}
	}
}
