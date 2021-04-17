#include "utilities.h"

int scope = 0;
int funcdef_counter = 0;
int anonymous_functions = 0;
int loop_counter = 0;

char* libfuncs[] = {
    "print",
    "input",
    "objectmemberkeys",
    "objecttotalmembers",
    "objectcopy",
    "totalarguments",
    "argument",
    "typeof",
    "strtonum",
    "sqrt",
    "cos",
    "sin"
};

symbol* symtable[MAX];
symbol* scope_link[MAX];


char* itoa(int val) {

    static char buf[32] = {0};
    int i = 30;

    for(; val && i; --i, val /= 10)
        buf[i] = "0123456789abcdef"[val % 10];

    return &buf[i+1];
}


int check_for_libfunc(const char* name) {
    int i;
    for(i = 0; i < 12; i++)
        if(strcmp(name, libfuncs[i]) == 0)
            return 1;
    return 0;
}


char* get_type(enum SymbolType type) {
    switch (type) {
        case 0:
            return "global variable";
        case 1:
            return "local variable";
        case 2:
            return "local variable";
        case 3:
            return "formal argument";
        case 4:
            return "user function";
        case 5:
            return "library function";
    }
    return null;
}


int hide_scope(const unsigned int scope) {
    symbol* scope_entry = scope_link[scope];

    while(scope_entry) {
        scope_entry->isActive = 0;
        scope_entry = scope_entry->next_in_scope;
    }

    return 1;
}


int scope_contains(const char* name, const unsigned int scope) {
    symbol* scope_entry = scope_link[scope];

    while(scope_entry) {
        const char* e_name = scope_entry->name;

        if(strcmp(name, e_name) == 0 && scope_entry->isActive)
            return 1;

        scope_entry = scope_entry->next_in_scope;
    }

    return 0;
}


symbol* scope_get(const char* name, const unsigned int scope) {

    symbol* scope_entry = scope_link[scope];

    while(scope_entry) {
        const char* e_name = scope_entry->name;

        if(strcmp(name, e_name) == 0 && scope_entry->isActive)
            return scope_entry;

        scope_entry = scope_entry->next_in_scope;
    }

    return null;
}


unsigned int symtable_hash(const char *pcKey) {
    size_t ui;
    unsigned int uiHash = 0U;

    for (ui = 0U; pcKey[ui] != '\0'; ui++)
        uiHash = uiHash * HASH_MULTIPLIER + pcKey[ui];

    return uiHash%MAX;
}


int symtable_insert(const char* name, enum SymbolType type) {
    unsigned int hash = symtable_hash(name);

    symbol* table_entry = symtable[hash];
    symbol* scope_entry = scope_link[type == 3 ? scope + 1 : scope];
    symbol* table_prev = null;
    symbol* scope_prev = null;
    symbol* new;

    while(table_entry) {

        if(type == 0){
            if(scope_contains(name, 0))
                return 0;
        }
        else if(type == 1){
            if (scope_contains(name, scope))
                return 1;
            if (check_for_libfunc(name) && scope != 0)
                return LIBFUNC_COLLISION;
        }
        else if (type == 2) {

            if (symtable_lookup(name, 3)) return FORMAL_ARGUMENT;
            int i;
            for (i = scope; i > 0; i--) {
                if (scope_contains(name, i)) {
                    if (symtable_lookup(name, 4)) return USER_FUNC;
                    if (symtable_lookup(name, 5)) return LIB_FUNC;
                    if (funcdef_counter == 0) return VARS;
                    else return NOT_ACCESSIBLE;
                }
            }
            if (scope_contains(name, 0)) return 1;
        }
        else if (type == 3) {
            if (scope_contains(name, scope)) return COLLISION;
            if (scope_contains(name, scope + 1)) return FORMAL_COLLISION;
            if (symtable_lookup(name, 5)) return LIBFUNC_COLLISION;
        }
        else if (type == 4) {
            if (scope_contains(name, scope))
                return COLLISION;
            if (check_for_libfunc(name))
                return LIBFUNC_COLLISION;
        }

        table_prev = table_entry;
        table_entry = table_entry->next;
    }

    while(scope_entry) {
        scope_prev = scope_entry;
        scope_entry = scope_entry->next_in_scope;
    }

    new = malloc(sizeof(struct symbol));
    new->isActive = 1;
    new->name = strdup(name);
    if (type == 3) new->scope = scope + 1;
    else new->scope = scope;
    new->line = type == 5 ? 0 : yylineno;
    new->type = type;
    new->next = null;
    new->next_in_scope = null;

    if   (table_prev) table_prev->next = new;
    else symtable[hash] = new;

    if   (scope_prev) scope_prev->next_in_scope = new;
    else scope_link[type == 3 ? scope + 1 : scope] = new;

    return 1;
}


symbol* symtable_get(const char* name, enum SymbolType type) {

    unsigned int hash = symtable_hash(name);
    symbol* table_entry = symtable[hash];

    while(table_entry) {
        if(strcmp(name, table_entry->name) == 0 && type == table_entry->type && table_entry->isActive) {
            return table_entry;
        }
        table_entry = table_entry->next;
    }

    return null;

}


int symtable_lookup(const char* name, enum SymbolType type) {
    unsigned int hash = symtable_hash(name);
    symbol* table_entry = symtable[hash];

    while(table_entry) {
        if(strcmp(name, table_entry->name) == 0 && type == table_entry->type && table_entry->isActive) {
            return 1;
        }
        table_entry = table_entry->next;
    }

    return 0;
}


void initialize_libfuncs() {
    int i;
    for (i = 0; i < 12; i++) {
        symtable_insert(libfuncs[i], 5);
    }
}
