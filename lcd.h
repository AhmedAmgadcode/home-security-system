#ifndef LCD_H
#define LCD_H

#include <stdint.h>

void lcd_init(void);
void lcd_clear(void);
void lcd_goto(uint8_t row, uint8_t col);
void lcd_data(uint8_t data);
void lcd_print(const char *s);

#endif
