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

#ifndef _MG_PARAMETER_COLLECTION_H
#define _MG_PARAMETER_COLLECTION_H

/// \defgroup MgParameterCollection MgParameterCollection
/// \ingroup Collections_Module
/// \{

class MgParameterCollection;
template class MG_PLATFORMBASE_API Ptr<MgParameterCollection>;

//////////////////////////////////////////////////////////////////
/// \brief
/// MgParameterCollection represents a collection of parameters.
/// All parameters contained in an instance of a parameter
/// collection are objects whose base class is MgParameter.
/// A call to a parameter object's GetType() method returns one
/// of the integer constants defined in class MgPropertyType. 
/// This collection does not allow duplicate key names. 
///
/// \note
/// Collections are <i>not</i> thread safe and ordered in the sequence of add operation.
///
class MG_PLATFORMBASE_API MgParameterCollection : public MgCollection
{
    MG_DECL_DYNCREATE();
    DECLARE_CLASSNAME(MgParameterCollection)

PUBLISHED_API:
    //////////////////////////////////////////////////////////
    /// \brief
    /// Constructs a MgParameterCollection. 
    /// The collection is initially empty.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgParameterCollection();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgParameterCollection();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgParameterCollection();
    /// \htmlinclude SyntaxBottom.html
    ///
    MgParameterCollection();

    //////////////////////////////////////////////////////////
    /// \brief
    /// Sets the item in the collection at the specified index to the specified value.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void SetItem(int index, MgParameter value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void SetItem(int index, MgParameter value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void SetItem(int index, MgParameter value);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param index (int)
    /// Input index.
    /// \param value (MgParameter)
    /// Input value.
    ///
    /// \return
    /// Returns nothing.
    ///
    /// \exception MgIndexOutOfRangeException if the index is out of range.
    /// \exception MgDuplicateObjectException if it is a duplicate.
    ///
    void SetItem(INT32 index, MgParameter* value);

    //////////////////////////////////////////////////////////
    /// \brief
    /// Adds the specified item to the end of the collection.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// int Add(MgParameter value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// int Add(MgParameter value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// int Add(MgParameter value);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param value (MgParameter)
    /// Input value.
    ///
    /// \return
    /// Returns nothing.
    ///
    /// \exception MgDuplicateObjectException if the index is a duplicate.
    ///
    void Add(MgParameter* value);

    //////////////////////////////////////////////////////////
    /// \brief
    /// Inserts the specified item at the specified index within the collection.
    /// Items following the insertion point are moved down to accommodate the new item.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void Insert(int index, MgParameter value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void Insert(int index, MgParameter value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void Insert(int index, MgParameter value);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param index (int)
    /// Input index
    /// \param value (MgParameter)
    /// Input value.
    ///
    /// \return
    /// Returns nothing.
    ///
    /// \exception MgIndexOutOfRangeException if the index is out of range.
    /// \exception MgDuplicateObjectException if it is a duplicate.
    ///
    void Insert(INT32 index, MgParameter* value);

    //////////////////////////////////////////////////////////
    /// \brief
    /// Removes the specified item from the collection.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// bool Remove(MgParameter value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// bool Remove(MgParameter value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// bool Remove(MgParameter value);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param value (MgParameter)
    /// Input value.
    ///
    /// \return
    /// Returns true if removal was successful.
    ///
    bool Remove(MgParameter* value);

    /// \brief
    /// Gets the item in the collection at the specified index.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgParameter GetItem(int index) const;
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgParameter GetItem(int index) const;
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgParameter GetItem(int index) const;
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param index (int)
    /// Input index.
    ///
    /// \return
    /// Returns the item in the collection at the specified index.
    ///
    /// \exception MgIndexOutOfRangeException if the index is out of range.
    ///
    MgParameter* GetItem(INT32 index) const;

    /// \brief
    /// Gets the number of items in the collection.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// int GetCount() const;
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// int GetCount() const;
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// int GetCount() const;
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the number of items in the collection.
    ///
    INT32 GetCount() const;

    /// \brief
    /// Removes all items from the collection.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void Clear();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void Clear();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void Clear();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns nothing.
    ///
    void Clear();

    /// \brief
    /// Removes the specified item from the collection.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void RemoveAt(int index);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void RemoveAt(int index);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void RemoveAt(int index);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param index (int)
    /// Input index.
    ///
    /// \return
    /// Returns nothing.
    ///
    /// \exception MgIndexOutOfRangeException
    ///
    void RemoveAt(INT32 index);

    /// \brief
    /// Returns true if the collection contains the specified item, false otherwise.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// bool Contains(const MgParameter value) const;
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// boolean Contains(const MgParameter value) const;
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// bool Contains(const MgParameter value) const;
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param value (MgParameter)
    /// Input value.
    ///
    /// \return
    /// Returns true if the collection contains the specified item, false otherwise.
    ///
    bool Contains(const MgParameter* value) const;

    /// \brief
    /// Returns the index of the specified item in the collection or -1 if the item does not exist.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// int IndexOf(const MgParameter value) const;
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// int IndexOf(const MgParameter value) const;
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// int IndexOf(const MgParameter value) const;
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param value (MgParameter)
    /// Input value.
    ///
    /// \return
    /// Returns the index of the specified item in the collection or -1 if the item does not exist.
    ///
    INT32 IndexOf(const MgParameter* value) const;

EXTERNAL_API:
    //////////////////////////////////////////////////////////
    /// \brief
    /// Creates an XML document representing the collection.
    ///
    /// \return
    /// A pointer to an MgByteReader object.
    ///
    MgByteReader* ToXml();

    //////////////////////////////////////////////////////////
    /// \brief
    /// Deletes the collection and the class definitions contained in it.
    ///
    /// \return
    /// Returns nothing.
    ///
    ~MgParameterCollection();

INTERNAL_API:
    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Serializes data to a TCP/IP stream.
    ///
    /// \param stream
    /// Stream.
    ///
    void Serialize(MgStream* stream);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Deserializes data from a TCP/IP stream.
    ///
    /// \param stream
    /// Stream.
    ///
    void Deserialize(MgStream* stream);

protected:

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the class Id.
    ///
    /// \return
    /// Returns the integer value of the class Id.
    ///
    INT32 GetClassId();

    virtual void Dispose();

private:

    Ptr<MgDisposableCollection> m_dCollection;

CLASS_ID:
    static const INT32 m_cls_id = PlatformBase_Collection_ParameterCollection;
};
/// \}

#endif //_MG_FEATURE_SCHEMA_COLLECTION_H
