#ifndef TRACELOGGER_H
#define TRACELOGGER_H

#define TRACELOG_MAX_LENGTH 256

struct tracelog_entry {
    uint16_t address;
    uint8_t opcode;
    uint8_t A; 
    uint8_t X;
    uint8_t Y;
    uint8_t flags;
};

#endif