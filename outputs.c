#include <avr/io.h>
#include "config.h"
#include "outputs.h"

void outputs_init(void)
{
    DDRB |= (1 << BUZZER_PIN) | (1 << GREEN_LED) | (1 << RED_LED);

    PORTB &= ~((1 << BUZZER_PIN) |
               (1 << GREEN_LED) |
               (1 << RED_LED));
}

void beep(uint16_t ms)
{
    PORTB |= (1 << BUZZER_PIN);

    while (ms--)
        WDT_DELAY_MS(1);

    PORTB &= ~(1 << BUZZER_PIN);
}

void error_beeps(void)
{
    beep(120);
    WDT_DELAY_MS(100);
    beep(120);
}
