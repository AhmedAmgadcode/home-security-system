#ifndef CONFIG_H
#define CONFIG_H

#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include <avr/wdt.h>
#include <util/delay.h>

#define WDT_DELAY_MS(x) do { wdt_reset(); _delay_ms(x); } while (0)

#define MAX_USERS       4
#define PIN_LEN         6
#define LOCKOUT_SECONDS 30UL

#define LCD_RS PC0
#define LCD_EN PC1

#define BUZZER_PIN PB0
#define GREEN_LED  PB2
#define RED_LED    PB3

#define TRIG_PIN PA0
#define ECHO_PIN PA1

#define PRESENCE_THRESHOLD_CM 100U
#define EMERGENCY_FLASH_SECONDS 10UL
#define CONTACT_PHONE_LEN 11

#define PIN_SALT 0x5A17C3D9UL

/* ---- DEMO CREDENTIALS -------------------------------------------
 * Placeholders for the simulation. Change them locally before use and
 * never commit real values (phone numbers, PINs) to a public repo.
 */
#define MASTER_PIN_VALUE   "123456"      /* must be PIN_LEN digits           */
#define OWNER_PHONE_NUMBER "01000000000" /* must be CONTACT_PHONE_LEN digits */

#endif
