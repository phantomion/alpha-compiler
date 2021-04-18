#include "manager.h"
#include "icode.h"
#include "utilities.h"

extern int scope;
extern int funcdef_counter;
extern int anonymous_functions;
extern int loop_counter;

extern int icode_phase;
extern int yylineno;
extern char* yytext;
extern FILE* yyin;
extern FILE* yyout;

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
        return null;
    }

    return lvalue_expr(scope_get(id, 0));
}


expr* manage_var(char *id) {

    int code = symtable_insert(id, 2);

    if (code == NOT_ACCESSIBLE) {
        yy_alphaerror("Symbol not accessible");
        return null;
    }
    else if (code == VARS) {
        symbol* symbol = symtable_get(id, 2);
        if (symbol) {
            return lvalue_expr(symbol);
        }
        symbol = symtable_get(id, 1);
        if (symbol) {
            return lvalue_expr(symbol);
        }
    }
    else if (code == FORMAL_ARGUMENT) {
        return lvalue_expr(symtable_get(id, 3));
    }
    else if (code == USER_FUNC) {
        return lvalue_expr(symtable_get(id, 4));
    }
    else if (code == LIB_FUNC) {
        return lvalue_expr(symtable_get(id, 5));
    }
    else {
        return lvalue_expr(symtable_get(id, 0));
    }

    return null;
}


expr* manage_local_var(char *id) {

    enum symbol_t type;
    if(scope > 0) {
        type = LOCALVAR;
        if (symtable_insert(id, type) == LIBFUNC_COLLISION) {
            yy_alphaerror("Trying to shadow library function");
            return null;
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
    new->index = null;
    new->next = null;
    return new;
}


expr* manage_bool(short int val) {
    expr* new = newexpr(constbool_e);
    new->bool_const = val ? true : false;
    new->index = null;
    new->next = null;
    return new;
}


expr* manage_nil() {
    expr* new = newexpr(nil_e);
    new->index = null;
    new->next = null;
    return new;
}


expr* manage_string(char *val) {
    expr* e = newexpr(conststring_e);
    e->str_const = strdup(val);
    e->index = null;
    e->next = null;
    return e;
}


expr* manage_number(int val) {
    expr* new = newexpr(constnum_e);
    new->num_const = val;
    new->index = null;
    new->next = null;
    return new;
}


expr* manage_assignexpr(expr* lvalue, expr* ex) {
    if (is_func(lvalue)) yy_alphaerror("Cannot assign to function");

    emit(assign, ex, null, lvalue, curr_quad, yylineno);

    expr* new = newexpr(assignexpr_e);
    new->sym = new_temp();
    new->index = null;
    new->next = null;

    emit(assign, lvalue, null, new, curr_quad, yylineno);
    return new;
}