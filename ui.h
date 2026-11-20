#ifndef UI_H
#define UI_H

#include <stdint.h>
#include <stdbool.h>

void read_pin(char *pin, const char *prompt);
void read_phone(char *buf);
void show_home(void);
void countdown_screen(uint32_t seconds_left, bool show_prompt);

#endif
