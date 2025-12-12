#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include "emulator/PPU/ppu.h"
#include "memory/heap/kheap.h"
#include "memory/memory.h"
#include "kernel.h"
#include "fs/file.h"
#include "drivers/vga/vga.h"

struct PPU ppu;

void ppu_init(void)
{
    // Zero out the PPU structure
    memset(&ppu, 0, sizeof(struct PPU));
    
    // ===== Allocate PPU Memory =====
    
    // PPU work RAM (not critical yet, but needed)
    ppu.RAM = kzalloc(0x800);
    if (!ppu.RAM) {
        print("PPU: RAM alloc failed\n");
        return;
    }
    
    // VRAM for nametables (2KB = two 1KB nametables)
    ppu.VRAM = kzalloc(0x800);
    if (!ppu.VRAM) {
        print("PPU: VRAM alloc failed\n");
        return;
    }
    
    // OAM for sprites (256 bytes)
    ppu.OAM = kzalloc(256);
    if (!ppu.OAM) {
        print("PPU: OAM alloc failed\n");
        return;
    }
    
    // Palette RAM (32 bytes: 16 for BG, 16 for sprites)
    ppu.palette_RAM = kzalloc(32);
    if (!ppu.palette_RAM) {
        print("PPU: Palette RAM alloc failed\n");
        return;
    }
    
    // ===== Initialize Registers =====
    ppu.registers.write_latch = false;
    ppu.registers.transfer_address = 0;
    ppu.registers.VRAM_address = 0;
    ppu.registers.PPUCTRL = 0;
    ppu.registers.PPUMASK = 0;
    ppu.registers.PPUSTATUS = 0xA0;  // VBlank flag set initially
    ppu.registers.OAMADDR = 0;
    ppu.registers.fine_x_scroll = 0;
    ppu.read_buffer = 0;
    
    // ===== Load ROM File =====
    ppu.filepath = "0:/test7.nes";
    
    int fd = fopen(ppu.filepath, "r");
    if (fd < 0) {
        print("PPU: could not open ");
        print(ppu.filepath);
        print("\n");
        return;
    }
    
    struct file_stat st;
    fstat(fd, &st);

    // ADD THIS DEBUG OUTPUT:
    print("PPU: File size = ");
    print_decimal(st.filesize);
    print(" bytes\n");
    
    // Validate minimum file size
    if (st.filesize < 16) {
        print("PPU: file too small (< 16 bytes)\n");
        fclose(fd);
        return;
    }
    
    // Allocate ROM buffer for entire .nes file
    ppu.ROM = kzalloc(st.filesize);
    if (!ppu.ROM) {
        print("PPU: ROM alloc failed\n");
        fclose(fd);
        return;
    }
    
    // Read entire file
    fread(ppu.ROM, st.filesize, 1, fd);
    fclose(fd);  // *** IMPORTANT: Close the file ***
    
    // ===== Parse iNES Header =====
    uint8_t* header = ppu.ROM;
    
    // Save header pointer
    ppu.header = header;
    
    // Verify magic number "NES\x1A"
    if (header[0] != 'N' || header[1] != 'E' || header[2] != 'S' || header[3] != 0x1A) {
        print("PPU: bad iNES magic (expected NES<1A>)\n");
        return;
    }
    
    // Parse header fields
    ppu.prg_banks = header[4];  // 16KB PRG-ROM banks
    ppu.chr_banks = header[5];  // 8KB CHR-ROM banks
    
    uint8_t flags6 = header[6];
    uint8_t flags7 = header[7];
    
    // Mirroring (bit 0 of flags6)
    ppu.mirroring = (flags6 & 0x01) ? MIRRORING_VERTICAL : MIRRORING_HORIZONTAL;
    
    // Mapper number (upper 4 bits of flags6 and flags7)
    ppu.mapper = ((flags7 & 0xF0) | (flags6 >> 4));
    
    print("PPU: PRG banks=");
    print_decimal(ppu.prg_banks);
    print(" CHR banks=");
    print_decimal(ppu.chr_banks);
    print(" Mapper=");
    print_decimal(ppu.mapper);
    print(" Mirror=");
    print(ppu.mirroring == MIRRORING_VERTICAL ? "V" : "H");
    print("\n");
    
    // ===== Handle CHR-ROM or CHR-RAM =====
    if (ppu.chr_banks == 0) {
        // CHR-RAM: Allocate writable 8KB
        print("PPU: Using CHR-RAM (8KB)\n");
        ppu.CHROM = kzalloc(0x2000);
        if (!ppu.CHROM) {
            print("PPU: CHR-RAM alloc failed\n");
            return;
        }
        ppu.chr_ram = true;
    } else {
        // CHR-ROM: Copy from file
        uint32_t chr_offset = 16 + ppu.prg_banks * 0x4000;
        uint32_t chr_size = ppu.chr_banks * 0x2000;
        
        // Validate CHR data fits in file
        if (chr_offset + chr_size > st.filesize) {
            print("PPU: CHR past end of file\n");
            return;
        }
        
        ppu.CHROM = kzalloc(chr_size);
        if (!ppu.CHROM) {
            print("PPU: CHR-ROM alloc failed\n");
            return;
        }
        
        memcpy(ppu.CHROM, ppu.ROM + chr_offset, chr_size);
        ppu.chr_ram = false;
        
        print("PPU: CHR-ROM loaded (");
        print_decimal(chr_size);
        print(" bytes)\n");
    }
    
    print("PPU: Initialized from ");
    print(ppu.filepath);
    print("\n");
}

void ppu_cleanup(void)
{
    if (ppu.RAM) kfree(ppu.RAM);
    if (ppu.VRAM) kfree(ppu.VRAM);
    if (ppu.OAM) kfree(ppu.OAM);
    if (ppu.palette_RAM) kfree(ppu.palette_RAM);
    if (ppu.ROM) kfree(ppu.ROM);
    if (ppu.CHROM) kfree(ppu.CHROM);
    
    memset(&ppu, 0, sizeof(struct PPU));
}

/*
 * Draw both pattern tables (background + sprite) to the top of the VGA screen.
 * This mirrors the blogger's C# pattern-table viewer.
 */
void ppu_draw_pattern_tables(void)
{
    if (!ppu.CHROM) {
        print("PPU: CHROM is NULL, nothing to draw\n");
        return;
    }

    // Each pattern table is 0x1000 bytes (4096) = 256 tiles.
    // Each tile is 16 bytes (8 low-plane + 8 high-plane).
    // We're going to draw both tables side-by-side:
    // left  = table 0 (x 0..127)
    // right = table 1 (x 128..255)

    for (int table = 0; table < 2; table++) {          // 0 = left, 1 = right
        for (int row = 0; row < 16; row++) {          // tile row (0..15)
            for (int column = 0; column < 16; column++) { // tile column (0..15)

                // For each pixel row inside the tile
                for (int y = 0; y < 8; y++) {

                    // This matches the blogger's formula:
                    // base = y + column*16 + row*256 + table*4096;
                    uint32_t base =
                        y +
                        column * 16 +
                        row    * 256 +
                        table  * 4096;

                    // Make sure we don't read past CHR-ROM
                    uint32_t chr_size = ppu.chr_banks > 0 ? 
                                       ppu.chr_banks * 0x2000 : 0x2000;
                    if (base + 8 >= chr_size) {
                        continue;  // Skip if out of bounds
                    }

                    uint8_t lowByte  = ppu.CHROM[base];
                    uint8_t highByte = ppu.CHROM[base + 8];

                    // For each pixel column inside the tile
                    for (int x = 0; x < 8; x++) {
                        int bit = 7 - x;

                        int twoBit  = (lowByte  >> bit) & 1;
                        twoBit     |= ((highByte >> bit) & 1) << 1;   // 0..3

                        // Map 2-bit NES pixel to a VGA palette index.
                        uint8_t colorIndex;
                        switch (twoBit) {
                            case 0: colorIndex = 0x00; break; // black
                            case 1: colorIndex = 0x08; break; // dark grey
                            case 2: colorIndex = 0x07; break; // light grey  
                            case 3: colorIndex = 0x0F; break; // white
                        }

                        // Final pixel coordinates in our 256x128 viewer
                        uint32_t px = x + column * 8 + table * 128;  // 0..255
                        uint32_t py = y + row    * 8;                 // 0..127

                        vga_put_pixel(px, py, colorIndex);
                    }
                }
            }
        }
    }

    vga_swap_buffers();
}