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

    uint8_t stack_pointer;
    uint16_t program_counter;
};

struct emulator { 
    char* filepath;

    struct registers registers;

    uint8_t* RAM;
    uint8_t* ROM;
    uint8_t* header;

    uint8_t cur_instruction;
    int total_CPU_cycles;
}; 

void emu_enable_logger(bool is_enabled);

void emu_init();
void emu_reset();
void emu_run();
uint8_t emu_read(uint16_t address);
void emu_write(uint16_t address, uint8_t value);
void push_stack(uint8_t value);
uint8_t pull_stack();
void set_status_flag(uint8_t flag, bool condition);
uint8_t read_increment_PC();
uint8_t read_PC();
void increment_PC();

void ASL_op(uint8_t input, uint16_t address);
void LSR_op(uint8_t input, uint16_t address);
void ROL_op(uint8_t input, uint16_t address);
void ROR_op(uint8_t input, uint16_t address);
void ADC_op(uint8_t input);
void SBC_op(uint8_t input);
void INC_op(uint8_t input, uint16_t address);
void DEC_op(uint8_t input, uint16_t address);
void CMP_op(uint8_t input, uint8_t reg_value);
void BIT_op(uint8_t input);
uint16_t get_absolute_addr();

extern struct emulator emu;


#endif