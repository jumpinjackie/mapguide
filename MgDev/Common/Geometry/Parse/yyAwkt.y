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

///////////////////////////////////////////////////////////////////////
// Name
//	yyAwkt.y - YACC grammar 
//								
// Synopsis	
//	GisInt32  fdo_awkb_yyparse(FdoParse* pParse)
//
// Description							
//	Parse AWKT and create an AWKB from it.
//
// Parameters
//	pParse		pointer to parsing context
//
// Function Value									
//	0 if successful, 1 otherwise (respect YYACCEPT and YYABORT macros).
//	If successful, the root node is set in FdoParse to be returned. 
///////////////////////////////////////////////////////////////////////

%{
#define NOTOKEN_H
#include "ParseAwkt.h"
#pragma warning(disable:4102)	// unreferenced labels in y.tab.c
%}

%union _YYSTYPE
{
	double		m_double;		// floating point values 
}

// keywords
// ... geometry types (WKT)
%token GisToken_POINT GisToken_LINESTRING GisToken_POLYGON
%token GisToken_MULTIPOINT GisToken_MULTILINESTRING GisToken_MULTIPOLYGON
%token GisToken_GEOMETRYCOLLECTION
// ... autodesk extensions (AWKT)
%token GisToken_CURVESTRING GisToken_CURVEPOLYGON
%token GisToken_MULTICURVESTRING GisToken_MULTICURVEPOLYGON
// ... reserved for future extensions
%token GisToken_CIRCLE GisToken_RECTANGLE
%token GisToken_TINTERRAIN GisToken_GRIDTERRAIN GisToken_CUSTOM
%token GisToken_BEZIERSEGMENT
// ... dimension types
%token GisToken_XY GisToken_XYZ GisToken_XYM GisToken_XYZM
// ... segment types
%token GisToken_CIRCULARARCSEGMENT GisToken_LINESTRINGSEGMENT
%token GisToken_ARC // synonym for CIRCULARARCSEGMENT

// data types
%token GisToken_DOUBLE

// operators
%token GisToken_LeftParenthesis GisToken_RightParenthesis GisToken_Comma

// node types
%type <m_double> GisToken_DOUBLE

%start gis
%%

gis : Awkt		{pParse->Done(); YYACCEPT;}
	| error		{pParse->Error(); YYABORT;}
	;

Awkt :
      GisToken_POINT {pParse->SetGeometryType(GisToken_POINT);}
			Dimensionality PointEntity
      | GisToken_LINESTRING {pParse->SetGeometryType(GisToken_LINESTRING);}
			Dimensionality LineString
      | GisToken_POLYGON {pParse->SetGeometryType(GisToken_POLYGON);}
			Dimensionality Polygon
	  | GisToken_CURVESTRING {pParse->SetGeometryType(GisToken_CURVESTRING);}
			Dimensionality CurveString
	  | GisToken_CURVEPOLYGON {pParse->SetGeometryType(GisToken_CURVEPOLYGON);}
			Dimensionality CurvePolygon
      | GisToken_MULTIPOINT {pParse->SetGeometryType(GisToken_MULTIPOINT);}
			Dimensionality MultiPoint
      | GisToken_MULTILINESTRING {pParse->SetGeometryType(GisToken_MULTILINESTRING);}
			Dimensionality MultiLineString
      | GisToken_MULTIPOLYGON {pParse->SetGeometryType(GisToken_MULTIPOLYGON);}
			Dimensionality MultiPolygon
	  | GisToken_MULTICURVESTRING {pParse->SetGeometryType(GisToken_MULTICURVESTRING);}
			Dimensionality MultiCurveString
	  | GisToken_MULTICURVEPOLYGON {pParse->SetGeometryType(GisToken_MULTICURVEPOLYGON);}
			Dimensionality MultiCurvePolygon
      | GisToken_GEOMETRYCOLLECTION {pParse->SetGeometryType(GisToken_GEOMETRYCOLLECTION);}
			GeometryCollection
      ;

// WKT geometries--linear only, but extended dimension types
PointEntity : '(' Point ')'						{Node_Trace(L"PointEntity\n");};
LineString : '(' PointCollection ')'			{Node_Trace(L"LineString\n");};
Polygon : '(' LineStringCollection ')'			{Node_Trace(L"Polygon\n");};
MultiPoint : '(' PointCollection ')'			{Node_Trace(L"MultiPoint\n");};
MultiLineString : '(' LineStringCollection ')'	{Node_Trace(L"MultiLineString\n");};
MultiPolygon : '(' PolygonCollection ')'		{Node_Trace(L"MultiPolygon\n");};
GeometryCollection : '(' AwktCollection ')'		{Node_Trace(L"GeometryCollection\n");};

// AWKT geometries--circular and linear segments
CurveString : '(' Point '(' CurveSegmentCollection ')' ')'	{Node_Trace(L"CurveString\n");};
CurvePolygon : '(' CurveStringCollection ')'				{Node_Trace(L"CurvePolygon\n");};
MultiCurveString : '(' CurveStringCollection ')'			{Node_Trace(L"MultiCurveString\n");};
MultiCurvePolygon : '(' CurvePolygonCollection ')'			{Node_Trace(L"MultiCurvePolygon\n");};

Dimensionality :		{pParse->SetDimensionality(-1);}
	| GisToken_XY		{pParse->SetDimensionality(GisToken_XY);}
	| GisToken_XYZ		{pParse->SetDimensionality(GisToken_XYZ);}
	| GisToken_XYM		{pParse->SetDimensionality(GisToken_XYM);}
	| GisToken_XYZM		{pParse->SetDimensionality(GisToken_XYZM);}
	;
	
Point : 
	GisToken_DOUBLE GisToken_DOUBLE									
		{pParse->AddPoint($1, $2);}
	| GisToken_DOUBLE GisToken_DOUBLE GisToken_DOUBLE					
		{pParse->AddPoint($1, $2, $3);}
	| GisToken_DOUBLE GisToken_DOUBLE GisToken_DOUBLE GisToken_DOUBLE	
		{pParse->AddPoint($1, $2, $3, $4);}
	;

PointCollection : Point						{Node_Trace(L"PointCollection[1]\n");};
	| PointCollection ',' Point				{Node_Trace(L"PointCollection[n]\n");};
	;
LineStringCollection : LineString			{Node_Trace(L"LineStringCollection[1]\n");};
	| LineStringCollection ',' {pParse->SetBreak();} LineString	{Node_Trace(L"LineStringCollection[n]\n");};
	;    
PolygonCollection : Polygon					{Node_Trace(L"PolygonCollection[1]\n");};
	| PolygonCollection ',' {pParse->SetOuterBreak();} Polygon			{Node_Trace(L"PolygonCollection[n]\n");};
	;    
AwktCollection : Awkt						{Node_Trace(L"AwktCollection[1]\n");};
	| AwktCollection ',' Awkt				{Node_Trace(L"AwktCollection[n]\n");};
	;

CurveSegment :	GisToken_CIRCULARARCSEGMENT {pParse->SetSegmentType(GisToken_CIRCULARARCSEGMENT);} '(' Point ',' Point ')'
	| GisToken_LINESTRINGSEGMENT {pParse->SetSegmentType(GisToken_LINESTRINGSEGMENT);} '(' PointCollection ')'
	;
CurveSegmentCollection : CurveSegment			{Node_Trace(L"CurveSegmentCollection[1]\n");};
	| CurveSegmentCollection ',' CurveSegment	{Node_Trace(L"CurveSegmentCollection[n]\n");};
	;
CurveStringCollection : CurveString				{Node_Trace(L"CurveStringCollection[1]\n");};
	| CurveStringCollection ',' {pParse->SetBreak();} CurveString	{Node_Trace(L"CurveStringCollection[n]\n");};
	;    
CurvePolygonCollection : CurvePolygon			{Node_Trace(L"CurvePolygonCollection[1]\n");};
	| CurvePolygonCollection ',' {pParse->SetOuterBreak();} CurvePolygon	{Node_Trace(L"CurvePolygonCollection[n]\n");};
	;    
%%
///////////////////////////////////////////////////////////////////////

#include <stdio.h>

GisInt32 yyerror_default(char *str, GisInt32 lineno)
{
#ifdef FDO_YYDEBUG
	fprintf(stderr, "line %d: %s\n", lineno, str);
#endif
	return 0;
}

typedef GisInt32 (*ERRFUNC)(char*, GisInt32);
static ERRFUNC yyerror_func = yyerror_default;

GisInt32 yyerror(GisParseAwkt* pParse, char* str)
{
	return (*yyerror_func)(str, yylineno);
}

ERRFUNC yyerror_set(ERRFUNC func)
{
	ERRFUNC old_func = yyerror_func;

	if (func != NULL)
		yyerror_func = func;

	return old_func;
}

static void  print_current_token()
{
//TODO: this has not been updated, but it is unused anyway
#ifdef FDO_YYDEBUG
	char  *token_name;
	
	if( yychar <= YYMAXTOKEN )
	{
		switch( yychar )
		{
			case IDENTIFIER:
				token_name = yylval.id;
				break;

			default:
				token_name = yyname[yychar];
				break;
		}
	}
	else
	{
		token_name = "illegal-name";
	}

	printf( "***  current token is %d (%s)  ***\n", yychar, token_name );
#endif
}
