%{
    #include "parser.h"

    typedef struct alpha_token_t {
        int lineno;
        int tokenno;
        char* content;
        char* type;
        char* category;
        struct alpha_token_t* next;
    } alpha_token_t;

    char* keyword_type(char* yytext);
    char* operator_type(char* yytext);
    char* punctuation_type(char* yytext);
    int tokenno = 1;

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

COMMENT     \/\/.*$

%x STR IN_COMMENT


%%


    char string_buf[1024];
    char *string_buf_ptr;
    int comment_nesting = 0;
    int tmp_nest = 0;
    int start_line = -1;
    int end_line = -1;


\"      string_buf_ptr = string_buf; BEGIN(STR);

<STR>\"        { /* saw closing quote - all done */
    BEGIN(INITIAL);
    *string_buf_ptr = '\0';
    alpha_token_t* new_token = malloc(sizeof(struct alpha_token_t));
    new_token->lineno = yylineno;
    new_token->tokenno = tokenno++;
    new_token->content = string_buf;
    new_token->type = malloc(9+sizeof(string_buf));

    strcat(new_token->type, "STRING \"");
    strcat(new_token->type, string_buf);
    strcat(new_token->type, "\"");

    new_token->category = "<-char*";
    yylval->next = new_token;

    fprintf(yyout, "%d: #%d \"%s\" %s %s\n", new_token->lineno, \
            new_token->tokenno, new_token->content, new_token->type, \
            new_token->category);

    yylval = yylval->next;

/* return string constant token type and
 * value to parser
 */
}

<STR>\n        {
    BEGIN(INITIAL);
    fprintf(yyout, "Unterminated string constant at line %d.\n", yylineno - 1);
    /* error - unterminated string constant */
    /* generate error message */
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
    alpha_token_t* new_token = malloc(sizeof(struct alpha_token_t));
    new_token->lineno = yylineno;
    new_token->tokenno = tokenno++;
    new_token->content = yytext;
    new_token->type = malloc(14+sizeof(yytext));

    strcat(new_token->type, "CONST FLOAT \"");
    strcat(new_token->type, yytext);
    strcat(new_token->type, "\"");

    new_token->category = "<-float";
    yylval->next = new_token;

    fprintf(yyout, "%d: #%d \"%s\" %s %s\n", new_token->lineno, \
            new_token->tokenno, new_token->content, new_token->type, \
            new_token->category);

    yylval = yylval->next;
    return REAL;
}

{NUMBER}        {
    alpha_token_t* new_token = malloc(sizeof(struct alpha_token_t));
    new_token->lineno = yylineno;
    new_token->tokenno = tokenno++;
    new_token->content = yytext;
    new_token->type = malloc(12+sizeof(yytext));

    strcat(new_token->type, "CONST INT \"");
    strcat(new_token->type, yytext);
    strcat(new_token->type, "\"");

    new_token->category = "<-integer";
    yylval->next = new_token;

    fprintf(yyout, "%d: #%d \"%s\" %s %s\n", new_token->lineno, \
            new_token->tokenno, new_token->content, new_token->type, \
            new_token->category);

    yylval = yylval->next;
    return INTEGER;
}

{IF} {return IF;}
{ELSE} {return ELSE;}
{FOR} {return FOR;}
{WHILE} {return WHILE;}
{BREAK}
{CONTINUE}
{FUNCTION}
{RETURN}
{LOCAL}
{AND}
{OR}
{NOT}
{TRUE}
{FALSE}
{NIL} {return NIL;}

{ASSIGN}|{ADD}|{SUB}|{INC}|{DEC}|{MUL}|{LE}|{DIV}|{MOD}|{EQUAL}|{NEQ}|{GT}|{LT}|{GE} {
    alpha_token_t* new_token = malloc(sizeof(struct alpha_token_t));
    new_token->lineno = yylineno;
    new_token->tokenno = tokenno++;
    new_token->content = yytext;
    new_token->type = operator_type(yytext);
    new_token->category = "<-enumerated";
    yylval->next = new_token;

    fprintf(yyout, "%d: #%d \"%s\" %s %s\n", new_token->lineno, \
            new_token->tokenno, new_token->content, new_token->type, \
            new_token->category);

    yylval = yylval->next;
}

{LCURLY}|{RCURLY}|{LBRACKET}|{RBRACKET}|{LPAREN}|{RPAREN}|{SEMICOLON}|{COLON}|{SCOPE}|{POINT}|{RANGE}|{COMMA} {
    alpha_token_t* new_token = malloc(sizeof(struct alpha_token_t));
    new_token->lineno = yylineno;
    new_token->tokenno = tokenno++;
    new_token->content = yytext;
    new_token->type = punctuation_type(yytext);
    new_token->category = "<-enumerated";
    yylval->next = new_token;

    fprintf(yyout, "%d: #%d \"%s\" %s %s\n", new_token->lineno, \
            new_token->tokenno, new_token->content, new_token->type, \
            new_token->category);

    yylval = yylval->next;
}

{ID}            {
    alpha_token_t* new_token = malloc(sizeof(struct alpha_token_t));
    new_token->lineno = yylineno;
    new_token->tokenno = tokenno++;
    new_token->content = yytext;
    new_token->type = malloc(5+sizeof(yytext));

    strcat(new_token->type, "ID \"");
    strcat(new_token->type, yytext);
    strcat(new_token->type, "\"");

    new_token->category = "<-char*";
    yylval->next = new_token;

    fprintf(yyout, "%d: #%d \"%s\" %s %s\n", new_token->lineno, \
            new_token->tokenno, new_token->content, new_token->type, \
            new_token->category);

    yylval = yylval->next;
}

{COMMENT}       {
    alpha_token_t* new_token = malloc(sizeof(struct alpha_token_t));
    new_token->lineno = yylineno;
    new_token->tokenno = tokenno++;
    new_token->content = yytext;
    new_token->type = "COMMENT LINE_COMMENT";
    new_token->category = "<-enumerated";
    yylval->next = new_token;

    fprintf(yyout, "%d: #%d \"%s\" %s %s\n", new_token->lineno, \
            new_token->tokenno, new_token->content, new_token->type, \
            new_token->category);

    yylval = yylval->next;
}

"/*"    start_line = yylineno; end_line = yylineno; BEGIN(IN_COMMENT);

<IN_COMMENT>"*/" {
    if (tmp_nest > 0) tmp_nest--;
    else {
        BEGIN(INITIAL);
        alpha_token_t* new_token = malloc(sizeof(struct alpha_token_t));
        new_token->lineno = yylineno;
        new_token->tokenno = tokenno++;
        char* start = (char*) malloc(sizeof(int));
        char* end = (char*) malloc(sizeof(int));
        sprintf(start, "%d", start_line);
        sprintf(end, "%d", end_line);
        new_token->content = malloc(sizeof(3 + sizeof(start) + sizeof(end)));

        strcat(new_token->content, start);
        strcat(new_token->content, " - ");
        strcat(new_token->content, end);

        new_token->type = "COMMENT BLOCK_COMMENT";
        new_token->category = "<-enumerated";
        yylval->next = new_token;

        fprintf(yyout, "%d: #%d \"%s\" %s %s\n", new_token->lineno, \
                new_token->tokenno, new_token->content, new_token->type, \
                new_token->category);

        yylval = yylval->next;
    }
}

<IN_COMMENT>"/*" {
    alpha_token_t* new_token = malloc(sizeof(struct alpha_token_t));
    new_token->lineno = yylineno;
    new_token->tokenno = tokenno++;
    new_token->content = "-";
    new_token->type = "COMMENT NESTED_COMMENT";
    new_token->category = "<-enumerated";
    yylval->next = new_token;

    fprintf(yyout, "%d: #%d \"%s\" %s %s\n", new_token->lineno, \
            new_token->tokenno, new_token->content, new_token->type, \
            new_token->category);

    yylval = yylval->next;
    tmp_nest++;
}

<IN_COMMENT>\n      end_line++;
<IN_COMMENT>.       {}
<IN_COMMENT><<EOF>> { BEGIN(INITIAL); fprintf(yyout, "Open multiline comment at EOF\n"); }

{WS}            {}
.               { fprintf(yyout, "Unknown identifier %s at line %d\n", yytext, yylineno); }


%%


char* keyword_type(char* yytext) {
    if (strcmp(yytext, "if") == 0) return "KEYWORD IF";
    if (strcmp(yytext, "else") == 0) return "KEYWORD ELSE";
    if (strcmp(yytext, "for") == 0) return "KEYWORD FOR";
    if (strcmp(yytext, "while") == 0) return "KEYWORD WHILE";
    if (strcmp(yytext, "break") == 0) return "KEYWORD BREAK";
    if (strcmp(yytext, "continue") == 0) return "KEYWORD CONTINUE";
    if (strcmp(yytext, "local") == 0) return "KEYWORD LOCAL";
    if (strcmp(yytext, "function") == 0) return "KEYWORD FUNCTION";
    if (strcmp(yytext, "return") == 0) return "KEYWORD RETURN";
    if (strcmp(yytext, "and") == 0) return "KEYWORD AND";
    if (strcmp(yytext, "or") == 0) return "KEYWORD OR";
    if (strcmp(yytext, "not") == 0) return "KEYWORD NOT";
    if (strcmp(yytext, "true") == 0) return "KEYWORD TRUE";
    if (strcmp(yytext, "false") == 0) return "KEYWORD FALSE";
    if (strcmp(yytext, "nil") == 0) return "KEYWORD NIL";
}

char* operator_type(char* yytext) {
    if (strcmp(yytext, "=") == 0) return "OPERATOR ASSIGNMENT";
    if (strcmp(yytext, "+") == 0) return "OPERATOR ADD";
    if (strcmp(yytext, "++") == 0) return "OPERATOR INCREMENT";
    if (strcmp(yytext, "-") == 0) return "OPERATOR SUB";
    if (strcmp(yytext, "--") == 0) return "OPERATOR DECREMENT";
    if (strcmp(yytext, "*") == 0) return "OPERATOR MULTIPLICATION";
    if (strcmp(yytext, "/") == 0) return "OPERATOR DIV";
    if (strcmp(yytext, "%") == 0) return "OPERATOR MOD";
    if (strcmp(yytext, "==") == 0) return "OPERATOR EQUAL";
    if (strcmp(yytext, "!=") == 0) return "OPERATOR NOT EQUAL";
    if (strcmp(yytext, ">") == 0) return "OPERATOR GREATER THAN";
    if (strcmp(yytext, ">=") == 0) return "OPERATOR GREATER EQUAL";
    if (strcmp(yytext, "<") == 0) return "OPERATOR LESS THAN";
    if (strcmp(yytext, "<=") == 0) return "OPERATOR LESS EQUAL";
}

char* punctuation_type(char* yytext) {
    if (strcmp(yytext, "{") == 0) return "PUNCTUATION LEFT CURLY BRACKET";
    if (strcmp(yytext, "}") == 0) return "PUNCTUATION RIGHT CURLY BRACKET";
    if (strcmp(yytext, "[") == 0) return "PUNCTUATION LEFT BRACKET";
    if (strcmp(yytext, "]") == 0) return "PUNCTUATION RIGHT BRACKET";
    if (strcmp(yytext, "(") == 0) return "PUNCTUATION RIGHT PARENTHESIS";
    if (strcmp(yytext, ")") == 0) return "PUNCTUATION LEFT PARENTHESIS";
    if (strcmp(yytext, ";") == 0) return "PUNCTUATION SEMICOLON";
    if (strcmp(yytext, ":") == 0) return "PUNCTUATION COLON";
    if (strcmp(yytext, "::") == 0) return "PUNCTUATION SCOPE";
    if (strcmp(yytext, ",") == 0) return "PUNCTUATION COMMA";
    if (strcmp(yytext, ".") == 0) return "PUNCTUATION POINT";
    if (strcmp(yytext, "..") == 0) return "PUNCTUATION RANGE";
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

    alpha_token_t* yylval = malloc(sizeof(alpha_token_t));
    alpha_token_t* head = yylval;
    fprintf(yyout, "------------- Lexical Analysis ------------\n");
    alpha_yylex(yylval);

    return 0;
}
