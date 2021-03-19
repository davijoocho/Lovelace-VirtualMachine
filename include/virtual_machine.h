#ifndef VIRTUAL_MACHINE_H
#define VIRTUAL_MACHINE_H

#include <stdint.h>
#include <string.h>
#include "garbage_collect.h"

#define MAX_OPERAND_STACK 255
#define MAX_EXECUTION_DEPTH 4096
#define MAX_LOCAL 255

enum op_code
{
    // INTEGER
    ICONST = 0x01,
    IADD = 0x02,
    ISUB = 0x03,
    IMUL = 0x04,
    IDIV = 0x05,
    IMOD = 0x06,
    IEQ = 0x07,   // RESULT: 0 or 1
    INEG = 0x08,
    ILOAD = 0x09,
    ISTORE = 0x0A,
    IGT = 0x0B,   // RESULT: 0 or 1
    ILT = 0x0C,   // RESULT: 0 or 1 
    IGTEQ = 0x0D, // RESULT: 0 or 1
    ILTEQ = 0x0E, // RESULT: 0 or 1

    // CONDITIONAL (USE BITWISE OPERATOR)
    JMP = 0x0F,
    JMPF = 0x10,
    JMPT = 0x11,
    OR = 0x12,
    AND = 0x13,
    NOT = 0x14,

    // LONG
    LCONST = 0x15,
    LADD = 0x16,
    LSUB = 0x17,
    LMUL = 0x18,
    LDIV = 0x19,
    LMOD = 0x1A,
    LEQ = 0x1B,   // RESULT: 0 or 1
    LNEG = 0x1C,
    LLOAD = 0x1D,
    LSTORE = 0x1E,
    LGT = 0x1F,   // RESULT: 0 or 1
    LLT = 0x20,   // RESULT: 0 or 1 
    LGTEQ = 0x21, // RESULT: 0 or 1
    LLTEQ = 0x22, // RESULT: 0 or 1

    // FLOAT
    FCONST = 0x23,
    FADD = 0x24,
    FSUB = 0x25,
    FMUL = 0x26,
    FDIV = 0x27,
    FEQ = 0x28,   // RESULT: 0 or 1
    FNEG = 0x29,
    FLOAD = 0x2A,
    FSTORE = 0x2B,
    FGT = 0x2C,   // RESULT: 0 or 1
    FLT = 0x2D,   // RESULT: 0 or 1 
    FGTEQ = 0x2E, // RESULT: 0 or 1
    FLTEQ = 0x2F, // RESULT: 0 or 1

    // DOUBLE
    DCONST = 0x30,
    DADD = 0x31,
    DSUB = 0x32,
    DMUL = 0x33,
    DDIV = 0x34,
    DEQ = 0x35,   // RESULT: 0 or 1
    DNEG = 0x36,
    DLOAD = 0x37,
    DSTORE = 0x38,
    DGT = 0x39,   // RESULT: 0 or 1
    DLT = 0x3A,   // RESULT: 0 or 1 
    DGTEQ = 0x3B, // RESULT: 0 or 1
    DLTEQ = 0x3C, // RESULT: 0 or 1

    // BOOLEAN
    BSTORE = 0x3D,
    BLOAD = 0x3E,
    BCONST = 0x3F,

    // CHAR
    CSTORE = 0x40,
    CLOAD = 0x41,
    CCONST = 0x42,

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
    IINIT_FIELD = 0x53,
    LINIT_FIELD = 0x54,
    FINIT_FIELD = 0x55,
    DINIT_FIELD = 0x56,
    CINIT_FIELD = 0x57,
    BINIT_FIELD = 0x58,
    STRUCT_INIT_FIELD = 0x59,
    ARRAY_INIT_FIELD = 0x5A,

    STRUCT_STORE = 0x5B,
    STRUCT_LOAD = 0x5C,
    REF_OFFSETS = 0x5D,

    // R-VALUE
    IGETFIELD = 0x5E,
    LGETFIELD = 0x5F,
    FGETFIELD = 0x60,
    DGETFIELD = 0x61,
    CGETFIELD = 0x62,
    BGETFIELD = 0x63,
    STRUCT_GETFIELD = 0x64,
    ARRAY_GETFIELD = 0x65,

    // L-VALUE
    ISETFIELD = 0x66,
    LSETFIELD = 0x67,
    FSETFIELD = 0x68,
    DSETFIELD = 0x69,

    // ARRAYS
    INEWARRAY = 0x6A,
    IARRAY_INDEX = 0x6B,
    IARRAY_STORE = 0x6C,

    LNEWARRAY = 0x6D,
    LARRAY_INDEX = 0x6E,
    LARRAY_STORE = 0x6F,

    FNEWARRAY = 0x70,
    FARRAY_INDEX = 0x71,
    FARRAY_STORE = 0x72,

    DNEWARRAY = 0x73,
    DARRAY_INDEX = 0x74,
    DARRAY_STORE = 0x75,

    BNEWARRAY = 0x76,
    BARRAY_INDEX = 0x77,
    BARRAY_STORE = 0x78,

    CNEWARRAY = 0x79,
    CARRAY_INDEX = 0x7A,
    CARRAY_STORE = 0x7B,

    SNEWARRAY = 0x7C,
    SARRAY_INDEX = 0x7D,
    SARRAY_STORE = 0x7E,

    AARRAY_INDEX = 0x7F,
    AARRAY_STORE = 0x80,

    ARRAY_LOAD = 0x81,     // Local
    ARRAY_STORE = 0x82,

    NULL_REF = 0x83,

    // FUNCTION
    LOCAL_CALL = 0x84,
    MODULE_CALL = 0x85,

    SYS_CALL = 0x86,
    REMOVE_REF = 0x87,
    RETURN = 0x88,
    END = 0x89,

    GET_OFFSET = 0x8A,
    OFFSET = 0x8B,

    // EXTRA
    CSETFIELD = 0x8C,
    BSETFIELD = 0x8D,
    STRUCT_SET_FIELD = 0x8E,
    ARRAY_SET_FIELD = 0x8F

    //STRING_CONCAT -> LIKE PYTHON?
};

struct stack_frame
{
    int8_t capacity;
    int8_t nrefs;
    uint8_t* refs;               // dynamic array of indexes into locals
    uint32_t ret_addr;
    int32_t locals[MAX_LOCAL];   // refs are int128_t: 1st 64: pointer to mem_handle | 2nd 64: pointer to roots
    struct stack_frame* prev_frame;
};

struct virt_mach
{
    int8_t* byte_code;
    int32_t operand_stack[MAX_OPERAND_STACK];
    struct stack_frame* execution_stack;
};


void execute (struct virt_mach* vm, int main);
void init_stack_frame (struct virt_mach* vm, int sp, int8_t n32, int8_t* ref_pos, int8_t n_refs, int32_t ret_a, struct mem_info* mem);
void free_roots (struct stack_frame* context, int8_t n);


#endif

