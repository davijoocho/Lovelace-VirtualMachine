#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "garbage_collect.h"
#include "virtual_machine.h"


int main(int argc, char* argv[])
{
    /*
    uint8_t byte_code[38] = {
        0x1A, 0x00, 0x05, 0x00,
        ISETFIELD,
        ISETFIELD,
        LSETFIELD,
        STRUCT_SET_FIELD,
        REF_OFFSETS, 0x01,
        0x08, 0x00,
        NULL_REF,
        LCONST, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        ICONST, 0x01, 0x00, 0x00, 0x00,
        ICONST, 0x02, 0x00, 0x00, 0x00,
        LOCAL_STRUCT_INIT, 0xDB, 0xFF, 0xFF, 0xFF,
        END
    };


    printf("GEN_0: %p  FREE_PTR_0: %p\n", mem.gen_ptrs[0], mem.free_ptrs[0]);
    printf("GEN_1: %p  FREE_PTR_1: %p\n", mem.gen_ptrs[1], mem.free_ptrs[1]);
    printf("GEN_2: %p  FREE_PTR_2: %p\n", mem.gen_ptrs[2], mem.free_ptrs[2]);
    printf("HEAP MEMORY\n");

    for (int i = 0; i < HEAP_SIZE; i++) {
        if (i % 256 == 0 && i != 0) {
            printf("\n");
        }
        printf("%d", mem.heap[i]);
    }

    struct virt_mach* vm = malloc(sizeof(struct virt_mach));
    vm->byte_code = byte_code;
    vm->execution_stack = NULL;

    execute(vm, 12);
    */

    return 0;
}

