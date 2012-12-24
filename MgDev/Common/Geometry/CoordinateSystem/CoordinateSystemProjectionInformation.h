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

#ifndef _MGCOORDINATESYSTEMPROJECTIONINFORMATION_H_
#define _MGCOORDINATESYSTEMPROJECTIONINFORMATION_H_

///////////////////////////////////////////////////////////////////////////////////////////////
/// \brief
/// Represents information about a coordinate system projection
///
class MgCoordinateSystemProjectionInformation : public MgGuardDisposable
{
    DECLARE_CLASSNAME(MgCoordinateSystemProjectionInformation)

PUBLISHED_API:
    virtual MgCoordinateSystemEnumInteger32* GetEnumProjections()=0;
    virtual INT32 GetParameterCount(INT32 nProjectionCode)=0;
    virtual bool IsUsingParameter(INT32 nProjectionCode, UINT32 ulIndex)=0;
    virtual bool IsUsingOriginLongitude(INT32 nProjectionCode)=0;
    virtual bool IsUsingOriginLatitude(INT32 nProjectionCode)=0;
    virtual bool IsUsingScaleReduction(INT32 nProjectionCode)=0;
    virtual bool IsUsingQuadrant(INT32 nProjectionCode)=0;
    virtual bool IsUsingOffset(INT32 nProjectionCode)=0;
    virtual double GetParameterMin(INT32 nProjectionCode, UINT32 ulIndex)=0;
    virtual double GetParameterMax(INT32 nProjectionCode, UINT32 ulIndex)=0;
    virtual double GetParameterDefault(INT32 nProjectionCode, UINT32 ulIndex)=0;
    virtual INT32 GetParameterLogicalType(INT32 nProjectionCode, UINT32 ulIndex)=0;
    virtual INT32 GetParameterFormatType(INT32 nProjectionCode, UINT32 ulIndex)=0;
    virtual INT32 GetParameterType(INT32 nProjectionCode, UINT32 ulIndex)=0;
    virtual INT32 GetUnitType(INT32 nProjectionCode)=0;
    virtual STRING GetTagString(INT32 nProjectionCode)=0;

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
    static const INT32 m_cls_id = CoordinateSystem_CoordinateSystemProjectionInformation;
};

#endif //_MGCOORDINATESYSTEMPROJECTIONINFORMATION_H_
