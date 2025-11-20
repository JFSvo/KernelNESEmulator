#ifndef EMULATOR_H
#define EMULATOR_H

#define RAM_SIZE 800 
#define ROM_SIZE 8000 

struct registers {
    uint8_t A; 
    uint8_t X;
    uint8_t Y;

    uint16_t programCounter;
};

struct emulator { 
    char* filepath;

    struct registers* registers;

    uint8_t* RAM;
    uint8_t* ROM;
}; 

void NES_run();
void reset();

#endif