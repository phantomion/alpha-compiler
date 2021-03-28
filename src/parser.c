/* A Bison parser, made by GNU Bison 3.7.3.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2020 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.7.3"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 1 "parser.y"


    #include <stdio.h>
    #include <string.h>

    int yyerror(char* message);
    int yylex();
    int scope = 0;

    extern int yylineno;
    extern char* yytext;
    extern FILE* yyin;
    extern FILE* yyout;
    #define MAX 509
    #define HASH_MULTIPLIER 65599
    #define null NULL

    typedef struct Variable {
        const char* name;
        unsigned int scope;
        unsigned int line;
    }Variable;

    typedef struct Function {
        const char* name;
        const char* args;
        unsigned int scope;
        unsigned int line;
    }Function;

    enum SymbolType {
        GLOBAL, LOCALVAR, FORMAL,
        USERFUNC, LIBFUNC
    };

    typedef struct SymbolTableEntry {
        short int isActive;
        union {
            Variable* varVal;
            Function* funcVal;
        }value;
        enum SymbolType type;
        struct SymbolTableEntry* next;
        struct SymbolTableEntry* next_in_scope;
    }SymbolTableEntry;

    SymbolTableEntry* symtable[MAX];
    SymbolTableEntry* scope_link[MAX];

    char* itoa(int val);
    unsigned int symtable_hash(const char *pcKey);
    int symtable_insert(Variable* var, Function* func, enum SymbolType type);
    int hide_scope(const unsigned int scope);
    int symtable_contains(const Variable* var, const Function* func, enum SymbolType type);
    int scope_contains(const Variable* var, const Function* func, enum SymbolType type);
    SymbolTableEntry* symtable_lookup(const Variable* var, const Function* func, enum SymbolType type);
    SymbolTableEntry* scope_lookup(const Variable* var, const Function* func, enum SymbolType type);


#line 131 "./src/parser.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "parser.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_ID = 3,                         /* ID  */
  YYSYMBOL_TRUE = 4,                       /* TRUE  */
  YYSYMBOL_FALSE = 5,                      /* FALSE  */
  YYSYMBOL_NIL = 6,                        /* NIL  */
  YYSYMBOL_IF = 7,                         /* IF  */
  YYSYMBOL_ELSE = 8,                       /* ELSE  */
  YYSYMBOL_WHILE = 9,                      /* WHILE  */
  YYSYMBOL_FOR = 10,                       /* FOR  */
  YYSYMBOL_FUNCTION = 11,                  /* FUNCTION  */
  YYSYMBOL_RETURN = 12,                    /* RETURN  */
  YYSYMBOL_BREAK = 13,                     /* BREAK  */
  YYSYMBOL_CONTINUE = 14,                  /* CONTINUE  */
  YYSYMBOL_LOCAL = 15,                     /* LOCAL  */
  YYSYMBOL_AND = 16,                       /* AND  */
  YYSYMBOL_OR = 17,                        /* OR  */
  YYSYMBOL_NOT = 18,                       /* NOT  */
  YYSYMBOL_NUMBER = 19,                    /* NUMBER  */
  YYSYMBOL_STRING = 20,                    /* STRING  */
  YYSYMBOL_REAL = 21,                      /* REAL  */
  YYSYMBOL_ASSIGN = 22,                    /* ASSIGN  */
  YYSYMBOL_COMMENT = 23,                   /* COMMENT  */
  YYSYMBOL_ADD = 24,                       /* ADD  */
  YYSYMBOL_INC = 25,                       /* INC  */
  YYSYMBOL_SUB = 26,                       /* SUB  */
  YYSYMBOL_DEC = 27,                       /* DEC  */
  YYSYMBOL_MUL = 28,                       /* MUL  */
  YYSYMBOL_DIV = 29,                       /* DIV  */
  YYSYMBOL_MOD = 30,                       /* MOD  */
  YYSYMBOL_EQUAL = 31,                     /* EQUAL  */
  YYSYMBOL_NEQ = 32,                       /* NEQ  */
  YYSYMBOL_GT = 33,                        /* GT  */
  YYSYMBOL_LT = 34,                        /* LT  */
  YYSYMBOL_GE = 35,                        /* GE  */
  YYSYMBOL_LE = 36,                        /* LE  */
  YYSYMBOL_LCURLY = 37,                    /* LCURLY  */
  YYSYMBOL_RCURLY = 38,                    /* RCURLY  */
  YYSYMBOL_LBRACKET = 39,                  /* LBRACKET  */
  YYSYMBOL_RBRACKET = 40,                  /* RBRACKET  */
  YYSYMBOL_LPAREN = 41,                    /* LPAREN  */
  YYSYMBOL_RPAREN = 42,                    /* RPAREN  */
  YYSYMBOL_SEMICOLON = 43,                 /* SEMICOLON  */
  YYSYMBOL_COMMA = 44,                     /* COMMA  */
  YYSYMBOL_COLON = 45,                     /* COLON  */
  YYSYMBOL_SCOPE = 46,                     /* SCOPE  */
  YYSYMBOL_POINT = 47,                     /* POINT  */
  YYSYMBOL_RANGE = 48,                     /* RANGE  */
  YYSYMBOL_PRINT = 49,                     /* PRINT  */
  YYSYMBOL_INPUT = 50,                     /* INPUT  */
  YYSYMBOL_OBJECTMEMBERKEYS = 51,          /* OBJECTMEMBERKEYS  */
  YYSYMBOL_OBJECTTOTALMEMBERS = 52,        /* OBJECTTOTALMEMBERS  */
  YYSYMBOL_OBJECTCOPY = 53,                /* OBJECTCOPY  */
  YYSYMBOL_TOTALARGUMENTS = 54,            /* TOTALARGUMENTS  */
  YYSYMBOL_ARGUMENT = 55,                  /* ARGUMENT  */
  YYSYMBOL_TYPEOF = 56,                    /* TYPEOF  */
  YYSYMBOL_STRTONUM = 57,                  /* STRTONUM  */
  YYSYMBOL_SQRT = 58,                      /* SQRT  */
  YYSYMBOL_COS = 59,                       /* COS  */
  YYSYMBOL_SIN = 60,                       /* SIN  */
  YYSYMBOL_MUL_COMMENT = 61,               /* MUL_COMMENT  */
  YYSYMBOL_YYACCEPT = 62,                  /* $accept  */
  YYSYMBOL_program = 63,                   /* program  */
  YYSYMBOL_stmt = 64,                      /* stmt  */
  YYSYMBOL_stmt_list = 65,                 /* stmt_list  */
  YYSYMBOL_expr = 66,                      /* expr  */
  YYSYMBOL_term = 67,                      /* term  */
  YYSYMBOL_assignexpr = 68,                /* assignexpr  */
  YYSYMBOL_primary = 69,                   /* primary  */
  YYSYMBOL_lvalue = 70,                    /* lvalue  */
  YYSYMBOL_member = 71,                    /* member  */
  YYSYMBOL_call = 72,                      /* call  */
  YYSYMBOL_callsuffix = 73,                /* callsuffix  */
  YYSYMBOL_normcall = 74,                  /* normcall  */
  YYSYMBOL_methodcall = 75,                /* methodcall  */
  YYSYMBOL_76_1 = 76,                      /* $@1  */
  YYSYMBOL_elist = 77,                     /* elist  */
  YYSYMBOL_commaexpr = 78,                 /* commaexpr  */
  YYSYMBOL_objectdef = 79,                 /* objectdef  */
  YYSYMBOL_indexed = 80,                   /* indexed  */
  YYSYMBOL_indexelemlist = 81,             /* indexelemlist  */
  YYSYMBOL_indexelem = 82,                 /* indexelem  */
  YYSYMBOL_funcdef = 83,                   /* funcdef  */
  YYSYMBOL_84_2 = 84,                      /* $@2  */
  YYSYMBOL_block = 85,                     /* block  */
  YYSYMBOL_86_3 = 86,                      /* $@3  */
  YYSYMBOL_libraryfuncs = 87,              /* libraryfuncs  */
  YYSYMBOL_const = 88,                     /* const  */
  YYSYMBOL_idlist = 89,                    /* idlist  */
  YYSYMBOL_90_4 = 90,                      /* $@4  */
  YYSYMBOL_commaidlist = 91,               /* commaidlist  */
  YYSYMBOL_ifstmt = 92,                    /* ifstmt  */
  YYSYMBOL_whilestmt = 93,                 /* whilestmt  */
  YYSYMBOL_forstmt = 94,                   /* forstmt  */
  YYSYMBOL_returnstmt = 95,                /* returnstmt  */
  YYSYMBOL_comment = 96                    /* comment  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_uint8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && ! defined __ICC && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                            \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  86
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   985

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  62
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  35
/* YYNRULES -- Number of rules.  */
#define YYNRULES  109
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  198

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   316


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   154,   154,   156,   157,   158,   159,   160,   161,   162,
     163,   164,   165,   166,   170,   171,   175,   176,   177,   178,
     179,   180,   181,   182,   183,   184,   185,   186,   187,   188,
     189,   190,   194,   195,   196,   197,   198,   199,   200,   201,
     205,   208,   209,   210,   211,   212,   216,   217,   218,   219,
     223,   224,   225,   226,   230,   231,   232,   233,   237,   238,
     242,   245,   245,   248,   249,   253,   254,   258,   259,   263,
     267,   268,   272,   275,   275,   284,   288,   288,   289,   293,
     293,   293,   293,   294,   294,   294,   294,   295,   295,   295,
     295,   299,   299,   299,   299,   299,   299,   302,   302,   303,
     307,   308,   312,   313,   317,   320,   323,   324,   327,   328
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "ID", "TRUE", "FALSE",
  "NIL", "IF", "ELSE", "WHILE", "FOR", "FUNCTION", "RETURN", "BREAK",
  "CONTINUE", "LOCAL", "AND", "OR", "NOT", "NUMBER", "STRING", "REAL",
  "ASSIGN", "COMMENT", "ADD", "INC", "SUB", "DEC", "MUL", "DIV", "MOD",
  "EQUAL", "NEQ", "GT", "LT", "GE", "LE", "LCURLY", "RCURLY", "LBRACKET",
  "RBRACKET", "LPAREN", "RPAREN", "SEMICOLON", "COMMA", "COLON", "SCOPE",
  "POINT", "RANGE", "PRINT", "INPUT", "OBJECTMEMBERKEYS",
  "OBJECTTOTALMEMBERS", "OBJECTCOPY", "TOTALARGUMENTS", "ARGUMENT",
  "TYPEOF", "STRTONUM", "SQRT", "COS", "SIN", "MUL_COMMENT", "$accept",
  "program", "stmt", "stmt_list", "expr", "term", "assignexpr", "primary",
  "lvalue", "member", "call", "callsuffix", "normcall", "methodcall",
  "$@1", "elist", "commaexpr", "objectdef", "indexed", "indexelemlist",
  "indexelem", "funcdef", "$@2", "block", "$@3", "libraryfuncs", "const",
  "idlist", "$@4", "commaidlist", "ifstmt", "whilestmt", "forstmt",
  "returnstmt", "comment", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_int16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316
};
#endif

#define YYPACT_NINF (-150)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-65)

#define yytable_value_is_error(Yyn) \
  ((Yyn) == YYTABLE_NINF)

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     238,  -150,  -150,  -150,  -150,  -150,   -36,   -19,    -9,     0,
     421,    -7,     6,    31,   661,  -150,  -150,  -150,  -150,   681,
     661,   681,     5,   109,   481,  -150,    35,  -150,  -150,  -150,
    -150,  -150,  -150,  -150,  -150,  -150,  -150,  -150,  -150,  -150,
      53,   176,  -150,   747,  -150,  -150,  -150,   410,  -150,    -2,
    -150,  -150,  -150,    18,  -150,  -150,  -150,  -150,  -150,  -150,
     661,   661,   541,  -150,    57,  -150,   768,  -150,  -150,  -150,
      41,    50,    15,    -2,   482,    15,  -150,   299,   661,   726,
      24,    26,    47,   810,    51,  -150,  -150,  -150,   661,   661,
     661,   661,   661,   661,   661,   661,   661,   661,   661,   661,
     661,  -150,   661,  -150,  -150,   661,   601,    91,    98,  -150,
    -150,  -150,   661,   100,  -150,   601,   831,   852,    61,    66,
    -150,    67,  -150,    69,    70,    -5,   661,  -150,  -150,  -150,
      89,  -150,  -150,    90,   949,   936,   482,   482,    86,    86,
      86,   106,   106,    64,    64,    64,    64,    41,   873,   101,
    -150,  -150,   894,  -150,   102,   360,   360,   661,    57,    88,
      96,    90,  -150,   661,   726,    47,   601,  -150,  -150,   104,
    -150,  -150,   143,  -150,   789,   110,   151,  -150,  -150,   915,
    -150,  -150,   115,   601,   360,   601,    96,    88,  -150,  -150,
     128,  -150,   129,  -150,  -150,  -150,   360,  -150
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       0,    31,    46,    94,    95,    93,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    91,    92,    96,   108,     0,
       0,     0,    76,     0,     0,    12,     0,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    90,   109,
       0,     0,     2,     0,    30,    16,    33,    41,    49,    42,
      43,    11,    10,     0,    45,     4,     5,     6,     7,    13,
       0,     0,     0,    73,    99,   107,     0,     8,     9,    47,
      35,     0,    36,     0,    34,    38,    78,     0,     0,    66,
       0,     0,    71,     0,     0,    48,     1,    14,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     3,     0,    37,    39,     0,     0,     0,     0,    55,
      58,    59,     0,     0,    54,     0,     0,     0,     0,     0,
      97,     0,   106,     0,     0,     0,     0,    63,    67,    68,
       0,    69,    32,    44,    28,    29,    17,    18,    19,    20,
      21,    26,    27,    22,    24,    23,    25,    40,     0,     0,
      50,    61,     0,    52,     0,     0,     0,     0,    99,   101,
       0,     0,    77,     0,    66,    71,     0,    51,    60,     0,
      53,    57,   102,   104,     0,     0,     0,    98,    75,     0,
      65,    70,     0,     0,     0,     0,     0,   101,    72,    56,
       0,   103,     0,    74,   100,    62,     0,   105
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -150,  -150,  -149,   -33,   -10,  -150,  -150,  -150,    -4,  -150,
      -1,  -150,   -40,  -150,  -150,   -60,     8,  -150,  -150,     9,
      48,   -23,  -150,  -144,  -150,  -150,  -150,    34,  -150,    -3,
    -150,  -150,  -150,  -150,  -150
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,   109,   110,   111,   169,    80,   127,    50,    81,   131,
      82,    51,   119,    52,    77,    53,    54,   121,   159,   177,
      55,    56,    57,    58,    59
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      66,    84,   118,    63,    70,    60,   172,   173,    87,   114,
      74,    88,    89,    79,    83,    72,   178,    75,    73,    90,
      73,    91,    61,    92,    93,    94,    95,    96,    97,    98,
      99,   100,    62,   114,    69,   191,    67,   112,    85,   106,
     163,    64,   193,    76,   124,   113,   149,   197,   123,    68,
     116,   117,    79,    86,   105,   154,   106,    88,    89,   115,
     120,     9,   107,   108,   128,    90,   129,    91,   125,    92,
      93,    94,    95,    96,    97,    98,    99,   100,   134,   135,
     136,   137,   138,   139,   140,   141,   142,   143,   144,   145,
     146,   130,   147,   133,   150,   148,    79,   -65,   -65,   -65,
     -65,   151,   152,   153,   157,    79,   182,   158,   162,   160,
       1,   161,     2,     3,     4,     5,   164,    95,    96,    97,
      98,    99,   100,   190,    13,   192,    78,    14,    15,    16,
      17,   166,   176,    22,    19,    20,    21,   -65,   -65,    97,
      98,    99,   100,   168,   171,   183,    78,   174,    23,   -64,
      24,   184,   186,   179,   187,    26,    79,   189,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
     195,   196,   180,    79,   181,    79,   -15,     1,   165,     2,
       3,     4,     5,     6,   194,     7,     8,     9,    10,    11,
      12,    13,   175,     0,    14,    15,    16,    17,     0,    18,
       0,    19,    20,    21,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    22,   -15,    23,     0,    24,     0,    25,
       0,     0,    26,     0,     0,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,   -15,     1,
       0,     2,     3,     4,     5,     6,     0,     7,     8,     9,
      10,    11,    12,    13,     0,     0,    14,    15,    16,    17,
       0,    18,     0,    19,    20,    21,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    22,     0,    23,     0,    24,
       0,    25,     0,     0,    26,     0,     0,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
       1,     0,     2,     3,     4,     5,     6,     0,     7,     8,
       9,    10,    11,    12,    13,     0,     0,    14,    15,    16,
      17,     0,    18,     0,    19,    20,    21,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    22,   -15,    23,     0,
      24,     0,    25,     0,     0,    26,     0,     0,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,     1,     0,     2,     3,     4,     5,     6,     0,     7,
       8,     9,    10,    11,    12,    13,     0,     0,    14,    15,
      16,    17,     0,    18,     0,    19,    20,    21,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    22,     0,    23,
       0,    24,     0,    25,     0,     0,    26,     0,     0,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,     1,     0,     2,     3,     4,     5,     0,     0,
       0,     0,   102,     0,     0,   103,    13,   104,     0,    14,
      15,    16,    17,     0,     0,     0,    19,    20,    21,   105,
       0,   106,     0,     0,     0,     0,     0,   107,   108,     0,
      23,     0,    24,     0,    65,     0,     0,    26,     0,     0,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,     1,     0,     2,     3,     4,     5,     0,     0,
       0,     0,     9,     0,     0,     0,    13,     0,     0,    14,
      15,    16,    17,     0,     0,     0,    19,    20,    21,     0,
      92,    93,    94,    95,    96,    97,    98,    99,   100,     0,
      23,     0,    24,     0,     0,     0,     0,    26,     0,     0,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,     1,     0,     2,     3,     4,     5,     0,     0,
       0,     0,     0,     0,     0,     0,    13,     0,     0,    14,
      15,    16,    17,     0,     0,     0,    19,    20,    21,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      23,     0,    24,     0,   -64,     0,     0,    26,     0,     0,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,     1,     0,     2,     3,     4,     5,     0,     0,
       0,     0,     0,     0,     0,     0,    13,     0,     0,    14,
      15,    16,    17,     0,     0,     0,    19,    20,    21,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      23,     0,    24,   -64,     0,     0,     0,    26,     0,     0,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,     1,     0,     2,     3,     4,     5,     0,     0,
       0,     0,     0,     0,     0,     0,    13,     0,     0,    14,
      15,    16,    17,     0,     2,     0,    19,    20,    21,     0,
       0,     0,     0,     0,     0,     0,    13,     0,     0,     0,
      23,     0,    24,     0,     0,     0,     0,    26,     0,     0,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    71,     0,     0,     0,     0,    26,     0,     0,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    88,    89,     0,     0,     0,     0,     0,     0,
      90,     0,    91,     0,    92,    93,    94,    95,    96,    97,
      98,    99,   100,    88,    89,     0,     0,     0,     0,     0,
     126,    90,     0,    91,     0,    92,    93,    94,    95,    96,
      97,    98,    99,   100,    88,    89,     0,     0,     0,     0,
     101,     0,    90,     0,    91,     0,    92,    93,    94,    95,
      96,    97,    98,    99,   100,    88,    89,     0,     0,     0,
       0,   122,     0,    90,     0,    91,     0,    92,    93,    94,
      95,    96,    97,    98,    99,   100,    88,    89,     0,     0,
       0,     0,   185,     0,    90,     0,    91,     0,    92,    93,
      94,    95,    96,    97,    98,    99,   100,    88,    89,     0,
       0,     0,   132,     0,     0,    90,     0,    91,     0,    92,
      93,    94,    95,    96,    97,    98,    99,   100,    88,    89,
       0,     0,     0,   155,     0,     0,    90,     0,    91,     0,
      92,    93,    94,    95,    96,    97,    98,    99,   100,    88,
      89,     0,     0,     0,   156,     0,     0,    90,     0,    91,
       0,    92,    93,    94,    95,    96,    97,    98,    99,   100,
      88,    89,     0,   167,     0,     0,     0,     0,    90,     0,
      91,     0,    92,    93,    94,    95,    96,    97,    98,    99,
     100,    88,    89,     0,   170,     0,     0,     0,     0,    90,
       0,    91,     0,    92,    93,    94,    95,    96,    97,    98,
      99,   100,    88,   188,     0,     0,     0,     0,     0,     0,
      90,     0,    91,     0,    92,    93,    94,    95,    96,    97,
      98,    99,   100,    90,     0,    91,     0,    92,    93,    94,
      95,    96,    97,    98,    99,   100
};

static const yytype_int16 yycheck[] =
{
      10,    24,    62,     3,    14,    41,   155,   156,    41,    49,
      20,    16,    17,    23,    24,    19,   160,    21,    19,    24,
      21,    26,    41,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    41,    73,     3,   184,    43,    39,     3,    41,
      45,    41,   186,    38,    77,    47,   106,   196,    71,    43,
      60,    61,    62,     0,    39,   115,    41,    16,    17,    41,
       3,    11,    47,    48,    40,    24,    40,    26,    78,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    88,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,    44,   102,    42,     3,   105,   106,    33,    34,    35,
      36,     3,   112,     3,    43,   115,   166,    41,    38,    42,
       1,    42,     3,     4,     5,     6,   126,    31,    32,    33,
      34,    35,    36,   183,    15,   185,    37,    18,    19,    20,
      21,    41,    44,    37,    25,    26,    27,    31,    32,    33,
      34,    35,    36,    42,    42,    41,    37,   157,    39,    40,
      41,     8,    42,   163,     3,    46,   166,    42,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      42,    42,   164,   183,   165,   185,     0,     1,   130,     3,
       4,     5,     6,     7,   187,     9,    10,    11,    12,    13,
      14,    15,   158,    -1,    18,    19,    20,    21,    -1,    23,
      -1,    25,    26,    27,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    37,    38,    39,    -1,    41,    -1,    43,
      -1,    -1,    46,    -1,    -1,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,     0,     1,
      -1,     3,     4,     5,     6,     7,    -1,     9,    10,    11,
      12,    13,    14,    15,    -1,    -1,    18,    19,    20,    21,
      -1,    23,    -1,    25,    26,    27,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    37,    -1,    39,    -1,    41,
      -1,    43,    -1,    -1,    46,    -1,    -1,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
       1,    -1,     3,     4,     5,     6,     7,    -1,     9,    10,
      11,    12,    13,    14,    15,    -1,    -1,    18,    19,    20,
      21,    -1,    23,    -1,    25,    26,    27,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    37,    38,    39,    -1,
      41,    -1,    43,    -1,    -1,    46,    -1,    -1,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      61,     1,    -1,     3,     4,     5,     6,     7,    -1,     9,
      10,    11,    12,    13,    14,    15,    -1,    -1,    18,    19,
      20,    21,    -1,    23,    -1,    25,    26,    27,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    37,    -1,    39,
      -1,    41,    -1,    43,    -1,    -1,    46,    -1,    -1,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,     1,    -1,     3,     4,     5,     6,    -1,    -1,
      -1,    -1,    22,    -1,    -1,    25,    15,    27,    -1,    18,
      19,    20,    21,    -1,    -1,    -1,    25,    26,    27,    39,
      -1,    41,    -1,    -1,    -1,    -1,    -1,    47,    48,    -1,
      39,    -1,    41,    -1,    43,    -1,    -1,    46,    -1,    -1,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,     1,    -1,     3,     4,     5,     6,    -1,    -1,
      -1,    -1,    11,    -1,    -1,    -1,    15,    -1,    -1,    18,
      19,    20,    21,    -1,    -1,    -1,    25,    26,    27,    -1,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    -1,
      39,    -1,    41,    -1,    -1,    -1,    -1,    46,    -1,    -1,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,     1,    -1,     3,     4,     5,     6,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    15,    -1,    -1,    18,
      19,    20,    21,    -1,    -1,    -1,    25,    26,    27,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      39,    -1,    41,    -1,    43,    -1,    -1,    46,    -1,    -1,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,     1,    -1,     3,     4,     5,     6,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    15,    -1,    -1,    18,
      19,    20,    21,    -1,    -1,    -1,    25,    26,    27,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      39,    -1,    41,    42,    -1,    -1,    -1,    46,    -1,    -1,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,     1,    -1,     3,     4,     5,     6,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    15,    -1,    -1,    18,
      19,    20,    21,    -1,     3,    -1,    25,    26,    27,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    15,    -1,    -1,    -1,
      39,    -1,    41,    -1,    -1,    -1,    -1,    46,    -1,    -1,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    41,    -1,    -1,    -1,    -1,    46,    -1,    -1,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    16,    17,    -1,    -1,    -1,    -1,    -1,    -1,
      24,    -1,    26,    -1,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    16,    17,    -1,    -1,    -1,    -1,    -1,
      44,    24,    -1,    26,    -1,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    16,    17,    -1,    -1,    -1,    -1,
      43,    -1,    24,    -1,    26,    -1,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    16,    17,    -1,    -1,    -1,
      -1,    43,    -1,    24,    -1,    26,    -1,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    16,    17,    -1,    -1,
      -1,    -1,    43,    -1,    24,    -1,    26,    -1,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    16,    17,    -1,
      -1,    -1,    42,    -1,    -1,    24,    -1,    26,    -1,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    16,    17,
      -1,    -1,    -1,    42,    -1,    -1,    24,    -1,    26,    -1,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    16,
      17,    -1,    -1,    -1,    42,    -1,    -1,    24,    -1,    26,
      -1,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      16,    17,    -1,    40,    -1,    -1,    -1,    -1,    24,    -1,
      26,    -1,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    16,    17,    -1,    40,    -1,    -1,    -1,    -1,    24,
      -1,    26,    -1,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    16,    38,    -1,    -1,    -1,    -1,    -1,    -1,
      24,    -1,    26,    -1,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    24,    -1,    26,    -1,    28,    29,    30,
      31,    32,    33,    34,    35,    36
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     1,     3,     4,     5,     6,     7,     9,    10,    11,
      12,    13,    14,    15,    18,    19,    20,    21,    23,    25,
      26,    27,    37,    39,    41,    43,    46,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      63,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      79,    83,    85,    87,    88,    92,    93,    94,    95,    96,
      41,    41,    41,     3,    41,    43,    66,    43,    43,     3,
      66,    41,    70,    72,    66,    70,    38,    86,    37,    66,
      77,    80,    82,    66,    83,     3,     0,    65,    16,    17,
      24,    26,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    43,    22,    25,    27,    39,    41,    47,    48,    73,
      74,    75,    39,    47,    74,    41,    66,    66,    77,    84,
       3,    89,    43,    83,    65,    66,    44,    78,    40,    40,
      44,    81,    42,    42,    66,    66,    66,    66,    66,    66,
      66,    66,    66,    66,    66,    66,    66,    66,    66,    77,
       3,     3,    66,     3,    77,    42,    42,    43,    41,    90,
      42,    42,    38,    45,    66,    82,    41,    40,    42,    76,
      40,    42,    64,    64,    66,    89,    44,    91,    85,    66,
      78,    81,    77,    41,     8,    43,    42,     3,    38,    42,
      77,    64,    77,    85,    91,    42,    42,    64
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_int8 yyr1[] =
{
       0,    62,    63,    64,    64,    64,    64,    64,    64,    64,
      64,    64,    64,    64,    65,    65,    66,    66,    66,    66,
      66,    66,    66,    66,    66,    66,    66,    66,    66,    66,
      66,    66,    67,    67,    67,    67,    67,    67,    67,    67,
      68,    69,    69,    69,    69,    69,    70,    70,    70,    70,
      71,    71,    71,    71,    72,    72,    72,    72,    73,    73,
      74,    76,    75,    77,    77,    78,    78,    79,    79,    80,
      81,    81,    82,    84,    83,    83,    86,    85,    85,    87,
      87,    87,    87,    87,    87,    87,    87,    87,    87,    87,
      87,    88,    88,    88,    88,    88,    88,    90,    89,    89,
      91,    91,    92,    92,    93,    94,    95,    95,    96,    96
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     2,     1,     1,     1,     1,     2,     2,
       1,     1,     1,     1,     2,     0,     1,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       1,     1,     3,     1,     2,     2,     2,     2,     2,     2,
       3,     1,     1,     1,     3,     1,     1,     2,     2,     1,
       3,     4,     3,     4,     2,     2,     6,     4,     1,     1,
       3,     0,     6,     2,     0,     3,     0,     3,     3,     2,
       3,     0,     5,     0,     7,     5,     0,     4,     2,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     0,     3,     0,
       3,     0,     5,     7,     5,     9,     3,     2,     1,     1
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)

/* This macro is provided for backward compatibility. */
# ifndef YY_LOCATION_PRINT
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif


# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YYUSE (yyoutput);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yykind < YYNTOKENS)
    YYPRINT (yyo, yytoknum[yykind], *yyvaluep);
# endif
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)]);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YYUSE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */
  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    goto yyexhaustedlab;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 31: /* expr: error  */
#line 190 "parser.y"
                    {yyclearin;}
#line 1509 "./src/parser.c"
    break;

  case 46: /* lvalue: ID  */
#line 216 "parser.y"
               {printf("id %s line %d\n", (yyvsp[0].strVal), yylineno);}
#line 1515 "./src/parser.c"
    break;

  case 47: /* lvalue: LOCAL ID  */
#line 217 "parser.y"
                       {printf("Local id %s line %d\n", (yyvsp[0].strVal), yylineno);}
#line 1521 "./src/parser.c"
    break;

  case 48: /* lvalue: SCOPE ID  */
#line 218 "parser.y"
                       {printf("Global id %s line %d\n", (yyvsp[0].strVal), yylineno);}
#line 1527 "./src/parser.c"
    break;

  case 50: /* member: lvalue POINT ID  */
#line 223 "parser.y"
                            {printf("Member id %s at line %d\n", (yyvsp[0].strVal), yylineno);}
#line 1533 "./src/parser.c"
    break;

  case 61: /* $@1: %empty  */
#line 245 "parser.y"
                     {printf("Method call %s at line %d\n", (yyvsp[0].strVal), yylineno);}
#line 1539 "./src/parser.c"
    break;

  case 73: /* $@2: %empty  */
#line 275 "parser.y"
                        {printf("function with id %s line %d\n", (yyvsp[0].strVal), yylineno);}
#line 1545 "./src/parser.c"
    break;

  case 74: /* funcdef: FUNCTION ID $@2 LPAREN idlist RPAREN block  */
#line 275 "parser.y"
                                                                                                            {
       //la8os h apo katw grammh
       /*printf("Function with id: %s\n", $2);*/
           /*Function* f = malloc(sizeof(struct Function));*/
           /*f->name = $2;*/
           /*f->scope = scope;*/
           /*f->line = yylineno;*/
           /*symtable_insert(null, f, USERFUNC);*/
       }
#line 1559 "./src/parser.c"
    break;

  case 75: /* funcdef: FUNCTION LPAREN idlist RPAREN block  */
#line 284 "parser.y"
                                                  {}
#line 1565 "./src/parser.c"
    break;

  case 76: /* $@3: %empty  */
#line 288 "parser.y"
                   {scope++;}
#line 1571 "./src/parser.c"
    break;

  case 77: /* block: LCURLY $@3 stmt_list RCURLY  */
#line 288 "parser.y"
                                               {hide_scope(scope--);}
#line 1577 "./src/parser.c"
    break;

  case 97: /* $@4: %empty  */
#line 302 "parser.y"
               {printf("Argument %s at line %d\n", (yyvsp[0].strVal), yylineno);}
#line 1583 "./src/parser.c"
    break;


#line 1587 "./src/parser.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
    }

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;


#if !defined yyoverflow
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturn;
#endif


/*-------------------------------------------------------.
| yyreturn -- parsing is finished, clean up and return.  |
`-------------------------------------------------------*/
yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 332 "parser.y"


int yyerror(char* message) {
    fprintf(yyout, "%s at line %d\n", message, yylineno);
    return 1;
}


// Int to String implementation because C
// IT MAY BE USELESS WE'LL SEE
char* itoa(int val){

	static char buf[32] = {0};
	int i = 30;

	for(; val && i; --i, val /= 10)
		buf[i] = "0123456789abcdef"[val % 10];

	return &buf[i+1];
}


// Hash Function
unsigned int symtable_hash(const char *pcKey) {

  	size_t ui;
  	unsigned int uiHash = 0U;

  	for (ui = 0U; pcKey[ui] != '\0'; ui++)
    	uiHash = uiHash * HASH_MULTIPLIER + pcKey[ui];

  	return uiHash%MAX;
}


// Insert an entry to the Symbol Table AND the Scope Link
// !!! Anything can be added except if every field (name, scope, type) is the same, then return error !!!
// ^^^^ That must be fixed once we understand the rules for definitions and redeclarations
int symtable_insert(Variable* var, Function* func, enum SymbolType type) {

    unsigned int hash = symtable_hash(var ? var->name : func->name);

	SymbolTableEntry* table_entry = symtable[hash];
	SymbolTableEntry* scope_entry = scope_link[var ? var->scope : func->scope];
	SymbolTableEntry* table_prev = null;
	SymbolTableEntry* scope_prev = null;
	SymbolTableEntry* new;

	new = malloc(sizeof(SymbolTableEntry));
    new->isActive = 1;
    new->value.varVal = var;
    new->value.funcVal = func;
    new->type = type;
	new->next = null;
    new->next_in_scope = null;

	while(table_entry) {

        if(var && table_entry->value.varVal){
            if(strcmp(var->name, table_entry->value.varVal->name) == 0 && var->scope == table_entry->value.varVal->scope && type == table_entry->type){
                free(new);
			    return 0;
            }
        }
        else if(func && table_entry->value.funcVal){
            if(strcmp(func->name, table_entry->value.funcVal->name) == 0 && func->scope == table_entry->value.funcVal->scope && type == table_entry->type){
                free(new);
			    return 0;
            }
        }

		table_prev = table_entry;
		table_entry = table_entry->next;
	}

	while(scope_entry) {

        if(var && scope_entry->value.varVal){
            if(strcmp(var->name, scope_entry->value.varVal->name) == 0 && var->scope == scope_entry->value.varVal->scope && type == scope_entry->type){
                free(new);
			    return 0;
            }
        }
        else if(func && scope_entry->value.funcVal){
            if(strcmp(func->name, scope_entry->value.funcVal->name) == 0 && func->scope == scope_entry->value.funcVal->scope && type == scope_entry->type){
                free(new);
			    return 0;
            }
        }

		scope_prev = scope_entry;
		scope_entry = scope_entry->next_in_scope;
	}

	if   (table_prev) table_prev->next = new;
	else symtable[hash] = new;

	if   (scope_prev) scope_prev->next_in_scope = new;
	else scope_link[var ? var->scope : func->scope] = new;

	return 1;
}


// Hide all entries in a scope
int hide_scope(const unsigned int scope) {

	SymbolTableEntry* scope_entry = scope_link[scope];

	while(scope_entry) {
        scope_entry->isActive = 0;
        scope_entry = scope_entry->next_in_scope;
	}

	return 1;
}


// Return true if a bucket contains an entry
int symtable_contains(const Variable* var, const Function* func, enum SymbolType type){

	unsigned int hash = symtable_hash(var ? var->name : func->name);
	SymbolTableEntry* table_entry = symtable[hash];

	while(table_entry) {
        if(var && table_entry->value.varVal){
            if(strcmp(var->name, table_entry->value.varVal->name) == 0 && type == table_entry->type)
			    return 1;
        }
        else if(func && table_entry->value.funcVal){
            if(strcmp(func->name, table_entry->value.funcVal->name) == 0 && type == table_entry->type)
			    return 1;
        }

		table_entry = table_entry->next;
	}

	return 0;
}


// Return true if a scope contains an entry
int scope_contains(const Variable* var, const Function* func, enum SymbolType type){

	SymbolTableEntry* scope_entry = scope_link[var ? var->scope : func->scope];

	while(scope_entry) {
        if(var && scope_entry->value.varVal){
            if(strcmp(var->name, scope_entry->value.varVal->name) == 0 && type == scope_entry->type)
			    return 1;
        }
        else if(func && scope_entry->value.funcVal){
            if(strcmp(func->name, scope_entry->value.funcVal->name) == 0 && type == scope_entry->type)
			    return 1;
        }

		scope_entry = scope_entry->next_in_scope;
	}

	return 0;
}


// Return an entry from a bucket
SymbolTableEntry* symtable_lookup(const Variable* var, const Function* func, enum SymbolType type){

	unsigned int hash = symtable_hash(var ? var->name : func->name);
	SymbolTableEntry* table_entry = symtable[hash];

	while(table_entry) {
        if(var && table_entry->value.varVal){
            if(strcmp(var->name, table_entry->value.varVal->name) == 0 && type == table_entry->type)
			    return table_entry;
        }
        else if(func && table_entry->value.funcVal){
            if(strcmp(func->name, table_entry->value.funcVal->name) == 0 && type == table_entry->type)
			    return table_entry;
        }

		table_entry = table_entry->next;
	}

	return null;
}


// Return an entry from a scope
SymbolTableEntry* scope_lookup(const Variable* var, const Function* func, enum SymbolType type){

	SymbolTableEntry* scope_entry = scope_link[var ? var->scope : func->scope];

	while(scope_entry) {
        if(var && scope_entry->value.varVal){
            if(strcmp(var->name, scope_entry->value.varVal->name) == 0 && type == scope_entry->type)
			    return scope_entry;
        }
        else if(func && scope_entry->value.funcVal){
            if(strcmp(func->name, scope_entry->value.funcVal->name) == 0 && type == scope_entry->type)
			    return scope_entry;
        }

		scope_entry = scope_entry->next_in_scope;
	}

	return null;
}
