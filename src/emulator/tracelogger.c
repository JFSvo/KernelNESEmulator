#include <stddef.h>  
#include <stdint.h>
#include <stdbool.h>
#include "status.h" 
#include "memory/heap/kheap.h" 
#include "kernel.h" 
#include "tracelogger.h"
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