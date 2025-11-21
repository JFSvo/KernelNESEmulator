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
        //fread(emu.header, 0x10, 1, fd);
        fseek(fd, 0x7FFF, SEEK_SET);
        fread(emu.ROM, 0x20, 1, fd);
    }
    print_hexdump(emu.ROM, 0x360);
    print("\n");
    read(0x8002);
    print("\n");
    uint8_t PClowbyte = read(0x7FFC);
    uint8_t PChighbyte = read(0x7FFE);
    emu.registers->programCounter = (PChighbyte << 8) | PClowbyte;
    print_hex16(emu.registers->programCounter);
    print("\n");
    print_hex16(0xA2B1);
    // printhex(emu.header, 0x10);
    // print("\n\n");
    // printhex(emu.ROM, 0x20);
}
