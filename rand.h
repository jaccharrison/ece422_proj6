/* rand.h */

#ifndef RANDH
#define RANDH

#include <msp430.h>

#define OFFSET 8643
#define RANDMULT 49381

void srand(unsigned); /* Re-seeds the pseudo-random number generator */
unsigned rand(void); /* Generate a pseudo-random number */

#endif
