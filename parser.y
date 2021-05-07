%{
    #include "utilities.h"
    #include "icode.h"
    #include "parser.h"
    #include "manager.h"

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
    extern int icode_phase;
    extern int functionlocal_offset;

    extern char* libfuncs[];

    extern symbol* symtable[MAX];
    extern symbol* scope_link[MAX];

%}


%defines
%output "./src/parser.c"
%union {
    int intVal;
    char* strVal;
    double doubleVal;
    struct expr* exprNode;
    struct call* callNode;
    struct index_elem* indexElemNode;
}

%type <exprNode> lvalue
%type <exprNode> member
%type <exprNode> call
%type <exprNode> funcdef
%type <exprNode> elist
%type <exprNode> commaexpr
%type <exprNode> expr
%type <exprNode> term
%type <exprNode> primary
%type <exprNode> assignexpr
%type <exprNode> objectdef
%type <exprNode> const
%type <exprNode> funcprefix
%type <callNode> callsuffix
%type <callNode> normcall
%type <callNode> methodcall
%type <indexElemNode> indexelem
%type <indexElemNode> indexelemlist
%type <indexElemNode> indexed
%type <strVal> funcname
%type <intVal> funcbody

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

stmt:       expr SEMICOLON {reset_temp();}
            | ifstmt
            | whilestmt
            | forstmt
            | returnstmt
            | BREAK SEMICOLON       {if (loop_counter == 0) yyerror("Usage of break outside of loop"); }
            | CONTINUE SEMICOLON    {if (loop_counter == 0) yyerror("Usage of continue outside of loop"); }
            | block
            | funcdef
            | SEMICOLON
            | comment
            ;


stmt_list:  stmt stmt_list
            |
            ;


expr:       assignexpr          {$$ = $1;}
            | expr ADD expr     {$$ = manage_add($1, $3);}
            | expr SUB expr     {$$ = manage_sub($1, $3);}
            | expr MUL expr     {$$ = manage_mul($1, $3);}
            | expr DIV expr     {$$ = manage_div($1, $3);}
            | expr MOD expr     {$$ = manage_mod($1, $3);}
            | expr GT expr      {$$ = manage_greater($1, $3);}
            | expr GE expr      {$$ = manage_greatereq($1, $3);}
            | expr LT expr      {$$ = manage_less($1, $3);}
            | expr LE expr      {$$ = manage_lesseq($1, $3);}
            | expr EQUAL expr   {$$ = manage_eq($1, $3);}
            | expr NEQ expr     {$$ = manage_neq($1, $3);}
            | expr AND expr     {$$ = manage_and($1, $3);}
            | expr OR expr      {$$ = manage_or($1, $3);}
            | term              {$$ = $1; }
            | error             {yyclearin;}
            ;


term:       LPAREN expr RPAREN      {$$ = $2;}
            | primary               {$$ = $1;}
            | SUB expr %prec UMINUS {$$ = manage_uminus($2);}
            | NOT expr              {$$ = manage_not($2);}
            | INC lvalue            {$$ = manage_pre_inc($2);}
            | lvalue INC            {$$ = manage_post_inc($1);}
            | DEC lvalue            {$$ = manage_pre_dec($2);}
            | lvalue DEC            {$$ = manage_post_dec($1);}
            ;


assignexpr: lvalue ASSIGN expr      {$$ = manage_assignexpr($1, $3);}


primary:    lvalue                  {$$ = emit_iftableitem($1);}
            | call                  {$$ = $1;}
            | objectdef             {$$ = $1;}
            | LPAREN funcdef RPAREN {$$ = $2;}
            | const                 {$$ = $1;}
            ;


lvalue:     ID          {$$ = manage_var($1);}
            | LOCAL ID  {$$ = manage_local_var($2);}
            | SCOPE ID  {$$ = manage_global_var($2);}
            | member    {$$ = $1; }
            ;


member:     lvalue POINT ID                 {$$ = manage_member_item($1, $3);}
            | lvalue LBRACKET expr RBRACKET {$$ = manage_array_item($1, $3); }
            | call POINT ID
            | call LBRACKET expr RBRACKET
            ;


call:       call LPAREN elist RPAREN {$$ = make_call($1, $3);}
            | lvalue callsuffix {$$ = manage_call_lvalue($1, $2);}
            | LPAREN funcdef RPAREN LPAREN elist RPAREN {$$ = manage_call_funcdef($2, $5);}
            ;


callsuffix: normcall {$$ = $1;}
            | methodcall {$$ = $1;}
            ;


normcall:   LPAREN elist RPAREN {$$ = manage_normcall($2);}


methodcall: RANGE ID LPAREN elist RPAREN {$$ = manage_methodcall($2, $4);}


elist:      expr commaexpr {$$ = manage_elist($1, $2);}
            | {$$ = null;}
            ;


commaexpr:  COMMA expr commaexpr {$$ = manage_elist($2, $3);}
            | {$$ = null;}
            ;


objectdef:  LBRACKET elist RBRACKET     {$$ = manage_tablemake($2); }
            | LBRACKET indexed RBRACKET {$$ = manage_mapmake($2); }
            ;


indexed:    indexelem indexelemlist {$$ = manage_indexelemlist($1, $2);}
            ;


indexelemlist:  COMMA indexelem indexelemlist {$$ = manage_indexelemlist($2, $3);}
                | {$$ = null;}
                ;


indexelem:  LCURLY expr COLON expr RCURLY {$$ = manage_indexelem($2, $4);}
            ;


funcname: ID {$$ = $1;}
        | {$$ = new_anonymous_function();}
        ;

funcprefix: FUNCTION funcname {$$ = manage_function($2);}
          ;


funcargs: LPAREN idlist RPAREN {enter_scopespace();}
        ;


funcbody: block {$$ = functionlocal_offset;}
        ;


funcdef: funcprefix funcargs funcbody {manage_function_exit($1, $3);}
         ;


block: LCURLY { scope++; } stmt_list RCURLY {hide_scope(scope--); }
       | LCURLY RCURLY
       ;


const:      NUMBER      {$$ = manage_number($1);}
            | STRING    {$$ = manage_string($1);}
            | NIL       {$$ = manage_nil(); }
            | TRUE      {$$ = manage_bool(1);}
            | FALSE     {$$ = manage_bool(0);}
            | REAL      {$$ = manage_real($1);}
            ;


idlist: ID { $<exprNode>$ = manage_args($1); } commaidlist
        |
        ;


commaidlist: COMMA ID { $<exprNode>$ = manage_args($2); } commaidlist
             |
             ;


ifstmt: IF LPAREN expr RPAREN stmt
        | IF LPAREN expr RPAREN stmt ELSE stmt
        ;


whilestmt:  WHILE LPAREN expr RPAREN { loop_counter++; } stmt {loop_counter--; };


forstmt:    FOR LPAREN elist SEMICOLON expr SEMICOLON elist RPAREN {loop_counter++;} stmt {loop_counter--;};


returnstmt: RETURN expr SEMICOLON   {if (funcdef_counter == 0) yyerror("Usage of return outside of function"); }
            | RETURN SEMICOLON      {if (funcdef_counter == 0) yyerror("Usage of return outside of function"); }
            ;


comment: COMMENT
         | MUL_COMMENT
         ;


%%

int yyerror(char* message) {
    icode_phase = 0;
    fprintf(yyout, COLOR_RED"Error:"COLOR_RESET" %s at token %s line %d\n", message, yytext, yylineno);
    return 1;
}


void print_scopes() {
    int i;
    for(i = 0; i < MAX; i++){

        symbol* curr = scope_link[i];
        if (!curr) break;
        fprintf(yyout, "\n--------------- Scope #%d ---------------\n", i);

        while(curr){
            char* type = get_type(curr->type);

            fprintf(yyout, "\"%s\" [%s] (line %d) (scope %d) (offset %d) (scopespace %d)\n",\
                curr->name, type, curr->line, curr->scope, curr->offset, curr->space);
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
    /*print_scopes();*/
    if (icode_phase) {
        print_quads();
    }

    return 0;
}
