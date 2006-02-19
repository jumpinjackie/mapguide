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

#ifndef _CCOORDINATESYSTEMCATEGORY_H_
#define _CCOORDINATESYSTEMCATEGORY_H_

namespace CSLibrary
{
class CCoordinateSystemInformation;
typedef vector<CCoordinateSystemInformation*> CoordinateSystemInformationVector;

class COORDINATE_SYSTEM_API CCoordinateSystemCategory
{
public:
    CCoordinateSystemCategory(CREFSTRING name);
    ~CCoordinateSystemCategory();

    STRING GetCategoryName();
    void Clear();

    void Add(CCoordinateSystemInformation* coordSysInfo);
    size_t ContainsCode(CREFSTRING coordSysCode);
    size_t ContainsProj4(CREFSTRING proj4Defn);
    CoordinateSystemInformationVector* GetCoordinateSystems();

private:
    CCoordinateSystemCategory();

    STRING m_name;
    CoordinateSystemInformationVector* m_coordinateSystems;
};

} // End of namespace

#endif
