/*
 * THEORETICAL: none of this drives any real phone/network hardware -
 * everything described as "calling", "contacting", or "sending" is
 * LCD text only. See the implementation comments in security_flow.c
 * for exactly what is and isn't real at each step.
 */

#ifndef SECURITY_FLOW_H
#define SECURITY_FLOW_H

#include <stdint.h>

void lockout(uint32_t start_ms);

#endif
