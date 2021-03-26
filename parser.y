%{
    #include <stdio.h>
    int yyerror(char* message);
    int alpha_yylex(alpha_token_t *yylval);

    extern int yylineno;
    extern char* yytext;
    extern FILE* yyin;
    #define MAX 509
    #define HASH_MULTIPLIER 65599

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
        bool isActive;
        union {
            Variable* varVal;
            Function* funcVal;
        }value;
        enum SymbolType type;
        struct SymbolTableEntry* next;
    }SymbolTableEntry;

    typedef struct SymbolTable {
        struct SymbolTableEntry* [MAX];
    }

    unsigned int symTableHash(const char* key);
    SymTable SymTable_new();
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

{
}
