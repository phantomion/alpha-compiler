%{

    #include "utilities.h"

    int yyerror(char* message);
    int yylex();

    extern int yylineno;
    extern char* yytext;
    extern FILE* yyin;
    extern FILE* yyout;

    extern int scope;
    extern int funcdef_counter;
    extern int anonymous_functions;
    extern int loop_counter;

    extern char* libfuncs[];

    extern Function* last_function;
    extern SymbolTableEntry* symtable[MAX];
    extern SymbolTableEntry* scope_link[MAX];
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
