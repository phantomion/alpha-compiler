#ifndef AVM_H
#define AVM_H
#include "generator.h"
#include <string.h>

#define AVM_STACKSIZE 4096
#define AVM_WIPEOUT(m) memset(&(m), 0, sizeof(m))
#define AVM_TABLE_HASHSIZE 211
#define null NULL
#define AVM_STACKENV_SIZE 4
#define AVM_MAX_INSTRUCTIONS (unsigned) nop_v
#define AVM_ENDING_PC code_size

typedef enum avm_memcell_t {
    number_m,
    string_m,
    bool_m,
    table_m,
    userfunc_m,
    libfunc_m,
    nil_m,
    undef_m
}avm_memcell_t;

typedef struct avm_table avm_table;

typedef struct avm_memcell {
    avm_memcell_t type;
    union {
        double num_val;
        char* str_val;
        unsigned char bool_val;
        avm_table* table_val;
        unsigned func_val;
        char* libfunc_val;
    }data;
}avm_memcell;

typedef struct avm_table_bucket {
    avm_memcell key;
    avm_memcell value;
    struct avm_table_bucket* next;
}avm_table_bucket;

typedef struct avm_table {
    unsigned ref_counter;
    avm_table_bucket* str_indexed[AVM_TABLE_HASHSIZE];
    avm_table_bucket* num_indexed[AVM_TABLE_HASHSIZE];
    unsigned total;
} avm_table;

avm_table* avm_tablenew();
void avm_tabledestroy(avm_table* t);
avm_memcell* avm_tablegetelem(avm_memcell* key);
void avm_tablesetelem(avm_memcell* key, avm_memcell* value);
void avm_memcell_clear(avm_memcell* m);

double consts_getnumber(unsigned index);
char* consts_getstring(unsigned index);
char* libfuncs_getused(unsigned index);

void execute_assign(instruction* instr);
void execute_add(instruction* instr);
void execute_sub(instruction* instr);
void execute_mul(instruction* instr);
void execute_div(instruction* instr);
void execute_mod(instruction* instr);
void execute_uminus(instruction* instr);
void execute_and(instruction* instr);
void execute_or(instruction* instr);
void execute_not(instruction* instr);
void execute_if_eq(instruction* instr);
void execute_if_noteq(instruction* instr);
void execute_if_lesseq(instruction* instr);
void execute_if_greatereq(instruction* instr);
void execute_if_less(instruction* instr);
void execute_if_greater(instruction* instr);
void execute_jump(instruction* instr);
void execute_call(instruction* instr);
void execute_param(instruction* instr);
void execute_ret(instruction* instr);
void execute_getretval(instruction* instr);
void execute_funcstart(instruction* instr);
void execute_funcend(instruction* instr);
void execute_tablecreate(instruction* instr);
void execute_tablegetelem(instruction* instr);
void execute_tablesetelem(instruction* instr);

#endif // !AVM_H
