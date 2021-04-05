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

    Lvalue* parse_var(char* id);
    Lvalue* parse_local_var(char* id);
    Lvalue* parse_global_var(char* id);
    void parse_function(char* id);
    void parse_anonymous_function();
    Lvalue* parse_first_args(char* id);
    Lvalue* parse_rest_args(char* id);
    void parse_lvalue(Lvalue* id);

%}


%defines
%output "./src/parser.c"
%union {
    int intVal;
    char* strVal;
    double doubleVal;
    struct Lvalue* exprNode;
}

%type <exprNode> lvalue
%type <exprNode> member
%type <exprNode> call
%type <exprNode> funcdef
%type <exprNode> elist
%type <exprNode> expr
%type <exprNode> term
%type <exprNode> primary
%type <exprNode> assignexpr
%type <exprNode> objectdef
%type <exprNode> const

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
            | ifstmt                {printf("ifstmt\n");}
            | whilestmt             {printf("whilestmt\n");}
            | forstmt               {printf("forstmt\n");}
            | returnstmt            {printf("returnstmt\n");}
            | BREAK SEMICOLON       {printf("BREAK SEMICOLON\n"); if (loop_counter == 0) yyerror("Error: Usage of break outside of loop"); }
            | CONTINUE SEMICOLON    {printf("BREAK SEMICOLON\n"); if (loop_counter == 0) yyerror("Error: Usage of continue outside of loop"); }
            | block                 {printf("block\n");}
            | funcdef               {printf("funcdef\n");}
            | SEMICOLON             {printf(";\n");}
            | comment               {printf("comment\n");}
            ;


stmt_list:  stmt stmt_list
            |
            ;


expr:       assignexpr          {printf("assignexpr\n");    $$ = $1; }
            | expr ADD expr     {printf("expr + expr\n");   if (is_func($1) || is_func($3)) yyerror("Error: Cannot use arithmetic operators on functions"); }
            | expr SUB expr     {printf("expr - expr\n");   if (is_func($1) || is_func($3)) yyerror("Error: Cannot use arithmetic operators on functions"); }
            | expr MUL expr     {printf("expr * expr\n");   if (is_func($1) || is_func($3)) yyerror("Error: Cannot use arithmetic operators on functions"); }
            | expr DIV expr     {printf("expr / expr\n");   if (is_func($1) || is_func($3)) yyerror("Error: Cannot use arithmetic operators on functions"); }
            | expr MOD expr     {printf("expr %% expr\n");  if (is_func($1) || is_func($3)) yyerror("Error: Cannot use arithmetic operators on functions"); }
            | expr GT expr      {printf("expr > expr\n");   if (is_func($1) || is_func($3)) yyerror("Error: Cannot use arithmetic operators on functions"); }
            | expr GE expr      {printf("expr >= expr\n");  if (is_func($1) || is_func($3)) yyerror("Error: Cannot use arithmetic operators on functions"); }
            | expr LT expr      {printf("expr < expr\n");   if (is_func($1) || is_func($3)) yyerror("Error: Cannot use arithmetic operators on functions"); }
            | expr LE expr      {printf("expr <= expr\n");  if (is_func($1) || is_func($3)) yyerror("Error: Cannot use arithmetic operators on functions"); }
            | expr EQUAL expr   {printf("expr == expr\n");  if (is_func($1) || is_func($3)) yyerror("Error: Cannot use arithmetic operators on functions"); }
            | expr NEQ expr     {printf("expr != expr\n");  if (is_func($1) || is_func($3)) yyerror("Error: Cannot use arithmetic operators on functions"); }
            | expr AND expr     {printf("expr AND expr\n"); if (is_func($1) || is_func($3)) yyerror("Error: Cannot use arithmetic operators on functions"); }
            | expr OR expr      {printf("expr OR expr\n");  if (is_func($1) || is_func($3)) yyerror("Error: Cannot use arithmetic operators on functions"); }
            | term              {printf("term\n");          $$ = $1; }
            | error             {printf("error\n");         yyclearin; }
            ;


term:       LPAREN expr RPAREN      {printf("(expr)\n"); $$ = $2; }
            | primary               {printf("primary\n"); $$ = $1; }
            | SUB expr %prec UMINUS {printf("- expr %%prec UMINUS\n"); parse_lvalue($2); }
            | NOT expr              {printf("NOT expr\n");   parse_lvalue($2); }
            | INC lvalue            {printf("++lvalue\n"); parse_lvalue($2); }
            | lvalue INC            {printf("lvalue++\n"); parse_lvalue($1); }
            | DEC lvalue            {printf("--lvalue\n"); parse_lvalue($2); }
            | lvalue DEC            {printf("lvalue--\n"); parse_lvalue($1); }
            ;


assignexpr: lvalue ASSIGN expr      {printf("lvalue = expr\n"); if (is_func($1)) yyerror("Error: Cannot assign to function"); }


primary:    lvalue                  {printf("lvalue\n");    $$ = $1; }
            | call                  {printf("call\n");      $$ = null; }
            | objectdef             {printf("objectdef\n"); $$ = null; }
            | LPAREN funcdef RPAREN {printf("(funcdef)\n"); $$ = null; }
            | const                 {printf("const\n");     $$ = $1; }
            ;


lvalue:     ID          {printf("ID\n"); $$ = parse_var($1); }
            | LOCAL ID  {printf("LOCAL ID\n"); $$ = parse_local_var($2);  }
            | SCOPE ID  {printf("::ID\n"); $$ = parse_global_var($2);  }
            | member    {printf("member\n"); $$ = null; }
            ;


member:     lvalue POINT ID                 {printf("lvalue.ID\n"); if (is_func($1)) yyerror("Error: Cannot access member of function"); }
            | lvalue LBRACKET expr RBRACKET {printf("lvalue [expr]\n"); if (is_func($1)) yyerror("Error: Cannot access member of function"); }
            | call POINT ID                 {printf("call.ID\n");}
            | call LBRACKET expr RBRACKET   {printf("call[expr]\n");}
            ;


call:       call LPAREN elist RPAREN                    {printf("call(elist)\n");}
            | lvalue callsuffix                         {printf("lvalue callsuffix\n");}
            | LPAREN funcdef RPAREN LPAREN elist RPAREN {printf("(funcdef) (elist)\n"); $$ = null; }
            ;


callsuffix: normcall     {printf("normcall\n");}
            | methodcall {printf("methodcall\n");}
            ;


normcall:   LPAREN elist RPAREN {printf("(elist)\n");};


methodcall: RANGE ID LPAREN elist RPAREN {printf("..ID(elist)\n");};


elist:      expr commaexpr {printf("expr commaexpr\n");}
            | {$$ = null;}
            ;


commaexpr:  COMMA expr commaexpr {printf(", expr commaexpr\n");}
            |
            ;


objectdef:  LBRACKET elist RBRACKET     {printf("[elist]\n");   $$ = null; }
            | LBRACKET indexed RBRACKET {printf("[indexed]\n"); $$ = null; }
            ;


indexed:    indexelem indexelemlist {printf("indexelem indexelemlist\n");}
            ;


indexelemlist:  COMMA indexelem indexelemlist {printf(", indexelem\n");}
                |
                ;


indexelem:  LCURLY expr COLON expr RCURLY {printf("{expr:expr}\n");}
            ;


funcdef: FUNCTION ID { parse_function($2); } LPAREN idlist RPAREN block         {printf("FUNCTION ID(idlist) block\n"); funcdef_counter--; }
         | FUNCTION  { parse_anonymous_function(); } LPAREN idlist RPAREN block {printf("FUNCTION (idlist) block\n"); funcdef_counter--; }
         ;


block: LCURLY { scope++; } stmt_list RCURLY {printf("{stmtlist}\n"); hide_scope(scope--); }
       | LCURLY RCURLY                      {printf("{}\n");}
       ;


const:      NUMBER      {printf("NUMBER\n"); $$ = new_lvalue($1, 0, null, null); }
            | STRING    {printf("STRING\n"); $$ = new_lvalue(0, 0, $1, null); }
            | NIL       {printf("NIL\n");    $$ = null; }
            | TRUE      {printf("TRUE\n");   $$ = null; }
            | FALSE     {printf("FALSE\n");  $$ = null; }
            | REAL      {printf("REAL\n");   $$ = new_lvalue(0, $1, null, null); }
            ;


idlist: ID { $<exprNode>$ = parse_first_args($1); } commaidlist {printf("ID commaidlist\n");}
        |
        ;


commaidlist: COMMA ID { $<exprNode>$ = parse_rest_args($2); } commaidlist {printf(", ID commaidlist\n");}
             |
             ;


ifstmt: IF LPAREN expr RPAREN stmt             {printf("IF (expr) stmt\n");}
        | IF LPAREN expr RPAREN stmt ELSE stmt {printf("IF (expr) stmt ELSE stmt\n");}
        ;


whilestmt:  WHILE LPAREN expr RPAREN { loop_counter++; } stmt {printf("WHILE (expr) stmt\n"); loop_counter--; };


forstmt:    FOR LPAREN elist SEMICOLON expr SEMICOLON elist RPAREN {loop_counter++;} stmt {printf("FOR (elist;expr;elist) stmt\n"); loop_counter--;};


returnstmt: RETURN expr SEMICOLON   {printf("RETURN expr\n"); if (funcdef_counter == 0) yyerror("Error: Usage of return outside of function"); }
            | RETURN SEMICOLON      {printf("RETURN;\n");     if (funcdef_counter == 0) yyerror("Error: Usage of return outside of function"); }
            ;


comment: COMMENT       {printf("COMMENT\n");}
         | MUL_COMMENT {printf("MUL_COMMENT\n");}
         ;


%%

int yyerror(char* message) {
    fprintf(yyout, "%s at token %s line %d\n", message, yytext, yylineno);
    return 1;
}


void print_scopes() {
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


void parse_lvalue(Lvalue* id) {
    if (is_func(id)) yyerror("Error: Cannot use arithmetic operators on functions");
}


Lvalue* parse_global_var(char* id) {
    if (!scope_contains(id, 0)) {
        yyerror("Error: Global symbol not found");
        return null;
    }

    return new_lvalue(0, 0, null, scope_get(id, 0));
}

Lvalue* parse_var(char *id) {

    Variable* var = malloc(sizeof(struct Variable));
    var->name = id;
    var->line = yylineno;
    var->scope = scope;
    int code = symtable_insert(var, null, 2);

    if (code == NOT_ACCESSIBLE) {
        yyerror("Error: Symbol not accessible");
        return null;
    }
    else if (code == VARS) {
        if (symtable_get(id, 2)) {
            return new_lvalue(0, 0, null, symtable_get(id, 2));
        }
        if (symtable_get(id, 1)) {
            return new_lvalue(0, 0, null, symtable_get(id, 1));
        }
    }
    else if (code == FORMAL_ARGUMENT) {
        return new_lvalue(0, 0, null, symtable_get(id, 3));
    }
    else if (code == USER_FUNC) {
        return new_lvalue(0, 0, null, symtable_get(id, 4));
    }
    else if (code == LIB_FUNC) {
        return new_lvalue(0, 0, null, symtable_get(id, 5));
    }
    else {
       return new_lvalue(0, 0, null, scope_get(id, 0));
    }

    return null;
}


Lvalue* parse_local_var(char *id) {

    Variable* var = malloc(sizeof(struct Variable));
    var->name = id;
    var->line = yylineno;
    var->scope = scope;

    if(scope > 0) {
        if (symtable_insert(var, null, 1) == LIBFUNC_COLLISION) {
            yyerror("Error: Trying to shadow library function");
            return null;
        }
    }
    else {
        if (symtable_insert(var, null, 0) == LIBFUNC_COLLISION) {
            yyerror("Error: Trying to shadow library function");
            return null;
        }
    }

    return new_lvalue(0, 0, null, scope_get(id, scope));
}


void parse_function(char *id) {

    Function* func = malloc(sizeof(struct Function));
    func->name = id;
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
}


void parse_anonymous_function() {
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
}


Lvalue* parse_first_args(char *id) {

    Variable* var = malloc(sizeof(struct Variable));
    var->name = id;
    var->scope = scope + 1;
    var->line = yylineno;

    last_function->args = malloc(strlen(id) + 1);
    strcat(last_function->args, id);
    strcat(last_function->args, " ");
    symtable_insert(var, null, 3);

    return new_lvalue(0, 0, null, symtable_get(id, 3));
}


Lvalue* parse_rest_args(char *id) {

    Variable* var = malloc(sizeof(struct Variable));
    var->name = id;
    var->scope = scope + 1;
    var->line = yylineno;

    int prev_args = strlen(last_function->args);
    last_function->args = realloc(last_function->args, prev_args + strlen(id) + 1);
    strcat(last_function->args, id);
    strcat(last_function->args, " ");
    symtable_insert(var, null, 3);

    return new_lvalue(0, 0, null, symtable_get(id, 3));
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
