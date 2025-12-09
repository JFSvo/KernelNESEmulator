#ifndef KERNEL_H
#define KERNEL_H

#include <stdint.h>

#define VGA_WIDTH 80
#define VGA_HEIGHT 20

#define PEACHOS_MAX_PATH 108

#define ERROR(value) (void*)(value) 
#define ERROR_I(value) (int)(value) 
#define ISERR(value) ((int)value < 0)


void kernel_main();
void reset_terminal();

void print(const char* str);
void print_spaces(int num_spaces);
void panic(const char* msg);
void print_hexdump(const uint8_t* data, int size);
void emu_print_hexdump(uint16_t emu_address, int size);
void print_hex8(uint8_t value);
void print_hex16(uint16_t value);
void print_decimal(int value);
void print_binary(uint8_t value);

#endif