/* Functions for interfacing a dual 7-segment display */
#include "dual7_seg.h"

static unsigned char digit_1, digit_2;

/**
 * disp_digit
 * Sets bits corresponding to illuminated segments in a 7-segment display
 * in one of the 'digit_n' variables. Note - this function does not immediately
 * update the value shown on the 7-seg display; a timer interrupt will update
 * the displayed value the next time it fires.
 *
 */
void disp_digit(unsigned char n, unsigned d)
{
    unsigned char *digit; /* Pointer to the digit we're writing to */

    if (!d)
        digit = &digit_2; /* d is non-zero - point to digit_2 */
    else
        digit = &digit_1; /* d is zero - point to digit_1 */

    /* Assign a value to *digit based on the character in n */
    switch (n) {
    case '0': *digit = SEVENSEG_0; break;
    case '1': *digit = SEVENSEG_1; break;
    case '2': *digit = SEVENSEG_2; break;
    case '3': *digit = SEVENSEG_3; break;
    case '4': *digit = SEVENSEG_4; break;
    case '5': *digit = SEVENSEG_5; break;
    case '6': *digit = SEVENSEG_6; break;
    case '7': *digit = SEVENSEG_7; break;
    case '8': *digit = SEVENSEG_8; break;
    case '9': *digit = SEVENSEG_9; break;
    default: break;
    }
}

/**
 * show_digit2
 * Toggles outputs and sets bits corresponding to illuminated segments of the
 * display on P2OUT. This is the ISR for the TA0CCR0 interrupt.
 */
#pragma vector = TIMER0_A0_VECTOR
__interrupt void show_digit2(void)
{
    P2OUT = 0x00; /* Clear output bits */
    P1OUT |= BIT4; /* Display on second seven-seg digit */
    P2OUT |= digit_2;
    return;
}

/**
 * show_digit1
 * Toggles outputs and sets bits corresponding to illuminated segements of the
 * display on P2OUT. This is the ISR for the TA0CCR1 interrupt.
 */
#pragma vector = TIMER0_A1_VECTOR
__interrupt void show_digit1(void)
{
    if (TAIV == 0x02) {
        P2OUT = 0x00; /* Clear output bits */
        P1OUT &= ~BIT4; /* Display on first seven-seg digit */
        P2OUT |= (digit_1 | BIT6);
    }
    return;
}
