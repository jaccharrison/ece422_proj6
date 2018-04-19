/* dice.h */

#ifndef DICE_H
#define DICE_H

#include <msp430.h>

enum DICE { D20 = 20, D12 = 12, D10 = 10, D8 = 8, D6 = 6, D4 = 4 };
#define NFACES 6

/* Returns the value of rolling a die with supplied number of faces */
unsigned roll(unsigned);

#endif
