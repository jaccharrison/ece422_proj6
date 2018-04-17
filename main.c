#include <msp430.h> 
#include "rand.h"
#include "dual7_seg.h"

#define NFACES 6
#define D20 20

/* Function prototypes kendra wuz here */
void gpio_init(void);
void roll(unsigned);

static unsigned seed; /* Stores the random number seed obtained from timer */
unsigned roll_delay, rolling, face_num;

int main(void)
{

    WDTCTL = WDTPW | WDTHOLD;   /* Stop watchdog timer */

    gpio_init();

    TACTL = (TASSEL_2 | MC_2); /* Stars timer in continuous mode */
    _BIS_SR(GIE | LPM1_bits); /* Wait for a button to be pressed to seed rand */

    srand(seed); /* Seed random number generator */

    /* Initialize a timer to make the 7-seg display 2 digits */
    TACCR0 = SEVENSEG_REFRESH;
    TACCR1 = SEVENSEG_REFRESH >> 1; /* Half of refresh interval */
    TAIV = 0; /* Clear timer interrupts */
    TACCTL0 |= CCIE; TACCTL1 |= CCIE; /* Enable timer interrupts */
    TACTL = (MC_1 | TASSEL_2 | TAIFG | TACLR);

    /*Initialize a timer for delay */


    face_num = D20; /* Initialize number of faces on die */

    while(1) {
        _BIS_SR(GIE | LPM1_bits);
        roll_delay = 0;
         roll(face_num);
         P1IE &= !BIT0 & !BIT1;
         unsigned int loop = (rand()% 0x10) + 0x8;
         unsigned int i;
         for(i = 0; i<=loop; i++){
             roll_delay = 2;
            roll(face_num);
            while(roll_delay == 2);
            roll_delay = 0;
        }
    }


}

/**
 * roll:
 * Simulates rolling a die
 */
void roll(unsigned nfaces) {

   disp_num(rand() % nfaces); /* Display new random number */

   return;
}

/**
 * gpio_init:
 * Initializes GPIO on the MSP430G2452 for use as a dice roller
 */
void gpio_init(void)
{
    /* Assign P2 to be outputs */
    P2OUT = 0;
    P2DIR = 0xFF;

    /* Assign P1.4 to be an output */
    P1DIR |= BIT4;

    /* Assign P2.6 to its secondary peripheral (TA0.1) */
    P2SEL = 0;

    /* Make P1.0 an input button */
    P1DIR &= ~BIT0;
    P1REN |= BIT0; P1OUT |= BIT0; /* Add pull-up resisitor */
    P1IE |= BIT0; P1IES |= BIT0; /* En. interrupt for falling edge transition */

    /* Make P1.1 an input button */
    P1DIR &= ~BIT1;
    P1REN |= BIT1; P1OUT |= BIT1; /* Add pull-up resisitor */
    P1IE |= BIT1; P1IES |= BIT1; /* En. interrupt for falling edge transition */
    P1IFG = 0; /* Clear any set interrupts */

    roll_delay = 0;

    return;
}

#pragma vector = PORT1_VECTOR
__interrupt void get_rand(void)
{
    unsigned const faces[NFACES] = { 20, 4, 6, 8, 10, 12 };
    static unsigned face_index = 0;

    if(!(P1IN & BIT0)){
        seed = TAR; /* Get current value of TA0 */

    }
    if(!(P1IN & BIT1)){
       if (face_index == NFACES) { face_index = 0; }
       face_num = faces[face_index++];
    }
    P1IFG &= ~BIT0;
    P1IFG &= ~BIT1;
    _BIC_SR(LPM1_EXIT);
    return;
}
