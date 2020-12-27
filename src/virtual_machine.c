#include <stdio.h>
#include <stdlib.h>
#include "virtual_machine.h"

virt_mach* construct_vm (uint8_t* bcode)
{
    virt_mach* vm = malloc(sizeof(virt_mach));
    vm->byte_c = bcode;
    return vm;
}

void dealloc_vm (virt_mach* vm) { free(vm); }

void construct_frame (virt_mach* vm, int sp, int argc, stack_frame* frame, int ret_pc) { 
    frame->ret_addr = ret_pc; 
    for (int i = 0; i < argc; i++) {
        frame->locals[i] = vm->operand_stack[sp-i];
    }
}


void execute_bytec (virt_mach* vm, int main_fn, uint8_t* const_pool)
{
    int sp = -1; // operand stack pointer
    int pc = main_fn; // program counter 
    int esi = 0; // execution stack index
    int cpi;

    op_c instruction = (op_c)vm->byte_c[pc];  // op_code
    int r, l, v, argc, ret_a, addr, offset; 

    while (1)
    {
        pc++;
        switch (instruction)
        {
            case IADD:
                r = vm->operand_stack[sp--];
                l = vm->operand_stack[sp--];
                vm->operand_stack[++sp] = l + r;
                break;

            case ISUB:
                r = vm->operand_stack[sp--];
                l = vm->operand_stack[sp--];
                vm->operand_stack[++sp] = l - r;
                break;

            case IMUL:
                r = vm->operand_stack[sp--];
                l = vm->operand_stack[sp--];
                vm->operand_stack[++sp] = l * r;
                break;

            case IDIV:
                r = vm->operand_stack[sp--];
                l = vm->operand_stack[sp--];
                vm->operand_stack[++sp] = l / r;
                break;

            case LOAD:
                offset = vm->byte_c[pc++];
                vm->operand_stack[++sp] = vm->exec_stack[esi].locals[offset];
                break;

            case STORE:
                offset = vm->byte_c[pc++];
                vm->exec_stack[esi].locals[offset] = vm->operand_stack[sp--];
                break;

            case GLOAD:
                addr = vm->byte_c[pc++];
                vm->operand_stack[++sp] = vm->globals[addr];
                break;

            case GSTORE:
                addr = vm->byte_c[pc++];
                vm->globals[addr] = vm->operand_stack[sp--];
                break;

            case ICONST:
                cpi = 0;
                cpi = (cpi | vm->byte_c[pc+1]) << 8 | (cpi | vm->byte_c[pc]);
                pc += 2;
                v = 0;
                v = (v | const_pool[cpi+3]) << 24 | (v | const_pool[cpi+2]) << 16 | 
                    (v | const_pool[cpi+1]) << 8 | (v | const_pool[cpi]); 
                vm->operand_stack[++sp] = v;
                break;

            case JMP: 
                addr = 0;
                addr = (addr | vm->byte_c[pc+1]) << 8 | (addr | vm->byte_c[pc]);
                pc = addr;
                break;

            case JMPF:  
                addr = 0;
                addr = (addr | vm->byte_c[pc+1]) << 8 | (addr | vm->byte_c[pc]);
                pc += 2;
                if (!vm->operand_stack[sp--]) pc = addr;
                break;

            case JMPT: 
                addr = 0;
                addr = (addr | vm->byte_c[pc+1]) << 8 | (addr | vm->byte_c[pc]);
                pc += 2;
                if (vm->operand_stack[sp--]) pc = addr;
                break;

            case LTEQ:
                r = vm->operand_stack[sp--];
                l = vm->operand_stack[sp--];
                vm->operand_stack[++sp] = l <= r;
                break;

            case CALL: 
                addr = 0;
                addr = (addr | vm->byte_c[pc+1]) << 8 | (addr | vm->byte_c[pc]);
                pc += 2;
                argc = vm->byte_c[pc++];
                construct_frame(vm, sp, argc, &vm->exec_stack[++esi], pc);
                sp -= argc;
                pc = addr;
                break;

            case RET:
                pc = vm->exec_stack[esi--].ret_addr;
                break;

            case OUT:
                v = vm->operand_stack[sp--];
                printf("%d\n", v);
                break;

            case END:
                return;

            default:
                printf("Error: Invalid OP_CODE detected in source code.");
                exit(EXIT_FAILURE);
                break;
        }

        instruction = (op_c)vm->byte_c[pc];
    }

    dealloc_vm( vm );
}
