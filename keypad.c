#include <avr/io.h>
#include <util/delay.h>
#include "config.h"
#include "keypad.h"

void keypad_init(void)
{
    DDRD = 0x0F;
    PORTD = 0xFF;
}

char keypad_read_raw(void)
{
    static const char map[4][4] = {
        {'1','2','3','A'},
        {'4','5','6','B'},
        {'7','8','9','C'},
        {'*','0','#','D'}
    };

    for (uint8_t r = 0; r < 4; r++) {
        PORTD = 0xFF;
        PORTD &= ~(1 << r);
        _delay_us(5);

        uint8_t cols = (PIND >> 4) & 0x0F;

        if (cols != 0x0F) {
            for (uint8_t c = 0; c < 4; c++) {
                if (!(cols & (1 << c)))
                    return map[r][c];
            }
        }
    }

    return 0;
}

char keypad_get_key(void)
{
    char k;

    do {
        wdt_reset();
        k = keypad_read_raw();
    } while (!k);

    WDT_DELAY_MS(25);

    while (keypad_read_raw()) {
        wdt_reset();
        WDT_DELAY_MS(5);
    }

    WDT_DELAY_MS(250);

    return k;
}
