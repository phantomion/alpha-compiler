%{

    typedef struct alpha_token_t {
        int lineno;
        int tokenno;
        char* content;
        char* type;
        struct alpha_token_t* next;
    } alpha_token_t;

    #define YY_DECL int alpha_yylex(alpha_token_t *yylval)

%}

%option header-file="./src/analyzer.h"
%option outfile="./src/analyzer.c"
%option yylineno
%option noyywrap

DELIM       [ \t\n]
WS          {DELIM}+
LETTER      [a-zA-Z]
DIGIT       [0-9]
UNDERSCORE  [_]
ID          {LETTER}({LETTER}|{DIGIT}|{UNDERSCORE})*
NUMBER      -?{DIGIT}+
REAL        -?{DIGIT}\.{DIGIT}+
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

%x STR IN_COMMENT
%%

    char string_buf[1024];
    char *string_buf_ptr;
    int comment_nesting = 0;
    int tmp_nest = 0;


\"      string_buf_ptr = string_buf; BEGIN(STR);

<STR>\"        { /* saw closing quote - all done */
    BEGIN(INITIAL);
    *string_buf_ptr = '\0';
    printf("string: %s\n", string_buf);
/* return string constant token type and
 * value to parser
 */
}
<STR>\n        {
    BEGIN(INITIAL);
    printf("Unterminated string constant at line %d.\n", yylineno - 1);
    /* error - unterminated string constant */
    /* generate error message */
}
<STR>\\[0-7]{1,3} {
    /* octal escape sequence */
    int result;

    (void) sscanf( yytext + 1, "%o", &result );

    if ( result > 0xff )
        /* error, constant is out-of-bounds */

        *string_buf_ptr++ = result;
}
<STR>\\n  *string_buf_ptr++ = '\n';
<STR>\\t  *string_buf_ptr++ = '\t';
<STR>\\r  *string_buf_ptr++ = '\r';
<STR>\\b  *string_buf_ptr++ = '\b';
<STR>\\f  *string_buf_ptr++ = '\f';

<STR>\\(.|\n)  *string_buf_ptr++ = yytext[1];

<STR>[^\\\n\"]+        {
    char *yptr = yytext;

    while ( *yptr )
        *string_buf_ptr++ = *yptr++;
}
{REAL}          { printf("real\n"); }
{NUMBER}        { printf("Number: %s\n", yytext); }
{TRUE}          {}
{FALSE}         {}
{NIL}           {}
{FUNCTION}      { printf("Function: %s\n", yytext); }
{IF}            {}
{ELSE}          {}
{WHILE}         {}
{BREAK}         {}
{RETURN}        {}
{FOR}           {}
{ID}            { printf("ID: %s\n", yytext); }
{COMMENT}       {}
"/*"            string_buf_ptr = string_buf; BEGIN(IN_COMMENT);
<IN_COMMENT>"*/" {
    if (tmp_nest > 0)
        tmp_nest--;
    else {
        BEGIN(INITIAL);
        *string_buf_ptr = '\0';
        printf("Comment: %s : %d nest level\n", string_buf, comment_nesting);
    }
}
<IN_COMMENT>"/*" {
    comment_nesting++;
    tmp_nest++;
}
<IN_COMMENT>\n  {}
<IN_COMMENT>.   {
    char* yptr = yytext;
    while (*yptr)
        *string_buf_ptr++ = *yptr++;
}
<IN_COMMENT><<EOF>> { BEGIN(INITIAL); printf("Open multiline comment at EOF\n"); }
{WS}            {}
.               { printf("Unknown identifier %s at line %d\n", yytext, yylineno); }

%%

int main(int argc, char** argv) {
    if(argc > 1) {
        if(!(yyin = fopen(argv[1], "r"))) {
            fprintf(stderr, "Cannot read file: %s\n", argv[1]);
            return 1;
        }
    }
    else yyin = stdin;

    alpha_token_t* yylval = malloc(sizeof(alpha_token_t));
    alpha_yylex(yylval);

    return 0;
}
