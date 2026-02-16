#include "keyboard.h"
#include "kernel.h"
#include "emulator/CPU/tracelogger.h"
#include "emulator/CPU/emulator.h"

void handle_scancode(uint8_t byte){
    switch(byte){
        case 0x11: // a key
            if(rom_selected){
                #if TRACELOGGER
                scroll_up(); // s key
                #endif
            } else {
                decrement_cursor();
            }

            break;
        case 0x1F:  
            if(rom_selected){
                #if TRACELOGGER
                scroll_down(); // s key
                #endif
            } else {
                increment_cursor();
            }

            break;

        case 0x1C:
            if(kernel_initialized){
                rom_selected = true;
            }
            break;
    }
}