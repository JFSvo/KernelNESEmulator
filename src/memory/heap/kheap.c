#include "kheap.h" 
#include "heap.h" 
#include "config.h"
#include "kernel.h" 
#include "memory/memory.h"

struct heap kernel_heap; 
struct heap_table kernel_heap_table; 

/** 
* This function creates our kernel heap and initializes it 
*/ 
void kheap_init() { 
    int total_table_entries = PEACHOS_HEAP_SIZE_BYTES / PEACHOS_HEAP_BLOCK_SIZE; 
    kernel_heap_table.entries = (HEAP_BLOCK_TABLE_ENTRY*)( PEACHOS_HEAP_TABLE_ADDRESS); 
    kernel_heap_table.total = total_table_entries; 
    
    void* end = (void*)(PEACHOS_HEAP_ADDRESS + PEACHOS_HEAP_SIZE_BYTES); 
    int res = heap_create(&kernel_heap, (void*)( PEACHOS_HEAP_ADDRESS), end, &kernel_heap_table); 
    if (res < 0) { 
        print("Failed to create heap\n"); 
    } 
} 

/** 
* Use this function to allocate kernel memory, its just like the "malloc" function 
*/ 
void* kmalloc(size_t size) {
    return heap_malloc(&kernel_heap, size); 
} 

/** 
* Use this function to free kernel memory 
*/ 
void kfree(void* ptr) { 
    heap_free(&kernel_heap, ptr); 
}

void* kzalloc(size_t size)
{
    void* ptr = kmalloc(size);
    if (!ptr)
        return 0;

    memset(ptr, 0x00, size);
    return ptr;
}