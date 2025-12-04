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
    emu.total_CPU_cycles = 8;
    emu.filepath = "0:/test4.nes";
    CPU_halted = false;
    emu_reset();
}

void emulate_CPU() {
    #if DEBUG
    uint16_t initial_PC = emu.registers.program_counter;
    #endif
    uint8_t opcode = emu_read(emu.registers.program_counter);
    emu.current_opcode = opcode;
    
    reset_tracker();
    add_tracelog_entry(&emu);
    
    emu.registers.program_counter++;

    uint8_t result;
    uint16_t address;
    uint8_t PC_lowbyte;
    uint8_t PC_highbyte;
    int signed_value;

    switch(opcode){
        case 0x02: // HLT
            CPU_halted = true;
            break;

        case 0xA0: // LDY Immediate 
            result = set_Y_register(read_increment_PC());
            set_status_flag(FLAG_ZERO, result == 0);
            set_status_flag(FLAG_NEGATIVE, result > 127);
            break;

        case 0xA2: // LDX Immediate 
            result = set_X_register(read_increment_PC());
            set_status_flag(FLAG_ZERO, result == 0);
            set_status_flag(FLAG_NEGATIVE, result > 127);
            break;

        case 0xA9: // LDA Immediate 
            result = set_A_register(read_increment_PC());
            set_status_flag(FLAG_ZERO, result == 0);
            set_status_flag(FLAG_NEGATIVE, result > 127);
            break;

        case 0xA5: ; // LDA Zero Page
            address = read_increment_PC();
            result = set_A_register(emu_read(address));
            set_status_flag(FLAG_ZERO, result == 0);
            set_status_flag(FLAG_NEGATIVE, result > 127);
            break;

        case 0xAD: ; // LDA Absolute
            PC_lowbyte = read_increment_PC();
            PC_highbyte = read_increment_PC();
            address = (PC_highbyte << 8) | PC_lowbyte;

            result = set_A_register(emu_read(address));
            set_status_flag(FLAG_ZERO, result == 0);
            set_status_flag(FLAG_NEGATIVE, result > 127);
            break;

        case 0x85: ; // STA Zero Page
            address = read_increment_PC();
            emu_write(address, reg_A());
            break;

        case 0x8D: ; // STA Absolute
            PC_lowbyte = read_increment_PC();
            PC_highbyte = read_increment_PC();
            address = (PC_highbyte << 8) | PC_lowbyte;

            emu_write(address, reg_A());
            break;

        case 0xD0: ; // BNE
            result = read_increment_PC();
            if(!(reg_status() & FLAG_ZERO)){ // Branch taken if zero flag cleared
                signed_value = result;
                if(signed_value > 127){
                    signed_value -= 256;
                }
                set_PC((uint16_t)((int)program_counter() + signed_value));
                emu.total_CPU_cycles++;
            }
            break;

        case 0xF0: ; // BEQ
            result = read_increment_PC();
            if(reg_status() & FLAG_ZERO){ // Branch taken if zero flag set
                signed_value = result;
                if(signed_value > 127){
                    signed_value -= 256;
                }
                set_PC((uint16_t)((int)program_counter() + signed_value));
                emu.total_CPU_cycles++;
            }
            break;

        case 0x10: ; // BPL
            result = read_increment_PC();
            if(!(reg_status() & FLAG_NEGATIVE)){ // Branch if negative flag cleared
                signed_value = result;
                if(signed_value > 127){
                    signed_value -= 256;
                }
                set_PC((uint16_t)((int)program_counter() + signed_value));
                emu.total_CPU_cycles++;
            }
            break;

        case 0x30: ; // BMI
            result = read_increment_PC();
            if(reg_status() & FLAG_NEGATIVE){ // Branch if negative flag set
                signed_value = result;
                if(signed_value > 127){
                    signed_value -= 256;
                }
                set_PC((uint16_t)((int)program_counter() + signed_value));
                emu.total_CPU_cycles++;
            }
            break;

        case 0x50: ; // BVC
            result = read_increment_PC();
            if(!(reg_status() & FLAG_OVERFLOW)){ // Branch if overflow flag cleared
                signed_value = result;
                if(signed_value > 127){
                    signed_value -= 256;
                }
                set_PC((uint16_t)((int)program_counter() + signed_value));
                emu.total_CPU_cycles++;
            }
            break;

        case 0x70: ; // BVS
            result = read_increment_PC();
            if(reg_status() & FLAG_OVERFLOW){ // Branch if overflow flag set
                signed_value = result;
                if(signed_value > 127){
                    signed_value -= 256;
                }
                set_PC((uint16_t)((int)program_counter() + signed_value));
                emu.total_CPU_cycles++;
            }
            break;

        case 0x90: ; // BCC
            result = read_increment_PC();
            if(!(reg_status() & FLAG_CARRY)){ // Branch if carry flag cleared
                signed_value = result;
                if(signed_value > 127){
                    signed_value -= 256;
                }
                set_PC((uint16_t)((int)program_counter() + signed_value));
                emu.total_CPU_cycles++;
            }
            break;

        case 0xB0: ; // BCS
            result = read_increment_PC();
            if(reg_status() & FLAG_CARRY){ // Branch if carry flag set
                signed_value = result;
                if(signed_value > 127){
                    signed_value -= 256;
                }
                set_PC((uint16_t)((int)program_counter() + signed_value));
                emu.total_CPU_cycles++;
            }
            break;

        case 0x48: ; // PLA
            push_stack(reg_A());
            break;

        case 0x68: ; // PHA
            result = set_A_register(pull_stack());
            set_status_flag(FLAG_ZERO, result == 0);
            set_status_flag(FLAG_NEGATIVE, result > 127);
            break;

        case 0x20: ; // JSR
            PC_lowbyte = read_increment_PC();
            PC_highbyte = read_PC();
            
            push_stack((uint8_t)(program_counter() >> 8));
            push_stack((uint8_t)program_counter());
            set_PC((PC_highbyte << 8) | PC_lowbyte);
            break;
        
        case 0x60: ; // RTS
            PC_lowbyte = pull_stack();
            PC_highbyte = pull_stack();
            set_PC((PC_highbyte << 8) | PC_lowbyte);
            read_increment_PC();
            break;

        default:
            // unknown opcode
            break;
    }
    #if DEBUG
    compare_with_table(opcode, initial_PC);
    #endif

    emu.total_CPU_cycles += opcode_table[opcode].cycles;
}

void emu_run() { 
    while(!CPU_halted){
        emulate_CPU();
    }
    
    #if TRACELOGGER
    print_tracelog();
    #endif
}

uint8_t emu_read(uint16_t address){
    #if DEBUG
    if(address != emu.registers.program_counter){ debug_tracker.reads_mem = true; }
    #endif 

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

    #if DEBUG
    debug_tracker.writes_mem = true; 
    #endif 
}


void push_stack(uint8_t value){
    emu_write(0x100+emu.registers.stack_pointer, value);
    emu.registers.stack_pointer--;
    
    #if DEBUG
    debug_tracker.reg_read_bitflag |= REG_STACK;
    debug_tracker.reg_write_bitflag |= REG_STACK;
    #endif 
}

uint8_t pull_stack(){
    #if DEBUG 
    bool old_value = debug_tracker.reads_mem; 
    #endif
    emu.registers.stack_pointer++;
    uint8_t result = emu_read(0x100+emu.registers.stack_pointer);

    #if DEBUG
    debug_tracker.reg_read_bitflag |= REG_STACK;
    debug_tracker.reg_write_bitflag |= REG_STACK;
    debug_tracker.reads_mem = old_value;
    #endif 

    return result;
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
    emu.registers.stack_pointer = 0xFD;
    print("\n");
    emu_run();
}


void emu_enable_logger(bool is_enabled){
    logger_enabled = is_enabled;
}

