#include <stddef.h>  
#include <stdint.h>
#include <stdbool.h>
#include "kernel.h"
#include "opcode_table.h"
#include "emulator_debug.h"

struct mem_reg_access debug_tracker;


void check_memory_read(const struct opcode_entry* table){
    if(debug_tracker.reads_mem != table->reads_mem){
        char* output = table->reads_mem ? "Instruction should read from memory, but doesn't.\n" 
                                        : "Instruction shouldn't read from memory, but does.\n";
        print(output);
    }
}

void check_memory_write(const struct opcode_entry* table){
    if(debug_tracker.writes_mem != table->writes_mem){
        char* output = table->writes_mem ? "Instruction should write to memory, but doesn't.\n" 
                                        : "Instruction shouldn't write to memory, but does.\n";
        print(output);
    }
}

void check_register_access(ACCESS_TYPE type, uint8_t actual, uint8_t table_reference){
    char* type_string = type ? "write" : "read";
    uint8_t mismatch_bitflag = actual ^ table_reference;

    struct {
        uint8_t mask;
        const char *name;
    } regs[] = {
        { REG_STATUS, "status" },
        { REG_STACK,  "stack"  },
        { REG_Y,      "Y"      },
        { REG_X,      "X"      },
        { REG_A,      "A"      },
    };

    for (int i = 0; i < 5; i++) {
        if (mismatch_bitflag & regs[i].mask) {

            print("Expected ");
            if (!(table_reference & regs[i].mask)) print("no ");
            print(type_string);
            print(" to ");
            print(regs[i].name);
            print(" register, but ");
            if (!(actual & regs[i].mask)) print("no ");
            print(type_string);
            print(" occurred.\n");
        }
    }
}

void print_header(uint8_t opcode, uint16_t program_counter){
    const struct opcode_entry* table = &opcode_table[opcode];
    print("------ PC:");
    print_hex16(program_counter);
    print(": 0x");
    print_hex8(opcode);
    print(" ");
    print(table->mnemonic);
    print(" ------\n");
}

void compare_with_table(uint8_t opcode, uint16_t program_counter){
    const struct opcode_entry* table = &opcode_table[opcode];
    uint8_t reg_read_mismatch = debug_tracker.reg_read_bitflag ^ table->reg_read_bitflag;
    uint8_t reg_write_mismatch = debug_tracker.reg_write_bitflag ^ table->reg_write_bitflag;
    bool mem_read_mismatch = debug_tracker.reads_mem != table->reads_mem;
    bool mem_write_mismatch = debug_tracker.writes_mem != table->writes_mem;

    if(reg_read_mismatch || reg_write_mismatch || mem_read_mismatch || mem_write_mismatch){
        print_header(opcode, program_counter);
    }
    check_register_access(READ, debug_tracker.reg_read_bitflag, table->reg_read_bitflag);
    check_register_access(WRITE, debug_tracker.reg_write_bitflag, table->reg_write_bitflag);
    check_memory_read(table);
    check_memory_write(table);
}

void reset_tracker(){
    debug_tracker.reg_read_bitflag = 0;
    debug_tracker.reg_write_bitflag = 0;
    debug_tracker.reads_mem = false;
    debug_tracker.writes_mem = false;
}

uint8_t read_increment_PC(){
    uint8_t result = emu_read(emu.registers.program_counter);
    emu.registers.program_counter++;
    return result;

}

void set_status_flag(uint8_t flag, bool condition){
    if(condition){
        emu.registers.flags |= flag;
    } else {
        emu.registers.flags &= ~flag;
    }
    #if DEBUG
    debug_tracker.reg_write_bitflag |= REG_STATUS; 
    #endif 
}