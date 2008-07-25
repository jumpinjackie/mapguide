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

#ifndef MGSTRINGPROPERTYCOLLECTION_H_
#define MGSTRINGPROPERTYCOLLECTION_H_

/// \cond INTERNAL
/////////////////////////////////////////////////////////////////
/// \brief
/// This class collects key-value pairs in Strings. The key name should be unique.
/// 
/// \remarks
/// The class can be useful in collecting requests, for example, http://localhost/MapAgent.php?REQUEST=TEST1&PARAM=TEST2
/// The collection could have REQUEST, PARAM as keys with TEST1, TEST2 as associated values respectively.
/// 
/// \note
/// Collections are NOT thread safe and ordered in the sequence of add operation.
class MG_PLATFORMBASE_API MgStringPropertyCollection : public MgCollection
{
    DECLARE_CREATE_OBJECT()
    DECLARE_CLASSNAME(MgStringPropertyCollection)

EXTERNAL_API:
    //////////////////////////////////////////////////////////
    /// \brief
    /// Constructs a StringPropertyCollection.  The collection is initially empty.
    ///
    MgStringPropertyCollection();

    //////////////////////////////////////////////////////////
    /// \brief
    /// Deletes the collection and the strings contained in it.
    ///
    ~MgStringPropertyCollection();

    //////////////////////////////////////////////////////////
    /// \brief
    /// Gets the number of items in the collection.
    ///
    /// \return
    /// Returns the number of items in the collection.
    ///
    virtual INT32 GetCount() const;

    //////////////////////////////////////////////////////////
    /// \brief
    /// Gets the name of the item in the collection at the specified index position.
    ///
    /// \param index
    /// Input index
    ///
    /// \return
    /// Returns the name of the item in the collection at the specified index position.
    ///
    /// \exception MgIndexOutOfRangeException if the index is out of range.
    ///
    virtual STRING GetName(INT32 index) const;

    //////////////////////////////////////////////////////////
    /// \brief
    /// Gets the value of the item in the collection at the specified index position.
    ///
    /// \param index
    /// Input index
    ///
    /// \return
    /// Returns the value of the item in the collection at the specified index position.
    ///
    /// \exception MgIndexOutOfRangeException if the index is out of range.
    ///
    virtual STRING GetValue(INT32 index) const;

    //////////////////////////////////////////////////////////
    /// \brief
    /// Gets the item in the collection with the specified key.
    ///
    /// \param keyname
    /// Key for item
    ///
    /// \return
    /// Returns the item in the collection at the specified index position.
    ///
    /// \exception MgObjectNotFoundException if the key does not exist.
    ///
    virtual STRING GetValue(CREFSTRING keyname) const;

    //////////////////////////////////////////////////////////
    /// \brief
    /// Sets the item in the collection with the specified key to the specified value.
    ///
    /// \param keyname
    /// Input key name
    /// \param value
    /// Input value
    ///
    /// \return
    /// Returns nothing.
    ///
    /// \exception MgObjectNotFoundException if the key does not exist.
    ///
    virtual void SetValue(CREFSTRING keyname, CREFSTRING value);

    //////////////////////////////////////////////////////////
    /// \brief
    /// Adds the specified item to the end of the collection. 
    ///
    /// \param keyname
    /// Name for input value
    /// \param value
    /// Input value
    ///
    /// \return
    /// Returns nothing.
    ///
    /// \exception MgInvalidArgumentException if the key is empty.
    ///
    virtual void Add(CREFSTRING keyname,CREFSTRING value);

    //////////////////////////////////////////////////////////
    /// \brief
    /// Adds item to the collection.
    ///
    /// \param property
    /// Property to add
    ///
    /// \return
    /// Returns nothing
    ///
    void Add(MgStringProperty* value);

    //////////////////////////////////////////////////////////
    /// \brief
    /// Inserts the specified item at the specified index position within the collection.
    /// Items following the insertion point are moved down to accommodate the new item.
    ///
    /// \param index
    /// Insertion point
    /// \param keyname
    /// Input key
    /// \param value
    /// Input value
    ///
    /// \return
    /// Returns nothing.
    ///
    /// \exception MgIndexOutOfRangeException if the index is out of range.
    ///
    virtual void Insert(INT32 index, CREFSTRING keyname, CREFSTRING value);

    //////////////////////////////////////////////////////////
    /// \brief
    /// Inserts the specified item at the specified index position within the collection.
    /// Items following the insertion point are moved down to accommodate the new item.
    ///
    /// \param index
    /// Insertion point
    /// \param keyname
    /// Input key
    /// \param value
    /// Input value
    ///
    /// \return
    /// Returns nothing.
    ///
    /// \exception MgIndexOutOfRangeException if the index is out of range.
    ///
    virtual void Insert(INT32 index, MgStringProperty* value);

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
    ///
    /// \param keyname
    /// Key for item
    ///
    /// \return
    /// Returns true if removal was successful.
    ///
    virtual bool Remove(CREFSTRING keyname);

    //////////////////////////////////////////////////////////
    /// \brief
    /// Removes the specified item from the collection.
    ///
    /// \param value
    /// Property to remove
    ///
    /// \return
    /// Returns true if removal was successful.
    ///
    virtual bool Remove(MgStringProperty* value);

    //////////////////////////////////////////////////////////
    /// \brief
    /// Removes the specified item from the collection.
    ///
    /// \param index
    /// Input index
    ///
    /// \return
    /// Returns nothing.
    ///
    /// \exception MgIndexOutOfRangeException if the index is out of range.
    ///
    virtual void RemoveAt(INT32 index);

    //////////////////////////////////////////////////////////
    /// \brief
    /// Returns true if the collection contains the specified item, false otherwise.
    ///
    /// \param keyname
    /// Key for item to find
    ///
    /// \return
    /// Returns true if the collection contains the specified item, false otherwise.
    ///
    virtual bool Contains(CREFSTRING keyname);

    //////////////////////////////////////////////////////////
    /// \brief
    /// Returns true if the collection contains the specified item, false otherwise.
    ///
    /// \param value
    /// Property to find
    ///
    /// \return
    /// Returns true if the collection contains the specified item, false otherwise.
    ///
    virtual bool Contains(MgStringProperty* value);

    //////////////////////////////////////////////////////////
    /// \brief
    /// Returns the index of the specified item in the collection or -1 if the item does not exist.
    ///
    /// \param keyname
    /// Key for item
    ///
    /// \return
    /// Returns the index of the specified item in the collection or -1 if the item does not exist.
    ///
    virtual INT32 IndexOf(CREFSTRING keyname);

    //////////////////////////////////////////////////////////
    /// \brief
    /// Returns the index of the specified item in the collection or -1 if the item does not exist.
    ///
    /// \param value
    /// Property to search for
    ///
    /// \return
    /// Returns the index of the specified item in the collection or -1 if the item does not exist.
    ///
    virtual INT32 IndexOf(MgStringProperty* value);

    //////////////////////////////////////////////////////////
    /// \brief
    /// Gets the item in the collection at the specified index position.
    ///
    /// \param index
    /// Input index
    ///
    /// \return
    /// Returns the item in the collection at the specified index position.
    ///
    /// \exception MgInvalidArgumentException
    ///
    MgStringProperty* GetItem(INT32 index);

    //////////////////////////////////////////////////////////
    /// \brief
    /// Sets the item in the collection at the specified index position.
    ///
    /// \param index
    /// Input index
    ///
    /// \param value
    /// Value to set
    ///
    /// \return
    /// Returns nothing.
    ///
    void SetItem(INT32 index, MgStringProperty* value);

    //////////////////////////////////////////////////////////
    /// \brief
    /// Creates an XML document representing the collection.
    /// The order of the properties in the XML is the same as the order of the properties in the collection.
    /// It starts with item zero to length of collection.
    /// TODO: Add a snippet of XML here?
    ///
    /// \return
    /// A pointer to an MgByteReader object.
    ///
    virtual MgByteReader* ToXml();

INTERNAL_API:
    //////////////////////////////////////////////////////////
    /// \brief
    /// Constructs a StringPropertyCollection.  The collection is initially empty.
    ///
    /// \param allowDuplicate
    /// Allow duplicate values if true.
    /// \param caseSensitive
    /// true - "Abc" will be different from "ABC".
    ///
    MgStringPropertyCollection(bool allowDuplicate, bool caseSensitive);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Serializes data to a TCP/IP stream.
    ///
    /// \param stream
    /// Stream
    ///
    virtual void Serialize(MgStream* stream);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Deserializes data from a TCP/IP stream.
    ///
    /// \param stream
    /// Stream
    ///
    virtual void Deserialize(MgStream* stream);

    //////////////////////////////////////////////////////////
    /// \brief
    /// Builds a log friendly string representation
    ///
    /// \return
    /// String suitable for logs
    ///
    STRING GetLogString();

    //////////////////////////////////////////////////////////
    /// \brief
    /// Returns the property collection this string collection is based upon
    ///
    MgPropertyCollection* GetPropertyCollection();

protected:

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the class Id.
    ///
    /// \return
    /// Returns the integer value of the class Id.
    ///
    INT32 GetClassId();

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Disposes the object.
    ///
    virtual void Dispose();

private:

    Ptr<MgPropertyCollection> m_strProperty;

CLASS_ID:
    static const INT32 m_cls_id = PlatformBase_Collection_StringPropertyCollection;
};
/// \endcond

#endif
