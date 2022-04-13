/* LCD.c
 * LCD.h function definition.
 * See LCD.h for more info
 *
 * Author: Bruno
 */
#include "LCD.h"
#include <msp430g2553.h>
#include <stdbool.h>
#include <string.h>

// Internal functions
enum _Register_select { IR, DR };

void _command(const uint8_t data,const enum _Register_select rs);
void _command4bit(const uint8_t data,const enum _Register_select rs);
bool _status4bit();

const uint8_t _init_sequence[] = {
                                   LCD_FUNCTIONSET|LCD_2LINE
                                  ,LCD_DISPLAYCONTROL|LCD_DISPLAYON
                                  ,LCD_CLEARDISPLAY
                                  ,LCD_ENTRYMODESET|LCD_ENTRYINCREMENT
};

// Definitions

void lcd_init()
{
    /* Adapted from HD44780U datasheet
     * Initializing by Instruction
     * figure 24, pages 45/46
     */

    // Setup pin modes
    P2DIR |= RS+RW+E+DB_bits;
    P2OUT &= ~(RS+RW+E+DB_bits);

    // wait >15ms
    // ~15000 cycles at 1MHz
    _delay_cycles(20000);

    _command4bit(LCD_FUNCTIONSET|LCD_8BIT, IR);
    _delay_cycles(5000);
    _command4bit(LCD_FUNCTIONSET|LCD_8BIT, IR);
    _delay_cycles(150);
    _command4bit(LCD_FUNCTIONSET|LCD_8BIT, IR);
    _delay_cycles(150);
    // display is ensured 8-bit mode
    // BF can be read

    // Set 4-bit interface
    _command4bit(LCD_FUNCTIONSET, IR);
    // wait for ready
    while(_status4bit());
    // After 4-bit set, use "normal" functions

    // send setup instructions
    uint8_t* instruction_ptr = (uint8_t*) _init_sequence;
    size_t instruction_i = sizeof _init_sequence;
    while(instruction_i != 0)
    {
        lcd_command(*instruction_ptr++);
        instruction_i--;
    }
}

void lcd_command(const uint8_t data)
{
    _command(data, IR);
    // wait for ready
    while(lcd_status() & BF);
}

void lcd_set_cursor(uint8_t col, uint8_t row)
{
    // Clip row
    if (row > 2)
        row = 1;
    // col is not checked

    uint8_t addr = col;
    switch(row){
    case 0:
        addr += LCD_ROW0OFFSET;
        break;
    case 1:
        addr += LCD_ROW1OFFSET;
    }
    lcd_command(LCD_SETDDRAMADDR | addr);
}

void _lcd_write_byte(const uint8_t val)
{
    _command(val, DR);
    while(lcd_status() & BF);
}

void _lcd_write_buff(const uint8_t* buffer, size_t size)
{
    // no safety
    while(size-- != 0)
    {
        _lcd_write_byte(*buffer++);
    }
}

void _lcd_write_str(const char* str)
{
    if (str == NULL)
        return;
    _lcd_write_buff((const uint8_t*)str, strlen(str));
}

uint8_t lcd_status()
{
volatile uint8_t status;
    // Prepare P2OUT
    P2OUT = 0x00;
    P2OUT |= RW;

    // Set DB to input
    P2DIR &= ~DB_bits;

    // Read upper nibble
    // Raise E to read
    P2OUT |= E;
    __asm(" nop");

    // read bytes
    status = P2IN & DB_bits;

    // lower E after read
    P2OUT &= ~E;
    __asm(" nop");

    // Read lower nibble
    P2OUT |= E;
    __asm(" nop");
    status |= (P2IN & DB_bits) >> 4;
    P2OUT &= ~E;

    // Return DB to output mode
    P2DIR |= DB_bits;
    return status;
}

// send data to lcd
void _command(const uint8_t data,const enum _Register_select rs)
{
    // prepare data nibbles
    const uint8_t upper_nibble = data & 0xF0;
    const uint8_t lower_nibble = (data & 0x0F) << 4;

    _command4bit(upper_nibble, rs);
    _command4bit(lower_nibble, rs);
}

// send only upper 4 bits of byte
void _command4bit(const uint8_t data,const enum _Register_select rs)
{
    // Reset P2OUT
    P2OUT = 0x00;
    // Set Register Select
    if(rs == DR)
        P2OUT |= RS;

    // Set data
    P2OUT |= (data & 0xF0);
    // Pulse E
    /* Enable pulse width >230 ns
     * from HD44780 datasheet bus timing characteristics, page 49
     *
     * <1 cycle at 1MHz
     */
    P2OUT |= E;
    __asm(" nop");
    P2OUT &= ~E;
}

// read busy flag in 8-bit mode
bool _status4bit()
{
volatile bool busy;

    // Reset P2OUT
    P2OUT = 0x00;
    // Set read mode
    P2OUT |= RW;

    // Set DB7 to input
    P2DIR &= ~DB7;

    // Raise E to read
    P2OUT |= E;
    __asm(" nop");

    // read flag
    busy = P2IN & BF;

    // lower E after read
    P2OUT &= ~E;
    // return DB7 to output mode
    P2DIR |= DB7;
    return busy;
}
