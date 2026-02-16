#ifndef OPCODE_TABLE_H
#define OPCODE_TABLE_H

#include <stdbool.h>
#include <stdint.h>

#define NUM_OPCODES 256

typedef unsigned int ADDRESSING_MODE; 
enum { 
    ADDR_IMPLIED,      // operation itself implies the byte to operate on
    ADDR_ACCUMULATOR,  // operation modifies accumulator, no operand
    ADDR_IMMEDIATE,    // takes a one byte immediate value as operand
    ADDR_ABSOLUTE,     // 2 byte address specified to operate on
    ADDR_ZERO_PAGE,    // 1 byte address to modify zero page (first 256 bytes)
    ADDR_ABSOLUTE_X,   // add X as offset to 2 byte address
    ADDR_ABSOLUTE_Y,   // add Y as offset to 2 byte address
    ADDR_ZERO_PAGE_X,  // add X as offset to 1 byte address for zero page
    ADDR_ZERO_PAGE_Y,  // add Y as offset to 1 byte address for zero page
    ADDR_INDIRECT_X,   // get value in ZP pointed to by (given 1 byte addr + X offset)
    ADDR_INDIRECT_Y,   // get value at (2 byte ptr starting at given 1 byte addr in ZP + Y offset)
    ADDR_ABS_INDIRECT, // get byte at given 2 byte ptr, and the byte after. jump to combined address.
    ADDR_RELATIVE,      // adjust program counter by signed byte value (-128 to 127)
    MODE_UNKNOWN
}; 

// Used for comparing register access/writes with opcode table bitflags 
#define REG_STATUS  0x01
#define REG_STACK   0x02
#define REG_Y       0x04
#define REG_X       0x08
#define REG_A       0x10

struct opcode_entry {
    uint8_t opcode;
    char* mnemonic;
    ADDRESSING_MODE addr_mode;
    uint8_t num_bytes;
    uint8_t cycles;
    uint8_t reg_read_bitflag;
    uint8_t reg_write_bitflag;
    bool reads_mem;
    bool writes_mem;
};

extern const struct opcode_entry opcode_table[];
#endif