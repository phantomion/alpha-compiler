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
    }SymbolTableEntry;

    SymbolTableEntry* symtable[MAX];

    char* itoa(int val);
    char* get_key(const Variable* var, const Function* func);
    unsigned int symtable_hash(const char *pcKey);
    int symtable_insert(Variable* var, Function* func, enum SymbolType type);
    int symtable_hide(const unsigned int scope);
    int symtable_contains(const Variable* var, const Function* func);
    SymbolTableEntry* symtable_lookup(const Variable* var, const Function* func);
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


block:      LCURLY {scope++;} stmt_list RCURLY {symtable_hide(scope--);}
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

char* itoa(int val){

	static char buf[32] = {0};
	int i = 30;

	for(; val && i; --i, val /= 10)
		buf[i] = "0123456789abcdef"[val % 10];

	return &buf[i+1];
}


char* get_key(const Variable* var, const Function* func) {

    char* key = malloc(strlen(var->name) + strlen(itoa(var->scope)));

    if(var){
        strcpy(key, var->name);
        strcat(key, itoa(var->scope));
    }
    else if(func){
        strcpy(key, func->name);
        strcat(key, itoa(func->scope));
    }

    return key;
}


unsigned int symtable_hash(const char *pcKey) {

  	size_t ui;
  	unsigned int uiHash = 0U;

  	for (ui = 0U; pcKey[ui] != '\0'; ui++)
    	uiHash = uiHash * HASH_MULTIPLIER + pcKey[ui];

  	return uiHash%MAX;
}


int symtable_insert(Variable* var, Function* func, enum SymbolType type) {

    unsigned int hash = symtable_hash(get_key(var, func));

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
        unsigned int binding_hash = symtable_hash(get_key(binding->value.varVal, binding->value.funcVal));

        if(binding_hash == hash){

            if(binding->isActive)
                return 0;

            new->next = binding->next;
            free(binding->value.varVal);
            free(binding->value.funcVal);
            free(binding);
        }

		prev = binding;
		binding = binding->next;
	}

	if (prev) prev->next = new;
	else symtable[hash] = new;

	return 1;
}


int symtable_hide(const unsigned int scope) {

    SymbolTableEntry* binding;

    for(short int i = 0; i < MAX; i++){
        binding = symtable[i];

	    while(binding) {

            if(binding->value.varVal){
                if (binding->value.varVal->scope == scope)
                    binding->isActive = 0;
            }
            else if(binding->value.funcVal){
                if (binding->value.funcVal->scope == scope)
                    binding->isActive = 0;
            }

            binding = binding->next;
	    }
    }

	return 1;
}


int symtable_contains(const Variable* var, const Function* func){

	unsigned int hash = symtable_hash(get_key(var, func));
	SymbolTableEntry* binding = symtable[hash];

	while (binding) {
        char* binding_key = get_key(binding->value.varVal, binding->value.funcVal);

        if(symtable_hash(binding_key) == hash)
			return 1;

		binding = binding->next;
	}

	return 0;
}


SymbolTableEntry* symtable_lookup(const Variable* var, const Function* func) {

	unsigned int hash = symtable_hash(get_key(var, func));
	SymbolTableEntry* binding = symtable[hash];

	while (binding) {
        char* binding_key = get_key(binding->value.varVal, binding->value.funcVal);

        if(symtable_hash(binding_key) == hash)
			return binding;

		binding = binding->next;
	}

	return null;
}
