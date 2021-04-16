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

stmt:       expr SEMICOLON          {printf("stmt <- expr ;\n");}
            | ifstmt                {printf("stmt <- ifstmt\n");}
            | whilestmt             {printf("stmt <- whilestmt\n");}
            | forstmt               {printf("stmt <- forstmt\n");}
            | returnstmt            {printf("stmt <- returnstmt\n");}
            | BREAK SEMICOLON       {printf("stmt <- BREAK SEMICOLON\n"); if (loop_counter == 0) yyerror("Usage of break outside of loop"); }
            | CONTINUE SEMICOLON    {printf("stmt <- BREAK SEMICOLON\n"); if (loop_counter == 0) yyerror("Usage of continue outside of loop"); }
            | block                 {printf("stmt <- block\n");}
            | funcdef               {printf("stmt <- funcdef\n");}
            | SEMICOLON             {printf("stmt <- ;\n");}
            | comment               {printf("stmt <- comment\n");}
            ;


stmt_list:  stmt stmt_list {printf("stmt_list <- stmt stmt_list\n");}
            |
            ;


expr:       assignexpr          {printf("expr <- assignexpr\n"); $$ = $1; }
            | expr ADD expr     {printf("expr <- expr + expr\n"); }
            | expr SUB expr     {printf("expr <- expr - expr\n"); }
            | expr MUL expr     {printf("expr <- expr * expr\n"); }
            | expr DIV expr     {printf("expr <- expr / expr\n"); }
            | expr MOD expr     {printf("expr <- expr %% expr\n"); }
            | expr GT expr      {printf("expr <- expr > expr\n");  }
            | expr GE expr      {printf("expr <- expr >= expr\n"); }
            | expr LT expr      {printf("expr <- expr < expr\n");  }
            | expr LE expr      {printf("expr <- expr <= expr\n"); }
            | expr EQUAL expr   {printf("expr <- expr == expr\n"); }
            | expr NEQ expr     {printf("expr <- expr != expr\n"); }
            | expr AND expr     {printf("expr <- expr AND expr\n"); }
            | expr OR expr      {printf("expr <- expr OR expr\n"); }
            | term              {printf("expr <- term\n"); $$ = $1; }
            | error             {yyclearin;}
            ;


term:       LPAREN expr RPAREN      {printf("term <- ( expr )\n"); $$ = $2; }
            | primary               {printf("term <- primary\n"); $$ = $1; }
            | SUB expr %prec UMINUS {printf("term <- - expr\n"); parse_lvalue($2); }
            | NOT expr              {printf("term <- NOT expr\n"); parse_lvalue($2); }
            | INC lvalue            {printf("term <- ++lvalue\n"); parse_lvalue($2); }
            | lvalue INC            {printf("term <- lvalue++\n"); parse_lvalue($1); }
            | DEC lvalue            {printf("term <- --lvalue\n"); parse_lvalue($2); }
            | lvalue DEC            {printf("term <- lvalue--\n"); parse_lvalue($1); }
            ;


assignexpr: lvalue ASSIGN expr      {printf("assignexpr <- lvalue = expr\n"); if (is_func($1)) yyerror("Cannot assign to function"); }


primary:    lvalue                  {printf("primary <- lvalue\n"); $$ = $1; }
            | call                  {printf("primary <- call\n"); $$ = null; }
            | objectdef             {printf("primary <- objectdef\n"); $$ = null; }
            | LPAREN funcdef RPAREN {printf("primary <- ( funcdef )\n"); $$ = null; }
            | const                 {printf("primary <- const\n"); $$ = $1; }
            ;


lvalue:     ID          {printf("lvalue <- ID\n"); $$ = parse_var($1); }
            | LOCAL ID  {printf("lvalue <- LOCAL ID\n"); $$ = parse_local_var($2);  }
            | SCOPE ID  {printf("lvalue <- ::ID\n"); $$ = parse_global_var($2);  }
            | member    {printf("lvalue <- member\n"); $$ = null; }
            ;


member:     lvalue POINT ID                 {printf("member <- lvalue.ID\n"); if (is_func($1)) yyerror("Cannot access member of function"); }
            | lvalue LBRACKET expr RBRACKET {printf("member <- lvalue [expr]\n"); if (is_func($1)) yyerror("Cannot access member of function"); }
            | call POINT ID                 {printf("member <- call.ID\n");}
            | call LBRACKET expr RBRACKET   {printf("member <- call[expr]\n");}
            ;


call:       call LPAREN elist RPAREN                    {printf("call <- call(elist)\n");}
            | lvalue callsuffix                         {printf("call <- lvalue callsuffix\n");}
            | LPAREN funcdef RPAREN LPAREN elist RPAREN {printf("call <- (funcdef) (elist)\n"); $$ = null; }
            ;


callsuffix: normcall     {printf("callsuffix <- normcall\n");}
            | methodcall {printf("callsuffix <- methodcall\n");}
            ;


normcall:   LPAREN elist RPAREN {printf("normcall <- (elist)\n");};


methodcall: RANGE ID LPAREN elist RPAREN {printf("methodcall <- ..ID(elist)\n");};


elist:      expr commaexpr {printf("elist <- expr commaexpr\n");}
            | {$$ = null;}
            ;


commaexpr:  COMMA expr commaexpr {printf("commaexpr <- , expr commaexpr\n");}
            |
            ;


objectdef:  LBRACKET elist RBRACKET     {printf("objectdef <- [elist]\n"); $$ = null; }
            | LBRACKET indexed RBRACKET {printf("objectdef <- [indexed]\n"); $$ = null; }
            ;


indexed:    indexelem indexelemlist {printf("indexed <- indexelem indexelemlist\n");}
            ;


indexelemlist:  COMMA indexelem indexelemlist {printf("indexelemlist <- , indexelem\n");}
                |
                ;


indexelem:  LCURLY expr COLON expr RCURLY {printf("indexelem <- {expr:expr}\n");}
            ;


funcdef: FUNCTION ID { parse_function($2); } LPAREN idlist RPAREN block         {printf("funcdef <- FUNCTION ID(idlist) block\n"); funcdef_counter--; }
         | FUNCTION  { parse_anonymous_function(); } LPAREN idlist RPAREN block {printf("funcdef <- FUNCTION (idlist) block\n"); funcdef_counter--; }
         ;


block: LCURLY { scope++; } stmt_list RCURLY {printf("block <- {stmtlist}\n"); hide_scope(scope--); }
       | LCURLY RCURLY                      {printf("block <- {}\n");}
       ;


const:      NUMBER      {printf("const <- NUMBER\n"); $$ = new_lvalue($1, 0, null, null); }
            | STRING    {printf("const <- STRING\n"); $$ = new_lvalue(0, 0, $1, null); }
            | NIL       {printf("const <- NIL\n"); $$ = null; }
            | TRUE      {printf("const <- TRUE\n"); $$ = null; }
            | FALSE     {printf("const <- FALSE\n"); $$ = null; }
            | REAL      {printf("const <- REAL\n"); $$ = new_lvalue(0, $1, null, null); }
            ;


idlist: ID { $<exprNode>$ = parse_first_args($1); } commaidlist {printf("idlist <- ID commaidlist\n");}
        |
        ;


commaidlist: COMMA ID { $<exprNode>$ = parse_rest_args($2); } commaidlist {printf("commaidlist <- , ID commaidlist\n");}
             |
             ;


ifstmt: IF LPAREN expr RPAREN stmt             {printf("ifstmt <- IF (expr) stmt\n");}
        | IF LPAREN expr RPAREN stmt ELSE stmt {printf("ifstmt <- IF (expr) stmt ELSE stmt\n");}
        ;


whilestmt:  WHILE LPAREN expr RPAREN { loop_counter++; } stmt {printf("whilestmt <- WHILE (expr) stmt\n"); loop_counter--; };


forstmt:    FOR LPAREN elist SEMICOLON expr SEMICOLON elist RPAREN {loop_counter++;} stmt {printf("forstmt <- FOR (elist;expr;elist) stmt\n"); loop_counter--;};


returnstmt: RETURN expr SEMICOLON   {printf("returnstmt <- RETURN expr\n"); if (funcdef_counter == 0) yyerror("Usage of return outside of function"); }
            | RETURN SEMICOLON      {printf("returnstmt <- RETURN;\n");     if (funcdef_counter == 0) yyerror("Usage of return outside of function"); }
            ;


comment: COMMENT       {printf("comment <- COMMENT\n");}
         | MUL_COMMENT {printf("comment <- MUL_COMMENT\n");}
         ;


%%

int yyerror(char* message) {
    fprintf(yyout,"\x1b[31mError:\x1b[0m %s at token %s line %d\n", message, yytext, yylineno);
    return 1;
}


void print_scopes() {
    int i;
    for(i = 0; i < MAX; i++){

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
    if (is_func(id)) yyerror("Cannot use arithmetic operators on functions");
}


Lvalue* parse_global_var(char* id) {
    if (!scope_contains(id, 0)) {
        yyerror("Global symbol not found");
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
        yyerror("Symbol not accessible");
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
            yyerror("Trying to shadow library function");
            return null;
        }
    }
    else {
        if (symtable_insert(var, null, 0) == LIBFUNC_COLLISION) {
            yyerror("Trying to shadow library function");
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
        yyerror("Trying to shadow libfunc");
    }
    else if (code == COLLISION) {
        yyerror("Symbol redefinition");
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
