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

    enum SymbolType type;
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

    funcdef_counter++;
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

    char* name = malloc(1 + strlen(itoa(anonymous_functions)));
    strcat(name, "$");
    strcat(name, itoa(anonymous_functions));
    funcdef_counter++;
    symtable_insert(name, 4);
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

