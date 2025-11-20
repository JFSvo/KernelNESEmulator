#include <stddef.h>  
#include <stdint.h>
#include "emulator.h"
#include "memory/heap/kheap.h"
#include "memory/memory.h"
#include "kernel.h"
#include "fs/file.h"

struct emulator emu;

void NES_run()  { 

    emu.RAM = kzalloc(0x800);
    emu.ROM = kzalloc(0x8000);
}

uint8_t read(uint16_t address){
    // optional: we can add RAM mirroring here
    if(address < 0x800){
        return emu.RAM[address];
    }
    if(address >= 0x8000){
        return emu.ROM[address-0x8000];
    }
    return 0;
}

void reset(){
    // int fd = fopen(filename, "r");
    // if(fd){
    //     print("Opened test1.nes\n");
    //     uint8_t buf[15];

    //     fread(buf, 11, 1, fd);
    //     buf[13] = 0x00;
    //     printhex(RAM, 11);
    // }
}
