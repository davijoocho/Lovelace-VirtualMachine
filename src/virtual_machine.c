#include <stdio.h>
#include <stdlib.h>
#include "garbage_collect.h"
#include "virtual_machine.h"


void execute (struct virt_mach* vm, int main)
{
    struct mem_info mem;
    init_mem_info(&mem);

    int32_t sp = -1;
    int32_t pc = main;

    enum op_code instruction = vm->byte_code[pc];

    struct mem_handle* handle;
    int8_t* field;
    uint16_t n_init, offset;        // STRUCT
    int32_t ret_a, addr;     // RET_A (RESERVED FOR STRUCT)
    int32_t r32, l32, v32;   // INTEGER
    int64_t r64, l64, v64;   // LONG
    float rf, lf, vf;        // FLOAT
    double rd, ld, vd;       // DOUBLE

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


            case CSTORE:
            case BSTORE:  // BSTORE, 00 || CSTORE, 00
                addr = vm->byte_code[pc++];
                vm->execution_stack->locals[addr] = vm->operand_stack[sp--];
                break;

            case CLOAD:
            case BLOAD:   // BLOAD, 00 || CLOAD, 00
                addr = vm->byte_code[pc++];
                vm->operand_stack[++sp] = vm->execution_stack->locals[addr];
                break;

            case CCONST:
            case BCONST:  // BCONST, 01 || CCONST, 01
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
            case I2S:
            case L2S:
            case F2S:
            case D2S:
            case B2S:
                break;

            case LOCAL_STRUCT_INIT: {  // LOCAL_STRUCT_INIT, 00, 00, 00, 00 ----> 00, 00, 00, 00 (mem_size, instruction_size)
                memcpy(&addr, vm->byte_code + pc, 4);
                pc += 4;
                ret_a = pc;
                pc += addr;

                uint16_t n_bytes;
                memcpy(&n_bytes, vm->byte_code + pc, 2);
                memcpy(&n_init, vm->byte_code + pc + 2, 2);

                handle = allocate_mem(&mem, vm->byte_code, n_bytes, -2);
                offset = 0; pc += 4;
                break;
            }

            case FSETFIELD:
            case ISETFIELD:
                if (n_init > 0) {
                    memcpy(handle->heap_ptr + offset, vm->operand_stack + sp--, 4);
                    offset += 4;
                    if (n_init == 1) {
                        memcpy(vm->operand_stack + sp + 1, &handle, 8);
                        pc = ret_a; sp += 2;
                    }
                    n_init -= 1;
                } else {
                    memcpy(&field, vm->operand_stack + sp - 2, 8);
                    memcpy(field, vm->operand_stack + sp, 4);
                    sp -= 3;
                }
                break;

            case LSETFIELD:
            case DSETFIELD:
                if (n_init > 0) {
                    memcpy(handle->heap_ptr + offset, vm->operand_stack + sp - 1, 8);
                    offset += 8; sp -= 2;
                    if (n_init == 1) {
                        memcpy(vm->operand_stack + sp + 1, &handle, 8);
                        pc = ret_a; sp += 2;
                    }
                    n_init -= 1;
                } else {
                    memcpy(&field, vm->operand_stack + sp - 3, 8);
                    memcpy(field, vm->operand_stack + sp - 1, 8);
                    sp -= 4;
                }
                break;

            case BSETFIELD:
            case CSETFIELD:
                if (n_init > 0) {
                    memcpy(handle->heap_ptr + offset, vm->operand_stack + sp--, 1);
                    offset += 1;
                    if (n_init == 1) {
                        memcpy(vm->operand_stack + sp + 1, &handle, 8);
                        pc = ret_a; sp += 2;
                    }
                    n_init -= 1;
                } else {
                    memcpy(&field, vm->operand_stack + sp - 2, 8);
                    memcpy(field, vm->operand_stack + sp, 1);
                    sp -= 3;
                }
                break;

            case STRUCT_SET_FIELD:   // STRUCT_SET_FIELD (INIT) || STRUCT_SET_FIELD, 00, 00 (MUTATE)
            case ARRAY_SET_FIELD:    // REF-FIELDS ARE 10 BYTES WIDE - 8 BYTES USED FOR REFERENCE, 2 BYTES USED FOR REM SET POS
                if (n_init > 0) {
                    memcpy(handle->heap_ptr + offset, vm->operand_stack + sp - 1, 8);
                    memset(handle->heap_ptr + offset + 8, 0, 2);
                    offset += 10; sp -= 2;
                    if (n_init == 1) {
                        memcpy(vm->operand_stack + sp + 1, &handle, 8);
                        pc = ret_a; sp += 2;
                    }
                    n_init -= 1;
                } else {
                    struct mem_handle* ref; 
                    struct mem_handle* prev_handle; 
                    struct y_rem_set* set;
                    uint16_t rem_set_pos; 

                    memcpy(&offset, vm->byte_code + pc, 2);            // OFFSET OF PREV_HNDL REF FIELD
                    memcpy(&handle, vm->operand_stack + sp - 1, 8);    // NEW_HNDL
                    memcpy(&prev_handle, vm->operand_stack + sp - 3, 8);

                    memcpy(&rem_set_pos, prev_handle->heap_ptr + offset + 8, 2);
                    memcpy(&ref, prev_handle->heap_ptr + offset, 8);

                    set = &mem.rem_sets[ref->gen];

                    if (handle == NULL || handle->gen >= prev_handle->gen) {
                        if (rem_set_pos != 0) {
                            set->ptr_set[rem_set_pos - 1] = NULL;

                            if (set->n_empty == set->empty_cap) {
                                set->empty_slots = realloc(set->empty_slots, sizeof(uint16_t) * set->empty_cap * 2);
                                set->empty_cap *= 2;
                            }
                            
                            set->empty_slots[set->n_empty++] = rem_set_pos - 1;
                            memset(prev_handle->heap_ptr + offset + 8, 0, 2);
                        }
                    } else {
                        if (rem_set_pos == 0) {
                            if (set->n_empty != 0) {
                                uint16_t idx = set->empty_slots[set->n_empty--];
                                set->ptr_set[idx++] = prev_handle->heap_ptr + offset;
                                memcpy(prev_handle->heap_ptr + offset + 8, &idx, 2);
                            } else {
                                if (set->pos == set->capacity) {
                                    set->ptr_set = realloc(set->ptr_set, sizeof(int8_t*) * set->capacity * 2);
                                    memset(set->ptr_set + set->capacity, 0, sizeof(int8_t*) * set->capacity);
                                    set->capacity *= 2;
                                }
                                set->ptr_set[set->pos++] = prev_handle->heap_ptr + offset;
                                memcpy(prev_handle->heap_ptr + offset + 8, &set->pos, 2);
                            }
                        }
                    }

                    memcpy(prev_handle->heap_ptr + offset, &handle, 8);
                    sp -= 4;
                    pc += 2;
                }
                break;


            case REF_OFFSETS:
                handle->ref_ptr = pc;
                memcpy(vm->operand_stack + sp + 1, &handle, 8);
                sp += 2; n_init -= 1;
                pc = ret_a;
                break;

            case STRUCT_STORE: // STRUCT_STORE, 00, 00
            case STRUCT_LOAD:
                // update_root
           
            case IGETFIELD: // IGETFIELD, 00, 00 <offset in object>
            case LGETFIELD:
            case FGETFIELD:
            case DGETFIELD:
            case CGETFIELD:
            case BGETFIELD:
            case STRUCT_GETFIELD:
            case ARRAY_GETFIELD:


            case MODULE_STRUCT_INIT: // MODULE_STRUCT_INIT, STRING_LENGTH, <STRING - STRUCT_NAME>
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

            case CNEWARRAY:
            case BNEWARRAY:
            case CARRAY_STORE:
            case BARRAY_STORE:
            case CARRAY_INDEX:
            case BARRAY_INDEX:

            case AARRAY_INDEX:
            case AARRAY_STORE:
            case SNEWARRAY:
            case SARRAY_INDEX:
            case SARRAY_STORE:
                // rem_set


            case ARRAY_LOAD:
            case ARRAY_STORE:
                // update_root

            case NULL_REF:
                v64 = 0;
                memcpy(vm->operand_stack + sp + 1, &v64, 8);
                sp += 2;
                break;
            case GET_OFFSET:
                break;

                /*
            case MODULE_CALL: {  // MODULE_CALL, 04, 02, 00, 02, 09, <STRING - FUNCTION_NAME> => nargs32 (04),  contains_ref (2 n_refs), ref_pos (02, 00), string_length (09)
                // GET N_ARGS_32_BITS 
                int8_t n32 = vm->byte_code[pc++];
                // GET_N_REFS
                int8_t n_refs = vm->byte_code[pc++];

                // GET N_REF_POS_8_BITS 
                int8_t ref_pos[n_refs];
                memcpy(ref_pos, vm->byte_code + pc, n_refs);
                pc += n_refs;

                // GET_FUNCTION_NAME
                int8_t len = vm->byte_code[pc++];
                char function_name[len];
                memcpy(function_name, vm->byte_code + pc, len);
                pc += len;

                // INITIALIZE STACK FRAME
                init_stack_frame(vm, sp, n32, ref_pos, n_refs, pc);
                sp -= n32;

                // REDIRECT PROGRAM COUNTER
                // addr = find_function_pos();
                pc += addr;
                break;
            } */

            case LOCAL_CALL: {   // LOCAL_CALL, 04, 02, 00, 02, 16, 00, 00, 00  - offset from cur position  (16, 00, 00, 00, <here>)
                uint8_t n32 = vm->byte_code[pc++];
                uint8_t n_refs = vm->byte_code[pc++];

                uint8_t ref_pos[n_refs];
                memcpy(ref_pos, vm->byte_code + pc, n_refs);
                pc += n_refs;

                memcpy(&addr, vm->byte_code + pc, 4);
                pc += 4;

                init_stack_frame(vm, sp, n32, ref_pos, n_refs, pc, &mem);
                sp -= n32;

                pc += addr;
                break;
            }

            case REMOVE_REF: // REMOVE_REF, 01   
                free_roots(vm->execution_stack, vm->byte_code[pc++]);
                break;

            case RETURN: {
                pc = vm->execution_stack->ret_addr;
                free_roots(vm->execution_stack, vm->execution_stack->nrefs);
                free(vm->execution_stack->refs);
                struct stack_frame* prev_frame = vm->execution_stack->prev_frame;
                free(vm->execution_stack);
                vm->execution_stack = prev_frame;
                break;
            }

            case SYS_CALL:
                break;

            case END:
                printf("HEAP MEMORY\n");

                for (int i = 0; i < HEAP_SIZE; i++) {
                    if (i % 100 == 0 && i != 0) {
                        printf("\n");
                    }
                    printf("%d", mem.heap[i]);
                }
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



void init_stack_frame (struct virt_mach* vm, int sp, uint8_t n32, uint8_t* ref_pos, uint8_t n_refs, int32_t ret_a, struct mem_info* mem)
{
    struct stack_frame* context = malloc(sizeof(struct stack_frame));
    memset(context->locals, 0, MAX_LOCAL * 4);
    context->capacity = 16;
    context->refs = malloc(sizeof(uint8_t) * 16);
    context->nrefs = n_refs;
    context->ret_addr = ret_a;

    int32_t* op_stack = vm->operand_stack + sp - n32 + 1;
    uint8_t ref_i = 0;
    uint8_t op_i = 0;
    uint8_t loc_i = 0;

    while (op_i < n32) {
        if (ref_i < n_refs && op_i == ref_pos[ref_i]) {
            struct mem_handle* handle;
            memcpy(&handle, op_stack + op_i, 8);
            update_root(mem, handle, context->locals + loc_i);

            context->refs[context->nrefs++] = loc_i;
            op_i += 2; ref_i += 1; loc_i += 4;
        } else {
            context->locals[loc_i++] = op_stack[op_i++];
        }
    }

    context->prev_frame = vm->execution_stack;
    vm->execution_stack = context;
}


void free_roots(struct stack_frame* context, int8_t n)
{
    struct mem_root* root;
    int32_t* local;

    while (n > 0) {
        local = context->locals + context->refs[--context->nrefs];
        memcpy(&root, local + 2, 8);
        memset(local, 0, 16);

        root->next->prev = root->prev;
        root->prev->next = root->next;
        free(root);

        n -= 1;
    }
}
