#ifndef AVM_H
#define AVM_H
#include "../generator.h"
#include <string.h>

#define AVM_STACKSIZE 4096
#define AVM_WIPEOUT(m) memset(&(m), 0, sizeof(m))
#define AVM_TABLE_HASHSIZE 211
#define null NULL
#define AVM_STACKENV_SIZE 4
#define AVM_MAX_INSTRUCTIONS (unsigned) nop_v
#define AVM_ENDING_PC code_size
#define AVM_NUMACTUALS_OFFSET   +4
#define AVM_SAVEDPC_OFFSET      +3
#define AVM_SAVEDTOP_OFFSET     +2
#define AVM_SAVEDTOPSP_OFFSET   +1

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
avm_memcell* avm_tablegetelem(avm_table* table, avm_memcell* index);
void avm_tablesetelem(avm_table* table, avm_memcell* index, avm_memcell* content);
void avm_memcell_clear(avm_memcell* m);

void memclear_string(avm_memcell* m);
void memclear_table(avm_memcell* m);

double consts_getnumber(unsigned index);
char* consts_getstring(unsigned index);
char* libfuncs_getused(unsigned index);

void libfunc_typeof();
void libfunc_totalarguments();
void avm_warning(char* msg);
void avm_error(char* msg);
avm_memcell* avm_translate_operand(vmarg* arg, avm_memcell* reg);
void avm_assign(avm_memcell* lv, avm_memcell* rv);
void execute_assign(instruction* instr);
char* avm_tostring(avm_memcell*);
void avm_calllibfunc(char* func_name);
void avm_callsaveenvironment();
void avm_dec_top();
void avm_push_envvalue(unsigned val);
void avm_callsaveenvironment();
user_func* avm_getfuncinfo(unsigned address);
void execute_funcenter(instruction* instr);
unsigned avm_get_envvalue(unsigned i);
void avm_calllibfunc(char* id);
unsigned avm_total_actuals();
void execute_pusharg(instruction* instr);

void avm_calllibfunc(char* id);
void libfunc_print();
void libfunc_typeof();
void libfunc_totalarguments();

char* number_tostring(avm_memcell*);
char* string_tostring(avm_memcell*);
char* bool_tostring(avm_memcell*);
char* table_tostring(avm_memcell*);
char* userfunc_tostring(avm_memcell*);
char* libfunc_tostring(avm_memcell*);
char* nil_tostring(avm_memcell*);
char* undef_tostring(avm_memcell*);
char* avm_tostring(avm_memcell* m);

double add_impl(double x, double y);
double sub_impl(double x, double y);
double mul_impl(double x, double y);
double div_impl(double x, double y);
double mod_impl(double x, double y);

unsigned lt_impl(double x, double y);
unsigned le_impl(double x, double y);
unsigned gt_impl(double x, double y);
unsigned ge_impl(double x, double y);

void execute_compare(instruction* instr);

unsigned char number_tobool(avm_memcell* m);
unsigned char string_tobool(avm_memcell* m);
unsigned char bool_tobool(avm_memcell* m);
unsigned char table_tobool(avm_memcell* m);
unsigned char userfunc_tobool(avm_memcell* m);
unsigned char libfunc_tobool(avm_memcell* m);
unsigned char nil_tobool(avm_memcell* m);
unsigned char undef_tobool(avm_memcell* m);
unsigned char avm_tobool(avm_memcell* m);

void execute_arithmetic(instruction* instr);
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
