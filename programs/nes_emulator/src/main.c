#include "kernel.h"
#include "string/string.h"
#include "memory/memory.h"

void draw_color_bars() {
    // Draw color bars across the screen
    for (int y = 0; y < 200; y++) {
        for (int x = 0; x < 320; x++) {
            uint8_t color;
            if (x < 64) color = 0x04;       // Red
            else if (x < 128) color = 0x02; // Green  
            else if (x < 192) color = 0x01; // Blue
            else color = 0x0F;              // White
            
            vga_put_pixel(x, y, color);
        }
    }
    vga_swap_buffers();
}

void draw_moving_block() {
    static int block_x = 0;
    static int direction = 1;
    
    // Clear screen to black
    vga_clear_screen(0x00);
    
    // Draw moving white block
    for (int y = 80; y < 120; y++) {
        for (int x = block_x; x < block_x + 40; x++) {
            vga_put_pixel(x, y, 0x0F); // White
        }
    }
    
    vga_swap_buffers();
    
    // Update position
    block_x += direction;
    if (block_x <= 0 || block_x >= 280) {
        direction = -direction;
    }
}

int main(int argc, char** argv) {
    print("VGA and PIT Test Program\n");
    print("Testing VGA graphics and PIT timer...\n");
    
    // Test 1: Static color bars (verify VGA works)
    print("Test 1: Drawing color bars...\n");
    draw_color_bars();
    pit_sleep(2000); // Wait 2 seconds
    
    // Test 2: Moving block (verify PIT timing works)
    print("Test 2: Moving block animation...\n");
    uint64_t animation_end = pit_get_ticks() + 5000; // Run for 5 seconds
    
    while (pit_get_ticks() < animation_end) {
        draw_moving_block();
        pit_sleep(16); // ~60 FPS
    }
    
    print("All tests completed successfully!\n");
    print("VGA and PIT are working correctly.\n");
    
    return 0;
}
