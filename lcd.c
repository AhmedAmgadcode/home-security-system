#include <avr/io.h>
#include <util/delay.h>
#include "config.h"
#include "lcd.h"

static void lcd_pulse_enable(void)
{
    PORTC |= (1 << LCD_EN);
    _delay_us(1);
    PORTC &= ~(1 << LCD_EN);
    _delay_us(50);
}

static void lcd_send_nibble(uint8_t nibble)
{
    PORTC = (PORTC & 0xC3) | ((nibble & 0x0F) << 2);
    lcd_pulse_enable();
}

static void lcd_command(uint8_t cmd)
{
    PORTC &= ~(1 << LCD_RS);
    lcd_send_nibble(cmd >> 4);
    lcd_send_nibble(cmd & 0x0F);

    if (cmd == 0x01 || cmd == 0x02)
        WDT_DELAY_MS(2);
}

void lcd_data(uint8_t data)
{
    PORTC |= (1 << LCD_RS);
    lcd_send_nibble(data >> 4);
    lcd_send_nibble(data & 0x0F);
}

void lcd_init(void)
{
    DDRC = 0xFF;
    WDT_DELAY_MS(20);

    PORTC &= ~(1 << LCD_RS);

    lcd_send_nibble(0x03);
    WDT_DELAY_MS(5);

    lcd_send_nibble(0x03);
    _delay_us(150);

    lcd_send_nibble(0x03);
    lcd_send_nibble(0x02);

    lcd_command(0x28);
    lcd_command(0x0C);
    lcd_command(0x06);
    lcd_command(0x01);
}

void lcd_clear(void)
{
    lcd_command(0x01);
}

void lcd_goto(uint8_t row, uint8_t col)
{
    lcd_command((row == 0 ? 0x80 : 0xC0) + col);
}

void lcd_print(const char *s)
{
    while (*s)
        lcd_data((uint8_t)*s++);
}
