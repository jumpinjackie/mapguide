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

#include "Foundation.h"
#include "CoordSysInformation.h"

using namespace CSLibrary;

CCoordinateSystemInformation::CCoordinateSystemInformation()
{
    m_code = L"";
    m_description = L"";
    m_projection = L"";
    m_projectionDescription = L"";
    m_datum = L"";
    m_datumDescription = L"";
    m_ellipsoid = L"";
    m_ellipsoidDescription = L"";
    m_proj4Definition = L"";
}

CCoordinateSystemInformation::~CCoordinateSystemInformation()
{
}

CCoordinateSystemInformation* CCoordinateSystemInformation::Clone()
{
    CCoordinateSystemInformation* coordSys = new CCoordinateSystemInformation();
    if (coordSys == NULL)
    {
        throw new MgOutOfMemoryException(L"CCoordinateSystemInformation.Clone", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    coordSys->m_code = m_code;
    coordSys->m_description = m_description;
    coordSys->m_projection = m_projection;
    coordSys->m_projectionDescription = m_projectionDescription;
    coordSys->m_datum = m_datum;
    coordSys->m_datumDescription = m_datumDescription;
    coordSys->m_ellipsoid = m_ellipsoid;
    coordSys->m_ellipsoidDescription = m_ellipsoidDescription;
    coordSys->m_proj4Definition = m_proj4Definition;

    return coordSys;
}
