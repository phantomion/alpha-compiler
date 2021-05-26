#ifndef GENERATOR_H
#define GENERATOR_H
#include <stdio.h>
#include "icode.h"

typedef enum vmopcode {
    assign_v, add_v, sub_v,
    mul_v, div_v, mod_v,
    uminus_v, and_v, or_v,
    not_v, jeq_v, jne_v,
    jle_v, jge_v, jlt_v,
    jgt_v, call_v, pusharg_v,
    funcenter_v, funcexit_v, newtable_v,
    tablegetelem_v, tablesetelem_v, nop_v
}vmopcode;

typedef enum vmarg_t {
    label_a,
    global_a,
    formal_a,
    local_a,
    number_a,
    string_a,
    bool_a,
    nil_a,
    userfunc_a,
    libfunc_a,
    retval_a
}vmarg_t;

typedef struct vmarg {
    vmarg_t type;
    unsigned val;
}vmarg;

typedef struct instruction {
    vmopcode opcode;
    vmarg* result;
    vmarg* arg1;
    vmarg* arg2;
    unsigned src_line;
}instruction;

typedef struct user_func {
    unsigned address;
    unsigned localSize;
    char* id;
}user_func;

//#define EXPAND_SIZE 1024
#define CURR_INSTR_SIZE   (total * sizeof(instruction))
#define NEW_INSTR_SIZE    (EXPAND_SIZE * sizeof(instruction) + CURR_INSTR_SIZE)

unsigned consts_newstring(char* s);
unsigned consts_newnumber(double n);
unsigned libfuncs_newused(char* s);
unsigned userfuncs_newfunc(symbol* sym);
void make_operand(expr* e, vmarg* arg);

void expand_instr();
void emit(instruction* instr);

void generate_assign(quad*);
void generate_add(quad*);
void generate_sub(quad*);
void generate_mul(quad*);
void generate_div(quad*);
void generate_mod(quad*);
void generate_uminus(quad*);
void generate_and(quad*);
void generate_or(quad*);
void generate_not(quad*);
void generate_if_eq(quad*);
void generate_if_noteq(quad*);
void generate_if_lesseq(quad*);
void generate_if_greatereq(quad*);
void generate_if_less(quad*);
void generate_if_greater(quad*);
void generate_jump(quad*);
void generate_call(quad*);
void generate_param(quad*);
void generate_ret(quad*);
void generate_getretval(quad*);
void generate_funcstart(quad*);
void generate_funcend(quad*);
void generate_tablecreate(quad*);
void generate_tablegetelem(quad*);
void generate_tablesetelem(quad*);
void generate_all();
#endif
