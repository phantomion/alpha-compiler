/*to do strings*/
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
delim       [ \t\n]
ws          {delim}+
letter      [a-zA-Z]
digit       [0-9]
underscore  [_]
id          {letter}({letter}|{digit}|{underscore})*
number      -?{digit}+
real        -?{digit}\.{digit}+
if          "if"
else        "else"
while       "while"
for         "for"
function    "function"
return      "return"
break       "break"
continue    "continue"
and         "and"
not         "not"
or          "or"
local       "local"
true        "true"
false       "nil"
assignment  "="
plus        "+"
inc         "++"
minus       "-"
dec         "--"
mul         "*"
div         "/"
mod         "%"
equal       "=="
nequal      "!="
gt          ">"
lt          "<"
ge          ">="
le          "<="
string      \"[^\t\n\"]*\"
lcurly      "{"
rcurly      "}"
lbracket    "["
rbracket    "]"
lparen      "("
rparen      ")"
semicolon   ";"
coma        ","
colon       ":"
scope       "::"
point       "."
range       ".."
comment     \/\/.*$
mul_comment \/\*([^*]|\*+[^*/])*\*\/

%%

{function}      {printf("Function: %s\n", yytext);}
{id}            {printf("ID: %s\n", yytext);}
{mul_comment}   {printf("Comment: %s\n", yytext);}

%%

int main(int argc, char **argv) {
    if (argc > 1) {
        if (!(yyin = fopen(argv[1], "r"))) {
            fprintf(stderr, "Cannot read file: %s\n", argv[1]);
            return 1;
        }
    }
    else
        yyin = stdin;
    alpha_token_t *yylval = malloc(sizeof(alpha_token_t));
    alpha_yylex(yylval);
    return 0;
}
