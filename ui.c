#include <string.h>
#include "config.h"
#include "lcd.h"
#include "keypad.h"
#include "auth.h"
#include "ui.h"

void read_pin(char *pin, const char *prompt)
{
    uint8_t n = 0;
    char key;

    memset(pin, 0, PIN_LEN + 1);

    lcd_clear();
    lcd_goto(0, 0);
    lcd_print(prompt);
    lcd_goto(1, 0);

    while (n < PIN_LEN) {
        key = keypad_get_key();

        if (key >= '0' && key <= '9') {
            pin[n++] = key;
            lcd_data('*');
        }
        else if (key == 'C' && n > 0) {
            n--;
            pin[n] = '\0';

            lcd_goto(1, n);
            lcd_data(' ');
            lcd_goto(1, n);
        }
    }

    pin[PIN_LEN] = '\0';
}

void read_phone(char *buf)
{
    uint8_t n = 0;
    char key;

    memset(buf, 0, CONTACT_PHONE_LEN + 1);

    lcd_clear();
    lcd_goto(0, 0);
    lcd_print("Owner phone:");
    lcd_goto(1, 0);

    while (n < CONTACT_PHONE_LEN) {
        key = keypad_get_key();

        if (key >= '0' && key <= '9') {
            buf[n++] = key;
            lcd_data(key);
        }
        else if (key == 'C' && n > 0) {
            n--;
            buf[n] = '\0';

            lcd_goto(1, n);
            lcd_data(' ');
            lcd_goto(1, n);
        }
    }

    buf[CONTACT_PHONE_LEN] = '\0';
}

void show_home(void)
{
    lcd_clear();
    lcd_goto(0, 0);
    lcd_print("HOME SECURITY");
    lcd_goto(1, 0);

    if (has_any_user())
        lcd_print("A=OPEN B=NEW");
    else
        lcd_print("B=NEW USER");
}

void countdown_screen(uint32_t seconds_left, bool show_prompt)
{
    uint16_t s = (uint16_t)seconds_left;

    lcd_clear();
    lcd_goto(0, 0);

    lcd_data('0' + (s / 100));
    lcd_data('0' + ((s / 10) % 10));
    lcd_data('0' + (s % 10));
    lcd_print("s LOCKED");

    if (show_prompt) {
        lcd_goto(1, 0);
        lcd_print("CONTACT OWNER?");
    }
}
