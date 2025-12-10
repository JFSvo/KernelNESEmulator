#include <stddef.h>  
#include <stdint.h>
#include <stdbool.h>
#include "ppu.h"
#include "memory/heap/kheap.h"
#include "memory/memory.h"
#include "kernel.h"
#include "fs/file.h"

struct PPU ppu;

void ppu_init() { 
    ppu.ROM = kzalloc(0x8000);
    ppu.CHROM = kzalloc(0x2000);
    ppu.filepath = "0:/test7.nes";
    
    int fd = fopen(ppu.filepath, "r");
    if(fd){
        print("Opened ");
        print(ppu.filepath);
        print("\n");
    }
}