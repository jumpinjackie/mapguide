//
//  Copyright (C) 2004-2008 by Autodesk, Inc.
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

#include "GeometryCommon.h"
#include "CoordSysCommon.h"

#include "CoordSysCategoryCollection.h"
#include "CoordSysCategory.h"

using namespace CSLibrary;

CCoordinateSystemCategoryCollection::CCoordinateSystemCategoryCollection()
{
    m_collection = new CSCategoryVector();
    if (m_collection == NULL)
    {
        throw new MgOutOfMemoryException(L"CCoordinateSystemCategoryCollection.CCoordinateSystemCategoryCollection", __LINE__, __WFILE__, NULL, L"", NULL);
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
    if (NULL == coordSysCategory)
    {
        throw new MgNullArgumentException(
            L"CCoordinateSystemCategoryCollection.Add",
            __LINE__, __WFILE__, NULL, L"", NULL);
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
            if(_wcsicmp(category->GetName().c_str(), categoryName.c_str()) == 0)
            {
                bFound = true;
                break;
            }
        }
    }

    return bFound;
}
