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
 * disp_num
 * Given an unsigned integer value <= 20, this will find the bottom two digits
 * in base 10 and display them on the 7 segment display
 */
void disp_num(unsigned n)
{
    n += 1;

    if (n < 10) {
        disp_digit(n + '0', DIGIT_2);
        disp_digit('0', DIGIT_1);
    } else if (n == 20) {
        disp_digit('2', DIGIT_1);
        disp_digit('0', DIGIT_2);
    } else {
        disp_digit('1', DIGIT_1);
        disp_digit(n - 10 + '0', DIGIT_2);
    }

    return;
}

/**
 * show_digit2
 * Toggles outputs and sets bits corresponding to illuminated segments of the
 * display on P2OUT. This is the ISR for the TA0CCR0 interrupt.
 *
 * Also, re-enables the P1.0 and P1.1 button interrupts - they were disabled
 * as a part of the debouncing logic
 *
 * Also, keeps track of delay when we're displaying the 'rolling' effect on a
 * die.
 */
#pragma vector = TIMER0_A0_VECTOR
__interrupt void show_digit2(void)
{
    extern unsigned die_delay, roll_flag;

    WDTCTL = WDTPW | WDTCNTCL; /* Pet WDT */

    if (roll_flag && !(--die_delay)) {
        roll_flag = die_delay = 0;
        _BIC_SR(LPM1_EXIT);
    }

    P1IE |= (BIT6 | BIT7); /* Re-enable button interrupts (debouncing logic) */

    P2OUT = digit_2; /* Display digit 2 */

    return;
}

/**
 * show_digit1
 * Toggles outputs and sets bits corresponding to illuminated segements of the
 * display on P2OUT. This is the ISR for the TA0CCR1 interrupt.
 *
 * Also counts out wait times for the LCD
 */
#pragma vector = TIMER0_A1_VECTOR
__interrupt void show_digit1(void)
{
    extern unsigned lcd_flag, wait_cycles;

    if (lcd_flag) {
        lcd_flag = 0;
        _BIC_SR(LPM1_EXIT);
    }

    P2OUT = (digit_1 | BIT6);
    TAIV = 0;

    return;
}
