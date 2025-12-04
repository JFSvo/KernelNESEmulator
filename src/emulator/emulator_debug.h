#ifndef EMULATOR_DEBUG_H
#define EMULATOR_DEBUG_H
#include <stdbool.h>
#include "kernel.h"
#include "opcode_table.h"
#include "emulator.h"

struct mem_reg_access{
    uint8_t reg_read_bitflag;
    uint8_t reg_write_bitflag;
    bool reads_mem;
    bool writes_mem;
};

extern struct mem_reg_access debug_tracker;

typedef unsigned int ACCESS_TYPE; 
enum { 
    READ, 
    WRITE
}; 

void compare_with_table(uint8_t opcode, uint16_t program_counter);
void reset_tracker();
#if DEBUG

static inline uint8_t set_A_register(uint8_t value) { 
    debug_tracker.reg_write_bitflag |= REG_A;
    emu.registers.A = value;
    return value; 
}
static inline uint8_t reg_A(void) { 
    debug_tracker.reg_read_bitflag |= REG_A; 
    return emu.registers.A; 
}
static inline uint8_t set_X_register(uint8_t value) { 
    debug_tracker.reg_write_bitflag |= REG_X;
    emu.registers.X = value;
    return value;
}
static inline uint8_t reg_X(void) {
    debug_tracker.reg_read_bitflag |= REG_X;  
    return emu.registers.X; 
}
static inline uint8_t set_Y_register(uint8_t value) { 
    debug_tracker.reg_write_bitflag |= REG_Y;
    emu.registers.Y = value;
    return value;
}
static inline uint8_t reg_Y(void) {
    debug_tracker.reg_read_bitflag |= REG_Y;
    return emu.registers.Y; 
}
static inline uint8_t reg_status(void) {
    debug_tracker.reg_read_bitflag |= REG_STATUS;
    return emu.registers.flags; 
}

#else
static inline uint8_t set_A_register(uint8_t value) { emu.registers.A = value; return value;}
static inline uint8_t reg_A(void) { return emu.registers.A; }

static inline uint8_t set_X_register(uint8_t value) { emu.registers.X = value; return value;}
static inline uint8_t reg_X(void) { return emu.registers.X; }

static inline uint8_t set_Y_register(uint8_t value) { emu.registers.Y = value; return value;}
static inline uint8_t reg_Y(void) { return emu.registers.Y; }

static inline uint8_t reg_status(void) { return emu.registers.flags;}
#endif

static inline uint8_t set_PC(uint16_t value) { emu.registers.program_counter = value; return value;}
static inline uint16_t program_counter(void) { return emu.registers.program_counter; }

uint8_t read_increment_PC();
uint8_t read_PC();

#endif