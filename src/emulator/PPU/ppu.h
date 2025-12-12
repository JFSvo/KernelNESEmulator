#ifndef PPU_H
#define PPU_H

#include <stdint.h>
#include <stdbool.h>

// Mirroring modes
typedef enum {
    MIRRORING_HORIZONTAL = 0,
    MIRRORING_VERTICAL = 1
} MirroringMode;

struct PPU_registers {
    // Address-related registers
    bool write_latch;
    uint16_t transfer_address;
    uint16_t VRAM_address;
    
    // PPU control registers
    uint8_t PPUCTRL;
    uint8_t PPUMASK;
    uint8_t PPUSTATUS;
    uint8_t OAMADDR;
    
    // Fine X scroll (3 bits)
    uint8_t fine_x_scroll;
};

struct PPU {
    char* filepath;
    struct PPU_registers registers;
    
    // Memory
    uint8_t* RAM;          // PPU work RAM (0x800 bytes)
    uint8_t* ROM;          // Full .nes file
    uint8_t* CHROM;        // CHR-ROM/RAM (8KB or more)
    uint8_t* VRAM;         // Nametables (0x800 bytes)
    uint8_t* OAM;          // Object Attribute Memory (256 bytes)
    uint8_t* palette_RAM;  // Color palettes (32 bytes)
    
    // Header info
    uint8_t* header;       // iNES header (16 bytes)
    
    // Cartridge info
    bool chr_ram;          // True if CHR-RAM, false if CHR-ROM
    MirroringMode mirroring;
    uint8_t mapper;
    int prg_banks;
    int chr_banks;
    
    // PPU state
    uint8_t read_buffer;   // Internal read buffer
};

void ppu_init(void);
void ppu_draw_pattern_tables(void);
void ppu_cleanup(void);

extern struct PPU ppu;

void ppu_write_data(uint8_t value);
uint8_t ppu_read_data(void);

#endif