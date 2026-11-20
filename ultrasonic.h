#ifndef ULTRASONIC_H
#define ULTRASONIC_H

#include <stdint.h>
#include <stdbool.h>

void ultrasonic_init(void);
uint16_t ultrasonic_read_cm(void);
bool ultrasonic_detect_presence(void);

#endif
