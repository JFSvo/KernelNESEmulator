#include <stddef.h>  
#include <stdint.h>
#include "memory/heap/kheap.h"
#include "memory/memory.h"
#include "kernel.h"

uint16_t programCounter;

uint8_t A; 
uint8_t X;
uint8_t Y;
uint8_t* RAM;

void NES_run()  { 
    A = 0;
    X = 0;
    Y = 0;
    programCounter = 0;
    print("hello!");

    RAM = kzalloc(4096);
    RAM[0] = 0x01;
}

uint8_t read(uint16_t address){
    print("Hi");
    return 5;
}
