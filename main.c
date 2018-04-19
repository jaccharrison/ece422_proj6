#include <msp430.h> 
#include "rand.h"
#include "dice.h"
#include "dual7_seg.h"
#include "lcd.h"

/* Function prototypes */
void gpio_init(void);

#define ROLL_SEM 0x01
#define PRINT_SEM 0x02

/* Indices in string that indicate how many faces we're rolling */
#define STR_DIG_1 10
#define STR_DIG_2 11

unsigned sem;

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;

    gpio_init();

    /* Initialize a timer to make the 7-seg display 2 digits */
    TACCR0 = SEVENSEG_REFRESH;
    TACCR1 = SEVENSEG_REFRESH >> 1; /* Half of refresh interval */
    TAIV = 0; /* Clear timer interrupts */
    TACCTL0 |= CCIE; TACCTL1 |= CCIE; /* Enable timer interrupts */
    TACTL = (MC_1 | TASSEL_2 | TAIFG | TACLR);

    init_lcd();

    sem = 0; /* No active semaphores */

    unsigned const faces[NFACES] = { D20, D6, D8, D10, D12 };
    static unsigned face_index = 1;
    unsigned nfaces = D20; /* Initialize number of faces on die */

    char nsides_str[] = "Rolling: D20";
    lcd_print_str(nsides_str);



    while(1) {
        _BIS_SR(GIE | LPM1_bits); /* Wait for a button to be pressed */

        switch (sem) {
        case ROLL_SEM: roll(nfaces); break;
        case PRINT_SEM:
            nfaces = faces[face_index++];
            if (nfaces < D10) {
                nsides_str[STR_DIG_1] = '0' + nfaces;
                nsides_str[STR_DIG_2] = ' ';
            } else if (nfaces == D20) {
                face_index = 1;
                nsides_str[STR_DIG_1] = '2';
                nsides_str[STR_DIG_2] = '0';
            } else {
                nsides_str[STR_DIG_1] = '1';
                nsides_str[STR_DIG_2] = nfaces - 10 + '0';
            }
            lcd_clr_screen();
            lcd_print_str(nsides_str);
            break;
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

    P1DIR = LCD_PINS; P1OUT = 0;

    /* Make P1.0 and P1.1 inputs w/ pull-up resistors */
    P1DIR &= ~(BIT6 | BIT7);
    P1REN |= (BIT6 | BIT7); P1OUT |= (BIT6 | BIT7); /* Add pull-up resisitor */
    P1IE |= (BIT6 | BIT7); /* Enable interrupts */
    P1IES |= (BIT6 | BIT7); /* Interrupt on falling edge */
    P1IFG = 0; /* Clear any set interrupts */

    return;
}

#define ROLL_BTN 0x40 /* Used to manipulate P1.0 */

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
    static unsigned rand_seeded = 0;

    P1IE &= ~(BIT6 | BIT7); /* Disable buttons until roll is over */


    /* The first time a button is pressed, seed the random number generator */
    if(!rand_seeded) {
        srand(TAR); /* Seed with current value of TA */
        rand_seeded = 1; /* We don't need to seed rand again */
    }

    /* Take action depending on which button was pressed */
    if (P1IFG & ROLL_BTN) {
        /* Roll button */
        P1IFG = 0;
        sem = ROLL_SEM;
        _BIC_SR(LPM1_EXIT);
    } else {
        /* Next die button */
        P1IFG = 0;
        sem = PRINT_SEM;
        _BIC_SR(LPM1_EXIT);
    }
    return;
}
