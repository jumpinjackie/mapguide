//
//  Copyright (C) 2004-2006  Autodesk, Inc.
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

#include "CoordSysCommon.h"
#include "CoordSysIncludes.h"

CCoordinateSystemCategoryCollection::CCoordinateSystemCategoryCollection()
{
    m_collection = new CSCategoryVector();
    if (m_collection == NULL)
    {
        throw new COutOfMemoryException(L"CCoordinateSystemCategoryCollection.CCoordinateSystemCategoryCollection", __LINE__, __WFILE__, L"Could not allocate CSCategoryVector.");
    }
}

CCoordinateSystemCategoryCollection::~CCoordinateSystemCategoryCollection()
{
    if(m_collection)
    {
        Clear();
        delete m_collection;
        m_collection = NULL;
    }
}

void CCoordinateSystemCategoryCollection::Add(CCoordinateSystemCategory* coordSysCategory)
{
    if(NULL == coordSysCategory)
    {
        throw new CNullArgumentException(L"CCoordinateSystemCategoryCollection.Add", __LINE__, __WFILE__, L"[1] - CCoordinateSystemCategory pointer.");
    }

    m_collection->push_back(coordSysCategory);
}

CCoordinateSystemCategory* CCoordinateSystemCategoryCollection::GetItem(size_t index) const
{
    return (CCoordinateSystemCategory*)m_collection->at(index);
}

size_t CCoordinateSystemCategoryCollection::GetCount() const
{
    return m_collection->size();
}

void CCoordinateSystemCategoryCollection::Clear()
{
    for(size_t index=0;index<m_collection->size();index++)
    {
        CCoordinateSystemCategory* category = m_collection->at(index);
        delete category;
        category = NULL;
    }

    m_collection->clear();
}

bool CCoordinateSystemCategoryCollection::Contains(CREFSTRING categoryName)
{
    bool bFound = false;

    for(size_t index=0;index<m_collection->size();index++)
    {
        CCoordinateSystemCategory* category = m_collection->at(index);
        if(category)
        {
            if(category->GetCategoryName() == categoryName)
            {
                bFound = true;
                break;
            }
        }
    }

    return bFound;
}
