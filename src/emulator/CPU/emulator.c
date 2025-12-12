// #include <stddef.h>  
// #include <stdint.h>
// #include <stdbool.h>
// #include "emulator.h"
// #include "emulator_debug.h"
// #include "memory/heap/kheap.h"
// #include "memory/memory.h"
// #include "kernel.h"
// #include "fs/file.h"
// #include "tracelogger.h"
// #include "../PPU/ppu.h"

// struct emulator emu;
// bool CPU_halted;
// bool logger_enabled = false;

// void emu_init()  { 
//     emu.RAM = kzalloc(0x800);
//     emu.ROM = kzalloc(0x8000);
//     emu.header = kzalloc(0x10);
//     emu.total_CPU_cycles = 8;
//     emu.filepath = "0:/test6.nes";
//     CPU_halted = false;
//     emu_reset();
// }

// void emulate_CPU() {
//     reset_tracker();
    
//     #if DEBUG
//     uint16_t initial_PC = emu.registers.program_counter;
//     #endif

//     add_tracelog_entry(&emu);
//     uint8_t opcode = read_increment_PC();
//     emu.cur_opcode = opcode;

//     uint8_t result;
//     uint8_t temp;
//     uint16_t address;
//     uint8_t PC_lowbyte;
//     uint8_t PC_highbyte;
//     int signed_value;

//     switch(opcode){
//         case 0x02: // HLT
//             CPU_halted = true;
//             break;

//         case 0xA0: // LDY Immediate 
//             result = set_Y_register(read_increment_PC());
//             set_status_flag(FLAG_ZERO, result == 0);
//             set_status_flag(FLAG_NEGATIVE, result >= 0x80);
//             break;

//         case 0xA2: // LDX Immediate 
//             result = set_X_register(read_increment_PC());
//             set_status_flag(FLAG_ZERO, result == 0);
//             set_status_flag(FLAG_NEGATIVE, result >= 0x80);
//             break;

//         case 0xA9: // LDA Immediate 
//             result = set_A_register(read_increment_PC());
//             set_status_flag(FLAG_ZERO, result == 0);
//             set_status_flag(FLAG_NEGATIVE, result >= 0x80);
//             break;

//         case 0xA5: ; // LDA Zero Page
//             address = read_increment_PC();
//             result = set_A_register(emu_read(address));
//             set_status_flag(FLAG_ZERO, result == 0);
//             set_status_flag(FLAG_NEGATIVE, result >= 0x80);
//             break;
            
//         case 0xB5: ; // LDA Zero Page, X
//             address = get_ZP_indexed_address(reg_X());
//             result = set_A_register(emu_read(address));
//             set_status_flag(FLAG_ZERO, result == 0);
//             set_status_flag(FLAG_NEGATIVE, result >= 0x80);
//             break;

//         case 0xAD: ; // LDA Absolute
//             address = get_absolute_addr();

//             result = set_A_register(emu_read(address));
//             set_status_flag(FLAG_ZERO, result == 0);
//             set_status_flag(FLAG_NEGATIVE, result >= 0x80);
//             break;

//         case 0xBD: ; // LDA Absolute, X
//             address = get_absolute_addr_X_indexed();

//             result = set_A_register(emu_read(address));
//             set_status_flag(FLAG_ZERO, result == 0);
//             set_status_flag(FLAG_NEGATIVE, result >= 0x80);
//             break;

//         case 0x85: ; // STA Zero Page
//             address = read_increment_PC();
//             emu_write(address, reg_A());
//             break;

//         case 0x95: ; // STA Zero Page, X
//             address = get_ZP_indexed_address(reg_X());       
//             emu_write(address, reg_A());
//             break;

//         case 0x8D: ; // STA Absolute
//             address = get_absolute_addr();
//             emu_write(address, reg_A());
//             break;

//         case 0x9D: ; // STA Absolute, X
//             address = get_absolute_addr_X_indexed();
//             emu_write(address, reg_A());
//             break;

//         case 0x91: ; // STA (Indirect), Y
//             address = get_indirect_address_Y_indexed();
//             emu_write(address, reg_A());
//             break;
        
//         case 0x81: ; // STA (Indirect, X)
//             address = get_indirect_address_X_indexed();
//             emu_write(address, reg_A());
//             break;

//         case 0x86: ; // STX Zero Page
//             address = read_increment_PC();
//             emu_write(address, reg_X());
//             break;

//         case 0x8E: ; // STX Absolute
//             address = get_absolute_addr();
//             emu_write(address, reg_X());
//             break;

//         case 0x84: ; // STY Zero Page
//             address = read_increment_PC();
//             emu_write(address, reg_Y());
//             break;

//         case 0x8C: ; // STY Absolute
//             address = get_absolute_addr();
//             emu_write(address, reg_Y());
//             break;

//         case 0xD0: ; // BNE
//             result = read_increment_PC();
//             if(!(reg_status() & FLAG_ZERO)){ // Branch taken if zero flag cleared
//                 signed_value = result;
//                 if(signed_value >= 0x80){
//                     signed_value -= 256;
//                 }
//                 set_PC((uint16_t)((int)program_counter() + signed_value));
//                 emu.total_CPU_cycles++;
//             }
//             break;

//         case 0xF0: ; // BEQ
//             result = read_increment_PC();
//             if(reg_status() & FLAG_ZERO){ // Branch taken if zero flag set
//                 signed_value = result;
//                 if(signed_value >= 0x80){
//                     signed_value -= 256;
//                 }
//                 set_PC((uint16_t)((int)program_counter() + signed_value));
//                 emu.total_CPU_cycles++;
//             }
//             break;

//         case 0x10: ; // BPL
//             result = read_increment_PC();
//             if(!(reg_status() & FLAG_NEGATIVE)){ // Branch if negative flag cleared
//                 signed_value = result;
//                 if(signed_value >= 0x80){
//                     signed_value -= 256;
//                 }
//                 set_PC((uint16_t)((int)program_counter() + signed_value));
//                 emu.total_CPU_cycles++;
//             }
//             break;

//         case 0x30: ; // BMI
//             result = read_increment_PC();
//             if(reg_status() & FLAG_NEGATIVE){ // Branch if negative flag set
//                 signed_value = result;
//                 if(signed_value >= 0x80){
//                     signed_value -= 256;
//                 }
//                 set_PC((uint16_t)((int)program_counter() + signed_value));
//                 emu.total_CPU_cycles++;
//             }
//             break;

//         case 0x50: ; // BVC
//             result = read_increment_PC();
//             if(!(reg_status() & FLAG_OVERFLOW)){ // Branch if overflow flag cleared
//                 signed_value = result;
//                 if(signed_value >= 0x80){
//                     signed_value -= 256;
//                 }
//                 set_PC((uint16_t)((int)program_counter() + signed_value));
//                 emu.total_CPU_cycles++;
//             }
//             break;

//         case 0x70: ; // BVS
//             result = read_increment_PC();
//             if(reg_status() & FLAG_OVERFLOW){ // Branch if overflow flag set
//                 signed_value = result;
//                 if(signed_value >= 0x80){
//                     signed_value -= 256;
//                 }
//                 set_PC((uint16_t)((int)program_counter() + signed_value));
//                 emu.total_CPU_cycles++;
//             }
//             break;

//         case 0x90: ; // BCC
//             result = read_increment_PC();
//             if(!(reg_status() & FLAG_CARRY)){ // Branch if carry flag cleared
//                 signed_value = result;
//                 if(signed_value >= 0x80){
//                     signed_value -= 256;
//                 }
//                 set_PC((uint16_t)((int)program_counter() + signed_value));
//                 emu.total_CPU_cycles++;
//             }
//             break;

//         case 0xB0: ; // BCS
//             result = read_increment_PC();
//             if(reg_status() & FLAG_CARRY){ // Branch if carry flag set
//                 signed_value = result;
//                 if(signed_value >= 0x80){
//                     signed_value -= 256;
//                 }
//                 set_PC((uint16_t)((int)program_counter() + signed_value));
//                 emu.total_CPU_cycles++;
//             }
//             break;

//         case 0x48: ; // PLA
//             push_stack(reg_A());
//             break;

//         case 0x68: ; // PHA
//             result = set_A_register(pull_stack());
//             set_status_flag(FLAG_ZERO, result == 0);
//             set_status_flag(FLAG_NEGATIVE, result >= 0x80);
//             break;

//         case 0x20: ; // JSR
//             PC_lowbyte = read_increment_PC();
//             PC_highbyte = read_PC();
            
//             push_stack((uint8_t)(program_counter() >> 8));
//             push_stack((uint8_t)program_counter());
//             set_PC((PC_highbyte << 8) | PC_lowbyte);
//             break;
        
//         case 0x60: ; // RTS
//             PC_lowbyte = pull_stack();
//             PC_highbyte = pull_stack();
//             set_PC((PC_highbyte << 8) | PC_lowbyte);
//             read_increment_PC();
//             break;
        
//         case 0x4C: ; // JMP
//             set_PC(get_absolute_addr());
//             break;
                
//         case 0x6C: ; // JMP indirect
//             address = get_absolute_addr();
//             PC_lowbyte = emu_read(address);
//             PC_highbyte = emu_read(address + 1);
//             set_PC((PC_highbyte << 8) | PC_lowbyte);
//             break;

//         case 0xE8: ; // INX
//             result = set_X_register(reg_X() + 1);
//             set_status_flag(FLAG_ZERO, result == 0);
//             set_status_flag(FLAG_NEGATIVE, result >= 0x80);
//             break;

//         case 0xCA: ; // DEX
//             result = set_X_register(reg_X() - 1);
//             set_status_flag(FLAG_ZERO, result == 0);
//             set_status_flag(FLAG_NEGATIVE, result >= 0x80);
//             break;

//         case 0xC8: ; // INY
//             result = set_Y_register(reg_Y() + 1);
//             set_status_flag(FLAG_ZERO, result == 0);
//             set_status_flag(FLAG_NEGATIVE, result >= 0x80);
//             break;

//         case 0x88: ; // DEY
//             result = set_X_register(reg_X() - 1);
//             set_status_flag(FLAG_ZERO, result == 0);
//             set_status_flag(FLAG_NEGATIVE, result >= 0x80);
//             break;
    
//         case 0xAA: ; // TAX
//             result = set_X_register(reg_A());
//             set_status_flag(FLAG_ZERO, result == 0);
//             set_status_flag(FLAG_NEGATIVE, result >= 0x80);
//             break;

//         case 0xA8: ; // TAY
//             result = set_Y_register(reg_A());
//             set_status_flag(FLAG_ZERO, result == 0);
//             set_status_flag(FLAG_NEGATIVE, result >= 0x80);
//             break;

//         case 0x8A: ; // TXA
//             result = set_A_register(reg_X());
//             set_status_flag(FLAG_ZERO, result == 0);
//             set_status_flag(FLAG_NEGATIVE, result >= 0x80);
//             break;

//         case 0x98: ; // TYA
//             result = set_A_register(reg_Y());
//             set_status_flag(FLAG_ZERO, result == 0);
//             set_status_flag(FLAG_NEGATIVE, result >= 0x80);
//             break;

//         case 0x9A: ; // TXS
//             result = set_SP(reg_X());
//             break;

//         case 0xBA: ; // TSX
//             result = set_X_register(stack_pointer());
//             set_status_flag(FLAG_ZERO, result == 0);
//             set_status_flag(FLAG_NEGATIVE, result >= 0x80);
//             break;

//         case 0x38: ; // SEC
//             set_status_flag(FLAG_CARRY, true);
//             break;

//         case 0x18: ; // CEC
//             set_status_flag(FLAG_CARRY, false);
//             break;

//         case 0xB8: ; // CLV 
//             set_status_flag(FLAG_OVERFLOW, false);
//             break;

//         case 0x78: ; // SEI
//             set_status_flag(FLAG_INTERRUPT_DISABLE, true);
//             break;

//         case 0x58: ; // CLI
//             set_status_flag(FLAG_INTERRUPT_DISABLE, false);
//             break;
                
//         case 0xF8: ; // SED
//             set_status_flag(FLAG_DECIMAL, true);
//             break;
        
//         case 0x08: ; // PHP
//             temp = reg_status();
//             temp |= 0x30; // Set bits 4 and 5
//             push_stack(temp);
//             break;

//         case 0x28: ; // PLP
//             result = 0;
//             set_status_register(pull_stack());
//             break;

//         case 0x0A: ; // ASL A
//             set_status_flag(FLAG_CARRY, (reg_A() & 0x80));
//             result = set_A_register(reg_A() << 1);
//             set_status_flag(FLAG_ZERO, result == 0);
//             set_status_flag(FLAG_NEGATIVE, result >= 0x80);
//             break;

//         case 0x06: ; // ASL Zero Page
//             address = read_increment_PC();
//             ASL_op(emu_read(address), address);
//             break;

//         case 0x0E: ; // ASL Absolute
//             address = get_absolute_addr();
//             ASL_op(emu_read(address), address);
//             break;

//         case 0x4A: ; // LSR A
//             if(reg_A() & 0x01){
//                 set_status_flag(FLAG_CARRY, true);
//             }
//             result = set_A_register(reg_A() >> 1);
//             set_status_flag(FLAG_ZERO, result == 0);
//             set_status_flag(FLAG_NEGATIVE, result >= 0x80);
//             break;

//         case 0x46: ; // LSR Zero Page
//             address = read_increment_PC();
//             LSR_op(emu_read(address), address);
//             break;

//         case 0x4E: ; // LSR Absolute
//             address = get_absolute_addr();
//             LSR_op(emu_read(address), address);
//             break;

//         case 0x2A: ; // ROL A
//             bool new_carry_bit = (reg_A() & 0x80);
//             result = set_A_register(reg_A() << 1);
//             if(reg_status() & FLAG_CARRY){
//                 result = bit_OR_A(0x01);
//             }

//             set_status_flag(FLAG_CARRY, new_carry_bit);
//             set_status_flag(FLAG_ZERO, result == 0);
//             set_status_flag(FLAG_NEGATIVE, result >= 0x80);
//             break;

//         case 0x26: ; // ROL Zero Page
//             address = read_increment_PC();
//             ROL_op(emu_read(address), address);
//             break;

//         case 0x2E: ; // ROL Absolute
//             address = get_absolute_addr();
//             ROL_op(emu_read(address), address);
//             break;

//         case 0x66: ; // ROR Zero Page
//             address = read_increment_PC();
//             ROR_op(emu_read(address), address);
//             break;

//         case 0x6E: ; // ROR Absolute
//             address = get_absolute_addr();
//             ROR_op(emu_read(address), address);
//             break;

//         case 0x09: ; // ORA Immediate
//             result = bit_OR_A(read_increment_PC());
//             set_status_flag(FLAG_ZERO, result == 0);
//             set_status_flag(FLAG_NEGATIVE, result >= 0x80);
//             break;
        
//         case 0x05: ; // ORA Zero Page
//             address = read_increment_PC();
//             temp = bit_OR_A(emu_read(address));
//             emu_write(address, temp);
//             set_status_flag(FLAG_ZERO, temp == 0);
//             set_status_flag(FLAG_NEGATIVE, temp >= 0x80);
//             break;
                
//         case 0x0D: ; // ORA Absolute
//             address = get_absolute_addr();
//             temp = bit_OR_A(emu_read(address));
//             emu_write(address, temp);
//             set_status_flag(FLAG_ZERO, temp == 0);
//             set_status_flag(FLAG_NEGATIVE, temp >= 0x80);
//             break;

//         case 0x49: ; // EOR Immediate
//             result = bit_XOR_A(read_increment_PC());
//             set_status_flag(FLAG_ZERO, result == 0);
//             set_status_flag(FLAG_NEGATIVE, result >= 0x80);
//             break;
        
//         case 0x45: ; // EOR Zero Page
//             address = read_increment_PC();
//             temp = bit_XOR_A(emu_read(address));
//             emu_write(address, temp);
//             set_status_flag(FLAG_ZERO, temp == 0);
//             set_status_flag(FLAG_NEGATIVE, temp >= 0x80);
//             break;
                
//         case 0x4D: ; // EOR Absolute
//             address = get_absolute_addr();
//             temp = bit_XOR_A(emu_read(address));
//             emu_write(address, temp);
//             set_status_flag(FLAG_ZERO, temp == 0);
//             set_status_flag(FLAG_NEGATIVE, temp >= 0x80);
//             break;

//         case 0x29: ; // AND Immediate
//             result = bit_AND_A(read_increment_PC());
//             set_status_flag(FLAG_ZERO, result == 0);
//             set_status_flag(FLAG_NEGATIVE, result >= 0x80);
//             break;
        
//         case 0x25: ; // AND Zero Page
//             address = read_increment_PC();
//             temp = bit_AND_A(emu_read(address));
//             emu_write(address, temp);
//             set_status_flag(FLAG_ZERO, temp == 0);
//             set_status_flag(FLAG_NEGATIVE, temp >= 0x80);
//             break;
                
//         case 0x2D: ; // AND Absolute
//             address = get_absolute_addr();
//             temp = bit_AND_A(emu_read(address));
//             emu_write(address, temp);
//             set_status_flag(FLAG_ZERO, temp == 0);
//             set_status_flag(FLAG_NEGATIVE, temp >= 0x80);
//             break;

//         case 0xE6: ; // INC Zero Page
//             address = read_increment_PC();
//             INC_op(emu_read(address), address);
//             break;

//         case 0xEE: ; // INC Absolute
//             address = get_absolute_addr();
//             INC_op(emu_read(address), address);
//             break;

//         case 0xFE: ; // INC Absolute, X
//             address = get_absolute_addr_X_indexed();
//             INC_op(emu_read(address), address);
//             break;
        
//         case 0xC6: ; // DEC Zero Page
//             address = read_increment_PC();
//             DEC_op(emu_read(address), address);
//             break;

//         case 0xCE: ; // DEC Absolute
//             address = get_absolute_addr();
//             DEC_op(emu_read(address), address);
//             break;

//         case 0x69: ; // ADC Immediate
//             ADC_op(read_increment_PC());
//             break;
        
//         case 0x75: ; // ADC Zero Page, X
//             address = get_ZP_indexed_address(reg_X());
//             ADC_op(emu_read(address));
//             break;

//         case 0x65: ; // ADC Zero Page
//             address = read_increment_PC();
//             ADC_op(emu_read(address));
//             break;
                
//         case 0x6D: ; // ADC Absolute
//             address = get_absolute_addr();
//             ADC_op(emu_read(address));
//             break;

//         case 0xE9: ; // SBC Immediate
//             SBC_op(read_increment_PC());
//             break;
        
//         case 0xE5: ; // SBC Zero Page
//             address = read_increment_PC();
//             SBC_op(emu_read(address));
//             break;
                
//         case 0xED: ; // SBC Absolute
//             address = get_absolute_addr();
//             SBC_op(emu_read(address));
//             break;

//         case 0xC9: ; // CMP
//             CMP_op(read_increment_PC(), reg_A());
//             break;

//         case 0xC5: ; // CMP Zero Page
//             address = read_increment_PC();
//             CMP_op(emu_read(address), reg_A());
//             break;
                
//         case 0xCD: ; // CMP Absolute
//             address = get_absolute_addr();
//             CMP_op(emu_read(address), reg_A());
//             break;

//         case 0xE0: ; // CPX
//             CMP_op(read_increment_PC(), reg_X());
//             break;

//         case 0xE4: ; // CPX Zero Page
//             address = read_increment_PC();
//             CMP_op(emu_read(address), reg_X());
//             break;
                
//         case 0xEC: ; // CPX Absolute
//             address = get_absolute_addr();
//             CMP_op(emu_read(address), reg_X());
//             break;

//         case 0xC0: ; // CPY
//             CMP_op(read_increment_PC(), reg_Y());
//             break;

//         case 0xC4: ; // CPY Zero Page
//             address = read_increment_PC();
//             CMP_op(emu_read(address), reg_Y());
//             break;
                
//         case 0xCC: ; // CPY Absolute
//             address = get_absolute_addr();
//             CMP_op(emu_read(address), reg_Y());
//             break;

//         case 0x24: ; // BIT Zero Page
//             address = read_increment_PC();
//             BIT_op(emu_read(address));
//             break;
                
//         case 0x2C: ; // BIT Absolute
//             address = get_absolute_addr();
//             BIT_op(emu_read(address));
//             break;

//         case 0x00: ; // BRK
//             increment_PC();
//             push_stack((uint8_t)(program_counter() >> 8));
//             push_stack((uint8_t) program_counter());
//             push_stack(reg_status());
//             PC_lowbyte = emu_read(0xFFFE);
//             PC_highbyte = emu_read(0xFFFF);
//             set_PC((PC_highbyte << 8) | PC_lowbyte);
//             break;
        
//         case 0x40: ; // RTI
//             set_status_register(pull_stack());
//             PC_lowbyte = pull_stack();
//             PC_highbyte = pull_stack();
//             set_PC((PC_highbyte << 8) | PC_lowbyte);
//             break;

//         case 0xEA: ; // NOP
//             break;

//         default:
//             // unknown opcode
//             break;
//     }
//     #if DEBUG
//     compare_with_table(opcode, initial_PC);
//     #endif

//     #if TRACELOGGER
//     print_latest_tracelog_entry();
//     #endif

//     emu.total_CPU_cycles += opcode_table[opcode].cycles;
// }

// void emu_run() { 
//     while(!CPU_halted && emu.total_CPU_cycles < 1000){
//         emulate_CPU();
//     }
//     #if TRACELOGGER
//     tracelog_print_entries();
//     #endif
// }

// uint8_t emu_read(uint16_t address) {
//     uint8_t return_value = 0;
    
//     // RAM ($0000-$1FFF) with mirroring
//     if (address < 0x2000) {
//         return_value = emu.RAM[address & 0x7FF];
//     }
//     // PPU Registers ($2000-$3FFF, mirrored)
//     else if (address < 0x4000) {
//         address = 0x2000 + (address & 0x07);
        
//         switch (address) {
//             case 0x2002:  // PPUSTATUS
//                 return_value = ppu.registers.PPUSTATUS;
//                 ppu.registers.PPUSTATUS &= 0x7F;  // Clear VBlank flag
//                 ppu.registers.write_latch = false;  // Reset latch
//                 break;
                
//             case 0x2004:  // OAMDATA
//                 return_value = ppu.OAM[ppu.registers.OAMADDR];
//                 break;
                
//             case 0x2007:  // PPUDATA
//                 return_value = ppu_read_data();
//                 // Auto-increment
//                 if (ppu.registers.PPUCTRL & 0x04) {
//                     ppu.registers.VRAM_address += 32;
//                 } else {
//                     ppu.registers.VRAM_address += 1;
//                 }
//                 ppu.registers.VRAM_address &= 0x3FFF;
//                 break;
                
//             default:
//                 return_value = 0;
//                 break;
//         }
//     }
//     // ROM ($8000-$FFFF)
//     else if (address >= 0x8000) {
//         // Simple NROM mapping for now
//         uint32_t rom_addr = address - 0x8000;
        
//         // If only 16KB PRG, mirror it
//         if (ppu.prg_banks == 1) {
//             rom_addr &= 0x3FFF;  // Mirror $C000-$FFFF to $8000-$BFFF
//         }
        
//         uint32_t prg_offset = 16 + rom_addr;  // Skip 16-byte header
//         return_value = ppu.ROM[prg_offset];
//     }

//     // Track reads for debugging
//     if (address != emu.registers.program_counter) { 
//         #if DEBUG
//         debug_tracker.reads_mem = true;
//         #endif
//         #if TRACELOGGER
//         tail_log_set_read_value(return_value);
//         #endif
//     }

//     return return_value;
// }

// void emu_write(uint16_t address, uint8_t value) {
//     #if DEBUG
//     debug_tracker.writes_mem = true; 
//     #endif 

//     // RAM ($0000-$1FFF) with mirroring
//     if (address < 0x2000) {
//         emu.RAM[address & 0x7FF] = value;
//         #if TRACELOGGER
//         tail_log_set_written_value(value);
//         #endif
//         return;
//     }
    
//     // PPU Registers ($2000-$3FFF, mirrored every 8 bytes)
//     if (address < 0x4000) {
//         address = 0x2000 + (address & 0x07);  // Mirror to $2000-$2007
        
//         switch (address) {
//             case 0x2000:  // PPUCTRL
//                 ppu.registers.PPUCTRL = value;
//                 // Update transfer address bits 10-11 (nametable select)
//                 ppu.registers.transfer_address = 
//                     (ppu.registers.transfer_address & 0xF3FF) | ((value & 0x03) << 10);
//                 break;
                
//             case 0x2001:  // PPUMASK
//                 ppu.registers.PPUMASK = value;
//                 break;
                
//             case 0x2003:  // OAMADDR
//                 ppu.registers.OAMADDR = value;
//                 break;
                
//             case 0x2004:  // OAMDATA
//                 ppu.OAM[ppu.registers.OAMADDR] = value;
//                 ppu.registers.OAMADDR++;  // Auto-increment
//                 break;
                
//             case 0x2005:  // PPUSCROLL
//                 if (!ppu.registers.write_latch) {
//                     // First write: X scroll
//                     ppu.registers.fine_x_scroll = value & 0x07;
//                     ppu.registers.transfer_address = 
//                         (ppu.registers.transfer_address & 0xFFE0) | (value >> 3);
//                 } else {
//                     // Second write: Y scroll
//                     ppu.registers.transfer_address = 
//                         (ppu.registers.transfer_address & 0x8FFF) | ((value & 0x07) << 12);
//                     ppu.registers.transfer_address = 
//                         (ppu.registers.transfer_address & 0xFC1F) | ((value & 0xF8) << 2);
//                 }
//                 ppu.registers.write_latch = !ppu.registers.write_latch;
//                 break;
                
//             case 0x2006:  // PPUADDR
//                 if (!ppu.registers.write_latch) {
//                     // First write: high byte
//                     ppu.registers.transfer_address = 
//                         (ppu.registers.transfer_address & 0x00FF) | ((value & 0x3F) << 8);
//                 } else {
//                     // Second write: low byte
//                     ppu.registers.transfer_address = 
//                         (ppu.registers.transfer_address & 0xFF00) | value;
//                     ppu.registers.VRAM_address = ppu.registers.transfer_address;
//                 }
//                 ppu.registers.write_latch = !ppu.registers.write_latch;
//                 break;
                
//             case 0x2007:  // PPUDATA
//                 ppu_write_data(value);
//                 // Auto-increment VRAM address
//                 if (ppu.registers.PPUCTRL & 0x04) {
//                     ppu.registers.VRAM_address += 32;  // Increment by 32
//                 } else {
//                     ppu.registers.VRAM_address += 1;   // Increment by 1
//                 }
//                 ppu.registers.VRAM_address &= 0x3FFF;  // Wrap at 14 bits
//                 break;
//         }
        
//         #if TRACELOGGER
//         tail_log_set_written_value(value);
//         #endif
//         return;
//     }
    
//     // APU/Controller/DMA registers ($4000-$4017)
//     if (address == 0x4014) {
//         // OAMDMA: Copy 256 bytes from CPU memory to OAM
//         uint16_t src_addr = value << 8;
//         for (int i = 0; i < 256; i++) {
//             ppu.OAM[i] = emu_read(src_addr + i);
//         }
//         #if TRACELOGGER
//         tail_log_set_written_value(value);
//         #endif
//         return;
//     }
    
//     // ROM area ($8000-$FFFF) - read only, ignore writes
//     if (address >= 0x8000) {
//         return;
//     }
    
//     #if TRACELOGGER
//     tail_log_set_written_value(value);
//     #endif
// }


// void push_stack(uint8_t value){
//     emu_write(0x100+emu.registers.stack_pointer, value);
//     emu.registers.stack_pointer--;
    
//     #if DEBUG
//     debug_tracker.reg_read_bitflag |= REG_STACK;
//     debug_tracker.reg_write_bitflag |= REG_STACK;
//     #endif 
// }

// uint8_t pull_stack(){
//     #if DEBUG 
//     bool old_value = debug_tracker.reads_mem; 
//     #endif
//     emu.registers.stack_pointer++;
//     uint8_t result = emu_read(0x100+emu.registers.stack_pointer);

//     #if DEBUG
//     debug_tracker.reg_read_bitflag |= REG_STACK;
//     debug_tracker.reg_write_bitflag |= REG_STACK;
//     debug_tracker.reads_mem = old_value;
//     #endif 

//     return result;
// }

// void emu_reset(){
//     int fd = fopen(emu.filepath, "r");
//     if(fd){
//         print("Opened ");
//         print(emu.filepath);
//         print("\n");
//         fread(emu.header, 0x10, 1, fd);
//         fseek(fd, 0x10, SEEK_SET);
//         fread(emu.ROM, 0x8000, 1, fd);
//         set_status_flag(FLAG_INTERRUPT_DISABLE, true);
//     }

//     uint8_t PC_lowbyte = emu_read(0xFFFC);
//     uint8_t PC_highbyte = emu_read(0xFFFD);
//     emu.registers.program_counter = (PC_highbyte << 8) | PC_lowbyte;
//     emu.registers.stack_pointer = 0xFD;
//     print("\n");
//     emu_run();
// }

// uint8_t read_increment_PC(){
//     uint8_t result = emu_read(emu.registers.program_counter);
//     emu.registers.program_counter++;
//     #if TRACELOGGER
//     tail_log_set_operand(result);
//     #endif
//     return result;
// }

// uint8_t read_PC(){
//     uint8_t result = emu_read(emu.registers.program_counter);
//     #if TRACELOGGER
//     tail_log_set_operand(result);
//     #endif
//     return result;
// }

// void increment_PC(){
//     emu.registers.program_counter++;
// }

// uint8_t set_PC(uint16_t address) { 
//     #if TRACELOGGER
//     tail_log_set_address(address);
//     #endif
//     emu.registers.program_counter = address; 
//     return address;
// }

// void set_status_flag(uint8_t flag, bool condition){
//     if(condition){
//         emu.registers.flags |= flag;
//     } else {
//         emu.registers.flags &= ~flag;
//     }
//     #if DEBUG
//     debug_tracker.reg_write_bitflag |= REG_STATUS; 
//     #endif 
// }


// void emu_enable_logger(bool is_enabled){
//     logger_enabled = is_enabled;
// }

// // Get ZP address indexed to X
// uint16_t get_ZP_indexed_address(uint8_t reg_value){
//     uint8_t indexed_address = read_increment_PC() + reg_value;

//     #if TRACELOGGER
//     tail_log_set_indexed_address((uint16_t)indexed_address);
//     #endif

//     return indexed_address;
// }


// // Get absolute address from byte in memory
// uint16_t get_absolute_addr(){
//     uint8_t PC_lowbyte = read_increment_PC();
//     uint8_t PC_highbyte = read_increment_PC();
//     uint16_t address = (PC_highbyte << 8) | PC_lowbyte;

//     #if TRACELOGGER
//     tail_log_set_address(address);
//     #endif
    
//     return address;
// }

// // Get absolute address indexed to X
// uint16_t get_absolute_addr_X_indexed(){
//     uint8_t PC_lowbyte = read_increment_PC();
//     uint8_t PC_highbyte = read_increment_PC();
//     uint16_t address = (PC_highbyte << 8) | PC_lowbyte;
//     address += reg_X();

//     #if TRACELOGGER
//     tail_log_set_address(address);
//     #endif

//     return address;
// }

// // Get absolute address indexed to Y
// uint16_t get_absolute_addr_Y_indexed(){
//     uint8_t PC_lowbyte = read_increment_PC();
//     uint8_t PC_highbyte = read_increment_PC();
//     uint16_t address = (PC_highbyte << 8) | PC_lowbyte;
//     address += reg_Y();

//     #if TRACELOGGER
//     tail_log_set_address(address);
//     #endif

//     return address;
// }

// // Get indirect indexed address to Y 
// uint16_t get_indirect_address_Y_indexed(){
//     uint8_t address = read_increment_PC();
//     uint8_t PC_lowbyte = emu_read(address);
//     uint8_t PC_highbyte = emu_read(address + 1);
//     uint16_t indirect_addr = (PC_highbyte << 8) | PC_lowbyte;
//     indirect_addr += reg_Y();

//     #if TRACELOGGER
//     tail_log_set_indexed_address(indirect_addr);
//     #endif

//     return indirect_addr;
// }

// // Get indexed indirect address to X 
// uint16_t get_indirect_address_X_indexed(){
//     uint8_t address = read_increment_PC() + reg_X();
//     uint8_t PC_lowbyte = emu_read(address);
//     uint8_t PC_highbyte = emu_read(address + 1);
//     uint16_t indirect_addr = (PC_highbyte << 8) | PC_lowbyte;

//     #if TRACELOGGER
//     tail_log_set_indexed_address(indirect_addr);
//     #endif

//     return indirect_addr;
// }

// void ASL_op(uint8_t input, uint16_t address){
//     set_status_flag(FLAG_CARRY, (input & 0x80));
//     input <<= 1;
//     emu_write(address, input);
//     set_status_flag(FLAG_ZERO, input == 0);
//     set_status_flag(FLAG_NEGATIVE, input >= 0x80);
// }

// void LSR_op(uint8_t input, uint16_t address){
//     set_status_flag(FLAG_CARRY, (input & 0x01)); 
//     input >>= 1;
//     emu_write(address, input);
//     set_status_flag(FLAG_ZERO, input == 0);
//     set_status_flag(FLAG_NEGATIVE, input >= 0x80);
// }

// void ROL_op(uint8_t input, uint16_t address){
//     bool new_carry_bit = (input >= 0x80);
//     input <<= 1;
//     if(reg_status() & FLAG_CARRY){
//         input |= 0x1;
//     }
//     emu_write(address, input);
//     set_status_flag(FLAG_CARRY, new_carry_bit);
//     set_status_flag(FLAG_ZERO, input == 0x00);
//     set_status_flag(FLAG_NEGATIVE, input >= 0x80);
// }

// void ROR_op(uint8_t input, uint16_t address){
//     bool new_carry_bit = (input & 0x01);
//     input >>= 1;
//     if(reg_status() & FLAG_CARRY){
//         input |= 0x80;
//     }
//     emu_write(address, input);
//     set_status_flag(FLAG_CARRY, new_carry_bit);
//     set_status_flag(FLAG_ZERO, input == 0);
//     set_status_flag(FLAG_NEGATIVE, input >= 0x80);
// }

// // Add with carry
// void ADC_op(uint8_t input){
//     int sum = reg_A() + input;
//     if(reg_status() & FLAG_CARRY){
//         sum++;
//     }
//     // Set overflow flag if add takes us from 2 pos. values -> neg. value or two neg. values -> pos. value 
//     set_status_flag(FLAG_OVERFLOW, (~bit_XOR_A(input) & bit_XOR_A(sum) & 0x80) != 0);
//     set_status_flag(FLAG_CARRY, sum > 0xFF);
//     uint8_t result = set_A_register((uint8_t) sum);
//     set_status_flag(FLAG_ZERO, result == 0);
//     set_status_flag(FLAG_NEGATIVE, result >= 0x80);
// }

// // Subtract with carry
// void SBC_op(uint8_t input){
//     int sum = reg_A() - input;
//     if((~reg_status() & FLAG_CARRY)){
//         sum--;
//     }
//     // Set overflow flag if add takes us from 2 pos. values -> neg. value or two neg. values -> pos. value 
//     set_status_flag(FLAG_OVERFLOW, (bit_XOR_A(input) & bit_XOR_A(sum) & 0x80) != 0);
//     set_status_flag(FLAG_CARRY, sum >= 0);
//     uint8_t result = set_A_register((uint8_t) sum);
//     set_status_flag(FLAG_ZERO, result == 0);
//     set_status_flag(FLAG_NEGATIVE, result >= 0x80);
// }

// void INC_op(uint8_t input, uint16_t address){
//     input++;
//     emu_write(address, input);
//     set_status_flag(FLAG_ZERO, input == 0);
//     set_status_flag(FLAG_NEGATIVE, input >= 0x80);
// }

// void DEC_op(uint8_t input, uint16_t address){
//     input--;
//     emu_write(address, input);
//     set_status_flag(FLAG_ZERO, input == 0);
//     set_status_flag(FLAG_NEGATIVE, input >= 0x80);
// }

// void CMP_op(uint8_t input, uint8_t reg_value){
//     set_status_flag(FLAG_CARRY, reg_value >= input);
//     set_status_flag(FLAG_ZERO, reg_value == input);
//     set_status_flag(FLAG_NEGATIVE, reg_value - input > 127);
// }

// void BIT_op(uint8_t input){
//     set_status_flag(FLAG_ZERO, (reg_A() & input) == 0);
//     set_status_flag(FLAG_NEGATIVE, (input & 0x80) != 0);
//     set_status_flag(FLAG_OVERFLOW, (input & 0x40) != 0);
// }
