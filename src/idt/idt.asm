section .asm

extern int21h_handler
extern no_interrupt_handler
extern pit_handler        ; C function from pit.c

global int21h
global idt_load
global no_interrupt
global enable_interrupts
global disable_interrupts
global pit_handler_asm    ; new symbol

enable_interrupts:
    sti
    ret

disable_interrupts:
    cli
    ret

idt_load:
    push ebp
    mov ebp, esp

    mov ebx, [ebp+8]
    lidt [ebx]

    pop ebp
    ret

int21h:
    cli
    pushad
    call int21h_handler
    popad
    sti
    iret

no_interrupt:
    cli
    pushad
    call no_interrupt_handler
    popad
    sti
    iret

; --- NEW: PIT IRQ0 stub ---
pit_handler_asm:
    cli
    pushad
    call pit_handler        ; C function
    popad
    sti
    iret