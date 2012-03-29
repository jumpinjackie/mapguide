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

// INTERNAL USE ONLY

#ifndef _PARSEAWKT_H_
#define _PARSEAWKT_H_

#ifdef _WIN32
#pragma once
#endif
#ifndef _WIN32
#include <stdint.h>
#endif

#include "../GeometryCommon.h"
#include "Array.h"

#ifdef _WIN32
typedef unsigned _int32 GisUInt32;
#else
typedef uint32_t GisUInt32;
#endif
#include "LexAwkt.h"
#ifdef _DEBUG
#include <stdio.h>
#endif

#ifndef GisToken_POINT
#include "yyAwkt.h"
#endif

#ifdef _WIN32

// The following is from yyAwkt.cpp
#ifdef GIS_AWKT_YYSTACKSIZE
#undef GIS_AWKT_YYMAXDEPTH
#define GIS_AWKT_YYMAXDEPTH GIS_AWKT_YYSTACKSIZE
#else
#ifdef GIS_AWKT_YYMAXDEPTH
#define GIS_AWKT_YYSTACKSIZE GIS_AWKT_YYMAXDEPTH
#else
#define GIS_AWKT_YYSTACKSIZE 500
#define GIS_AWKT_YYMAXDEPTH 500
#endif
#endif

#endif

class MgParseAwkt
{
public:
    MgParseAwkt();
    ~MgParseAwkt();

    enum _types
    {
    /// From GisGeometryType
        kNone = 0,
        kPoint = 1,
        kLineString = 2,
        kPolygon = 3,
        kMultiPoint = 4,
        kMultiLineString = 5,
        kMultiPolygon = 6,
        kMultiGeometry = 7,
        kCurveString = 10,
        kCurvePolygon = 11,
        kMultiCurveString = 12,
        kMultiCurvePolygon = 13,

    /// From GisGeometryComponentType
        kLinearRing = 129,
        kCircularArcSegment = 130,
        kLineStringSegment = 131,
        kRing = 132,

        kBreak = 200
    };
    enum _dims
    {
        kXY = 0,
        kXYZ = 1,
        kXYM = 2,
        kXYZM = 3
    };

    /// parse AWKT
    MgGeometry* ParseAwkt(wchar_t* pwzAwkt);
    INT32       GetLexeme(void* yylval);

    /// geometry library production
    void    SetGeometryType(INT32 iType);
    void    SetDimensionality(INT32 iType);
    void    SetSegmentType(INT32 iType);
    void    SetBreak();
    void    SetOuterBreak();
    void    AddPoint(double x, double y, double zm=0.0, double m=0.0);
    void    Done();
    void    Error();
#ifdef _DEBUG
    void    Trace(wchar_t*);
#endif

    int                     gis_awkt_yydebug;
    int                     gis_awkt_yynerrs;
    int                     gis_awkt_yyerrflag;
    int                     gis_awkt_yychar;
    short                   *gis_awkt_yyssp;

    GIS_AWKT_YYSTYPE        *gis_awkt_yyvsp;
    GIS_AWKT_YYSTYPE        gis_awkt_yyval;
    GIS_AWKT_YYSTYPE        gis_awkt_yylval;
#ifdef _WIN32
    short                   gis_awkt_yyss[GIS_AWKT_YYSTACKSIZE];
    GIS_AWKT_YYSTYPE        gis_awkt_yyvs[GIS_AWKT_YYSTACKSIZE];
#else
    short                   *gis_awkt_yyss;
    short                   *gis_awkt_yysslim;
    GIS_AWKT_YYSTYPE        *gis_awkt_yyvs;
    int                     gis_awkt_yystacksize;
#endif
    INT32                gis_awkt_yylineno;
//
protected:
    INT32       DimToCount(_dims dim);
    INT32       DimToDimensionality(_dims dim);
    INT32       CountSame(INT32 iContext, _types type);

    MgPoint* DoPoint(INT32& iContext, double* doubles);
    MgPolygon* DoPolygon(INT32& iContext, double* doubles, _types type);
    MgGeometry* DoGeometryCollection(INT32 iContextStart, bool bCollect);
    MgLineString* DoLineString(INT32& iContext, double* doubles, _types type);
    MgMultiPolygon* DoMultiPolygon(INT32& iContext, double* doubles);
    MgMultiPoint* DoMultiPoint(INT32& iContext, double* doubles);
    MgCurveString* DoCurveString(INT32& iContext, double* doubles);
    MgCurvePolygon* DoCurvePolygon(INT32& iContext, double* doubles);
    MgMultiLineString* DoMultiLineString(INT32& iContext, double* doubles);
    MgMultiCurveString* DoMultiCurveString(INT32& iContext, double* doubles);
    MgMultiCurvePolygon* DoMultiCurvePolygon(INT32& iContext, double* doubles);
    MgCurveSegmentCollection* DoCurveSegmentCollection(INT32& iContext, double* doubles);

    GisLexAwkt*             m_lex;
    Ptr<MgGeometry>         m_geometry;
    _dims                   m_dim;
    _types                  m_type;
    GisDoubleArray*         m_values;
    GisIntArray*            m_types;
    GisIntArray*            m_dims;
    GisIntArray*            m_starts;
    bool                    m_break;
};

// AWKT YACC interface
extern INT32        gis_awkt_yyerror(char* str);
extern INT32        gis_awkt_yyparse(MgParseAwkt*);
extern INT32        gis_awkt_yylex(MgParseAwkt*);

// tree management
#if _DEBUG
#define Node_Trace(a)   pParse->Trace(a)
#else
#define Node_Trace(a)
#endif
#endif

