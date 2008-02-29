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

#ifndef _CCOORDINATESYSTEMINFORMATION_H_
#define _CCOORDINATESYSTEMINFORMATION_H_

namespace CSLibrary
{
class CCoordinateSystemInformation
{
public:
    CCoordinateSystemInformation();
    ~CCoordinateSystemInformation();

    CCoordinateSystemInformation* Clone();

    STRING m_code;
    STRING m_description;
    STRING m_projection;
    STRING m_projectionDescription;
    STRING m_datum;
    STRING m_datumDescription;
    STRING m_ellipsoid;
    STRING m_ellipsoidDescription;
    STRING m_proj4Definition;
};

} // End of namespace

#endif
