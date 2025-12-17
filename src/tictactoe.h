#ifndef TICTACTOE_H
#define TICTACTOE_H

#include <stdint.h>

void tictactoe_init(void);                 // set up initial board state
void tictactoe_update_and_draw(void);      // draw board / winner every frame
void tictactoe_on_key(uint8_t scancode);   // handle keyboard input

#endif