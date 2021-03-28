%{

    #include <stdio.h>
    #include <string.h>

    int yyerror(char* message);
    int yylex();
    int scope = 0;

    extern int yylineno;
    extern char* yytext;
    extern FILE* yyin;
    extern FILE* yyout;
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
        const char* args;
        unsigned int scope;
        unsigned int line;
    }Function;

    enum SymbolType {
        GLOBAL, LOCALVAR, FORMAL,
        USERFUNC, LIBFUNC
    };

    typedef struct SymbolTableEntry {
        short int isActive;
        union {
            Variable* varVal;
            Function* funcVal;
        }value;
        enum SymbolType type;
        struct SymbolTableEntry* next;
        struct SymbolTableEntry* next_in_scope;
    }SymbolTableEntry;

    SymbolTableEntry* symtable[MAX];
    SymbolTableEntry* scope_link[MAX];

    char* itoa(int val);
    unsigned int symtable_hash(const char *pcKey);
    int symtable_insert(Variable* var, Function* func, enum SymbolType type);
    int hide_scope(const unsigned int scope);
    int symtable_contains(const Variable* var, const Function* func, enum SymbolType type);
    int scope_contains(const Variable* var, const Function* func, enum SymbolType type);
    SymbolTableEntry* symtable_lookup(const Variable* var, const Function* func, enum SymbolType type);
    SymbolTableEntry* scope_lookup(const Variable* var, const Function* func, enum SymbolType type);

%}


%defines
%output "./src/parser.c"
%union {
    int intVal;
    char* strVal;
    double doubleVal;
    struct SymbolTableEntry* exprNode;
}

%expect 2

%start program

%token <strVal> ID
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
%token <intVal> NUMBER
%token <strVal> STRING
%token <doubleVal> REAL

%token ASSIGN
%token COMMENT
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
%token MUL_COMMENT

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
%left LCURLY RCURLY
%nonassoc EQUAL NEQ
%nonassoc GT GE LT LE

%%


program:    stmt_list;

stmt:       expr SEMICOLON
            | ifstmt
            | whilestmt
            | forstmt
            | returnstmt
            | BREAK SEMICOLON
            | CONTINUE SEMICOLON
            | block
            | funcdef
            | SEMICOLON
            | comment
            ;


stmt_list:  stmt stmt_list
            |
            ;


expr:       assignexpr
            | expr ADD expr
            | expr SUB expr
            | expr MUL expr
            | expr DIV expr
            | expr MOD expr
            | expr GT expr
            | expr GE expr
            | expr LT expr
            | expr LE expr
            | expr EQUAL expr
            | expr NEQ expr
            | expr AND expr
            | expr OR expr
            | term
            | error {yyclearin;}
            ;


term:       LPAREN expr RPAREN
            | primary
            | SUB expr
            | NOT expr
            | INC lvalue
            | lvalue INC
            | DEC lvalue
            | lvalue DEC
            ;


assignexpr: lvalue ASSIGN expr;


primary:    lvalue
            | call
            | objectdef
            | LPAREN funcdef RPAREN
            | const
            ;


lvalue:     ID {printf("id %s line %d\n", $1, yylineno);}
            | LOCAL ID {printf("Local id %s line %d\n", $2, yylineno);}
            | SCOPE ID {printf("Global id %s line %d\n", $2, yylineno);}
            | member
            ;


member:     lvalue POINT ID {printf("Member id %s at line %d\n", $3, yylineno);}
            | lvalue LBRACKET expr RBRACKET
            | call POINT ID
            | call LBRACKET expr RBRACKET
            ;


call:       call normcall
            | lvalue callsuffix
            | LPAREN funcdef RPAREN LPAREN elist RPAREN
            | libraryfuncs LPAREN elist RPAREN
            ;


callsuffix: normcall
            | methodcall
            ;


normcall:   LPAREN elist RPAREN;


methodcall: RANGE ID {printf("Method call %s at line %d\n", $2, yylineno);} LPAREN elist RPAREN;


elist:      expr commaexpr
            |
            ;


commaexpr:  COMMA expr commaexpr
            |
            ;


objectdef:  LBRACKET elist RBRACKET
            | LBRACKET indexed RBRACKET
            ;


indexed:    indexelem indexelemlist
            ;


indexelemlist:  COMMA indexelem indexelemlist
                |
                ;


indexelem:  LCURLY expr COLON expr RCURLY;


funcdef:    FUNCTION ID {printf("function with id %s line %d\n", $2, yylineno);} LPAREN idlist RPAREN block {
       //la8os h apo katw grammh
       /*printf("Function with id: %s\n", $2);*/
           /*Function* f = malloc(sizeof(struct Function));*/
           /*f->name = $2;*/
           /*f->scope = scope;*/
           /*f->line = yylineno;*/
           /*symtable_insert(null, f, USERFUNC);*/
       }
            | FUNCTION LPAREN idlist RPAREN block {}
            ;


block:      LCURLY {scope++;} stmt_list RCURLY {hide_scope(scope--);}
            | LCURLY RCURLY
            ;


libraryfuncs:   PRINT | INPUT | OBJECTMEMBERKEYS | OBJECTTOTALMEMBERS
                | OBJECTCOPY | TOTALARGUMENTS | ARGUMENT | TYPEOF
                | STRTONUM | SQRT | COS | SIN
                ;


const:      NUMBER | STRING | NIL | TRUE | FALSE | REAL;


idlist:     ID {printf("Argument %s at line %d\n", $1, yylineno);} commaidlist
            |
            ;


commaidlist: COMMA ID commaidlist
            |
            ;


ifstmt:     IF LPAREN expr RPAREN stmt
            | IF LPAREN expr RPAREN stmt ELSE stmt
            ;


whilestmt:  WHILE LPAREN expr RPAREN stmt;


forstmt:    FOR LPAREN elist SEMICOLON expr SEMICOLON elist RPAREN stmt;


returnstmt: RETURN expr SEMICOLON
          | RETURN SEMICOLON
          ;

comment: COMMENT
       | MUL_COMMENT
       ;


%%

int yyerror(char* message) {
    fprintf(yyout, "%s at line %d\n", message, yylineno);
    return 1;
}


// Int to String implementation because C
// IT MAY BE USELESS WE'LL SEE
char* itoa(int val){

	static char buf[32] = {0};
	int i = 30;

	for(; val && i; --i, val /= 10)
		buf[i] = "0123456789abcdef"[val % 10];

	return &buf[i+1];
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

    unsigned int hash = symtable_hash(var ? var->name : func->name);

	SymbolTableEntry* table_entry = symtable[hash];
	SymbolTableEntry* scope_entry = scope_link[var ? var->scope : func->scope];
	SymbolTableEntry* table_prev = null;
	SymbolTableEntry* scope_prev = null;
	SymbolTableEntry* new;

	new = malloc(sizeof(SymbolTableEntry));
    new->isActive = 1;
    new->value.varVal = var;
    new->value.funcVal = func;
    new->type = type;
	new->next = null;
    new->next_in_scope = null;

	while(table_entry) {

        if(var && table_entry->value.varVal){
            if(strcmp(var->name, table_entry->value.varVal->name) == 0 && var->scope == table_entry->value.varVal->scope && type == table_entry->type){
                free(new);
			    return 0;
            }
        }
        else if(func && table_entry->value.funcVal){
            if(strcmp(func->name, table_entry->value.funcVal->name) == 0 && func->scope == table_entry->value.funcVal->scope && type == table_entry->type){
                free(new);
			    return 0;
            }
        }

		table_prev = table_entry;
		table_entry = table_entry->next;
	}

	while(scope_entry) {

        if(var && scope_entry->value.varVal){
            if(strcmp(var->name, scope_entry->value.varVal->name) == 0 && var->scope == scope_entry->value.varVal->scope && type == scope_entry->type){
                free(new);
			    return 0;
            }
        }
        else if(func && scope_entry->value.funcVal){
            if(strcmp(func->name, scope_entry->value.funcVal->name) == 0 && func->scope == scope_entry->value.funcVal->scope && type == scope_entry->type){
                free(new);
			    return 0;
            }
        }

		scope_prev = scope_entry;
		scope_entry = scope_entry->next_in_scope;
	}

	if   (table_prev) table_prev->next = new;
	else symtable[hash] = new;

	if   (scope_prev) scope_prev->next_in_scope = new;
	else scope_link[var ? var->scope : func->scope] = new;

	return 1;
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


// Return true if a bucket contains an entry
int symtable_contains(const Variable* var, const Function* func, enum SymbolType type){

	unsigned int hash = symtable_hash(var ? var->name : func->name);
	SymbolTableEntry* table_entry = symtable[hash];

	while(table_entry) {
        if(var && table_entry->value.varVal){
            if(strcmp(var->name, table_entry->value.varVal->name) == 0 && type == table_entry->type)
			    return 1;
        }
        else if(func && table_entry->value.funcVal){
            if(strcmp(func->name, table_entry->value.funcVal->name) == 0 && type == table_entry->type)
			    return 1;
        }

		table_entry = table_entry->next;
	}

	return 0;
}


// Return true if a scope contains an entry
int scope_contains(const Variable* var, const Function* func, enum SymbolType type){

	SymbolTableEntry* scope_entry = scope_link[var ? var->scope : func->scope];

	while(scope_entry) {
        if(var && scope_entry->value.varVal){
            if(strcmp(var->name, scope_entry->value.varVal->name) == 0 && type == scope_entry->type)
			    return 1;
        }
        else if(func && scope_entry->value.funcVal){
            if(strcmp(func->name, scope_entry->value.funcVal->name) == 0 && type == scope_entry->type)
			    return 1;
        }

		scope_entry = scope_entry->next_in_scope;
	}

	return 0;
}


// Return an entry from a bucket
SymbolTableEntry* symtable_lookup(const Variable* var, const Function* func, enum SymbolType type){

	unsigned int hash = symtable_hash(var ? var->name : func->name);
	SymbolTableEntry* table_entry = symtable[hash];

	while(table_entry) {
        if(var && table_entry->value.varVal){
            if(strcmp(var->name, table_entry->value.varVal->name) == 0 && type == table_entry->type)
			    return table_entry;
        }
        else if(func && table_entry->value.funcVal){
            if(strcmp(func->name, table_entry->value.funcVal->name) == 0 && type == table_entry->type)
			    return table_entry;
        }

		table_entry = table_entry->next;
	}

	return null;
}


// Return an entry from a scope
SymbolTableEntry* scope_lookup(const Variable* var, const Function* func, enum SymbolType type){

	SymbolTableEntry* scope_entry = scope_link[var ? var->scope : func->scope];

	while(scope_entry) {
        if(var && scope_entry->value.varVal){
            if(strcmp(var->name, scope_entry->value.varVal->name) == 0 && type == scope_entry->type)
			    return scope_entry;
        }
        else if(func && scope_entry->value.funcVal){
            if(strcmp(func->name, scope_entry->value.funcVal->name) == 0 && type == scope_entry->type)
			    return scope_entry;
        }

		scope_entry = scope_entry->next_in_scope;
	}

	return null;
}
