#include <avr/eeprom.h>
#include <avr/wdt.h>
#include <string.h>
#include "config.h"
#include "lcd.h"
#include "outputs.h"
#include "ui.h"
#include "auth.h"

static const char MASTER_PIN[PIN_LEN + 1] = MASTER_PIN_VALUE;

static uint32_t pin_hash(const char *pin)
{
    uint32_t h = PIN_SALT;

    for (uint8_t i = 0; i < PIN_LEN; i++) {
        h ^= (uint8_t)pin[i];
        h *= 16777619UL;
    }

    return h;
}

typedef struct {
    uint8_t valid;
    uint32_t pin_hash;
} user_t;

static user_t EEMEM ee_users[MAX_USERS];

int8_t find_matching_user(const char *pin)
{
    user_t u;
    uint32_t h = pin_hash(pin);

    for (uint8_t i = 0; i < MAX_USERS; i++) {
        eeprom_read_block(&u, &ee_users[i], sizeof(u));

        if (u.valid == 0xA5 && u.pin_hash == h)
            return (int8_t)i;
    }

    return -1;
}

static int8_t find_free_user(void)
{
    user_t u;

    for (uint8_t i = 0; i < MAX_USERS; i++) {
        eeprom_read_block(&u, &ee_users[i], sizeof(u));

        if (u.valid != 0xA5)
            return (int8_t)i;
    }

    return -1;
}

bool has_any_user(void)
{
    user_t u;

    for (uint8_t i = 0; i < MAX_USERS; i++) {
        eeprom_read_block(&u, &ee_users[i], sizeof(u));

        if (u.valid == 0xA5)
            return true;
    }

    return false;
}

static void reboot_system(void)
{
    lcd_clear();
    lcd_goto(0, 0);
    lcd_print("WRONG MASTER PIN");
    lcd_goto(1, 0);
    lcd_print("REBOOTING...");

    error_beeps();
    WDT_DELAY_MS(500);

    /* THEORETICAL: nothing here logs the breach anywhere persistent
       or alerts anyone off-device; the event is lost the moment the
       MCU resets. (The reset itself is real, watchdog-triggered.) */
    wdt_enable(WDTO_15MS);
    for (;;) {
    }
}

void signup(void)
{
    char p1[PIN_LEN + 1];
    char p2[PIN_LEN + 1];
    char master_pin[PIN_LEN + 1];
    int8_t slot;
    user_t u;

    read_pin(master_pin, "MASTER PIN:");

    lcd_clear();
    lcd_goto(0, 0);
    lcd_print("CHECKING...");
    WDT_DELAY_MS(1000);

    if (memcmp(master_pin, MASTER_PIN, PIN_LEN + 1) != 0) {
        reboot_system();
        return;
    }

    slot = find_free_user();

    if (slot < 0) {
        lcd_clear();
        lcd_goto(0, 0);
        lcd_print("4 USERS ALREADY");
        lcd_goto(1, 0);
        lcd_print("MEMORY FULL");

        beep(500);
        WDT_DELAY_MS(1500);
        return;
    }

    read_pin(p1, "PIN (6 digits):");
    read_pin(p2, "Enter PIN again:");

    if (memcmp(p1, p2, PIN_LEN + 1) != 0) {
        lcd_clear();
        lcd_goto(0, 0);
        lcd_print("PIN MISMATCH");
        lcd_goto(1, 0);
        lcd_print("TRY AGAIN");

        error_beeps();
        WDT_DELAY_MS(1200);
        return;
    }

    u.valid = 0xA5;
    u.pin_hash = pin_hash(p1);

    eeprom_update_block(&u, &ee_users[slot], sizeof(u));

    lcd_clear();
    lcd_goto(0, 0);
    lcd_print("USER ADDED");
    lcd_goto(1, 0);
    lcd_print("SLOT ");
    lcd_data('1' + slot);

    beep(150);
    WDT_DELAY_MS(1200);
}

bool authenticate(void)
{
    char pin[PIN_LEN + 1];

    read_pin(pin, "PIN (6 digits):");

    lcd_clear();
    lcd_goto(0, 0);
    lcd_print("CHECKING...");
    WDT_DELAY_MS(1500);

    return find_matching_user(pin) >= 0;
}
