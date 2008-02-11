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

#ifndef _CCOORDINATESYSTEMCATEGORYCOLLECTION_H_
#define _CCOORDINATESYSTEMCATEGORYCOLLECTION_H_

namespace CSLibrary
{
class CCoordinateSystemCategory;
typedef vector<CCoordinateSystemCategory*> CSCategoryVector;

class CCoordinateSystemCategoryCollection
{
public:
    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Constructs an empty collection of CoordinateSystemCategory objects.
    /// </summary>
    CCoordinateSystemCategoryCollection();

    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Destructs the collection and the object contained in it.
    /// </summary>
    ~CCoordinateSystemCategoryCollection();

    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Adds the specified item to the end of the collection.
    /// </summary>
    /// <param name="coordSysCategory">Input value</param>
    void Add(CCoordinateSystemCategory* coordSysCategory);

    ///////////////////////////////////////////////////////////////////////////
    /// <summary>Gets the item in the collection at the specified index.</summary>
    /// <param name="index">Input index</param>
    /// <returns>Returns the item in the collection at the specified index</returns>
    CCoordinateSystemCategory* GetItem(size_t index) const;

    ///////////////////////////////////////////////////////////////////////////
    /// <summary>Gets the number of items in the collection.</summary>
    /// <returns>Returns the number of items in the collection.</returns>
    size_t GetCount() const;

    ///////////////////////////////////////////////////////////////////////////
    /// <summary>Removes all items from the collection.</summary>
    /// <returns>Returns nothing.</returns>
    void Clear();

    ///////////////////////////////////////////////////////////////////////////
    /// <summary>Returns true if the collection contains the specified item, false otherwise.</summary>
    /// <param name="value">Input value</param>
    /// <returns>Returns true if the collection contains the specified item, false otherwise.</returns>
    bool Contains(CREFSTRING category);

private:
    CSCategoryVector* m_collection;
};

} // End of namespace

#endif
