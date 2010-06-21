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

#ifndef MGPROPERTYDEFINITIONCOLLECTION_H_
#define MGPROPERTYDEFINITIONCOLLECTION_H_

/// \defgroup MgPropertyDefinitionCollection MgPropertyDefinitionCollection
/// \ingroup Collections_Module
/// \{

class MgPropertyDefinitionCollection;
template class MG_PLATFORMBASE_API Ptr<MgPropertyDefinitionCollection>;

/////////////////////////////////////////////////////////////////
/// \brief
/// This class contains property definitions for a feature class and makes up a
/// portion of the complete feature class definition.
///
/// \remarks
/// MgClassDefinition encapsulates the
/// entire Fdo feature class definition.  The following property definitions may be contained
/// in the collection:
/// <TABLE border="1" class="RuledTable">
///   <tr>
///     <th>Class</th>
///     <th>Description</th>
///   </tr>
///   <tr>
///     <td>MgDataPropertyDefinition</td>
///     <td>Defines properties consisting of simple value types, or collections of simple value types.</td>
///   </tr>
///   <tr>
///     <td>MgGeometricPropertyDefinition</td>
///     <td>Defines geometric properties</td>
///   </tr>
///   <tr>
///     <td>MgObjectPropertyDefinition</td>
///     <td>Defines properties which are containers for or collections of objects belonging to another feature class</td>
///   </tr>
/// </TABLE>
/// \note
/// Collections are <i>not</i> thread safe and ordered in the sequence of add operation.
///
class MG_PLATFORMBASE_API MgPropertyDefinitionCollection : public MgCollection
{
    MG_DECL_DYNCREATE();
    DECLARE_CLASSNAME(MgPropertyDefinitionCollection)

PUBLISHED_API:
    //////////////////////////////////////////////////////////
    /// \brief
    /// Constructs a PropertyDefinitionCollection.  Collection is initially empty.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgPropertyDefinitionCollection();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgPropertyDefinitionCollection();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgPropertyDefinitionCollection();
    /// \htmlinclude SyntaxBottom.html
    ///
    MgPropertyDefinitionCollection();

    //////////////////////////////////////////////////////////
    /// \brief
    /// Sets the item in the collection at the specified index to the specified value.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual void SetItem(int index, MgPropertyDefinition value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual void SetItem(int index, MgPropertyDefinition value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual void SetItem(int index, MgPropertyDefinition value);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param index (int)
    /// Input index
    /// \param value (MgPropertyDefinition)
    /// Input value
    ///
    /// \return
    /// Returns nothing
    ///
    /// \exception MgIndexOutOfRangeException if index is out of range
    /// \exception MgDuplicateObjectException if it is duplicate
    ///
    virtual void SetItem(INT32 index, MgPropertyDefinition* value);

    //////////////////////////////////////////////////////////
    /// \brief
    /// Adds the specified item to the end of the collection.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual int Add(MgPropertyDefinition value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual int Add(MgPropertyDefinition value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual int Add(MgPropertyDefinition value);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param value (MgPropertyDefinition)
    /// Input value
    ///
    /// \return
    /// Returns nothing.
    ///
    /// \exception MgDuplicateObjectException if it is duplicate
    ///
    virtual void Add(MgPropertyDefinition* value);

    //////////////////////////////////////////////////////////
    /// \brief
    /// Inserts the specified item at the specified index within the collection.
    /// Items following the insertion point are moved down to accommodate the new item.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual void Insert(int index, MgPropertyDefinition value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual void Insert(int index, MgPropertyDefinition value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual void Insert(int index, MgPropertyDefinition value);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param index (int)
    /// Input index
    /// \param value (MgPropertyDefinition)
    /// Input value
    ///
    /// \return
    /// Returns nothing
    ///
    /// \exception MgIndexOutOfRangeException if index is out of range
    /// \exception MgDuplicateObjectException if it is duplicate
    ///
    virtual void Insert(INT32 index, MgPropertyDefinition* value);

    //////////////////////////////////////////////////////////
    /// \brief
    /// Removes the specified item from the collection.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual bool Remove(MgPropertyDefinition value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual bool Remove(MgPropertyDefinition value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual bool Remove(MgPropertyDefinition value);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param value (MgPropertyDefinition)
    /// Input value
    ///
    /// \return
    /// Returns true if removal was successful
    ///
    virtual bool Remove(MgPropertyDefinition* value);

    //////////////////////////////////////////////////////////
    /// \brief
    /// Gets the item in the collection at the specified index
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgPropertyDefinition GetItem(int index) const;
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgPropertyDefinition GetItem(int index) const;
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgPropertyDefinition GetItem(int index) const;
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param index (int)
    /// Input index
    ///
    /// \return
    /// Returns the item in the collection at the specified index
    ///
    /// \exception MgIndexOutOfRangeException if index is out of range
    ///
    virtual MgPropertyDefinition* GetItem(INT32 index) const;

    //////////////////////////////////////////////////////////
    /// \brief
    /// Gets the item in the collection with the specified name
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgPropertyDefinition GetItem(string name);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgPropertyDefinition GetItem(String name);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgPropertyDefinition GetItem(string name);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param name (String/string)
    /// Input item name
    ///
    /// \return
    /// Returns the item in the collection with the specified name
    ///
    /// \exception MgObjectNotFoundException if the item does not exist within the collection.
    ///
    virtual MgPropertyDefinition* GetItem(CREFSTRING name);

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
    /// Returns number of items in the collection
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
    /// Returns nothing
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
    /// Returns true if the collection contains the specified item, false otherwise
    ///
    virtual bool Contains(const MgPropertyDefinition* value);

    //////////////////////////////////////////////////////////
    /// \brief
    /// Returns true if the collection contains the specified item, false otherwise.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual bool Contains(string name);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual boolean Contains(String name);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual bool Contains(string name);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param name (String/string)
    /// Input the item name
    ///
    /// \return
    /// Returns true if the collection contains the specified item, false otherwise
    ///
    virtual bool Contains(CREFSTRING name);

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
    /// Returns the index of the specified item in the collection or -1 if the item does not exist
    ///
    /// \exception MgInvalidArgumentException
    ///
    virtual INT32 IndexOf(const MgPropertyDefinition* value) const;

    //////////////////////////////////////////////////////////
    /// \brief
    /// Returns the index of the specified item (by name) in the collection or -1 if the item does not exist.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual int IndexOf(string name) const;
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual int IndexOf(String name) const;
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual int IndexOf(string name) const;
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param name (String/string)
    /// Input the item name
    ///
    /// \return
    /// Returns the index of the specified item in the collection or -1 if the item does not exist
    ///
    virtual INT32 IndexOf(CREFSTRING name) const;

EXTERNAL_API:
    //////////////////////////////////////////////////////////
    /// \brief
    /// Creates an XML document representing the collection.
    /// Order of properties in XML would be same as order of properties in collection.
    /// It starts with item zero to length of collection.
    /// TODO: Add a snippet of XML here?
    ///
    /// \return
    /// Pointer to an MgByteReader object
    ///
    virtual MgByteReader* ToXml();

    //////////////////////////////////////////////////////////
    /// \brief
    /// Deletes the collection and the contained properties
    ///
    /// \return
    /// Nothing
    ///
    virtual ~MgPropertyDefinitionCollection();

INTERNAL_API:
    //////////////////////////////////////////////////////////
    /// \brief
    /// Serialize data to TCP/IP stream
    ///
    /// \param stream
    /// Stream
    ///
    virtual void Serialize(MgStream* stream);

    //////////////////////////////////////////////////////////
    /// \brief
    /// Deserialize data from TCP/IP stream
    ///
    /// \param stream
    /// Stream
    ///
    virtual void Deserialize(MgStream* stream);

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
    virtual MgPropertyDefinition* FindItem(CREFSTRING name);

    //////////////////////////////////////////////////////////
    /// \brief
    /// Constructs a PropertyDefinitionCollection.  Collection is initially empty.
    ///
    /// \param allowDuplicate
    /// Allow duplicate values if true
    /// \param caseSensitive
    /// true - "Abc" will be different from "ABC"
    ///
    MgPropertyDefinitionCollection(bool allowDuplicate, bool caseSensitive);

    /// Convert to XML
    void ToXml(string& xmlStr);

    /// Convert to Column
    void ToColumnDefinitions(string& xmlStr);

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

    Ptr<MgPropertyCollection> m_pCollection;

CLASS_ID:
    static const INT32 m_cls_id = PlatformBase_Collection_PropertyDefinitionCollection;
};
/// \}

#endif
