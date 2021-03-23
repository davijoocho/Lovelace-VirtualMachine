#ifndef VIRTUAL_MACHINE_H
#define VIRTUAL_MACHINE_H

#include <stdint.h>
#include <string.h>
#include "garbage_collect.h"

#define MAX_OPERAND_STACK 255
#define MAX_EXECUTION_DEPTH 4096
#define MAX_LOCAL 255

// definition location

enum op_code
{
    // INTEGER
    IADD = 0x02,
    ISUB = 0x03,
    IMUL = 0x04,
    IDIV = 0x05,
    IMOD = 0x06,
    INEG = 0x08,
    ICMP = 0x09,

    // CMP
    EQ = 0x0A,
    LT = 0x0B,
    GT = 0x0C,
    GTEQ = 0x0D,
    LTEQ = 0x0E,

    // CONDITIONAL (USE BITWISE OPERATOR)
    JMP = 0x0F,
    JMPF = 0x10,
    JMPT = 0x11,
    OR = 0x12,
    AND = 0x13,
    NOT = 0x14,

    // LONG
    LADD = 0x16,
    LSUB = 0x17,
    LMUL = 0x18,
    LDIV = 0x19,
    LMOD = 0x1A,
    LNEG = 0x1C,
    LCMP = 0x1D,

    // FLOAT
    FADD = 0x24,
    FSUB = 0x25,
    FMUL = 0x26,
    FDIV = 0x27,
    FNEG = 0x29,
    FCMP = 0x2A,

    // DOUBLE
    DADD = 0x31,
    DSUB = 0x32,
    DMUL = 0x33,
    DDIV = 0x34,
    DNEG = 0x36,
    DCMP = 0x37,

    // CONSTANTS
    UNI_CONST = 0x38,
    QUAD_CONST = 0x39,
    OCTA_CONST = 0x3A,

    UNI_STORE = 0x3B,
    QUAD_STORE = 0x3C,
    OCTA_STORE = 0x3D,

    UNI_LOAD = 0x3E,
    QUAD_LOAD = 0x3F,
    OCTA_LOAD = 0x40,

    // CONVERSIONS
    I2L = 0x43,
    I2D = 0x44,
    I2F = 0x45,
    F2D = 0x46,
    F2I = 0x47,
    F2L = 0x48,
    L2D = 0x49,
    L2I = 0x4A,
    L2F = 0x4B,
    I2S = 0x4C,
    L2S = 0x4D,
    F2S = 0x4E,
    D2S = 0x4F,
    B2S = 0x50,

    // STRUCT INIT
    LOCAL_STRUCT_INIT = 0x51,
    MODULE_STRUCT_INIT = 0x52,
    REF_OFFSETS = 0x5D,
    END_INIT = 0x5E,
    INIT_FIELD = 0x5F,

    // R-VALUE
    GET_FIELD = 0x62,

    // L-VALUE
    SET_FIELD = 0x63,
    SET_REF_FIELD = 0x64,

    // ARRAY
    NULL_REF = 0x83,

    REF_LOAD = 0x90,
    REF_STORE = 0x91,

    NEW_ARRAY = 0x92,
    ARRAY_LENGTH = 0x93,

    // FUNCTION
    LOCAL_CALL = 0x84,
    MODULE_CALL = 0x85,
    PRINT = 0x98,

    SYS_CALL = 0x86,
    REMOVE_REF = 0x87,
    RETURN = 0x88,
    END = 0x89,

    OFFSET = 0x8A,
    GET_OFFSET = 0x8B
};

struct stack_frame
{
    uint8_t capacity;
    uint8_t nrefs;
    uint8_t* refs;               // dynamic array of indexes into locals
    uint32_t ret_addr;
    int32_t locals[MAX_LOCAL];   // refs are int128_t: 1st 64: pointer to mem_handle | 2nd 64: pointer to roots
    struct stack_frame* prev_frame;
};

struct virt_mach
{
    uint8_t* byte_code;
    int32_t operand_stack[MAX_OPERAND_STACK];
    struct stack_frame* execution_stack;
};


void execute (struct virt_mach* vm, int main);
void init_stack_frame (struct virt_mach* vm, int sp, uint8_t n32, uint8_t* ref_pos, uint8_t n_refs, int32_t ret_a, struct mem_info* mem);
void free_roots (struct stack_frame* context, uint8_t n);

#endif

