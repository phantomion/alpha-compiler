#include "generator.h"
#include "parser.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "icode.h"
#include "manager.h"

double* num_consts = null;
unsigned total_num_consts = 0;
char** string_consts = null;
unsigned total_string_consts = 0;
char** lib_funcs = null;
unsigned total_lib_funcs = 0;
user_func** user_funcs = null;
unsigned total_user_funcs = 0;

extern quad* quads;
extern FILE* yyout;

instruction** instructions = null;
unsigned int total_instr = 1;
unsigned int curr_instr = 1;

void print_instrs();

void expand_instr() {
    assert(total_instr == curr_instr);
    instruction** i = malloc(NEW_INSTR_SIZE);
    if (instructions) {
        memcpy(i, instructions, CURR_INSTR_SIZE);
        free(instructions);
    }
    instructions = i;
    total_instr += EXPAND_SIZE;
}

void emit_instr(instruction* instr) {
    if (curr_instr == total_instr) expand_instr();
    instructions[curr_instr++] = instr;
}

void generate(vmopcode op, quad* quad) {
    instruction* t = malloc(sizeof(instruction));
    t->arg1 = malloc(sizeof(vmarg));
    t->arg2 = malloc(sizeof(vmarg));
    t->result = malloc(sizeof(vmarg));
    t->opcode = op;
    make_operand(quad->arg1, t->arg1);
    make_operand(quad->arg2, t->arg2);
    make_operand(quad->result, t->result);
    quad->taddress = curr_instr;
    emit_instr(t);
}

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

    if (!string_consts) string_consts = calloc(1, total_string_consts * sizeof(char *));
    else string_consts = realloc(string_consts, total_string_consts * sizeof(char *));

    string_consts[total_string_consts - 1] = s;

    return total_string_consts - 1;
}


unsigned consts_newnumber(double n) {
    for (size_t i = 0; i < total_num_consts; i++) {
        if (num_consts[i] == n) return i;
    }

    total_num_consts++;

    if (!num_consts) num_consts = calloc(1, total_num_consts * sizeof(double));
    else num_consts = realloc(num_consts, total_num_consts * sizeof(double));

    num_consts[total_num_consts - 1] = n;

    return total_num_consts - 1;
}


unsigned libfuncs_newused(char* s) {
    for (size_t i = 0; i < total_lib_funcs; i++) {
        if (strcmp(lib_funcs[i], s) == 0) return i;
    }

    total_lib_funcs++;

    if (!lib_funcs) lib_funcs = calloc(1, total_lib_funcs * sizeof(char *));
    else lib_funcs = realloc(lib_funcs, total_lib_funcs * sizeof(char *));

    lib_funcs[total_lib_funcs - 1] = s;

    return total_lib_funcs - 1;
}


unsigned userfuncs_newfunc(symbol* sym) {
    total_user_funcs++;
    if (!user_funcs) user_funcs = calloc(1, total_user_funcs * sizeof(user_func*));
    else user_funcs = realloc(user_funcs, total_user_funcs * sizeof(user_func*));

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
    for (unsigned i = 1; i < curr_quad; ++i) {
        (*generators[quads[i].op])(quads + i);
    }
    print_instrs();
}

void generate_assign(quad* quad) {}


void generate_add(quad* quad) {
    generate(add_v, quad);
}


void generate_sub(quad* quad) {}
void generate_mul(quad* quad) {}
void generate_div(quad* quad) {}
void generate_mod(quad* quad) {}
void generate_uminus(quad* quad) {}
void generate_and(quad* quad) {}
void generate_or(quad* quad) {}
void generate_not(quad* quad) {}
void generate_if_eq(quad* quad) {}
void generate_if_noteq(quad* quad) {}
void generate_if_lesseq(quad* quad) {}
void generate_if_greatereq(quad* quad) {}
void generate_if_less(quad* quad) {}
void generate_if_greater(quad* quad) {}
void generate_jump(quad* quad){}
void generate_call(quad* quad){}
void generate_param(quad* quad){}
void generate_ret(quad* quad){}
void generate_getretval(quad* quad){}
void generate_funcstart(quad* quad){}
void generate_funcend(quad* quad){}
void generate_tablecreate(quad* quad){}
void generate_tablegetelem(quad* quad){}
void generate_tablesetelem(quad* quad){}

void print_instr_arg(vmarg* vma) {
    switch (vma->type) {
        case global_a:
        case formal_a:
        case local_a:
        case bool_a:
            fprintf(yyout, "%d:%-10d", vma->type, vma->val);
            break;
        case number_a:
            fprintf(yyout, "%d:%-10g", vma->type, num_consts[vma->val]);
            break;
        case nil_a:
            fprintf(yyout, "%-10d", vma->type);
            break;
        case string_a:
            fprintf(yyout, "%d:%-10s", vma->type, string_consts[vma->val]);
            break;
        case userfunc_a:
            fprintf(yyout, "%d:%-10s", vma->type, user_funcs[vma->val]->id);
            break;
        case libfunc_a:
            fprintf(yyout, "%d:%-10s", vma->type, lib_funcs[vma->val]);
            break;
        default:
            break;
    }
}


void print_instrs() {
    int i = 1;
    fprintf(yyout, "instr#\t%-15s%-10s%-10s%-10s%-10s%-10s\n", "opcode", "result", "arg1", "arg2", "label", "line");
    fprintf(yyout, "-------------------------------------------------------------------\n");
    for (i = 1; i < curr_instr; i++) {
        fprintf(yyout, "%d:\t", i);
        fprintf(yyout, "%-15d", instructions[i]->opcode);
        if (instructions[i]->result) {
            print_instr_arg(instructions[i]->result);
        }
        else fprintf(yyout, "%-10s", " ");
        if (instructions[i]->arg1) {
            print_instr_arg(instructions[i]->arg1);
        }
        else fprintf(yyout, "%-10s", " ");
        if (instructions[i]->arg2) {
            print_instr_arg(instructions[i]->arg2);
        }
        else fprintf(yyout, "%-10s", " ");
        fprintf(yyout, "\n");
    }
}
