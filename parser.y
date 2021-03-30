%{

    #include <stdio.h>
    #include <string.h>
    #include <stdlib.h>

    int yyerror(char* message);
    int yylex();
    int scope = 0;
    int funcdef_counter = 0;
    int anonymous_functions = 0;
    int loop_counter = 0;

    extern int yylineno;
    extern char* yytext;
    extern FILE* yyin;
    extern FILE* yyout;

    #define MAX 509
    #define HASH_MULTIPLIER 65599
    #define null NULL

    #define LIBFUNC_COLLISION 2
    #define COLLISION 3
    #define NOT_ACCESSIBLE 4

    char* libfuncs[] = { "print", "input", "objectmemberkeys", "objecttotalmembers",
    "objectcopy", "totalarguments", "argument", "typeof", "strtonum", "sqrt",
    "cos", "sin"};

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
        GLOBAL, LOCALVAR, VAR, FORMAL,
        USERFUNC, LIBFUNC
    };

    typedef struct SymbolTableEntry {
        short int isActive;
        Variable* varVal;
        Function* funcVal;
        enum SymbolType type;
        struct SymbolTableEntry* next;
        struct SymbolTableEntry* next_in_scope;
    }SymbolTableEntry;

    Function* last_function;
    SymbolTableEntry* symtable[MAX];
    SymbolTableEntry* scope_link[MAX];

    char* itoa(int val);
    unsigned int symtable_hash(const char *pcKey);
    int symtable_insert(Variable* var, Function* func, enum SymbolType type);
    int check_for_libfunc(const char* name);
    int hide_scope(const unsigned int scope);
    int symtable_contains(const Variable* var, const Function* func, enum SymbolType type);
    int scope_contains(const char* name, const unsigned int scope);
    int symtable_lookup(const char* name, enum SymbolType type);
    SymbolTableEntry* scope_lookup(const Variable* var, const Function* func, enum SymbolType type);
    char* get_type(enum SymbolType type);
    void print_scopes();

%}


%defines
%output "./src/parser.c"
%union {
    int intVal;
    char* strVal;
    double doubleVal;
    SymbolTableEntry* exprNode;
}

%type <exprNode> lvalue

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
%token MUL_COMMENT

%right ASSIGN
%right NOT INC DEC UMINUS
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
            | BREAK SEMICOLON {if (loop_counter == 0) yyerror("Error: Usage of break outside of loop");}
            | CONTINUE SEMICOLON {if (loop_counter == 0) yyerror("Error: Usage of continue outside of loop");}
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
            | SUB expr %prec UMINUS
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


lvalue:     ID  {
                    Variable* var = malloc(sizeof(struct Variable));
                    var->name = $1;
                    var->line = yylineno;
                    var->scope = scope;
                    if (symtable_insert(var, null, 2) == NOT_ACCESSIBLE) {
                        yyerror("Error: Symbol not accessible");
                    }
                }
            | LOCAL ID          {
                                    Variable* var = malloc(sizeof(struct Variable));
                                    var->name = $2;
                                    var->line = yylineno;
                                    var->scope = scope;
                                    if(scope > 0) {
                                        if (symtable_insert(var, null, 1) == LIBFUNC_COLLISION) {
                                            yyerror("Error: Trying to shadow library function");
                                        }
                                    }
                                    else {
                                        if (symtable_insert(var, null, 0) == LIBFUNC_COLLISION) {
                                            yyerror("Error: Trying to shadow library function");
                                        }
                                    }
                                }
            | SCOPE ID {
                            if (!scope_contains($2, 0)) yyerror("Error: Global symbol not found");
                       }
            | member
            ;


member:     lvalue POINT ID
            | lvalue LBRACKET expr RBRACKET
            | call POINT ID
            | call LBRACKET expr RBRACKET
            ;


call:       call LPAREN elist RPAREN
            | lvalue callsuffix
            | LPAREN funcdef RPAREN LPAREN elist RPAREN
            ;


callsuffix: normcall
            | methodcall
            ;


normcall:   LPAREN elist RPAREN;


methodcall: RANGE ID LPAREN elist RPAREN;


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


funcdef:    FUNCTION ID {
       Function* func = malloc(sizeof(struct Function));
       func->name = $2;
       func->scope = scope;
       func->line = yylineno;
       func->args = null;
       funcdef_counter++;
       last_function = func;
       int code = symtable_insert(null, func, 4);
       if (code == LIBFUNC_COLLISION) {
           yyerror("Error: Trying to shadow libfunc");
       }
       else if (code == COLLISION) {
           yyerror("Error: Symbol redefinition");
       }
       } LPAREN idlist RPAREN block {funcdef_counter--;}
            | FUNCTION {
                   anonymous_functions++;
                   Function* func = malloc(sizeof(struct Function));
                   func->name = malloc(1 + strlen(itoa(anonymous_functions)));
                   strcat(func->name, "$");
                   strcat(func->name, itoa(anonymous_functions));
                   func->scope = scope;
                   func->line = yylineno;
                   func->args = null;
                   funcdef_counter++;
                   last_function = func;
                   symtable_insert(null, func, 4);
            } LPAREN idlist RPAREN block {funcdef_counter--;}
            ;


block:      LCURLY {scope++;} stmt_list RCURLY {hide_scope(scope--);}
            | LCURLY RCURLY
            ;


const:      NUMBER | STRING | NIL | TRUE | FALSE | REAL;


idlist:     ID {
                    Variable* var = malloc(sizeof(struct Variable));
                    var->name = $1;
                    var->scope = scope + 1;
                    var->line = yylineno;
                    last_function->args = malloc(strlen($1) + 1);
                    strcat(last_function->args, $1);
                    strcat(last_function->args, " ");
                    int code = symtable_insert(var, null, 3);
                    if (code == COLLISION) {
                        yyerror("Error: Symbol redefinition");
                    }
                    else if (code == LIBFUNC_COLLISION) {
                        yyerror("Error: Trying to shadow library function");
                    }
               } commaidlist
            |
            ;


commaidlist: COMMA ID {
                        Variable* var = malloc(sizeof(struct Variable));
                        var->name = $2;
                        var->scope = scope + 1;
                        var->line = yylineno;
                        int prev_args = strlen(last_function->args);
                        last_function->args = realloc(last_function->args, prev_args + strlen($2) + 1);
                        strcat(last_function->args, $2);
                        strcat(last_function->args, " ");
                        int code = symtable_insert(var, null, 3);
                        if (code == COLLISION) {
                            yyerror("Error: Symbol redefinition");
                        }
                        else if (code == LIBFUNC_COLLISION) {
                            yyerror("Error: Trying to shadow library function");
                        }
                      } commaidlist
            |
            ;


ifstmt:     IF LPAREN expr RPAREN stmt
            | IF LPAREN expr RPAREN stmt ELSE stmt
            ;


whilestmt:  WHILE LPAREN expr RPAREN {loop_counter++;} stmt {loop_counter--;};


forstmt:    FOR LPAREN elist SEMICOLON expr SEMICOLON elist RPAREN {loop_counter++;} stmt {loop_counter--;};


returnstmt: RETURN expr SEMICOLON {if (funcdef_counter == 0) yyerror("Error: Usage of return outside of function");}
          | RETURN SEMICOLON {if (funcdef_counter == 0) yyerror("Error: Usage of return outside of function");}
          ;

comment: COMMENT
       | MUL_COMMENT
       ;


%%

int yyerror(char* message) {
    fprintf(yyout, "%s at token %s line %d\n", message, yytext, yylineno);
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
            if (scope_contains(name, scope - 1)) return COLLISION;
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


int check_for_libfunc(const char* name){
    for(int i = 0; i < 12; i++)
        if(strcmp(name, libfuncs[i]) == 0)
            return 1;
    return 0;
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


void print_scopes(){
    for(int i = 0; i < MAX; i++){
        SymbolTableEntry* curr = scope_link[i];
        if (!curr) break;
        fprintf(yyout, "\n--------------- Scope #%d ---------------\n", i);
        while(curr){
            char* type = get_type(curr->type);
            if(curr->varVal) {
                fprintf(yyout, "\"%s\" [%s] (line %d) (scope %d)\n", curr->varVal->name, type, curr->varVal->line, curr->varVal->scope);
            }
            else {
                if (curr->funcVal->args)
                    fprintf(yyout, "\"%s\" { args: %s} [%s] (line %d) (scope %d)\n", curr->funcVal->name, curr->funcVal->args, type, curr->funcVal->line, curr->funcVal->scope);
                else
                    fprintf(yyout, "\"%s\" [%s] (line %d) (scope %d)\n", curr->funcVal->name, type, curr->funcVal->line, curr->funcVal->scope);
            }
            curr = curr->next_in_scope;
        }
    }
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

int main(int argc, char** argv) {
    if(argc > 1) {
        if(!(yyin = fopen(argv[1], "r"))) {
            fprintf(stderr, "Cannot read file: %s\n", argv[1]);
            return 1;
        }
    }
    else yyin = stdin;

    if (argc > 2) {
        yyout = fopen(argv[2], "w");
    }
    else yyout = stdout;

    initialize_libfuncs();
    yyparse();
    print_scopes();

    return 0;
}
