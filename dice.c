#include "rand.h"
#include "dual7_seg.h"

unsigned die_delay; /* Decremented in an interrupt until it is 0 to create
                       'die rolling' effect */
unsigned roll_flag;

/**
 * roll:
 * Simulates rolling a die
 */
void roll(unsigned nfaces) {
    unsigned i, nchanges, total_delay;
    nchanges = (rand() & 0x07) + 0x0A; /* Generate rand number 10 <= n <= 17 */
    die_delay = total_delay = 0;

    for (i = 0; i < nchanges; ++i) {
        total_delay += 0x02;
        die_delay = total_delay;
        disp_num(rand() % nfaces);
        roll_flag = 1;
        _BIS_SR(GIE | LPM1_bits);
    }

   return;

}
