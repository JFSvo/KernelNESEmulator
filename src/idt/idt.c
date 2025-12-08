
#include "idt.h"  
#include "config.h"  
#include "kernel.h" 
#include "memory/memory.h"
#include "io/io.h"

struct idt_desc idt_descriptors[PEACHOS_TOTAL_INTERRUPTS];  
struct idtr_desc idtr_descriptor;  

extern void idt_load(struct idtr_desc* ptr);  
extern void int21h();
extern void no_interrupt();
extern void pit_irq();


void int21h_handler(){
    print("Keyboard pressed!\n");
    outb(0x20, 0x20);
}

void no_interrupt_handler(){
    outb(0x20, 0x20);
}

void idt_zero()  {  
    print("Divide by zero error\n");  
}  

/**
* This function is used to map an interrupt to an address 
* for example if you passed 0x80 as the interrupt number then  
* whenever you run INT 0x80 the CPU would begin executing  
* at the address provided.  
*/  
void idt_set(int interrupt_no, void* address)  {
    struct idt_desc* desc = &idt_descriptors[interrupt_no];  
    desc->offset_1 = (uint32_t) address & 0x0000ffff;  
    desc->selector = KERNEL_CODE_SELECTOR;  
    desc->zero = 0x00;  
    desc->type_attr = 0xEE;  
    desc->offset_2 = (uint32_t) address >> 16;  
}  

void idt_init()  {  
    // memset will be implemented soon  
    memset(idt_descriptors, 0, sizeof(idt_descriptors));  
    
    // This is for the actual table size  
    idtr_descriptor.limit = sizeof(idt_descriptors) -1;  
    // This is the base pointer of where our interrupt descriptor  
    // table is stored. We pass in the pointer  
    // and cast it to uin32_t to store it in the  
    // descriptor.  
    idtr_descriptor.base = (uint32_t) idt_descriptors;  

    for (int i = 0; i < PEACHOS_TOTAL_INTERRUPTS; i++){
        idt_set(i, no_interrupt);
    }
    
    // Map our divide by zero interrupt handler  
    // now when there's an interrupt exception, our idt_zero function  
    // will print Divide by zero error.  
    idt_set(0, idt_zero);  
    idt_set(0x21, int21h);
    idt_set(0x20, pit_irq);
    
    // Load the interrupt descriptor table  
    idt_load(&idtr_descriptor);  
} 