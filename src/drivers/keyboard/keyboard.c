#include <stdint.h>
#include "keyboard.h"
#include "tictactoe.h"
#include "emulator/CPU/tracelogger.h"

void handle_scancode(uint8_t byte)
{
    // 1) Tracelogger controls 
#if TRACELOGGER
    switch (byte) {
        case 0x11:   // w key
            scroll_up();
            break;
        case 0x1F:   // S key
            scroll_down();
            break;
    }
#endif

    // 2) Always forward key to the game
    tictactoe_on_key(byte);
}