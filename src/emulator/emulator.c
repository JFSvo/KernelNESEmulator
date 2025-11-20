#include <stddef.h>  
#include <stdint.h>
#include "memory/heap/kheap.h"
#include "memory/memory.h"
#include "kernel.h"
#include "fs/file.h"

uint16_t programCounter;

uint8_t A; 
uint8_t X;
uint8_t Y;
uint8_t* RAM;
uint8_t* ROM;

void NES_run()  { 
    A = 0;
    X = 0;
    Y = 0;
    programCounter = 0;
    print("hello!");

    RAM = kzalloc(0x800);
    RAM[0] = 0x01;

    int fd = fopen("0:/test1.nes", "r");
    if(fd){
        print("\nOpened Example 1.nes\n");
        uint8_t buf[15];
        // fread(buf, 13, 1, fd);
        fread(buf, 0x16, 0x00, fd);
        buf[13] = 0x00;
        printhex(buf, 13);
    }
}

uint8_t read(uint16_t address){
    print("Hi");
    return 5;
}

void reset(){
    print("Hi");
}
