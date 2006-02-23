/******************************************************************************
 * $Id: ili1readerp.h,v 1.2 2005/08/06 22:21:53 pka Exp $
 *
 * Project:  Interlis 1 Reader
 * Purpose:  Private Declarations for Reader code.
 * Author:   Pirmin Kalberer, Sourcepole AG
 *
 ******************************************************************************
 * Copyright (c) 2004, Pirmin Kalberer, Sourcepole AG
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
 * DEALINGS IN THE SOFTWARE.
 ******************************************************************************
 *
 * $Log: ili1readerp.h,v $
 * Revision 1.2  2005/08/06 22:21:53  pka
 * Area polygonizer added
 *
 * Revision 1.1  2005/07/08 22:10:57  pka
 * Initial import of OGR Interlis driver
 *
 */

#ifndef _CPL_ILI1READERP_H_INCLUDED
#define _CPL_ILI1READERP_H_INCLUDED

#include "ili1reader.h"
#include "ogr_ili1.h"
#include "iom/iom.h"


class ILI1Reader;
class OGRILI1Layer;

/************************************************************************/
/*                              ILI1Reader                              */
/************************************************************************/

class ILI1Reader : public IILI1Reader
{    
private:
    FILE         *fpItf;
    int          nLayers;
    OGRLayer     **papoLayers;
    OGRILI1Layer *curLayer;

public:
                 ILI1Reader();
                ~ILI1Reader();

    int          OpenFile( const char *pszFilename );
    int          ReadModel( const char *pszModelFilename );
    int          ReadFeatures();
    int          ReadTable();
    OGRGeometry  *ReadGeom(char **stgeom, OGRwkbGeometryType eType);
    char         **ReadParseLine();

    void         AddLayer( OGRLayer * poNewLayer );    
    int          AddIliGeom(OGRFeature *feature, int iField, long fpos);
    OGRMultiPolygon* Polygonize( OGRGeometryCollection* poLines );
    OGRLayer     *GetLayer( int );
    OGRLayer     *GetLayerByName( const char* );
    int          GetLayerCount();

    const char*  GetLayerNameString(const char* topicname, const char* tablename);
    const char*  GetLayerName(IOM_BASKET model, IOM_OBJECT table);
    void         AddCoord(OGRLayer* layer, IOM_BASKET model, IOM_OBJECT modelele, IOM_OBJECT typeobj);
    void         AddGeomTable(const char* datalayername, const char* geomname, OGRwkbGeometryType eType);
    void         AddField(OGRLayer* layer, IOM_BASKET model, IOM_OBJECT obj);
    unsigned int GetCoordDim(IOM_BASKET model, IOM_OBJECT typeobj);
};


#endif
