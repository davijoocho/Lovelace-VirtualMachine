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
    uint32_t offset;
    int32_t ret_a, addr;           // RET_A (RESERVED FOR STRUCT)

    int32_t r32, l32, v32;         // INTEGER
    int64_t r64, l64, v64;         // LONG
    float rf, lf, vf;              // FLOAT
    double rd, ld, vd;             // DOUBLE

    while (1)
    {
        pc++;
        switch (instruction)
        {
            case QUAD_CONST: // QUAD_CONST, 01, 00, 00, 00
                memcpy(&v32, vm->byte_code + pc, 4);
                vm->operand_stack[++sp] = v32;
                pc += 4;
                break;
            case OCTA_CONST: // OCTA_CONST, 00, 00, 00, 00, 00, 00, 00, 00
                memcpy(vm->operand_stack + sp + 1, vm->byte_code + pc, 8);
                pc += 8;
                sp += 2;
                break;
            case QUAD_STORE: // QUAD_STORE, 00
                addr = vm->byte_code[pc++];
                vm->execution_stack->locals[addr] = vm->operand_stack[sp--];
                break;
            case QUAD_LOAD: // QUAD_LOAD, 00
                addr = vm->byte_code[pc++];
                vm->operand_stack[++sp] = vm->execution_stack->locals[addr];
                break;
            case OCTA_STORE: // OCTA_STORE, 00
                addr = vm->byte_code[pc++];
                memcpy(vm->execution_stack->locals + addr, vm->operand_stack + sp - 1, 8);
                sp -= 2;
                break;
            case OCTA_LOAD: // OCTA_LOAD, 00
                addr = vm->byte_code[pc++];
                memcpy(vm->operand_stack + sp + 1, vm->execution_stack->locals + addr, 8);
                sp += 2;
                break;

            case UNI_STORE: // UNI_STORE, 00
                addr = vm->byte_code[pc++];
                vm->execution_stack->locals[addr] = vm->operand_stack[sp--];
                break;
            case UNI_LOAD: // UNI_LOAD, 00
                addr = vm->byte_code[pc++];
                vm->operand_stack[++sp] = vm->execution_stack->locals[addr];
                break;
            case UNI_CONST: // UNI_CONST, 00
                vm->operand_stack[++sp] = vm->byte_code[pc++];
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
            case INEG:
                v32 = vm->operand_stack[sp];
                vm->operand_stack[sp] = -v32;
                break;
            case ICMP:  // ICMP, <OP>
                r32 = vm->operand_stack[sp--];
                l32 = vm->operand_stack[sp];

                switch (vm->byte_code[pc++]) {
                    case EQ: vm->operand_stack[sp] = l32 == r32; break;
                    case GT: vm->operand_stack[sp] = l32 > r32; break;
                    case LT: vm->operand_stack[sp] = l32 < r32; break;
                    case GTEQ: vm->operand_stack[sp] = l32 >= r32; break;
                    case LTEQ: vm->operand_stack[sp] = l32 <= r32; break;
                    default: break;
                }
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
                vm->operand_stack[++sp] = l32 && r32;
                break;
            case OR:
                r32 = vm->operand_stack[sp--];
                l32 = vm->operand_stack[sp--];
                vm->operand_stack[++sp] = l32 || r32;
                break;
            case NOT:
                v32 = vm->operand_stack[sp--];
                vm->operand_stack[++sp] = !v32;
                break;


            // LONG
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
            case LNEG:
                memcpy(&v64, vm->operand_stack + sp - 1, 8);
                v64 = -v64;
                memcpy(vm->operand_stack + sp - 1, &v64, 8);
                break;

            case LCMP:  // LCMP, <OP>
                memcpy(&r64, vm->operand_stack + sp - 1, 8);
                memcpy(&l64, vm->operand_stack + sp - 3, 8);
                sp -= 3;

                switch (vm->byte_code[pc++]) {
                    case EQ: vm->operand_stack[sp] = l64 == r64; break;
                    case GT: vm->operand_stack[sp] = l64 > r64; break;
                    case LT: vm->operand_stack[sp] = l64 < r64; break;
                    case GTEQ: vm->operand_stack[sp] = l64 >= r64; break;
                    case LTEQ: vm->operand_stack[sp] = l64 <= r64; break;
                    default: break;
                }
                break;

            // FLOAT
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
            case FNEG:
                memcpy(&vf, vm->operand_stack + sp, 4);
                vf = -vf;
                memcpy(vm->operand_stack + sp, &vf, 4);
                break;

            case FCMP:  // FCMP, <OP>
                memcpy(&rf, vm->operand_stack + sp--, 4);
                memcpy(&lf, vm->operand_stack + sp, 4);

                switch (vm->byte_code[pc++]) {
                    case EQ: vm->operand_stack[sp] = lf == rf; break;
                    case GT: vm->operand_stack[sp] = lf > rf; break;
                    case LT: vm->operand_stack[sp] = lf < rf; break;
                    case GTEQ: vm->operand_stack[sp] = lf >= rf; break;
                    case LTEQ: vm->operand_stack[sp] = lf <= rf; break;
                    default: break;
                }
                break;


            // DOUBLE
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
            case DNEG:
                memcpy(&vd, vm->operand_stack + sp - 1, 8);
                vd = -vd;
                memcpy(vm->operand_stack + sp - 1, &vd, 8);
                break;

            case DCMP:  // DCMP, <OP>
                memcpy(&rd, vm->operand_stack + sp - 1, 8);
                memcpy(&ld, vm->operand_stack + sp - 3, 8);
                sp -= 3;

                switch (vm->byte_code[pc++]) {
                    case EQ: vm->operand_stack[sp] = ld == rd; break;
                    case GT: vm->operand_stack[sp] = ld > rd; break;
                    case LT: vm->operand_stack[sp] = ld < rd; break;
                    case GTEQ: vm->operand_stack[sp] = ld >= rd; break;
                    case LTEQ: vm->operand_stack[sp] = ld <= rd; break;
                    default: break;
                }
                break;

            case I2L:
            case I2D:
            case I2F:
            case F2D:
            case L2D:
            case L2F:
            /*
            case I2S:
            case L2S:
            case F2S:
            case D2S:
            case B2S:
            */
                break;

            case LOCAL_STRUCT_INIT: {  // LOCAL_STRUCT_INIT, 00, 00, 00, 00 ----> 00, 00 (mem_size)
                memcpy(&addr, vm->byte_code + pc, 4);
                pc += 4;
                ret_a = pc;
                pc += addr;

                uint16_t n_bytes;
                memcpy(&n_bytes, vm->byte_code + pc, 2);

                handle = allocate_mem(&mem, vm->byte_code, n_bytes, -2, 1);
                offset = 0; pc += 2;
                break;
            }

            case INIT_FIELD: {   // INIT_FIELD, 00 (word size)
                uint8_t word_size = vm->byte_code[pc];
                memcpy(handle->heap_ptr + offset, vm->operand_stack + sp - (word_size / 8), (word_size == 10) ? 8 : word_size);
                sp -= word_size / 4;
                offset += word_size;
                pc += 1;
                break;
            }

            case END_INIT:
                memcpy(vm->operand_stack + sp + 1, &handle, 8);
                pc = ret_a;
                sp += 2;
                break;

            case SET_FIELD: {  // SET_FIELD, 00 (word size)
                uint8_t word_size = vm->byte_code[pc++];
                memcpy(&field, vm->operand_stack + sp - (word_size / 8) - 2, 8);
                memcpy(field, vm->operand_stack + sp - (word_size / 8), word_size);
                sp -= (word_size / 8) + 3;
            }

            case SET_REF_FIELD: {    // REF-FIELDS ARE 10 BYTES WIDE - 8 BYTES USED FOR REFERENCE, 2 BYTES USED FOR REM SET POS
                    struct mem_handle* ref; 
                    struct mem_handle* new_handle; 
                    struct y_rem_set* set;
                    uint16_t rem_set_pos; 

                    memcpy(&new_handle, vm->operand_stack + sp - 1, 8);
                    memcpy(&field, vm->operand_stack + sp - 3, 8);

                    memcpy(&rem_set_pos, field + 8, 2);
                    memcpy(&ref, field, 8);

                    set = &mem.rem_sets[ref->gen];

                    if (handle == NULL || new_handle->gen >= handle->gen) {
                        if (rem_set_pos != 0) {
                            set->ptr_set[rem_set_pos - 1] = NULL;

                            if (set->n_empty == set->empty_cap) {
                                set->empty_slots = realloc(set->empty_slots, sizeof(uint16_t) * set->empty_cap * 2);
                                set->empty_cap *= 2;
                            }
                            
                            set->empty_slots[set->n_empty++] = rem_set_pos - 1;
                            memset(field + 8, 0, 2);
                        }
                    } else {
                        if (rem_set_pos == 0) {
                            if (set->n_empty != 0) {
                                uint16_t idx = set->empty_slots[set->n_empty--];
                                set->ptr_set[idx++] = field;
                                memcpy(field + 8, &idx, 2);
                            } else {
                                if (set->pos == set->capacity) {
                                    set->ptr_set = realloc(set->ptr_set, sizeof(int8_t*) * set->capacity * 2);
                                    memset(set->ptr_set + set->capacity, 0, sizeof(int8_t*) * set->capacity);
                                    set->capacity *= 2;
                                }
                                set->ptr_set[set->pos++] = field;
                                memcpy(field + 8, &set->pos, 2);
                            }
                        }
                    }

                    memcpy(field, &handle, 8);
                    sp -= 4;
                }
                break;

            case REF_OFFSETS:   // REF_OFFSETS, 00, ...
                handle->ref_ptr = pc;
                pc += 1;
                break;

            case REF_STORE: {    // REF_STORE, 00
                uint8_t lpos = vm->byte_code[pc++];
                struct stack_frame* context = vm->execution_stack;
                memcpy(&handle, vm->operand_stack + sp - 1, 8);

                if (context->nrefs == 0 || context->refs[context->nrefs - 1] < lpos) {
                    if (context->nrefs == context->capacity) {
                        context->refs = realloc(context->refs, sizeof(uint8_t) * context->capacity * 2);
                        memset(context->refs + context->capacity, 0, sizeof(uint8_t) * context->capacity);
                        context->capacity *= 2;
                    }
                    context->refs[context->nrefs++] = lpos;
                }

                update_root(&mem, handle, context->locals + lpos);
                sp -= 2;
            }
                break;

            case REF_LOAD:  // REF_LOAD, 00
                offset = vm->byte_code[pc++];
                memcpy(vm->operand_stack + sp + 1, vm->execution_stack->locals + offset,  8);
                sp += 2;
                break;

            // R-VALUE
            case GET_FIELD: {  // GET_FIELD, 00 (word size)
                uint8_t word_size = vm->byte_code[pc++];
                offset = vm->operand_stack[sp--];
                memcpy(&handle, --sp + vm->operand_stack, 8);
                v64 = 0;
                memcpy(&v64, handle->heap_ptr + offset, word_size);
                memcpy(vm->operand_stack + sp, &v64, word_size);
                sp += word_size / 8;
                break;
            }

            case NEW_ARRAY: { // NEW_ARRAY, 01, 02  (nlevels, word size)
                uint8_t nlevels = vm->byte_code[pc++];
                uint8_t word_size = vm->byte_code[pc++];
                handle = allocate_arr(&mem, vm->byte_code, word_size, nlevels, vm->operand_stack + sp - nlevels);
                sp -= nlevels;
                memcpy(vm->operand_stack + sp++, &handle, 8);
                break;
            }

            //case ARRAY_LENGTH:

            case NULL_REF:
                v64 = 0;
                memcpy(vm->operand_stack + sp + 1, &v64, 8);
                sp += 2;
                break;

            // L-VALUE
            case OFFSET:       // OFFSET, 00, 00, 00, 00
                memcpy(&v32, vm->byte_code + pc, 4);
                vm->operand_stack[++sp] = v32;
                pc += 4;
                break;

            case GET_OFFSET:
                offset = vm->operand_stack[sp--];
                memcpy(&handle, --sp + vm->operand_stack, 8);
                field = handle->heap_ptr + offset;
                memcpy(vm->operand_stack + sp++, &field, 8);
                break;

            case LOCAL_CALL: {   
                // LOCAL_CALL, 04, 02, 00, 02, 16, 00, 00, 00  // PARAMETER + REFS
                // LOCAL_CALL, 00, <offset>  // NO PARAMETERS
                // LOCAL_CALL, 01, 00, <offset>  // PARAMETERS BUT NO REFS
                uint8_t n32 = vm->byte_code[pc++];

                uint8_t* ref_pos = NULL;
                uint8_t nrefs = 0;

                if (n32 > 0) {
                    nrefs = vm->byte_code[pc++];
                    if (nrefs > 0) {
                        ref_pos = malloc(sizeof(uint8_t) * nrefs);
                        memcpy(ref_pos, vm->byte_code + pc, nrefs);
                        pc += nrefs;
                    }
                }

                memcpy(&addr, vm->byte_code + pc, 4);
                pc += 4;
                init_stack_frame(vm, sp, n32, ref_pos, nrefs, pc, &mem);
                free(ref_pos);
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

            case PRINT:  // TEMPORARY HACK
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



void free_roots(struct stack_frame* context, uint8_t n)
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
