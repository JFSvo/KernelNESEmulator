#ifndef EMULATOR_DEBUG_H
#define EMULATOR_DEBUG_H

#if DEBUG
// TBD: set some flag for the debug conditions that marks when registers are accessed for an instruction
static inline void set_A_register(uint8_t value) { emu.registers.A = value;  }
static inline uint8_t reg_A(void) { return emu.registers.A; }

static inline void set_X_register(uint8_t value) { emu.registers.X = value; }
static inline uint8_t reg_X(void) { return emu.registers.X; }

static inline void set_Y_register(uint8_t value) { emu.registers.Y = value; }
static inline uint8_t reg_Y(void) { return emu.registers.Y; }

static inline void set_PC(uint16_t value) { emu.registers.program_counter = value; }
static inline void increment_PC() { emu.registers.program_counter++; }
static inline uint16_t program_counter(void) { return emu.registers.program_counter; }

#else
static inline void set_A_register(uint8_t value) { emu.registers.A = value;  }
static inline uint8_t reg_A(void) { return emu.registers.A; }

static inline void set_X_register(uint8_t value) { emu.registers.X = value; }
static inline uint8_t reg_X(void) { return emu.registers.X; }

static inline void set_Y_register(uint8_t value) { emu.registers.Y = value; }
static inline uint8_t reg_Y(void) { return emu.registers.Y; }

static inline void set_PC(uint16_t value) { emu.registers.program_counter = value; }
static inline void increment_PC() { emu.registers.program_counter++; }
static inline uint16_t program_counter(void) { return emu.registers.program_counter; }
#endif

#endif