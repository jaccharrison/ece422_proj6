#include <msp430.h> 
#include "rand.h"
#include "dual7_seg.h"

/* Function prototypes kendra wuz here */
void gpio_init(void);
void roll(unsigned);

static unsigned seed; /* Stores the random number seed obtained from timer */
unsigned roll_delay, rolling;

int main(void)
{

    WDTCTL = WDTPW | WDTHOLD;	/* Stop watchdog timer */
	
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

	while(1) {
	    _BIS_SR(GIE | LPM1_bits);
	    roll(20);
	}

	return 0;
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
    P1IFG = 0; /* Clear any set interrupts */

    return;
}

#pragma vector = PORT1_VECTOR
__interrupt void get_rand(void)
{
    P1IE &= ~BIT1;
    seed = TAR; /* Get current value of TA0 */
    P1IFG &= ~BIT0;
    _BIC_SR(LPM1_EXIT);
    return;
}
