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
typedef union _GIS_AWKT_YYSTYPE
{
    double      m_double;       /* floating point values */
} GIS_AWKT_YYSTYPE;
