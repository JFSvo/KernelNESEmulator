#ifndef TRACELOGGER_H
#define TRACELOGGER_H

#include "emulator/emulator.h"

#define TRACELOG_MAX_LENGTH 256

struct tracelog_entry {
    // The next entry in the tracelog 
    struct tracelog_entry* next;
    // Previous entry in the tracelog 
    struct tracelog_entry* prev;

    uint8_t opcode;
    int total_CPU_cycles;

    struct registers registers;
};

struct tracelog_entry* add_tracelog_entry(struct emulator* emu);
void remove_first_entry();
void init_log_entry(struct tracelog_entry* entry, struct emulator* emu);
void print_tracelog();

#endif