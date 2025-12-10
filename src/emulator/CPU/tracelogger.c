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
int current_operand = 0;
struct tracelog_entry* cur_selected_entry;
int cur_selected_index = 0;

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
        cur_selected_entry = log_head;
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

void tail_log_set_operand(uint8_t byte) {
    if(log_tail->opcode == 0){
        log_tail->opcode = byte;
    } else {
        log_tail->operands[current_operand] = byte;
        current_operand++;
    }
}

void tail_log_set_address(uint16_t address) {
    log_tail->address = address;
}

void tail_log_set_indexed_address(uint16_t indexed_address) {
    log_tail->indexed_address = indexed_address;
}

void tail_log_set_written_value(uint8_t byte) {
    log_tail->written_value = byte;
    log_tail->write_value_set = true;
}

void tail_log_set_read_value(uint8_t byte) {
    log_tail->read_value = byte;
    log_tail->read_value_set = true;
}

void init_log_entry(struct tracelog_entry* entry, struct emulator* emu) {
    entry->registers = emu->registers;
    entry->total_CPU_cycles = emu->total_CPU_cycles;
    current_operand = 0;
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
        //print_tracelog_entry();

        cur_log_entry = cur_log_entry->next;
    }
}

void print_latest_tracelog_entry() {
    if(current_length >= 00 && current_length < 30){
        //print_tracelog_entry(log_tail);
    }
}

void print_tracelog_entry(struct tracelog_entry* cur_log_entry) {
    print_hex16(cur_log_entry->registers.program_counter);
    const struct opcode_entry* opcode_entry = &opcode_table[cur_log_entry->opcode];
    print(" ");
    print_hex8(cur_log_entry->opcode);
    print("  ");
    print(opcode_entry->mnemonic);
    print(" ");
    print_tracelog_operands(cur_log_entry, opcode_entry);
    print("A:");
    print_hex8(cur_log_entry->registers.A);
    print(" X:");
    print_hex8(cur_log_entry->registers.X);
    print(" Y:");
    print_hex8(cur_log_entry->registers.Y);
    print(" S:");
    print_hex8(cur_log_entry->registers.stack_pointer);
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
}

void print_tracelog_operands(struct tracelog_entry* cur_log_entry, const struct opcode_entry* opcode_entry){
    
    uint16_t address;

    switch(opcode_entry->addr_mode){

        case ADDR_ZERO_PAGE: 
            address = cur_log_entry->operands[0];
            print_tracelog_address(cur_log_entry, address);
            break;

        case ADDR_ABSOLUTE: case ADDR_ABSOLUTE_Y: case ADDR_ABSOLUTE_X: case ADDR_RELATIVE: 
            print_tracelog_address(cur_log_entry, cur_log_entry->address);
            break;
            
        case ADDR_ZERO_PAGE_X:
            print_tracelog_ZP_indexed(cur_log_entry, "X");
            break;

        case ADDR_ZERO_PAGE_Y:
            print_tracelog_ZP_indexed(cur_log_entry, "Y");
            break;

        case ADDR_INDIRECT_Y:
            print_tracelog_indirect_indexed(cur_log_entry, 'Y');
            break;

        case ADDR_INDIRECT_X:
            print_tracelog_indirect_indexed(cur_log_entry, 'X');
            
            break;

        case ADDR_IMMEDIATE: 
            print("#$");
            print_hex8(cur_log_entry->operands[0]);
            print_spaces(19);
            break;

        default:
            print_spaces(23);
            break;
    }
}

void print_tracelog_address(struct tracelog_entry* cur_log_entry, uint16_t address){
    int extra_spaces = 12;
    print("$");
    if(address <= 0xFF){
        print_hex8(address);
        extra_spaces += 2;
    } else {
        print_hex16(address);
    }

    uint8_t print_value = cur_log_entry->write_value_set ? cur_log_entry->written_value : cur_log_entry->read_value;
    if(cur_log_entry->read_value_set || cur_log_entry->write_value_set){
        print(" = $");
        print_hex8(print_value);
    } else {
        extra_spaces += 6;
    }
    print_spaces(extra_spaces);
}

void print_tracelog_ZP_indexed(struct tracelog_entry* cur_log_entry, char* reg_name){
    int extra_spaces = 4;
    print("$");
    print_hex8(cur_log_entry->operands[0]);
    print(",");
    print(reg_name);
    print(" [$");
    print_hex16(cur_log_entry->indexed_address);
    print("]");

    uint8_t print_value = cur_log_entry->write_value_set ? cur_log_entry->written_value : cur_log_entry->read_value;
    if(cur_log_entry->read_value_set || cur_log_entry->write_value_set){
        print(" = $");
        print_hex8(print_value);
    } else {
        extra_spaces += 6;
    }
    print_spaces(extra_spaces);
}

void print_tracelog_indirect_indexed(struct tracelog_entry* cur_log_entry, char reg_name){
    int extra_spaces = 2;
    print("($");
    print_hex8(cur_log_entry->operands[0]);
    const char* index_string = (reg_name == 'Y' ? "),Y [$" : ",X) [$");
    print(index_string);
    print_hex16(cur_log_entry->indexed_address);
    print("]");

    uint8_t print_value = cur_log_entry->write_value_set ? cur_log_entry->written_value : cur_log_entry->read_value;
    if(cur_log_entry->read_value_set || cur_log_entry->write_value_set){
        print(" = $");
        print_hex8(print_value);
    } else {
        extra_spaces += 8;
    }
    print_spaces(extra_spaces);
}

void tracelog_print_entries(){
    reset_terminal();
    struct tracelog_entry* print_entry = cur_selected_entry;
    for(int i = 0; i <= NUM_PRINT_ENTRIES; i++){
        print_tracelog_entry(print_entry);
        if(print_entry != log_tail){
            print_entry = print_entry->next;
        }
    }
}

void scroll_down(){
    if(cur_selected_index + NUM_PRINT_ENTRIES < current_length){
        cur_selected_entry = cur_selected_entry->next;
        cur_selected_index++;
    }
    tracelog_print_entries();
}

void scroll_up(){
    if(cur_selected_entry != log_head){
        cur_selected_entry = cur_selected_entry->prev;
        cur_selected_index--;
    }
    tracelog_print_entries();
}
