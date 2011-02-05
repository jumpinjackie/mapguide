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

#ifndef _MG_PARSEAWKTUTIL_H_
#define _MG_PARSEAWKTUTIL_H_

class MgCoordinate;
class MgLineString;
class MgCoordinateCollection;
class MgLinearRing;
class MgLinearSegment;
class MgPoint;

/// \cond INTERNAL
class MgParseAwktUtil
{
public:
    static MgCoordinate* CreateCoordinate(INT32 dimension, double* doubles, INT32 &index);
    static MgLineString* CreateLineString(INT32 dimensionType, INT32 numOfOrdinates, double* ordinates, INT32& index);
    static MgCoordinateCollection* CreateCoordinateCollection(INT32 dimensionType, INT32 numOfCoordinates, double* ordinates, INT32& index);
    static MgLinearRing* CreateLinearRing(INT32 dimensionType, INT32 numOfCoordinates, double* ordinates, INT32& index);
    static MgLinearSegment* CreateLinearSegment(INT32 dimensionType, INT32 numOfCoordinates, double* ordinates, INT32& index);
    static MgPoint* CreatePoint(INT32 dimension, double* doubles, INT32& index);
};
/// \endcond

#endif
