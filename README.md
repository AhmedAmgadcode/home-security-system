# Home Security System (ATmega32A)

A keypad-and-LCD home security system written in embedded C for the ATmega32A microcontroller. It detects a person approaching with an ultrasonic sensor, authenticates users with a 6-digit PIN stored (hashed) in EEPROM, locks out after repeated wrong attempts, and runs a simulated owner-verification and emergency flow.

Built as a university team project by a team of four. I was the team leader: I planned the module breakdown, assigned the work and integrated everyone's parts into one firmware. The final presentation was on 11 September 2026.

> **Simulation notice.** This project has no phone, network, camera or door-lock hardware. Anything described as "calling", "contacting the owner", "sending a live feed" or "door unlocked" is **LCD text and an LED only**. The comments in `security_flow.c` and `main.c` mark every simulated step.

## Features

- **Presence detection:** the system stays idle until an ultrasonic sensor sees someone closer than 100 cm.
- **PIN authentication:** up to 4 users, 6-digit PINs, masked on the LCD with backspace support.
- **Secure-ish storage:** PINs are stored as salted hashes (FNV-style) in EEPROM, never as plain text.
- **Master PIN enrollment:** adding a user requires a master PIN. A wrong master PIN shows a message and forces a watchdog reset.
- **Lockout:** 3 wrong PINs trigger a 30-second lockout with a countdown and a beep every 10 seconds.
- **Owner verification (simulated):** during lockout the user can enter the owner's phone number. A match starts an owner-approval step, and a mismatch starts a 10-second emergency sequence.
- **Fault recovery:** a 2-second watchdog timer restarts the firmware if it ever hangs.
- **Timing:** Timer0 generates a 1 ms interrupt tick used for lockout and emergency timing.

## Hardware and pin map

| Part | Pins |
|---|---|
| 16x2 character LCD (4-bit mode) | RS = PC0, EN = PC1, D4–D7 = PC2–PC5 |
| 4x4 keypad | Rows = PD0–PD3 (outputs), columns = PD4–PD7 (inputs with pull-ups) |
| Ultrasonic sensor | TRIG = PA0, ECHO = PA1 |
| Buzzer | PB0 |
| Green LED | PB2 |
| Red LED | PB3 |

The clock is the ATmega32A's **internal 8 MHz RC oscillator** (no crystal).

## Using it

| Key | Where | Action |
|---|---|---|
| `A` | Home screen | Enter PIN to open (access granted shows the green LED and a message) |
| `B` | Home screen | Add a new user (asks for the master PIN first) |
| `C` | Home screen | Reset the wrong-attempt counter |
| `C` | While typing a PIN or phone number | Backspace |
| `A` | During lockout | Contact the owner (simulated) |
| `D` | During lockout | Dismiss the "contact owner?" prompt |
| `*` / `#` | Owner and emergency prompts | Approve / decline (read from the local keypad as a stand-in for a remote owner) |

## Demo credentials

The master PIN and owner phone number are placeholders in `config.h`:

```c
#define MASTER_PIN_VALUE   "123456"
#define OWNER_PHONE_NUMBER "01000000000"
```

Change them locally before demoing. Do not commit real values.

## Building

Requirements: `avr-gcc`, `avr-libc`, `make` (and `avrdude` for flashing).

```bash
make          # builds home_security.hex
make flash    # programs the chip (settings in home_security.h)
make clean
```

All build settings (MCU, clock, optimization flags, programmer) are in `home_security.h`. It uses Makefile syntax and is read by the Makefile's `include`, so it is not a C header despite the extension. Hardware constants (pins, PIN length, lockout time, thresholds) are in `config.h`.

The firmware builds with `-Wall -Wextra` and no warnings. It uses about 4.4 KB of flash and about 0.5 KB of RAM.

### Fuses for real hardware

For a physical ATmega32A, set the fuses for the internal 8 MHz oscillator and **disable JTAG** (PC2–PC5 are JTAG pins and are used by the LCD). Typical values are `lfuse=0xE4` and `hfuse=0xD9`. Check them with a fuse calculator first:

```bash
avrdude -c usbasp -p m32 -U lfuse:w:0xE4:m -U hfuse:w:0xD9:m
```

## Project structure

| File | Purpose |
|---|---|
| `main.c` | Startup, main loop, key handling |
| `auth.c/.h` | PIN hashing, EEPROM user storage, sign-up and login |
| `security_flow.c/.h` | Lockout, owner verification and emergency sequence |
| `ui.c/.h` | LCD screens, PIN and phone entry |
| `lcd.c/.h` | 4-bit HD44780 LCD driver |
| `keypad.c/.h` | 4x4 keypad scan with debounce |
| `ultrasonic.c/.h` | Distance measurement and presence detection |
| `outputs.c/.h` | Buzzer and LED helpers |
| `timer.c/.h` | Timer0 1 ms tick (`millis_now()`) |
| `config.h` | Pins, limits and demo credentials |
| `home_security.h` | Build settings for the Makefile |

## Known limitations

- Door unlocking, phone calls and the live feed are simulated (see the notice above).
- The owner's approve/decline decision is read from the local keypad, not from a remote device.
- PIN hashing uses a fixed compile-time salt over a 6-digit space, so it hides PINs from casual inspection but would not stop an attacker who can read the EEPROM.
- The master PIN is stored in the firmware.
- Ultrasonic distance is measured with a software delay loop, so readings are approximate. Timer1 input capture would be more accurate.
- LCD text lives in RAM; moving it to flash (`PROGMEM`) would free RAM.

## License

MIT, see `LICENSE`.
