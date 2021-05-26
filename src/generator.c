#include "generator.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "icode.h"

double* num_consts = null;
unsigned total_num_consts = 0;
char** string_consts = null;
unsigned total_string_consts = 0;
char** lib_funcs = null;
unsigned total_lib_funcs = 0;
user_func** user_funcs = null;
unsigned total_user_funcs = 0;

extern unsigned int total;
extern quad* quads;

void make_operand(expr* e, vmarg* arg) {
    switch (e->type) {
        case var_e:
        case tableitem_e:
        case arithexpr_e:
        case boolexpr_e:
        case newtable_e: {
            assert(e->sym);
            arg->val = e->sym->offset;
            switch (e->sym->space) {
                case programvar:
                    arg->type = global_a;
                    break;
                case functionlocal:
                    arg->type = local_a;
                    break;
                case formalarg:
                    arg->type = formal_a;
                    break;
                default:
                    assert(0);
            }
            break;
        }
        case constbool_e: {
            arg->val = e->bool_const;
            arg->type = bool_a;
            break;
        }
        case conststring_e: {
            arg->val = consts_newstring(e->str_const);
            arg->type = string_a;
            break;
        }
        case constnum_e: {
            arg->val = consts_newnumber(e->num_const);
            arg->type = number_a;
            break;
        }
        case nil_e:
            arg->type = nil_a;
            break;
        case programfunc_e: {
            arg->type = userfunc_a;
            arg->val = userfuncs_newfunc(e->sym);
            break;
        }
        case libraryfunc_e: {
            arg->type = libfunc_a;
            arg->val = libfuncs_newused(e->sym->name);
            break;
        }
        default:
            assert(0);
    }
}

unsigned consts_newstring(char* s) {
    for (size_t i = 0; i < total_string_consts; i++) {
        if (strcmp(string_consts[i], s) == 0) return i;
    }

    total_string_consts++;

    if (!string_consts) string_consts = calloc(1, total_string_consts);
    else string_consts = realloc(string_consts, total_string_consts);

    string_consts[total_string_consts - 1] = s;

    return total_string_consts - 1;
}


unsigned consts_newnumber(double n) {
    for (size_t i = 0; i < total_num_consts; i++) {
        if (num_consts[i] == n) return i;
    }

    total_num_consts++;

    if (!num_consts) num_consts = calloc(1, total_num_consts);
    else num_consts = realloc(num_consts, total_num_consts);

    num_consts[total_num_consts - 1] = n;

    return total_num_consts - 1;
}


unsigned libfuncs_newused(char* s) {
    for (size_t i = 0; i < total_lib_funcs; i++) {
        if (strcmp(lib_funcs[i], s) == 0) return i;
    }

    total_lib_funcs++;

    if (!lib_funcs) lib_funcs = calloc(1, total_lib_funcs);
    else lib_funcs = realloc(lib_funcs, total_lib_funcs);

    lib_funcs[total_lib_funcs - 1] = s;

    return total_lib_funcs - 1;
}


unsigned userfuncs_newfunc(symbol* sym) {
    total_user_funcs++;
    if (!user_funcs) user_funcs = calloc(1, total_user_funcs);
    else user_funcs = realloc(user_funcs, total_user_funcs);

    user_funcs[total_user_funcs - 1] = calloc(1, sizeof(user_func));
    user_funcs[total_user_funcs - 1]->address = sym->func_addr;
    user_funcs[total_user_funcs - 1]->localSize = sym->total_locals;
    user_funcs[total_user_funcs - 1]->id = sym->name;

    return total_user_funcs - 1;
}


typedef void (*generator_func_t)(quad*);

generator_func_t generators[] = {
    generate_assign,       generate_add,          generate_sub,
    generate_mul,          generate_div,          generate_mod,
    generate_uminus,       generate_and,          generate_or,
    generate_not,          generate_if_eq,        generate_if_noteq,
    generate_if_lesseq,    generate_if_greatereq, generate_if_less,
    generate_if_greater,   generate_jump,         generate_call,
    generate_param,        generate_ret,          generate_getretval,
    generate_funcstart,    generate_funcend,      generate_tablecreate,
    generate_tablegetelem, generate_tablesetelem,
};

void generate_all() {
    for (unsigned i = 1; i < total; ++i) {
        (*generators[quads[i].op])(quads + i);
    }
}

void generate_assign(quad*);


void generate_add(quad* quad) {

}


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
