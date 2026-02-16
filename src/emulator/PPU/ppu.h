#ifndef PPU_H
#define PPU_H

struct PPU_registers {
    bool write_latch;
    uint16_t transfer_address;
    uint16_t VRAM_address;
};

struct PPU { 
    char* filepath;

    struct PPU_registers registers;

    uint8_t* RAM;
    uint8_t* ROM;
    uint8_t* CHROM;
}; 

void ppu_init();

extern struct PPU ppu;

#endif