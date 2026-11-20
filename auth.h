#ifndef AUTH_H
#define AUTH_H

#include <stdint.h>
#include <stdbool.h>

bool has_any_user(void);
int8_t find_matching_user(const char *pin);
bool authenticate(void);
void signup(void);

#endif
