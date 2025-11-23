#ifndef EMULATOR_DEBUG_H
#define EMULATOR_DEBUG_H
#include <stdbool.h>
#include "kernel.h"
#include "opcode_table.h"
#include "emulator.h"

#define DEBUG false

struct mem_reg_access{
    uint8_t reg_access_bitflag;
    uint8_t reg_write_bitflag;
    bool reads_mem;
    bool writes_mem;

};

extern struct mem_reg_access mem_reg_access;

#if DEBUG

static inline void set_A_register(uint8_t value) { 
    mem_reg_access.reg_write_bitflag |= REG_A;
    emu.registers.A = value; 
}
static inline uint8_t reg_A(void) { 
    mem_reg_access.reg_read_bitflag |= REG_A; 
    return emu.registers.A; 
}
static inline void set_X_register(uint8_t value) { 
    mem_reg_access.reg_write_bitflag |= REG_X;
    emu.registers.X = value; 
}
static inline uint8_t reg_X(void) {
    mem_reg_access.reg_read_bitflag |= REG_X;  
    return emu.registers.X; 
}
static inline void set_Y_register(uint8_t value) { 
    mem_reg_access.reg_write_bitflag |= REG_Y;
    emu.registers.Y = value; 
}
static inline uint8_t reg_Y(void) {
    mem_reg_access.reg_read_bitflag |= REG_Y;
    return emu.registers.Y; 
}

#else
static inline void set_A_register(uint8_t value) { emu.registers.A = value; }
static inline uint8_t reg_A(void) { return emu.registers.A; }

static inline void set_X_register(uint8_t value) { emu.registers.X = value; }
static inline uint8_t reg_X(void) { return emu.registers.X; }

static inline void set_Y_register(uint8_t value) { emu.registers.Y = value; }
static inline uint8_t reg_Y(void) { return emu.registers.Y; }
#endif

static inline void set_PC(uint16_t value) { emu.registers.program_counter = value; }
static inline uint16_t program_counter(void) { return emu.registers.program_counter; }

uint8_t read_increment_PC(){
    return emu_read(emu.registers.program_counter++);
}

#endif