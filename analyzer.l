%{

    typedef struct alpha_token_t {
        int line_no;
        int token_no;
        char *token;
    } alpha_token_t;

    #define YY_DECL int alpha_yylex(alpha_token_t *yylval)

%}

%option header-file="./src/analyzer.h"
%option outfile="./src/analyzer.c"

%option yylineno
%option noyywrap

DELIM       [ \t\n]
WS          {delim}+
LETTER      [a-zA-Z]
DIGIT       [0-9]
UNDERSCORE  [_]
ID          {letter}({letter}|{digit}|{underscore})*
NUMBER      -?{digit}+
REAL        -?{digit}\.{digit}+
STRING      \"(\\[^\n]|[^"\\])*\"
TRUE        "true"
FALSE       "false"
NIL         "nil"

IF          "if"
ELSE        "else"
WHILE       "while"
FOR         "for"
FUNCTION    "function"
RETURN      "return"
BREAK       "break"
CONTINUE    "continue"
LOCAL       "local"

AND         "and"
OR          "or"
NOT         "not"
ASSIGNMENT  "="
ADD         "+"
INC         "++"
SUB         "-"
DEC         "--"
MUL         "*"
DIV         "/"
MOD         "%"
EQUAL       "=="
NEQ         "!="
GT          ">"
LT          "<"
GE          ">="
LE          "<="

LCURLY      "{"
RCURLY      "}"
LBRACKET    "["
RBRACKET    "]"
LPAREN      "("
RPAREN      ")"
SEMICOLON   ";"
COMA        ","
COLON       ":"
SCOPE       "::"
POINT       "."
RANGE       ".."

COMMENT     \/\/.*$
MULCOMMENT  \/\*([^*]|\*+[^*/])*\*\/

%%

{FUNCTION}      { printf("Function: %s\n", yytext); }
{ID}            { printf("ID: %s\n", yytext); }
{MULCOMMENT}    { printf("Comment: %s\n", yytext); }

%%

int main(int argc, char **argv) {
    if(argc > 1) {
        if(!(yyin = fopen(argv[1], "r"))) {
            fprintf(stderr, "Cannot read file: %s\n", argv[1]);
            return 1;
        }
    }
    else yyin = stdin;

    alpha_token_t *yylval = malloc(sizeof(alpha_token_t));
    alpha_yylex(yylval);

    return 0;
}
