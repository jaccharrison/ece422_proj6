#include "lcd.h"

#define LCD_4bit_tx(A)							\
  P1OUT &= 0xF0; P1OUT |= 0x0F & A; P1OUT |= LCD_E; P1OUT &= ~LCD_E

#define LCD_wait() lcd_flag = 1; _BIS_SR(GIE | LPM1_bits)

void LCD_txbyte(char); /* Internal function used to send data to the LCD */

unsigned lcd_flag = 0;

/**
 * init_lcd
 * Initializes the microcontroller for use with an external HD44780U LCD
 * display on port 2.
 *
 * The following code should either be copied into the initialization function
 * below or included in an init_gpio function:
 *   P2DIR = LCD_PINS; // Sets pins 2.0-2.5 as outputs to control LCD
 *
 * 15ms must have passed between LCD power-on and execution of this function.
 */
void init_lcd(void)
{
    LCD_wait();

    /* Send a sequence of instructions that forces re-init of the LCD */
    LCD_4bit_tx(LCD_INIT);

    LCD_wait(); /* Wait >= 4.1 ms */

    LCD_4bit_tx(LCD_INIT);
    LCD_wait();
    LCD_txbyte(LCD_FUNC_SET | LCD_FUNC_8BIT | LCD_SET_4BIT);
    /* Finalize 4-bit interface mode and set font set 1 display line */
    LCD_txbyte(LCD_FUNC_SET);
    /* Set the display mode */
    LCD_txbyte(0x0F);
    /* Clear the display */
    LCD_txbyte(LCD_CLEAR_DISP);
    /* Set entry point to home */
    LCD_txbyte(LCD_RETURN_HOME);
    /* Set cursor to move to the right after a character is written */
    LCD_txbyte(LCD_ENTRY_MODE_SET | LCD_ENTRY_INC);
    LCD_wait();
}

/**
 * LCD_txbyte
 * Internal utility function - the real way that any byte of data gets sent to
 * the LCD. This is only used by other functions in the external LCD API - it
 * is not part of the API.
 */
void LCD_txbyte(char c)
{
    LCD_4bit_tx((c & 0xF0) >> 4);
    LCD_wait();
    LCD_4bit_tx(c & 0x0F);
    LCD_wait();
    return;
}

/**
 * lcd_print_char
 * Prints a single character on the LCD
 */
void lcd_print_char(char c)
{
  P1OUT |= LCD_RS;
  LCD_txbyte(c);
  P1OUT &= ~LCD_RS;
  LCD_wait();
}

/**
 * lcd_print_str
 * Prints a null-terminated string starting at the address pointed to by the
 * argument
 */
void lcd_print_str(char* c)
{
  while (*c) {
    P1OUT |= LCD_RS;
    LCD_txbyte(*(c++));
    P1OUT &= ~LCD_RS;
    LCD_wait();
  }
  return;
}

/**
 * lcd_clr_screen
 * Clears the lcd screen
 */
void lcd_clr_screen(void)
{
  LCD_txbyte(LCD_CLEAR_DISP);
  LCD_wait();
  return;
}
