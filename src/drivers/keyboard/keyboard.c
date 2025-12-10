#include "keyboard.h"
#include "kernel.h"
#include "emulator/tracelogger.h"
#include "emulator/emulator.h"

void handle_scancode(uint8_t byte){
    switch(byte){
        case 0x11: // a key
            #if TRACELOGGER
            scroll_up();
            #endif
            break;
        case 0x1F:
            #if TRACELOGGER
            scroll_down(); // s key
            #endif
            break;

    }
}