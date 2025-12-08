#include "drivers/timer/pit.h"
#include "io/io.h"

static uint64_t system_ticks = 0;

void pit_init(uint32_t frequency) {
    uint32_t divisor = 1193180 / frequency;

    // Command: channel 0, lo/hi byte, mode 3, binary
    outb(0x43, 0x36);

    // Divisor low byte, then high byte
    outb(0x40, divisor & 0xFF);
    outb(0x40, (divisor >> 8) & 0xFF);
}

void pit_handler() {
    system_ticks++;
    // Send EOI to PIC
    outb(0x20, 0x20);
}

void pit_sleep(uint32_t milliseconds) {
    uint64_t target_ticks = system_ticks + milliseconds;
    while (system_ticks < target_ticks) {
        asm volatile("hlt");
    }
}

uint64_t pit_get_ticks() {
    return system_ticks;
}