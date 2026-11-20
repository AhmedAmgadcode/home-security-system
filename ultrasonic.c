#include <avr/io.h>
#include <util/delay.h>
#include "config.h"
#include "ultrasonic.h"

void ultrasonic_init(void)
{
    DDRA |= (1 << TRIG_PIN);
    DDRA &= ~(1 << ECHO_PIN);
    PORTA &= ~(1 << TRIG_PIN);
}

uint16_t ultrasonic_read_cm(void)
{
    uint32_t pulse_us = 0;
    uint16_t timeout;

    PORTA &= ~(1 << TRIG_PIN);
    _delay_us(2);
    PORTA |= (1 << TRIG_PIN);
    _delay_us(10);
    PORTA &= ~(1 << TRIG_PIN);

    timeout = 30000;
    while (!(PINA & (1 << ECHO_PIN))) {
        _delay_us(1);
        if (--timeout == 0)
            return 0;
    }

    timeout = 30000;
    while (PINA & (1 << ECHO_PIN)) {
        _delay_us(1);
        pulse_us++;
        if (--timeout == 0)
            break;
    }

    return (uint16_t)(pulse_us / 58UL);
}

bool ultrasonic_detect_presence(void)
{
    uint16_t cm = ultrasonic_read_cm();
    return (cm > 0 && cm < PRESENCE_THRESHOLD_CM);
}
