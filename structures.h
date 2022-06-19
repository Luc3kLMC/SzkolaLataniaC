#ifndef STRUCTURES_H
#define STRUCTURES_H

struct stack {
    UBYTE value;  // sum of already played card's value, needed to check if current play is correct (higher value)
    UBYTE count;  // counting the card's number for penalty points
    UBYTE cardsDisplay; // three variables to set the drawing cards on the stack
    UBYTE stackDisplayX;
    UBYTE stackDisplayY;   
};

struct select {
    UBYTE slot;
    UBYTE count;
};

struct hand {
    UBYTE cardInHandDisplayX;
    UBYTE cardInHandDisplayY;
    UBYTE selectedCardY;
};

struct player {
    UBYTE hand[6];
    UBYTE selectedToPlay[6];
    UBYTE penaltyPoints;
    UBYTE PCturn;  // if true its time for PC opponent to play
};

#endif

