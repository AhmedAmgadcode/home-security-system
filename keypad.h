#ifndef KEYPAD_H
#define KEYPAD_H

void keypad_init(void);
char keypad_read_raw(void);
char keypad_get_key(void);

#endif
