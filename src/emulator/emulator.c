#include <stddef.h>  
#include <stdint.h>
#include <stdbool.h>
#include "emulator.h"
#include "memory/heap/kheap.h"
#include "memory/memory.h"
#include "kernel.h"
#include "fs/file.h"

struct emulator emu;
bool CPU_halted;

void emu_init()  { 

    emu.RAM = kzalloc(0x800);
    emu.ROM = kzalloc(0x8000);
    emu.header = kzalloc(0x10);
    emu.filepath = "0:/test1.nes";
    CPU_halted = false;
    emu_reset();
}

void emulate_CPU() {
    uint8_t CPU_cycles;
    print("program counter: ");
    print_hex16(emu.registers.program_counter);
    print("   ");
    uint8_t opcode = emu_read(emu.registers.program_counter);
    print("opcode is: ");
    print_hex8(opcode);
    print("   ");
    emu.registers.program_counter++;

    switch(opcode){
        case 0x02: // HLT
            CPU_halted = true;
            CPU_cycles = 2;
            print("HALT CPU");
            break;
        case 0xA0: // LDY Immediate 
            emu.registers.Y = emu_read(emu.registers.program_counter);
            print("Register Y: ");
            print_hex8(emu.registers.Y);
            print("   ");
            CPU_cycles = 2;
            break;
        case 0xA2: // LDX Immediate 
            emu.registers.X = emu_read(emu.registers.program_counter);
            print("Register X: ");
            print_hex8(emu.registers.X);
            print("   ");
            CPU_cycles = 2;
            break;
        case 0xA9: // LDA Immediate 
            emu.registers.A = emu_read(emu.registers.program_counter);
            print("Register A: ");
            print_hex8(emu.registers.A);
            print("   ");
            CPU_cycles = 2;
            break;

        default:
            // unknown opcode
            break;
    }
    print("\n");
    emu.registers.program_counter++;
    // temp. to stop compiler complaining
    CPU_cycles = CPU_cycles;
}

void emu_run() { 
    while(!CPU_halted){
        emulate_CPU();
    }
    print("\n");
    emu_print_hexdump(0x8000, 0x10);
}

uint8_t emu_read(uint16_t address){
    // optional: we can add RAM mirroring here
    if(address < 0x800){
        return emu.RAM[address];
    }
    if(address >= 0x8000){
        return emu.ROM[address-0x8000];
    }
    return 0;
}

void emu_reset(){
    int fd = fopen(emu.filepath, "r");
    if(fd){
        print("Opened test1.nes\n");
        fread(emu.header, 0x10, 1, fd);
        fseek(fd, 0x10, SEEK_SET);
        fread(emu.ROM, 0x8000, 1, fd);
    }

    uint8_t PClowbyte = emu_read(0xFFFC);
    uint8_t PChighbyte = emu_read(0xFFFD);
    emu.registers.program_counter = (PChighbyte << 8) | PClowbyte;
    print_hex16(emu.registers.program_counter);
    print("\n");
    emu_run();
}

