#include <stddef.h>  
#include <stdint.h>
#include <stdbool.h>
#include "emulator.h"
#include "emulator_debug.h"
#include "memory/heap/kheap.h"
#include "memory/memory.h"
#include "kernel.h"
#include "fs/file.h"
#include "tracelogger.h"

struct emulator emu;
bool CPU_halted;
bool logger_enabled = false;

void emu_init()  { 
    emu.RAM = kzalloc(0x800);
    emu.ROM = kzalloc(0x8000);
    emu.header = kzalloc(0x10);
    emu.filepath = "0:/test2.nes";
    CPU_halted = false;
    emu_reset();
}

void emulate_CPU() {
    uint8_t CPU_cycles = 0;

    uint8_t opcode = emu_read(emu.registers.program_counter);
    emu.current_opcode = opcode;
    add_tracelog_entry(&emu);
    
    emu.registers.program_counter++;

    uint8_t address;
    uint8_t PC_lowbyte;
    uint8_t PC_highbyte;

    switch(opcode){
        case 0x02: // HLT
            CPU_halted = true;
            CPU_cycles = 2;
            break;

        case 0xA0: // LDY Immediate 
            set_Y_register(emu_read(program_counter()));
            increment_PC();
            set_status_flag(FLAG_ZERO, reg_Y() == 0);
            CPU_cycles = 2;
            break;

        case 0xA2: // LDX Immediate 
            set_X_register(emu_read(program_counter()));
            increment_PC();
            set_status_flag(FLAG_ZERO, reg_X() == 0);
            CPU_cycles = 2;
            break;

        case 0xA9: // LDA Immediate 
            set_A_register(emu_read(program_counter()));
            increment_PC();
            set_status_flag(FLAG_ZERO, reg_A() == 0);
            CPU_cycles = 2;
            break;

        case 0xA5: ; // LDA Zero Page
            address = emu_read(program_counter());
            increment_PC();

            set_A_register(emu_read(address));
            set_status_flag(FLAG_ZERO, reg_A() == 0);
            CPU_cycles = 3;
            break;

        case 0xAD: ; // LDA Absolute
            PC_lowbyte = emu_read(program_counter());
            increment_PC();
            PC_highbyte = emu_read(program_counter());
            increment_PC();
            address = (uint16_t)((PC_highbyte << 8) | PC_lowbyte);

            set_A_register(emu_read(address));
            set_status_flag(FLAG_ZERO, reg_A() == 0);
            CPU_cycles = 4;
            break;
        case 0x85: ; // STA Zero Page
            address = emu_read(program_counter());
            increment_PC();
            emu_write(address, reg_A());
            CPU_cycles = 3;
            break;
        case 0x8D: ; // STA Absolute
            PC_lowbyte = emu_read(program_counter());
            increment_PC();
            PC_highbyte = emu_read(program_counter());
            increment_PC();

            address = (uint16_t)((PC_highbyte << 8) | PC_lowbyte);
            emu_write(address, reg_A());
            CPU_cycles = 4;
            break;

        default:
            // unknown opcode
            break;
    }

    emu.total_CPU_cycles += CPU_cycles;
}

void emu_run() { 
    while(!CPU_halted){
        emulate_CPU();
    }
    print_tracelog();
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

void emu_write(uint16_t address, uint8_t value){
    // optional: we can add RAM mirroring here
    if(address < 0x800){
        emu.RAM[address] = value;
    }
}

void emu_reset(){
    int fd = fopen(emu.filepath, "r");
    if(fd){
        print("Opened ");
        print(emu.filepath);
        print("\n");
        fread(emu.header, 0x10, 1, fd);
        fseek(fd, 0x10, SEEK_SET);
        fread(emu.ROM, 0x8000, 1, fd);
        set_status_flag(FLAG_INTERRUPT_DISABLE, true);
    }

    uint8_t PC_lowbyte = emu_read(0xFFFC);
    uint8_t PC_highbyte = emu_read(0xFFFD);
    emu.registers.program_counter = (PC_highbyte << 8) | PC_lowbyte;
    print("\n");
    emu_run();
}

void set_status_flag(uint8_t flag, bool condition){
    if(condition){
        emu.registers.flags |= flag;
    } else {
        emu.registers.flags &= ~flag;
    }
}

void emu_enable_logger(bool is_enabled){
    logger_enabled = is_enabled;
}

