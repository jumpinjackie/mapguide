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

#ifndef MGBATCHPROPERTYCOLLECTION_H_
#define MGBATCHPROPERTYCOLLECTION_H_

class MgBatchPropertyCollection;
template class MG_FOUNDATION_API Ptr<MgBatchPropertyCollection>;

/// \defgroup MgBatchPropertyCollection MgBatchPropertyCollection
/// \ingroup Collections_Module
/// \{

//////////////////////////////////////////////////////////////////
/// \brief
/// MgBatchPropertyCollection represents a collection of properties. Properties
/// are name/value pairs used for specifying the values of properties.
///
/// \note
/// Collections are NOT thread safe and ordered in the sequence of add operation.
class MG_FOUNDATION_API MgBatchPropertyCollection : public MgCollection
{
    MG_DECL_DYNCREATE();
    DECLARE_CLASSNAME(MgBatchPropertyCollection)

PUBLISHED_API:
    //////////////////////////////////////////////////////////
    /// \brief
    /// Constructs a PropertyDefinitionCollection.  The collection is initially empty.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgBatchPropertyCollection();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgBatchPropertyCollection();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgBatchPropertyCollection();
    /// \htmlinclude SyntaxBottom.html
    ///
    MgBatchPropertyCollection();

    //////////////////////////////////////////////////////////
    /// \brief
    /// Sets the item in the collection at the specified index to the specified value.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual void SetItem(int index, MgPropertyCollection value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual void SetItem(int index, MgPropertyCollection value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual void SetItem(int index, MgPropertyCollection value);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param index (int)
    /// Input index
    /// \param value (MgPropertyCollection)
    /// Input value
    ///
    /// \return
    /// Returns nothing.
    ///
    /// \exception MgIndexOutOfRangeException if index is out of range.
    /// \exception MgDuplicateObjectException if it is duplicate.
    ///
    virtual void SetItem(INT32 index, MgPropertyCollection* value);

    //////////////////////////////////////////////////////////
    /// \brief
    /// Adds the specified item to the end of the collection.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual int Add(MgPropertyCollection value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual int Add(MgPropertyCollection value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual int Add(MgPropertyCollection value);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param value (MgPropertyCollection)
    /// Input value
    ///
    /// \return
    /// Returns nothing.
    ///
    /// \exception MgDuplicateObjectException if it is duplicate.
    ///
    virtual void Add(MgPropertyCollection* value);

    //////////////////////////////////////////////////////////
    /// \brief
    /// Inserts the specified item at the specified index within the collection.
    /// Items following the insertion point are moved down to accommodate the new item.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual void Insert(int index, MgPropertyCollection value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual void Insert(int index, MgPropertyCollection value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual void Insert(int index, MgPropertyCollection value);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param index (int)
    /// Input index
    /// \param value (MgPropertyCollection)
    /// Input value
    ///
    /// \return
    /// Returns nothing.
    ///
    /// \exception MgIndexOutOfRangeException if index is out of range.
    /// \exception MgDuplicateObjectException if it is duplicate.
    ///
    virtual void Insert(INT32 index, MgPropertyCollection* value);

    //////////////////////////////////////////////////////////
    /// \brief
    /// Removes the specified item from the collection.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual bool Remove(MgPropertyCollection value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual bool Remove(MgPropertyCollection value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual bool Remove(MgPropertyCollection value);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param value (MgPropertyCollection)
    /// Input value
    ///
    /// \return
    /// Returns true if removal was successful.
    ///
    ///
    virtual bool Remove(MgPropertyCollection* value);

    //////////////////////////////////////////////////////////
    /// \brief
    /// Gets the item in the collection at the specified index.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgPropertyCollection GetItem(int index) const;
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgPropertyCollection GetItem(int index) const;
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgPropertyCollection GetItem(int index) const;
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param index (int)
    /// Input index
    ///
    /// \return
    /// Returns the item in the collection at the specified index
    ///
    /// \exception MgIndexOutOfRangeException if the index is out of range.
    ///
    virtual MgPropertyCollection* GetItem(INT32 index) const;

    //////////////////////////////////////////////////////////
    /// \brief
    /// Gets the number of items in the collection.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual int GetCount() const;
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual int GetCount() const;
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual int GetCount() const;
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the number of items in the collection.
    ///
    virtual INT32 GetCount() const;

    //////////////////////////////////////////////////////////
    /// \brief
    /// Removes all items from the collection.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual void Clear();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual void Clear();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual void Clear();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns nothing.
    ///
    virtual void Clear();

    //////////////////////////////////////////////////////////
    /// \brief
    /// Removes the specified item from the collection.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual void RemoveAt(int index);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual void RemoveAt(int index);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual void RemoveAt(int index);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param index (int)
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
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual bool Contains(const MgPropertyDefinition value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual boolean Contains(const MgPropertyDefinition value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual bool Contains(const MgPropertyDefinition value);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param value (MgPropertyDefinition)
    /// Input value
    ///
    /// \return
    /// Returns true if the collection contains the specified item, false otherwise.
    ///
    virtual bool Contains(const MgPropertyDefinition* value);

    //////////////////////////////////////////////////////////
    /// \brief
    /// Returns the index of the specified item in the collection or -1 if the item does not exist.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual int IndexOf(const MgPropertyDefinition value) const;
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual int IndexOf(const MgPropertyDefinition value) const;
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual int IndexOf(const MgPropertyDefinition value) const;
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param value (MgPropertyDefinition)
    /// Input value
    ///
    /// \return
    /// Returns the index of the specified item in the collection or -1 if the item does not exist.
    ///
    virtual INT32 IndexOf(const MgPropertyDefinition* value) const;

EXTERNAL_API:
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

    //////////////////////////////////////////////////////////
    /// \brief
    /// Deletes the collection and the properties contained in it.
    ///
    /// \return
    /// Returns nothing.
    ///
    virtual ~MgBatchPropertyCollection();

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

    /// Convert to XML
    void ToXml(string& xmlStr);

protected:

    //////////////////////////////////////////////////////////
    /// \brief
    /// Gets the class Id
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

    Ptr<MgDisposableCollection> m_dCollection;

CLASS_ID:
    static const INT32 m_cls_id = Foundation_Collection_BatchPropertyCollection;
};
/// \}

#endif
