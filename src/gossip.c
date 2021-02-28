#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "garbage_collect.h"
#include "virtual_machine.h"


int main(int argc, char* argv[])
{
    int8_t byte_code[20] = {
        0x30, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x00, 0x08, 0x40,
        0x30, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x00, 0xf8, 0x3f,
        0x31,
        0x6D
    };

   
    /*
    printf("GEN_0: %p  FREE_PTR_0: %p\n", mem.gen_ptrs[0], mem.free_ptrs[0]);
    printf("GEN_1: %p  FREE_PTR_1: %p\n", mem.gen_ptrs[1], mem.free_ptrs[1]);
    printf("GEN_2: %p  FREE_PTR_2: %p\n", mem.gen_ptrs[2], mem.free_ptrs[2]);
    printf("HEAP MEMORY\n");

    for (int i = 0; i < HEAP_SIZE; i++) {
        if (i % 100 == 0 && i != 0) {
            printf("\n");
        }
        printf("%d", mem.heap[i]);
    }
    */

    /*
    struct virt_mach* vm = malloc(sizeof(struct virt_mach));
    vm->byte_code = byte_code;
    vm->execution_stack = NULL;

    execute(vm, 0);
    */

    return 0;
}

