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

#ifndef _CCOORDINATESYSTEMENUMCOORDINATESYSTEMINCATEGORY_H_
#define _CCOORDINATESYSTEMENUMCOORDINATESYSTEMINCATEGORY_H_

namespace CSLibrary
{

class CCoordinateSystemEnumCoordinateSystemInCategory : public MgCoordinateSystemEnum
{
public:
    CCoordinateSystemEnumCoordinateSystemInCategory(MgCoordinateSystemCatalog *pCatalog);
    CCoordinateSystemEnumCoordinateSystemInCategory(CCoordinateSystemEnumCoordinateSystemInCategory const& copyFrom);
    virtual ~CCoordinateSystemEnumCoordinateSystemInCategory();

    void Initialize(std::vector<STRING> const& allCsNames);

    virtual MgDisposableCollection* Next(UINT32 ulCount);
    virtual void Skip(UINT32 ulSkipCount);
    virtual void Reset();
    virtual MgCoordinateSystemEnum* CreateClone();
    virtual void AddFilter(MgCoordinateSystemFilter *pFilter);
    virtual MgStringCollection* NextName(UINT32 ulCount);
    virtual MgStringCollection* NextDescription(UINT32 ulCount);

protected:
    virtual void Dispose();

protected:
    bool IsFilteredOut(MgGuardDisposable *pDef);
    bool IsFilteredOut(const wchar_t *kpName);
    void ClearFilter();

protected:
    //Data members
    std::vector<STRING> m_csNames;
    std::vector<STRING>::const_iterator m_iter;
    Ptr<MgCoordinateSystemCatalog> m_pCatalog;
    std::vector<MgCoordinateSystemFilter*> m_vectFilter;

private:
    CCoordinateSystemEnumCoordinateSystemInCategory();
};

} // End of namespace

#endif //_CCOORDINATESYSTEMENUMCOORDINATESYSTEMINCATEGORY_H_
