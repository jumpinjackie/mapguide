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

#include "DumpFFGF.h"
#include "Buffer/GreatCircleBufferUtil.h"

// Customize
#define LOCATION_FFGF   "C:\\FFGF_MAP\\"

#define EXTENTION_FFGF  "ffgf"


MgByte *MgDumpFFGF::GetFGF( MgGeometry* pGeom )
{
    MgAgfReaderWriter agfHelper;

    // create an FdoByteArray
    Ptr<MgByteReader> byteReader = agfHelper.Write(pGeom);
    INT64 numBytes = byteReader->GetLength();
    MgByte   *spBytes = new MgByte();

    BYTE bytes[1024];
    int nRead = 0;
    do
    {
        nRead = byteReader->Read(bytes, 1024);
        if (nRead > 0)
            spBytes->Append(bytes, nRead);
    }
    while (nRead > 0);

    return spBytes;
}

FILE *MgDumpFFGF::createFile( char* name, int order, char* stepname )
{
    char name2[100];
    sprintf(name2, "%s%d_%s.%s", LOCATION_FFGF, order, name, EXTENTION_FFGF);

    FILE *ffgfFile = ::fopen( name2, "wb");

    return ffgfFile;
}

void MgDumpFFGF::closeFile( FILE *ffgfFile )
{
    try
    {
        char  eof = (CHAR)0x1a; //END_OF_FILE

        ::fwrite(&eof, sizeof(char), 1, ffgfFile);
        ::fclose(ffgfFile);

    }
    catch (...)
    {
    }
}

void MgDumpFFGF::writeFile( FILE *ffgfFile, int featnum, MgGeometry *pGeom )
{
    try
    {
        int feat_num = featnum;

        MgByte *fgf = GetFGF(pGeom);

        ::fwrite(&feat_num, sizeof(int), 1, ffgfFile);
        ::fwrite(fgf->Bytes(), sizeof(char), fgf->GetLength(), ffgfFile);

        delete fgf;
    }
    catch (...)
    {
    }
}

void MgDumpFFGF::writeFile( FILE *ffgfFile, FloatTransform* transform, int featnum, const OpsFloatPoint vertices[], int nVertices )
{
    try
    {
        MgCoordinateCollection* coordinates = new MgCoordinateCollection();
        MgGeometryFactory factory;

        // Create a line string
        for ( int i = 0; i < nVertices; i++ ) {

            OpsFloatPoint v = vertices[i];

            Ptr<MgCoordinate> coord = transform ? transform->Float2Double(vertices[i]) :
                                    factory.CreateCoordinateXY(vertices[i].x, vertices[i].y);
            coordinates->Add(coord);
        }
        MgLineString *pGeom = new MgLineString(coordinates);

        writeFile(ffgfFile, featnum, pGeom);

        delete pGeom;
    }
    catch (...)
    {
    }
}

void MgDumpFFGF::writeFile( FILE *ffgfFile, FloatTransform* transform, int featnum, float x1, float y1, float x2, float y2 )
{
    try
    {
        MgCoordinateCollection* coordinates = new MgCoordinateCollection();
        MgGeometryFactory factory;

        OpsFloatPoint pt1(x1, y1);
        OpsFloatPoint pt2(x2, y2);

        // Create a line string
        Ptr<MgCoordinate> coord1 = transform->Float2Double(pt1);
        coordinates->Add(coord1);

        Ptr<MgCoordinate> coord2 = transform->Float2Double(pt2);
        coordinates->Add(coord2);

        MgLineString *pGeom = new MgLineString(coordinates);

        writeFile(ffgfFile, featnum, pGeom);

        delete pGeom;
    }
    catch (...)
    {
    }
}

void MgDumpFFGF::writeFile( FILE *ffgfFile, FloatTransform* transform, int featnum, float x, float y )
{
    try
    {
        OpsFloatPoint pt(x, y);

        // Create a point
        Ptr<MgCoordinate> coord = transform->Float2Double(pt);

        MgPoint *pGeom = new MgPoint(coord);

        writeFile(ffgfFile, featnum, pGeom);

        delete pGeom;
    }
    catch (...)
    {
    }
}
