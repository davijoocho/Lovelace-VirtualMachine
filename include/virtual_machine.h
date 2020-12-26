#ifndef VIRTUAL_MACHINE_H
#define VIRTUAL_MACHINE_H

#include <stdint.h>



#define MAX_OPERAND_STACK 256
#define MAX_EXECUTION_STACK 128
#define MAX_LOCAL 16
#define MAX_GLOBAL 32

typedef enum op_c
{
    IADD = 0x01,
    ISUB = 0x02,
    IMUL = 0x03,
    IDIV = 0x04,
    LOAD = 0x05,
    STORE = 0x06,
    GLOAD = 0x07,
    GSTORE = 0x08,
    ICONST = 0x09,
    //AND = 0x0A,
    //OR = 0x0B,
    //NOT = 0x0C,
    //NEG = 0x0D,
    JMP = 0x0E,
    JMPF = 0x0F,
    JMPT = 0x10,
    //EQ = 0x11,
    //GT = 0x12,
    //LT = 0x13,
    //GTEQ = 0x14,
    LTEQ = 0x15,
    CALL = 0x16,
    RET = 0x17,
    OUT = 0x18,
    END = 0x19
} op_c;

typedef struct stack_frame
{
    int ret_addr;
    int locals[MAX_LOCAL];
} stack_frame;

typedef struct virt_mach
{
    uint8_t* byte_c;
    int operand_stack[MAX_OPERAND_STACK];
    stack_frame exec_stack[MAX_EXECUTION_STACK];
    int globals[MAX_GLOBAL]; 
} virt_mach;


void execute_bytec (virt_mach* vm, int main_fn, uint8_t* const_pool);
virt_mach* construct_vm (uint8_t* bcode);
void construct_frame (virt_mach* vm, int sp, int argc, stack_frame* frame, int ret_pc);
void dealloc_vm (virt_mach* vm);

#endif

