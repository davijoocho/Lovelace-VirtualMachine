#include <stddef.h>
#include "virtual_machine.h"

// TEMPORARY DEFAULT SIZE
// DON'T IMPLEMENT HEAP RESIZING YET

#define HEAP_SIZE 4096
#define GEN_0 0;
#define GEN_1 1024;
#define GEN_2 2048;

struct mem_handle {
    int8_t* heap_ptr;
    int8_t is_active;            
    int8_t gen;
    int32_t ref_ptr; 
    int32_t size;             

    struct mem_handle* prev;
    struct mem_handle* next;
};

struct mem_root {
    int32_t* local_ptr;           // pointer to local variable in some arbitrary stack frame.
    struct mem_root* prev;
    struct mem_root* next;
};

struct rem_set {
    uint16_t n_ptrs;
    uint16_t capacity;
    struct mem_handle** ptr_set;
};

struct mem_info {
    int8_t* heap;        
    int8_t* gen_ptrs[4]; 
    int8_t* free_ptrs[3];

    struct mem_handle sent_handle;
    struct mem_handle* last_handles[3];
    struct mem_root root_sets[3];  // [0]: GEN0 [1]: GEN1 [2]: GEN2
    struct rem_set rem_sets[2];    // TO-DO: SET_FIELDS AND AALOADS
};

// ALLOCATE
void init_mem_info(struct mem_info* mem);
struct mem_handle* allocate_mem(struct mem_info* mem, int8_t* byte_code, int32_t size, int32_t ref);    // FOR NEW ARRAYS AND STRUCTS
struct mem_root* alloc_mem_root(struct mem_info* mem, int32_t* local);

// GARBAGE COLLECT
void free_mem(struct mem_info* mem, int8_t* byte_code);
void mark_roots(struct mem_info* mem, uint32_t* active_mem, int8_t* byte_code, int8_t gen_c, struct rem_set* gt_gen);      
void mark_handles(struct mem_info* mem, struct mem_handle* handle, 
        uint32_t* active_mem, int8_t* byte_code, int8_t gen_c, struct rem_set* gt_gen);
void sweep(struct mem_info* mem, struct rem_set* gt_gen, int8_t gen_c, uint8_t has_recursed);
void update_roots(struct mem_info* mem, int8_t upto_gen);




