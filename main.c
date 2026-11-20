#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>

#include "config.h"
#include "timer.h"
#include "lcd.h"
#include "keypad.h"
#include "outputs.h"
#include "ultrasonic.h"
#include "ui.h"
#include "auth.h"
#include "security_flow.h"

int main(void)
{
    uint8_t wrong_attempts = 0;

    MCUCSR &= ~(1 << WDRF);
    wdt_disable();

    keypad_init();
    outputs_init();
    timer0_init();
    ultrasonic_init();

    sei();

    wdt_enable(WDTO_2S);

    lcd_init();
    lcd_clear();

    while (!ultrasonic_detect_presence()) {
        wdt_reset();
        lcd_clear();
        WDT_DELAY_MS(200);
    }

    show_home();

    while (1) {
        char key = keypad_get_key();

        if (key == 'A') {
            if (!has_any_user()) {
                /* No users enrolled yet: ignore 'A'. The home screen
                   already prompts for B (new user) in this state. */
            }
            else if (authenticate()) {
                wrong_attempts = 0;

                PORTB |= (1 << GREEN_LED);

                lcd_clear();
                lcd_goto(0, 0);
                lcd_print("ACCESS GRANTED");
                lcd_goto(1, 0);
                /* THEORETICAL: "DOOR UNLOCKED" text/green LED only -
                   no solenoid/servo/relay exists in this design, so
                   nothing mechanical actually unlocks. Status only. */
                lcd_print("DOOR UNLOCKED");

                beep(100);

                for (uint8_t i = 0; i < 50; i++)
                    WDT_DELAY_MS(100);

                PORTB &= ~(1 << GREEN_LED);
            }
            else {
                wrong_attempts++;

                PORTB |= (1 << RED_LED);
                error_beeps();

                lcd_clear();
                lcd_goto(0, 0);
                lcd_print("WRONG PASSWORD");
                lcd_goto(1, 0);
                lcd_print("TRIES: ");
                lcd_data('0' + wrong_attempts);

                WDT_DELAY_MS(1200);
                PORTB &= ~(1 << RED_LED);

                if (wrong_attempts >= 3) {
                    uint32_t lock_start = millis_now();

                    lockout(lock_start);
                    wrong_attempts = 0;
                }
            }

            show_home();
        }
        else if (key == 'B') {
            signup();
            show_home();
        }
        else if (key == 'C') {
            wrong_attempts = 0;
            show_home();
        }
    }
}
