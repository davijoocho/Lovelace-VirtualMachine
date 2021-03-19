
#include "garbage_collect.h"
#include <stdio.h>
#include <stdlib.h>

void free_mem(struct mem_info* mem, int8_t* byte_code) 
{
    struct o_rem_set old_gen = {.n_ptrs = 0, .capacity = 16, .ptr_set = malloc(sizeof(struct mem_handle*) * 16)};
    uint32_t active_mem = 0;
    uint8_t no_free_space = 1; 
    int8_t i = 0;

    while (no_free_space) {
        active_mem = 0;
        mark_roots(mem, &active_mem, byte_code, i, &old_gen);

        if (i == 2 || (i < 2 && mem->free_ptrs[i+1] + active_mem <= mem->gen_ptrs[i+2])) {
            sweep(mem, &old_gen, i, 0);
            update_roots(mem, i);
            no_free_space = 0;
        }
        i += 1;
    }
}

void mark_roots(struct mem_info* mem, uint32_t* active_mem, int8_t* byte_code, int8_t gen_c, struct o_rem_set* old_gen)
{
    struct mem_root* root = mem->root_sets[gen_c].next; 
    struct mem_handle* handle;

    while (root != &mem->root_sets[gen_c]) {
        memcpy(&handle, root->local_ptr, 8);
        mark_handles(mem, handle, active_mem, byte_code, gen_c, old_gen);
        root = root->next;
    }

    if (gen_c < 2) {
        struct y_rem_set* set = &mem->rem_sets[gen_c];
        for (int i = 0; i < set->n_ptrs; i++) {
            if (set->ptr_set[i] != NULL) {
                mark_handles(mem, set->ptr_set[i], active_mem, byte_code, gen_c, old_gen);
            }
        }

        set->empty_slots = realloc(set->empty_slots, sizeof(uint16_t) * 16);
        set->n_empty = 0;
        set->empty_cap = 16;

        set->ptr_set = realloc(set->ptr_set, sizeof(struct mem_handle*) * 16);
        set->n_ptrs = 0;
        set->capacity = 16;
    }
}

void mark_handles(struct mem_info* mem, struct mem_handle* handle, 
        uint32_t* active_mem, int8_t* byte_code, int8_t gen_c, struct o_rem_set* old_gen)
{
    // -1: ALL REFERENCES, >= 0: PARTIAL REFERENCES
    if (handle->gen >= gen_c && handle->is_active != 1) {
        handle->is_active = 1;

        if (handle->gen == gen_c) {
            *active_mem += handle->size;
        }

        if (gen_c < 2 && handle->gen > gen_c) { 
            if (old_gen->n_ptrs == old_gen->capacity) {
                old_gen->ptr_set = realloc(old_gen->ptr_set, old_gen->capacity * sizeof(struct mem_handle*) * 2);
            }
            old_gen->ptr_set[old_gen->n_ptrs++] = handle;
        }


        if (handle->ref_ptr == -1 || handle->ref_ptr >= 0) {
            int32_t pos = handle->ref_ptr;  // NOT USED IF FLAG == -1
            int64_t h_addr;
            int16_t n = 0;
            int16_t offset;   
            int16_t n_iter = (handle->ref_ptr == -1) ? handle->size / 10 : *(byte_code + pos++);

            while (n < n_iter) {
                if (handle->ref_ptr == -1) {
                    offset = n * 10;
                } else {
                    memcpy(&offset, byte_code + pos, 2);
                    pos += 2;
                }

                memcpy(&h_addr, handle->heap_ptr + offset, 8);
                if (h_addr != 0) {
                    mark_handles(mem, (struct mem_handle*)h_addr, active_mem, byte_code, gen_c, old_gen);
                }

                n += 1;
            }
        }
    }
}

void sweep(struct mem_info* mem, struct o_rem_set* old_gen, int8_t gen_c, uint8_t has_recursed)
{
    if (gen_c >= 0) {
        if (!has_recursed) {
            if (gen_c < 2) {
                for (int i = 0; i < old_gen->n_ptrs; i++) {
                    if (old_gen->ptr_set[i]->gen > gen_c) {
                        old_gen->ptr_set[i]->is_active = 0;
                    }
                }
            }
            free(old_gen->ptr_set);
        } 

        struct mem_handle* handle = mem->last_handles[gen_c];
        int i = 1;
        if (gen_c < 2) {
            while (handle != &mem->sent_handle && handle->gen == gen_c) {
                if (handle->is_active) {
                    memcpy(mem->free_ptrs[gen_c + 1], handle->heap_ptr, handle->size);

                    handle->heap_ptr = mem->free_ptrs[gen_c + 1];
                    mem->free_ptrs[gen_c + 1] += handle->size;

                    handle->gen += 1;
                    handle->is_active = 0;

                    handle = handle->prev;
                } else {
                    struct mem_handle* dealloc_handle = handle;
                    handle->next->prev = handle->prev;
                    handle->prev->next = handle->next;

                    handle = handle->prev;
                    free(dealloc_handle);
                }
                i += 1;
            }
            mem->last_handles[gen_c + 1] = handle->next;
            mem->last_handles[gen_c] = NULL;
            mem->free_ptrs[gen_c] = mem->gen_ptrs[gen_c];
        } else {
            struct mem_handle* lw_addr_hndl = NULL;

            while (handle->gen == gen_c) {
                if (handle->is_active) {
                    if (lw_addr_hndl == NULL || handle->heap_ptr < lw_addr_hndl->heap_ptr) {
                        lw_addr_hndl = handle;
                    }
                    handle->is_active = 0;
                    handle = handle->prev;
                } else {
                    struct mem_handle* non_active_hndl = handle;

                    handle->next->prev = handle->prev;
                    handle->prev->next = handle->next;

                    handle = handle->prev;
                    free(non_active_hndl);
                }
            }

            mem->free_ptrs[gen_c] = mem->gen_ptrs[gen_c];

            if (lw_addr_hndl != NULL) {
                memcpy(mem->free_ptrs[gen_c], lw_addr_hndl->heap_ptr, lw_addr_hndl->size);
                lw_addr_hndl->heap_ptr = mem->free_ptrs[gen_c];
                mem->free_ptrs[gen_c] += lw_addr_hndl->size;
            }
    
            handle = handle->next;
            while (handle != &mem->sent_handle) {
                if (handle != lw_addr_hndl) {
                    memcpy(mem->free_ptrs[gen_c], handle->heap_ptr, handle->size);
                    handle->heap_ptr = mem->free_ptrs[gen_c];
                    mem->free_ptrs[gen_c] += handle->size;
                }
                handle = handle->next;
            }

            if (lw_addr_hndl == NULL) {
                mem->last_handles[gen_c] = NULL; 
            } else {
                mem->last_handles[gen_c] = handle->prev;
            }
        }

        sweep(mem, NULL, gen_c - 1, 1);
    }
}


void update_root(struct mem_info* mem, struct mem_handle* new_handle, int32_t* local) 
{
    struct mem_root* root;
    memcpy(&root, local + 2, 8);

    struct mem_handle* prev_handle;
    memcpy(&prev_handle, local, 8);

    if (new_handle == NULL && root != NULL) {
        memset(local, 0, 16);
        root->next->prev = root->prev;
        root->prev->next = root->next;
        free(root);

    } else if (new_handle->gen != prev_handle->gen) {
        struct mem_root* to_root = &mem->root_sets[new_handle->gen];

        root->next->prev = root->prev;
        root->prev->next = root->next;

        root->prev = to_root;
        root->next = to_root->next;
        root->prev->next = root;
        root->next->prev = root;

        memcpy(local, &new_handle, 8);

    } else if (new_handle != NULL && root == NULL) {
        struct mem_root* new_root = malloc(sizeof(struct mem_root));
        struct mem_root* to_root = &mem->root_sets[new_handle->gen];

        new_root->prev = to_root;
        new_root->next = to_root->next;
      
        new_root->prev->next = new_root;
        new_root->next->prev = new_root;

        new_root->local_ptr = local;

        memcpy(local, &new_handle, 8);
        memcpy(local + 2, &new_root, 8);
    }
}

void update_roots(struct mem_info* mem, int8_t upto_gen)
{
    struct mem_handle* handle;
    struct mem_root* to_root;

    for (int i = 0; i <= upto_gen; i++) {
        struct mem_root* from_root = mem->root_sets[i].next;

        while (from_root != &mem->root_sets[i]) {
            memcpy(&handle, from_root->local_ptr, 8);

            if (handle->gen != i) {
                from_root->prev->next = from_root->next;
                from_root->next->prev = from_root->prev;

                to_root = &mem->root_sets[handle->gen];

                from_root->prev = to_root;
                from_root->next = to_root->next;
                from_root->prev->next = from_root;
                from_root->next->prev = from_root;
            }

            from_root = from_root->next;
        }
    }
}

struct mem_handle* allocate_mem(struct mem_info* mem, int8_t* byte_code, int32_t size, int32_t ref)
{
    struct mem_handle* handle = malloc(sizeof(struct mem_handle));

    // if no free space in generation 1, free memory
    if (mem->free_ptrs[0] + size > mem->gen_ptrs[1])
        free_mem(mem, byte_code);

    // if no last handle exists for generation 1, set the allocated handle to last handle of generation 1
    if (mem->last_handles[0] == NULL)
        mem->last_handles[0] = handle;

    // initialize mem_handle
    handle->heap_ptr = mem->free_ptrs[0];
    handle->size = size;
    handle->is_active = 0;
    handle->gen = 0;
    handle->ref_ptr = ref;

    // set front, back pointers of mem_handle
    handle->prev = &mem->sent_handle;
    handle->next = mem->sent_handle.next;
    handle->prev->next = handle;
    handle->next->prev = handle;

    // increment free_ptr by allocated size
    mem->free_ptrs[0] += size;

    return handle;
}

// Fix this ugliness (MACRO?)
void init_mem_info(struct mem_info* mem)
{
    mem->heap = malloc(HEAP_SIZE);

    mem->gen_ptrs[0] = mem->free_ptrs[0] = mem->heap + GEN_0;
    mem->gen_ptrs[1] = mem->free_ptrs[1] = mem->heap + GEN_1;
    mem->gen_ptrs[2] = mem->free_ptrs[2] = mem->heap + GEN_2;
    mem->gen_ptrs[3] = mem->heap + HEAP_SIZE;

    mem->sent_handle.next = mem->sent_handle.prev = &mem->sent_handle;

    for (int i = 0; i < 3; i++) {
        mem->last_handles[i] = NULL;
    }

    for (int i = 0; i < 3; i++) {
        mem->root_sets[i].next = mem->root_sets[i].prev = &mem->root_sets[i];
    }

    for (int i = 0; i < 2; i++) {
        struct y_rem_set* rem_set = &mem->rem_sets[i];

        rem_set->ptr_set = malloc(sizeof(struct mem_handle*) * 16);
        rem_set->n_ptrs = 0;
        rem_set->capacity = 16;

        rem_set->empty_slots = malloc(sizeof(uint16_t) * 16);
        rem_set->n_empty = 0;
        rem_set->empty_cap = 16;
    }
}
