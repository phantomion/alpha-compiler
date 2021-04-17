#ifndef _UTILITIES
#define _UTILITIES 0

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "parser.h"
extern int yylineno;
extern int scope;

#define MAX 509
#define HASH_MULTIPLIER 65599
#define null NULL
#define LIBFUNC_COLLISION -2
#define COLLISION -3
#define NOT_ACCESSIBLE -4
#define FORMAL_COLLISION -5
#define FORMAL_ARGUMENT 2
#define VARS 3
#define USER_FUNC 4
#define LIB_FUNC 5
#define COLOR_RED "\x1b[31m"
#define COLOR_RESET "\x1b[0m"


enum SymbolType {
    GLOBAL,
    LOCALVAR,
    VAR,
    FORMAL,
    USERFUNC,
    LIBFUNC
};

typedef struct symbol {
    char* name;
    unsigned int scope;
    unsigned int line;
    short int isActive;
    enum SymbolType type;
    struct symbol* next;
    struct symbol* next_in_scope;
}symbol;


char* itoa(int val);
int check_for_libfunc(const char* name);
char* get_type(enum SymbolType type);

int hide_scope(const unsigned int scope);
int scope_contains(const char* name, const unsigned int scope);
symbol* scope_get(const char* name, const unsigned int scope);

unsigned int symtable_hash(const char *pcKey);
int symtable_insert(const char* name, enum SymbolType type);
symbol* symtable_get(const char* name, enum SymbolType type);
int symtable_lookup(const char* name, enum SymbolType type);

void initialize_libfuncs();

#endif
