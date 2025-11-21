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
    emu.header = kzalloc(0x10);
    emu.filepath = "0:/test1.nes";
    reset();
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
    int fd = fopen(emu.filepath, "r");
    if(fd){
        print("Opened test1.nes\n");
        fread(emu.header, 0x10, 1, fd);
        fseek(fd, 0x10, SEEK_SET);
        fread(emu.ROM, 0x8000, 1, fd);
    }

    uint8_t PClowbyte = read(0xFFFC);
    uint8_t PChighbyte = read(0xFFFD);
    emu.registers->programCounter = (PChighbyte << 8) | PClowbyte;
    print_hex16(emu.registers->programCounter);
    print("\n");
}
