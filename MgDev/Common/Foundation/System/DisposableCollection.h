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

#ifndef _MG_DISPOSABLE_COLLECTION_H_
#define _MG_DISPOSABLE_COLLECTION_H_


class MgDisposableCollection;
template class MG_FOUNDATION_API Ptr<MgDisposableCollection>;

/// \cond INTERNAL
/////////////////////////////////////////////////////////////////
/// \brief
/// This class represents a collection of MgDisposable objects.  The objects
/// are owned and managed by the collection.
class MG_FOUNDATION_API MgDisposableCollection : public MgCollection
{
    MG_DECL_DYNCREATE()
    DECLARE_CLASSNAME(MgDisposableCollection)

PUBLISHED_API:
    //////////////////////////////////////////////////////////
    /// \brief
    /// Constructs a DisposableCollection.  The collection is initially empty.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgDisposableCollection();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgDisposableCollection();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgDisposableCollection();
    /// \htmlinclude SyntaxBottom.html
    ///
    MgDisposableCollection();

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
    /// Gets the item in the collection at the specified index. Throws an invalid argument exception if the index is out of range.
    ///
    /// \param index
    /// Input index
    ///
    /// \return
    /// Returns the item in the collection at the specified index
    ///
    /// \exception MgIndexOutOfRangeException
    ///
    virtual MgDisposable* GetItem(INT32 index) const;

    //////////////////////////////////////////////////////////
    /// \brief
    /// Sets the item in the collection at the specified index to the specified value. Throws an invalid argument exception if the index is out of range.
    ///
    /// \param index
    /// Input index
    /// \param value
    /// Input value
    ///
    /// \return
    /// Returns nothing
    ///
    /// \exception MgIndexOutOfRangeException
    ///
    virtual void SetItem(INT32 index, MgDisposable* value);

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
    virtual void Add(MgDisposable* value);

    //////////////////////////////////////////////////////////
    /// \brief
    /// Inserts the specified item at the specified index within the collection.
    /// Items following the insertion point are moved down to accommodate the new item.
    /// Throws an invalid argument exception if the specified index is out of range.
    ///
    /// \param index
    /// Input index
    /// \param value
    /// Input value
    ///
    /// \return
    /// Returns nothing
    ///
    /// \exception MgIndexOutOfRangeException
    ///
    virtual void Insert(INT32 index, MgDisposable* value);

    //////////////////////////////////////////////////////////
    /// \brief
    /// Removes all items from the collection.
    ///
    /// \return
    /// Returns nothing
    ///
    virtual void Clear();

    //////////////////////////////////////////////////////////
    /// \brief
    /// Removes the specified item from the collection.
    ///
    /// \param value
    /// Input value
    ///
    /// \return
    /// Returns true if removal was successful
    ///
    ///
    virtual bool Remove(const MgDisposable* value);

    //////////////////////////////////////////////////////////
    /// \brief
    /// Removes the specified item from the collection. Throws an invalid argument exception if the item does not exist within the collection.
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
    /// Returns true if the collection contains the specified item, false otherwise
    ///
    virtual bool Contains(const MgDisposable* value) const;

    //////////////////////////////////////////////////////////
    /// \brief
    /// Returns the index of the specified item in the collection or -1 if the item does not exist.
    ///
    /// \param value
    /// Input value
    ///
    /// \return
    /// Returns the index of the specified item in the collection or -1 if the item does not exist
    ///
    virtual INT32 IndexOf(const MgDisposable* value) const;

    //////////////////////////////////////////////////////////
    /// \brief
    /// Creates an XML document representing the collection.
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

EXTERNAL_API:

    //////////////////////////////////////////////////////////
    /// \brief
    /// Deletes the collection and the properties contained in it.
    ///
    /// \return
    /// Returns nothing.
    ///
    ~MgDisposableCollection();

INTERNAL_API:

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

    static const INT32 INIT_CAPACITY = 10;

private:

    /// resize the array
    void resize();

    MgDisposable** m_list;
    INT32          m_capacity;
    INT32          m_size;

CLASS_ID:
    static const INT32 m_cls_id = Foundation_Collection_Collection;
};
/// \endcond

#endif
