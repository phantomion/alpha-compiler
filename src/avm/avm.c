#include "avm.h"
#include "../generator.h"
#include "../icode.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

avm_memcell stack[AVM_STACKSIZE];
avm_memcell ax, bx, cx;
avm_memcell retval;
unsigned top, topsp;
unsigned char execution_finished = 0;
unsigned pc = 0;
unsigned curr_line = 0;
unsigned code_size = 0;
unsigned total_actuals = 0;
instruction* code = null;

extern double* num_consts;
extern char** string_consts;
extern char** lib_funcs;


static void avm_initstack() {
    for (int i = 0; i < AVM_STACKSIZE; i++) {
        AVM_WIPEOUT(stack[i]);
        stack[i].type = undef_m;
    }
}

void avm_warning(char* msg) {
    fprintf(stdout, "Warning: %s\n", msg);
}

void avm_error(char* msg) {
    fprintf(stdout, "Error: %s\n", msg);
    execution_finished = 1;
}

double consts_getnumber(unsigned index) {
    return num_consts[index];
}

char* consts_getstring(unsigned index) {
    return string_consts[index];
}

char* libfuncs_getused(unsigned index) {
    return lib_funcs[index];
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

void avm_assign(avm_memcell* lv, avm_memcell* rv);

void execute_assign(instruction* instr) {
    avm_memcell* lv = avm_translate_operand(instr->result, (avm_memcell*) 0);
    avm_memcell* rv = avm_translate_operand(instr->arg1, &ax);

    assert(lv && (&stack[AVM_STACKSIZE-1] >= lv && lv > &stack[top] || lv == &retval));
    assert(rv);

    avm_assign(lv, rv);
}

char* avm_tostring(avm_memcell*);
void avm_calllibfunc(char* func_name);
void avm_callsaveenvironment();

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

void avm_assign(avm_memcell* lv, avm_memcell* rv) {
    if(lv == rv) return;
    if(lv->type == table_m &&
            rv->type == table_m &&
            lv->data.table_val == rv->data.table_val) return;

    if(rv->type == undef_m) avm_warning("Assigning from \'undef\' content!");

    avm_memcell_clear(lv);
    memcpy(lv, rv, sizeof(avm_memcell));

    if(lv->type == string_m) lv->data.str_val = strdup(rv->data.str_val);
    else if(lv->type == table_m) avm_tableincrefcounter(lv->data.table_val);
}

void avm_dec_top() {
    if(!top) {
        avm_error("stack overflow");
    }
    else --top;
}

void avm_push_envvalue(unsigned val) {
    stack[top].type = number_m;
    stack[top].data.num_val = val;
    avm_dec_top();
}

void avm_callsaveenvironment() {
    avm_push_envvalue(total_actuals);
    avm_push_envvalue(pc + 1);
    avm_push_envvalue(top + total_actuals + 2);
    avm_push_envvalue(topsp);
}

user_func* avm_getfuncinfo(unsigned address);

void execute_funcstart(instruction* instr) {

    avm_memcell* func = avm_translate_operand(instr->result, &ax);
    assert(func);
    assert(pc == func->data.func_val);

    total_actuals = 0;
    user_func* func_info = avm_getfuncinfo(pc);
    topsp = top;
    top = top - func_info->localSize;
}

unsigned avm_get_envvalue(unsigned i) {
    assert((stack[i].type = number_m));
    unsigned val = (unsigned) stack[i].data.num_val;
    assert(stack[i].data.num_val == ((double) val));

    return val;
}

typedef void (*library_func_t)(void);
library_func_t avm_getlibraryfunc(char* id);

void avm_calllibfunc(char* id) {
    library_func_t f = avm_getlibraryfunc(id);

    if(!f) {
        printf("Error: Unsupported lib func \'%s\' called!\n", id);
        execution_finished = 1;
    }
    else {
        topsp = top;
        total_actuals = 0;
        (*f)();
        if(!execution_finished) execute_funcend((instruction*) 0);
    }
}

unsigned avm_total_actuals() {
    return avm_get_envvalue(topsp + AVM_NUMACTUALS_OFFSET);
}

avm_memcell* avm_get_actual(unsigned i) {
    assert(i < avm_total_actuals());
    return &stack[topsp + AVM_STACKENV_SIZE + i + 1];
}

void libfunc_print() {
    unsigned n = avm_total_actuals();
    for(unsigned i = 0; i < n; i++) {
        char* s = avm_tostring(avm_get_actual(i));
        puts(s);
        free(s);
    }
}

void execute_pusharg(instruction* instr) {
    avm_memcell* arg = avm_translate_operand(instr->arg1, &ax);
    assert(arg);

    avm_assign(&stack[top], arg);
    ++total_actuals;
    avm_dec_top();
}

typedef char* (*tostring_func_t)(avm_memcell*);

char* number_tostring(avm_memcell*);
char* string_tostring(avm_memcell*);
char* bool_tostring(avm_memcell*);
char* table_tostring(avm_memcell*);
char* userfunc_tostring(avm_memcell*);
char* libfunc_tostring(avm_memcell*);
char* nil_tostring(avm_memcell*);
char* undef_tostring(avm_memcell*);

tostring_func_t tostring_funcs[] = {
    number_tostring,
    string_tostring,
    bool_tostring,
    table_tostring,
    userfunc_tostring,
    libfunc_tostring,
    nil_tostring,
    undef_tostring
};

char* avm_tostring(avm_memcell* m) {
    assert(m->type >= 0 && m->type <= undef_m);
    return (*tostring_funcs[m->type])(m);
}

typedef double (*arithmetic_func_t)(double x, double y);

double add_impl(double x, double y) {return x + y;}
double sub_impl(double x, double y) {return x - y;}
double mul_impl(double x, double y) {return x * y;}
double div_impl(double x, double y) {return x / y;} // check error div 0
// check error div 0
double mod_impl(double x, double y) {return ((unsigned) x % (unsigned) y);}

arithmetic_func_t arithmetic_funcs[] = {
    add_impl,
    sub_impl,
    mul_impl,
    div_impl,
    mod_impl
};

void execute_arithmetic(instruction* instr) {
    avm_memcell* lv = avm_translate_operand(instr->result, (avm_memcell*) 0);
    avm_memcell* rv1 = avm_translate_operand(instr->arg1, &ax);
    avm_memcell* rv2 = avm_translate_operand(instr->arg2, &bx);

    assert(lv && (&stack[AVM_STACKSIZE - 1] >= lv && lv > &stack[top] || lv == &retval));
    assert(rv1 && rv2);

    if(rv1->type != number_m || rv2->type != number_m) {
        avm_error("Not a number in arithmetic!");
        execution_finished = 1;
    }
    else {
        arithmetic_func_t op = arithmetic_funcs[instr->opcode - add_v];
        avm_memcell_clear(lv);
        lv->type = number_m;
        lv->data.num_val = (*op)(rv1->data.num_val, rv2->data.num_val);
    }
}

// Tread carefully

typedef unsigned (*cmp_func_t)(double x, double y);

unsigned lt_impl(double x, double y) {return x < y;}
unsigned le_impl(double x, double y) {return x <= y;}
unsigned gt_impl(double x, double y) {return x > y;}
unsigned ge_impl(double x, double y) {return x >= y;}

cmp_func_t compare_funcs[] = {
    lt_impl,
    le_impl,
    gt_impl,
    ge_impl
};

void execute_compare(instruction* instr) {
    assert(instr->result->type == label_a);

    avm_memcell* rv1 = avm_translate_operand(instr->arg1, &ax);
    avm_memcell* rv2 = avm_translate_operand(instr->arg2, &bx);
    unsigned char result = 0;

    assert(rv1 && rv2);

    if(rv1->type != number_m || rv2->type != number_m) {
        avm_error("Not a number in compare!");
    }
    else {
        cmp_func_t op = compare_funcs[instr->opcode - jle_v]; // Is this right?????
        result = (*op)(rv1->data.num_val, rv2->data.num_val);
    }

    if(!execution_finished && result)
        pc = instr->result->val;
}

typedef unsigned char (*tobool_func_t)(avm_memcell*);

unsigned char number_tobool(avm_memcell* m) {return m->data.num_val != 0;}
unsigned char string_tobool(avm_memcell* m) {return m->data.str_val[0] != 0;}
unsigned char bool_tobool(avm_memcell* m) {return m->data.bool_val;}
unsigned char table_tobool(avm_memcell* m) {return 1;}
unsigned char userfunc_tobool(avm_memcell* m) {return 1;}
unsigned char libfunc_tobool(avm_memcell* m) {return 1;}
unsigned char nil_tobool(avm_memcell* m) {return 0;}
unsigned char undef_tobool(avm_memcell* m) {assert(0); return 0;}

tobool_func_t tobool_funcs[] = {
    number_tobool,
    string_tobool,
    bool_tobool,
    table_tobool,
    userfunc_tobool,
    libfunc_tobool,
    nil_tobool,
    undef_tobool
};

unsigned char avm_tobool(avm_memcell* m) {
    assert(m->type >= 0 && m->type < undef_m);
    return (*tobool_funcs[m->type])(m);
}

char* type_strings[] = {
    "number",
    "string",
    "bool",
    "table",
    "userfunc",
    "libfunc",
    "nil",
    "undef"
};

void libfunc_typeof() {
    unsigned n = avm_total_actuals();

    if(n != 1)
        avm_error("Expected ONE argument in \'typeof\'!");
    else {
        avm_memcell_clear(&retval);
        retval.type = string_m;
        retval.data.str_val = strdup(type_strings[avm_get_actual(0)->type]);
    }
}

void libfunc_totalarguments() {
    unsigned p_topsp = avm_get_envvalue(topsp + AVM_SAVEDTOPSP_OFFSET);
    avm_memcell_clear(&retval);

    if(!p_topsp) {
        avm_error("\'totalarguments\' called outside a function!");
        retval.type = nil_m;
    }
    else {
        retval.type = number_m;
        retval.data.num_val = avm_get_envvalue(p_topsp + AVM_NUMACTUALS_OFFSET);
    }
}

avm_memcell* avm_tablegetelem(avm_table* table, avm_memcell* index);

void avm_tablesetelem(avm_table* table, avm_memcell* index, avm_memcell* content);

void execute_uminus(instruction* instr) {}
void execute_and(instruction* instr) {}
void execute_or(instruction* instr) {}
void execute_not(instruction* instr) {}

void execute_if_eq(instruction* instr) {
    assert(instr->result->type == label_a);

    avm_memcell* rv1 = avm_translate_operand(instr->arg1, &ax);
    avm_memcell* rv2 = avm_translate_operand(instr->arg2, &bx);

    unsigned char result = 0;

    if(rv1->type == undef_m || rv2->type == undef_m)
        avm_error("\'undef\' involved in equality!");
    else if(rv1->type == nil_m || rv2->type == nil_m)
        result = rv1->type == nil_m && rv2->type == nil_m;
    else if(rv1->type == bool_m || rv2->type == bool_m)
        result = (avm_tobool(rv1) == avm_tobool(rv2));
    else if(rv1->type != rv2->type)
        avm_error("== between these two operands is illegal!");
    else
        result = (*tobool_funcs[rv1->type])(rv1) == (*tobool_funcs[rv1->type])(rv2); // Is this right?

    if(!execution_finished && result)
        pc = instr->result->val;
}

void execute_if_noteq(instruction* instr) {}
void execute_jump(instruction* instr) {}

void execute_call(instruction* instr) {

    avm_memcell* func = avm_translate_operand(instr->result, &ax);
    assert(func);

    avm_callsaveenvironment();

    switch(func->type) {
        case userfunc_m:
            pc = func->data.func_val;
            assert(pc < AVM_ENDING_PC);
            assert(code[pc].opcode == funcenter_v);
            break;
        case string_m:
            avm_calllibfunc(func->data.str_val);
            break;
        case libfunc_m:
            avm_calllibfunc(func->data.libfunc_val);
            break;
        default: {
            char* s = avm_tostring(func);
            printf("Error: call: cannot bind \'%s\' to function!\n", s);
            free(s);
            execution_finished = 1;
        }
    }
}

void execute_param(instruction* instr) {}
void execute_ret(instruction* instr) {}
void execute_getretval(instruction* instr) {}

void execute_funcend(instruction* instr) {
    unsigned old_top = top;
    top = avm_get_envvalue(topsp + AVM_SAVEDTOP_OFFSET);
    pc = avm_get_envvalue(topsp + AVM_SAVEDPC_OFFSET);
    topsp = avm_get_envvalue(topsp + AVM_SAVEDTOPSP_OFFSET);

    while(++old_top <= top) avm_memcell_clear(&stack[old_top]);
}

void execute_tablecreate(instruction* instr) {
    avm_memcell* lv = avm_translate_operand(instr->result, (avm_memcell*) 0);
    assert(lv && (&stack[AVM_STACKSIZE - 1] >= lv && lv > &stack[top] || lv == &retval));

    avm_memcell_clear(lv);

    lv->type = table_m;
    lv->data.table_val = avm_tablenew();
    avm_tableincrefcounter(lv->data.table_val);
}

void execute_tablegetelem(instruction* instr) {
    avm_memcell* lv = avm_translate_operand(instr->result, (avm_memcell*) 0);
    avm_memcell* t = avm_translate_operand(instr->arg1, (avm_memcell*) 0);
    avm_memcell* i = avm_translate_operand(instr->arg2, &ax);

    assert(lv && &stack[AVM_STACKSIZE - 1] >= lv && lv > &stack[top] || lv == &retval);
    assert(lv && &stack[AVM_STACKSIZE - 1] >= lv && lv > &stack[top]);
    assert(i);

    avm_memcell_clear(lv);
    lv->type = nil_m;

    if(t->type != table_m) avm_error("Not a table!");
    else {
        avm_memcell* content = avm_tablegetelem(t->data.table_val, i);
        if(content) avm_assign(lv, content);
        else {
            char* ts = avm_tostring(t);
            char* is = avm_tostring(i);
            avm_warning("Element not found!");
            free(ts);
            free(is);
        }
    }
}

void execute_tablesetelem(instruction* instr) {
    avm_memcell* t = avm_translate_operand(instr->result, (avm_memcell*) 0);
    avm_memcell* i = avm_translate_operand(instr->arg1, &ax);
    avm_memcell* c = avm_translate_operand(instr->arg2, &bx);

    assert(t && &stack[AVM_STACKSIZE - 1] >= t && t > &stack[top]);
    assert(i && c);

    if(t->type != table_m)
        avm_error("Illegal use lof type as table!");
    else
        avm_tablesetelem(t->data.table_val, i, c);
}

void avm_registerlibfunc(char* id, library_func_t addr);

void libfunc_typeof();
void libfunc_totalarguments();

void avm_initialize() {
    avm_initstack();

    avm_registerlibfunc("print", libfunc_print);
    avm_registerlibfunc("typeof", libfunc_typeof);
    avm_registerlibfunc("totalarguments", libfunc_totalarguments);
    /*avm_registerlibfunc("argument", libfunc_argument);*/
}

