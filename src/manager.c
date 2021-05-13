#include "manager.h"
#include "icode.h"
#include "utilities.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

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

int yy_alphaerror(const char* message) {
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

    if(scope > 0) {
        switch (symtable_insert(id, LOCALVAR)) {
            case LIBFUNC_COLLISION:
                yy_alphaerror("Trying to shadow library function");
                return newexpr(nil_e);
                break;
            case VARS: {
                symbol* sym = symtable_get(id, LOCALVAR);
                if (sym) return lvalue_expr(sym);
                else return lvalue_expr(symtable_get(id, VAR));
                break;
            }
            default:
                return lvalue_expr(symtable_get(id, LOCALVAR));
        }
    }
    symtable_insert(id, GLOBAL);

    return lvalue_expr(scope_get(id, 0));
}


expr* manage_function(char *id) {

    int code = symtable_insert(id, USERFUNC);

    symbol* func = null;
    if (code == LIBFUNC_COLLISION) {
        yy_alphaerror("Trying to shadow libfunc");
    }
    else if (code == COLLISION) {
        yy_alphaerror("Symbol redefinition");
    }
    else {
        func = symtable_get(id, USERFUNC);
        func->func_addr = curr_quad;
    }

    save_functionlocal_offset();
    reset_formalarg_offset();
    funcdef_counter++;

    expr* new_lvalue = lvalue_expr(func);
    emit(funcstart, null, null, new_lvalue, curr_quad, yylineno);

    return new_lvalue;
}

char* new_anonymous_function() {
    anonymous_functions++;
    char* name = malloc(1 + strlen(itoa(anonymous_functions)));
    strcat(name, "$");
    strcat(name, itoa(anonymous_functions));
    return name;
}

expr* manage_function_exit(expr* func, int locals) {
    funcdef_counter--;
    reset_functionlocal_offset();
    exit_scopespace();

    if (func->sym) {
        func->sym->total_locals = locals;
    }

    emit(funcend, null, null, func, curr_quad, yylineno);
    return func;
}


expr* manage_add(expr* arg1, expr* arg2) {

    check_arith(arg1, "Cannot add non-arithmetic value");
    check_arith(arg2, "Cannot add non-arithmetic value");
    expr* new = newexpr(arithexpr_e);
    new->num_const = arg1->num_const + arg2->num_const;
    new->sym = new_temp();

    emit(add, arg1, arg2, new, curr_quad, yylineno);
    return new;
}


expr* manage_sub(expr* arg1, expr* arg2) {

    check_arith(arg1, "Cannot subtract non-arithmetic value");
    check_arith(arg2, "Cannot subtract non-arithmetic value");

    expr* new = newexpr(arithexpr_e);
    new->num_const = arg1->num_const - arg2->num_const;
    new->sym = new_temp();

    emit(sub, arg1, arg2, new, curr_quad, yylineno);
    return new;
}


expr* manage_mul(expr* arg1, expr* arg2) {

    check_arith(arg1, "Cannot multiply non-arithmetic value");
    check_arith(arg2, "Cannot multiply non-arithmetic value");

    expr* new = newexpr(arithexpr_e);
    new->num_const = arg1->num_const * arg2->num_const;
    new->sym = new_temp();

    emit(mul, arg1, arg2, new, curr_quad, yylineno);
    return new;
}


expr* manage_div(expr* arg1, expr* arg2) {

    check_arith(arg1, "Cannot divide non-arithmetic value");
    check_arith(arg2, "Cannot divide non-arithmetic value");

    expr* new = newexpr(arithexpr_e);
    new->num_const = arg1->num_const / arg2->num_const;
    new->sym = new_temp();

    emit(div_i, arg1, arg2, new, curr_quad, yylineno);
    return new;
}


expr* manage_mod(expr* arg1, expr* arg2) {

    check_arith(arg1, "Cannot use modulo on non-arithmetic value");
    check_arith(arg2, "Cannot use modulo on non-arithmetic value");

    expr* new = newexpr(arithexpr_e);
    new->num_const = (int)arg1->num_const % (int)arg2->num_const;
    new->sym = new_temp();

    emit(mod, arg1, arg2, new, curr_quad, yylineno);
    return new;
}


expr* manage_uminus(expr* ex) {
    check_arith(ex, "Cannot use unary minus on non-arithmetic value");
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
    check_arith(ex, "Cannot increment non-arithmetic value");
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
    check_arith(ex, "Cannot increment non-arithmetic value");
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
    check_arith(ex, "Cannot decrement non-arithmetic value");
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
    check_arith(ex, "Cannot decrement non-arithmetic value");
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

    if (lvalue && lvalue->type == tableitem_e) {
        emit(tablesetelem, lvalue, lvalue->index, ex, curr_quad, yylineno);
        expr* new = emit_iftableitem(lvalue);
        new->type = assignexpr_e;
        return new;
    }

    emit(assign, ex, null, lvalue, curr_quad, yylineno);

    expr* new = newexpr(assignexpr_e);
    new->sym = new_temp();
    new->num_const = ex->num_const;

    emit(assign, lvalue, null, new, curr_quad, yylineno);
    return new;
}

void manage_stmtlist(stmt_t* stmt, stmt_t* stmts) {
    stmts->breaklist = mergelist(stmts->breaklist, stmt->breaklist);
    stmts->contlist = mergelist(stmts->contlist, stmt->contlist);
}

void manage_break(stmt_t* break_quad) {
    make_stmt(break_quad);
    break_quad->breaklist = newlist(curr_quad);
    emit(jump, null, null, null, 0, yylineno);
}


void manage_continue(stmt_t *cont_quad) {
    make_stmt(cont_quad);
    cont_quad->contlist = newlist(curr_quad);
    emit(jump, null, null, null, 0, yylineno);
}

unsigned int manage_ifprefix(expr* ex) {
    emit(if_eq, ex, manage_bool(1), null, curr_quad + 2, yylineno);
    unsigned int quad = curr_quad;
    emit(jump, null, null, null, 0, yylineno);
    return quad;
}

void manage_ifstmt(unsigned int qq) {
    patchlabel(qq, curr_quad);
}

unsigned int manage_elseprefix() {
    unsigned int quad = curr_quad;
    emit(jump, null, null, null, 0, yylineno);
    return quad;
}

void manage_ifelse(unsigned int ifp_quad, unsigned int elsep_quad) {
    patchlabel(ifp_quad, elsep_quad+1);
    patchlabel(elsep_quad, curr_quad);
}

unsigned int manage_whilestart() {
    unsigned int quad = curr_quad;
    return quad;
}

unsigned int manage_whilecond(expr* ex) {
    emit(if_eq, ex, manage_bool(1), null, curr_quad + 2, yylineno);
    unsigned int quad = curr_quad;
    emit(jump, null, null, null, 0, yylineno);
    return quad;
}

void manage_whilestmt(unsigned int whilestart_quad, unsigned int whilecond_quad, stmt_t *stmt_quad) {
    emit(jump, null, null, null, whilestart_quad, yylineno);
    patchlabel(whilecond_quad, curr_quad);
    patchlist(stmt_quad->breaklist, curr_quad);
    patchlist(stmt_quad->contlist, whilestart_quad);
}

expr* manage_less(expr* arg1, expr* arg2) {

    check_arith(arg1, "Cannot use relative operators on non-arithmetic value");
    check_arith(arg2, "Cannot use relative operators on non-arithmetic value");
    expr* new = newexpr(boolexpr_e);
    new->sym = new_temp();
    new->bool_const = arg1->num_const < arg2->num_const;

    emit(if_less, arg1, arg2, null, curr_quad + 3, yylineno);
    emit(assign, manage_bool(0), null, new, curr_quad, yylineno);
    emit(jump, null, null, null, curr_quad + 2, yylineno);
    emit(assign, manage_bool(1), null, new, curr_quad, yylineno);
    return new;
}

expr* manage_lesseq(expr* arg1, expr* arg2) {
    check_arith(arg1, "Cannot use relative operators on non-arithmetic value");
    check_arith(arg2, "Cannot use relative operators on non-arithmetic value");

    expr* new = newexpr(boolexpr_e);
    new->sym = new_temp();
    new->bool_const = arg1->num_const <= arg2->num_const;

    emit(if_lesseq, arg1, arg2, null, curr_quad + 3, yylineno);
    emit(assign, manage_bool(0), null, new, curr_quad, yylineno);
    emit(jump, null, null, null, curr_quad + 2, yylineno);
    emit(assign, manage_bool(1), null, new, curr_quad, yylineno);
    return new;
}

expr* manage_greater(expr* arg1, expr* arg2) {
    check_arith(arg1, "Cannot use relative operators on non-arithmetic value");
    check_arith(arg2, "Cannot use relative operators on non-arithmetic value");

    expr* new = newexpr(boolexpr_e);
    new->sym = new_temp();
    new->bool_const = arg1->num_const > arg2->num_const;

    emit(if_greater, arg1, arg2, null, curr_quad + 3, yylineno);
    emit(assign, manage_bool(0), null, new, curr_quad, yylineno);
    emit(jump, null, null, null, curr_quad + 2, yylineno);
    emit(assign, manage_bool(1), null, new, curr_quad, yylineno);
    return new;
}

expr* manage_greatereq(expr* arg1, expr* arg2) {
    check_arith(arg1, "Cannot use relative operators on non-arithmetic value");
    check_arith(arg2, "Cannot use relative operators on non-arithmetic value");

    expr* new = newexpr(boolexpr_e);
    new->sym = new_temp();
    new->bool_const = arg1->num_const >= arg2->num_const;

    emit(if_greatereq, arg1, arg2, null, curr_quad + 3, yylineno);
    emit(assign, manage_bool(0), null, new, curr_quad, yylineno);
    emit(jump, null, null, null, curr_quad + 2, yylineno);
    emit(assign, manage_bool(1), null, new, curr_quad, yylineno);
    return new;
}

expr* manage_eq(expr* arg1, expr* arg2) {
    expr* new = newexpr(boolexpr_e);
    new->sym = new_temp();
    new->bool_const = arg1->num_const == arg2->num_const;

    emit(if_eq, arg1, arg2, null, curr_quad + 3, yylineno);
    emit(assign, manage_bool(0), null, new, curr_quad, yylineno);
    emit(jump, null, null, null, curr_quad + 2, yylineno);
    emit(assign, manage_bool(1), null, new, curr_quad, yylineno);
    return new;
}

expr* manage_neq(expr* arg1, expr* arg2) {
    expr* new = newexpr(boolexpr_e);
    new->sym = new_temp();
    new->bool_const = arg1->num_const != arg2->num_const;

    emit(if_noteq, arg1, arg2, null, curr_quad + 3, yylineno);
    emit(assign, manage_bool(0), null, new, curr_quad, yylineno);
    emit(jump, null, null, null, curr_quad + 2, yylineno);
    emit(assign, manage_bool(1), null, new, curr_quad, yylineno);
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

expr* manage_member_item(expr* lv, char* name) {
    if (is_func(lv)) yy_alphaerror("Cannot access member of function");

    lv = emit_iftableitem(lv);
    expr* new = newexpr(tableitem_e);
    new->sym = lv->sym;
    new->index = manage_string(name);

    return new;
}

expr* manage_array_item(expr* lv, expr* ex) {
    if (is_func(lv)) yy_alphaerror("Cannot access member of function");

    if (!lv) return newexpr(nil_e);

    lv = emit_iftableitem(lv);
    expr* new = newexpr(tableitem_e);
    new->sym = lv->sym;
    new->index = ex;

    return new;
}

expr* make_call(expr* lv, expr* reversed_elist) {
    expr* func = emit_iftableitem(lv);

    while (reversed_elist) {
        emit(param, null, null, reversed_elist, curr_quad, yylineno);
        reversed_elist = reversed_elist->next;
    }

    emit(call, null, null, func, curr_quad, yylineno);
    expr* result = newexpr(var_e);
    result->sym = new_temp();
    emit(getretval, null, null, result, curr_quad, yylineno);

    return result;
}

expr* manage_call_funcdef(expr* funcdef, expr* elist) {
    expr* func = newexpr(programfunc_e);
    func->sym = funcdef->sym;

    return make_call(func, elist);
}

struct call* manage_methodcall(char* id, expr* elist) {
    struct call* methodcall = calloc(1, sizeof(struct call));

    methodcall->elist = elist;
    methodcall->method = 1;
    methodcall->name = id;

    return methodcall;
}

struct call* manage_normcall(expr* elist) {
    struct call* normcall = calloc(1, sizeof(struct call));

    normcall->elist = elist;
    normcall->method = 0;
    normcall->name = null;

    return normcall;
}

expr* manage_call_lvalue(expr* lvalue, struct call* callsuffix) {
    lvalue = emit_iftableitem(lvalue);

    if (callsuffix->method) {
        expr* t = lvalue;
        lvalue = emit_iftableitem(manage_member_item(t, callsuffix->name));
        callsuffix->elist = insert_last(callsuffix->elist, t);
    }

    return make_call(lvalue, callsuffix->elist);
}

expr* manage_elist(expr* expr, struct expr* curr_list) {
    return insert_last(curr_list, expr);
}

expr* manage_tablemake(expr* elist) {
    expr* t = newexpr(newtable_e);
    int i = 0;

    t->sym = new_temp();
    emit(tablecreate, null, null, t, curr_quad, yylineno);

    while (elist) {
        emit(tablesetelem, manage_number(i++), elist, t, curr_quad, yylineno);
        elist = elist->next;
    }

    return t;
}

index_elem* manage_indexelem(expr* key, expr* value) {
    index_elem* elem = calloc(1, sizeof(index_elem));
    elem->key = key;
    elem->value = value;

    return elem;
}

index_elem* manage_indexelemlist(index_elem* node, index_elem* list) {
    if (!list) return node;

    node->next = list;
    return node;
}

expr* manage_mapmake(index_elem* list) {
    expr* t = newexpr(newtable_e);
    t->sym = new_temp();
    emit(tablecreate, null, null, t, curr_quad, yylineno);

    while(list) {
        emit(tablesetelem, list->key, list->value, t, curr_quad, yylineno);
        list = list->next;
    }

    return t;
}

void manage_return(expr* expr) {
    if (funcdef_counter == 0) {
        yy_alphaerror("Usage of return outside of function");
    }
    emit(ret, null, null, expr, curr_quad, yylineno);
}

void print_arg(expr* e) {
    switch (e->type) {
        case var_e:
        case programfunc_e:
        case assignexpr_e:
        case arithexpr_e:
        case boolexpr_e:
        case tableitem_e:
        case newtable_e:
        case libraryfunc_e:
            printf("%-10s", e->sym->name);
            break;
        case constnum_e:
            printf("%-10g",  e->num_const);
            break;
        case nil_e:
            printf("%-10s", "NIL");
            break;
        case constbool_e: {
            if (e->bool_const == true) {
                printf("%-10s", "TRUE");
            }
            else {
                printf("%-10s", "FALSE");
            }
            break;
        }
        case conststring_e:
            printf("%-10s", e->str_const);
            break;
        default:
            break;
    }
}

void print_quads() {
    int i = 0;
    printf("quad#\t%-15s%-10s%-10s%-10s%-10s\n", "opcode", "result", "arg1", "arg2", "label");
    printf("----------------------------------------------------------\n");
    for (i = 0; i < curr_quad; i++) {
        printf("%d:\t", i);
        printf("%-15s", opcodes[quads[i].op]);
        if (quads[i].result) {
            print_arg(quads[i].result);
        }
        else printf("%-10s", " ");
        if (quads[i].arg1) {
            print_arg(quads[i].arg1);
        }
        else printf("%-10s", " ");
        if (quads[i].arg2) {
            print_arg(quads[i].arg2);
        }
        else printf("%-10s", " ");
        if (quads[i].label != i) {
            printf("%-10d", quads[i].label);
        }
        else printf("%-10s", " ");
        printf("\n");
    }
}
