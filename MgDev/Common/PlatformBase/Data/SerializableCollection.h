//
//  Copyright (C) 2004-2010 by Autodesk, Inc.
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

#ifndef MGSERIALIZABLECOLLECTION_H_
#define MGSERIALIZABLECOLLECTION_H_

class MgSerializableCollection;
template class MG_PLATFORMBASE_API Ptr<MgSerializableCollection>;

/// \cond INTERNAL
/////////////////////////////////////////////////////////////////
/// \brief
/// MgSerializableCollection represents a collection of serializable objects.
/// NOTE:
/// Collections are NOT thread safe and ordered in the sequence of add operation.
class MG_PLATFORMBASE_API MgSerializableCollection : public MgCollection
{
    MG_DECL_DYNCREATE();
    DECLARE_CLASSNAME(MgSerializableCollection)

EXTERNAL_API:
    //////////////////////////////////////////////////////////
    /// \brief
    /// Constructs an empty collection of serializable objects.
    ///
    MgSerializableCollection();

    //////////////////////////////////////////////////////////
    /// \brief
    /// Destructs the collection and the object contained in it.
    ///
    virtual ~MgSerializableCollection();

    //////////////////////////////////////////////////////////
    /// \brief
    /// Adds the specified item to the end of the collection. Returns nothing
    ///
    /// \param value
    /// Input value
    ///
    /// \return
    /// Returns nothing
    ///
    /// \exception MgDuplicateObjectException if it is duplicate.
    ///
    virtual void Add(MgSerializable* value);

    //////////////////////////////////////////////////////////
    /// \brief
    /// Inserts the specified item at the specified index within the collection.
    /// Items following the insertion point are moved down to accommodate the new item.
    ///
    /// \param index
    /// Input index
    /// \param value
    /// Input value
    ///
    /// \return
    /// Returns nothing.
    ///
    /// \exception MgIndexOutOfRangeException if index is out of range.
    /// \exception MgDuplicateObjectException if it is duplicate.
    ///
    virtual void Insert(INT32 index, MgSerializable* value);

    //////////////////////////////////////////////////////////
    /// \brief
    /// Removes the specified item from the collection.
    ///
    /// \param value
    /// Input value
    ///
    /// \return
    /// Returns true if successful.
    ///
    virtual bool Remove(MgSerializable* value);

    //////////////////////////////////////////////////////////
    /// \brief
    /// Sets the item in the collection at the specified index to the specified value.
    ///
    /// \param index
    /// Input index
    /// \param value
    /// Input value
    ///
    /// \return
    /// Returns nothing.
    ///
    /// \exception MgIndexOutOfRangeException if index is out of range.
    /// \exception MgDuplicateObjectException if it is duplicate.
    ///
    virtual void SetItem(INT32 index, MgSerializable* value);

    //////////////////////////////////////////////////////////
    /// \brief
    /// Gets the item in the collection at the specified index.
    ///
    /// \param index
    /// Input index
    ///
    /// \return
    /// Returns the item in the collection at the specified index
    ///
    /// \exception MgIndexOutOfRangeException if the index is out of range.
    ///
    virtual MgSerializable* GetItem(INT32 index) const;

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
    virtual bool Contains(const MgSerializable* value);

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
    virtual INT32 IndexOf(const MgSerializable* value) const;

    //////////////////////////////////////////////////////////
    /// \brief
    /// Creates an XML document representing the collection.
    /// The order of properties in the XML would be same as the order of the properties in the collection.
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

private:

    Ptr<MgDisposableCollection> m_collection;

CLASS_ID:
    static const INT32 m_cls_id = PlatformBase_Collection_SerializableCollection;
};
/// \endcond

#endif
