#ifndef PPU_H
#define PPU_H

struct PPU { 
    char* filepath;

    uint8_t* ROM;
    uint8_t* CHROM;
}; 

void ppu_init();

#endif