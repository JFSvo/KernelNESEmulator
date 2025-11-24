#ifndef VGA_H
#define VGA_H

#include <stdint.h>

void vga_init();
void vga_put_pixel(uint32_t x, uint32_t y, uint8_t color);
void vga_clear_screen(uint8_t color);
void vga_swap_buffers();

uint32_t vga_get_width();
uint32_t vga_get_height();

#endif
