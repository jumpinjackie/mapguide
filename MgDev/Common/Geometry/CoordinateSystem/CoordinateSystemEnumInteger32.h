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

#ifndef _MGCOORDINATESYSTEMENUMINTEGER32_H_
#define _MGCOORDINATESYSTEMENUMINTEGER32_H_

class MgIntCollection;
class MgCoordinateSystemEnumInteger32 : public MgGuardDisposable
{
    DECLARE_CLASSNAME(MgCoordinateSystemEnumInteger32)

PUBLISHED_API:
//    virtual MgIntCollection* Next(UINT32 ulCount)=0;
    virtual MgStringCollection* Next(UINT32 ulCount)=0;
    virtual void Skip(UINT32 ulSkipCount)=0;
    virtual void Reset()=0;
    virtual MgCoordinateSystemEnumInteger32* CreateClone()=0;
    virtual void AddFilter(MgCoordinateSystemFilterInteger32 *pFilter)=0;

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
    static const INT32 m_cls_id = CoordinateSystem_CoordinateSystemEnumInteger32;
};

#endif //_MGCOORDINATESYSTEMENUMINTEGER32_H_
