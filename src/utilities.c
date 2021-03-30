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
        "sin"};

Function* last_function;
SymbolTableEntry* symtable[MAX];
SymbolTableEntry* scope_link[MAX];


// Int to String implementation because C
// IT MAY BE USELESS WE'LL SEE
char* itoa(int val){

	static char buf[32] = {0};
	int i = 30;

	for(; val && i; --i, val /= 10)
		buf[i] = "0123456789abcdef"[val % 10];

	return &buf[i+1];
}

int check_for_libfunc(const char* name){
    for(int i = 0; i < 12; i++)
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

// Hide all entries in a scope
int hide_scope(const unsigned int scope) {

	SymbolTableEntry* scope_entry = scope_link[scope];

	while(scope_entry) {
        scope_entry->isActive = 0;
        scope_entry = scope_entry->next_in_scope;
	}

	return 1;
}

// Return true if a scope contains an entry
int scope_contains(const char* name, const unsigned int scope){

	SymbolTableEntry* scope_entry = scope_link[scope];

	while(scope_entry) {
        const char* e_name = scope_entry->varVal ? scope_entry->varVal->name : scope_entry->funcVal->name;

        if(strcmp(name, e_name) == 0 && scope_entry->isActive)
			    return 1;

		scope_entry = scope_entry->next_in_scope;
	}

	return 0;
}


// Return an entry from a scope
SymbolTableEntry* scope_lookup(const Variable* var, const Function* func, enum SymbolType type){

	SymbolTableEntry* scope_entry = scope_link[var ? var->scope : func->scope];

	while(scope_entry) {
        if(var && scope_entry->varVal){
            if(strcmp(var->name, scope_entry->varVal->name) == 0 && type == scope_entry->type)
			    return scope_entry;
        }
        else if(func && scope_entry->funcVal){
            if(strcmp(func->name, scope_entry->funcVal->name) == 0 && type == scope_entry->type)
			    return scope_entry;
        }

		scope_entry = scope_entry->next_in_scope;
	}

	return null;
}

// Hash Function
unsigned int symtable_hash(const char *pcKey) {

  	size_t ui;
  	unsigned int uiHash = 0U;

  	for (ui = 0U; pcKey[ui] != '\0'; ui++)
    	uiHash = uiHash * HASH_MULTIPLIER + pcKey[ui];

  	return uiHash%MAX;
}

// Insert an entry to the Symbol Table AND the Scope Link
// !!! Anything can be added except if every field (name, scope, type) is the same, then return error !!!
// ^^^^ That must be fixed once we understand the rules for definitions and redeclarations
int symtable_insert(Variable* var, Function* func, enum SymbolType type) {

    const char* name = var ? var->name : func->name;
    unsigned int hash = symtable_hash(name);


	SymbolTableEntry* table_entry = symtable[hash];
	SymbolTableEntry* scope_entry = scope_link[var ? var->scope : func->scope];
	SymbolTableEntry* table_prev = null;
	SymbolTableEntry* scope_prev = null;
	SymbolTableEntry* new;

	while(table_entry) {

        unsigned int scope = var ? var->scope : func->scope;
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
            if (symtable_lookup(name, 3)) return 1;
            for (int i = scope; i > 0; i--) {
                if (scope_contains(name, i)) {
                    if (symtable_lookup(name, 4)) return 1;
                    if (symtable_lookup(name, 5)) return 1;
                    if (funcdef_counter == 0) return 1;
                    else return NOT_ACCESSIBLE;
                }
            }
            if (scope_contains(name, 0)) return 1;
        }
        else if (type == 3) {
            if (scope_contains(name, scope - 1)) return 0;
            if (symtable_lookup(name, 5)) return 0;
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

	new = malloc(sizeof(struct SymbolTableEntry));
    new->isActive = 1;
    new->varVal = var;
    new->funcVal = func;
    new->type = type;
	new->next = null;
    new->next_in_scope = null;

	if   (table_prev) table_prev->next = new;
	else symtable[hash] = new;

	if   (scope_prev) scope_prev->next_in_scope = new;
	else scope_link[var ? var->scope : func->scope] = new;

	return 1;
}




// Return true if a bucket contains an entry
int symtable_contains(const Variable* var, const Function* func, enum SymbolType type){

	unsigned int hash = symtable_hash(var ? var->name : func->name);
	SymbolTableEntry* table_entry = symtable[hash];

	while(table_entry) {
        if(var && table_entry->varVal){
            if(strcmp(var->name, table_entry->varVal->name) == 0 && type == table_entry->type)
			    return 1;
        }
        else if(func && table_entry->funcVal){
            if(strcmp(func->name, table_entry->funcVal->name) == 0 && type == table_entry->type)
			    return 1;
        }

		table_entry = table_entry->next;
	}

	return 0;
}



// Return an entry from a bucket
int symtable_lookup(const char* name, enum SymbolType type){

	unsigned int hash = symtable_hash(name);
	SymbolTableEntry* table_entry = symtable[hash];

	while(table_entry) {
        if (table_entry->varVal) {
            if(strcmp(name, table_entry->varVal->name) == 0 && type == table_entry->type && table_entry->isActive)
                return 1;
        }
        else if (table_entry->funcVal) {
            if(strcmp(name, table_entry->funcVal->name) == 0 && type == table_entry->type && table_entry->isActive)
                return 1;
        }


		table_entry = table_entry->next;
	}

	return 0;
}

void initialize_libfuncs() {
    for (int i = 0; i < 12; i++) {
        Function* func = malloc(sizeof(Function));
        func->name = libfuncs[i];
        func->scope = 0;
        func->line = 0;
        func->args = null;
        symtable_insert(null, func, 5);
    }
}

