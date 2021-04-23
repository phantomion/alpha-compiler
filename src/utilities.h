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
#define GLOBAL_VAR 6
#define COLOR_RED "\x1b[31m"
#define COLOR_RESET "\x1b[0m"


enum symbol_t {
    GLOBAL,
    LOCALVAR,
    VAR,
    FORMAL,
    USERFUNC,
    LIBFUNC
};

enum scopespace_t {
    programvar,
    functionlocal,
    formalarg
};

typedef struct symbol {
    char* name;
    unsigned int scope;
    unsigned int line;
    unsigned int offset;
    unsigned int func_addr;
    unsigned int total_locals;
    short int isActive;
    enum scopespace_t space;
    enum symbol_t type;
    struct symbol* next;
    struct symbol* next_in_scope;
}symbol;

typedef struct functionoffset_stack {
    int localfunction_offset;
    struct functionoffset_stack* next;
}functionoffset_stack;


char* itoa(int val);
int check_for_libfunc(const char* name);
char* get_type(enum symbol_t type);

int hide_scope(const unsigned int scope);
int scope_contains(const char* name, const unsigned int scope);
symbol* scope_get(const char* name, const unsigned int scope);

unsigned int symtable_hash(const char *pcKey);
int symtable_insert(const char* name, enum symbol_t type);
symbol* symtable_get(const char* name, enum symbol_t type);
int symtable_lookup(const char* name, enum symbol_t type);

void initialize_libfuncs();
void enter_scopespace();

void exit_scopespace();
void reset_formalarg_offset();
void reset_functionlocal_offset();
void save_functionlocal_offset();

void function_push(int offset);
int function_pop();

#endif
