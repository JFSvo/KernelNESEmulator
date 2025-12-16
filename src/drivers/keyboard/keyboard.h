#ifndef KEYBOARD_H
#define KEYBOARD_H
#include <stdint.h>


void handle_scancode(uint8_t byte);

void keyboard_init(void);

#endif