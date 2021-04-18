#include "icode.h"
#include "utilities.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

quad* quads = null;
unsigned int total = 0;
unsigned int curr_quad = 0;
unsigned int temp_counter = 1;
int icode_phase = 1;

char* opcodes[] = {
    "assign", "add", "sub", "mul", "div_i", "mod", "uminus", "and_i", "or_i", "not_i",
    "if_eq", "if_noteq", "if_lesseq", "if_greatereq", "if_less", "if_greater",
    "jump", "call", "param", "ret", "getretval", "funcstart", "funcend",
    "tablecreate", "tablegetelem", "tablesetelem"
};

char* new_temp_name() {

    char* counter = itoa(temp_counter);
    char* temp = calloc(1, 2 + strlen(counter));
    strcat(temp, "_t");
    strcat(temp, counter);

    return temp;
}

void reset_temp() {
    temp_counter = 1;
}

symbol* new_temp(){
    char* name = new_temp_name();
    symbol* sym = symtable_get(name, LOCALVAR);

    temp_counter++;
    if(sym == null) {
        if(symtable_insert(name, LOCALVAR) > 0) {
            return symtable_get(name, LOCALVAR);
        }
        return null;
    }
    return sym;
}

void expand() {
    assert(total == curr_quad);
    quad* p = malloc(NEW_SIZE);
    if (quads) {
        memcpy(p, quads, CURR_SIZE);
        free(quads);
    }
    quads = p;
    total += EXPAND_SIZE;
}

void emit(iopcode op,
          expr* arg1,
          expr* arg2,
          expr* result,
          unsigned int label,
          unsigned int line
        ) {
    if (curr_quad == total) expand();

    quad* p = quads + curr_quad++;
    p->op = op;
    p->arg1 = arg1;
    p->arg2 = arg2;
    p->result = result;
    p->label = label;
    p->line = line;
}

expr* lvalue_expr(symbol* sym) {
    expr* e = calloc(1, sizeof(expr));
    e->sym = sym;

    if(!sym) return e;

    switch (sym->type) {
        case VAR:
        case LOCALVAR:
        case GLOBAL:
        case FORMAL:
            e->type = var_e;
            break;
        case USERFUNC:
            e->type = programfunc_e;
            break;
        case LIBFUNC:
            e->type = libraryfunc_e;
            break;
        default:
            assert(0);
    }
    return e;
}

expr* newexpr(expr_t type) {
    expr* e = calloc(1, sizeof(expr));
    e->type = type;
    return e;
}


/*auto dn douleuei akoma exei o 8eos*/
/*PROSOXH RE MALAKES*/
/*8ELEI ALLAGES*/
expr* emit_iftableitem(expr* e) {
    if (e->type != tableitem_e) {
        return e;
    }
    expr* result = newexpr(var_e);
    result->sym = new_temp();
    emit(tablegetelem, e, e->index, result, 0, 0);

    return result;
}

int is_func(expr* e) {
    if (!e) return 0;

    switch (e->type) {
        case programfunc_e:
        case libraryfunc_e:
            return 1;
        default:
            return 0;
    }
}

int is_num(expr* e) {
    if (!e) return 0;

    switch (e->type) {
        case arithexpr_e:
        case constnum_e:
            return 1;
        default:
            return 0;
    }
}
