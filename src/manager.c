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
extern int is_not;

extern int icode_phase;
extern int yylineno;
extern char* yytext;
extern FILE* yyin;
extern FILE* yyout;
char* yyfile;

extern char* opcodes[];
extern quad* quads;

stack_node* func_head = null;
stack_node* ret_head = null;

int yy_alphaerror(const char* message) {
    icode_phase = 0;
    fprintf(yyout, "%s:%d: "COLOR_RED"error:"COLOR_RESET" %s at token %s\n", yyfile, yylineno, message, yytext);
    return 1;
}


void function_stack_push(int label) {
    stack_node* new = malloc(sizeof(stack_node));
    new->label = label;

    if(!func_head) {
        func_head = new;
        func_head->prev = null;
    }
    else {
        new->prev = func_head;
        func_head = new;
    }
}


void return_stack_push(int label) {
    stack_node* new = malloc(sizeof(stack_node));
    new->label = label;

    if(!ret_head) {
        ret_head = new;
        ret_head->prev = null;
    }
    else {
        new->prev = ret_head;
        ret_head = new;
    }
}


int function_stack_pop() {
    stack_node* temp = func_head;
    int label = temp->label;
    func_head = func_head->prev;
    free(temp);
    return label;
}


int return_stack_pop() {
    stack_node* temp = ret_head;
    int label = temp->label;
    ret_head = ret_head->prev;
    free(temp);
    return label;
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
                sym = symtable_get(id, VAR);
                if (sym) return lvalue_expr(symtable_get(id, VAR));
                sym = symtable_get(id, FORMAL);
                if (sym) return lvalue_expr(symtable_get(id, FORMAL));
                else return lvalue_expr(symtable_get(id, USERFUNC));
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

    emit(jump, null, null, null, 0, yylineno);
    function_stack_push(curr_quad-1);

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
    patchlabel(function_stack_pop(), curr_quad);

    while(ret_head) {
        patchlabel(return_stack_pop(), curr_quad-1);
    }

    return func;
}


symbol* manage_arith_relop_exprs(expr* arg1, expr* arg2) {
    if (istempexpr(arg1)) return arg1->sym;
    else if (istempexpr(arg2)) return arg2->sym;
    else return new_temp();
}


expr* manage_add(expr* arg1, expr* arg2) {

    check_arith(arg1, "Cannot add non-arithmetic value");
    check_arith(arg2, "Cannot add non-arithmetic value");
    expr* new = newexpr(arithexpr_e);
    new->sym = manage_arith_relop_exprs(arg1, arg2);

    emit(add, arg1, arg2, new, curr_quad, yylineno);
    return new;
}


expr* manage_sub(expr* arg1, expr* arg2) {

    check_arith(arg1, "Cannot subtract non-arithmetic value");
    check_arith(arg2, "Cannot subtract non-arithmetic value");

    expr* new = newexpr(arithexpr_e);
    new->sym = manage_arith_relop_exprs(arg1, arg2);

    emit(sub, arg1, arg2, new, curr_quad, yylineno);
    return new;
}


expr* manage_mul(expr* arg1, expr* arg2) {

    check_arith(arg1, "Cannot multiply non-arithmetic value");
    check_arith(arg2, "Cannot multiply non-arithmetic value");

    expr* new = newexpr(arithexpr_e);
    new->sym = manage_arith_relop_exprs(arg1, arg2);

    emit(mul, arg1, arg2, new, curr_quad, yylineno);
    return new;
}


expr* manage_div(expr* arg1, expr* arg2) {

    check_arith(arg1, "Cannot divide non-arithmetic value");
    check_arith(arg2, "Cannot divide non-arithmetic value");

    expr* new = newexpr(arithexpr_e);
    new->sym = manage_arith_relop_exprs(arg1, arg2);

    emit(div_i, arg1, arg2, new, curr_quad, yylineno);
    return new;
}


expr* manage_mod(expr* arg1, expr* arg2) {

    check_arith(arg1, "Cannot use modulo on non-arithmetic value");
    check_arith(arg2, "Cannot use modulo on non-arithmetic value");

    expr* new = newexpr(arithexpr_e);
    new->sym = manage_arith_relop_exprs(arg1, arg2);

    emit(mod, arg1, arg2, new, curr_quad, yylineno);
    return new;
}


expr* manage_uminus(expr* ex) {
    check_arith(ex, "Cannot use unary minus on non-arithmetic value");
    expr* new = newexpr(arithexpr_e);
    new->sym = istempexpr(ex) ? ex->sym : new_temp();

    emit(uminus, ex, null, new, curr_quad, yylineno);
    return new;
}


expr* manage_not(expr* ex) {
    manage_short_circuit(ex);
    if (!ex->falselist && !ex->truelist) {
        is_not = 1;
    }
    expr* new = newexpr(boolexpr_e);
    new->sym = istempexpr(ex) ? ex->sym : new_temp();
    new->falselist = ex->truelist;
    new->truelist = ex->falselist;

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
    expr* term = newexpr(var_e);
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
    expr* term = newexpr(var_e);
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


expr* create_short_circuit_assigns(expr* ex) {
    if (!ex->falselist && !ex->truelist) return ex;
    expr* new = newexpr(assignexpr_e);
    new->sym = new_temp();
    int truelist_quad = curr_quad;
    emit(assign, manage_bool(1), null, new, curr_quad, yylineno);
    emit(jump, null, null, null, curr_quad+2, yylineno);
    int falselist_quad = curr_quad;
    emit(assign, manage_bool(0), null, new, curr_quad, yylineno);

    patchlist(ex->truelist, truelist_quad);
    patchlist(ex->falselist, falselist_quad);
    return new;
}


expr* manage_assignexpr(expr* lvalue, expr* ex) {
    if (is_func(lvalue)) yy_alphaerror("Cannot assign to function");

    if (lvalue && lvalue->type == tableitem_e) {
        emit(tablesetelem, lvalue->index, ex, lvalue, curr_quad, yylineno);
        expr* new = emit_iftableitem(lvalue);
        new->type = assignexpr_e;
        return new;
    }

    ex = create_short_circuit_assigns(ex);

    emit(assign, ex, null, lvalue, curr_quad, yylineno);

    expr* new = newexpr(assignexpr_e);
    new->sym = new_temp();
    new->num_const = ex->num_const;

    emit(assign, lvalue, null, new, curr_quad, yylineno);
    return new;
}


unsigned int manage_ifprefix(expr* ex) {
    ex = create_short_circuit_assigns(ex);
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
    ex = create_short_circuit_assigns(ex);
    emit(if_eq, ex, manage_bool(1), null, curr_quad + 2, yylineno);
    unsigned int quad = curr_quad;
    emit(jump, null, null, null, 0, yylineno);
    return quad;
}


void manage_whilestmt(unsigned int whilestart_quad, unsigned int whilecond_quad, stmt_t* stmt) {
    emit(jump, null, null, null, whilestart_quad, yylineno);
    patchlabel(whilecond_quad, curr_quad);
    if (!stmt) return;
    patchlist(stmt->breaklist, curr_quad);
    patchlist(stmt->contlist, whilestart_quad);
}


expr* manage_relop(iopcode relop, expr* arg1, expr* arg2) {
    if (relop != if_eq && relop != if_noteq) {
        check_arith(arg1, "Invalid use of comparison operator on non-arithmetic expression");
        check_arith(arg2, "Invalid use of comparison operator on non-arithmetic expression");
    }

    expr* new = newexpr(boolexpr_e);
    new->sym = manage_arith_relop_exprs(arg1, arg2);
    new->truelist = curr_quad;
    new->falselist = curr_quad + 1;

    emit(relop, arg1, arg2, null, 0, yylineno);
    emit(jump, null, null, null, 0, yylineno);
    return new;
}


expr* manage_or(expr* arg1, expr* arg2, int M) {
    expr* new = newexpr(boolexpr_e);
    new->sym = manage_arith_relop_exprs(arg1, arg2);
    manage_short_circuit(arg2);
    patchlist(arg1->falselist, M);
    new->truelist = mergelist(arg1->truelist, arg2->truelist);
    new->falselist = arg2->falselist;

    return new;
}


expr* manage_and(expr* arg1, expr* arg2, int M) {
    expr* new = newexpr(boolexpr_e);
    new->sym = manage_arith_relop_exprs(arg1, arg2);
    patchlist(arg1->truelist, M);
    manage_short_circuit(arg2);
    new->truelist = arg2->truelist;
    new->falselist = mergelist(arg1->falselist, arg2->falselist);

    return new;
}


void manage_short_circuit(expr* ex) {
    if (ex->type == boolexpr_e) return;
    ex->truelist = curr_quad;
    ex->falselist = curr_quad + 1;
    if (is_not) {
        int tmp = ex->falselist;
        ex->falselist = ex->truelist;
        ex->truelist = tmp;
        is_not = 0;
    }
    emit(if_eq, ex, manage_bool(1), null, 0, yylineno);
    emit(jump, null, null, null, 0, yylineno);
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

    create_short_circuit_assigns(ex);

    lv = emit_iftableitem(lv);
    expr* new = newexpr(tableitem_e);
    new->sym = lv->sym;
    new->index = ex;

    return new;
}


expr* make_call(expr* lv, expr* reversed_elist) {
    expr* func = emit_iftableitem(lv);
    expr* result = newexpr(var_e);

    while (reversed_elist) {
        emit(param, null, null, reversed_elist, curr_quad, yylineno);
        if (!result->sym && istempexpr(reversed_elist)) {
            result->sym = reversed_elist->sym;
        }
        reversed_elist = reversed_elist->next;
    }

    emit(call, null, null, func, curr_quad, yylineno);
    if (!result->sym && istempexpr(lv)) {
        result->sym = lv->sym;
    }
    else {
        result->sym = new_temp();
    }
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
    create_short_circuit_assigns(value);
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


int manage_N() {
    int temp = curr_quad;

    emit(jump, null, null, null, 0, yylineno);

    return temp;
}


int manage_M() {
    return curr_quad;
}


for_stmt* manage_forprefix(int M, expr* ex) {
    create_short_circuit_assigns(ex);
    for_stmt* prefix = calloc(1, sizeof(for_stmt));
    prefix->test = M;
    prefix->enter = curr_quad;
    emit(if_eq, ex, manage_bool(1), null, 0, yylineno);
    return prefix;
}


void manage_forstmt(for_stmt* prefix, int N1, int N2, stmt_t* st, int N3) {
    --loop_counter;

    patchlabel(prefix->enter, N2+1);
    patchlabel(N1, curr_quad);
    patchlabel(N2, prefix->test);
    patchlabel(N3, N1+1);

    if(!st) return;
    patchlist(st->breaklist, curr_quad);
    patchlist(st->contlist, N1+1);
}


void manage_return(expr* expr) {
    if (funcdef_counter == 0) {
        yy_alphaerror("Invalid use of return outside of function");
    }
    if (expr) {
        expr = create_short_circuit_assigns(expr);
    }
    emit(ret, null, null, expr, curr_quad, yylineno);
    emit(jump, null, null, null, 0, yylineno);
    return_stack_push(curr_quad-1);
}


stmt_t* manage_break() {
    if (!loop_counter) yy_alphaerror("Invalid use of break outside of loop");
    stmt_t* new = calloc(1, sizeof(stmt_t));
    new->breaklist = newlist(curr_quad);
    emit(jump, null, null, null, 0, yylineno);
    return new;
}


stmt_t* manage_continue() {
    if (!loop_counter) yy_alphaerror("Invalid use of continue outside of loop");
    stmt_t* new = calloc(1, sizeof(stmt_t));
    new->contlist = newlist(curr_quad);
    emit(jump, null, null, null, 0, yylineno);
    return new;
}


stmt_t* manage_stmtlist(stmt_t* stmt_list, stmt_t* stmt) {
    if (!stmt_list && !stmt) return null;
    if (!stmt_list) return stmt;
    if (!stmt) return stmt_list;
    stmt_t* stmts = calloc(1, sizeof(stmt_t));
    stmts->breaklist = mergelist(stmt_list->breaklist, stmt->breaklist);
    stmts->contlist = mergelist(stmt_list->contlist, stmt->contlist);
    return stmts;
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
            fprintf(yyout, "%-10s", e->sym->name);
            break;
        case constnum_e:
            fprintf(yyout, "%-10g",  e->num_const);
            break;
        case nil_e:
            fprintf(yyout, "%-10s", "NIL");
            break;
        case constbool_e: {
            if (e->bool_const == true) {
                fprintf(yyout, "%-10s", "TRUE");
            }
            else {
                fprintf(yyout, "%-10s", "FALSE");
            }
            break;
        }
        case conststring_e:
            fprintf(yyout, "%-10s", e->str_const);
            break;
        default:
            break;
    }
}


void print_quads() {
    int i = 1;
    fprintf(yyout, "quad#\t%-15s%-10s%-10s%-10s%-10s%-10s\n", "opcode", "result", "arg1", "arg2", "label", "line");
    fprintf(yyout, "-------------------------------------------------------------------\n");
    for (i = 1; i < curr_quad; i++) {
        fprintf(yyout, "%d:\t", i);
        fprintf(yyout, "%-15s", opcodes[quads[i].op]);
        if (quads[i].result) {
            print_arg(quads[i].result);
        }
        else fprintf(yyout, "%-10s", " ");
        if (quads[i].arg1) {
            print_arg(quads[i].arg1);
        }
        else fprintf(yyout, "%-10s", " ");
        if (quads[i].arg2) {
            print_arg(quads[i].arg2);
        }
        else fprintf(yyout, "%-10s", " ");
        if (quads[i].label != i) {
            fprintf(yyout, "%-10d", quads[i].label);
        }
        else fprintf(yyout, "%-10s", " ");
        fprintf(yyout, "%-10d", quads[i].line);
        fprintf(yyout, "\n");
    }
}
