#include <msp430.h> 
#include "rand.h"
#include "dual7_seg.h"

/* Function prototypes kendra wuz here */
void gpio_init(void);

int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
	gpio_init();

	/* Initialize a timer with outmodes to make the 7-seg display 2 digits */
	TACCR0 = SEVENSEG_REFRESH;
	TACCR1 = SEVENSEG_REFRESH >> 1; /* Half of refresh interval */
	TAIV = 0; /* Clear timer interrupts */
	TACCTL0 |= CCIE; TACCTL1 |= CCIE; /* Enable timer interrupts */
	TACTL = (MC_1 | TASSEL_2 | TAIFG);

	disp_digit('9', DIGIT_1);
	disp_digit('0', DIGIT_2);

	_BIS_SR(GIE);

	while(1);

	return 0;
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
}
