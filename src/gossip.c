#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "garbage_collect.h"
#include "virtual_machine.h"


int main(int argc, char* argv[])
{
    uint8_t byte_code[60] = {
        QUAD_CONST, 0x06, 0x00, 0x00, 0x00,
        QUAD_CONST, 0x06, 0x00, 0x00, 0x00,
        IADD,
        NEW_QUAD_ARRAY, 0x00,
        REF_STORE, 0x00,
        REF_LOAD, 0x00,
        OFFSET, 0x00, 0x00, 0x00, 0x00,
        GET_OFFSET,
        QUAD_CONST, 0x10, 0x00, 0x00, 0x00,
        SET_QUAD_FIELD,
        REF_LOAD, 0x00,
        OFFSET, 0x00, 0x00, 0x00, 0x00,
        GET_QUAD_FIELD,
        SYS_CALL,
        RETURN,
        LOCAL_CALL, 0x00, 0xD3, 0xFF, 0xFF, 0xFF,
        END
    };

    struct virt_mach* vm = malloc(sizeof(struct virt_mach));
    vm->byte_code = byte_code;
    vm->execution_stack = NULL;

    execute(vm, 39);

    return 0;
}

