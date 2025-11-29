#include <stddef.h>  
#include <stdint.h>
#include <stdbool.h>
#include "status.h" 
#include "memory/heap/kheap.h" 
#include "kernel.h" 
#include "tracelogger.h"
#include "opcode_table.h"
#include "emulator.h"

// Tracelog linked list 
struct tracelog_entry* log_head = NULL;
struct tracelog_entry* log_tail = NULL;

int current_length = 0;

struct tracelog_entry* add_tracelog_entry(struct emulator* emu){
    int res = 0;

    if(current_length + 1 > TRACELOG_MAX_LENGTH){
        remove_first_entry();
    }

    struct tracelog_entry* entry = kzalloc(sizeof(struct tracelog_entry));

    if (!entry) {
        res = -ENOMEM;
        goto out;
    }

    if (log_head == 0) {
        log_head = entry;
        log_tail = entry;
        init_log_entry(entry, emu);
        goto out;
    }

    log_tail->next = entry;
    entry->prev = log_tail;
    log_tail = entry;

    init_log_entry(entry, emu);

    current_length++;

out: 
    if (ISERR(res)) {
        kfree(entry);
        return ERROR(res);
    }
    
    return entry;
}

void init_log_entry(struct tracelog_entry* entry, struct emulator* emu) {
    entry->registers = emu->registers;
    entry->opcode = emu->current_opcode;
    entry->total_CPU_cycles = emu->total_CPU_cycles;
}

void remove_first_entry() {
    if(!log_head) {
        return;
    }

    struct tracelog_entry* old_head = log_head;
    if(log_head == log_tail) {
        log_head = NULL;
        log_tail = NULL;
    } else {
        log_head = old_head->next;
        log_head->prev = NULL;
    }

    kfree(old_head);
    
    current_length--;
}

void print_tracelog() {
    struct tracelog_entry* cur_log_entry = log_head;
    while(cur_log_entry != NULL) {
        print_hex16(cur_log_entry->registers.program_counter);
        const struct opcode_entry* opcode_entry = &opcode_table[cur_log_entry->opcode];
        print(" ");
        print(opcode_entry->mnemonic);
        print("  ");
        print_hex8(cur_log_entry->opcode);
        print("  ");
        print("A:");
        print_hex8(cur_log_entry->registers.A);
        print(" X:");
        print_hex8(cur_log_entry->registers.X);
        print(" Y:");
        print_hex8(cur_log_entry->registers.Y);
        print(" P:");
        uint8_t flags = cur_log_entry->registers.flags;
        print(flags & FLAG_NEGATIVE ? "N" : "n");
        print(flags & FLAG_OVERFLOW ? "V" : "v");
        print("--");
        print(flags & FLAG_DECIMAL ? "D" : "d");
        print(flags & FLAG_INTERRUPT_DISABLE ? "I" : "i");
        print(flags & FLAG_ZERO ? "Z" : "z");
        print(flags & FLAG_CARRY ? "C" : "c");
        print(" Cycle:");
        print_decimal(cur_log_entry->total_CPU_cycles);
        print("\n");

        cur_log_entry = cur_log_entry->next;
    }
}