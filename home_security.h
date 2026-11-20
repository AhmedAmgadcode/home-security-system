# home_security.h
# ---------------------
# Build-time configuration, included by the Makefile. Edit values
# here to retarget the MCU, clock, or programmer without touching
# the Makefile itself.

# Target MCU (must match the fuse-configured clock source below).
MCU = atmega32a

# CPU frequency in Hz. NO EXTERNAL CRYSTAL: this project runs on the
# ATmega32A's internal 8 MHz RC oscillator - set the MCU fuse/Proteus
# clock to "8 MHz internal RC" to match.
F_CPU = 8000000UL

# avr-gcc optimization/warning flags.
OPT     = -Os
WARN    = -Wall -Wextra
STD     = -std=c11

# Programmer settings, used only by the `make flash` target.
AVRDUDE_PROGRAMMER = usbasp
AVRDUDE_PORT       = usb
AVRDUDE_MCU        = m32

# Output artifact base name.
TARGET = home_security
