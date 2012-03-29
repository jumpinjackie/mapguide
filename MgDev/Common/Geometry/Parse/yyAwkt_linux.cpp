//
//  Copyright (C) 2004-2011 by Autodesk, Inc.
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of version 2.1 of the GNU Lesser
//  General Public License as published by the Free Software Foundation.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//

#ifndef lint
static char const
gis_awkt_yyrcsid[] = "$FreeBSD: src/usr.bin/yacc/skeleton.c,v 1.28 2000/01/17 02:04:06 bde Exp $";
#endif
#include <stdlib.h>
#define GIS_AWKT_YYBYACC 1
#define GIS_AWKT_YYMAJOR 1
#define GIS_AWKT_YYMINOR 9
#define GIS_AWKT_YYLEX gis_awkt_yylex(pParse)
#define GIS_AWKT_YYEMPTY -1
#define gis_awkt_yyclearin (pParse->gis_awkt_yychar=(GIS_AWKT_YYEMPTY))
#define gis_awkt_yyerrok (pParse->gis_awkt_yyerrflag=0)
#define GIS_AWKT_YYRECOVERING() (pParse->gis_awkt_yyerrflag!=0)
class MgParseAwkt; static int gis_awkt_yygrowstack(MgParseAwkt *pParse);
#define GIS_AWKT_YYPREFIX "gis_awkt_yy"
#line 37 "yyAwkt.y"
#define NOTOKEN_H
#include "Foundation.h"
#include "ParseAwkt.h"
#pragma warning(disable:4102)   /* unreferenced labels in gis_awkt_yyAwkt.cpp*/
#line 42 "yyAwkt.y"
#if 0
{
    double      m_double;       /* floating point values */
#endif
#line 26 "yyAwkt.cpp"
#define GIS_AWKT_YYERRCODE 256
#define GisToken_POINT 257
#define GisToken_LINESTRING 258
#define GisToken_POLYGON 259
#define GisToken_MULTIPOINT 260
#define GisToken_MULTILINESTRING 261
#define GisToken_MULTIPOLYGON 262
#define GisToken_GEOMETRYCOLLECTION 263
#define GisToken_CURVESTRING 264
#define GisToken_CURVEPOLYGON 265
#define GisToken_MULTICURVESTRING 266
#define GisToken_MULTICURVEPOLYGON 267
#define GisToken_CIRCLE 268
#define GisToken_RECTANGLE 269
#define GisToken_TINTERRAIN 270
#define GisToken_GRIDTERRAIN 271
#define GisToken_CUSTOM 272
#define GisToken_BEZIERSEGMENT 273
#define GisToken_XY 274
#define GisToken_XYZ 275
#define GisToken_XYM 276
#define GisToken_XYZM 277
#define GisToken_CIRCULARARCSEGMENT 278
#define GisToken_LINESTRINGSEGMENT 279
#define GisToken_ARC 280
#define GisToken_DOUBLE 281
#define GisToken_LeftParenthesis 282
#define GisToken_RightParenthesis 283
#define GisToken_Comma 284
const short gis_awkt_yylhs[] = {                                        -1,
    0,    0,    3,    1,    5,    1,    7,    1,    9,    1,
   11,    1,   13,    1,   15,    1,   17,    1,   19,    1,
   21,    1,   24,    1,    4,    6,    8,   14,   16,   18,
   23,   10,   12,   20,   22,    2,    2,    2,    2,    2,
   25,   25,   25,   26,   26,   27,   33,   27,   28,   34,
   28,   29,   29,   36,   35,   37,   35,   30,   30,   31,
   38,   31,   32,   39,   32,
};
const short gis_awkt_yylen[] = {                                         2,
    1,    1,    0,    4,    0,    4,    0,    4,    0,    4,
    0,    4,    0,    4,    0,    4,    0,    4,    0,    4,
    0,    4,    0,    3,    3,    3,    3,    3,    3,    3,
    3,    6,    3,    3,    3,    0,    1,    1,    1,    1,
    2,    3,    4,    1,    3,    1,    0,    4,    1,    0,
    4,    1,    3,    0,    7,    0,    5,    1,    3,    1,
    0,    4,    1,    0,    4,
};
const short gis_awkt_yydefred[] = {                                      0,
    2,    3,    5,    7,   13,   15,   17,   23,    9,   11,
   19,   21,    0,    1,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,   37,   38,   39,   40,    0,
    0,    0,    0,    0,    0,    0,   24,    0,    0,    0,
    0,    0,    4,    0,    6,    0,    8,    0,   14,    0,
   16,    0,   18,   52,    0,    0,   10,    0,   12,    0,
   20,    0,   22,    0,    0,   44,    0,   46,    0,    0,
    0,   49,    0,   31,    0,    0,   60,    0,    0,   63,
    0,    0,   25,   26,    0,   27,   47,   28,   29,   30,
   50,   53,    0,   33,   61,   34,   35,   64,    0,   45,
    0,    0,   54,   56,    0,   58,    0,    0,   43,   48,
   51,    0,    0,    0,    0,   62,   65,    0,    0,   32,
   59,    0,    0,    0,   57,    0,   55,
};
const short gis_awkt_yydgoto[] = {                                      13,
   14,   30,   15,   43,   16,   68,   17,   47,   22,   77,
   23,   59,   18,   49,   19,   51,   20,   53,   24,   61,
   25,   63,   37,   21,   66,   67,   69,   73,   55,  105,
   78,   81,  101,  102,  106,  112,  113,  107,  108,
};
const short gis_awkt_yysindex[] = {                                   -241,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0, -236, -236, -236, -236, -236, -236,
    2, -236, -236, -236, -236,    0,    0,    0,    0,    8,
   12,   20,   47,   49,   50, -230,    0,   51,   52,   53,
   54, -269,    0, -269,    0,   12,    0, -269,    0,   12,
    0,   20,    0,    0,    3, -269,    0,   51,    0,   51,
    0,   52,    0, -186,   55,    0,    5,    0,    9,   24,
   25,    0,   26,    0, -230,   57,    0,   33,   34,    0,
   40, -183,    0,    0, -269,    0,    0,    0,    0,    0,
    0,    0, -199,    0,    0,    0,    0,    0, -182,    0,
   12,   20,    0,    0,   42,    0,   51,   52,    0,    0,
    0,   60,   61,   62, -199,    0,    0, -269, -269,    0,
    0,   58,   44, -269,    0,   63,    0,
};
const short gis_awkt_yyrindex[] = {                                      0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    2,    2,    2,    2,    2,    2,
    0,    2,    2,    2,    2,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,  -33,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,  -31,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,
};
const short gis_awkt_yygindex[] = {                                      0,
  -30,   39,    0,    0,    0,  -28,    0,  -48,    0,  -36,
    0,  -57,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,  -42,  -47,   56,    0,    0,    0,
   45,    0,    0,    0,   -8,    0,    0,    0,    0,
};
#define GIS_AWKT_YYTABLESIZE 107
const short gis_awkt_yytable[] = {                                      65,
   70,   57,   45,   72,   80,   54,   41,   41,   42,   42,
   41,   64,   42,   76,    1,    2,    3,    4,    5,    6,
    7,    8,    9,   10,   11,   12,    2,    3,    4,    5,
    6,    7,    8,    9,   10,   11,   12,   26,   27,   28,
   29,   36,  100,   74,   92,   84,   75,   42,   85,   86,
  117,   44,   87,  111,   31,   32,   33,   34,   35,   46,
   38,   39,   40,   41,   88,   89,   90,   85,   87,   91,
  116,  123,  110,   94,   96,  122,   95,   95,  103,  104,
   97,  126,  114,   98,  125,  115,   48,   85,   50,   52,
   56,   58,   60,   62,   82,   83,   93,   99,  109,  118,
  119,  124,  120,  127,   79,   71,  121,
};
const short gis_awkt_yycheck[] = {                                      42,
   48,   38,   31,   52,   62,   36,   40,   41,   40,   41,
   44,  281,   44,   56,  256,  257,  258,  259,  260,  261,
  262,  263,  264,  265,  266,  267,  257,  258,  259,  260,
  261,  262,  263,  264,  265,  266,  267,  274,  275,  276,
  277,   40,   85,   41,   75,   41,   44,   40,   44,   41,
  108,   40,   44,  102,   16,   17,   18,   19,   20,   40,
   22,   23,   24,   25,   41,   41,   41,   44,   44,   44,
  107,  119,  101,   41,   41,  118,   44,   44,  278,  279,
   41,  124,   41,   44,   41,   44,   40,   44,   40,   40,
   40,   40,   40,   40,  281,   41,   40,  281,  281,   40,
   40,   44,   41,   41,   60,   50,  115,
};
#define GIS_AWKT_YYFINAL 13
#ifndef GIS_AWKT_YYDEBUG
#define GIS_AWKT_YYDEBUG 0
#endif
#define GIS_AWKT_YYMAXTOKEN 284
#if GIS_AWKT_YYDEBUG
const char * const gis_awkt_yyname[] = {
"end-of-file",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,"'('","')'",0,0,"','",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"GisToken_POINT",
"GisToken_LINESTRING","GisToken_POLYGON","GisToken_MULTIPOINT",
"GisToken_MULTILINESTRING","GisToken_MULTIPOLYGON",
"GisToken_GEOMETRYCOLLECTION","GisToken_CURVESTRING","GisToken_CURVEPOLYGON",
"GisToken_MULTICURVESTRING","GisToken_MULTICURVEPOLYGON","GisToken_CIRCLE",
"GisToken_RECTANGLE","GisToken_TINTERRAIN","GisToken_GRIDTERRAIN",
"GisToken_CUSTOM","GisToken_BEZIERSEGMENT","GisToken_XY","GisToken_XYZ",
"GisToken_XYM","GisToken_XYZM","GisToken_CIRCULARARCSEGMENT",
"GisToken_LINESTRINGSEGMENT","GisToken_ARC","GisToken_DOUBLE",
"GisToken_LeftParenthesis","GisToken_RightParenthesis","GisToken_Comma",
};
const char * const gis_awkt_yyrule[] = {
"$accept : gis",
"gis : Awkt",
"gis : error",
"$$1 :",
"Awkt : GisToken_POINT $$1 Dimensionality PointEntity",
"$$2 :",
"Awkt : GisToken_LINESTRING $$2 Dimensionality LineString",
"$$3 :",
"Awkt : GisToken_POLYGON $$3 Dimensionality Polygon",
"$$4 :",
"Awkt : GisToken_CURVESTRING $$4 Dimensionality CurveString",
"$$5 :",
"Awkt : GisToken_CURVEPOLYGON $$5 Dimensionality CurvePolygon",
"$$6 :",
"Awkt : GisToken_MULTIPOINT $$6 Dimensionality MultiPoint",
"$$7 :",
"Awkt : GisToken_MULTILINESTRING $$7 Dimensionality MultiLineString",
"$$8 :",
"Awkt : GisToken_MULTIPOLYGON $$8 Dimensionality MultiPolygon",
"$$9 :",
"Awkt : GisToken_MULTICURVESTRING $$9 Dimensionality MultiCurveString",
"$$10 :",
"Awkt : GisToken_MULTICURVEPOLYGON $$10 Dimensionality MultiCurvePolygon",
"$$11 :",
"Awkt : GisToken_GEOMETRYCOLLECTION $$11 GeometryCollection",
"PointEntity : '(' Point ')'",
"LineString : '(' PointCollection ')'",
"Polygon : '(' LineStringCollection ')'",
"MultiPoint : '(' PointCollection ')'",
"MultiLineString : '(' LineStringCollection ')'",
"MultiPolygon : '(' PolygonCollection ')'",
"GeometryCollection : '(' AwktCollection ')'",
"CurveString : '(' Point '(' CurveSegmentCollection ')' ')'",
"CurvePolygon : '(' CurveStringCollection ')'",
"MultiCurveString : '(' CurveStringCollection ')'",
"MultiCurvePolygon : '(' CurvePolygonCollection ')'",
"Dimensionality :",
"Dimensionality : GisToken_XY",
"Dimensionality : GisToken_XYZ",
"Dimensionality : GisToken_XYM",
"Dimensionality : GisToken_XYZM",
"Point : GisToken_DOUBLE GisToken_DOUBLE",
"Point : GisToken_DOUBLE GisToken_DOUBLE GisToken_DOUBLE",
"Point : GisToken_DOUBLE GisToken_DOUBLE GisToken_DOUBLE GisToken_DOUBLE",
"PointCollection : Point",
"PointCollection : PointCollection ',' Point",
"LineStringCollection : LineString",
"$$12 :",
"LineStringCollection : LineStringCollection ',' $$12 LineString",
"PolygonCollection : Polygon",
"$$13 :",
"PolygonCollection : PolygonCollection ',' $$13 Polygon",
"AwktCollection : Awkt",
"AwktCollection : AwktCollection ',' Awkt",
"$$14 :",
"CurveSegment : GisToken_CIRCULARARCSEGMENT $$14 '(' Point ',' Point ')'",
"$$15 :",
"CurveSegment : GisToken_LINESTRINGSEGMENT $$15 '(' PointCollection ')'",
"CurveSegmentCollection : CurveSegment",
"CurveSegmentCollection : CurveSegmentCollection ',' CurveSegment",
"CurveStringCollection : CurveString",
"$$16 :",
"CurveStringCollection : CurveStringCollection ',' $$16 CurveString",
"CurvePolygonCollection : CurvePolygon",
"$$17 :",
"CurvePolygonCollection : CurvePolygonCollection ',' $$17 CurvePolygon",
};
#endif
#if GIS_AWKT_YYDEBUG
#include <stdio.h>
#endif
#ifdef GIS_AWKT_YYSTACKSIZE
#undef GIS_AWKT_YYMAXDEPTH
#define GIS_AWKT_YYMAXDEPTH GIS_AWKT_YYSTACKSIZE
#else
#ifdef GIS_AWKT_YYMAXDEPTH
#define GIS_AWKT_YYSTACKSIZE GIS_AWKT_YYMAXDEPTH
#else
#define GIS_AWKT_YYSTACKSIZE 10000
#define GIS_AWKT_YYMAXDEPTH 10000
#endif
#endif
#define GIS_AWKT_YYINITSTACKSIZE 200
#line 163 "yyAwkt.y"
///////////////////////////////////////////////////////////////////////

#include <stdio.h>

INT32 gis_awkt_yyerror_default(char *str, INT32 lineno)
{
#ifdef FDO_GIS_AWKT_YYDEBUG
    fprintf(stderr, "line %d: %s\n", lineno, str);
#endif
    return 0;
}

typedef INT32 (*ERRFUNC)(char*, INT32);
static ERRFUNC gis_awkt_yyerror_func = gis_awkt_yyerror_default;

INT32 gis_awkt_yyerror(MgParseAwkt* pParse, char* str)
{
    return (*gis_awkt_yyerror_func)(str, pParse->gis_awkt_yylineno);
}

ERRFUNC gis_awkt_yyerror_set(ERRFUNC func)
{
    ERRFUNC old_func = gis_awkt_yyerror_func;

    if (func != NULL)
        gis_awkt_yyerror_func = func;

    return old_func;
}

static void  print_current_token()
{
//TODO: this has not been updated, but it is unused anyway
#ifdef FDO_GIS_AWKT_YYDEBUG
    char  *token_name;

    if( pParse->gis_awkt_yychar <= GIS_AWKT_YYMAXTOKEN )
    {
        switch( pParse->gis_awkt_yychar )
        {
            case IDENTIFIER:
                token_name = pParse->gis_awkt_yylval.id;
                break;

            default:
                token_name = gis_awkt_yyname[pParse->gis_awkt_yychar];
                break;
        }
    }
    else
    {
        token_name = "illegal-name";
    }

    printf( "***  current token is %d (%s)  ***\n", pParse->gis_awkt_yychar, token_name );
#endif
}
#line 336 "yyAwkt.cpp"
/* allocate initial stack or double stack size, up to GIS_AWKT_YYMAXDEPTH */
static int gis_awkt_yygrowstack(MgParseAwkt *pParse)
{
    int newsize, i;
    short *newss;
    GIS_AWKT_YYSTYPE *newvs;

    if ((newsize = pParse->gis_awkt_yystacksize) == 0)
        newsize = GIS_AWKT_YYINITSTACKSIZE;
    else if (newsize >= GIS_AWKT_YYMAXDEPTH)
        return -1;
    else if ((newsize *= 2) > GIS_AWKT_YYMAXDEPTH)
        newsize = GIS_AWKT_YYMAXDEPTH;
    i = pParse->gis_awkt_yyssp - pParse->gis_awkt_yyss;
    newss = pParse->gis_awkt_yyss ? (short *)realloc(pParse->gis_awkt_yyss, newsize * sizeof *newss) :
      (short *)malloc(newsize * sizeof *newss);
    if (newss == NULL)
        return -1;
    pParse->gis_awkt_yyss = newss;
    pParse->gis_awkt_yyssp = newss + i;
    newvs = pParse->gis_awkt_yyvs ? (GIS_AWKT_YYSTYPE *)realloc(pParse->gis_awkt_yyvs, newsize * sizeof *newvs) :
      (GIS_AWKT_YYSTYPE *)malloc(newsize * sizeof *newvs);
    if (newvs == NULL)
        return -1;
    pParse->gis_awkt_yyvs = newvs;
    pParse->gis_awkt_yyvsp = newvs + i;
    pParse->gis_awkt_yystacksize = newsize;
    pParse->gis_awkt_yysslim = pParse->gis_awkt_yyss + newsize - 1;
    return 0;
}

#define GIS_AWKT_YYABORT goto gis_awkt_yyabort
#define GIS_AWKT_YYREJECT goto gis_awkt_yyabort
#define GIS_AWKT_YYACCEPT goto gis_awkt_yyaccept
#define GIS_AWKT_YYERROR goto gis_awkt_yyerrlab

#ifndef GIS_AWKT_YYPARSE_PARAM
#if defined(__cplusplus) || __STDC__
#define GIS_AWKT_YYPARSE_PARAM_ARG void
#define GIS_AWKT_YYPARSE_PARAM_DECL
#else   /* ! ANSI-C/C++ */
#define GIS_AWKT_YYPARSE_PARAM_ARG
#define GIS_AWKT_YYPARSE_PARAM_DECL
#endif  /* ANSI-C/C++ */
#else   /* GIS_AWKT_YYPARSE_PARAM */
#ifndef GIS_AWKT_YYPARSE_PARAM_TYPE
#define GIS_AWKT_YYPARSE_PARAM_TYPE void *
#endif
#if defined(__cplusplus) || __STDC__
#define GIS_AWKT_YYPARSE_PARAM_ARG GIS_AWKT_YYPARSE_PARAM_TYPE GIS_AWKT_YYPARSE_PARAM
#define GIS_AWKT_YYPARSE_PARAM_DECL
#else   /* ! ANSI-C/C++ */
#define GIS_AWKT_YYPARSE_PARAM_ARG GIS_AWKT_YYPARSE_PARAM
#define GIS_AWKT_YYPARSE_PARAM_DECL GIS_AWKT_YYPARSE_PARAM_TYPE GIS_AWKT_YYPARSE_PARAM;
#endif  /* ANSI-C/C++ */
#endif  /* ! GIS_AWKT_YYPARSE_PARAM */

INT32
gis_awkt_yyparse(MgParseAwkt* pParse)
    GIS_AWKT_YYPARSE_PARAM_DECL
{
    register int gis_awkt_yym, gis_awkt_yyn, gis_awkt_yystate;
#if GIS_AWKT_YYDEBUG
    register const char *gis_awkt_yys;

    if ((gis_awkt_yys = getenv("GIS_AWKT_YYDEBUG")))
    {
        gis_awkt_yyn = *gis_awkt_yys;
        if (gis_awkt_yyn >= '0' && gis_awkt_yyn <= '9')
            pParse->gis_awkt_yydebug = gis_awkt_yyn - '0';
    }
#endif

    pParse->gis_awkt_yynerrs = 0;
    pParse->gis_awkt_yyerrflag = 0;
    pParse->gis_awkt_yychar = (-1);

    if (pParse->gis_awkt_yyss == NULL && gis_awkt_yygrowstack(pParse)) goto gis_awkt_yyoverflow;
    pParse->gis_awkt_yyssp = pParse->gis_awkt_yyss;
    pParse->gis_awkt_yyvsp = pParse->gis_awkt_yyvs;
    *pParse->gis_awkt_yyssp = gis_awkt_yystate = 0;

gis_awkt_yyloop:
    if ((gis_awkt_yyn = gis_awkt_yydefred[gis_awkt_yystate])) goto gis_awkt_yyreduce;
    if (pParse->gis_awkt_yychar < 0)
    {
        if ((pParse->gis_awkt_yychar = gis_awkt_yylex(pParse)) < 0) pParse->gis_awkt_yychar = 0;
#if GIS_AWKT_YYDEBUG
        if (pParse->gis_awkt_yydebug)
        {
            gis_awkt_yys = 0;
            if (pParse->gis_awkt_yychar <= GIS_AWKT_YYMAXTOKEN) gis_awkt_yys = gis_awkt_yyname[pParse->gis_awkt_yychar];
            if (!gis_awkt_yys) gis_awkt_yys = "illegal-symbol";
            printf("%sdebug: state %d, reading %d (%s)\n",
                    GIS_AWKT_YYPREFIX, gis_awkt_yystate, pParse->gis_awkt_yychar, gis_awkt_yys);
        }
#endif
    }
    if ((gis_awkt_yyn = gis_awkt_yysindex[gis_awkt_yystate]) && (gis_awkt_yyn += pParse->gis_awkt_yychar) >= 0 &&
            gis_awkt_yyn <= GIS_AWKT_YYTABLESIZE && gis_awkt_yycheck[gis_awkt_yyn] == pParse->gis_awkt_yychar)
    {
#if GIS_AWKT_YYDEBUG
        if (pParse->gis_awkt_yydebug)
            printf("%sdebug: state %d, shifting to state %d\n",
                    GIS_AWKT_YYPREFIX, gis_awkt_yystate, gis_awkt_yytable[gis_awkt_yyn]);
#endif
        if (pParse->gis_awkt_yyssp >= pParse->gis_awkt_yysslim && gis_awkt_yygrowstack(pParse))
        {
            goto gis_awkt_yyoverflow;
        }
        *++pParse->gis_awkt_yyssp = gis_awkt_yystate = gis_awkt_yytable[gis_awkt_yyn];
        *++pParse->gis_awkt_yyvsp = pParse->gis_awkt_yylval;
        pParse->gis_awkt_yychar = (-1);
        if (pParse->gis_awkt_yyerrflag > 0)  --pParse->gis_awkt_yyerrflag;
        goto gis_awkt_yyloop;
    }
    if ((gis_awkt_yyn = gis_awkt_yyrindex[gis_awkt_yystate]) && (gis_awkt_yyn += pParse->gis_awkt_yychar) >= 0 &&
            gis_awkt_yyn <= GIS_AWKT_YYTABLESIZE && gis_awkt_yycheck[gis_awkt_yyn] == pParse->gis_awkt_yychar)
    {
        gis_awkt_yyn = gis_awkt_yytable[gis_awkt_yyn];
        goto gis_awkt_yyreduce;
    }
    if (pParse->gis_awkt_yyerrflag) goto gis_awkt_yyinrecovery;
#if defined(lint) || defined(__GNUC__)
    goto gis_awkt_yynewerror;
#endif
gis_awkt_yynewerror:
   gis_awkt_yyerror( pParse,"syntax error");
#if defined(lint) || defined(__GNUC__)
    goto gis_awkt_yyerrlab;
#endif
gis_awkt_yyerrlab:
    ++pParse->gis_awkt_yynerrs;
gis_awkt_yyinrecovery:
    if (pParse->gis_awkt_yyerrflag < 3)
    {
        pParse->gis_awkt_yyerrflag = 3;
        for (;;)
        {
            if ((gis_awkt_yyn = gis_awkt_yysindex[*pParse->gis_awkt_yyssp]) && (gis_awkt_yyn += GIS_AWKT_YYERRCODE) >= 0 &&
                    gis_awkt_yyn <= GIS_AWKT_YYTABLESIZE && gis_awkt_yycheck[gis_awkt_yyn] == GIS_AWKT_YYERRCODE)
            {
#if GIS_AWKT_YYDEBUG
                if (pParse->gis_awkt_yydebug)
                    printf("%sdebug: state %d, error recovery shifting\
 to state %d\n", GIS_AWKT_YYPREFIX, *pParse->gis_awkt_yyssp, gis_awkt_yytable[gis_awkt_yyn]);
#endif
                if (pParse->gis_awkt_yyssp >= pParse->gis_awkt_yysslim && gis_awkt_yygrowstack(pParse))
                {
                    goto gis_awkt_yyoverflow;
                }
                *++pParse->gis_awkt_yyssp = gis_awkt_yystate = gis_awkt_yytable[gis_awkt_yyn];
                *++pParse->gis_awkt_yyvsp = pParse->gis_awkt_yylval;
                goto gis_awkt_yyloop;
            }
            else
            {
#if GIS_AWKT_YYDEBUG
                if (pParse->gis_awkt_yydebug)
                    printf("%sdebug: error recovery discarding state %d\n",
                            GIS_AWKT_YYPREFIX, *pParse->gis_awkt_yyssp);
#endif
                if (pParse->gis_awkt_yyssp <= pParse->gis_awkt_yyss) goto gis_awkt_yyabort;
                --pParse->gis_awkt_yyssp;
                --pParse->gis_awkt_yyvsp;
            }
        }
    }
    else
    {
        if (pParse->gis_awkt_yychar == 0) goto gis_awkt_yyabort;
#if GIS_AWKT_YYDEBUG
        if (pParse->gis_awkt_yydebug)
        {
            gis_awkt_yys = 0;
            if (pParse->gis_awkt_yychar <= GIS_AWKT_YYMAXTOKEN) gis_awkt_yys = gis_awkt_yyname[pParse->gis_awkt_yychar];
            if (!gis_awkt_yys) gis_awkt_yys = "illegal-symbol";
            printf("%sdebug: state %d, error recovery discards token %d (%s)\n",
                    GIS_AWKT_YYPREFIX, gis_awkt_yystate, pParse->gis_awkt_yychar, gis_awkt_yys);
        }
#endif
        pParse->gis_awkt_yychar = (-1);
        goto gis_awkt_yyloop;
    }
gis_awkt_yyreduce:
#if GIS_AWKT_YYDEBUG
    if (pParse->gis_awkt_yydebug)
        printf("%sdebug: state %d, reducing by rule %d (%s)\n",
                GIS_AWKT_YYPREFIX, gis_awkt_yystate, gis_awkt_yyn, gis_awkt_yyrule[gis_awkt_yyn]);
#endif
    gis_awkt_yym = gis_awkt_yylen[gis_awkt_yyn];
    pParse->gis_awkt_yyval = pParse->gis_awkt_yyvsp[1-gis_awkt_yym];
    switch (gis_awkt_yyn)
    {
case 1:
#line 77 "yyAwkt.y"
{pParse->Done(); GIS_AWKT_YYACCEPT;}
break;
case 2:
#line 78 "yyAwkt.y"
{pParse->Error(); GIS_AWKT_YYABORT;}
break;
case 3:
#line 82 "yyAwkt.y"
{pParse->SetGeometryType(GisToken_POINT);}
break;
case 5:
#line 84 "yyAwkt.y"
{pParse->SetGeometryType(GisToken_LINESTRING);}
break;
case 7:
#line 86 "yyAwkt.y"
{pParse->SetGeometryType(GisToken_POLYGON);}
break;
case 9:
#line 88 "yyAwkt.y"
{pParse->SetGeometryType(GisToken_CURVESTRING);}
break;
case 11:
#line 90 "yyAwkt.y"
{pParse->SetGeometryType(GisToken_CURVEPOLYGON);}
break;
case 13:
#line 92 "yyAwkt.y"
{pParse->SetGeometryType(GisToken_MULTIPOINT);}
break;
case 15:
#line 94 "yyAwkt.y"
{pParse->SetGeometryType(GisToken_MULTILINESTRING);}
break;
case 17:
#line 96 "yyAwkt.y"
{pParse->SetGeometryType(GisToken_MULTIPOLYGON);}
break;
case 19:
#line 98 "yyAwkt.y"
{pParse->SetGeometryType(GisToken_MULTICURVESTRING);}
break;
case 21:
#line 100 "yyAwkt.y"
{pParse->SetGeometryType(GisToken_MULTICURVEPOLYGON);}
break;
case 23:
#line 102 "yyAwkt.y"
{pParse->SetGeometryType(GisToken_GEOMETRYCOLLECTION);}
break;
case 25:
#line 107 "yyAwkt.y"
{Node_Trace(L"PointEntity\n");}
break;
case 26:
#line 108 "yyAwkt.y"
{Node_Trace(L"LineString\n");}
break;
case 27:
#line 109 "yyAwkt.y"
{Node_Trace(L"Polygon\n");}
break;
case 28:
#line 110 "yyAwkt.y"
{Node_Trace(L"MultiPoint\n");}
break;
case 29:
#line 111 "yyAwkt.y"
{Node_Trace(L"MultiLineString\n");}
break;
case 30:
#line 112 "yyAwkt.y"
{Node_Trace(L"MultiPolygon\n");}
break;
case 31:
#line 113 "yyAwkt.y"
{Node_Trace(L"GeometryCollection\n");}
break;
case 32:
#line 116 "yyAwkt.y"
{Node_Trace(L"CurveString\n");}
break;
case 33:
#line 117 "yyAwkt.y"
{Node_Trace(L"CurvePolygon\n");}
break;
case 34:
#line 118 "yyAwkt.y"
{Node_Trace(L"MultiCurveString\n");}
break;
case 35:
#line 119 "yyAwkt.y"
{Node_Trace(L"MultiCurvePolygon\n");}
break;
case 36:
#line 121 "yyAwkt.y"
{pParse->SetDimensionality(-1);}
break;
case 37:
#line 122 "yyAwkt.y"
{pParse->SetDimensionality(GisToken_XY);}
break;
case 38:
#line 123 "yyAwkt.y"
{pParse->SetDimensionality(GisToken_XYZ);}
break;
case 39:
#line 124 "yyAwkt.y"
{pParse->SetDimensionality(GisToken_XYM);}
break;
case 40:
#line 125 "yyAwkt.y"
{pParse->SetDimensionality(GisToken_XYZM);}
break;
case 41:
#line 130 "yyAwkt.y"
{pParse->AddPoint(pParse->gis_awkt_yyvsp[-1].m_double, pParse->gis_awkt_yyvsp[0].m_double);}
break;
case 42:
#line 132 "yyAwkt.y"
{pParse->AddPoint(pParse->gis_awkt_yyvsp[-2].m_double, pParse->gis_awkt_yyvsp[-1].m_double, pParse->gis_awkt_yyvsp[0].m_double);}
break;
case 43:
#line 134 "yyAwkt.y"
{pParse->AddPoint(pParse->gis_awkt_yyvsp[-3].m_double, pParse->gis_awkt_yyvsp[-2].m_double, pParse->gis_awkt_yyvsp[-1].m_double, pParse->gis_awkt_yyvsp[0].m_double);}
break;
case 44:
#line 137 "yyAwkt.y"
{Node_Trace(L"PointCollection[1]\n");}
break;
case 45:
#line 138 "yyAwkt.y"
{Node_Trace(L"PointCollection[n]\n");}
break;
case 46:
#line 140 "yyAwkt.y"
{Node_Trace(L"LineStringCollection[1]\n");}
break;
case 47:
#line 141 "yyAwkt.y"
{pParse->SetBreak();}
break;
case 48:
#line 141 "yyAwkt.y"
{Node_Trace(L"LineStringCollection[n]\n");}
break;
case 49:
#line 143 "yyAwkt.y"
{Node_Trace(L"PolygonCollection[1]\n");}
break;
case 50:
#line 144 "yyAwkt.y"
{pParse->SetOuterBreak();}
break;
case 51:
#line 144 "yyAwkt.y"
{Node_Trace(L"PolygonCollection[n]\n");}
break;
case 52:
#line 146 "yyAwkt.y"
{Node_Trace(L"AwktCollection[1]\n");}
break;
case 53:
#line 147 "yyAwkt.y"
{Node_Trace(L"AwktCollection[n]\n");}
break;
case 54:
#line 150 "yyAwkt.y"
{pParse->SetSegmentType(GisToken_CIRCULARARCSEGMENT);}
break;
case 56:
#line 151 "yyAwkt.y"
{pParse->SetSegmentType(GisToken_LINESTRINGSEGMENT);}
break;
case 58:
#line 153 "yyAwkt.y"
{Node_Trace(L"CurveSegmentCollection[1]\n");}
break;
case 59:
#line 154 "yyAwkt.y"
{Node_Trace(L"CurveSegmentCollection[n]\n");}
break;
case 60:
#line 156 "yyAwkt.y"
{Node_Trace(L"CurveStringCollection[1]\n");}
break;
case 61:
#line 157 "yyAwkt.y"
{pParse->SetBreak();}
break;
case 62:
#line 157 "yyAwkt.y"
{Node_Trace(L"CurveStringCollection[n]\n");}
break;
case 63:
#line 159 "yyAwkt.y"
{Node_Trace(L"CurvePolygonCollection[1]\n");}
break;
case 64:
#line 160 "yyAwkt.y"
{pParse->SetOuterBreak();}
break;
case 65:
#line 160 "yyAwkt.y"
{Node_Trace(L"CurvePolygonCollection[n]\n");}
break;
#line 739 "yyAwkt.cpp"
    }
    pParse->gis_awkt_yyssp -= gis_awkt_yym;
    gis_awkt_yystate = *pParse->gis_awkt_yyssp;
    pParse->gis_awkt_yyvsp -= gis_awkt_yym;
    gis_awkt_yym = gis_awkt_yylhs[gis_awkt_yyn];
    if (gis_awkt_yystate == 0 && gis_awkt_yym == 0)
    {
#if GIS_AWKT_YYDEBUG
        if (pParse->gis_awkt_yydebug)
            printf("%sdebug: after reduction, shifting from state 0 to\
 state %d\n", GIS_AWKT_YYPREFIX, GIS_AWKT_YYFINAL);
#endif
        gis_awkt_yystate = GIS_AWKT_YYFINAL;
        *++pParse->gis_awkt_yyssp = GIS_AWKT_YYFINAL;
        *++pParse->gis_awkt_yyvsp = pParse->gis_awkt_yyval;
        if (pParse->gis_awkt_yychar < 0)
        {
            if ((pParse->gis_awkt_yychar = gis_awkt_yylex(pParse)) < 0) pParse->gis_awkt_yychar = 0;
#if GIS_AWKT_YYDEBUG
            if (pParse->gis_awkt_yydebug)
            {
                gis_awkt_yys = 0;
                if (pParse->gis_awkt_yychar <= GIS_AWKT_YYMAXTOKEN) gis_awkt_yys = gis_awkt_yyname[pParse->gis_awkt_yychar];
                if (!gis_awkt_yys) gis_awkt_yys = "illegal-symbol";
                printf("%sdebug: state %d, reading %d (%s)\n",
                        GIS_AWKT_YYPREFIX, GIS_AWKT_YYFINAL, pParse->gis_awkt_yychar, gis_awkt_yys);
            }
#endif
        }
        if (pParse->gis_awkt_yychar == 0) goto gis_awkt_yyaccept;
        goto gis_awkt_yyloop;
    }
    if ((gis_awkt_yyn = gis_awkt_yygindex[gis_awkt_yym]) && (gis_awkt_yyn += gis_awkt_yystate) >= 0 &&
            gis_awkt_yyn <= GIS_AWKT_YYTABLESIZE && gis_awkt_yycheck[gis_awkt_yyn] == gis_awkt_yystate)
        gis_awkt_yystate = gis_awkt_yytable[gis_awkt_yyn];
    else
        gis_awkt_yystate = gis_awkt_yydgoto[gis_awkt_yym];
#if GIS_AWKT_YYDEBUG
    if (pParse->gis_awkt_yydebug)
        printf("%sdebug: after reduction, shifting from state %d \
to state %d\n", GIS_AWKT_YYPREFIX, *pParse->gis_awkt_yyssp, gis_awkt_yystate);
#endif
    if (pParse->gis_awkt_yyssp >= pParse->gis_awkt_yysslim && gis_awkt_yygrowstack(pParse))
    {
        goto gis_awkt_yyoverflow;
    }
    *++pParse->gis_awkt_yyssp = gis_awkt_yystate;
    *++pParse->gis_awkt_yyvsp = pParse->gis_awkt_yyval;
    goto gis_awkt_yyloop;
gis_awkt_yyoverflow:
   gis_awkt_yyerror( pParse,"yacc stack overflow");
gis_awkt_yyabort:
    return (1);
gis_awkt_yyaccept:
    return (0);
}
