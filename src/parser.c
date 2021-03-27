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
    #include <stdlib.h>
    #include <string.h>

    int yyerror(char* message);
    int alpha_yylex(alpha_token_t *yylval);

    extern int yylineno;
    extern char* yytext;
    extern FILE* yyin;
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
        const char** args;
        unsigned int scope;
        unsigned int line;
    }Function;

    enum SymbolType {
        GLOBAL, LOCAL, FORMAL,
        USERFUNC, LIBFUNC
    };

    typedef struct SymbolTableEntry {
        const char* key;
        short int isActive;
        union {
            Variable* varVal;
            Function* funcVal;
        }value;
        enum SymbolType type;
        struct SymbolTableEntry* next;
    }SymbolTableEntry;

    SymbolTableEntry* symtable[MAX];

    unsigned int symTableHash(const char* key);
    int ins(const char *key, const void *pvValue);
    void* lookup(const char *key);
    void hide(const char* key);

#line 123 "./src/parser.c"

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
  YYSYMBOL_id = 3,                         /* id  */
  YYSYMBOL_4_ = 4,                         /* '='  */
  YYSYMBOL_5_not_ = 5,                     /* "not"  */
  YYSYMBOL_6_ = 6,                         /* "++"  */
  YYSYMBOL_7_ = 7,                         /* "--"  */
  YYSYMBOL_8_ = 8,                         /* ','  */
  YYSYMBOL_9_or_ = 9,                      /* "or"  */
  YYSYMBOL_10_and_ = 10,                   /* "and"  */
  YYSYMBOL_11_ = 11,                       /* '+'  */
  YYSYMBOL_12_ = 12,                       /* '-'  */
  YYSYMBOL_13_ = 13,                       /* '*'  */
  YYSYMBOL_14_ = 14,                       /* '/'  */
  YYSYMBOL_15_ = 15,                       /* '%'  */
  YYSYMBOL_16_ = 16,                       /* '.'  */
  YYSYMBOL_17_ = 17,                       /* ".."  */
  YYSYMBOL_18_ = 18,                       /* '['  */
  YYSYMBOL_19_ = 19,                       /* ']'  */
  YYSYMBOL_20_ = 20,                       /* '('  */
  YYSYMBOL_21_ = 21,                       /* ')'  */
  YYSYMBOL_22_ = 22,                       /* "=="  */
  YYSYMBOL_23_ = 23,                       /* "!="  */
  YYSYMBOL_24_ = 24,                       /* '>'  */
  YYSYMBOL_25_ = 25,                       /* ">="  */
  YYSYMBOL_26_ = 26,                       /* '<'  */
  YYSYMBOL_27_ = 27,                       /* "<="  */
  YYSYMBOL_28_ = 28,                       /* ';'  */
  YYSYMBOL_29_break_ = 29,                 /* "break"  */
  YYSYMBOL_30_continue_ = 30,              /* "continue"  */
  YYSYMBOL_31_local_ = 31,                 /* "local"  */
  YYSYMBOL_32_ = 32,                       /* "::"  */
  YYSYMBOL_33_ = 33,                       /* '{'  */
  YYSYMBOL_34_ = 34,                       /* ':'  */
  YYSYMBOL_35_ = 35,                       /* '}'  */
  YYSYMBOL_36_function_ = 36,              /* "function"  */
  YYSYMBOL_37_number_ = 37,                /* "number"  */
  YYSYMBOL_38_string_ = 38,                /* "string"  */
  YYSYMBOL_39_nil_ = 39,                   /* "nil"  */
  YYSYMBOL_40_true_ = 40,                  /* "true"  */
  YYSYMBOL_41_false_ = 41,                 /* "false"  */
  YYSYMBOL_42_if_ = 42,                    /* "if"  */
  YYSYMBOL_43_else_ = 43,                  /* "else"  */
  YYSYMBOL_44_while_ = 44,                 /* "while"  */
  YYSYMBOL_45_for_ = 45,                   /* "for"  */
  YYSYMBOL_46_return_ = 46,                /* "return"  */
  YYSYMBOL_YYACCEPT = 47,                  /* $accept  */
  YYSYMBOL_program = 48,                   /* program  */
  YYSYMBOL_stmt = 49,                      /* stmt  */
  YYSYMBOL_stmt_list = 50,                 /* stmt_list  */
  YYSYMBOL_expr = 51,                      /* expr  */
  YYSYMBOL_op = 52,                        /* op  */
  YYSYMBOL_term = 53,                      /* term  */
  YYSYMBOL_assignexpr = 54,                /* assignexpr  */
  YYSYMBOL_primary = 55,                   /* primary  */
  YYSYMBOL_lvalue = 56,                    /* lvalue  */
  YYSYMBOL_member = 57,                    /* member  */
  YYSYMBOL_call = 58,                      /* call  */
  YYSYMBOL_callsuffix = 59,                /* callsuffix  */
  YYSYMBOL_normcall = 60,                  /* normcall  */
  YYSYMBOL_methodcall = 61,                /* methodcall  */
  YYSYMBOL_elist = 62,                     /* elist  */
  YYSYMBOL_commaexpr = 63,                 /* commaexpr  */
  YYSYMBOL_objectdef = 64,                 /* objectdef  */
  YYSYMBOL_indexed = 65,                   /* indexed  */
  YYSYMBOL_indexelemlist = 66,             /* indexelemlist  */
  YYSYMBOL_indexelem = 67,                 /* indexelem  */
  YYSYMBOL_block = 68,                     /* block  */
  YYSYMBOL_funcdef = 69,                   /* funcdef  */
  YYSYMBOL_const = 70,                     /* const  */
  YYSYMBOL_idlist = 71,                    /* idlist  */
  YYSYMBOL_commaidlist = 72,               /* commaidlist  */
  YYSYMBOL_ifstmt = 73,                    /* ifstmt  */
  YYSYMBOL_whilestmt = 74,                 /* whilestmt  */
  YYSYMBOL_forstmt = 75,                   /* forstmt  */
  YYSYMBOL_returnstmt = 76                 /* returnstmt  */
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
#define YYFINAL  66
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   484

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  47
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  30
/* YYNRULES -- Number of rules.  */
#define YYNRULES  91
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  160

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   283


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
       2,     2,     2,     2,     2,     2,     2,    15,     2,     2,
      20,    21,    13,    11,     8,    12,    16,    14,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    34,    28,
      26,     4,    24,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    18,     2,    19,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    33,     2,    35,     2,     2,     2,     2,
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
       2,     2,     2,     2,     2,     2,     1,     2,     3,     5,
       6,     7,     9,    10,    17,    22,    23,    25,    27,    29,
      30,    31,    32,    36,    37,    38,    39,    40,    41,    42,
      43,    44,    45,    46
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint8 yyrline[] =
{
       0,    74,    74,    76,    77,    78,    79,    80,    81,    82,
      83,    84,    85,    86,    89,    90,    93,    94,    95,    98,
      98,    98,    98,    98,    98,    98,    98,    98,    98,    98,
      98,    98,   101,   102,   103,   104,   105,   106,   107,   108,
     111,   113,   114,   115,   116,   117,   120,   121,   122,   123,
     126,   127,   128,   129,   132,   133,   134,   137,   138,   141,
     143,   145,   146,   147,   150,   151,   154,   155,   158,   159,
     160,   163,   164,   167,   169,   170,   173,   175,   175,   175,
     175,   175,   177,   178,   179,   182,   183,   186,   187,   190,
     192,   194
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
  "\"end of file\"", "error", "\"invalid token\"", "id", "'='", "\"not\"",
  "\"++\"", "\"--\"", "','", "\"or\"", "\"and\"", "'+'", "'-'", "'*'",
  "'/'", "'%'", "'.'", "\"..\"", "'['", "']'", "'('", "')'", "\"==\"",
  "\"!=\"", "'>'", "\">=\"", "'<'", "\"<=\"", "';'", "\"break\"",
  "\"continue\"", "\"local\"", "\"::\"", "'{'", "':'", "'}'",
  "\"function\"", "\"number\"", "\"string\"", "\"nil\"", "\"true\"",
  "\"false\"", "\"if\"", "\"else\"", "\"while\"", "\"for\"", "\"return\"",
  "$accept", "program", "stmt", "stmt_list", "expr", "op", "term",
  "assignexpr", "primary", "lvalue", "member", "call", "callsuffix",
  "normcall", "methodcall", "elist", "commaexpr", "objectdef", "indexed",
  "indexelemlist", "indexelem", "block", "funcdef", "const", "idlist",
  "commaidlist", "ifstmt", "whilestmt", "forstmt", "returnstmt", YY_NULLPTR
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
       0,   256,   257,   258,    61,   259,   260,   261,    44,   262,
     263,    43,    45,    42,    47,    37,    46,   264,    91,    93,
      40,    41,   265,   266,    62,   267,    60,   268,    59,   269,
     270,   271,   272,   123,    58,   125,   273,   274,   275,   276,
     277,   278,   279,   280,   281,   282,   283
};
#endif

#define YYPACT_NINF (-65)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
      11,   -65,   205,    51,    51,   205,   150,   166,   -65,   -26,
     -22,     6,     9,   105,    -5,   -65,   -65,   -65,   -65,   -65,
      -1,    25,    38,    41,    64,    11,   -65,   283,   -65,   -65,
     -65,     4,   -65,    52,   -65,   -65,   -65,   -65,   -65,   -65,
     -65,   -65,   457,    29,    10,    52,    10,   206,   205,   304,
      54,    55,    67,   343,    57,   -65,   -65,   -65,   -65,   -65,
      44,    73,   205,   205,   205,   205,   -65,   -65,   -65,   -65,
     -65,   -65,   -65,   -65,   -65,   -65,   -65,   -65,   -65,   -65,
     -65,   -65,   205,   205,   -65,   -65,    81,    82,   205,   205,
     -65,   -65,   -65,    85,   205,   205,    69,   257,   205,   -65,
     -65,   -65,    58,   -65,   -65,    75,   -65,    90,    78,   362,
     381,    72,   400,   457,   457,   -65,    83,   419,    80,   -65,
     438,    84,    75,   205,   304,    67,   205,    99,   -65,    71,
      11,    11,   205,    79,   205,   -65,   -65,   -65,   -65,   238,
     -65,   -65,    88,    90,   -65,    63,   -65,   323,   -65,    92,
     -65,   -65,   -65,    11,   205,   -65,   -65,    93,    11,   -65
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_int8 yydefact[] =
{
      13,    46,     0,     0,     0,     0,    63,     0,    12,     0,
       0,     0,     0,     0,     0,    77,    78,    79,    80,    81,
       0,     0,     0,     0,     0,    13,     2,     0,    18,    16,
      39,    41,    49,    42,    43,    10,    11,    45,     4,     5,
       6,     7,    34,     0,    35,     0,    37,    33,     0,    61,
       0,     0,    68,     0,     0,     8,     9,    47,    48,    75,
       0,    84,     0,     0,    63,     0,     1,    14,    31,    30,
      19,    20,    21,    22,    23,    28,    29,    24,    25,    26,
      27,     3,     0,     0,    36,    38,     0,     0,     0,    63,
      55,    57,    58,     0,     0,    63,     0,     0,     0,    62,
      66,    67,     0,    69,    32,    44,    74,    82,     0,     0,
       0,     0,     0,    17,    40,    50,     0,     0,     0,    52,
       0,     0,     0,     0,    65,    72,    63,     0,    83,     0,
      13,    13,     0,     0,    63,    51,    59,    53,    54,     0,
      64,    71,     0,    86,    76,    87,    89,     0,    91,     0,
      73,    56,    85,    13,    63,    60,    88,     0,    13,    90
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -65,   -65,   -64,     0,    -2,   -65,   -65,   -65,   -65,    30,
     -65,    32,   -65,   -65,   -65,   -57,    -9,   -65,   -65,    -7,
      14,   -10,    -6,   -65,   -65,   -23,   -65,   -65,   -65,   -65
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    24,    25,    26,    27,    82,    28,    29,    30,    31,
      32,    33,    90,    91,    92,    50,    99,    34,    51,   103,
      52,    35,    36,    37,   108,   128,    38,    39,    40,    41
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_uint8 yytable[] =
{
      42,    54,    55,    47,    49,    53,    56,   111,    83,    57,
      84,    85,    58,    60,     1,    61,     2,     3,     4,    62,
      86,    87,    88,     5,    89,    67,    86,    87,    88,     6,
      89,     7,   118,    44,    46,    45,    45,    96,   121,     8,
       9,    10,    11,    12,    13,    63,    97,    14,    15,    16,
      17,    18,    19,    20,     1,    21,    22,    23,    64,    65,
     109,   110,    49,   112,    66,    14,   145,   146,    93,   142,
      94,    43,    95,   100,   101,   102,   107,   149,   105,   106,
     113,   114,    11,    12,   115,   116,   117,    49,   119,   156,
     122,    48,   120,    49,   159,   126,   124,   157,   127,   129,
     132,   136,   143,   134,    13,   138,   153,   148,     1,   151,
       2,     3,     4,   155,   158,   140,   125,     5,   141,   144,
     152,   139,     0,     6,    49,     7,     0,     0,     0,     0,
     147,     0,    49,     8,     9,    10,    11,    12,    13,     0,
      59,    14,    15,    16,    17,    18,    19,    20,     0,    21,
      22,    23,    49,     1,     0,     2,     3,     4,     0,     0,
       0,     0,     5,     0,     0,     0,     0,     0,     6,     1,
       7,     2,     3,     4,     0,     0,     0,     0,     5,     0,
       0,    11,    12,    48,     6,     0,     7,    15,    16,    17,
      18,    19,     0,     0,     0,     0,     0,    11,    12,     0,
       0,     0,    14,    15,    16,    17,    18,    19,     1,     0,
       2,     3,     4,     0,     0,     0,     0,     5,     0,    72,
      73,    74,     0,     6,     0,     7,     0,     0,    75,    76,
      77,    78,    79,    80,     0,     0,    11,    12,     0,     0,
       0,     0,    15,    16,    17,    18,    19,    68,    69,    70,
      71,    72,    73,    74,     0,     0,     0,     0,     0,     0,
      75,    76,    77,    78,    79,    80,    68,    69,    70,    71,
      72,    73,    74,   150,     0,     0,     0,     0,     0,    75,
      76,    77,    78,    79,    80,     0,     0,     0,     0,     0,
       0,   123,    68,    69,    70,    71,    72,    73,    74,     0,
       0,     0,     0,     0,     0,    75,    76,    77,    78,    79,
      80,    81,    98,    68,    69,    70,    71,    72,    73,    74,
       0,     0,     0,     0,     0,     0,    75,    76,    77,    78,
      79,    80,    68,    69,    70,    71,    72,    73,    74,     0,
       0,     0,     0,     0,     0,    75,    76,    77,    78,    79,
      80,   154,    68,    69,    70,    71,    72,    73,    74,     0,
       0,     0,     0,     0,   104,    75,    76,    77,    78,    79,
      80,    68,    69,    70,    71,    72,    73,    74,     0,     0,
       0,     0,     0,   130,    75,    76,    77,    78,    79,    80,
      68,    69,    70,    71,    72,    73,    74,     0,     0,     0,
       0,     0,   131,    75,    76,    77,    78,    79,    80,    68,
      69,    70,    71,    72,    73,    74,     0,     0,     0,   133,
       0,     0,    75,    76,    77,    78,    79,    80,    68,    69,
      70,    71,    72,    73,    74,     0,     0,     0,   135,     0,
       0,    75,    76,    77,    78,    79,    80,    68,    69,    70,
      71,    72,    73,    74,     0,     0,     0,   137,     0,     0,
      75,    76,    77,    78,    79,    80,    68,    69,    70,    71,
      72,    73,    74,     0,     0,     0,     0,     0,     0,    75,
      76,    77,    78,    79,    80
};

static const yytype_int16 yycheck[] =
{
       2,     7,    28,     5,     6,     7,    28,    64,     4,     3,
       6,     7,     3,    13,     3,    20,     5,     6,     7,    20,
      16,    17,    18,    12,    20,    25,    16,    17,    18,    18,
      20,    20,    89,     3,     4,     3,     4,    43,    95,    28,
      29,    30,    31,    32,    33,    20,    48,    36,    37,    38,
      39,    40,    41,    42,     3,    44,    45,    46,    20,    18,
      62,    63,    64,    65,     0,    36,   130,   131,    16,   126,
      18,    20,    20,    19,    19,     8,     3,   134,    21,    35,
      82,    83,    31,    32,     3,     3,    88,    89,     3,   153,
      21,    33,    94,    95,   158,    20,    98,   154,     8,    21,
      28,    21,     3,    20,    33,    21,    43,    28,     3,    21,
       5,     6,     7,    21,    21,   124,   102,    12,   125,   129,
     143,   123,    -1,    18,   126,    20,    -1,    -1,    -1,    -1,
     132,    -1,   134,    28,    29,    30,    31,    32,    33,    -1,
      35,    36,    37,    38,    39,    40,    41,    42,    -1,    44,
      45,    46,   154,     3,    -1,     5,     6,     7,    -1,    -1,
      -1,    -1,    12,    -1,    -1,    -1,    -1,    -1,    18,     3,
      20,     5,     6,     7,    -1,    -1,    -1,    -1,    12,    -1,
      -1,    31,    32,    33,    18,    -1,    20,    37,    38,    39,
      40,    41,    -1,    -1,    -1,    -1,    -1,    31,    32,    -1,
      -1,    -1,    36,    37,    38,    39,    40,    41,     3,    -1,
       5,     6,     7,    -1,    -1,    -1,    -1,    12,    -1,    13,
      14,    15,    -1,    18,    -1,    20,    -1,    -1,    22,    23,
      24,    25,    26,    27,    -1,    -1,    31,    32,    -1,    -1,
      -1,    -1,    37,    38,    39,    40,    41,     9,    10,    11,
      12,    13,    14,    15,    -1,    -1,    -1,    -1,    -1,    -1,
      22,    23,    24,    25,    26,    27,     9,    10,    11,    12,
      13,    14,    15,    35,    -1,    -1,    -1,    -1,    -1,    22,
      23,    24,    25,    26,    27,    -1,    -1,    -1,    -1,    -1,
      -1,    34,     9,    10,    11,    12,    13,    14,    15,    -1,
      -1,    -1,    -1,    -1,    -1,    22,    23,    24,    25,    26,
      27,    28,     8,     9,    10,    11,    12,    13,    14,    15,
      -1,    -1,    -1,    -1,    -1,    -1,    22,    23,    24,    25,
      26,    27,     9,    10,    11,    12,    13,    14,    15,    -1,
      -1,    -1,    -1,    -1,    -1,    22,    23,    24,    25,    26,
      27,    28,     9,    10,    11,    12,    13,    14,    15,    -1,
      -1,    -1,    -1,    -1,    21,    22,    23,    24,    25,    26,
      27,     9,    10,    11,    12,    13,    14,    15,    -1,    -1,
      -1,    -1,    -1,    21,    22,    23,    24,    25,    26,    27,
       9,    10,    11,    12,    13,    14,    15,    -1,    -1,    -1,
      -1,    -1,    21,    22,    23,    24,    25,    26,    27,     9,
      10,    11,    12,    13,    14,    15,    -1,    -1,    -1,    19,
      -1,    -1,    22,    23,    24,    25,    26,    27,     9,    10,
      11,    12,    13,    14,    15,    -1,    -1,    -1,    19,    -1,
      -1,    22,    23,    24,    25,    26,    27,     9,    10,    11,
      12,    13,    14,    15,    -1,    -1,    -1,    19,    -1,    -1,
      22,    23,    24,    25,    26,    27,     9,    10,    11,    12,
      13,    14,    15,    -1,    -1,    -1,    -1,    -1,    -1,    22,
      23,    24,    25,    26,    27
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     3,     5,     6,     7,    12,    18,    20,    28,    29,
      30,    31,    32,    33,    36,    37,    38,    39,    40,    41,
      42,    44,    45,    46,    48,    49,    50,    51,    53,    54,
      55,    56,    57,    58,    64,    68,    69,    70,    73,    74,
      75,    76,    51,    20,    56,    58,    56,    51,    33,    51,
      62,    65,    67,    51,    69,    28,    28,     3,     3,    35,
      50,    20,    20,    20,    20,    18,     0,    50,     9,    10,
      11,    12,    13,    14,    15,    22,    23,    24,    25,    26,
      27,    28,    52,     4,     6,     7,    16,    17,    18,    20,
      59,    60,    61,    16,    18,    20,    69,    51,     8,    63,
      19,    19,     8,    66,    21,    21,    35,     3,    71,    51,
      51,    62,    51,    51,    51,     3,     3,    51,    62,     3,
      51,    62,    21,    34,    51,    67,    20,     8,    72,    21,
      21,    21,    28,    19,    20,    19,    21,    19,    21,    51,
      63,    66,    62,     3,    68,    49,    49,    51,    28,    62,
      35,    21,    72,    43,    28,    21,    49,    62,    21,    49
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_int8 yyr1[] =
{
       0,    47,    48,    49,    49,    49,    49,    49,    49,    49,
      49,    49,    49,    49,    50,    50,    51,    51,    51,    52,
      52,    52,    52,    52,    52,    52,    52,    52,    52,    52,
      52,    52,    53,    53,    53,    53,    53,    53,    53,    53,
      54,    55,    55,    55,    55,    55,    56,    56,    56,    56,
      57,    57,    57,    57,    58,    58,    58,    59,    59,    60,
      61,    62,    62,    62,    63,    63,    64,    64,    65,    65,
      65,    66,    66,    67,    68,    68,    69,    70,    70,    70,
      70,    70,    71,    71,    71,    72,    72,    73,    73,    74,
      75,    76
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     2,     1,     1,     1,     1,     2,     2,
       1,     1,     1,     0,     2,     0,     1,     3,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     3,     2,     2,     2,     2,     2,     2,     1,
       3,     1,     1,     1,     3,     1,     1,     2,     2,     1,
       3,     4,     3,     4,     4,     2,     6,     1,     1,     3,
       5,     1,     2,     0,     3,     0,     3,     3,     1,     2,
       0,     3,     0,     5,     3,     2,     5,     1,     1,     1,
       1,     1,     1,     2,     0,     3,     0,     5,     7,     5,
       9,     5
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

#line 1354 "./src/parser.c"

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

#line 196 "parser.y"


char* itoa(int val, int base){

	static char buf[32] = {0};
	int i = 30;

	for(; val && i ; --i, val /= base)
		buf[i] = "0123456789abcdef"[val % base];

	return &buf[i+1];
}


char* get_key(const Variable* var, const Function* func) {

    char* key;

    if(var){
        strcpy(key, var->name);
        strcat(key, itoa(var->scope, 10));
    }
    else if(func){
        strcpy(key, func->name);
        strcat(key, itoa(func->scope, 10));
    }

    return key;
}


unsigned int SymTable_hash(const char *pcKey) {

  	size_t ui;
  	unsigned int uiHash = 0U;

  	for (ui = 0U; pcKey[ui] != '\0'; ui++)
    	uiHash = uiHash * HASH_MULTIPLIER + pcKey[ui];

  	return uiHash%MAX;
}


int SymTable_put(Variable* var, Function* func, enum SymbolType type) {

    unsigned int hash = SymTable_hash(get_key(var, func));

	SymbolTableEntry* binding = symtable[hash];
	SymbolTableEntry* prev = null;
	SymbolTableEntry* new;

	new = malloc(sizeof(SymbolTableEntry));
    new->isActive = 1;
    new->value.varVal = var;
    new->value.funcVal = func;
    new->type = type;
	new->next = null;

	while(binding) {
        unsigned int binding_hash = SymTable_hash(get_key(binding->value.varVal, binding->value.funcVal));

        if(binding_hash == hash){

            if(binding->isActive) return 0;
            new->next = binding->next;
            free(binding);
        }

		prev = binding;
		binding = binding->next;
	}

	if (prev) prev->next = new;
	else symtable[hash] = new;

	return 1;
}


int SymTable_hide(const Variable* var, const Function* func) {

    int hash = SymTable_hash(get_key(var, func));
	SymbolTableEntry* binding = symtable[hash];

	while(binding) {
        char* binding_key = get_key(binding->value.varVal, binding->value.funcVal);

        if(SymTable_hash(binding_key) == hash){
			binding->isActive = 0;
			return 1;
		}

        binding = binding->next;
	}

	return 0;
}


int SymTable_contains(const Variable* var, const Function* func){

	unsigned int hash = SymTable_hash(get_key(var, func));
	SymbolTableEntry* binding = symtable[hash];

	while (binding) {
        char* binding_key = get_key(binding->value.varVal, binding->value.funcVal);

        if(SymTable_hash(binding_key) == hash)
			return 1;

		binding = binding->next;
	}

	return 0;
}


SymbolTableEntry* SymTable_get(const Variable* var, const Function* func) {

	unsigned int hash = SymTable_hash(get_key(var, func));
	SymbolTableEntry* binding = symtable[hash];

	while (binding) {
        char* binding_key = get_key(binding->value.varVal, binding->value.funcVal);

        if(SymTable_hash(binding_key) == hash)
			return binding;

		binding = binding->next;
	}

	return null;
}
