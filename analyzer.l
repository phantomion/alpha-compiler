%{
    #include "parser.h"

%}

%option header-file="./src/analyzer.h"
%option outfile="./src/analyzer.c"

%option yylineno
%option noyywrap
%option nounput
%option noinput

DELIM       [ \t\n]
WS          {DELIM}+
LETTER      [a-zA-Z]
DIGIT       [0-9]
UNDERSCORE  [_]
ID          {LETTER}({LETTER}|{DIGIT}|{UNDERSCORE})*
NUMBER      {DIGIT}+
REAL        {DIGIT}+\.{DIGIT}+

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

ASSIGN      "="
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
COMMA        ","
COLON       ":"
SCOPE       "::"
POINT       "."
RANGE       ".."
PRINT       "print"
INPUT       "input"
OBJECTMEMBERKEYS "objectmemberkeys"
OBJECTTOTALMEMBERS "objecttotalmembers"
OBJECTCOPY  "objectcopy"
TOTALARGUMENTS "totalarguments"
ARGUMENT    "argument"
TYPEOF      "typeof"
STRTONUM    "strtonum"
SQRT        "sqrt"
COS         "cos"
SIN         "sin"

COMMENT     \/\/.*$

%x STR IN_COMMENT


%%


    char string_buf[1024];
    char *string_buf_ptr;
    int tmp_nest = 0;


\"      string_buf_ptr = string_buf; BEGIN(STR);

<STR>\"        { /* saw closing quote - all done */
    BEGIN(INITIAL);
    *string_buf_ptr = '\0';
    yylval.strVal = string_buf;
    return STRING;
}

<STR>\n        {
    BEGIN(INITIAL);
    fprintf(yyout, "Unterminated string constant at line %d.\n", yylineno - 1);
}

<STR>\\n        *string_buf_ptr++ = '\n';
<STR>\\t        *string_buf_ptr++ = '\t';
<STR>\\r        *string_buf_ptr++ = '\r';
<STR>\\b        *string_buf_ptr++ = '\b';
<STR>\\f        *string_buf_ptr++ = '\f';
<STR>\\(.|\n)   *string_buf_ptr++ = yytext[1];

<STR>[^\\\n\"]+        {
    char *yptr = yytext;

    while ( *yptr )
        *string_buf_ptr++ = *yptr++;
}

{REAL}          {
    yylval.doubleVal = atof(yytext);
    return REAL;
}

{NUMBER}       {
    yylval.intVal = atoi(yytext);
    return NUMBER;
}

{IF}            {return IF;}
{ELSE}          {return ELSE;}
{FOR}           {return FOR;}
{WHILE}         {return WHILE;}
{BREAK}         {return BREAK;}
{CONTINUE}      {return CONTINUE;}
{FUNCTION}      {return FUNCTION;}
{RETURN}        {return RETURN;}
{LOCAL}         {return LOCAL;}
{AND}           {return AND;}
{OR}            {return OR;}
{NOT}           {return NOT;}
{TRUE}          {return TRUE;}
{FALSE}         {return FALSE;}
{NIL}           {return NIL;}

{ASSIGN}        {return ASSIGN;}
{ADD}           {return ADD;}
{SUB}           {return SUB;}
{INC}           {return INC;}
{DEC}           {return DEC;}
{MUL}           {return MUL;}
{LE}            {return LE;}
{DIV}           {return DIV;}
{MOD}           {return MOD;}
{EQUAL}         {return EQUAL;}
{NEQ}           {return NEQ;}
{GT}            {return GT;}
{LT}            {return LT;}
{GE}            {return GE;}

{LCURLY}        {return LCURLY;}
{RCURLY}        {return RCURLY;}
{LBRACKET}      {return LBRACKET;}
{RBRACKET}      {return RBRACKET;}
{LPAREN}        {return LPAREN;}
{RPAREN}        {return RPAREN;}
{SEMICOLON}     {return SEMICOLON;}
{COLON}         {return COLON;}
{SCOPE}         {return SCOPE;}
{POINT}         {return POINT;}
{RANGE}         {return RANGE;}
{COMMA}         {return COMMA;}
{PRINT}         {return PRINT;}
{INPUT}         {return INPUT;}
{OBJECTMEMBERKEYS} {return OBJECTMEMBERKEYS;}
{OBJECTTOTALMEMBERS} {return OBJECTTOTALMEMBERS;}
{OBJECTCOPY}    {return OBJECTCOPY;}
{TOTALARGUMENTS} {return TOTALARGUMENTS;}
{ARGUMENT}      {return ARGUMENT;}
{TYPEOF}        {return TYPEOF;}
{STRTONUM}      {return STRTONUM;}
{SQRT}          {return SQRT;}
{COS}           {return COS;}
{SIN}           {return SIN;}

{ID}            {yylval.strVal = yytext; return ID;}

{COMMENT}       {return COMMENT;}

"/*"    BEGIN(IN_COMMENT);

<IN_COMMENT>"*/" {
    if (tmp_nest > 0) tmp_nest--;
    else {
        BEGIN(INITIAL);
        return MUL_COMMENT;
    }
}

<IN_COMMENT>"/*" {
    tmp_nest++;
}

<IN_COMMENT>\n      {}
<IN_COMMENT>.       {}
<IN_COMMENT><<EOF>> { BEGIN(INITIAL); fprintf(yyout, "Open multiline comment at EOF\n"); }

{WS}            {}
.               { fprintf(yyout, "Unknown identifier %s at line %d\n", yytext, yylineno); }


%%

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

    yyparse();

    return 0;
}
