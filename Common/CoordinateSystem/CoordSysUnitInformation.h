//
//  Copyright (C) 2004-2010 by Autodesk, Inc.
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

#ifndef _CCOORDINATESYSTEMUNITINFORMATION_H_
#define _CCOORDINATESYSTEMUNITINFORMATION_H_

namespace CSLibrary
{

class CCoordinateSystemUnitInformation : public MgCoordinateSystemUnitInformation
{
public:
    CCoordinateSystemUnitInformation();
    virtual ~CCoordinateSystemUnitInformation();

    virtual MgCoordinateSystemEnumInteger32* GetEnumLinearUnits();
    virtual MgCoordinateSystemEnumInteger32* GetEnumAngularUnits();
    virtual double GetLinearUnitScale(INT32 nUnitCode);
    virtual double GetAngularUnitScale(INT32 nUnitCode);
    virtual INT32 GetUnitType(INT32 nUnitCode);
    virtual STRING GetTagString(INT32 nUnitCode);
    virtual STRING GetAbbreviation(INT32 nUnitCode);

protected:
    virtual void Dispose();

private:
    //Private member functions
    double GetScale(INT32 nUnitCode, INT32 nDesiredUnitType);

private:
    CCoordinateSystemUnitInformation(const CCoordinateSystemUnitInformation&);
    CCoordinateSystemUnitInformation& operator=(const CCoordinateSystemUnitInformation&);
};

} // End of namespace

#endif //_CCOORDINATESYSTEMUNITINFORMATION_H_