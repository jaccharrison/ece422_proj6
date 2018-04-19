#include <msp430.h> 
#include "rand.h"
#include "dice.h"
#include "dual7_seg.h"

/* Function prototypes */
void gpio_init(void);

#define ROLL_SEM 0x01
#define PRINT_SEM 0x02

unsigned sem, nfaces;

int main(void)
{
    gpio_init();

    /* Initialize a timer to make the 7-seg display 2 digits */
    TACCR0 = SEVENSEG_REFRESH;
    TACCR1 = SEVENSEG_REFRESH >> 1; /* Half of refresh interval */
    TAIV = 0; /* Clear timer interrupts */
    TACCTL0 |= CCIE; TACCTL1 |= CCIE; /* Enable timer interrupts */
    TACTL = (MC_1 | TASSEL_2 | TAIFG | TACLR);

    sem = 0; /* No active semaphores */
    nfaces = D20; /* Initialize number of faces on die */

    char nsides_str[] = "You are rolling: D20";

    while(1) {
        _BIS_SR(GIE | LPM1_bits); /* Wait for a button to be pressed */

        switch (sem) {
        case ROLL_SEM: roll(nfaces); break;
        //case PRINT_SEM: lcd_print_str(nsides_str);
        default: break;
        }
    }
}

/**
 * gpio_init:
 * Initializes GPIO on the MSP430G2452 for use as a dice roller
 */
void gpio_init(void)
{
    /* Assign P2 to be general purpose outputs */
    P2OUT = 0; P2SEL = 0; P2DIR = 0xFF;

    /* Make P1.0 and P1.1 inputs w/ pull-up resistors */
    P1DIR &= ~(BIT0 | BIT1);
    P1REN |= (BIT0 | BIT1); P1OUT |= (BIT0 | BIT1); /* Add pull-up resisitor */
    P1IE |= (BIT0 | BIT1); /* Enable interrupts */
    P1IES |= (BIT0 | BIT1); /* Interrupt on falling edge */
    P1IFG = 0; /* Clear any set interrupts */

    WDTCTL = WDTPW | WDTCNTCL; /* Pet WDT */

    return;
}

#define ROLL_BTN 0x01 /* Used to manipulate P1.0 */
static unsigned rand_seeded = 0; /* Flag that tracks whether rand is seeded */

/*
 * PORT1_ISR
 * The first time a button on Port 1 is pressed, the random number generator
 * will get seeded.
 * The button connected to P1.0 is used to roll the dice, and the button
 * connected to P1.1 is used to cycle between the number of faces on the die
 * getting rolled.
 */
#pragma vector = PORT1_VECTOR
__interrupt void PORT1_ISR(void)
{
    unsigned const faces[NFACES] = { D20, D4, D6, D8, D10, D12 };
    static unsigned face_index = 1;

    /* The first time a button is pressed, seed the random number generator */
    if(!rand_seeded) {
        srand(TAR); /* Seed with current value of TA */
        rand_seeded = 1; /* We don't need to seed rand again */
    }

    P1IE &= ~(BIT0 | BIT1); /* Disable buttons until roll is over */

    /* Take action depending on which button was pressed */
    if (P1IFG & ROLL_BTN) {
        /* Roll button */
        sem = ROLL_SEM;
        _BIC_SR(LPM1_EXIT);
    } else {
        /* Next die button */
        if (face_index == NFACES)
            face_index = 0; /* Roll over to beginning of list */
        nfaces = faces[face_index++];
        sem = PRINT_SEM;
    }

    P1IFG = 0;

    return;
}
