#ifndef EMULATOR_H
#define EMULATOR_H

#define RAM_SIZE 800 
#define ROM_SIZE 8000 

struct registers {
    uint8_t A; 
    uint8_t X;
    uint8_t Y;

    uint16_t program_counter;
};

struct emulator { 
    char* filepath;

    struct registers registers;

    uint8_t* RAM;
    uint8_t* ROM;
    uint8_t* header;
}; 

void emu_init();
void emu_reset();
void emu_run();
uint8_t emu_read(uint16_t address);

#endif