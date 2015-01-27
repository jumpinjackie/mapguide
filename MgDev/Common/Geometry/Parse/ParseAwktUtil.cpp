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

#include "GeometryCommon.h"

#include "ParseAwktUtil.h"

MgCoordinate* MgParseAwktUtil::CreateCoordinate(INT32 dimension, double* doubles, INT32& index)
{
    MgGeometryFactory factory;
    Ptr<MgCoordinate> coord;

    switch(dimension)
    {
        case 0: // XY
        {
            coord = factory.CreateCoordinateXY(doubles[index], doubles[index+1]);
            index += 2;
            break;
        }
        case 1: // XYZ
        {
            coord = factory.CreateCoordinateXYZ(doubles[index], doubles[index+1], doubles[index+2]);
            index += 3;
            break;
        }
        case 2: // XYM
        {
            coord = factory.CreateCoordinateXYM(doubles[index], doubles[index+1], doubles[index+2]);
            index += 3;
            break;
        }
        case 3: // XYZM
        {
            coord = factory.CreateCoordinateXYZM(doubles[index], doubles[index+1], doubles[index+2], doubles[index+3]);
            index += 4;
            break;
        }
    }

    return coord.Detach();
}

MgCoordinateCollection* MgParseAwktUtil::CreateCoordinateCollection(INT32 dimensionType, INT32 numOfCoordinates, double* ordinates, INT32& index)
{
    assert(ordinates != NULL);
    assert(index >= 0);
    assert(numOfCoordinates > 0);

    CHECKARGUMENTNULL(ordinates, L"MgParseAwktUtil.CreateCoordinateCollection");

    Ptr<MgCoordinateCollection> coordCol = new MgCoordinateCollection();
    for (INT32 i = 0; i < numOfCoordinates; i++)
    {
        Ptr<MgCoordinate> coord = CreateCoordinate(dimensionType, ordinates, index);
        coordCol->Add(coord);
    }

    return coordCol.Detach();
}

MgPoint* MgParseAwktUtil::CreatePoint(INT32 dimension, double* doubles, INT32& index)
{
    // points have single context entry and use DimToCount values
    MgGeometryFactory factory;

    Ptr<MgCoordinate> coord = CreateCoordinate(dimension, doubles, index);
    return factory.CreatePoint(coord);
}

MgLineString* MgParseAwktUtil::CreateLineString(INT32 dimensionType, INT32 numOfCoordinates, double* ordinates, INT32& index)
{
    MgGeometryFactory factory;

    Ptr<MgCoordinateCollection> coordCol = CreateCoordinateCollection(dimensionType,numOfCoordinates,ordinates,index);
    return factory.CreateLineString(coordCol);
}

MgLinearRing* MgParseAwktUtil::CreateLinearRing(INT32 dimensionType, INT32 numOfCoordinates, double* ordinates, INT32& index)
{
    MgGeometryFactory factory;

    Ptr<MgCoordinateCollection> coordCol = CreateCoordinateCollection(dimensionType,numOfCoordinates,ordinates,index);
    return factory.CreateLinearRing(coordCol);
}

MgLinearSegment* MgParseAwktUtil::CreateLinearSegment(INT32 dimensionType, INT32 numOfCoordinates, double* ordinates, INT32& index)
{
    MgGeometryFactory factory;

    Ptr<MgCoordinateCollection> coordCol = CreateCoordinateCollection(dimensionType,numOfCoordinates,ordinates,index);
    return factory.CreateLinearSegment(coordCol);
}
