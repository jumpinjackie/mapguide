//
//  Copyright (C) 2004-2009 by Autodesk, Inc.
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

#ifndef _MGWEBCOMMANDCOLLECTION_H_
#define _MGWEBCOMMANDCOLLECTION_H_

class MgWebCommandCollection;
template class MG_WEBAPP_API Ptr<MgWebCommandCollection>;

class MG_WEBAPP_API MgWebCommandCollection : public MgGuardDisposable
{
PUBLISHED_API:

    /// <summary>
    /// Returns the number of items in the collection
    /// </summary>
    /// <returns>
    /// Number of items
    /// </returns>
    INT32 GetCount();

    /// <summary>
    /// Returns the item at the specified index
    /// Throws an out of range exception if the index is invalid
    /// </summary>
    /// <param name="index">
    /// Index of the item
    /// </param>
    /// <returns>
    /// Command
    /// </returns>
    MgWebCommand* GetItem(INT32 index);

    /// <summary>
    /// Returns the item with the specified name
    /// Throws an illegal argument exception if the name does not exist
    /// </summary>
    /// <param name="name">
    /// Name of the item
    /// </param>
    /// <returns>
    /// Command.
    /// </returns>
    MgWebCommand* GetItem(CREFSTRING name);

    /// <summary>
    /// Returns true if the collection contains the specified item, false otherwise
    /// </summary>
    /// <param name="name">
    /// Name of the item
    /// </param>
    /// <returns>
    /// true if the collection contains the specified item, false otherwise
    /// </returns>
    bool Contains(CREFSTRING name);

    /// <summary>
    /// Returns true if the collection contains the specified item, false otherwise
    /// </summary>
    /// <param name="value">
    /// Value of the item
    /// </param>
    /// <returns>
    /// true if the collection contains the specified item, false otherwise
    /// </returns>
    bool Contains(MgWebCommand* value);

    /// <summary>
    /// Returns the index of the specified item in the collection or -1 if the item does not exist.
    /// </summary>
    /// <param name="name">
    /// Name of the item
    /// </param>
    /// <returns>
    /// index if the collection contains the specified item, -1 otherwise
    /// </returns>
    INT32 IndexOf(CREFSTRING name);

    /// <summary>
    /// Returns the index of the specified item in the collection or -1 if the item does not exist.
    /// </summary>
    /// <param name="value">
    /// Value of the item
    /// </param>
    /// <returns>
    /// index if the collection contains the specified item, false otherwise
    /// </returns>
    INT32 IndexOf(MgWebCommand* value);

INTERNAL_API:

    /// <summary>
    /// Constructs an empty MgWebCommandCollection object.
    /// </summary>
    MgWebCommandCollection();

    /// <summary>
    /// Adds the specified item to the end of the collection.
    /// </summary>
    /// <param name="value">
    /// Command
    /// </param>
    /// <returns>
    /// index of the newly added item
    /// </returns>
    void Add(MgWebCommand* value);

    /// <summary>
    /// Inserts the specified item at the specified index within the collection.
    /// Items following the insertion point are moved down to accommodate the new item.
    /// Throws an out of range argument exception if the specified index is out of range
    /// </summary>
    /// <param name="index">
    /// Position in the collection
    /// </param>
    /// <param name="value">
    /// Command to be inserted in the collection
    /// </param>
    void Insert(INT32 index, MgWebCommand* value);

    /// <summary>
    /// Removes all items from the collection
    /// </summary>
    void Clear();

    /// <summary>
    /// Removes an item from the collection
    /// </summary>
    /// <param name="value">
    /// Command to be removed
    /// </param>
    bool Remove(MgWebCommand* value);

    /// <summary>
    /// Removes an item from the collection at the specified index
    /// Throws an out of range exception if the item does not exist within the collection.
    /// </summary>
    /// <param name="index">
    /// index of the item to be removed
    /// </param>
    void RemoveAt(INT32 index);

protected:

    ///<summary>
    /// Get the unique identifier for the class
    ///</summary>
    ///<returns>Class Identifider.</returns>
    virtual INT32 GetClassId();

    // <summary>Dispose this object.</summary>
    // <returns>Returns nothing</returns>
    virtual void Dispose();

CLASS_ID:
    static const INT32 m_cls_id = WebApp_CommandCollection;

private:
    Ptr<MgNamedCollection>   m_commands;
};

#endif
