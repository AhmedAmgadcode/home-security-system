# Makefile for the ATmega32A Home Security System.
# Build settings live in home_security.h, not here.

include home_security.h

CC      = avr-gcc
OBJCOPY = avr-objcopy
AVRDUDE = avrdude

CFLAGS  = -mmcu=$(MCU) -DF_CPU=$(F_CPU) $(OPT) $(WARN) $(STD)

SRCS = main.c \
       timer.c \
       lcd.c \
       keypad.c \
       outputs.c \
       ultrasonic.c \
       ui.c \
       auth.c \
       security_flow.c

OBJS = $(SRCS:.c=.o)

.PHONY: all clean flash

all: $(TARGET).hex

%.o: %.c config.h
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET).elf: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@

$(TARGET).hex: $(TARGET).elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@

flash: $(TARGET).hex
	$(AVRDUDE) -c $(AVRDUDE_PROGRAMMER) -p $(AVRDUDE_MCU) -P $(AVRDUDE_PORT) \
	           -U flash:w:$(TARGET).hex:i

clean:
	rm -f $(OBJS) $(TARGET).elf $(TARGET).hex
