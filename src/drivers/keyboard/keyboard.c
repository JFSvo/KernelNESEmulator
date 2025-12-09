#include "keyboard.h"
#include "kernel.h"
#include "emulator/tracelogger.h"

void handle_scancode(uint8_t byte){
    switch(byte){
        case 0x11: // a key
            scroll_up();
            break;
        case 0x1F:
            scroll_down(); // s key
            break;

    }
}