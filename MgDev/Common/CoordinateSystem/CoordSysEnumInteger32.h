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

#ifndef _CCOORDINATESYSTEMENUMINTEGER32_H_
#define _CCOORDINATESYSTEMENUMINTEGER32_H_

namespace CSLibrary
{

class CCoordinateSystemEnumInteger32 : public MgCoordinateSystemEnumInteger32
{
public:
    CCoordinateSystemEnumInteger32();
    virtual ~CCoordinateSystemEnumInteger32();
    void SetList(INT32 *pInt32s, UINT32 ulSize);

//    virtual MgIntCollection* Next(UINT32 ulCount);
    virtual MgStringCollection* Next(UINT32 ulCount);
    virtual void Skip(UINT32 ulSkipCount);
    virtual void Reset();
    virtual MgCoordinateSystemEnumInteger32* CreateClone();
    virtual void AddFilter(MgCoordinateSystemFilterInteger32 *pFilter);

protected:
    virtual void Dispose();

protected:
    bool IsFilteredOut(INT32 nValue);
    void ClearFilter();

protected:
    //Data members
    std::vector<MgCoordinateSystemFilterInteger32*> m_vectFilter;
    //Data members
    INT32 *m_pnValues;
    UINT32 m_ulSize;
    UINT32 m_ulPos;

private:
    CCoordinateSystemEnumInteger32(const CCoordinateSystemEnumInteger32&);
    CCoordinateSystemEnumInteger32& operator=(const CCoordinateSystemEnumInteger32&);
};

} // End of namespace

#endif //_CCOORDINATESYSTEMENUMINTEGER32_H_
