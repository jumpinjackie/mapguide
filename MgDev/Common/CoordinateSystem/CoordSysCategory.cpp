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

CCoordinateSystemCategory::CCoordinateSystemCategory()
{
}

CCoordinateSystemCategory::CCoordinateSystemCategory(CREFSTRING name) :
    m_name(name)
{
    m_coordinateSystems = new CoordinateSystemInformationVector();
    if (m_coordinateSystems == NULL)
    {
        throw new COutOfMemoryException(L"CCoordinateSystemCategory.CCoordinateSystemCategory", __LINE__, __WFILE__, L"Could not allocate CoordinateSystemInformationVector.");
    }
}

CCoordinateSystemCategory::~CCoordinateSystemCategory()
{
    if(m_coordinateSystems)
    {
        Clear();
        delete m_coordinateSystems;
        m_coordinateSystems = NULL;
    }
}

STRING CCoordinateSystemCategory::GetCategoryName()
{
    return m_name;
}

void CCoordinateSystemCategory::Add(CCoordinateSystemInformation* coordSysInfo)
{
    if(NULL == coordSysInfo)
    {
        throw new CNullArgumentException(L"CCoordinateSystemCategory.Add", __LINE__, __WFILE__, L"[1] - CCoordinateSystemInformation pointer.");
    }

    m_coordinateSystems->push_back(coordSysInfo);
}

size_t CCoordinateSystemCategory::ContainsCode(CREFSTRING coordSysCode)
{
    size_t index = -1;
    bool bFound = false;

    for(index=0;index<m_coordinateSystems->size();index++)
    {
        CCoordinateSystemInformation* coordSysInfo = m_coordinateSystems->at(index);
        if(coordSysInfo->m_code == coordSysCode)
        {
            bFound = true;
            break;
        }
    }

    return bFound?index:-1;
}

size_t CCoordinateSystemCategory::ContainsProj4(CREFSTRING proj4Defn)
{
    size_t index = -1;
    bool bFound = false;

    for(index=0;index<m_coordinateSystems->size();index++)
    {
        CCoordinateSystemInformation* coordSysInfo = m_coordinateSystems->at(index);
        if(coordSysInfo->m_proj4Definition == proj4Defn)
        {
            bFound = true;
            break;
        }
    }

    return bFound?index:-1;
}

void CCoordinateSystemCategory::Clear()
{
    for(size_t index=0;index<m_coordinateSystems->size();index++)
    {
        CCoordinateSystemInformation* coordSysInfo = m_coordinateSystems->at(index);
        delete coordSysInfo;
        coordSysInfo = NULL;
    }

    m_coordinateSystems->clear();
}

CoordinateSystemInformationVector* CCoordinateSystemCategory::GetCoordinateSystems()
{
    return m_coordinateSystems;
}
