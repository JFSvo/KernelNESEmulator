#include "drivers/vga/vga.h"
#include "io/io.h"
#include "memory/heap/kheap.h"

#define VGA_WIDTH  320
#define VGA_HEIGHT 200
#define VGA_BUFFER 0xA0000

static uint8_t* vga_buffer = 0;
static uint8_t* back_buffer = 0;

void vga_init() {
    // Bootloader has already set mode 0x13.
    vga_buffer = (uint8_t*)VGA_BUFFER;
    back_buffer = kzalloc(VGA_WIDTH * VGA_HEIGHT);

    // Clear both the back buffer and the actual screen
    vga_clear_screen(0x00);
    vga_swap_buffers();
}

void vga_put_pixel(uint32_t x, uint32_t y, uint8_t color) {
    if (x >= VGA_WIDTH || y >= VGA_HEIGHT) return;
    back_buffer[y * VGA_WIDTH + x] = color;
}

void vga_clear_screen(uint8_t color) {
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        back_buffer[i] = color;
    }
}

void vga_swap_buffers() {
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        vga_buffer[i] = back_buffer[i];
    }
}

uint32_t vga_get_width()  { return VGA_WIDTH; }
uint32_t vga_get_height() { return VGA_HEIGHT; }
