#include <avr/io.h>
#include <avr/interrupt.h>
#include "config.h"
#include "timer.h"

static volatile uint32_t ms_ticks = 0;

void timer0_init(void)
{
    TCCR0 = (1 << WGM01) | (1 << CS01) | (1 << CS00);
    OCR0 = 124;
    TIMSK |= (1 << OCIE0);
}

ISR(TIMER0_COMP_vect)
{
    ms_ticks++;
}

uint32_t millis_now(void)
{
    uint32_t t;
    uint8_t sreg = SREG;

    cli();
    t = ms_ticks;
    SREG = sreg;

    return t;
}
