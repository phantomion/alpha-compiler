%{
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>

    int yyerror(char* message);
    int alpha_yylex(alpha_token_t *yylval);

    extern int yylineno;
    extern char* yytext;
    extern FILE* yyin;
    #define MAX 509
    #define HASH_MULTIPLIER 65599
    #define null NULL

    typedef struct Variable {
        const char* name;
        unsigned int scope;
        unsigned int line;
    }Variable;

    typedef struct Function {
        const char* name;
        const char** args;
        unsigned int scope;
        unsigned int line;
    }Function;

    enum SymbolType {
        GLOBAL, LOCAL, FORMAL,
        USERFUNC, LIBFUNC
    };

    typedef struct SymbolTableEntry {
        const char* key;
        short int isActive;
        union {
            Variable* varVal;
            Function* funcVal;
        }value;
        enum SymbolType type;
        struct SymbolTableEntry* next;
    }SymbolTableEntry;

    SymbolTableEntry* symtable[MAX];

    unsigned int symTableHash(const char* key);
    int ins(const char *key, const void *pvValue);
    void* lookup(const char *key);
    void hide(const char* key);
%}

%defines
%output "./src/parser.c"

%start program

%token id

%right '='
%right "not" "++" "--"
%left ','
%left "or"
%left "and"
%left '+' '-'
%left '*' '/' '%'
%left '.' ".."
%left '[' ']'
%left '(' ')'
%nonassoc "==" "!="
%nonassoc '>' ">=" '<' "<="

%%
program: stmt_list;

stmt: expr ';'
        | ifstmt
        | whilestmt
        | forstmt
        | returnstmt
        | "break" ';'
        | "continue" ';'
        | block
        | funcdef
        | ';'
        |
        ;

stmt_list: stmt stmt_list
         |
         ;

expr: assignexpr
        | expr op expr
        | term
        ;

op: '+' | '-' | '*' | '/' | '%' | '>' | ">=" | '<' | "<=" | "==" | "!=" | "and"| "or";


term: '(' expr ')'
        | '-' expr
        | "not" expr
        | "++"lvalue
        | lvalue"++"
        | "--"lvalue
        | lvalue"--"
        | primary
        ;

assignexpr: lvalue '=' expr;

primary: lvalue
        | call
        | objectdef
        | '(' funcdef ')'
        | const
        ;

lvalue: id
        | "local" id
        | "::" id
        | member
        ;

member: lvalue '.' id
        | lvalue '[' expr ']'
        | call '.' id
        | call '[' expr ']'
        ;

call: call '(' elist ')'
        | lvalue callsuffix
        | '(' funcdef ')' '(' elist ')'
        ;

callsuffix: normcall
        | methodcall
        ;

normcall: '(' elist ')';

methodcall: ".." id '(' elist ')';

elist:  expr
        | expr commaexpr
        |
        ;

commaexpr: ',' expr commaexpr
         |
         ;

objectdef: '[' elist ']'
        | '[' indexed ']'
        ;

indexed: indexelem
        | indexelem indexelemlist
        |
        ;

indexelemlist: ',' indexelem indexelemlist
             |
             ;

indexelem: '{' expr ':' expr '}';

block: '{' stmt_list '}'
     | '{' '}'
     ;

funcdef: "function" [id] '(' idlist ')' block;

const: "number" | "string" | "nil" | "true" | "false";

idlist: id
      | id commaidlist
      |
      ;

commaidlist: ',' id commaidlist
           |
           ;

ifstmt: "if" '(' expr ')' stmt
      | "if" '(' expr ')' stmt "else" stmt
      ;

whilestmt: "while" '(' expr ')' stmt;

forstmt: "for" '(' elist';' expr';' elist')' stmt;

returnstmt: "return" '['expr']' ';';

%%

char* itoa(int val, int base){

	static char buf[32] = {0};
	int i = 30;

	for(; val && i ; --i, val /= base)
		buf[i] = "0123456789abcdef"[val % base];

	return &buf[i+1];
}


char* get_key(const Variable* var, const Function* func) {

    char* key;

    if(var){
        strcpy(key, var->name);
        strcat(key, itoa(var->scope, 10));
    }
    else if(func){
        strcpy(key, func->name);
        strcat(key, itoa(func->scope, 10));
    }

    return key;
}


unsigned int SymTable_hash(const char *pcKey) {

  	size_t ui;
  	unsigned int uiHash = 0U;

  	for (ui = 0U; pcKey[ui] != '\0'; ui++)
    	uiHash = uiHash * HASH_MULTIPLIER + pcKey[ui];

  	return uiHash%MAX;
}


int SymTable_put(Variable* var, Function* func, enum SymbolType type) {

    unsigned int hash = SymTable_hash(get_key(var, func));

	SymbolTableEntry* binding = symtable[hash];
	SymbolTableEntry* prev = null;
	SymbolTableEntry* new;

	new = malloc(sizeof(SymbolTableEntry));
    new->isActive = 1;
    new->value.varVal = var;
    new->value.funcVal = func;
    new->type = type;
	new->next = null;

	while(binding) {
        unsigned int binding_hash = SymTable_hash(get_key(binding->value.varVal, binding->value.funcVal));

        if(binding_hash == hash){

            if(binding->isActive) return 0;
            new->next = binding->next;
            free(binding);
        }

		prev = binding;
		binding = binding->next;
	}

	if (prev) prev->next = new;
	else symtable[hash] = new;

	return 1;
}


int SymTable_hide(const Variable* var, const Function* func) {

    int hash = SymTable_hash(get_key(var, func));
	SymbolTableEntry* binding = symtable[hash];

	while(binding) {
        char* binding_key = get_key(binding->value.varVal, binding->value.funcVal);

        if(SymTable_hash(binding_key) == hash){
			binding->isActive = 0;
			return 1;
		}

        binding = binding->next;
	}

	return 0;
}


int SymTable_contains(const Variable* var, const Function* func){

	unsigned int hash = SymTable_hash(get_key(var, func));
	SymbolTableEntry* binding = symtable[hash];

	while (binding) {
        char* binding_key = get_key(binding->value.varVal, binding->value.funcVal);

        if(SymTable_hash(binding_key) == hash)
			return 1;

		binding = binding->next;
	}

	return 0;
}


SymbolTableEntry* SymTable_get(const Variable* var, const Function* func) {

	unsigned int hash = SymTable_hash(get_key(var, func));
	SymbolTableEntry* binding = symtable[hash];

	while (binding) {
        char* binding_key = get_key(binding->value.varVal, binding->value.funcVal);

        if(SymTable_hash(binding_key) == hash)
			return binding;

		binding = binding->next;
	}

	return null;
}
