#include "avm.h"
#include "generator.h"
#include <assert.h>
#include <stdlib.h>

avm_memcell stack[AVM_STACKSIZE];
avm_memcell ax, bx, cx;
avm_memcell retval;
unsigned top, topsp;
unsigned char execution_finished = 0;
unsigned pc = 0;
unsigned curr_line = 0;
unsigned code_size = 0;
instruction* code = null;

extern double* num_consts;
extern char** string_consts;
extern char** lib_funcs;

double consts_getnumber(unsigned index) {
    return num_consts[index];
}

char* consts_getstring(unsigned index) {
    return string_consts[index];
}

char* libfuncs_getused(unsigned index) {
    return lib_funcs[index];
}


static void avm_initstack() {
    for (int i = 0; i < AVM_STACKSIZE; i++) {
        AVM_WIPEOUT(stack[i]);
        stack[i].type = undef_m;
    }
}

void avm_tableincrefcounter(avm_table* t) {
    ++t->ref_counter;
}

void avm_tabledecrefcounter(avm_table* t) {
    assert(t->ref_counter > 0);
    if (!--t->ref_counter) {
        avm_tabledestroy(t);
    }
}

void avm_tablebucketsinit(avm_table_bucket** p) {
    for (size_t i = 0; i < AVM_TABLE_HASHSIZE; i++) {
        p[i] = null;
    }
}

avm_table* avm_tablenew() {
    avm_table* t = calloc(1, sizeof(avm_table));

    avm_tablebucketsinit(t->num_indexed);
    avm_tablebucketsinit(t->str_indexed);

    return t;
}


typedef void (*memclear_func_t) (avm_memcell*);

memclear_func_t memclear_funcs[] = {
    0,
    memclear_string,
    0,
    memclear_table,
    0,
    0,
    0,
    0,
};

void memclear_string(avm_memcell* m) {
    assert(m->data.str_val);
    free(m->data.str_val);
}

void memclear_table(avm_memcell* m) {
    assert(m->data.table_val);
    avm_tabledecrefcounter(m->data.table_val);
}

void avm_memcell_clear(avm_memcell* m) {
    if (m->type == undef_m) return;
    memclear_func_t f = memclear_funcs[m->type];
    if (f) {
        (*f) (m);
    }
    m->type = undef_m;
}


void avm_tablebucketsdestroy(avm_table_bucket** p) {
    for (size_t i = 0; i < AVM_TABLE_HASHSIZE; i++) {
        for (avm_table_bucket* b = *p; b;) {
            avm_table_bucket* del = b;
            b = b->next;
            avm_memcell_clear(&del->key);
            avm_memcell_clear(&del->value);
            free(del);
        }
        p[i] = null;
    }
}

void avm_tabledestroy(avm_table *t) {
    avm_tablebucketsdestroy(t->str_indexed);
    avm_tablebucketsdestroy(t->num_indexed);
    free(t);
}

avm_memcell* avm_translate_operand(vmarg* arg, avm_memcell* reg) {
    switch (arg->type) {
        case global_a:
            return &stack[AVM_STACKSIZE - 1 - arg->val];
        case local_a:
            return &stack[topsp - arg->val];
        case formal_a:
            return &stack[topsp + AVM_STACKENV_SIZE + arg->val];
        case retval_a:
            return &retval;
        case number_a:
            reg->type = number_m;
            reg->data.num_val = consts_getnumber(arg->val);
            return reg;
        case string_a:
            reg->type = string_m;
            reg->data.str_val = strdup(consts_getstring(arg->val));
            return reg;
        case bool_a:
            reg->type = bool_m;
            reg->data.bool_val = arg->val;
            return reg;
        case nil_a:
            reg->type = nil_m;
            return reg;
        case userfunc_a:
            reg->type = userfunc_m;
            reg->data.func_val = arg->val;
            return reg;
        case libfunc_a:
            reg->type = libfunc_m;
            reg->data.libfunc_val = libfuncs_getused(arg->val);
            return reg;
        default:
            assert(0);
    }
}

typedef void (*execute_func_t) (instruction*);

execute_func_t execute_funcs[] = {
    execute_assign,       execute_add,          execute_sub,
    execute_mul,          execute_div,          execute_mod,
    execute_uminus,       execute_and,          execute_or,
    execute_not,          execute_if_eq,        execute_if_noteq,
    execute_if_lesseq,    execute_if_greatereq, execute_if_less,
    execute_if_greater,   execute_jump,         execute_call,
    execute_param,        execute_ret,          execute_getretval,
    execute_funcstart,    execute_funcend,      execute_tablecreate,
    execute_tablegetelem, execute_tablesetelem,
};

void execute_assign(instruction* instr) {}
void execute_add(instruction* instr) {}
void execute_sub(instruction* instr) {}
void execute_mul(instruction* instr) {}
void execute_div(instruction* instr) {}
void execute_mod(instruction* instr) {}
void execute_uminus(instruction* instr) {}
void execute_and(instruction* instr) {}
void execute_or(instruction* instr) {}
void execute_not(instruction* instr) {}
void execute_if_eq(instruction* instr) {}
void execute_if_noteq(instruction* instr) {}
void execute_if_lesseq(instruction* instr) {}
void execute_if_greatereq(instruction* instr) {}
void execute_if_less(instruction* instr) {}
void execute_if_greater(instruction* instr) {}
void execute_jump(instruction* instr) {}
void execute_call(instruction* instr) {}
void execute_param(instruction* instr) {}
void execute_ret(instruction* instr) {}
void execute_getretval(instruction* instr) {}
void execute_funcstart(instruction* instr) {}
void execute_funcend(instruction* instr) {}
void execute_tablecreate(instruction* instr) {}
void execute_tablegetelem(instruction* instr) {}
void execute_tablesetelem(instruction* instr) {}

void execute_cycle() {
    if (execution_finished) {
        return;
    }
    else if (pc == AVM_ENDING_PC) {
        execution_finished = 1;
        return;
    }
    assert(pc < AVM_ENDING_PC);
    instruction* instr = code + pc;
    assert(instr->opcode >= 0 && instr->opcode <= AVM_MAX_INSTRUCTIONS);
    if (instr->src_line) {
        curr_line = instr->src_line;
    }
    unsigned oldpc = pc;
    (*execute_funcs[instr->opcode]) (instr);
    if (pc == oldpc) {
        ++pc;
    }
}
