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

#ifndef _DUMPFFGF_H
#define _DUMPFFGF_H

#include <wchar.h>

#include "opoint.h"
#include "GeometryCommon.h"
#include "..\Foundation\Data\Byte.h"

class FloatTransform;

class MgDumpFFGF
{
public:
    // Dump to file
    static FILE* createFile( char* name, int order, char* stepname );
    static void closeFile( FILE *ffgfFile );
    static void writeFile( FILE *ffgfFile, int featnum, MgGeometry *pGeom );
    static void writeFile( FILE *ffgfFile, FloatTransform* transform, int featnum, const OpsFloatPoint vertices[], int nVertices);
    static void writeFile( FILE *ffgfFile, FloatTransform* transform, int featnum, float x1, float y1, float x2, float y2);
    static void writeFile( FILE *ffgfFile, FloatTransform* transform, int featnum, float x, float y);

    static MgByte* GetFGF( MgGeometry* pGeom );
};

#endif
