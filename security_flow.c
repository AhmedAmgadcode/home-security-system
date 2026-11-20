#include <avr/io.h>
#include <avr/wdt.h>
#include <string.h>
#include <stdbool.h>
#include "config.h"
#include "lcd.h"
#include "keypad.h"
#include "outputs.h"
#include "ultrasonic.h"
#include "timer.h"
#include "ui.h"
#include "security_flow.h"

static const char OWNER_PHONE[CONTACT_PHONE_LEN + 1] = OWNER_PHONE_NUMBER;
/* THEORETICAL: this is only a string compared in RAM/EEPROM - no
   telephony hardware validates or dials this number for anything
   real. */

static void emergency_sequence(void)
{
    uint32_t start = millis_now();
    uint32_t end = start + EMERGENCY_FLASH_SECONDS * 1000UL;
    bool intruder_left = false;
    bool guest_recognized = false;
    char owner_key;

    lcd_clear();
    lcd_goto(0, 0);
    lcd_print("INTRUDER DETECT");
    lcd_goto(1, 0);
    /* THEORETICAL: no phone/network hardware exists to place this
       call - nothing is actually dialed or contacted. */
    lcd_print("CALLING RESPONSE");

    while ((int32_t)(millis_now() - end) < 0) {
        wdt_reset();

        PORTB |= (1 << RED_LED);
        beep(100);
        PORTB &= ~(1 << RED_LED);
        WDT_DELAY_MS(150);

        if (!ultrasonic_detect_presence()) {
            intruder_left = true;
            break;
        }

        /* THEORETICAL: reads the LOCAL keypad, not a remote receiver
           at the owner's end - there is no hardware channel for an
           actual remote owner to send this decision in. */
        owner_key = keypad_read_raw();

        if (owner_key == '*') {
            guest_recognized = true;
            break;
        }
        else if (owner_key == '#') {
            break;
        }
    }

    PORTB &= ~(1 << RED_LED);

    lcd_clear();
    lcd_goto(0, 0);

    if (guest_recognized) {
        lcd_print("GUEST RECOGNIZED");
        lcd_goto(1, 0);
        /* THEORETICAL: "DOOR UNLOCKED" text/green LED only - no
           solenoid/servo/relay exists in this design, so nothing
           mechanical actually unlocks. Status indication only. */
        lcd_print("DOOR UNLOCKED");

        beep(100);
        PORTB |= (1 << GREEN_LED);

        for (uint8_t i = 0; i < 50; i++)
            WDT_DELAY_MS(100);

        PORTB &= ~(1 << GREEN_LED);
    }
    else if (intruder_left) {
        lcd_print("INTRUDER LEFT");
        lcd_goto(1, 0);
        lcd_print("CALL CANCELLED");
        WDT_DELAY_MS(1500);
    }
    else {
        /* THEORETICAL: no phone/network hardware exists to place
           this call - nothing is actually dialed or contacted. */
        lcd_print("RESPONDERS ON");
        lcd_goto(1, 0);
        lcd_print("THE WAY");
        WDT_DELAY_MS(1500);
    }
}

static void owner_verification_flow(void)
{
    char phone[CONTACT_PHONE_LEN + 1];

    read_phone(phone);

    lcd_clear();
    lcd_goto(0, 0);
    lcd_print("CHECKING...");
    WDT_DELAY_MS(1000);

    if (memcmp(phone, OWNER_PHONE, CONTACT_PHONE_LEN) == 0) {
        lcd_clear();
        lcd_goto(0, 0);
        /* THEORETICAL: no GSM/Wi-Fi/cellular hardware exists on any
           pin - no call, SMS, or data is ever actually sent. */
        lcd_print("CONTACTING OWNER");
        lcd_goto(1, 0);
        /* THEORETICAL: "live feed" implies a camera, but this design
           has no camera - only the ultrasonic distance sensor. */
        lcd_print("SENDING LIVE FEED");
        WDT_DELAY_MS(1500);

        char key;

        /* THEORETICAL: this reads the LOCAL keypad, not a remote
           receiver at the owner's end - there is no hardware channel
           for an actual remote person to send this decision in. */
        do {
            wdt_reset();
            key = keypad_read_raw();
        } while (key != '*' && key != '#');

        WDT_DELAY_MS(25);
        while (keypad_read_raw()) {
            wdt_reset();
            WDT_DELAY_MS(5);
        }

        lcd_clear();
        lcd_goto(0, 0);

        if (key == '*') {
            lcd_print("OWNER APPROVED");

            beep(100);
            PORTB |= (1 << GREEN_LED);

            /* THEORETICAL: "DOOR UNLOCKED" text/green LED only - no
               solenoid/servo/relay exists in this design, so nothing
               mechanical actually unlocks. Status indication only. */
            for (uint8_t i = 0; i < 50; i++)
                WDT_DELAY_MS(100);

            PORTB &= ~(1 << GREEN_LED);
        }
        else {
            lcd_print("OWNER DECLINED");
            WDT_DELAY_MS(1500);
        }
    }
    else {
        emergency_sequence();
    }
}

void lockout(uint32_t start_ms)
{
    uint32_t end_ms = start_ms + LOCKOUT_SECONDS * 1000UL;
    uint32_t next_beep = start_ms + 10000UL;
    bool asked = false;

    PORTB |= (1 << RED_LED);

    while ((int32_t)(millis_now() - end_ms) < 0) {
        wdt_reset();

        uint32_t now = millis_now();
        uint32_t remaining = (end_ms - now + 999UL) / 1000UL;

        countdown_screen(remaining, !asked);

        if (!asked) {
            char key = keypad_read_raw();

            if (key == 'A') {
                asked = true;
                owner_verification_flow();
            }
            else if (key == 'D') {
                asked = true;
            }
        }

        if ((int32_t)(now - next_beep) >= 0) {
            beep(150);
            next_beep += 10000UL;
        }

        WDT_DELAY_MS(250);
    }

    PORTB &= ~(1 << RED_LED);

    lcd_clear();
    lcd_goto(0, 0);
    lcd_print("LOCKOUT ENDED");
    WDT_DELAY_MS(800);
}
