#ifndef TRACELOGGER_H
#define TRACELOGGER_H

#include "emulator/emulator.h"
#include "emulator/opcode_table.h"

#define TRACELOG_MAX_LENGTH 256

struct tracelog_entry {
    // The next entry in the tracelog 
    struct tracelog_entry* next;
    // Previous entry in the tracelog 
    struct tracelog_entry* prev;

    uint8_t opcode;
    uint8_t operands[2];
    uint16_t address;
    uint16_t indexed_address;
    uint8_t written_value;
    uint8_t read_value;

    int total_CPU_cycles;

    struct registers registers;
};

struct tracelog_entry* add_tracelog_entry(struct emulator* emu);
void remove_first_entry();
void init_log_entry(struct tracelog_entry* entry, struct emulator* emu);

void tail_log_set_operand(uint8_t byte);
void tail_log_set_address(uint16_t address);
void tail_log_set_indexed_address(uint16_t indexed_address);
void tail_log_set_written_value(uint8_t byte);
void tail_log_set_read_value(uint8_t byte);

void print_tracelog();
void print_latest_tracelog_entry();
void print_tracelog_entry(struct tracelog_entry* entry);
void print_tracelog_operands(struct tracelog_entry* cur_log_entry, const struct opcode_entry* opcode_entry);
void print_tracelog_address(struct tracelog_entry* cur_log_entry, uint16_t address);
void print_tracelog_ZP_indexed(struct tracelog_entry* cur_log_entry, uint8_t address, char* reg_name);

void tracelog_print_entries(int num_entries);

#endif