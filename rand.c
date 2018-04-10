/* rand.c: Functions for generating random integers on MSP430 */
#include "rand.h"

static unsigned seed;

void srand(unsigned s)
{
    seed = s;
    return;
}

unsigned rand(void)
{
    seed *= RANDMULT;
    seed += OFFSET;
    return seed; /* Truncate to unsigned - acts as 16 bit modulo */
}
