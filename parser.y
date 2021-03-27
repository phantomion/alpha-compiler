%{
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>

    int yyerror(char* message);
    int alpha_yylex();

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
%union {
    int intVal;
    char* strVal;
    double doubleVal;
    SymbolTableEntry* exprNode;
    int lineno;
}

%start program

%token ID
%token TRUE
%token FALSE
%token NIL
%token IF
%token ELSE
%token WHILE
%token FOR
%token FUNCTION
%token RETURN
%token BREAK
%token CONTINUE
%token LOCAL
%token AND
%token OR
%token NOT
%token NUMBER
%token STRING

%token ASSIGN
%token ADD
%token INC
%token SUB
%token DEC
%token MUL
%token DIV
%token MOD
%token EQUAL
%token NEQ
%token GT
%token LT
%token GE
%token LE

%token LCURLY
%token RCURLY
%token LBRACKET
%token RBRACKET
%token LPAREN
%token RPAREN
%token SEMICOLON
%token COMMA
%token COLON
%token SCOPE
%token POINT
%token RANGE
%token PRINT
%token INPUT
%token OBJECTMEMBERKEYS
%token OBJECTTOTALMEMBERS
%token OBJECTCOPY
%token TOTALARGUMENTS
%token ARGUMENT
%token TYPEOF
%token STRTONUM
%token SQRT
%token COS
%token SIN

%right ASSIGN
%right NOT INC DEC
%left COMMA
%left OR
%left AND
%left ADD SUB
%left MUL DIV MOD
%left POINT RANGE
%left LBRACKET RBRACKET
%left LPAREN RPAREN
%nonassoc EQUAL NEQ
%nonassoc GT GE LT LE
%precedence RCURLY

%%
program: stmt_list;

stmt: expr SEMICOLON
        | ifstmt
        | whilestmt
        | forstmt
        | returnstmt
        | BREAK SEMICOLON
        | CONTINUE SEMICOLON
        | block
        | funcdef
        | SEMICOLON
        ;

stmt_list: stmt stmt_list
         |
         ;

expr: assignexpr
        | expr op expr
        | term
        ;

op: ADD | SUB | MUL | DIV | MOD | GT | GE | LT | LE | EQUAL | NEQ | AND | OR;


term: RPAREN expr LPAREN
        | SUB expr
        | NOT expr
        | INC lvalue
        | lvalue INC
        | DEC lvalue
        | lvalue DEC
        | primary
        ;

assignexpr: lvalue ASSIGN expr;

primary: lvalue
        | call
        | objectdef
        | RPAREN funcdef LPAREN
        | const
        ;

lvalue: ID
        | LOCAL ID
        | SCOPE ID
        | member
        ;

member: lvalue POINT ID
        | lvalue LBRACKET expr RBRACKET
        | call POINT ID
        | call LBRACKET expr RBRACKET
        ;

call: call LPAREN elist RPAREN
        | lvalue callsuffix
        | LPAREN funcdef RPAREN LPAREN elist RPAREN
        ;

callsuffix: normcall
        | methodcall
        ;

normcall: LPAREN elist RPAREN;

methodcall: RANGE ID LPAREN elist RPAREN;

elist:  expr
        | expr commaexpr
        ;

commaexpr: COMMA expr commaexpr
         |
         ;

objectdef: LBRACKET elist RBRACKET
        | LBRACKET indexed RBRACKET
        ;

indexed: indexelem
        | indexelem indexelemlist
        ;

indexelemlist: COMMA indexelem indexelemlist
             |
             ;

indexelem: LCURLY expr COLON expr RCURLY;

funcdef: FUNCTION ID LPAREN idlist RPAREN block
       |FUNCTION LPAREN idlist RPAREN block
       | FUNCTION libraryfuncs LPAREN idlist RPAREN block
       ;

block: LCURLY stmt_list RCURLY
     | LCURLY RCURLY
     ;

libraryfuncs: PRINT | INPUT | OBJECTMEMBERKEYS | OBJECTTOTALMEMBERS
            | OBJECTCOPY | TOTALARGUMENTS | ARGUMENT | TYPEOF
            | STRTONUM | SQRT | COS | SIN

const: NUMBER | STRING | NIL | TRUE | FALSE;

idlist:ID
      | ID commaidlist
      |
      ;

commaidlist: COMMA ID commaidlist
           |
           ;

ifstmt: IF LPAREN expr RPAREN stmt
      | IF LPAREN expr RPAREN stmt ELSE stmt
      ;

whilestmt: WHILE LPAREN expr RPAREN stmt;

forstmt: FOR LPAREN elist SEMICOLON expr SEMICOLON elist RPAREN stmt;

returnstmt: RETURN LBRACKET expr RBRACKET SEMICOLON;

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
