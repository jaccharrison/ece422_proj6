#ifndef DUAL7SEG_H
#define DUAL7SEG_H

#include <msp430.h>

#define SEVENSEG_0 0x3F
#define SEVENSEG_1 0x06
#define SEVENSEG_2 0x9B
#define SEVENSEG_3 0x8F
#define SEVENSEG_4 0xA6
#define SEVENSEG_5 0xAD
#define SEVENSEG_6 0xBD
#define SEVENSEG_7 0x07
#define SEVENSEG_8 0xBF
#define SEVENSEG_9 0xA7

#define DIGIT_1 0x00
#define DIGIT_2 0x01

#define SEVENSEG_REFRESH 18300 /* Refresh at ~60Hz */

void disp_num(unsigned);
void disp_digit(unsigned char, unsigned); /* Sets digit to display on 7-seg */

#endif
