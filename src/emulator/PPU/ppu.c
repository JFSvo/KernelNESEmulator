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
    ppu.RAM = kzalloc(0x800);
    ppu.ROM = kzalloc(0x8000);
    ppu.CHROM = kzalloc(0x2000);
    ppu.filepath = "0:/test7.nes";
    ppu.registers.write_latch = false;
    ppu.registers.transfer_address = 0;
    ppu.registers.VRAM_address = 0;
    
    int fd = fopen(ppu.filepath, "r");
    if(fd){
        struct file_stat stat;
        fstat(fd, &stat);
        fread(ppu.ROM, stat.filesize, 1, fd);
        fseek(fd, 0x10, SEEK_SET);
        fread(ppu.CHROM, 0x2000, 1, fd);
        print("Opened ");
        print(ppu.filepath);
        print("\n");
    }
}