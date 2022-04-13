/* LCD.h
 *
 * Functions for interfacing Hitachi HD44780 LCD Driver
 * in 4-bit interface mode
 *
 * Pin correspondence:
 *  P2.0    : E
 *  P2.1    : RW
 *  P2.2    : RS
 *  P2.4-7  : DB4-DB7
 *
 * Author: Bruno
 */

#ifndef LCD_H_
#define LCD_H_

#include <stdint.h>
#include <stddef.h>

/*****
 * Bit definitions
 *****/
#define E   (0x01)  /* Starts data read/write */
#define RW  (0x02)  /* Selects read or write. 0:Write / 1:Read */
#define RS  (0x04)  /* Selects registers. 0:IR/BF / 1:DR */
// bit3 unused
/* High order bidirectional data bus pins. */
#define DB4 (0x10)
#define DB5 (0x20)
#define DB6 (0x40)
#define DB7 (0x80)

#define DB_bits (DB4+DB5+DB6+DB7)

/* Status Flags */
#define BF  (DB7)
#define ADD (DB4+DB5+DB6)

/*****
 * INSTRUCTIONS
 *  See Table 6 of Hitachi LCD datasheet, pages 24/25
 *****/
#define LCD_CLEARDISPLAY    (0x01)  /* Clear display */
#define LCD_RETURNHOME      (0x02)  /* Return home */
#define LCD_ENTRYMODESET    (0x04)  /* Entry mode set */
#define LCD_DISPLAYCONTROL  (0x08)  /* Display on/off control */
#define LCD_CURSDISPSHIFT   (0x10)  /* Cursor or display shift */
#define LCD_FUNCTIONSET     (0x20)  /* Function set */
#define LCD_SETCGRAMADDR    (0x40)  /* Set CGRAM address */
#define LCD_SETDDRAMADDR    (0x80)  /* Set DDRAM address */

/* Entry mode flags */
#define LCD_ENTRYINCREMENT  (0x02)  /* Cursor direction left to right */
#define LCD_ENTRYSHIFT      (0x01)  /* Accompanies display shift */

/* Display control flags */
#define LCD_DISPLAYON       (0x04)  /* Display on */
#define LCD_CURSORON        (0x02)  /* Cursor display on */
#define LCD_BLINKON         (0x01)  /* Cursor blink on */

/* Cursor/display shift flags */
#define LCD_DISPLAYSHIFT    (0x08)  /* Display shift */
#define LCD_SHIFTRIGHT      (0x04)  /* Shift to the right */
/* Shift functions */
#define LCD_CURSSHIFTLEFT   (LCD_CURSDISPSHIFT)                                 /* Shifts the cursor position to the left   */
#define LCD_CURSSHIFTRIGHT  (LCD_CURSDISPSHIFT+LCD_SHIFTRIGHT)                  /* Shifts the cursor position to the right  */
#define LCD_DISPSHIIFTLEFT  (LCD_CURSDISPSHIFT+LCD_DISPLAYSHIFT)                /* Shifts the entire display to the left    */
#define LCD_DISPSHIFTRIGHT  (LCD_CURSDISPSHIFT+LCD_DISPLAYSHIFT+LCD_SHIFTRIGHT) /* Shifts the entire display to the right   */

/* Function set flags */
#define LCD_8BIT            (0x10)  /* 8-bit interface */
#define LCD_2LINE           (0x08)  /* 2 display lines */
#define LCD_5X10DOTS        (0x04)  /* 5x10 dots font */

/*****
 * Constants
 *****/
#define LCD_ROW0OFFSET  (0x00)
#define LCD_ROW1OFFSET  (0x40)

/*****
 * Function declarations
 *****/

/* lcd_init()
 *
 * Initializes display in:
 *  4-bit interface mode
 *  2 lines
 * Clears screen and sets cursor at 0,0
 */
void lcd_init();


/* lcd_command(cmd)
 *
 * Send command byte to lcd
 */
void lcd_command(const uint8_t);

/* lcd_set_cursor(row, col)
 *
 * Set cursor position
 */
void lcd_set_cursor(uint8_t col, uint8_t row);

/* write(data)
 * write(*str)
 * write(*buffer,size)
 *
 * Process data and send to display
 *
 * Macros based on:
 *  https://stackoverflow.com/a/46222749/10723430
 * Requires C11
 */
void _lcd_write_byte(const uint8_t val);
void _lcd_write_str(const char* str);
void _lcd_write_buff(const uint8_t* buffer, size_t size);
#define lcd_write(...) SELECT(__VA_ARGS__)(__VA_ARGS__)

#define SELECT(...) CONCAT(SELECT_, NARG(__VA_ARGS__))(__VA_ARGS__)
#define CONCAT(X, Y) CONCAT_(X, Y)
#define CONCAT_(X,Y) X ## Y
// Select 1 or 2 arguments
#define SELECT_1(_1) _Generic((_1), \
        uint8_t: _lcd_write_byte,   \
        char*: _lcd_write_str       \
)
#define SELECT_2(_1, _2) _lcd_write_buff(_1, _2)    // Only one case, no need for generic

#define NARG(...) NARG_(0, ## __VA_ARGS__, 2,1,0)
#define NARG_(_0,_1,_2,N,...) N


/* lcd_status()
 *
 * Return status byte (busy flag & address)
 */
uint8_t lcd_status();

#endif /* LCD_H_ */
