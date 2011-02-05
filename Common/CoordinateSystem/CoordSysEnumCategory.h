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

#ifndef _CCOORDINATESYSTEMENUMCATEGORY_H_
#define _CCOORDINATESYSTEMENUMCATEGORY_H_

namespace CSLibrary
{

class CCoordinateSystemEnumCategory : public MgCoordinateSystemEnum
{
public:
    CCoordinateSystemEnumCategory();
    virtual ~CCoordinateSystemEnumCategory();

    void Initialize(MgCoordinateSystemCategoryDictionary* pDict, CCategoryNameList *kpCategoryNameList);

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
    void ClearFilter();
    bool IsFilteredOut(MgGuardDisposable *pDef);
    bool IsFilteredOut(const char *kpName);

    //Data members
    MgCoordinateSystemCategoryDictionary *m_pDict;
    CCategoryNameList *m_pCategoryNameList;
    CCategoryNameList::const_iterator m_iter;
    std::vector<MgCoordinateSystemFilter*> m_vectFilter;

private:
    CCoordinateSystemEnumCategory(const CCoordinateSystemEnumCategory&);
    CCoordinateSystemEnumCategory& operator=(const CCoordinateSystemEnumCategory&);
};

} // End of namespace

#endif //_CCOORDINATESYSTEMENUMCATEGORY_H_
