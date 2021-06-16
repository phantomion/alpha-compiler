#include "avm.h"
#include "../vm_structs.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

avm_memcell stack[AVM_STACKSIZE];
avm_memcell ax, bx, cx;
avm_memcell retval;
unsigned top = AVM_STACKSIZE - 1, topsp = AVM_STACKSIZE - 1;
unsigned char execution_finished = 0;
unsigned pc = 1;
unsigned curr_line = 0;
unsigned code_size = 0;
unsigned total_actuals = 0;
unsigned total_globals = 0;
instruction* code = null;

extern double* num_consts;
extern unsigned total_num_consts;
extern char** string_consts;
extern unsigned total_string_consts;
extern char** lib_funcs;
extern unsigned total_lib_funcs;
extern user_func** user_funcs;
extern unsigned total_user_funcs;


static void avm_initstack() {
    for (int i = 0; i < AVM_STACKSIZE; i++) {
        AVM_WIPEOUT(stack[i]);
        stack[i].type = undef_m;
    }
}


void avm_warning(char* msg) {
    fprintf(stdout, "%d: "COLOR_YELLOW"warning:"COLOR_RESET" %s\n", curr_line, msg);
}


void avm_error(char* msg) {
    fprintf(stdout, "%d: "COLOR_RED"error:"COLOR_RESET" %s\n", curr_line, msg);
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

unsigned userfuncs_getaddr(unsigned index) {
    return user_funcs[index]->address;
}

user_func* userfuncs_get(unsigned index) {
    return user_funcs[index];
}


char* userfuncs_getid(unsigned funcaddr) {
    for (size_t i = 0; i < total_user_funcs; i++) {
        user_func* func= userfuncs_get(i);
        if (func->address == funcaddr) return func->id;
    }
    return null;
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
    avm_tablebucketsinit(t->bool_indexed);
    avm_tablebucketsinit(t->userfunc_indexed);
    avm_tablebucketsinit(t->libfunc_indexed);

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
    0
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
    avm_tablebucketsdestroy(t->bool_indexed);
    avm_tablebucketsdestroy(t->userfunc_indexed);
    avm_tablebucketsdestroy(t->libfunc_indexed);
    free(t);
}


avm_memcell* avm_translate_operand(vmarg* arg, avm_memcell* reg) {
    switch (arg->type) {
        case global_a:
            return &stack[AVM_STACKSIZE - 1 - arg->val];
        case local_a:
            return &stack[topsp - arg->val];
        case formal_a:
            return &stack[topsp + AVM_STACKENV_SIZE + arg->val + 1];
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
            reg->data.func_val = userfuncs_getaddr(arg->val);
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
    execute_jump,         execute_if_lesseq,    execute_if_greatereq,
    execute_if_less,      execute_if_greater,   execute_call,
    execute_param,        execute_funcstart,    execute_funcend,
    execute_tablecreate,  execute_tablegetelem, execute_tablesetelem,
};


void execute_assign(instruction* instr) {
    avm_memcell* lv = avm_translate_operand(instr->result, (avm_memcell*) 0);
    avm_memcell* rv = avm_translate_operand(instr->arg1, &ax);

    assert(lv);
    assert((&stack[AVM_STACKSIZE-1] >= lv && lv > &stack[top]) || lv == &retval);
    assert(rv);

    avm_assign(lv, rv);
}


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
            lv->data.table_val == rv->data.table_val)
        return;

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


user_func* avm_getfuncinfo(unsigned address) {
    for (size_t i = 0; i < total_user_funcs; i++) {
        if (address == user_funcs[i]->address) {
            return user_funcs[i];
        }
    }
    avm_error("User function not found");
    assert(0);
}


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
    assert((stack[i].type == number_m));
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
        printf("%s", s);
        free(s);
    }
}


void execute_param(instruction* instr) {
    avm_memcell* arg = avm_translate_operand(instr->result, &ax);
    assert(arg);

    avm_assign(&stack[top], arg);
    ++total_actuals;
    avm_dec_top();
}


typedef char* (*tostring_func_t)(avm_memcell*);

char* itoa(int val) {
    if (!val) return "0";

    static char buf[32] = {0};
    int i = 30;

    for(; val && i; --i, val /= 10)
        buf[i] = "0123456789abcdef"[val % 10];

    return &buf[i+1];
}


char* number_tostring(avm_memcell* m) {
    char to_num[20];
    sprintf(to_num, "%g", m->data.num_val);
    return strdup(to_num);
}

char* string_tostring(avm_memcell* m) {return strdup(m->data.str_val);}
char* bool_tostring(avm_memcell* m) {
    return strdup(m->data.bool_val == '1' ? "true" : "false");
}

char* buckets_tostring(avm_table_bucket* bucket, avm_memcell* m, char* table) {
        while(bucket) {
            char* key_val = avm_tostring(&bucket->key);
            table = realloc(table, strlen(table) + strlen(key_val) + 1);
            strcat(table, key_val);

            table = realloc(table, strlen(table) + 3);
            strcat(table, ": ");

            if (bucket->value.type == table_m &&
                    bucket->value.data.table_val == m->data.table_val) {
                table = realloc(table, strlen(table) + 6);
                strcat(table, "Table");
            }
            else {
                char* val_val = avm_tostring(&bucket->value);
                table = realloc(table, strlen(table) + strlen(val_val) + 1);
                strcat(table, val_val);
            }

            table = realloc(table, strlen(table) + 2);
            strcat(table, "\n");

            bucket = bucket->next;
        }
        return table;
}

char* table_tostring(avm_memcell* m) {
    char* table = calloc(1, 9);
    strcat(table, "Table[ \n");

    for (size_t i = 0; i < AVM_TABLE_HASHSIZE; i++)
        table = buckets_tostring(m->data.table_val->num_indexed[i], m, table);
    for (size_t i = 0; i < AVM_TABLE_HASHSIZE; i++)
        table = buckets_tostring(m->data.table_val->str_indexed[i], m, table);
    for (size_t i = 0; i < AVM_TABLE_HASHSIZE; i++)
        table = buckets_tostring(m->data.table_val->bool_indexed[i], m, table);
    for (size_t i = 0; i < AVM_TABLE_HASHSIZE; i++)
        table = buckets_tostring(m->data.table_val->userfunc_indexed[i], m, table);
    for (size_t i = 0; i < AVM_TABLE_HASHSIZE; i++)
        table = buckets_tostring(m->data.table_val->libfunc_indexed[i], m, table);

    table = realloc(table, strlen(table) + 2);
    strcat(table, "]");

    return strdup(table);
}

char* userfunc_tostring(avm_memcell* m) {
    char* funcaddr = itoa(m->data.func_val);
    char* result = calloc(1, 15 + strlen(funcaddr));
    strcpy(result, "user function ");
    strcat(result, funcaddr);
    return strdup(result);
}
char* libfunc_tostring(avm_memcell* m) {
    char* result = calloc(1, 18 + strlen(m->data.libfunc_val));
    strcpy(result, "library function ");
    strcat(result, m->data.libfunc_val);
    return strdup(result);
}
char* nil_tostring(avm_memcell* m) {return strdup("nil");}
char* undef_tostring(avm_memcell* m) {return strdup("undef");}


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
double div_impl(double x, double y) {
    if (y == 0) {
        avm_error("Can't divide by 0");
        return -1;
    }
    return x / y;
}
double mod_impl(double x, double y) {
    if (y == 0) {
        avm_error("Can't divide by 0");
        return -1;
    }
    return ((unsigned) x % (unsigned) y);
}


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

    assert(lv && ((&stack[AVM_STACKSIZE - 1] >= lv && lv > &stack[top]) || lv == &retval));
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


typedef unsigned (*cmp_func_t)(double x, double y);


unsigned lt_impl(double x, double y) {return x < y;}
unsigned le_impl(double x, double y) {return x <= y;}
unsigned gt_impl(double x, double y) {return x > y;}
unsigned ge_impl(double x, double y) {return x >= y;}


cmp_func_t compare_funcs[] = {
    le_impl,
    ge_impl,
    lt_impl,
    gt_impl,
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
        cmp_func_t op = compare_funcs[instr->opcode - jle_v];
        result = (*op)(rv1->data.num_val, rv2->data.num_val);
    }

    if(!execution_finished && result)
        pc = instr->result->val;
}


typedef unsigned char (*tobool_func_t)(avm_memcell*);


unsigned char number_tobool(avm_memcell* m) {return m->data.num_val != 0;}
unsigned char string_tobool(avm_memcell* m) {return m->data.str_val[0] != 0;}
unsigned char bool_tobool(avm_memcell* m) {
    if (m->data.bool_val == '1') return 1;
    else return 0;
}
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


void libfunc_objecttotalmembers() {

    unsigned n = avm_total_actuals();
    avm_memcell_clear(&retval);

    if (n != 1) {
        avm_error("Expected ONE argument in \'objecttotalmembers\'!");
    }

    avm_memcell* arg = avm_get_actual(0);
    if (arg->type != table_m) {
        avm_error("Expected table value for \'objecttotalmembers\'");
        return;
    }

    /*unsigned counter = 0;*/

    /*for (size_t i = 0; i < AVM_TABLE_HASHSIZE; i++) {*/
        /*avm_table_bucket* num_bucket = arg->data.table_val->num_indexed[i];*/

        /*while(num_bucket) {*/
            /*counter++;*/
            /*num_bucket = num_bucket->next;*/
        /*}*/
    /*}*/

    /*for (size_t i = 0; i < AVM_TABLE_HASHSIZE; i++) {*/
        /*avm_table_bucket* str_bucket = arg->data.table_val->str_indexed[i];*/

        /*while(str_bucket) {*/
            /*counter++;*/
            /*str_bucket = str_bucket->next;*/
        /*}*/
    /*}*/

    /*for (size_t i = 0; i < AVM_TABLE_HASHSIZE; i++) {*/
        /*avm_table_bucket* bool_bucket = arg->data.table_val->bool_indexed[i];*/

        /*while(bool_bucket) {*/
            /*counter++;*/
            /*bool_bucket = bool_bucket->next;*/
        /*}*/
    /*}*/
    /*for (size_t i = 0; i < AVM_TABLE_HASHSIZE; i++) {*/
        /*avm_table_bucket* userfunc_bucket = arg->data.table_val->userfunc_indexed[i];*/

        /*while(userfunc_bucket) {*/
            /*counter++;*/
            /*userfunc_bucket = userfunc_bucket->next;*/
        /*}*/
    /*}*/
    /*for (size_t i = 0; i < AVM_TABLE_HASHSIZE; i++) {*/
        /*avm_table_bucket* libfunc_bucket = arg->data.table_val->libfunc_indexed[i];*/

        /*while(libfunc_bucket) {*/
            /*counter++;*/
            /*libfunc_bucket = libfunc_bucket->next;*/
        /*}*/
    /*}*/
    retval.type = number_m;
    retval.data.num_val = arg->data.table_val->total;
}


void libfunc_objectcopy() {
    unsigned n = avm_total_actuals();
    avm_memcell_clear(&retval);

    if (n != 1) {
        avm_error("Expected ONE argument in \'objectcopy\'!");
    }

    avm_memcell* arg = avm_get_actual(0);
    if (arg->type != table_m) {
        avm_error("Expected table value for \'objectcopy\'");
        return;
    }

    avm_table* new = avm_tablenew();
    avm_tableincrefcounter(new);

    for (size_t i = 0; i < AVM_TABLE_HASHSIZE; i++) {
        avm_table_bucket* num_bucket = arg->data.table_val->num_indexed[i];

        while(num_bucket) {
            avm_memcell* index = calloc(1, sizeof(avm_memcell));
            index->type = number_m;
            index->data.num_val = num_bucket->key.data.num_val;

            avm_memcell* content = avm_tablegetelem(arg->data.table_val, index);
            avm_tablesetelem(new, index, content);

            num_bucket = num_bucket->next;
        }

        avm_table_bucket* str_bucket = arg->data.table_val->str_indexed[i];

        while(str_bucket) {
            avm_memcell* index = calloc(1, sizeof(avm_memcell));
            index->type = string_m;
            index->data.str_val = strdup(str_bucket->key.data.str_val);

            avm_memcell* content = avm_tablegetelem(arg->data.table_val, index);
            avm_tablesetelem(new, index, content);

            str_bucket = str_bucket->next;
        }
    }

    retval.type = table_m;
    retval.data.table_val = new;
}


void libfunc_objectmemberkeys() {

    unsigned n = avm_total_actuals();
    avm_memcell_clear(&retval);

    if (n != 1) {
        avm_error("Expected ONE argument in \'objectmemberkeys\'!");
    }

    avm_memcell* arg = avm_get_actual(0);
    if (arg->type != table_m) {
        avm_error("Expected table value for \'objectmemberkeys\'");
        return;
    }

    avm_table* new = avm_tablenew();
    avm_tableincrefcounter(new);
    unsigned pos = 0;

    avm_memcell* m = arg;

    for (size_t i = 0; i < AVM_TABLE_HASHSIZE; i++) {
        avm_table_bucket* num_bucket = m->data.table_val->num_indexed[i];

        while(num_bucket) {
            avm_memcell* index = calloc(1, sizeof(avm_memcell));
            index->type = number_m;
            index->data.num_val = pos;

            avm_memcell* content = calloc(1, sizeof(avm_memcell));
            memcpy(content, num_bucket, sizeof(avm_memcell));

            avm_tablesetelem(new, index, content);

            pos++;
            num_bucket = num_bucket->next;
        }
    }

    for (size_t i = 0; i < AVM_TABLE_HASHSIZE; i++) {
        avm_table_bucket* str_bucket = m->data.table_val->str_indexed[i];

        while(str_bucket) {
            avm_memcell* index = calloc(1, sizeof(avm_memcell));
            index->type = number_m;
            index->data.num_val = pos;

            avm_memcell* content = calloc(1, sizeof(avm_memcell));
            memcpy(content, str_bucket, sizeof(avm_memcell));

            avm_tablesetelem(new, index, content);

            pos++;
            str_bucket = str_bucket->next;
        }
    }

    for (size_t i = 0; i < AVM_TABLE_HASHSIZE; i++) {
        avm_table_bucket* bool_bucket = m->data.table_val->bool_indexed[i];

        while(bool_bucket) {
            avm_memcell* index = calloc(1, sizeof(avm_memcell));
            index->type = number_m;
            index->data.num_val = pos;

            avm_memcell* content = calloc(1, sizeof(avm_memcell));
            memcpy(content, bool_bucket, sizeof(avm_memcell));

            avm_tablesetelem(new, index, content);

            pos++;
            bool_bucket = bool_bucket->next;
        }
    }
    for (size_t i = 0; i < AVM_TABLE_HASHSIZE; i++) {
        avm_table_bucket* userfunc_bucket = m->data.table_val->userfunc_indexed[i];

        while(userfunc_bucket) {
            avm_memcell* index = calloc(1, sizeof(avm_memcell));
            index->type = number_m;
            index->data.num_val = pos;

            avm_memcell* content = calloc(1, sizeof(avm_memcell));
            memcpy(content, userfunc_bucket, sizeof(avm_memcell));

            avm_tablesetelem(new, index, content);

            pos++;
            userfunc_bucket = userfunc_bucket->next;
        }
    }
    for (size_t i = 0; i < AVM_TABLE_HASHSIZE; i++) {
        avm_table_bucket* libfunc_bucket = m->data.table_val->libfunc_indexed[i];

        while(libfunc_bucket) {
            avm_memcell* index = calloc(1, sizeof(avm_memcell));
            index->type = number_m;
            index->data.num_val = pos;

            avm_memcell* content = calloc(1, sizeof(avm_memcell));
            memcpy(content, libfunc_bucket, sizeof(avm_memcell));

            avm_tablesetelem(new, index, content);

            pos++;
            libfunc_bucket = libfunc_bucket->next;
        }
    }
    retval.type = table_m;
    retval.data.table_val = new;
}


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


void libfunc_input() {
    unsigned n = avm_total_actuals();

    if (n != 0) {
        avm_error("Expected ZERO arguments in \'input\'!");
    }
    else {
        avm_memcell_clear(&retval);
        char* input = calloc(1, 256);
        int input_max = 256;
        int i = 0;
        char c = getc(stdin);
        while (c != '\n' && c != EOF) {
            if (i == input_max) {
                input = realloc(input, input_max + 256);
                input_max = input_max + 256;
            }
            input[i++] = c;
            c = getc(stdin);
        }
        input[i] = '\0';
        double num;

        if ((num = atof(input))) {
            retval.type = number_m;
            retval.data.num_val = num;
        }
        else if (strcmp(input, "true") == 0) {
            retval.type = bool_m;
            retval.data.bool_val = 1;
        }
        else if (strcmp(input, "false") == 0) {
            retval.type = bool_m;
            retval.data.bool_val = 0;
        }
        else if (strcmp(input, "nil") == 0) {
            retval.type = nil_m;
        }
        else {
            retval.type = string_m;
            retval.data.str_val = input;
        }
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


void libfunc_strtonum() {
    unsigned n = avm_total_actuals();
    avm_memcell_clear(&retval);

    if (n != 1) {
        avm_error("Expected ONE argument in \'strtonum\'!");
    }

    avm_memcell* arg = avm_get_actual(0);
    if (arg->type != string_m) {
        avm_error("Expected alphanumeric value for \'strtonum\'");
        return;
    }

    if (strcmp(arg->data.str_val, "0") == 0) {
        retval.type = number_m;
        retval.data.num_val = 0;
        return;
    }

    for (size_t i = 0; i < strlen(arg->data.str_val); i++) {
        if (!isdigit(arg->data.str_val[i]) && arg->data.str_val[i] != '.') {
            retval.type = nil_m;
            return;
        }
    }

    double result = atof(arg->data.str_val);
    if (!result) {
        retval.type = nil_m;
        return;
    }

    retval.type = number_m;
    retval.data.num_val = result;
}


void libfunc_sqrt() {
    unsigned n = avm_total_actuals();
    avm_memcell_clear(&retval);

    if (n != 1) {
        avm_error("Expected ONE argument in \'sqrt\'!");
    }

    avm_memcell* arg = avm_get_actual(0);
    if (arg->type != number_m) {
        avm_error("Expected numeric value for \'sqrt\'");
        return;
    }

    if (arg->data.num_val < 0) {
        retval.type = nil_m;
        return;
    }

    double result = sqrt(arg->data.num_val);

    retval.type = number_m;
    retval.data.num_val = result;
}


void libfunc_cos() {
    unsigned n = avm_total_actuals();
    avm_memcell_clear(&retval);

    if (n != 1) {
        avm_error("Expected ONE argument in \'cos\'!");
    }

    avm_memcell* arg = avm_get_actual(0);
    if (arg->type != number_m) {
        avm_error("Expected numeric value for \'cos\'");
        return;
    }

    double result = cos(arg->data.num_val);

    retval.type = number_m;
    retval.data.num_val = result;
}


void libfunc_sin() {
    unsigned n = avm_total_actuals();
    avm_memcell_clear(&retval);

    if (n != 1) {
        avm_error("Expected ONE argument in \'sin\'!");
        return;
    }

    avm_memcell* arg = avm_get_actual(0);
    if (arg->type != number_m) {
        avm_error("Expected numeric value for \'sin\'");
        return;
    }

    double result = sin(arg->data.num_val);

    retval.type = number_m;
    retval.data.num_val = result;
}


void libfunc_argument() {
    unsigned p_topsp = avm_get_envvalue(topsp + AVM_SAVEDTOPSP_OFFSET);
    avm_memcell_clear(&retval);

    unsigned n = avm_total_actuals();
    unsigned arg = avm_get_actual(0)->data.num_val;
    unsigned num_actuals = avm_get_envvalue(p_topsp + AVM_NUMACTUALS_OFFSET);

    if(!p_topsp) {
        avm_error("\'argument\' called outside a function!");
        retval.type = nil_m;
    }
    else if(n != 1){
        avm_error("Expected ONE argument in \'argument\'!");
        retval.type = nil_m;
    }
    else if(arg < 0) {
        avm_error("Index cannot be negative in \'argument\'!");
        retval.type = nil_m;
    }
    else if(arg > num_actuals) {
        avm_error("Index bigger than total arguments in \'argument\'!");
        retval.type = nil_m;
    }
    else {
        avm_memcell* actual = &stack[p_topsp + AVM_NUMACTUALS_OFFSET + arg + 1];
        retval.type = actual->type;
        if (retval.type == string_m) {
            retval.data.str_val = strdup(actual->data.str_val);
        }
        else {
            retval.data = actual->data;
        }
    }
}


unsigned int avm_tablehashstring(char* index) {
    size_t ui;
    unsigned int uiHash = 0U;

    for (ui = 0U; index[ui] != '\0'; ui++)
        uiHash = uiHash * HASH_MULTIPLIER + index[ui];

    return uiHash % AVM_TABLE_HASHSIZE;
}


unsigned int avm_tablehashnumber(double index) {
    return EUCMOD((int)index, AVM_TABLE_HASHSIZE);
}


avm_memcell* avm_tablegetelem(avm_table* table, avm_memcell* index) {
    if(index->type == string_m) {
        avm_table_bucket* bucket = table->str_indexed[avm_tablehashstring(index->data.str_val)];

        while(bucket) {
            if(strcmp(index->data.str_val, bucket->key.data.str_val) == 0) return &bucket->value;
            bucket = bucket->next;
        }

        return null;
    }
    else if(index->type == number_m) {
        avm_table_bucket* bucket = table->num_indexed[avm_tablehashnumber(index->data.num_val)];

        while(bucket) {
            if(index->data.num_val == bucket->key.data.num_val) return &bucket->value;
            bucket = bucket->next;
        }

        return null;
    }
    else if (index->type == bool_m) {
        int index_b;
        if (index->data.bool_val == '1') index_b = 1;
        else index_b = 0;
        avm_table_bucket* bucket = table->bool_indexed[index_b];
        if(bucket && index->data.bool_val == bucket->key.data.bool_val) return &bucket->value;

        return null;
    }
    else if(index->type == userfunc_m) {
        avm_table_bucket* bucket = table->userfunc_indexed[avm_tablehashnumber(index->data.func_val)];

        while(bucket) {
            if(index->data.func_val == bucket->key.data.func_val) return &bucket->value;
            bucket = bucket->next;
        }

        return null;
    }
    else if (index->type == libfunc_m) {
        avm_table_bucket* bucket = table->libfunc_indexed[avm_tablehashstring(index->data.libfunc_val)];

        while(bucket) {
            if(strcmp(index->data.libfunc_val, bucket->key.data.libfunc_val) == 0) return &bucket->value;
            bucket = bucket->next;
        }

        return null;
    }

    avm_error("Illegal table index type!");
    return null;
}


void avm_tablesetelem(avm_table* table, avm_memcell* index, avm_memcell* content) {
    avm_table_bucket* new = calloc(1, sizeof(avm_table_bucket));
    new->key = *index;
    avm_assign(&new->value, content);
    new->next = null;

    avm_table_bucket* prev = null;

    if(index->type == string_m) {
        avm_table_bucket* bucket = table->str_indexed[avm_tablehashstring(index->data.str_val)];

        if(bucket){
            while(bucket) {
                if(strcmp(index->data.str_val, bucket->key.data.str_val) == 0) {
                    avm_assign(&bucket->value, content);
                    if (content->type == nil_m) table->total--;
                    return;
                }
                prev = bucket;
                bucket = bucket->next;
            }
            prev->next = new;
        }
        else {
            if (content->type != nil_m) table->total++;
            table->str_indexed[avm_tablehashstring(index->data.str_val)] = new;
        }

        return;
    }
    else if(index->type == number_m) {
        avm_table_bucket* bucket = table->num_indexed[avm_tablehashnumber(index->data.num_val)];

        if(bucket){
            while(bucket) {
                if (bucket->key.data.num_val == index->data.num_val) {
                    avm_assign(&bucket->value, content);
                    if (content->type == nil_m) table->total--;
                    return;
                }
                prev = bucket;
                bucket = bucket->next;
            }
            prev->next = new;
        }
        else {
            if (content->type != nil_m) table->total++;
            table->num_indexed[avm_tablehashnumber(index->data.num_val)] = new;
        }

        return;
    }
    else if(index->type == bool_m) {
        int index_b;
        if (index->data.bool_val == '1') index_b = 1;
        else index_b = 0;
        avm_table_bucket* bucket = table->bool_indexed[index_b];
        if (bucket && bucket->key.data.bool_val == index->data.bool_val) {
            avm_assign(&bucket->value, content);
            if (content->type == nil_m) table->total--;
            return;
        }
        if (content->type != nil_m) table->total++;
        table->bool_indexed[index_b] = new;
        return;
    }
    else if(index->type == userfunc_m) {
        avm_table_bucket* bucket = table->userfunc_indexed[avm_tablehashnumber(index->data.func_val)];

        if(bucket){
            while(bucket) {
                if (bucket->key.data.func_val == index->data.func_val) {
                    avm_assign(&bucket->value, content);
                    if (content->type == nil_m) table->total--;
                    return;
                }
                prev = bucket;
                bucket = bucket->next;
            }
            prev->next = new;
        }
        else {
            if (content->type != nil_m) table->total++;
            table->userfunc_indexed[avm_tablehashnumber(index->data.func_val)] = new;
        }

        return;
    }
    else if(index->type == libfunc_m) {
        avm_table_bucket* bucket = table->libfunc_indexed[avm_tablehashstring(index->data.libfunc_val)];

        if(bucket){
            while(bucket) {
                if(strcmp(index->data.libfunc_val, bucket->key.data.libfunc_val) == 0) {
                    avm_assign(&bucket->value, content);
                    if (content->type == nil_m) table->total--;
                    return;
                }
                prev = bucket;
                bucket = bucket->next;
            }
            prev->next = new;
        }
        else {
            if (content->type != nil_m) table->total++;
            table->libfunc_indexed[avm_tablehashstring(index->data.libfunc_val)] = new;
        }

        return;
    }

    avm_error("Illegal table index type!");
}


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
    else {
        //big IF
        if (rv1->type == number_m) {
            result = rv1->data.num_val == rv2->data.num_val;
        }
        else if (rv1->type == string_m) {
            result = strcmp(rv1->data.str_val, rv2->data.str_val) == 0;
        }
        else {
            result = (*tobool_funcs[rv1->type])(rv1) == (*tobool_funcs[rv1->type])(rv2);
        }
    }

    if(!execution_finished && result)
        pc = instr->result->val;
}


void execute_if_noteq(instruction* instr) {
    assert(instr->result->type == label_a);

    avm_memcell* rv1 = avm_translate_operand(instr->arg1, &ax);
    avm_memcell* rv2 = avm_translate_operand(instr->arg2, &bx);

    unsigned char result = 0;

    if(rv1->type == undef_m || rv2->type == undef_m)
        avm_error("\'undef\' involved in inequality!");
    else if(rv1->type == nil_m || rv2->type == nil_m)
        result = !(rv1->type == nil_m && rv2->type == nil_m);
    else if(rv1->type == bool_m || rv2->type == bool_m)
        result = (avm_tobool(rv1) != avm_tobool(rv2));
    else if(rv1->type != rv2->type)
        avm_error("!= between these two operands is illegal!");
    else {
        //big IF
        if (rv1->type == number_m) {
            result = rv1->data.num_val != rv2->data.num_val;
        }
        else if (rv1->type == string_m) {
            result = strcmp(rv1->data.str_val, rv2->data.str_val) != 0;
        }
        else {
            result = (*tobool_funcs[rv1->type])(rv1) != (*tobool_funcs[rv1->type])(rv2);
        }
    }

    if(!execution_finished && result)
        pc = instr->result->val;
}

void execute_jump(instruction* instr) {
    assert(instr->result->type == label_a);

    pc = instr->result->val;
}

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
            printf("Error: call: cannot bind \'%s\' to function at line %d!\n", s, instr->src_line);
            free(s);
            execution_finished = 1;
        }
    }
}


void execute_funcend(instruction* instr) {
    unsigned old_top = top;
    top = avm_get_envvalue(topsp + AVM_SAVEDTOP_OFFSET);
    pc = avm_get_envvalue(topsp + AVM_SAVEDPC_OFFSET);
    topsp = avm_get_envvalue(topsp + AVM_SAVEDTOPSP_OFFSET);

    while(++old_top <= top) avm_memcell_clear(&stack[old_top]);
}


void execute_tablecreate(instruction* instr) {
    avm_memcell* lv = avm_translate_operand(instr->result, (avm_memcell*) 0);
    assert(lv && ((&stack[AVM_STACKSIZE - 1] >= lv && lv > &stack[top]) || lv == &retval));

    avm_memcell_clear(lv);

    lv->type = table_m;
    lv->data.table_val = avm_tablenew();
    avm_tableincrefcounter(lv->data.table_val);
}


void execute_tablegetelem(instruction* instr) {
    avm_memcell* lv = avm_translate_operand(instr->result, (avm_memcell*) 0);
    avm_memcell* t = avm_translate_operand(instr->arg1, (avm_memcell*) 0);
    avm_memcell* i = avm_translate_operand(instr->arg2, &ax);

    assert(lv && ((&stack[AVM_STACKSIZE - 1] >= lv && lv > &stack[top]) || lv == &retval));
    assert(lv && &stack[AVM_STACKSIZE - 1] >= lv && lv > &stack[top]);
    assert(i);

    avm_memcell_clear(lv);
    lv->type = nil_m;

    if(t->type != table_m) avm_error("Not a table!");
    else {
        avm_memcell* content = avm_tablegetelem(t->data.table_val, i);
        if(content) avm_assign(lv, content);
        else avm_warning("Element not found!");
    }
}


void execute_tablesetelem(instruction* instr) {
    avm_memcell* t = avm_translate_operand(instr->result, (avm_memcell*) 0);
    avm_memcell* i = avm_translate_operand(instr->arg1, &ax);
    avm_memcell* c = avm_translate_operand(instr->arg2, &bx);

    assert(t && &stack[AVM_STACKSIZE - 1] >= t && t > &stack[top]);
    assert(i && c);

    if(t->type != table_m)
        avm_error("Illegal use of type as table!");
    else
        avm_tablesetelem(t->data.table_val, i, c);
}


typedef void (*library_func_t)(void);


library_func_t library_funcs[] = {
    libfunc_print,
    libfunc_typeof,
    libfunc_totalarguments,
    libfunc_argument,
    libfunc_input,
    libfunc_strtonum,
    libfunc_sqrt,
    libfunc_cos,
    libfunc_sin,
    libfunc_objectmemberkeys,
    libfunc_objecttotalmembers,
    libfunc_objectcopy
};


char* lib_func_array[] = {
    "print",
    "typeof",
    "totalarguments",
    "argument",
    "input",
    "strtonum",
    "sqrt",
    "cos",
    "sin",
    "objectmemberkeys",
    "objecttotalmembers",
    "objectcopy"
};


unsigned get_libfunc_id(char* id) {
    for(unsigned i = 0; i < 12; ++i) {
        if(strcmp(lib_func_array[i], id) == 0) {
            return i;
        }
    }

    return -1;
}


library_func_t avm_getlibraryfunc(char* id) {
    return (*library_funcs[get_libfunc_id(id)]);
}


void consts_newstring(char* s, int i) {
    if (!string_consts)
        string_consts = calloc(1, total_string_consts * sizeof(char *));

    string_consts[i] = s;
}


void consts_newnumber(double n, int i) {
    if (!num_consts)
        num_consts = calloc(1, total_num_consts * sizeof(double));

    num_consts[i] = n;
}


void new_userfunc(user_func* user, int i) {
    if (!user_funcs)
        user_funcs = calloc(1, total_user_funcs * sizeof(user_func));

    user_funcs[i] = user;
}


void consts_newlib(char* s, int i) {
    if (!lib_funcs)
        lib_funcs = calloc(1, total_lib_funcs * sizeof(char *));

    lib_funcs[i] = s;
}


void instr_add(instruction* instr, int i) {
    if (!code)
        code = calloc(1, code_size * sizeof(instruction));

    instruction* ins = code + i;
    ins->arg1 = calloc(1, sizeof(vmarg));
    ins->arg2 = calloc(1, sizeof(vmarg));
    ins->result = calloc(1, sizeof(vmarg));

    ins->opcode = instr->opcode;
    ins->arg1 = instr->arg1;
    ins->arg2 = instr->arg2;
    ins->result = instr->result;
    ins->src_line = instr->src_line;
}


void set_globals(unsigned offset) {
    if (offset > total_globals)
        total_globals = offset;
}


void read_abc_bin() {
    char* magicnumber[9];
    FILE* fp = fopen("alpha_bin.abc", "rb");
    if (!fp) {
        fprintf(stderr, "Binary file not found\n");
        exit(EXIT_FAILURE);
    }
    fread(magicnumber, 9, 1, fp);

    fread(&total_string_consts, sizeof(total_string_consts), 1, fp);
    for (size_t i = 0; i < total_string_consts; i++) {
        int str_len = 0;
        fread(&str_len, sizeof(int), 1, fp);

        char* str_const = calloc(1, str_len);
        fread(str_const, str_len, 1, fp);

        consts_newstring(str_const, i);
    }

    fread(&total_num_consts, sizeof(total_num_consts), 1, fp);
    for (size_t i = 0; i < total_num_consts; i++) {

        double num_const = 0;
        fread(&num_const, sizeof(double), 1, fp);

        consts_newnumber(num_const, i);
    }

    fread(&total_user_funcs, sizeof(total_user_funcs), 1, fp);
    for (size_t i = 0; i < total_user_funcs; i++) {
        user_func* func = calloc(1, sizeof(user_func));
        int func_id_len = 0;

        fread(&func_id_len, sizeof(int), 1, fp);
        func->id = calloc(1, func_id_len);

        fread(&func->address, sizeof(int), 1, fp);
        fread(&func->localSize, sizeof(int), 1, fp);
        fread(func->id, func_id_len, 1, fp);

        new_userfunc(func, i);
    }

    fread(&total_lib_funcs, sizeof(total_lib_funcs), 1, fp);
    for (size_t i = 0; i < total_lib_funcs; i++) {
        int str_len = 0;
        fread(&str_len, sizeof(int), 1, fp);

        char* lib_const = calloc(1, str_len);
        fread(lib_const, str_len, 1, fp);

        consts_newlib(lib_const, i);
    }

    fread(&code_size, sizeof(code_size), 1, fp);
    for (size_t i = 1; i < code_size; i++) {
        instruction* instr = calloc(1, sizeof(instruction));
        instr->result = calloc(1, sizeof(vmarg));
        instr->arg1 = calloc(1, sizeof(vmarg));
        instr->arg2 = calloc(1, sizeof(vmarg));

        fread(&instr->opcode, sizeof(vmarg_t), 1, fp);
        fread(&instr->result->type, sizeof(vmarg_t), 1, fp);
        fread(&instr->result->val, sizeof(int), 1, fp);
        fread(&instr->arg1->type, sizeof(vmarg_t), 1, fp);
        fread(&instr->arg1->val, sizeof(int), 1, fp);
        fread(&instr->arg2->type, sizeof(vmarg_t), 1, fp);
        fread(&instr->arg2->val, sizeof(int), 1, fp);
        if (instr->result->type == global_a)
            set_globals(instr->result->val);
        if (instr->arg1->type == global_a)
            set_globals(instr->arg1->val);
        if (instr->arg2->type == global_a)
            set_globals(instr->arg2->val);
        fread(&instr->src_line, sizeof(int), 1, fp);
        instr_add(instr, i);
    }
}


int main(int argc, char** argv) {
    read_abc_bin();
    avm_initstack();
    top = AVM_STACKSIZE - total_globals - 2;
    topsp = AVM_STACKSIZE - total_globals - 2;

    while (!execution_finished) {
        execute_cycle();
    }

    return 0;
}
