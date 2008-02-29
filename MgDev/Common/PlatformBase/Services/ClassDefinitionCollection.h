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

#ifndef _MG_CLASS_DEFINITION_COLLECTION_H
#define _MG_CLASS_DEFINITION_COLLECTION_H

/// \defgroup MgClassDefinitionCollection MgClassDefinitionCollection
/// \ingroup Collections_Module
/// \{

class MgClassDefinitionCollection;
template class MG_PLATFORMBASE_API Ptr<MgClassDefinitionCollection>;

//////////////////////////////////////////////////////////////////
/// \brief
/// ClassDefinitionCollection represents a collection of class definitions in a feature schema.
///
/// \remarks
/// All class definitions are objects of MgClassDefinition class
///
/// \note
/// Collections are <i>not</i> thread safe and ordered in the sequence of add operation.
///
class MG_PLATFORMBASE_API MgClassDefinitionCollection : public MgCollection
{
    MG_DECL_DYNCREATE();
    DECLARE_CLASSNAME(MgClassDefinitionCollection)

PUBLISHED_API:
    //////////////////////////////////////////////////////////
    /// \brief
    /// Constructs a MgClassDefinitionCollection.  The collection is initially empty.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgClassDefinitionCollection();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgClassDefinitionCollection();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgClassDefinitionCollection();
    /// \htmlinclude SyntaxBottom.html
    ///
    MgClassDefinitionCollection();

    //////////////////////////////////////////////////////////
    /// \brief
    /// Sets the item in the collection at the specified index to the specified value.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void SetItem(int index, MgClassDefinition value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void SetItem(int index, MgClassDefinition value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void SetItem(int index, MgClassDefinition value);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param index (int)
    /// Input index
    /// \param value (MgClassDefinition)
    /// Input value
    ///
    /// \return
    /// Returns nothing.
    ///
    /// \exception MgIndexOutOfRangeException if the index is out of range.
    /// \exception MgDuplicateObjectException if it is a duplicate.
    ///
    void SetItem(INT32 index, MgClassDefinition* value);

    //////////////////////////////////////////////////////////
    /// \brief
    /// Adds the specified item to the end of the collection.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void Add(MgClassDefinition value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void Add(MgClassDefinition value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void Add(MgClassDefinition value);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param value (MgClassDefinition)
    /// Input value
    ///
    /// \return
    /// Returns nothing
    ///
    /// \exception MgDuplicateObjectException if the index is a duplicate.
    ///
    void Add(MgClassDefinition* value);

    //////////////////////////////////////////////////////////
    /// \brief
    /// Inserts the specified item at the specified index within the collection.
    /// Items following the insertion point are moved down to accommodate the new item.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void Insert(int index, MgClassDefinition value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void Insert(int index, MgClassDefinition value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void Insert(int index, MgClassDefinition value);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param index (int)
    /// Input index
    /// \param value (MgClassDefinition)
    /// Input value
    ///
    /// \return
    /// Returns nothing.
    ///
    /// \exception MgIndexOutOfRangeException if the index is out of range.
    /// \exception MgDuplicateObjectException if it is a duplicate.
    ///
    void Insert(INT32 index, MgClassDefinition* value);

    //////////////////////////////////////////////////////////
    /// \brief
    /// Removes the specified item from the collection.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// bool Remove(MgClassDefinition value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// bool Remove(MgClassDefinition value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// bool Remove(MgClassDefinition value);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param value (MgClassDefinition)
    /// Input value
    ///
    /// \return
    /// Returns true if removal was successful
    ///
    ///
    bool Remove(MgClassDefinition* value);

    /// \brief
    /// Gets the item in the collection at the specified index.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgClassDefinition GetItem(int index) const;
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgClassDefinition GetItem(int index) const;
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgClassDefinition GetItem(int index) const;
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param index (int)
    /// Input index
    ///
    /// \return
    /// Returns the item in the collection at the specified index.
    ///
    /// \exception MgIndexOutOfRangeException if the index is out of range.
    ///
    MgClassDefinition* GetItem(INT32 index) const;

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
    /// Input index
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
    /// bool Contains(const MgClassDefinition value) const;
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// boolean Contains(const MgClassDefinition value) const;
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// bool Contains(const MgClassDefinition value) const;
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param value (MgClassDefinition)
    /// Input value
    ///
    /// \return
    /// Returns true if the collection contains the specified item, false otherwise.
    ///
    bool Contains(const MgClassDefinition* value) const;

    /// \brief
    /// Returns the index of the specified item in the collection or -1 if the item does not exist.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// int IndexOf(const MgClassDefinition value) const;
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// int IndexOf(const MgClassDefinition value) const;
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// int IndexOf(const MgClassDefinition value) const;
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param value (MgClassDefinition)
    /// Input value
    ///
    /// \return
    /// Returns the index of the specified item in the collection or -1 if the item does not exist.
    ///
    INT32 IndexOf(const MgClassDefinition* value) const;

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
    ~MgClassDefinitionCollection();

INTERNAL_API:

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Serializes data to a TCP/IP stream.
    ///
    /// \param stream
    /// Stream
    ///
    void Serialize(MgStream* stream);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Deserializes data from a TCP/IP stream.
    ///
    /// \param stream
    /// Stream
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
    static const INT32 m_cls_id = PlatformBase_FeatureService_ClassDefinitionCollection;
};
/// \}

#endif //_MG_CLASS_DEFINITION_COLLECTION_H
