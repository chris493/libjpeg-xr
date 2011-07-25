/* A Bison parser, made by GNU Bison 2.1. */

/* Skeleton parser for Yacc-like parsing with Bison,
Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005 Free Software Foundation, Inc.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor,
Boston, MA 02110-1301, USA. */

/* As a special exception, when this file is copied by Bison into a
Bison output file, you may use that output file without restriction.
This special exception was added by the Free Software Foundation
in version 1.24 of Bison. */

/* Written by Richard Stallman by simplifying the original so called
``semantic'' parser. */

/* All symbols defined below should begin with yy or YY, to avoid
infringing on user name space. This should be done even for local
variables, as they might otherwise be expanded by user macros.
There are some unavoidable exceptions within include files to
define necessary library symbols; they are noted "INFRINGES ON
USER NAME SPACE" below. */

/* Identify Bison output. */
#define YYBISON 1

/* Bison version. */
#define YYBISON_VERSION "2.1"

/* Skeleton name. */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers. */
#define YYPURE 0

/* Using locations. */
#define YYLSP_NEEDED 0

/* Substitute the variable and function names. */
#define yyparse qp_parse
#define yylex qp_lex
#define yyerror qp_error
#define yylval qp_lval
#define yychar qp_char
#define yydebug qp_debug
#define yynerrs qp_nerrs


/* Tokens. */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
/* Put the tokens into the symbol table, so that GDB and other debuggers
know about them. */
enum yytokentype{
    K_DC = 258,
    K_CHANNEL = 259,
    K_HP = 260,
    K_INDEPENDENT = 261,
    K_LP = 262,
    K_SEPARATE = 263,
    K_TILE = 264,
    K_UNIFORM = 265,
    NUMBER = 266
};
#endif
/* Tokens. */
#define K_DC 258
#define K_CHANNEL 259
#define K_HP 260
#define K_INDEPENDENT 261
#define K_LP 262
#define K_SEPARATE 263
#define K_TILE 264
#define K_UNIFORM 265
#define NUMBER 266




/* Copy the first part of user declarations. */
#line 2 "qp_parse.y"

/*************************************************************************
*
* This software module was originally contributed by Microsoft
* Corporation in the course of development of the
* ITU-T T.832 | ISO/IEC 29199-2 ("JPEG XR") format standard for
* reference purposes and its performance may not have been optimized.
*
* This software module is an implementation of one or more
* tools as specified by the JPEG XR standard.
*
* ITU/ISO/IEC give You a royalty-free, worldwide, non-exclusive
* copyright license to copy, distribute, and make derivative works
* of this software module or modifications thereof for use in
* products claiming conformance to the JPEG XR standard as
* specified by ITU-T T.832 | ISO/IEC 29199-2.
*
* ITU/ISO/IEC give users the same free license to this software
* module or modifications thereof for research purposes and further
* ITU/ISO/IEC standardization.
*
* Those intending to use this software module in products are advised
* that its use may infringe existing patents. ITU/ISO/IEC have no
* liability for use of this software module or modifications thereof.
*
* Copyright is not released for products that do not conform to
* to the JPEG XR standard as specified by ITU-T T.832 |
* ISO/IEC 29199-2.
*
* Microsoft Corporation retains full right to modify and use the code
* for its own purpose, to assign or donate the code to a third party,
* and to inhibit third parties from using the code for products that
* do not conform to the JPEG XR standard as specified by ITU-T T.832 |
* ISO/IEC 29199-2.
*
* This copyright notice must be included in all copies or derivative
* works.
*
* Copyright (c) ITU-T/ISO/IEC 2008.
***********************************************************************/


#ifdef _MSC_VER
#pragma comment (user,"$Id: qp_parse.y,v 1.8 2008/03/20 18:10:29 steve Exp $")
#else
#ident "$Id: qp_parse.y,v 1.8 2008/03/20 18:10:29 steve Exp $"
#endif

# include "jpegxr.h"
# include <stdlib.h>
# include <assert.h>
# define YYINCLUDED_STDLIB_H

extern int qp_lex(void);
static void yyerror(const char*txt);

static int errors = 0;

/* Parse state variables. */

/* The image we are getting QP values for */
static jxr_image_t cur_image;
/* Some characteristics of that image. */
static unsigned tile_columns;
static unsigned tile_rows;

/* The array of all the tiles to be calculated. */
static struct jxr_tile_qp* tile_qp = 0;

/* Tile currently in progress. */
static unsigned cur_tilex = 0;
static unsigned cur_tiley = 0;
static struct jxr_tile_qp*cur_tile = 0;

static unsigned mb_in_tile = 0;

static unsigned cur_channel = 0;



/* Enabling traces. */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages. */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table. */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif

#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
#line 45 "qp_parse.y"
typedef union YYSTYPE{
    unsigned number;

    struct{
        unsigned char*data;
        unsigned count;
    }map_list;

    struct sixteen_nums{
        unsigned char num;
        unsigned char qp[16];
    }qp_set;
}YYSTYPE;
/* Line 196 of yacc.c. */
#line 172 "qp.tab.c"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations. */


/* Line 219 of yacc.c. */
#line 184 "qp.tab.c"

#if ! defined (YYSIZE_T) && defined (__SIZE_TYPE__)
# define YYSIZE_T __SIZE_TYPE__
#endif
#if ! defined (YYSIZE_T) && defined (size_t)
# define YYSIZE_T size_t
#endif
#if ! defined (YYSIZE_T) && (defined (__STDC__) || defined (__cplusplus))
# include <stddef.h> /* INFRINGES ON USER NAME SPACE */
# define YYSIZE_T size_t
#endif
#if ! defined (YYSIZE_T)
# define YYSIZE_T unsigned int
#endif

#ifndef YY_
# if YYENABLE_NLS
# if ENABLE_NLS
# include <libintl.h> /* INFRINGES ON USER NAME SPACE */
# define YY_(msgid) dgettext ("bison-runtime", msgid)
# endif
# endif
# ifndef YY_
# define YY_(msgid) msgid
# endif
#endif

#if ! defined (yyoverflow) || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols. */

# ifdef YYSTACK_USE_ALLOCA
# if YYSTACK_USE_ALLOCA
# ifdef __GNUC__
# define YYSTACK_ALLOC __builtin_alloca
# else
# define YYSTACK_ALLOC alloca
# if defined (__STDC__) || defined (__cplusplus)
# include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
# define YYINCLUDED_STDLIB_H
# endif
# endif
# endif
# endif

# ifdef YYSTACK_ALLOC
/* Pacify GCC's `empty if-body' warning. */
# define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
# ifndef YYSTACK_ALLOC_MAXIMUM
/* The OS might guarantee only one guard page at the bottom of the stack,
and a page size can be as small as 4096 bytes. So we cannot safely
invoke alloca (N) if N exceeds 4096. Use a slightly smaller number
to allow for a few compiler-allocated temporary stack slots. */
# define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2005 */
# endif
# else
# define YYSTACK_ALLOC YYMALLOC
# define YYSTACK_FREE YYFREE
# ifndef YYSTACK_ALLOC_MAXIMUM
# define YYSTACK_ALLOC_MAXIMUM ((YYSIZE_T) -1)
# endif
# ifdef __cplusplus
extern "C"{
# endif
# ifndef YYMALLOC
# define YYMALLOC malloc
# if (! defined (malloc) && ! defined (YYINCLUDED_STDLIB_H) \
    && (defined (__STDC__) || defined (__cplusplus)))
    void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
# endif
# endif
# ifndef YYFREE
# define YYFREE free
# if (! defined (free) && ! defined (YYINCLUDED_STDLIB_H) \
    && (defined (__STDC__) || defined (__cplusplus)))
    void free (void *); /* INFRINGES ON USER NAME SPACE */
# endif
# endif
# ifdef __cplusplus
}
# endif
# endif
#endif /* ! defined (yyoverflow) || YYERROR_VERBOSE */


#if (! defined (yyoverflow) \
    && (! defined (__cplusplus) \
    || (defined (YYSTYPE_IS_TRIVIAL) && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member. */
union yyalloc
{
    short int yyss;
    YYSTYPE yyvs;
};

/* The size of the maximum gap between one aligned stack and the next. */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
N elements. */
# define YYSTACK_BYTES(N) \
    ((N) * (sizeof (short int) + sizeof (YYSTYPE)) \
    + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO. The source and destination do
not overlap. */
# ifndef YYCOPY
# if defined (__GNUC__) && 1 < __GNUC__
# define YYCOPY(To, From, Count) \
    __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
# else
# define YYCOPY(To, From, Count) \
    do \
{ \
    YYSIZE_T yyi; \
    for (yyi = 0; yyi < (Count); yyi++) \
    (To)[yyi] = (From)[yyi]; \
} \
    while (0)
# endif
# endif

/* Relocate STACK from its old location to the new one. The
local variables YYSIZE and YYSTACKSIZE give the old and new number of
elements in the stack, and YYPTR gives the new location of the
stack. Advance YYPTR to a properly aligned location for the next
stack. */
# define YYSTACK_RELOCATE(Stack) \
    do \
{ \
    YYSIZE_T yynewbytes; \
    YYCOPY (&yyptr->Stack, Stack, yysize); \
    Stack = &yyptr->Stack; \
    yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
    yyptr += yynewbytes / sizeof (*yyptr); \
} \
    while (0)

#endif

#if defined (__STDC__) || defined (__cplusplus)
typedef signed char yysigned_char;
#else
typedef short int yysigned_char;
#endif

/* YYFINAL -- State number of the termination state. */
#define YYFINAL 6
/* YYLAST -- Last index in YYTABLE. */
#define YYLAST 51

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS 19
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS 14
/* YYNRULES -- Number of rules. */
#define YYNRULES 26
/* YYNRULES -- Number of states. */
#define YYNSTATES 56

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX. */
#define YYUNDEFTOK 2
#define YYMAXUTOK 266

#define YYTRANSLATE(YYX) \
    ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX. */
static const unsigned char yytranslate[] =
{
    0, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    12, 14, 2, 2, 13, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 17, 2, 18, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 15, 2, 16, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 1, 2, 3, 4,
    5, 6, 7, 8, 9, 10, 11
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
YYRHS. */
static const unsigned char yyprhs[] =
{
    0, 0, 3, 5, 8, 10, 11, 23, 25, 27,
    29, 32, 34, 35, 42, 47, 52, 55, 57, 62,
    67, 72, 76, 78, 80, 81, 84
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const yysigned_char yyrhs[] =
{
    20, 0, -1, 21, -1, 21, 22, -1, 22, -1,
    -1, 9, 12, 11, 13, 11, 14, 23, 15, 24,
    25, 16, -1, 10, -1, 8, -1, 6, -1, 25,
    26, -1, 26, -1, -1, 4, 11, 27, 15, 28,
    16, -1, 7, 17, 32, 18, -1, 5, 17, 32,
    18, -1, 28, 29, -1, 29, -1, 3, 15, 11,
    16, -1, 7, 15, 30, 16, -1, 5, 15, 30,
    16, -1, 30, 31, 11, -1, 11, -1, 13, -1,
    -1, 32, 11, -1, 11, -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined. */
static const unsigned char yyrline[] =
{
    0, 70, 70, 73, 74, 81, 80, 132, 133, 134,
    138, 139, 144, 143, 151, 160, 171, 172, 176, 179,
    186, 196, 208, 214, 214, 217, 227
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
First, the terminals, then, starting at YYNTOKENS, nonterminals. */
static const char *const yytname[] =
{
    "$end", "error", "$undefined", "K_DC", "K_CHANNEL", "K_HP",
    "K_INDEPENDENT", "K_LP", "K_SEPARATE", "K_TILE", "K_UNIFORM", "NUMBER",
    "'('", "','", "')'", "'{'", "'}'", "'['", "']'", "$accept", "start",
    "tile_list", "tile", "@1", "tile_comp_mode", "tile_body", "tile_item",
    "@2", "channel_body", "channel_item", "lphp_qp_list", "optional_comma",
    "map_list", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
token YYLEX-NUM. */
static const unsigned short int yytoknum[] =
{
    0, 256, 257, 258, 259, 260, 261, 262, 263, 264,
    265, 266, 40, 44, 41, 123, 125, 91, 93
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives. */
static const unsigned char yyr1[] =
{
    0, 19, 20, 21, 21, 23, 22, 24, 24, 24,
    25, 25, 27, 26, 26, 26, 28, 28, 29, 29,
    29, 30, 30, 31, 31, 32, 32
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN. */
static const unsigned char yyr2[] =
{
    0, 2, 1, 2, 1, 0, 11, 1, 1, 1,
    2, 1, 0, 6, 4, 4, 2, 1, 4, 4,
    4, 3, 1, 1, 0, 2, 1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
STATE-NUM when YYTABLE doesn't specify something else to do. Zero
means the default is an error. */
static const unsigned char yydefact[] =
{
    0, 0, 0, 2, 4, 0, 1, 3, 0, 0,
    0, 5, 0, 0, 9, 8, 7, 0, 0, 0,
    0, 0, 11, 12, 0, 0, 6, 10, 0, 26,
    0, 0, 0, 25, 15, 14, 0, 0, 0, 0,
    17, 0, 0, 0, 13, 16, 0, 22, 24, 24,
    18, 23, 20, 0, 19, 21
};

/* YYDEFGOTO[NTERM-NUM]. */
static const yysigned_char yydefgoto[] =
{
    -1, 2, 3, 4, 12, 17, 21, 22, 28, 39,
    40, 48, 53, 30
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
STATE-NUM. */
#define YYPACT_NINF -12
static const yysigned_char yypact[] =
{
    0, -11, 11, 0, -12, 17, -12, -12, 9, 19,
    18, -12, 21, 10, -12, -12, -12, 20, 22, 14,
    23, 1, -12, -12, 24, 24, -12, -12, 26, -12,
    -8, -4, 16, -12, -12, -12, 27, 28, 29, -3,
    -12, 34, 35, 35, -12, -12, 31, -12, -1, 13,
    -12, -12, -12, 37, -12, -12
};

/* YYPGOTO[NTERM-NUM]. */
static const yysigned_char yypgoto[] =
{
    -12, -12, -12, 36, -12, -12, -12, 30, -12, -12,
    -5, -6, -12, 25
};

/* YYTABLE[YYPACT[STATE-NUM]]. What to do in state STATE-NUM. If
positive, shift that token. If negative, reduce the rule which
number is the opposite. If zero, do what YYDEFACT says.
If YYTABLE_NINF, syntax error. */
#define YYTABLE_NINF -1
static const unsigned char yytable[] =
{
    36, 5, 37, 33, 38, 18, 19, 33, 20, 1,
    34, 6, 51, 44, 35, 52, 14, 26, 15, 36,
    16, 37, 9, 38, 18, 19, 51, 20, 8, 54,
    10, 24, 11, 23, 45, 29, 13, 49, 0, 7,
    25, 32, 41, 42, 43, 46, 47, 50, 55, 0,
    31, 27
};

static const yysigned_char yycheck[] =
{
    3, 12, 5, 11, 7, 4, 5, 11, 7, 9,
    18, 0, 13, 16, 18, 16, 6, 16, 8, 3,
    10, 5, 13, 7, 4, 5, 13, 7, 11, 16,
    11, 17, 14, 11, 39, 11, 15, 43, -1, 3,
    17, 15, 15, 15, 15, 11, 11, 16, 11, -1,
    25, 21
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
symbol of state STATE-NUM. */
static const unsigned char yystos[] =
{
    0, 9, 20, 21, 22, 12, 0, 22, 11, 13,
    11, 14, 23, 15, 6, 8, 10, 24, 4, 5,
    7, 25, 26, 11, 17, 17, 16, 26, 27, 11,
    32, 32, 15, 11, 18, 18, 3, 5, 7, 28,
    29, 15, 15, 15, 16, 29, 11, 11, 30, 30,
    16, 13, 16, 31, 16, 11
};

#define yyerrok (yyerrstatus = 0)
#define yyclearin (yychar = YYEMPTY)
#define YYEMPTY (-2)
#define YYEOF 0

#define YYACCEPT goto yyacceptlab
#define YYABORT goto yyabortlab
#define YYERROR goto yyerrorlab


/* Like YYERROR except do call yyerror. This remains here temporarily
to ease the transition to the new meaning of YYERROR, for GCC.
Once GCC version 2 has supplanted version 1, this can go. */

#define YYFAIL goto yyerrlab

#define YYRECOVERING() (!!yyerrstatus)

#define YYBACKUP(Token, Value) \
    do \
    if (yychar == YYEMPTY && yylen == 1) \
{ \
    yychar = (Token); \
    yylval = (Value); \
    yytoken = YYTRANSLATE (yychar); \
    YYPOPSTACK; \
    goto yybackup; \
    } \
else \
{ \
    yyerror (YY_("syntax error: cannot back up")); \
    YYERROR; \
    } \
    while (0)


#define YYTERROR 1
#define YYERRCODE 256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
If N is 0, then set CURRENT to the empty location which ends
the previous symbol: RHS[0] (always defined). */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N) \
    do \
    if (N) \
{ \
    (Current).first_line = YYRHSLOC (Rhs, 1).first_line; \
    (Current).first_column = YYRHSLOC (Rhs, 1).first_column; \
    (Current).last_line = YYRHSLOC (Rhs, N).last_line; \
    (Current).last_column = YYRHSLOC (Rhs, N).last_column; \
    } \
else \
{ \
    (Current).first_line = (Current).last_line = \
    YYRHSLOC (Rhs, 0).last_line; \
    (Current).first_column = (Current).last_column = \
    YYRHSLOC (Rhs, 0).last_column; \
    } \
    while (0)
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
This macro was not mandated originally: define only if we know
we won't break user code: when these are the locations we know. */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
# define YY_LOCATION_PRINT(File, Loc) \
    fprintf (File, "%d.%d-%d.%d", \
    (Loc).first_line, (Loc).first_column, \
    (Loc).last_line, (Loc).last_column)
# else
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments. */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested. */
#if YYDEBUG

# ifndef YYFPRINTF
# include <stdio.h> /* INFRINGES ON USER NAME SPACE */
# define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args) \
    do { \
    if (yydebug) \
    YYFPRINTF Args; \
    } while (0)

# define YY_SYMBOL_PRINT(Title, Type, Value, Location) \
    do { \
    if (yydebug) \
{ \
    YYFPRINTF (stderr, "%s ", Title); \
    yysymprint (stderr, \
    Type, Value); \
    YYFPRINTF (stderr, "\n"); \
    } \
    } while (0)

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included). |
`------------------------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_stack_print (short int *bottom, short int *top)
#else
static void
yy_stack_print (bottom, top)
short int *bottom;
short int *top;
#endif
{
    YYFPRINTF (stderr, "Stack now");
    for (/* Nothing. */; bottom <= top; ++bottom)
        YYFPRINTF (stderr, " %d", *bottom);
    YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top) \
    do { \
    if (yydebug) \
    yy_stack_print ((Bottom), (Top)); \
    } while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced. |
`------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_reduce_print (int yyrule)
#else
static void
yy_reduce_print (yyrule)
int yyrule;
#endif
{
    int yyi;
    unsigned long int yylno = yyrline[yyrule];
    YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu), ",
        yyrule - 1, yylno);
    /* Print the symbols being reduced, and their result. */
    for (yyi = yyprhs[yyrule]; 0 <= yyrhs[yyi]; yyi++)
        YYFPRINTF (stderr, "%s ", yytname[yyrhs[yyi]]);
    YYFPRINTF (stderr, "-> %s\n", yytname[yyr1[yyrule]]);
}

# define YY_REDUCE_PRINT(Rule) \
    do { \
    if (yydebug) \
    yy_reduce_print (Rule); \
    } while (0)

/* Nonzero means print parse trace. It is left uninitialized so that
multiple parsers can coexist. */
int yydebug;
#else/* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks. */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
if the built-in stack extension method is used).

Do not make this value too large; the results are undefined if
YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
evaluated with infinite-precision integer arithmetic. */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
# if defined (__GLIBC__) && defined (_STRING_H)
# define yystrlen strlen
# else
/* Return the length of YYSTR. */
static YYSIZE_T
# if defined (__STDC__) || defined (__cplusplus)
yystrlen (const char *yystr)
# else
yystrlen (yystr)
const char *yystr;
# endif
{
    const char *yys = yystr;

    while (*yys++ != '\0')
        continue;

    return yys - yystr - 1;
}
# endif
# endif

# ifndef yystpcpy
# if defined (__GLIBC__) && defined (_STRING_H) && defined (_GNU_SOURCE)
# define yystpcpy stpcpy
# else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
YYDEST. */
static char *
# if defined (__STDC__) || defined (__cplusplus)
yystpcpy (char *yydest, const char *yysrc)
# else
yystpcpy (yydest, yysrc)
char *yydest;
const char *yysrc;
# endif
{
    char *yyd = yydest;
    const char *yys = yysrc;

    while ((*yyd++ = *yys++) != '\0')
        continue;

    return yyd - 1;
}
# endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
quotes and backslashes, so that it's suitable for yyerror. The
heuristic is that double-quoting is unnecessary unless the string
contains an apostrophe, a comma, or backslash (other than
backslash-backslash). YYSTR is taken from yytname. If YYRES is
null, do not copy; instead, return the length of what the result
would have been. */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
    if (*yystr == '"')
    {
        size_t yyn = 0;
        char const *yyp = yystr;

        for (;;)
            switch (*++yyp)
        {
            case '\'':
            case ',':
                goto do_not_strip_quotes;

            case '\\':
                if (*++yyp != '\\')
                    goto do_not_strip_quotes;
                /* Fall through. */
            default:
                if (yyres)
                    yyres[yyn] = *yyp;
                yyn++;
                break;

            case '"':
                if (yyres)
                    yyres[yyn] = '\0';
                return yyn;
        }
do_not_strip_quotes: ;
    }

    if (! yyres)
        return yystrlen (yystr);

    return yystpcpy (yyres, yystr) - yyres;
}
# endif

#endif /* YYERROR_VERBOSE */



#if YYDEBUG
/*--------------------------------.
| Print this symbol on YYOUTPUT. |
`--------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yysymprint (FILE *yyoutput, int yytype, YYSTYPE *yyvaluep)
#else
static void
yysymprint (yyoutput, yytype, yyvaluep)
FILE *yyoutput;
int yytype;
YYSTYPE *yyvaluep;
#endif
{
    /* Pacify ``unused variable'' warnings. */
    (void) yyvaluep;

    if (yytype < YYNTOKENS)
        YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
    else
        YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);


# ifdef YYPRINT
    if (yytype < YYNTOKENS)
        YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
    switch (yytype)
    {
    default:
        break;
    }
    YYFPRINTF (yyoutput, ")");
}

#endif /* ! YYDEBUG */
/*-----------------------------------------------.
| Release the memory associated to this symbol. |
`-----------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
const char *yymsg;
int yytype;
YYSTYPE *yyvaluep;
#endif
{
    /* Pacify ``unused variable'' warnings. */
    (void) yyvaluep;

    if (!yymsg)
        yymsg = "Deleting";
    YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

    switch (yytype)
    {

        default:
            break;
    }
}


/* Prevent warnings from -Wmissing-prototypes. */

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM);
# else
int yyparse ();
# endif
#else/* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */



/* The look-ahead symbol. */
int yychar;

/* The semantic value of the look-ahead symbol. */
YYSTYPE yylval;

/* Number of syntax errors so far. */
int yynerrs;



/*----------.
| yyparse. |
`----------*/

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM)
# else
int yyparse (YYPARSE_PARAM)
void *YYPARSE_PARAM;
# endif
#else/* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int
yyparse (void)
#else
int
yyparse ()
;
#endif
#endif
{

    int yystate;
    int yyn;
    int yyresult;
    /* Number of tokens to shift before error messages enabled. */
    int yyerrstatus;
    /* Look-ahead token as an internal (translated) token number. */
    int yytoken = 0;

    /* Three stacks and their tools:
    `yyss': related to states,
    `yyvs': related to semantic values,
    `yyls': related to locations.

    Refer to the stacks thru separate pointers, to allow yyoverflow
    to reallocate them elsewhere. */

    /* The state stack. */
    short int yyssa[YYINITDEPTH];
    short int *yyss = yyssa;
    short int *yyssp;

    /* The semantic value stack. */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp;



#define YYPOPSTACK (yyvsp--, yyssp--)

    YYSIZE_T yystacksize = YYINITDEPTH;

    /* The variables used to return semantic value and location from the
    action routines. */
    YYSTYPE yyval;


    /* When reducing, the number of symbols on the RHS of the reduced
    rule. */
    int yylen;

    YYDPRINTF ((stderr, "Starting parse\n"));

    yystate = 0;
    yyerrstatus = 0;
    yynerrs = 0;
    yychar = YYEMPTY; /* Cause a token to be read. */

    /* Initialize stack pointers.
    Waste one element of value and location stack
    so that they stay on the same level as the state stack.
    The wasted elements are never initialized. */

    yyssp = yyss;
    yyvsp = yyvs;

    goto yysetstate;

    /*------------------------------------------------------------.
    | yynewstate -- Push a new state, which is found in yystate. |
    `------------------------------------------------------------*/
yynewstate:
    /* In all cases, when you get here, the value and location stacks
    have just been pushed. so pushing a state here evens the stacks.
    */
    yyssp++;

yysetstate:
    *yyssp = yystate;

    if (yyss + yystacksize - 1 <= yyssp)
    {
        /* Get the current used size of the three stacks, in elements. */
        YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
        {
            /* Give user a chance to reallocate the stack. Use copies of
            these so that the &'s don't force the real ones into
            memory. */
            YYSTYPE *yyvs1 = yyvs;
            short int *yyss1 = yyss;


            /* Each stack pointer address is followed by the size of the
            data in use in that stack, in bytes. This used to be a
            conditional around just the two extra args, but that might
            be undefined if yyoverflow is a macro. */
            yyoverflow (YY_("memory exhausted"),
                &yyss1, yysize * sizeof (*yyssp),
                &yyvs1, yysize * sizeof (*yyvsp),

                &yystacksize);

            yyss = yyss1;
            yyvs = yyvs1;
        }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
        goto yyexhaustedlab;
# else
        /* Extend the stack our own way. */
        if (YYMAXDEPTH <= yystacksize)
            goto yyexhaustedlab;
        yystacksize *= 2;
        if (YYMAXDEPTH < yystacksize)
            yystacksize = YYMAXDEPTH;

        {
            short int *yyss1 = yyss;
            union yyalloc *yyptr =
                (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
            if (! yyptr)
                goto yyexhaustedlab;
            YYSTACK_RELOCATE (yyss);
            YYSTACK_RELOCATE (yyvs);

# undef YYSTACK_RELOCATE
            if (yyss1 != yyssa)
                YYSTACK_FREE (yyss1);
        }
# endif
#endif /* no yyoverflow */

        yyssp = yyss + yysize - 1;
        yyvsp = yyvs + yysize - 1;


        YYDPRINTF ((stderr, "Stack size increased to %lu\n",
            (unsigned long int) yystacksize));

        if (yyss + yystacksize - 1 <= yyssp)
            YYABORT;
    }

    YYDPRINTF ((stderr, "Entering state %d\n", yystate));

    goto yybackup;

    /*-----------.
    | yybackup. |
    `-----------*/
yybackup:

    /* Do appropriate processing given the current state. */
    /* Read a look-ahead token if we need one and don't already have one. */
    /* yyresume: */

    /* First try to decide what to do without reference to look-ahead token. */

    yyn = yypact[yystate];
    if (yyn == YYPACT_NINF)
        goto yydefault;

    /* Not known => get a look-ahead token if don't already have one. */

    /* YYCHAR is either YYEMPTY or YYEOF or a valid look-ahead symbol. */
    if (yychar == YYEMPTY)
    {
        YYDPRINTF ((stderr, "Reading a token: "));
        yychar = YYLEX;
    }

    if (yychar <= YYEOF)
    {
        yychar = yytoken = YYEOF;
        YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
    else
    {
        yytoken = YYTRANSLATE (yychar);
        YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

    /* If the proper action on seeing token YYTOKEN is to reduce or to
    detect an error, take that action. */
    yyn += yytoken;
    if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
        goto yydefault;
    yyn = yytable[yyn];
    if (yyn <= 0)
    {
        if (yyn == 0 || yyn == YYTABLE_NINF)
            goto yyerrlab;
        yyn = -yyn;
        goto yyreduce;
    }

    if (yyn == YYFINAL)
        YYACCEPT;

    /* Shift the look-ahead token. */
    YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

    /* Discard the token being shifted unless it is eof. */
    if (yychar != YYEOF)
        yychar = YYEMPTY;

    *++yyvsp = yylval;


    /* Count tokens shifted since error; after three, turn off error
    status. */
    if (yyerrstatus)
        yyerrstatus--;

    yystate = yyn;
    goto yynewstate;


    /*-----------------------------------------------------------.
    | yydefault -- do the default action for the current state. |
    `-----------------------------------------------------------*/
yydefault:
    yyn = yydefact[yystate];
    if (yyn == 0)
        goto yyerrlab;
    goto yyreduce;


    /*-----------------------------.
    | yyreduce -- Do a reduction. |
    `-----------------------------*/
yyreduce:
    /* yyn is the number of a rule to reduce with. */
    yylen = yyr2[yyn];

    /* If YYLEN is nonzero, implement the default value of the action:
    `$$ = $1'.

    Otherwise, the following line sets YYVAL to garbage.
    This behavior is undocumented and Bison
    users should not rely upon it. Assigning to YYVAL
    unconditionally makes the parser a bit smaller, and it avoids a
    GCC warning that YYVAL may be used uninitialized. */
    yyval = yyvsp[1-yylen];


    YY_REDUCE_PRINT (yyn);
    switch (yyn) {
        case 5:
#line 81 "qp_parse.y"
            { cur_tilex = (yyvsp[-3].number),
                cur_tiley = (yyvsp[-1].number);
            assert(cur_tilex < tile_columns);
            assert(cur_tiley < tile_rows);
            cur_tile = tile_qp + cur_tiley*tile_columns + cur_tilex;
            mb_in_tile = jxr_get_TILE_WIDTH(cur_image, cur_tilex) * jxr_get_TILE_HEIGHT(cur_image, cur_tiley);
            ;}
            break;

        case 6:
#line 89 "qp_parse.y"
            { /* Check the sanity of the calculated tile. */
                int idx;
                switch (cur_tile->component_mode) {
                    case JXR_CM_UNIFORM:
                        break;
                    case JXR_CM_SEPARATE:
                        if (cur_tile->channel[0].num_lp != cur_tile->channel[1].num_lp) {
                            fprintf(stderr, "channel-0 and channel-1 LP counts don't match\n");
                            errors += 1;
                        }
                        if (cur_tile->channel[0].num_hp != cur_tile->channel[1].num_hp) {
                            fprintf(stderr, "channel-0 and channel-1 HP counts don't match\n");
                            errors += 1;
                        }
                        if (jxr_get_IMAGE_CHANNELS(cur_image) == 1) {
                            fprintf(stderr, "Gray (1-channel) tiles must be channel mode UNIFORM\n");
                            errors += 1;
                        }
                        break;
                    case JXR_CM_INDEPENDENT:
                        for (idx = 1 ; idx < jxr_get_IMAGE_CHANNELS(cur_image) ; idx += 1) {
                            if (cur_tile->channel[0].num_lp != cur_tile->channel[idx].num_lp) {
                                fprintf(stderr, "channel-0 and channel-%d LP counts don't match\n", idx);
                                errors += 1;
                            }
                            if (cur_tile->channel[0].num_hp != cur_tile->channel[idx].num_hp) {
                                fprintf(stderr, "channel-0 and channel-%d HP counts don't match\n", idx);
                                errors += 1;
                            }
                        }
                        if (jxr_get_IMAGE_CHANNELS(cur_image) == 1) {
                            fprintf(stderr, "Gray (1-channel) tiles must be channel mode UNIFORM\n");
                            errors += 1;
                        }
                        break;
                    case JXR_CM_Reserved:
                        assert(0);
                        break;
                }
            ;}
        break;

        case 7:
#line 132 "qp_parse.y"
            { cur_tile->component_mode = JXR_CM_UNIFORM; ;}
            break;

        case 8:
#line 133 "qp_parse.y"
            { cur_tile->component_mode = JXR_CM_SEPARATE; ;}
            break;

        case 9:
#line 134 "qp_parse.y"
            { cur_tile->component_mode = JXR_CM_INDEPENDENT; ;}
            break;

        case 12:
#line 144 "qp_parse.y"
            { cur_channel = (yyvsp[0].number);
            assert(cur_channel < 16);
            cur_tile->channel[cur_channel].num_lp = 0;
            cur_tile->channel[cur_channel].num_hp = 0;
            ;}
            break;

        case 14:
#line 152 "qp_parse.y"
            { cur_tile->lp_map = (yyvsp[-1].map_list).data;
            if ((yyvsp[-1].map_list).count != mb_in_tile) {
                errors += 1;
                fprintf(stderr, "parse qp: In tile %u,%u, expected %u lp blocks, got %u.\n",
                    cur_tilex, cur_tiley, mb_in_tile, (yyvsp[-1].map_list).count);
            }
            ;}
            break;

        case 15:
#line 161 "qp_parse.y"
            { cur_tile->hp_map = (yyvsp[-1].map_list).data;
            if ((yyvsp[-1].map_list).count != mb_in_tile) {
                errors += 1;
                fprintf(stderr, "parse qp: In tile %u,%u, expected %u lp blocks, got %u.\n",
                    cur_tilex, cur_tiley, mb_in_tile, (yyvsp[-1].map_list).count);
            }
            ;}
            break;

        case 18:
#line 177 "qp_parse.y"
            { cur_tile->channel[cur_channel].dc_qp = (yyvsp[-1].number); ;}
            break;

        case 19:
#line 180 "qp_parse.y"
            { cur_tile->channel[cur_channel].num_lp = (yyvsp[-1].qp_set).num;
            int idx;
            for (idx = 0 ; idx < (yyvsp[-1].qp_set).num ; idx += 1)
                cur_tile->channel[cur_channel].lp_qp[idx] = (yyvsp[-1].qp_set).qp[idx];
            ;}
            break;

        case 20:
#line 187 "qp_parse.y"
            { cur_tile->channel[cur_channel].num_hp = (yyvsp[-1].qp_set).num;
            int idx;
            for (idx = 0 ; idx < (yyvsp[-1].qp_set).num ; idx += 1)
                cur_tile->channel[cur_channel].hp_qp[idx] = (yyvsp[-1].qp_set).qp[idx];
            ;}
            break;

        case 21:
#line 197 "qp_parse.y"
            { unsigned cnt = (yyvsp[-2].qp_set).num;
            if (cnt >= 16) {
                fprintf(stderr, "Too many (>16) QP values in QP list.\n");
                errors += 1;
            } else {
                assert(cnt < 16);
                (yyvsp[-2].qp_set).qp[cnt] = (yyvsp[0].number);
                (yyvsp[-2].qp_set).num += 1;
            }
            (yyval.qp_set) = (yyvsp[-2].qp_set);
            ;}
            break;

        case 22:
#line 209 "qp_parse.y"
            { (yyval.qp_set).num = 1;
            (yyval.qp_set).qp[0] = (yyvsp[0].number);
            ;}
            break;

        case 25:
#line 218 "qp_parse.y"
            { if ((yyvsp[-1].map_list).count >= mb_in_tile) {
                fprintf(stderr, "Too many map items for tile!\n");
                errors += 1;
            } else {
                (yyvsp[-1].map_list).data[(yyvsp[-1].map_list).count++] = (yyvsp[0].number);
            }
            (yyval.map_list) = (yyvsp[-1].map_list);
            ;}
            break;

        case 26:
#line 228 "qp_parse.y"
            { assert(mb_in_tile >= 1);
            (yyval.map_list).data = (unsigned char*)calloc(mb_in_tile, 1);
            (yyval.map_list).count = 1;
            (yyval.map_list).data[0] = (yyvsp[0].number);
            ;}
            break;


        default: break;
    }

    /* Line 1126 of yacc.c. */
#line 1373 "qp.tab.c"
    
    yyvsp -= yylen;
    yyssp -= yylen;


    YY_STACK_PRINT (yyss, yyssp);

    *++yyvsp = yyval;


    /* Now `shift' the result of the reduction. Determine what state
    that goes to, based on the state we popped back to and the rule
    number reduced by. */

    yyn = yyr1[yyn];

    yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
    if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
        yystate = yytable[yystate];
    else
        yystate = yydefgoto[yyn - YYNTOKENS];

    goto yynewstate;


    /*------------------------------------.
    | yyerrlab -- here on detecting error |
    `------------------------------------*/
yyerrlab:
    /* If not already recovering from an error, report this error. */
    if (!yyerrstatus)
    {
        ++yynerrs;
#if YYERROR_VERBOSE
        yyn = yypact[yystate];

        if (YYPACT_NINF < yyn && yyn < YYLAST)
        {
            int yytype = YYTRANSLATE (yychar);
            YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
            YYSIZE_T yysize = yysize0;
            YYSIZE_T yysize1;
            int yysize_overflow = 0;
            char *yymsg = 0;
# define YYERROR_VERBOSE_ARGS_MAXIMUM 5
            char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
            int yyx;

#if 0
            /* This is so xgettext sees the translatable formats that are
            constructed on the fly. */
            YY_("syntax error, unexpected %s");
            YY_("syntax error, unexpected %s, expecting %s");
            YY_("syntax error, unexpected %s, expecting %s or %s");
            YY_("syntax error, unexpected %s, expecting %s or %s or %s");
            YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
#endif
            char *yyfmt;
            char const *yyf;
            static char const yyunexpected[] = "syntax error, unexpected %s";
            static char const yyexpecting[] = ", expecting %s";
            static char const yyor[] = " or %s";
            char yyformat[sizeof yyunexpected
                + sizeof yyexpecting - 1
                + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
                * (sizeof yyor - 1))];
            char const *yyprefix = yyexpecting;

            /* Start YYX at -YYN if negative to avoid negative indexes in
            YYCHECK. */
            int yyxbegin = yyn < 0 ? -yyn : 0;

            /* Stay within bounds of both yycheck and yytname. */
            int yychecklim = YYLAST - yyn;
            int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
            int yycount = 1;

            yyarg[0] = yytname[yytype];
            yyfmt = yystpcpy (yyformat, yyunexpected);

            for (yyx = yyxbegin; yyx < yyxend; ++yyx)
                if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
                {
                    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                    {
                        yycount = 1;
                        yysize = yysize0;
                        yyformat[sizeof yyunexpected - 1] = '\0';
                        break;
                    }
                    yyarg[yycount++] = yytname[yyx];
                    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
                    yysize_overflow |= yysize1 < yysize;
                    yysize = yysize1;
                    yyfmt = yystpcpy (yyfmt, yyprefix);
                    yyprefix = yyor;
                }

                yyf = YY_(yyformat);
                yysize1 = yysize + yystrlen (yyf);
                yysize_overflow |= yysize1 < yysize;
                yysize = yysize1;

                if (!yysize_overflow && yysize <= YYSTACK_ALLOC_MAXIMUM)
                    yymsg = (char *) YYSTACK_ALLOC (yysize);
                if (yymsg)
                {
                    /* Avoid sprintf, as that infringes on the user's name space.
                    Don't have undefined behavior even if the translation
                    produced a string with the wrong number of "%s"s. */
                    char *yyp = yymsg;
                    int yyi = 0;
                    while ((*yyp = *yyf))
                    {
                        if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
                        {
                            yyp += yytnamerr (yyp, yyarg[yyi++]);
                            yyf += 2;
                        }
                        else
                        {
                            yyp++;
                            yyf++;
                        }
                    }
                    yyerror (yymsg);
                    YYSTACK_FREE (yymsg);
                }
                else
                {
                    yyerror (YY_("syntax error"));
                    goto yyexhaustedlab;
                }
        }
        else
#endif /* YYERROR_VERBOSE */
            yyerror (YY_("syntax error"));
    }



    if (yyerrstatus == 3)
    {
        /* If just tried and failed to reuse look-ahead token after an
        error, discard it. */

        if (yychar <= YYEOF)
        {
            /* Return failure if at end of input. */
            if (yychar == YYEOF)
                YYABORT;
        }
        else
        {
            yydestruct ("Error: discarding", yytoken, &yylval);
            yychar = YYEMPTY;
        }
    }

    /* Else will try to reuse look-ahead token after shifting the error
    token. */
    goto yyerrlab1;


    /*---------------------------------------------------.
    | yyerrorlab -- error raised explicitly by YYERROR. |
    `---------------------------------------------------*/
yyerrorlab:

    /* Pacify compilers like GCC when the user code never invokes
    YYERROR and the label yyerrorlab therefore never appears in user
    code. */
    if (0)
        goto yyerrorlab;

    yyvsp -= yylen;
    yyssp -= yylen;
    yystate = *yyssp;
    goto yyerrlab1;


    /*-------------------------------------------------------------.
    | yyerrlab1 -- common code for both syntax error and YYERROR. |
    `-------------------------------------------------------------*/
yyerrlab1:
    yyerrstatus = 3; /* Each real token shifted decrements this. */

    for (;;)
    {
        yyn = yypact[yystate];
        if (yyn != YYPACT_NINF)
        {
            yyn += YYTERROR;
            if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
            {
                yyn = yytable[yyn];
                if (0 < yyn)
                    break;
            }
        }

        /* Pop the current state because it cannot handle the error token. */
        if (yyssp == yyss)
            YYABORT;


        yydestruct ("Error: popping", yystos[yystate], yyvsp);
        YYPOPSTACK;
        yystate = *yyssp;
        YY_STACK_PRINT (yyss, yyssp);
    }

    if (yyn == YYFINAL)
        YYACCEPT;

    *++yyvsp = yylval;


    /* Shift the error token. */
    YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

    yystate = yyn;
    goto yynewstate;


    /*-------------------------------------.
    | yyacceptlab -- YYACCEPT comes here. |
    `-------------------------------------*/
yyacceptlab:
    yyresult = 0;
    goto yyreturn;

    /*-----------------------------------.
    | yyabortlab -- YYABORT comes here. |
    `-----------------------------------*/
yyabortlab:
    yyresult = 1;
    goto yyreturn;

#ifndef yyoverflow
    /*-------------------------------------------------.
    | yyexhaustedlab -- memory exhaustion comes here. |
    `-------------------------------------------------*/
yyexhaustedlab:
    yyerror (YY_("memory exhausted"));
    yyresult = 2;
    /* Fall through. */
#endif

yyreturn:
    if (yychar != YYEOF && yychar != YYEMPTY)
        yydestruct ("Cleanup: discarding lookahead",
        yytoken, &yylval);
    while (yyssp != yyss)
    {
        yydestruct ("Cleanup: popping",
            yystos[*yyssp], yyvsp);
        YYPOPSTACK;
    }
#ifndef yyoverflow
    if (yyss != yyssa)
        YYSTACK_FREE (yyss);
#endif
    return yyresult;
}


#line 235 "qp_parse.y"


extern void qp_restart(FILE*fd);

int qp_parse_file(FILE*fd, jxr_image_t image)
{
    cur_image = image;
    tile_columns = jxr_get_TILE_COLUMNS(image);
    tile_rows = jxr_get_TILE_ROWS(image);

    tile_qp = (struct jxr_tile_qp *) calloc(tile_columns*tile_rows, sizeof(*tile_qp));
    assert(tile_qp);

#if defined(DETAILED_DEBUG)
    printf("qp_parse: tile_columns=%u, tile_rows=%u\n", tile_columns, tile_rows);
#endif

    qp_restart(fd);
    yyparse();

#if defined(DETAILED_DEBUG)
    printf("qp_parse: parse done with %u errors\n", errors);
#endif

    if (errors) return -1;

    jxr_set_QP_DISTRIBUTED(image, tile_qp);
    return 0;
}



static void yyerror(const char*txt)
{
    fprintf(stderr, "parsing QP file: %s\n", txt);
    errors += 1;
}

