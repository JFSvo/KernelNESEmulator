#include "keyboard.h"
#include "kernel.h"
#include "emulator/CPU/tracelogger.h"
#include "emulator/CPU/emulator.h"

void handle_scancode(uint8_t byte){
    switch(byte){
        case 0x11: // a key
            if(kernel_initialized){
                #if TRACELOGGER
                scroll_up(); // s key
                #endif
            }

            break;
        case 0x1F:  
            if(kernel_initialized){
                #if TRACELOGGER
                scroll_down(); // s key
                #endif
            }

            break;

        case 0x1C:
            // enter key
            break;
    }
}