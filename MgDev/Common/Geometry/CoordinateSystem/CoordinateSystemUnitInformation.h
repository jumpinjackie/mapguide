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

#ifndef _MGCOORDINATESYSTEMUNITINFORMATION_H_
#define _MGCOORDINATESYSTEMUNITINFORMATION_H_

///////////////////////////////////////////////////////////////////////////////////////////////
/// \brief
/// Represents information about a coordinate system unit
///
class MgCoordinateSystemUnitInformation : public MgGuardDisposable
{
    DECLARE_CLASSNAME(MgCoordinateSystemUnitInformation)

PUBLISHED_API:
    virtual MgCoordinateSystemEnumInteger32* GetEnumLinearUnits()=0;
    virtual MgCoordinateSystemEnumInteger32* GetEnumAngularUnits()=0;
    virtual double GetLinearUnitScale(INT32 nUnitCode)=0;
    virtual double GetAngularUnitScale(INT32 nUnitCode)=0;
    virtual INT32 GetUnitType(INT32 nUnitCode)=0;
    virtual STRING GetTagString(INT32 nUnitCode)=0;
    virtual STRING GetAbbreviation(INT32 nUnitCode)=0;

protected:
    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Get the class Id
    ///
    /// \return
    /// The integer value
    ///
    INT32 GetClassId(){return m_cls_id;};

CLASS_ID:
    static const INT32 m_cls_id = CoordinateSystem_CoordinateSystemUnitInformation;
};

#endif //_MGCOORDINATESYSTEMUNITINFORMATION_H_
