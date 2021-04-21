#include "manager.h"
#include "icode.h"
#include "utilities.h"
#include <stdio.h>

extern int scope;
extern int funcdef_counter;
extern int anonymous_functions;
extern int loop_counter;

extern int icode_phase;
extern int yylineno;
extern char* yytext;
extern FILE* yyin;
extern FILE* yyout;

extern char* opcodes[];
extern quad* quads;

int yy_alphaerror(char* message) {
    icode_phase = 0;
    fprintf(yyout, COLOR_RED"Error:"COLOR_RESET" %s at token %s line %d\n", message, yytext, yylineno);
    return 1;
}

void manage_lvalue(expr* id) {
    if (is_func(id)) yy_alphaerror("Cannot use arithmetic operators on functions");
}


expr* manage_global_var(char* id) {
    if (!scope_contains(id, 0)) {
        yy_alphaerror("Global symbol not found");
        return newexpr(nil_e);
    }

    return lvalue_expr(scope_get(id, 0));
}


expr* manage_var(char *id) {

    int code = symtable_insert(id, VAR);

    switch (code) {
        case NOT_ACCESSIBLE:
            yy_alphaerror("Symbol not accessible");
            return newexpr(nil_e);
        case VARS: {
            symbol* symbol = symtable_get(id, VAR);
            if (symbol) {
                return lvalue_expr(symbol);
            }
            symbol = symtable_get(id, LOCALVAR);
            if (symbol) {
                return lvalue_expr(symbol);
            }
        }
        case FORMAL_ARGUMENT:
            return lvalue_expr(symtable_get(id, FORMAL));
        case USERFUNC:
            return lvalue_expr(symtable_get(id, USERFUNC));
        case LIBFUNC:
            return lvalue_expr(symtable_get(id, LIBFUNC));
        case GLOBAL_VAR:
            return lvalue_expr(scope_get(id, 0));
        default:
            return lvalue_expr(symtable_get(id, VAR));
    }
}


expr* manage_local_var(char *id) {

    enum symbol_t type;
    if(scope > 0) {
        type = LOCALVAR;
        if (symtable_insert(id, type) == LIBFUNC_COLLISION) {
            yy_alphaerror("Trying to shadow library function");
            return newexpr(nil_e);
        }
    }
    else {
        type = GLOBAL;
        symtable_insert(id, type);
    }

    return lvalue_expr(symtable_get(id, type));
}


void manage_function(char *id) {

    save_functionlocal_offset();
    funcdef_counter++;
    reset_formalarg_offset();

    int code = symtable_insert(id, USERFUNC);

    if (code == LIBFUNC_COLLISION) {
        yy_alphaerror("Trying to shadow libfunc");
    }
    else if (code == COLLISION) {
        yy_alphaerror("Symbol redefinition");
    }
}


void manage_anonymous_function() {
    anonymous_functions++;
    save_functionlocal_offset();
    reset_formalarg_offset();
    funcdef_counter++;

    char* name = malloc(1 + strlen(itoa(anonymous_functions)));
    strcat(name, "$");
    strcat(name, itoa(anonymous_functions));
    symtable_insert(name, 4);
}


void manage_function_exit() {
    funcdef_counter--;
    reset_functionlocal_offset();
    exit_scopespace();
}


expr* manage_add(expr* arg1, expr* arg2) {

    expr* new = newexpr(arithexpr_e);
    new->num_const = arg1->num_const + arg2->num_const;
    new->sym = new_temp();

    emit(add, arg1, arg2, new, curr_quad, yylineno);
    return new;
}


expr* manage_sub(expr* arg1, expr* arg2) {

    expr* new = newexpr(arithexpr_e);
    new->num_const = arg1->num_const - arg2->num_const;
    new->sym = new_temp();

    emit(sub, arg1, arg2, new, curr_quad, yylineno);
    return new;
}


expr* manage_mul(expr* arg1, expr* arg2) {

    expr* new = newexpr(arithexpr_e);
    new->num_const = arg1->num_const * arg2->num_const;
    new->sym = new_temp();

    emit(mul, arg1, arg2, new, curr_quad, yylineno);
    return new;
}


expr* manage_div(expr* arg1, expr* arg2) {

    expr* new = newexpr(arithexpr_e);
    new->num_const = arg1->num_const / arg2->num_const;
    new->sym = new_temp();

    emit(div_i, arg1, arg2, new, curr_quad, yylineno);
    return new;
}


expr* manage_mod(expr* arg1, expr* arg2) {

    expr* new = newexpr(arithexpr_e);
    new->num_const = (int)arg1->num_const % (int)arg2->num_const;
    new->sym = new_temp();

    emit(mod, arg1, arg2, new, curr_quad, yylineno);
    return new;
}


expr* manage_uminus(expr* ex) {
    expr* new = newexpr(arithexpr_e);
    new->num_const = -(ex->num_const);
    new->sym = new_temp();

    emit(uminus, ex, null, new, curr_quad, yylineno);
    return new;
}


expr* manage_not(expr* ex) {
    expr* new = newexpr(boolexpr_e);
    new->bool_const = !(ex->bool_const);
    new->sym = new_temp();

    emit(not_i, ex, null, new, curr_quad, yylineno);
    return new;
}


expr* manage_pre_inc(expr* ex) {
    expr* one = newexpr(constnum_e);
    one->num_const = 1;

    if(ex->type == tableitem_e) {
        expr* term = emit_iftableitem(ex);

        emit(add, term, one, term, curr_quad, yylineno);
        emit(tablesetelem, ex, ex->index, term, curr_quad, yylineno);

        return term;
    }
    else {
        emit(add, ex, one, ex, curr_quad, yylineno);

        expr* term = newexpr(arithexpr_e);
        term->sym = new_temp();

        emit(assign, ex, null, term, curr_quad, yylineno);

        return term;
    }
}


expr* manage_post_inc(expr* ex) {
    expr* term = newexpr(arithexpr_e);
    term->sym = new_temp();

    expr* one = newexpr(constnum_e);
    one->num_const = 1;

    if(ex->type == tableitem_e) {
        expr* val = emit_iftableitem(ex);

        emit(assign, val, null, term, curr_quad, yylineno);
        emit(add, val, one, val, curr_quad, yylineno);
        emit(tablesetelem, ex, ex->index, val, curr_quad, yylineno);
    }
    else {
        emit(assign, ex, null, term, curr_quad, yylineno);
        emit(add, ex, one, ex, curr_quad, yylineno);
    }
    return term;
}


expr* manage_pre_dec(expr* ex) {
    expr* one = newexpr(constnum_e);
    one->num_const = 1;

    if(ex->type == tableitem_e) {
        expr* term = emit_iftableitem(ex);

        emit(sub, term, one, term, curr_quad, yylineno);
        emit(tablesetelem, ex, ex->index, term, curr_quad, yylineno);

        return term;
    }
    else {
        emit(sub, ex, one, ex, curr_quad, yylineno);

        expr* term = newexpr(arithexpr_e);
        term->sym = new_temp();

        emit(assign, ex, null, term, curr_quad, yylineno);

        return term;
    }
}


expr* manage_post_dec(expr* ex) {
    expr* term = newexpr(arithexpr_e);
    term->sym = new_temp();

    expr* one = newexpr(constnum_e);
    one->num_const = 1;

    if(ex->type == tableitem_e) {
        expr* val = emit_iftableitem(ex);

        emit(assign, val, null, term, curr_quad, yylineno);
        emit(sub, val, one, val, curr_quad, yylineno);
        emit(tablesetelem, ex, ex->index, val, curr_quad, yylineno);
    }
    else {
        emit(assign, ex, null, term, curr_quad, yylineno);
        emit(sub, ex, one, ex, curr_quad, yylineno);
    }
    return term;
}


expr* manage_args(char *id) {
    short int code = symtable_insert(id, 3);
    if (code == LIBFUNC_COLLISION) {
        yy_alphaerror("Argument trying to shadow libfunc");
    }
    else if (code == COLLISION) {
        yy_alphaerror("Symbol redefinition by argument");
    }
    else if (code == FORMAL_COLLISION) {
        yy_alphaerror("Argument redefinition");
    }

    return lvalue_expr(symtable_get(id, 3));
}


expr* manage_real(double val) {
    expr* new = newexpr(constnum_e);
    new->num_const = val;
    return new;
}


expr* manage_bool(short int val) {
    expr* new = newexpr(constbool_e);
    new->bool_const = val ? true : false;
    return new;
}


expr* manage_nil() {
    expr* new = newexpr(nil_e);
    return new;
}


expr* manage_string(char *val) {
    expr* e = newexpr(conststring_e);
    e->str_const = strdup(val);
    return e;
}


expr* manage_number(int val) {
    expr* new = newexpr(constnum_e);
    new->num_const = val;
    return new;
}


expr* manage_assignexpr(expr* lvalue, expr* ex) {
    if (is_func(lvalue)) yy_alphaerror("Cannot assign to function");

    emit(assign, ex, null, lvalue, curr_quad, yylineno);

    expr* new = newexpr(assignexpr_e);
    new->sym = new_temp();
    new->num_const = ex->num_const;

    emit(assign, lvalue, null, new, curr_quad, yylineno);
    return new;
}

expr* manage_less(expr* arg1, expr* arg2) {

    expr* new = newexpr(boolexpr_e);
    new->sym = new_temp();
    new->bool_const = arg1->num_const < arg2->num_const;

    emit(if_less, arg1, arg2, new, curr_quad, yylineno);
    return new;
}

expr* manage_lesseq(expr* arg1, expr* arg2) {
    expr* new = newexpr(boolexpr_e);
    new->sym = new_temp();
    new->bool_const = arg1->num_const <= arg2->num_const;

    emit(if_lesseq, arg1, arg2, new, curr_quad, yylineno);
    return new;
}

expr* manage_greater(expr* arg1, expr* arg2) {
    expr* new = newexpr(boolexpr_e);
    new->sym = new_temp();
    new->bool_const = arg1->num_const > arg2->num_const;

    emit(if_greatereq, arg1, arg2, new, curr_quad, yylineno);
    return new;
}

expr* manage_greatereq(expr* arg1, expr* arg2) {
    expr* new = newexpr(boolexpr_e);
    new->sym = new_temp();
    new->bool_const = arg1->num_const >= arg2->num_const;

    emit(if_greatereq, arg1, arg2, new, curr_quad, yylineno);
    return new;
}

expr* manage_eq(expr* arg1, expr* arg2) {
    expr* new = newexpr(boolexpr_e);
    new->sym = new_temp();
    new->bool_const = arg1->num_const == arg2->num_const;

    emit(if_eq, arg1, arg2, new, curr_quad, yylineno);
    return new;
}

expr* manage_neq(expr* arg1, expr* arg2) {
    expr* new = newexpr(boolexpr_e);
    new->sym = new_temp();
    new->bool_const = arg1->num_const != arg2->num_const;

    emit(if_noteq, arg1, arg2, new, curr_quad, yylineno);
    return new;
}

expr* manage_or(expr* arg1, expr* arg2) {
    expr* new = newexpr(boolexpr_e);
    new->sym = new_temp();
    new->bool_const = arg1->num_const || arg2->num_const;

    emit(or_i, arg1, arg2, new, curr_quad, yylineno);
    return new;
}

expr* manage_and(expr* arg1, expr* arg2) {
    expr* new = newexpr(boolexpr_e);
    new->sym = new_temp();
    new->bool_const = arg1->num_const && arg2->num_const;

    emit(and_i, arg1, arg2, new, curr_quad, yylineno);
    return new;
}

void print_arg(expr* e) {
    switch (e->type) {
        case var_e:
        case programfunc_e:
        case assignexpr_e:
        case arithexpr_e:
        case boolexpr_e:
        case libraryfunc_e:
            printf("%-10s", e->sym->name);
            break;
        case constnum_e:
            printf("%-10g",  e->num_const);
            break;
        case nil_e:
            printf("%-10s", "NIL");
            break;
        case constbool_e:
            printf("%-10d", e->bool_const);
            break;
        case conststring_e:
            printf("%-10s", e->str_const);
            break;
        default:
            break;
    }
}

void print_quads() {
    int i = 0;
    printf("quad#\t%-15s%-12s%-10s%-10s\n", "opcode", "result", "arg1", "arg2");
    printf("----------------------------------------------------------\n");
    for (i = 0; i < curr_quad; i++) {
        if (!quads[i].result) break;
        printf("%d:\t", i);
        printf("%-15s", opcodes[quads[i].op]);
        printf("%-12s", quads[i].result->sym->name);
        if (quads[i].arg1) {
            print_arg(quads[i].arg1);
        }
        if (quads[i].arg2) {
            print_arg(quads[i].arg2);
        }
        printf("\n");
    }
}
