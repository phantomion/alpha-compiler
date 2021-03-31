#ifndef _UTILITIES
#define _UTILITIES 0

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX 509
#define HASH_MULTIPLIER 65599
#define null NULL
#define LIBFUNC_COLLISION -2
#define COLLISION -3
#define NOT_ACCESSIBLE -4
#define FORMAL_ARGUMENT 2
#define VARS 3
#define USER_FUNC 4
#define LIB_FUNC 5


typedef struct Variable {
    char* name;
    unsigned int scope;
    unsigned int line;
}Variable;


typedef struct Function {
    char* name;
    char* args;
    unsigned int scope;
    unsigned int line;
}Function;


enum SymbolType {
    GLOBAL,
    LOCALVAR,
    VAR,
    FORMAL,
    USERFUNC,
    LIBFUNC
};


typedef struct SymbolTableEntry {
    short int isActive;
    Variable* varVal;
    Function* funcVal;
    enum SymbolType type;
    struct SymbolTableEntry* next;
    struct SymbolTableEntry* next_in_scope;
}SymbolTableEntry;

typedef struct Lvalue {
    int intValue;
    double doubleValue;
    char* stringValue;
    SymbolTableEntry* entry;
}Lvalue;


char* itoa(int val);
int check_for_libfunc(const char* name);
char* get_type(enum SymbolType type);
Lvalue* new_lvalue(int intValue, double doubleValue, char* stringValue, SymbolTableEntry* entry);

int hide_scope(const unsigned int scope);
int scope_contains(const char* name, const unsigned int scope);
SymbolTableEntry* scope_get(const char* name, const unsigned int scope);

unsigned int symtable_hash(const char *pcKey);
int symtable_insert(Variable* var, Function* func, enum SymbolType type);
SymbolTableEntry* symtable_get(const char* name, enum SymbolType type);
int symtable_lookup(const char* name, enum SymbolType type);

void initialize_libfuncs();
void print_scopes();

int is_func(Lvalue* lvalue);

#endif
