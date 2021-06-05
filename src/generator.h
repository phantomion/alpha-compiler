#ifndef GENERATOR_H
#define GENERATOR_H
#include <stdio.h>
#include "icode.h"
#include "vm_structs.h"


typedef struct incomplete_jump {
    unsigned instrNo;
    unsigned iaddress;
    struct incomplete_jump* next;
}incomplete_jump;

#define CURR_INSTR_SIZE   (total_instr * sizeof(instruction *))
#define NEW_INSTR_SIZE    (EXPAND_SIZE * sizeof(instruction *) + CURR_INSTR_SIZE)

unsigned consts_newstring(char* s);
unsigned consts_newnumber(double n);
unsigned libfuncs_newused(char* s);
unsigned userfuncs_newfunc(symbol* sym);
void make_operand(expr* e, vmarg* arg);

void expand_instr();
void emit_instr(instruction* instr);

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
void write_abc_bin();
void write_abc_text();
#endif
