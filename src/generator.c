#include "generator.h"

#include <assert.h>

#include "icode.h"

double* num_consts;
unsigned total_num_consts;
char** string_consts;
unsigned total_string_consts;
char** lib_funcs;
unsigned total_lib_funcs;
user_func* user_funcs;
unsigned total_user_funcs;

extern unsigned int total;
extern quad* quads;

instruction* instructions = null;
unsigned int total_instr = 1;
unsigned int curr_instr = 1;

void expand_instr() {
    assert(total_instr == curr_instr);
    instruction* i = malloc(NEW_INSTR_SIZE);
    if (instructions) {
        memcpy(i, instructions, CURR_INSTR_SIZE);
        free(instructions);
    }
    instructions = i;
    total_instr += EXPAND_SIZE;
}

void emit_instr(instruction* instr) {
    if (curr_instr == total) expand();
    instruction* temp = instructions + curr_instr++;
    memcpy(temp, instr, sizeof(struct instruction));
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
            arg->val = e->sym->func_addr;
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
