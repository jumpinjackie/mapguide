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

#ifndef _MG_NAMED_COLLECTION_H
#define _MG_NAMED_COLLECTION_H

#include <map>
#include <cctype>
using namespace std;

#ifdef _WIN32
#pragma once
#endif

class MgNamedCollection;
template class MG_FOUNDATION_API Ptr<MgNamedCollection>;

// Build a map when the collection exceeds this number of elements.
// GetItem by name is faster through the map, when the collection
// exceeds 10 elements. The threshold is set a bit higher to account
// for the overhead of building and deleting the map.
#define COLL_MAP_THRESHOLD 10

/// \cond INTERNAL
/////////////////////////////////////////////////////////////////
/// \brief
/// NamedCollection is a template for collections keyed by element name.  The
/// MgNamedSerializable class must provide a GetName() function that returns the
/// element name as a String* and a CanSetName() function that returns true if
/// the class allows modification of the name (implements SetName()), and false
/// if it does not. This class also provides fast access by name for large
/// collections. When CanSetName() returns true, the access by name is a little
/// less efficient because linear searches need to be done in some cases. The
/// reason for this is that after the name of an object changes, that object
/// is no longer in the right position in the collection's name map.
class MG_FOUNDATION_API MgNamedCollection : public MgCollection
{
    DECLARE_CLASSNAME(MgNamedCollection)

INTERNAL_API:
    //////////////////////////////////////////////////////////
    /// \brief
    /// Gets the number of items in the collection.
    ///
    /// \return
    /// Returns number of items in the collection
    ///
    virtual INT32 GetCount() const;

    //////////////////////////////////////////////////////////
    /// \brief
    /// Gets the item in the collection at the specified index position. Throws an invalid argument exception if the index is out of range.
    ///
    /// \param index
    /// Input index
    ///
    /// \return
    /// Returns the item in the collection at the specified index position.
    ///
    /// \exception MgIndexOutOfRangeException
    ///
    virtual MgNamedSerializable* GetItem(INT32 index) const;

    //////////////////////////////////////////////////////////
    /// \brief
    /// Gets the item in the collection with the specified name. Throws an exception if the item is not found.
    ///
    /// \param name
    /// Input item name
    ///
    /// \return
    /// Returns the item in the collection with the specified name.
    ///
    /// \exception MgObjectNotFoundException
    ///
    virtual MgNamedSerializable* GetItem(CREFSTRING name);

    //////////////////////////////////////////////////////////
    /// \brief
    /// Finds the item in the collection with the specified name.
    ///
    /// \param name
    /// Input item name
    ///
    /// \return
    /// Returns the item in the collection with the specified name.
    /// Returns NULL if the item was not found.
    ///
    virtual MgNamedSerializable* FindItem(CREFSTRING name);

    //////////////////////////////////////////////////////////
    /// \brief
    /// Sets the item in the collection at the specified index position to the specified value. Throws an invalid argument exception if the index is out of range.
    ///
    /// \param index
    /// Input index
    /// \param value
    /// Input value
    ///
    /// \return
    /// Returns nothing.
    ///
    /// \exception MgDuplicateObjectException
    ///
    virtual void SetItem(INT32 index, MgNamedSerializable* value);

    //////////////////////////////////////////////////////////
    /// \brief
    /// Adds the specified item to the end of the collection.
    ///
    /// \param value
    /// Input value
    ///
    /// \return
    /// Returns nothing
    ///
    /// \exception MgDuplicateObjectException
    ///
    virtual void Add(MgNamedSerializable* value);

    //////////////////////////////////////////////////////////
    /// \brief
    /// Inserts the specified item at the specified index position.
    /// Items following the insertion point are moved down to accommodate the new item.
    /// Throws an invalid argument exception if the specified index is out of range.
    ///
    /// \param index
    /// Input index
    /// \param value
    /// Input value
    ///
    /// \return
    /// Returns nothing.
    ///
    /// \exception MgDuplicateObjectException
    ///
    virtual void Insert(INT32 index, MgNamedSerializable* value);

    //////////////////////////////////////////////////////////
    /// \brief
    /// Removes all items from the collection.
    ///
    /// \return
    /// Returns nothing.
    ///
    virtual void Clear();

    //////////////////////////////////////////////////////////
    /// \brief
    /// Removes the specified item from the collection.
    /// \param value
    /// Input value
    ///
    /// \return
    ///  Returns true if successful.
    ///
    ///
    virtual bool Remove(const MgNamedSerializable* value);

    //////////////////////////////////////////////////////////
    /// \brief
    /// Removes the specified item from the collection. Throws an invalid argument exception if the item does not exist in the collection.
    ///
    /// \param index
    /// Input index
    ///
    /// \return
    /// Returns nothing
    ///
    /// \exception MgIndexOutOfRangeException
    ///
    virtual void RemoveAt(INT32 index);

    //////////////////////////////////////////////////////////
    /// \brief
    /// Returns true if the collection contains the specified item, false otherwise.
    ///
    /// \param value
    /// Input value
    ///
    /// \return
    /// Returns true if the collection contains the specified item, false otherwise.
    ///
    virtual bool Contains(const MgNamedSerializable* value);

    //////////////////////////////////////////////////////////
    /// \brief
    /// Returns true if the collection contains the specified item, false otherwise.
    ///
    /// \param name
    /// Input the item name
    ///
    /// \return
    /// Returns true if the collection contains the specified item, false otherwise.
    ///
    virtual bool Contains(CREFSTRING name);

    //////////////////////////////////////////////////////////
    /// \brief
    /// Returns the index of the specified item in the collection or -1 if the item does not exist.
    ///
    /// \param value
    /// Input value
    ///
    /// \return
    /// Returns the index of the specified item in the collection or -1 if the item does not exist.
    ///
    virtual INT32 IndexOf(const MgNamedSerializable* value) const;

    //////////////////////////////////////////////////////////
    /// \brief
    /// Returns the index of the specified item (by name) in the collection or -1 if the item does not exist.
    ///
    /// \param name
    /// Input the item name
    ///
    /// \return
    /// Returns the index of the specified item in the collection or -1 if the item does not exist.
    ///
    /// \exception MgInvalidArgumentException
    ///
    virtual INT32 IndexOf(CREFSTRING name) const;

    //////////////////////////////////////////////////////////
    /// \brief
    /// Creates an XML document representing the collection.
    ///
    /// \return
    /// A pointer to an MgByteReader object.
    ///
    virtual MgByteReader* ToXml();

    //////////////////////////////////////////////////////////
    /// \brief
    /// Serializes data to a TCP/IP stream.
    ///
    /// \param stream
    /// Stream
    ///
    virtual void Serialize(MgStream* stream);

    //////////////////////////////////////////////////////////
    /// \brief
    /// Deserializes data from a TCP/IP stream.
    ///
    /// \param stream
    /// Stream
    ///
    virtual void Deserialize(MgStream* stream);


    MgNamedCollection(bool allowDuplicate, bool caseSensitive);
    MgNamedCollection(bool caseSensitive = true);
    bool GetAllowDuplicate();
    bool GetCaseSenstive();
    void SetCheckForDuplicates(bool bDup);

protected:

    //////////////////////////////////////////////////////////
    /// \brief
    /// Gets the class Id.
    ///
    /// \return
    /// Returns the integer value of the class Id.
    ///
    INT32 GetClassId();

    //////////////////////////////////////////////////////////
    /// \brief
    /// Disposes the object.
    ///
    virtual void Dispose();

    virtual ~MgNamedCollection();
    int Compare(CREFSTRING str1, CREFSTRING str2) const;
    void CheckDuplicate(MgNamedSerializable* item, INT32 index);

private:

    void InitMap();
    void InsertMap(MgNamedSerializable* value);

    /// Remove the element at the specified index from the map.
    void RemoveMapAt(INT32 index);

    /// Remove the given element from the map.
    void RemoveMap(const MgNamedSerializable* value);

    /// Get the named element from the map. Returns NULL if element not in map.
    MgNamedSerializable* GetMap(CREFSTRING name) const;

    STRING Lower(STRING str) const;

    /// list data
    Ptr<MgDisposableCollection> m_dCollection;

    /// A map keyed by name, to help performance of large collections.
    std::map<STRING, MgNamedSerializable* > *m_pNameMap;

    bool m_bAllowDuplicate;
    bool m_bCaseSensitive;
    bool m_bCheckDuplicate;

CLASS_ID:
    static const INT32 m_cls_id = Foundation_Collection_NamedCollection;
};
/// \endcond

#endif
