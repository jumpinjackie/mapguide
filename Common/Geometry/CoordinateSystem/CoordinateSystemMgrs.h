//
//  Copyright (C) 2004-2009 by Autodesk, Inc.
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

#ifndef _MGCOORDINATESYSTEMMGRS_H_
#define _MGCOORDINATESYSTEMMGRS_H_

class MgCoordinateSystemMgrs : public MgCoordinateSystemGridBase
{
PUBLISHED_API:
    //section that reads/writes MGRS coordinates
    virtual STRING ConvertFromLonLat(double dLongitude, double dLatitude, INT32 nPrecision)=0;
    virtual STRING ConvertFromLonLat(MgCoordinate* pLonLat, INT32 nPrecision)=0;
    virtual MgCoordinate* ConvertToLonLat(CREFSTRING sMgrs)=0;

    virtual INT8 GetLetteringScheme()=0;

INTERNAL_API:
    //section that reads/writes MGRS coordinates
    virtual INT32 ConvertFromLonLat(double dLongitude, double dLatitude, INT32 nPrecision, REFSTRING sMgrs)=0;
    virtual INT32 ConvertFromLonLat(MgCoordinate* pLonLat, INT32 nPrecision, REFSTRING sMgrs)=0;
    virtual INT32 ConvertToLonLat(CREFSTRING sMgrs, MgCoordinate* pLonLat)=0;

protected:
    INT32 GetClassId(){return m_cls_id;};

CLASS_ID:
    static const INT32 m_cls_id = CoordinateSystem_CoordinateSystemMgrs;
};

#endif //_MGCOORDINATESYSTEMMGRS_H_
