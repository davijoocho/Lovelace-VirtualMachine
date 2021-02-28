#include <stdio.h>
#include <stdlib.h>
#include "virtual_machine.h"


void execute (struct virt_mach* vm, int main)
{
    int32_t sp = -1;
    int32_t pc = main;

    enum op_code instruction = vm->byte_code[pc];
    int32_t r32, l32, v32, addr;
    int64_t r64, l64, v64;
    float rf, lf, vf;
    double rd, ld, vd;

    while (1)
    {
        pc++;
        switch (instruction)
        {
            // INTEGER 
            case ICONST: // ICONST, 01, 00, 00, 00
                memcpy(&v32, vm->byte_code + pc, 4);
                vm->operand_stack[++sp] = v32;
                pc += 4;
                break;
                
            case IADD:
                r32 = vm->operand_stack[sp--];
                l32 = vm->operand_stack[sp--];
                vm->operand_stack[++sp] = l32 + r32;
                break;

            case ISUB:
                r32 = vm->operand_stack[sp--];
                l32 = vm->operand_stack[sp--];
                vm->operand_stack[++sp] = l32 - r32;
                break;

            case IMUL:
                r32 = vm->operand_stack[sp--];
                l32 = vm->operand_stack[sp--];
                vm->operand_stack[++sp] = l32 * r32;
                break;

            case IDIV:
                r32 = vm->operand_stack[sp--];
                l32 = vm->operand_stack[sp--];
                vm->operand_stack[++sp] = l32 / r32;
                break;

            case IMOD:
                r32 = vm->operand_stack[sp--];
                l32 = vm->operand_stack[sp--];
                vm->operand_stack[++sp] = l32 % r32;
                break;

            case IEQ:
                r32 = vm->operand_stack[sp--];
                l32 = vm->operand_stack[sp--];
                vm->operand_stack[++sp] = (l32 == r32) ? 1 : 0;
                break;

            case INEG:
                v32 = vm->operand_stack[sp];
                vm->operand_stack[sp] = -v32;
                break;

            case ILOAD: // ILOAD, 01
                addr = vm->byte_code[pc++];
                v32 = vm->execution_stack->locals[addr];
                vm->operand_stack[++sp] = v32;
                break;

            case ISTORE: // ISTORE, 01
                addr = vm->byte_code[pc++];
                v32 = vm->operand_stack[sp--];
                vm->execution_stack->locals[addr] = v32;
                break;

            case IGT:
                r32 = vm->operand_stack[sp--];
                l32 = vm->operand_stack[sp--];
                vm->operand_stack[++sp] = (l32 > r32) ? 1 : 0;
                break;

            case ILT:
                r32 = vm->operand_stack[sp--];
                l32 = vm->operand_stack[sp--];
                vm->operand_stack[++sp] = (l32 < r32) ? 1 : 0;
                break;

            case IGTEQ:
                r32 = vm->operand_stack[sp--];
                l32 = vm->operand_stack[sp--];
                vm->operand_stack[++sp] = (l32 >= r32) ? 1 : 0;
                break;

            case ILTEQ:
                r32 = vm->operand_stack[sp--];
                l32 = vm->operand_stack[sp--];
                vm->operand_stack[++sp] = (l32 <= r32) ? 1 : 0;
                break;

            case JMP:  // JMP, 01, 00
                memcpy(&addr, vm->byte_code + pc, 2);
                pc = addr;
                break;

            case JMPF: // JMPF, 01, 00
                memcpy(&addr, vm->byte_code + pc, 2);
                pc += 2;
                if (!vm->operand_stack[sp--]) pc = addr;
                break;

            case JMPT: // JMPT, 01, 00
                memcpy(&addr, vm->byte_code + pc, 2);
                pc += 2;
                if (vm->operand_stack[sp--]) pc = addr;
                break;

            case AND:
                r32 = vm->operand_stack[sp--];
                l32 = vm->operand_stack[sp--];
                vm->operand_stack[++sp] = (l32 == 0) ? l32 : r32;
                break;

            case OR:
                r32 = vm->operand_stack[sp--];
                l32 = vm->operand_stack[sp--];
                vm->operand_stack[++sp] = (l32 == 1) ? l32 : r32;
                break;

            case NOT:
                v32 = vm->operand_stack[sp--];
                vm->operand_stack[++sp] = (v32 == 0) ? 1 : 0;
                break;

            // LONG
            case LCONST: // LCONST, 01, 00, 00, 00, 00, 00, 00, 00
                memcpy(vm->operand_stack + sp + 1, vm->byte_code + pc, 8);
                sp += 2;
                pc += 8;
                break;
                
            case LADD:
                memcpy(&r64, vm->operand_stack + sp - 1, 8);
                memcpy(&l64, vm->operand_stack + sp - 3, 8);
                v64 = l64 + r64;
                memcpy(vm->operand_stack + sp - 3, &v64, 8);
                sp -= 2;
                break;

            case LSUB:
                memcpy(&r64, vm->operand_stack + sp - 1, 8);
                memcpy(&l64, vm->operand_stack + sp - 3, 8);
                v64 = l64 - r64;
                memcpy(vm->operand_stack + sp - 3, &v64, 8);
                sp -= 2;
                break;

            case LMUL:
                memcpy(&r64, vm->operand_stack + sp - 1, 8);
                memcpy(&l64, vm->operand_stack + sp - 3, 8);
                v64 = l64 * r64;
                memcpy(vm->operand_stack + sp - 3, &v64, 8);
                sp -= 2;
                break;

            case LDIV:
                memcpy(&r64, vm->operand_stack + sp - 1, 8);
                memcpy(&l64, vm->operand_stack + sp - 3, 8);
                v64 = l64 / r64;
                memcpy(vm->operand_stack + sp - 3, &v64, 8);
                sp -= 2;
                break;

            case LMOD:
                memcpy(&r64, vm->operand_stack + sp - 1, 8);
                memcpy(&l64, vm->operand_stack + sp - 3, 8);
                v64 = l64 % r64;
                memcpy(vm->operand_stack + sp - 3, &v64, 8);
                sp -= 2;
                break;

            case LEQ:
                memcpy(&r64, vm->operand_stack + sp - 1, 8);
                memcpy(&l64, vm->operand_stack + sp - 3, 8);
                v32 = (l64 == r64) ? 1 : 0;
                memcpy(vm->operand_stack + sp - 3, &v32, 4);
                sp -= 3;
                break;

            case LNEG:
                memcpy(&v64, vm->operand_stack + sp - 1, 8);
                v64 = -v64;
                memcpy(vm->operand_stack + sp - 1, &v64, 8);
                break;

            case LLOAD: // LLOAD, 01
                addr = vm->byte_code[pc++];
                memcpy(vm->operand_stack + sp + 1, vm->execution_stack->locals + addr, 8);
                sp += 2;
                break;

            case LSTORE: // LSTORE, 01
                addr = vm->byte_code[pc++];
                memcpy(vm->execution_stack->locals + addr, vm->operand_stack + sp - 1, 8);
                sp -= 2;
                break;

            case LGT:
                memcpy(&r64, vm->operand_stack + sp - 1, 8);
                memcpy(&l64, vm->operand_stack + sp - 3, 8);
                v32 = (l64 > r64) ? 1 : 0;
                memcpy(vm->operand_stack + sp - 3, &v32, 4);
                sp -= 3;
                break;

            case LLT:
                memcpy(&r64, vm->operand_stack + sp - 1, 8);
                memcpy(&l64, vm->operand_stack + sp - 3, 8);
                v32 = (l64 < r64) ? 1 : 0;
                memcpy(vm->operand_stack + sp - 3, &v32, 4);
                sp -= 3;
                break;

            case LGTEQ:
                memcpy(&r64, vm->operand_stack + sp - 1, 8);
                memcpy(&l64, vm->operand_stack + sp - 3, 8);
                v32 = (l64 >= r64) ? 1 : 0;
                memcpy(vm->operand_stack + sp - 3, &v32, 4);
                sp -= 3;
                break;

            case LLTEQ:
                memcpy(&r64, vm->operand_stack + sp - 1, 8);
                memcpy(&l64, vm->operand_stack + sp - 3, 8);
                v32 = (l64 <= r64) ? 1 : 0;
                memcpy(vm->operand_stack + sp - 3, &v32, 4);
                sp -= 3;
                break;

            // FLOAT - COPY THE MEMORY LAYOUT, DON'T CAST TO INT32_T
            case FCONST: // FCONST, 01, 00, 00, 00
                memcpy(&v32, vm->byte_code + pc, 4);
                vm->operand_stack[++sp] = v32;
                pc += 4;
                break;

            case FADD:
                memcpy(&rf, vm->operand_stack + sp, 4);
                memcpy(&lf, vm->operand_stack + sp - 1, 4);
                vf = lf + rf;
                memcpy(vm->operand_stack + sp - 1, &vf, 4);
                sp -= 1;
                break;

            case FSUB:
                memcpy(&rf, vm->operand_stack + sp, 4);
                memcpy(&lf, vm->operand_stack + sp - 1, 4);
                vf = lf - rf;
                memcpy(vm->operand_stack + sp - 1, &vf, 4);
                sp -= 1;
                break;

            case FMUL:
                memcpy(&rf, vm->operand_stack + sp, 4);
                memcpy(&lf, vm->operand_stack + sp - 1, 4);
                vf = lf * rf;
                memcpy(vm->operand_stack + sp - 1, &vf, 4);
                sp -= 1;
                break;

            case FDIV:
                memcpy(&rf, vm->operand_stack + sp, 4);
                memcpy(&lf, vm->operand_stack + sp - 1, 4);
                vf = lf / rf;
                memcpy(vm->operand_stack + sp - 1, &vf, 4);
                sp -= 1;
                break;

            case FEQ:
                memcpy(&rf, vm->operand_stack + sp, 4);
                memcpy(&lf, vm->operand_stack + sp - 1, 4);
                vm->operand_stack[sp - 1] = (lf == rf) ? 1 : 0;
                sp -= 1;
                break;

            case FNEG:
                memcpy(&vf, vm->operand_stack + sp, 4);
                vf = -vf;
                memcpy(vm->operand_stack + sp, &vf, 4);
                break;

            case FLOAD:  // FLOAD, 00
                addr = vm->byte_code[pc++];
                memcpy(vm->operand_stack + sp + 1, vm->execution_stack->locals + addr, 4);
                break;

            case FSTORE: // FSTORE, 00
                addr = vm->byte_code[pc++];
                memcpy(vm->execution_stack->locals + addr, vm->operand_stack + sp, 4);
                sp -= 1;
                break;

            case FGT:
                memcpy(&rf, vm->operand_stack + sp, 4);
                memcpy(&lf, vm->operand_stack + sp - 1, 4);
                vm->operand_stack[sp - 1] = (lf > rf) ? 1 : 0;
                sp -= 1;
                break;

            case FLT:
                memcpy(&rf, vm->operand_stack + sp, 4);
                memcpy(&lf, vm->operand_stack + sp - 1, 4);
                vm->operand_stack[sp - 1] = (lf < rf) ? 1 : 0;
                sp -= 1;
                break;

            case FGTEQ:
                memcpy(&rf, vm->operand_stack + sp, 4);
                memcpy(&lf, vm->operand_stack + sp - 1, 4);
                vm->operand_stack[sp - 1] = (lf >= rf) ? 1 : 0;
                sp -= 1;
                break;

            case FLTEQ:
                memcpy(&rf, vm->operand_stack + sp, 4);
                memcpy(&lf, vm->operand_stack + sp - 1, 4);
                vm->operand_stack[sp - 1] = (lf <= rf) ? 1 : 0;
                sp -= 1;
                break;

            // DOUBLE

            case DCONST: // DCONST, 00, 00, 00, 00, 00, 00, 00, 00
                memcpy(vm->operand_stack + sp + 1, vm->byte_code + pc, 8);
                pc += 8;
                sp += 2;
                break;
                
            case DADD:
                memcpy(&rd, vm->operand_stack + sp - 1, 8);
                memcpy(&ld, vm->operand_stack + sp - 3, 8);
                vd = ld + rd;
                memcpy(vm->operand_stack + sp - 3, &vd, 8);
                sp -= 2;
                break;

            case DSUB:
                memcpy(&rd, vm->operand_stack + sp - 1, 8);
                memcpy(&ld, vm->operand_stack + sp - 3, 8);
                vd = ld - rd;
                memcpy(vm->operand_stack + sp - 3, &vd, 8);
                sp -= 2;
                break;

            case DMUL:
                memcpy(&rd, vm->operand_stack + sp - 1, 8);
                memcpy(&ld, vm->operand_stack + sp - 3, 8);
                vd = ld * rd;
                memcpy(vm->operand_stack + sp - 3, &vd, 8);
                sp -= 2;
                break;

            case DDIV:
                memcpy(&rd, vm->operand_stack + sp - 1, 8);
                memcpy(&ld, vm->operand_stack + sp - 3, 8);
                vd = ld / rd;
                memcpy(vm->operand_stack + sp - 3, &vd, 8);
                sp -= 2;
                break;

            case DEQ:
                memcpy(&rd, vm->operand_stack + sp - 1, 8);
                memcpy(&ld, vm->operand_stack + sp - 3, 8);
                v32 = (ld == rd) ? 1 : 0;
                memcpy(vm->operand_stack + sp - 3, &v32, 4);
                sp -= 3;
                break;

            case DNEG:
                memcpy(&vd, vm->operand_stack + sp - 1, 8);
                vd = -vd;
                memcpy(vm->operand_stack + sp - 1, &vd, 8);
                break;

            case DLOAD:  // DLOAD, 00
                addr = vm->byte_code[pc++];
                memcpy(vm->operand_stack + sp + 1, vm->execution_stack->locals + addr, 8);
                sp += 2;
                break;

            case DSTORE: // DSTORE, 00
                addr = vm->byte_code[pc++];
                memcpy(vm->execution_stack->locals + addr, vm->operand_stack + sp - 1, 8);
                sp -= 2;
                break;

            case DGT:
                memcpy(&rd, vm->operand_stack + sp - 1, 8);
                memcpy(&ld, vm->operand_stack + sp - 3, 8);
                v32 = (ld > rd) ? 1 : 0;
                memcpy(vm->operand_stack + sp - 3, &v32, 4);
                sp -= 3;
                break;

            case DLT:
                memcpy(&rd, vm->operand_stack + sp - 1, 8);
                memcpy(&ld, vm->operand_stack + sp - 3, 8);
                v32 = (ld < rd) ? 1 : 0;
                memcpy(vm->operand_stack + sp - 3, &v32, 4);
                sp -= 3;
                break;

            case DGTEQ:
                memcpy(&rd, vm->operand_stack + sp - 1, 8);
                memcpy(&ld, vm->operand_stack + sp - 3, 8);
                v32 = (ld >= rd) ? 1 : 0;
                memcpy(vm->operand_stack + sp - 3, &v32, 4);
                sp -= 3;
                break;

            case DLTEQ:
                memcpy(&rd, vm->operand_stack + sp - 1, 8);
                memcpy(&ld, vm->operand_stack + sp - 3, 8);
                v32 = (ld <= rd) ? 1 : 0;
                memcpy(vm->operand_stack + sp - 3, &v32, 4);
                sp -= 3;
                break;

            case BSTORE:  // BSTORE, 00
                addr = vm->byte_code[pc++];
                vm->execution_stack->locals[addr] = vm->operand_stack[sp--];
                break;

            case BLOAD:   // BLOAD, 00
                addr = vm->byte_code[pc++];
                vm->operand_stack[++sp] = vm->execution_stack->locals[addr];
                break;

            case BCONST:  // BCONST, 01
                v32 = 0;
                memcpy(&v32, vm->byte_code + pc, 1);
                vm->operand_stack[++sp] = v32;
                pc += 1;
                break;

            case CSTORE:
                addr = vm->byte_code[pc++];
                vm->execution_stack->locals[addr] = vm->operand_stack[sp--];
                break;

            case CLOAD:
                addr = vm->byte_code[pc++];
                vm->operand_stack[++sp] = vm->execution_stack->locals[addr];
                break;

            case CCONST:  // CCONST, 01
                v32 = 0;
                memcpy(&v32, vm->byte_code + pc, 1);
                vm->operand_stack[++sp] = v32;
                pc += 1;
                break;
           

            // IMPLEMENT
            case I2L:
            case I2D:
            case I2F:
            case F2D:
            case L2D:
            case L2F:
                break;

            case LOCAL_STRUCT_INIT:  // LOCAL_STRUCT_INIT, 00, 00
            case MODULE_STRUCT_INIT: // MODULE_STRUCT_INIT, STRING_LENGTH, <STRING - STRUCT_NAME>
            case ISETFIELD:
            case LSETFIELD:
            case FSETFIELD:
            case DSETFIELD:
            case STRUCT_SETFIELD:
            case ARRAY_SETFIELD:
            case STRUCT_STORE:
            case STRUCT_LOAD:
            case REF_OFFSETS:
                break;

            case INEWARRAY:
            case IARRAY_INDEX:
            case IARRAY_STORE:
            case LNEWARRAY:
            case LARRAY_INDEX:
            case LARRAY_STORE:
            case FNEWARRAY:
            case FARRAY_INDEX:
            case FARRAY_STORE:
            case DNEWARRAY:
            case DARRAY_INDEX:
            case DARRAY_STORE:
            case SNEWARRAY:
            case SARRAY_INDEX:
            case SARRAY_STORE:
            case ARRAY_LOAD:
            case ARRAY_STORE:
                break;

            case NULL_REF:
            case MODULE_CALL: // MODULE_CALL, <STRING - FUNCTION_NAME>
                break;

            case LOCAL_CALL: // LOCAL_CALL, 04, 01, 00 - size of total args in 32bits (04) , offset from cur_pos (01, 00)
                // also have to deal with potential reference parameters.
                v32 = vm->byte_code[pc++];
                memcpy(&addr, vm->byte_code + pc, 2);
                pc += 2;
                init_stack_frame(vm, sp, v32, pc);
                sp -= v32;
                pc += addr;
                break;

            case SYS_CALL:
                break;

            case RETURN:
                pc = vm->execution_stack->ret_addr;
                // dealloc_frame here.
                // pop off nrefs of refs from roots.
                break;

            case END:
                return;

            default:
                printf("Error: Invalid OP_CODE detected in source code.");
                exit(EXIT_FAILURE);
                break;
        }

        instruction = vm->byte_code[pc];
    }

    free(vm->execution_stack);
    free(vm->byte_code);
    free(vm);
}


void init_stack_frame (struct virt_mach* vm, int sp, int n32, int ret_index) { 
    struct stack_frame* context = malloc(sizeof(struct stack_frame));
    context->ret_addr = ret_index;
    context->nrefs = 0;
    memcpy(context->locals, vm->operand_stack + sp - n32 + 1, n32 * 4);
    context->prev_frame = vm->execution_stack;
    vm->execution_stack = context;
}
