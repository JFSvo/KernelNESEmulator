#ifndef EMULATOR_H
#define EMULATOR_H

#define DEBUG true
#define TRACELOGGER true

#define RAM_SIZE 800 
#define ROM_SIZE 8000 

// Status flags
#define FLAG_CARRY 0x01
#define FLAG_ZERO 0x02
#define FLAG_INTERRUPT_DISABLE 0x04
#define FLAG_DECIMAL 0x08
// 0x10 and 0x20 bits are unused by CPU
#define FLAG_OVERFLOW 0x40
#define FLAG_NEGATIVE 0x80

struct registers {
    uint8_t A; 
    uint8_t X;
    uint8_t Y;
    uint8_t flags;

    uint16_t program_counter;
};

struct emulator { 
    char* filepath;

    struct registers registers;

    uint8_t* RAM;
    uint8_t* ROM;
    uint8_t* header;

    uint8_t current_opcode;
    int total_CPU_cycles;
}; 

void emu_enable_logger(bool is_enabled);

void emu_init();
void emu_reset();
void emu_run();
uint8_t emu_read(uint16_t address);
void emu_write(uint16_t address, uint8_t value);
void set_status_flag(uint8_t flag, bool condition);

extern struct emulator emu;


#endif