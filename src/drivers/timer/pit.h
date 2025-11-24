#ifndef PIT_H
#define PIT_H

#include <stdint.h>

void pit_init(uint32_t frequency);
void pit_sleep(uint32_t milliseconds);
uint64_t pit_get_ticks();

/* C handler called by the ASM ISR stub */
void pit_handler(void);

#endif
