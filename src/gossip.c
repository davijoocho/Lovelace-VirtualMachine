#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "garbage_collect.h"
#include "virtual_machine.h"


int main(int argc, char* argv[])
{
    uint8_t byte_code[60] = {
        QUAD_CONST, 0x0A, 0x00, 0x00, 0x00,
        QUAD_CONST, 0x0A, 0x00, 0x00, 0x00,
        QUAD_CONST, 0x0A, 0x00, 0x00, 0x00,
        NEW_ARRAY, 0x03, 0x04,
        END
    };

    struct virt_mach* vm = malloc(sizeof(struct virt_mach));
    vm->byte_code = byte_code;
    vm->execution_stack = NULL;

    execute(vm, 0);

    return 0;
}

