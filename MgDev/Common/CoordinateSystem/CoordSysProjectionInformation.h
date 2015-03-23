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

#ifndef _CCOORDINATESYSTEMPROJECTIONINFORMATION_H_
#define _CCOORDINATESYSTEMPROJECTIONINFORMATION_H_

namespace CSLibrary
{

class CCoordinateSystemProjectionInformation : public MgCoordinateSystemProjectionInformation
{
public:
    CCoordinateSystemProjectionInformation();
    virtual ~CCoordinateSystemProjectionInformation();

    virtual MgCoordinateSystemEnumInteger32* GetEnumProjections();
    virtual INT32 GetParameterCount(INT32 nProjectionCode);
    virtual bool IsUsingParameter(INT32 nProjectionCode, UINT32 ulIndex);
    virtual bool IsUsingOriginLongitude(INT32 nProjectionCode);
    virtual bool IsUsingOriginLatitude(INT32 nProjectionCode);
    virtual bool IsUsingScaleReduction(INT32 nProjectionCode);
    virtual bool IsUsingQuadrant(INT32 nProjectionCode);
    virtual bool IsUsingOffset(INT32 nProjectionCode);
    virtual double GetParameterMin(INT32 nProjectionCode, UINT32 ulIndex);
    virtual double GetParameterMax(INT32 nProjectionCode, UINT32 ulIndex);
    virtual double GetParameterDefault(INT32 nProjectionCode, UINT32 ulIndex);
    virtual INT32 GetParameterLogicalType(INT32 nProjectionCode, UINT32 ulIndex);
    virtual INT32 GetParameterFormatType(INT32 nProjectionCode, UINT32 ulIndex);
    virtual INT32 GetParameterType(INT32 nProjectionCode, UINT32 ulIndex);
    virtual INT32 GetUnitType(INT32 nProjectionCode);
    virtual STRING GetTagString(INT32 nProjectionCode);

protected:
    virtual void Dispose();

private:
    CCoordinateSystemProjectionInformation(const CCoordinateSystemProjectionInformation&);
    CCoordinateSystemProjectionInformation& operator=(const CCoordinateSystemProjectionInformation&);
};

} // End of namespace

#endif //_CCOORDINATESYSTEMPROJECTIONINFORMATION_H_
