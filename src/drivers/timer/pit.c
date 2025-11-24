#include "drivers/timer/pit.h"
#include "io/io.h"

static volatile uint64_t system_ticks = 0;

void pit_init(uint32_t frequency) {
    if (!frequency) {
        frequency = 1000; // avoid divide-by-zero, default 1kHz
    }

    uint32_t divisor = 1193180 / frequency;

    // Command: Channel 0, Lo/Hi, Mode 3 (square wave), binary
    outb(0x43, 0x36);

    // Divisor low, then high byte
    outb(0x40, divisor & 0xFF);
    outb(0x40, (divisor >> 8) & 0xFF);
}

void pit_handler(void) {
    system_ticks++;

    // Send EOI to master PIC
    outb(0x20, 0x20);
}

void pit_sleep(uint32_t milliseconds) {
    uint64_t target_ticks = system_ticks + milliseconds;
    while (system_ticks < target_ticks) {
        asm volatile("hlt");
    }
}

uint64_t pit_get_ticks(void) {
    return system_ticks;
}
