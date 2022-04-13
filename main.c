#include <msp430g2553.h>
#include "LCD.h"


void main(void)
{
    WDTCTL = WDTPW | WDTHOLD;       // stop watchdog timer
    /* Unused I/O pins should be configured as I/O function, output direction */
    P1OUT = 0x00;
    P1SEL = 0x00;
    P1DIR = 0xFF;
    P2OUT = 0x00;
    P2SEL = 0x00;   // >:C
    P2DIR = 0xFF;

    // Init LCD
    lcd_init();
    lcd_set_cursor(0, 0);
    lcd_write("Microcontrollers");
    lcd_set_cursor(0, 1);
    lcd_write("Tue-Fri 11:30-13");

    P1DIR |= 0x01;                  // configure P1.0 as output

    while(1)
    {
        P1OUT ^= 0x01;              // toggle P1.0
        _delay_cycles(500000);
    }
}
