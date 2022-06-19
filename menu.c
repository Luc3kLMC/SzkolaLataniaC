#include <ace/managers/viewport/simplebuffer.h>
#include <ace/managers/joy.h>
#include <ace/managers/key.h>
#include <ace/managers/system.h>
#include <ace/managers/game.h>
#include <ace/utils/palette.h>
#include <ace/utils/font.h>
#include <stdio.h>
#include <ace/managers/state.h>
#include <ace/managers/rand.h>
#include "structures.h"

#define PC_NO_PLAY = 3

static tView *s_pView;
static tVPort *s_pVp;
static tSimpleBufferManager *s_pVpManager;

static tFont *s_pFont;
static tTextBitMap *s_pBmText;

static tBitMap *s_pCard2;
static tBitMap *s_pCard3;
static tBitMap *s_pCard4;
static tBitMap *s_pCard5;
static tBitMap *s_pCard6;
static tBitMap *s_pCard7;
static tBitMap *s_pCard8;
static tBitMap *s_pCard9;
static tBitMap *s_pCard10;
static tBitMap *s_pCard11;
static tBitMap *s_pCard12;
static tBitMap *s_pCard13;

struct stack stack;
struct select select;
struct hand hand;
struct player p1;
struct player p2;

//extern tState g_sStateGame;
extern tStateManager *g_pStateMachineGame;

char s_pText[50];
const char tekstMenu[] = "Menu, shuflling. Press RETURN to start.";

#define NUMBER_OF_CARDS_IN_DECK 51
int ArrayOfCards[NUMBER_OF_CARDS_IN_DECK] = {2,2,2,2,2,3,3,3,3,3,
											 3,4,4,4,4,4,4,5,5,5,
											 5,5,5,6,6,6,6,6,6,7,
											 7,7,7,7,8,8,8,8,8,9,
											 9,9,10,10,10,11,11,12,12,13,
											 13};

//BOOL run = TRUE;
BOOL game = FALSE;
BOOL menu = TRUE;
BOOL gameover = FALSE;

BOOL printMenuOnce = FALSE;

UBYTE inDeckPosition = 0; // not removing elements from deck array, so will be counting each drawn card
					// and to 'remeber' which was drawn and move to next

BOOL wasKeyPressed = FALSE; // everytime when selecting card to play (or remove selection)
						//  display will be updated

void clearScreen (void){
	blitRect(s_pVpManager->pBack, 0, 0, 320, 128, 0);
    blitRect(s_pVpManager->pBack, 0, 128, 320, 128, 0);
}

void displayMessage(char *text, UBYTE dstY, UBYTE height){
	blitRect(s_pVpManager->pBack, 0, dstY, 320, height, 0);
	sprintf(s_pText, text);
  	fontFillTextBitMap(s_pFont, s_pBmText, s_pText);
  	fontDrawTextBitMap(s_pVpManager->pBack, s_pBmText, 2, dstY, 3, FONT_COOKIE);
}

void displayHUDinfo(){
	blitRect(s_pVpManager->pBack, 0, 0, 320, 32, 0);
	sprintf(s_pText, "Stack value: %d , Cards on stack: %d", stack.value, stack.count);
	fontFillTextBitMap(s_pFont, s_pBmText, s_pText);
	fontDrawTextBitMap(s_pVpManager->pBack, s_pBmText, 2, 4, 3, FONT_COOKIE);
	sprintf(s_pText, "P1 penalty: %d , P2 penalty: %d", p1.penaltyPoints, p2.penaltyPoints);
	fontFillTextBitMap(s_pFont, s_pBmText, s_pText);
	fontDrawTextBitMap(s_pVpManager->pBack, s_pBmText, 2, 20, 3, FONT_COOKIE);

}

void printMenuText(void){
	sprintf(s_pText, tekstMenu);
  	fontFillTextBitMap(s_pFont, s_pBmText, s_pText);
  	fontDrawTextBitMap(s_pVpManager->pBack, s_pBmText, 12, 12, 3, FONT_COOKIE);
}

void initialSetup(void){

	inDeckPosition = 0;
	printMenuOnce = FALSE;

	stack.value = 0;
	stack.count = 0;
	stack.cardsDisplay = 0;
	stack.stackDisplayX = 2;
	stack.stackDisplayY = 100;

	select.slot = 0;
	select.count = 0;

	hand.cardInHandDisplayX = 2;
	hand.selectedCardY = 190;
	hand.cardInHandDisplayY = 200;
	

	for (UBYTE i = 0; i < 6; ++i){
		p1.hand[i] = 0;
		p1.selectedToPlay[i] = 0; 
	}
	p1.penaltyPoints = 0;
	p1.PCturn = FALSE;

	// same as above for PC player
	for (UBYTE i = 0; i < 6; ++i){
		p2.hand[i] = 0;
		p2.selectedToPlay[i] = 0; 
	}
	p2.penaltyPoints = 0;
	p2.PCturn = FALSE;

}

void deckShuffle(void)
{	
  //for (UBYTE i = 0; i < NUMBER_OF_CARDS_IN_DECK; ++i)
  //{
  //  ArrayOfCards[i] = i;
  //}
  for (UBYTE i = 0; i < NUMBER_OF_CARDS_IN_DECK - 1; ++i)
  {
    size_t j = i + uwRandMinMax(0, NUMBER_OF_CARDS_IN_DECK) / (NUMBER_OF_CARDS_IN_DECK / (NUMBER_OF_CARDS_IN_DECK - i) + 1);
    int t = ArrayOfCards[j];
    ArrayOfCards[j] = ArrayOfCards[i];
    ArrayOfCards[i] = t;
  }
}

void drawStartingHand(void){
	for (UBYTE i = 0; i < 6; ++i ){
		p1.hand[i] = ArrayOfCards[inDeckPosition];
		++inDeckPosition;
		p2.hand[i] = ArrayOfCards[inDeckPosition];
		++inDeckPosition;
	}
}

void displayPlayerHand(void){
	for (UBYTE i = 0; i < 6; ++i){
		UBYTE whichCard = p1.hand[i];

		switch (whichCard){
			case 0:
				blitRect(s_pVpManager->pBack, i * 36, 238, 32, 16, 0);
			break;
			case 2:
				blitCopy(s_pCard2, 0,0, s_pVpManager->pBack, i * 36, hand.cardInHandDisplayY, 32, 48, MINTERM_COOKIE);
			break;

			case 3:
				blitCopy(s_pCard3, 0,0, s_pVpManager->pBack, i * 36, hand.cardInHandDisplayY, 32, 48, MINTERM_COOKIE);
			break;

			case 4:
				blitCopy(s_pCard4, 0,0, s_pVpManager->pBack, i * 36, hand.cardInHandDisplayY, 32, 48, MINTERM_COOKIE);
			break;

			case 5:
				blitCopy(s_pCard5, 0,0, s_pVpManager->pBack, i * 36, hand.cardInHandDisplayY, 32, 48, MINTERM_COOKIE);
			break;

			case 6:
				blitCopy(s_pCard6, 0,0, s_pVpManager->pBack, i * 36, hand.cardInHandDisplayY, 32, 48, MINTERM_COOKIE);
			break;

			case 7:
				blitCopy(s_pCard7, 0,0, s_pVpManager->pBack, i * 36, hand.cardInHandDisplayY, 32, 48, MINTERM_COOKIE);
			break;

			case 8:
				blitCopy(s_pCard8, 0,0, s_pVpManager->pBack, i * 36, hand.cardInHandDisplayY, 32, 48, MINTERM_COOKIE);
			break;

			case 9:
				blitCopy(s_pCard9, 0,0, s_pVpManager->pBack, i * 36, hand.cardInHandDisplayY, 32, 48, MINTERM_COOKIE);
			break;

			case 10:
				blitCopy(s_pCard10, 0,0, s_pVpManager->pBack, i * 36, hand.cardInHandDisplayY, 32, 48, MINTERM_COOKIE);
			break;

			case 11:
				blitCopy(s_pCard11, 0,0, s_pVpManager->pBack, i * 36, hand.cardInHandDisplayY, 32, 48, MINTERM_COOKIE);
			break;

			case 12:
				blitCopy(s_pCard12, 0,0, s_pVpManager->pBack, i * 36, hand.cardInHandDisplayY, 32, 48, MINTERM_COOKIE);
			break;

			case 13:
				blitCopy(s_pCard13, 0,0, s_pVpManager->pBack, i * 36, hand.cardInHandDisplayY, 32, 48, MINTERM_COOKIE);
			break;
		}
	}
}

void displaySelectedCards(void){
	for (UBYTE i = 0; i < 6 ; ++i){
		UBYTE selectedCard = p1.selectedToPlay[i];
		switch (selectedCard){
			case 0:
				blitRect(s_pVpManager->pBack, i * 36, 190, 32, 16, 0);
			break;
			case 2:
				blitCopy(s_pCard2, 0,0, s_pVpManager->pBack, i * 36, hand.selectedCardY, 32, 48, MINTERM_COOKIE);
			break;
			case 3:
				blitCopy(s_pCard3, 0,0, s_pVpManager->pBack, i * 36, hand.selectedCardY, 32, 48, MINTERM_COOKIE);
			break;
			case 4:
				blitCopy(s_pCard4, 0,0, s_pVpManager->pBack, i * 36, hand.selectedCardY, 32, 48, MINTERM_COOKIE);
			break;
			case 5:
				blitCopy(s_pCard5, 0,0, s_pVpManager->pBack, i * 36, hand.selectedCardY, 32, 48, MINTERM_COOKIE);
			break;
			case 6:
				blitCopy(s_pCard6, 0,0, s_pVpManager->pBack, i * 36, hand.selectedCardY, 32, 48, MINTERM_COOKIE);
			break;
			case 7:
				blitCopy(s_pCard7, 0,0, s_pVpManager->pBack, i * 36, hand.selectedCardY, 32, 48, MINTERM_COOKIE);
			break;
			case 8:
				blitCopy(s_pCard8, 0,0, s_pVpManager->pBack, i * 36, hand.selectedCardY, 32, 48, MINTERM_COOKIE);
			break;
			case 9:
				blitCopy(s_pCard9, 0,0, s_pVpManager->pBack, i * 36, hand.selectedCardY, 32, 48, MINTERM_COOKIE);
			break;
			case 10:
				blitCopy(s_pCard10, 0,0, s_pVpManager->pBack, i * 36, hand.selectedCardY, 32, 48, MINTERM_COOKIE);
			break;
			case 11:
				blitCopy(s_pCard11, 0,0, s_pVpManager->pBack, i * 36, hand.selectedCardY, 32, 48, MINTERM_COOKIE);
			break;
			case 12:
				blitCopy(s_pCard12, 0,0, s_pVpManager->pBack, i * 36, hand.selectedCardY, 32, 48, MINTERM_COOKIE);
			break;
			case 13:
				blitCopy(s_pCard13, 0,0, s_pVpManager->pBack, i * 36, hand.selectedCardY, 32, 48, MINTERM_COOKIE);
			break;

		}
	}
}

void selectCard(void){
	if (p1.selectedToPlay[select.slot - 1] == 0){
		p1.selectedToPlay[select.slot - 1] = p1.hand[select.slot - 1];
		p1.hand[select.slot - 1] = 0;
		select.count += 1;
	}
	else if (p1.selectedToPlay[select.slot - 1] != 0){
		p1.hand[select.slot - 1] = p1.selectedToPlay[select.slot - 1];
		p1.selectedToPlay[select.slot - 1] = 0;
		select.count -= 1;
	}
}

void clearStackDisplay(void){
	stack.stackDisplayX = 2;
	blitRect(s_pVpManager->pBack, 0, stack.stackDisplayY, 320, 48, 0);
}

void discardPlayedCardsAndDraw(){
	for (UBYTE i = 0; i < 6; ++i){
		p1.selectedToPlay[i] = 0;
		if (p1.hand[i] == 0 && inDeckPosition < 51){
			p1.hand[i] = ArrayOfCards[inDeckPosition];
			++inDeckPosition;
		}
	}
	displaySelectedCards();
	displayPlayerHand();	
}

void incorrectPlayHandReset(void){
	for (UBYTE i = 0; i < 6; ++i){
		if (p1.selectedToPlay[i] != 0){
			p1.hand[i] = p1.selectedToPlay[i];
			p1.selectedToPlay[i] = 0;
		}
	}
	displaySelectedCards();
	displayPlayerHand();
}

void displayCardsOnStack(){
	for (UBYTE i = 0; i < 6; ++i){
		UBYTE selectedCard = p1.selectedToPlay[i];

		if (selectedCard != 0){
			stack.stackDisplayX += 20;
		}

		switch (selectedCard){
			case 2:
				blitCopy(s_pCard2, 0,0, s_pVpManager->pBack, stack.stackDisplayX, stack.stackDisplayY, 32, 48, MINTERM_COOKIE);
			break;
			case 3:
				blitCopy(s_pCard3, 0,0, s_pVpManager->pBack, stack.stackDisplayX, stack.stackDisplayY, 32, 48, MINTERM_COOKIE);
			break;
			case 4:
				blitCopy(s_pCard4, 0,0, s_pVpManager->pBack, stack.stackDisplayX, stack.stackDisplayY, 32, 48, MINTERM_COOKIE);
			break;
			case 5:
				blitCopy(s_pCard5, 0,0, s_pVpManager->pBack, stack.stackDisplayX, stack.stackDisplayY, 32, 48, MINTERM_COOKIE);
			break;
			case 6:
				blitCopy(s_pCard6, 0,0, s_pVpManager->pBack, stack.stackDisplayX, stack.stackDisplayY, 32, 48, MINTERM_COOKIE);
			break;
			case 7:
				blitCopy(s_pCard7, 0,0, s_pVpManager->pBack, stack.stackDisplayX, stack.stackDisplayY, 32, 48, MINTERM_COOKIE);
			break;
			case 8:
				blitCopy(s_pCard8, 0,0, s_pVpManager->pBack, stack.stackDisplayX, stack.stackDisplayY, 32, 48, MINTERM_COOKIE);
			break;
			case 9:
				blitCopy(s_pCard9, 0,0, s_pVpManager->pBack, stack.stackDisplayX, stack.stackDisplayY, 32, 48, MINTERM_COOKIE);
			break;
			case 10:
				blitCopy(s_pCard10, 0,0, s_pVpManager->pBack, stack.stackDisplayX, stack.stackDisplayY, 32, 48, MINTERM_COOKIE);
			break;
			case 11:
				blitCopy(s_pCard11, 0,0, s_pVpManager->pBack, stack.stackDisplayX, stack.stackDisplayY, 32, 48, MINTERM_COOKIE);
			break;
			case 12:
				blitCopy(s_pCard12, 0,0, s_pVpManager->pBack, stack.stackDisplayX, stack.stackDisplayY, 32, 48, MINTERM_COOKIE);
			break;
			case 13:
				blitCopy(s_pCard13, 0,0, s_pVpManager->pBack, stack.stackDisplayX, stack.stackDisplayY, 32, 48, MINTERM_COOKIE);
			break;
		}
	}	
}

void checkWithStackValue(void){
	UBYTE playValue = 0;
	for (UBYTE i = 0; i < 6; ++i){
		playValue += p1.selectedToPlay[i];
	}
	if (playValue > stack.value){
		blitRect(s_pVpManager->pBack, 0, 48, 320, 16, 0);
		sprintf(s_pText, "Zagrales za %d pkt.", playValue);
		fontFillTextBitMap(s_pFont, s_pBmText, s_pText);
		fontDrawTextBitMap(s_pVpManager->pBack, s_pBmText, 2, 48, 3, FONT_COOKIE);	

		stack.value = playValue;
		stack.count = stack.count + select.count;
		select.count = 0;
		p2.PCturn = TRUE;
		displayCardsOnStack();
		discardPlayedCardsAndDraw();
	}
	else if (playValue <= stack.value){
		displayMessage("Za mala wartosc zagrania", 48, 16);
		incorrectPlayHandReset();
	}
}

void checkCorrectPlay(void){
	BOOL correctPlay = TRUE;
	UBYTE playPass = 0;

	for (UBYTE i = 0; i < 6; ++i){  // sprawdz wszystkie sloty do zagrywania
		if (p1.selectedToPlay[i] != 0){ // jesli w danym slocie jest wskazana karta
			UBYTE wzor = p1.selectedToPlay[i]; // przypisz jej wartosc jako wzor (jesli wskazane wiecej to pozostanie ta ostatnia, jest to troche slabe)
									//przypisz jej wartosc jako wzor (jesli wskazane wiecej to pozostanie ta ostatnia, jest to troche slabe)
			for (UBYTE j = 0; j < 6; ++j){ // jesli slot nie jest pusty
				if (p1.selectedToPlay[j] != 0){ // i jesli nie jest taki jak wzor, czyli rozne wartosci
					if (p1.selectedToPlay[j] != wzor){  //zagranie jest nieprawidlowe
						correctPlay = FALSE;
					}
				}
			}
		}
	}
	// check if pass (no possible play)
	for (UBYTE i = 0; i < 6; ++i){
		if (p1.selectedToPlay[i] == 0){
			playPass += 1;
		}
	}

	// check if no cards in hand - opponent plays 'til no cards  
	if (p1.hand[0] == 0 && p1.hand[1] == 0 && p1.hand[2] == 0 && p1.hand[3] == 0 && p1.hand[4] == 0 && p1.hand[5] == 0){
		p1.penaltyPoints += stack.count;
		stack.value = 0;
		stack.count = 0;
		select.count = 0;
		playPass = 0;
		p2.PCturn = TRUE;
		clearStackDisplay();
	}

	if (playPass == 6){
		p1.penaltyPoints += stack.count;
		stack.value = 0;
		stack.count = 0;
		select.count = 0;
		playPass = 0;
		clearStackDisplay();
		return;
	}

	if (correctPlay == FALSE){
		displayMessage("Nieprawidowe zagranie stryjku", 48, 16);
		
		
		incorrectPlayHandReset();
	}
	if (correctPlay == TRUE){
		displayMessage("Super zagranie stryjku", 48, 16);

		checkWithStackValue();
	}

}

void stateMenuCreate(void){
	s_pView = viewCreate(0,
		TAG_VIEW_COPLIST_MODE, COPPER_MODE_BLOCK,
		TAG_VIEW_GLOBAL_CLUT, 1,
		TAG_END
	);
	s_pVp = vPortCreate(0,
		TAG_VPORT_VIEW, s_pView,
		TAG_VPORT_BPP, 5,
		TAG_END
	);

	
	paletteLoad("data/szkola.plt", s_pVp->pPalette, 32);

	s_pVpManager = simpleBufferCreate(0,
			TAG_SIMPLEBUFFER_VPORT, s_pVp,
			TAG_SIMPLEBUFFER_BITMAP_FLAGS, BMF_CLEAR | BMF_INTERLEAVED,
			TAG_SIMPLEBUFFER_IS_DBLBUF, 0,
			TAG_END
	);

	s_pFont = fontCreate("data/topaz.fnt");
  	s_pBmText = fontCreateTextBitMap(320, s_pFont->uwHeight);

	s_pCard2 = bitmapCreateFromFile("data/card2.bm", 0);
	s_pCard3 = bitmapCreateFromFile("data/card3.bm", 0);
	s_pCard4 = bitmapCreateFromFile("data/card4.bm", 0);
	s_pCard5 = bitmapCreateFromFile("data/card5.bm", 0);
	s_pCard6 = bitmapCreateFromFile("data/card6.bm", 0);
	s_pCard7 = bitmapCreateFromFile("data/card7.bm", 0);
	s_pCard8 = bitmapCreateFromFile("data/card8.bm", 0);
	s_pCard9 = bitmapCreateFromFile("data/card9.bm", 0);
	s_pCard10 = bitmapCreateFromFile("data/card10.bm", 0);
	s_pCard11 = bitmapCreateFromFile("data/card11.bm", 0);
	s_pCard12 = bitmapCreateFromFile("data/card12.bm", 0);
	s_pCard13 = bitmapCreateFromFile("data/card13.bm", 0);

	randInit(461);
	joyOpen();
	keyCreate();
	systemUnuse();
	viewLoad(s_pView);
}

void stateMenuLoop(void){

	while (menu == TRUE){

		if (printMenuOnce == FALSE){
			initialSetup();
			clearScreen();
			printMenuText();
			printMenuOnce = TRUE;
		} 

		volatile UWORD uwTmp = uwRandMinMax(0, 10000);
		//uwRandMinMax(0, 10000);
	
		joyProcess();
		keyProcess();

		if(keyUse(KEY_ESCAPE)) {
			gameExit();
			return;
		}

		if(joyUse(JOY1_FIRE) || keyUse(KEY_RETURN)) {
			clearScreen();
			menu = FALSE;
			game = TRUE;
			deckShuffle();
			drawStartingHand();
			displayPlayerHand();
			displayHUDinfo();

			//stateChange(g_pStateMachineGame, &g_sStateGame);
			return;
		}
		logWrite("Current random: %hu\n", uwTmp);
	}

	while (game == TRUE){
		joyProcess();
		keyProcess();

		if(keyUse(KEY_ESCAPE)) {
			printMenuOnce = FALSE;
			game = FALSE;
			menu = TRUE;
			return;
		}

		if(keyUse(KEY_1)){
			wasKeyPressed = TRUE;
			select.slot = 1;
			selectCard();
			return;
		}

		if(keyUse(KEY_2)){
			wasKeyPressed = TRUE;
			select.slot = 2;
			selectCard();
			return;
		}

		if(keyUse(KEY_3)){
			wasKeyPressed = TRUE;
			select.slot = 3;
			selectCard();
			return;
		}

		if(keyUse(KEY_4)){
			wasKeyPressed = TRUE;
			select.slot = 4;
			selectCard();
			return;
		}

		if(keyUse(KEY_5)){
			wasKeyPressed = TRUE;
			select.slot = 5;
			selectCard();
			return;
		}
		if(keyUse(KEY_6)){
			wasKeyPressed = TRUE;
			select.slot = 6;
			selectCard();
			return;
		}

		if(keyUse(KEY_RETURN)){
			checkCorrectPlay();
			displayHUDinfo();
			return;
		}

		if (wasKeyPressed == TRUE){
			wasKeyPressed = FALSE;
			displaySelectedCards();
			displayPlayerHand();
		}

	}
	
	viewProcessManagers(s_pView);
	copProcessBlocks();
	vPortWaitForEnd(s_pVp);
}

void stateMenuDestroy(void){
	systemUse();
	viewDestroy(s_pView);
	joyClose();
	keyDestroy();
}


tState g_sStateMenu = {
  .cbCreate = stateMenuCreate,
  .cbLoop = stateMenuLoop,
  .cbDestroy = stateMenuDestroy,
  .cbSuspend = 0,
  .cbResume = 0,
  .pPrev = 0
};
