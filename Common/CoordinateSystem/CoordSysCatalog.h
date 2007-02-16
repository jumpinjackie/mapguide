//
//  Copyright (C) 2004-2007 by Autodesk, Inc.
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

#ifndef _CCOORDINATESYSTEMCATALOG_H_
#define _CCOORDINATESYSTEMCATALOG_H_

namespace CSLibrary
{
class CCoordinateSystemCategoryCollection;
class CCoordinateSystemCategory;

class CCoordinateSystemCatalog
{
public:
    CCoordinateSystemCatalog();
    ~CCoordinateSystemCatalog();

    CCoordinateSystemCategoryCollection* GetCoordinateSystemCategories();
    CCoordinateSystemCategory* GetCoordinateSystemCategory(CREFSTRING categoryName);

private:
    CCoordinateSystemCategoryCollection* m_categories;

    void ReadCategoryDictionary(CREFSTRING fileName);
    void ReadCategoryCoordinateSystems(CREFSTRING fileName, CCoordinateSystemCategory* category);
    STRING ReadString(FILE* file, int size);
};

} // End of namespace

#endif
