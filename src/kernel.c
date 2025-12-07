#include "kernel.h"  
#include <stddef.h>  
#include <stdint.h>
#include "idt/idt.h"
#include "io/io.h"
#include "memory/memory.h"
#include "gdt/gdt.h"
#include "config.h"
#include "memory/heap/kheap.h"
#include "memory/paging/paging.h"
#include "disk/disk.h"
#include "disk/streamer.h"
#include "task/tss.h"
#include "string/string.h"
#include "fs/pparser.h"
#include "fs/file.h"
#include "emulator/emulator.h"

struct tss tss; 
struct gdt gdt_real[PEACHOS_TOTAL_GDT_SEGMENTS]; 
struct gdt_structured gdt_structured[PEACHOS_TOTAL_GDT_SEGMENTS] = { 
  {.base = 0x00, .limit = 0x00, .type = 0x00}, // NULL Segment 
  {.base = 0x00, .limit = 0xffffffff, .type = 0x9a}, // Kernel code segment 
  {.base = 0x00, .limit = 0xffffffff, .type = 0x92},// Kernel data segment 
  {.base = 0x00, .limit = 0xffffffff, .type = 0xf8},// User code segment
  {.base = 0x00, .limit = 0xffffffff, .type = 0xf2},// User data segment 
  {.base = (uint32_t)&tss, .limit=sizeof(tss), .type = 0xE9}// TSS Segment
};

uint16_t* video_mem = 0;  
uint16_t terminal_row = 0;  
uint16_t terminal_col = 0;  

uint16_t terminal_make_char(char c, char colour)  {  
    return (colour << 8) | c;  
}  

void terminal_putchar(int x, int y, char c, char colour)  {  
    video_mem[(y * VGA_WIDTH) + x] = terminal_make_char(c, colour);  
}  

void terminal_writechar(char c, char colour) {  
    if (c == '\n')  {
        terminal_row += 1;  
        terminal_col = 0;  
        return;  
    }  
    terminal_putchar(terminal_col, terminal_row, c, colour);  
    terminal_col += 1;
    if (terminal_col >= VGA_WIDTH)  {  
        terminal_col = 0;  terminal_row += 1;  
    }  
}  

void terminal_initialize()  {  
    video_mem = (uint16_t*)(0xB8000);  
    terminal_row = 0;  
    terminal_col = 0;  
    for (int y = 0; y < VGA_HEIGHT; y++)  {  
        for (int x = 0; x < VGA_WIDTH; x++)  {  
            terminal_putchar(x, y, ' ', 0);  
        }  
    }  
}  

void print(const char* str)  {  
    size_t len = strlen(str);  
    for (int i = 0; i < len; i++)  {  
        terminal_writechar(str[i], 15);  
    }  
}

void print_spaces(int num_spaces){
    for(int i = 0; i <  num_spaces; i++){
        print(" ");
    }
}

void print_hexdump(const uint8_t* data, int size)  {  
    for (int i = 0; i < size+1; i++) {
        uint8_t byte = data[i];
        terminal_writechar(gethexchar(byte >> 4), 15); 
        terminal_writechar(gethexchar(byte & 0xF), 15);  
    }  
}  

void emu_print_hexdump(uint16_t emu_address, int size)  {  
    for (int i = 0; i < size+1; i++) {
        uint8_t byte = emu_read(emu_address+i);
        terminal_writechar(gethexchar(byte >> 4), 15); 
        terminal_writechar(gethexchar(byte & 0xF), 15);
        terminal_writechar(' ', 0);    
    }  
}  

void print_hex8(uint8_t value)  {  
    terminal_writechar(gethexchar(value >> 4), 15); 
    terminal_writechar(gethexchar(value & 0xF), 15);  
}  

void print_hex16(uint16_t value)  {  
    print_hex8((value >> 8) & 0xFF); 
    print_hex8(value & 0xFF);  
}  

void print_decimal(uint8_t value) {
    if (value == 0) {
        terminal_writechar('0', 15);
        return;
    }

    char digits[3];
    int i = 0;

    while (value > 0) {
        digits[i++] = (value % 10) + '0';
        value /= 10;
    }

    for (int j = i-1; j >= 0; j--) {
        terminal_writechar(digits[j], 15);
    }
}

void panic(const char*msg){
    print(msg);
    while(1) {}
}

static struct paging_4gb_chunk* kernel_chunk = 0;

void kernel_main()  {  
    terminal_initialize();  

    // GDT CODE
    memset(gdt_real, 0x00, sizeof(gdt_real)); 
    gdt_structured_to_gdt(gdt_real, gdt_structured, PEACHOS_TOTAL_GDT_SEGMENTS); 
    gdt_load(gdt_real, sizeof(gdt_real)); 

    kheap_init();
    
    fs_init();
    
    idt_init();

    // Setup the TSS 
    memset(&tss, 0x00, sizeof(tss)); 
    tss.esp0 = 0x600000; 
    tss.ss0 = KERNEL_DATA_SELECTOR; 
    
     // Setup paging
    kernel_chunk = paging_new_4gb(PAGING_IS_WRITEABLE | PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL);
    
    // Switch to kernel paging chunk
    paging_switch(paging_4gb_chunk_get_directory(kernel_chunk));

    char* ptr = kzalloc(4096); 
    paging_set(paging_4gb_chunk_get_directory(kernel_chunk), (void*)0x1000, (uint32_t)ptr | PAGING_ACCESS_FROM_ALL | PAGING_IS_PRESENT | PAGING_IS_WRITEABLE);

    // Enable paging
    enable_paging();

    disk_search_and_init();

    enable_interrupts();

    emu_enable_logger(true);
    emu_init();

    while(1) {} 

}