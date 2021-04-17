#include "icode.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

quad* quads = null;
unsigned int total = 0;
unsigned int curr_quad = 0;
int icode_phase = 1;

symbol* newtemp(){return null;}

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
    p->arg1 = arg1;
    p->arg2 = arg2;
    p->result = result;
    p->label = label;
    p->line = line;
}

expr* lvalue_expr(symbol* sym) {
    expr* e = calloc(1, sizeof(expr));
    e->next = null;
    e->sym = sym;

    if(!sym) return e;

    switch (sym->type) {
        case VAR:
            e->type = var_e;
            break;
        case LOCALVAR:
            e->type = var_e;
            break;
        case GLOBAL:
            e->type = var_e;
            break;
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

expr* newexpr_conststring(char* s) {
    expr* e = newexpr(conststring_e);
    e->str_const = strdup(s);
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
    result->sym = newtemp();
    emit(tablegetelem, e, e->index, result, 0, 0);

    return result;
}

int is_func(expr* e) {
    if (!e) return 0;

    switch (e->type) {
        case programfunc_e:
            return 1;
        case libraryfunc_e:
            return 1;
        default:
            return 0;
    }
}
