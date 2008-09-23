//
//  Copyright (C) 2004-2008 by Autodesk, Inc.
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

#ifndef _CCOORDINATESYSTEMMATHCOMPARATOR_H_
#define _CCOORDINATESYSTEMMATHCOMPARATOR_H_

namespace CSLibrary
{

class CCoordinateSystemMathComparator : public MgCoordinateSystemMathComparator
{
public:
    CCoordinateSystemMathComparator();
    virtual ~CCoordinateSystemMathComparator();

    virtual bool Same(MgGuardDisposable *pDefinition1, MgGuardDisposable *pDefinition2);
    virtual bool SameEllipsoid(MgCoordinateSystemEllipsoid *pDefinition1, MgCoordinateSystemEllipsoid *pDefinition2);
    virtual bool SameDatum(MgCoordinateSystemDatum *pDefinition1, MgCoordinateSystemDatum *pDefinition2);
    virtual bool SameCoordinateSystem(MgCoordinateSystem *pDefinition1, MgCoordinateSystem *pDefinition2);

protected:
//    INT32 GetClassId();
    //from MgDisposable
    virtual void Dispose();
};

} // End of namespace

#endif //_CCOORDINATESYSTEMMATHCOMPARATOR_H_
