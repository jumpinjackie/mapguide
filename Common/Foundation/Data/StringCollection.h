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

#ifndef MGSTRINGCOLLECTION_H_
#define MGSTRINGCOLLECTION_H_

/// \defgroup MgStringCollection MgStringCollection
/// \ingroup Collections_Module
/// \{

class MgByteReader;
class MgStream;

/////////////////////////////////////////////////////////////////
/// \brief
/// This class represents a collection of strings.  The collection maintains its own copy of strings.
///
/// \note
/// Collections are <i>not</i> thread safe and ordered in the sequence of add operation.
class MG_FOUNDATION_API MgStringCollection : public MgCollection
{
    MG_DECL_DYNCREATE();
    DECLARE_CLASSNAME(MgStringCollection)

PUBLISHED_API:
    //////////////////////////////////////////////////////////
    /// \brief
    /// Constructs a StringCollection.  The collection is initially empty.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgStringCollection();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgStringCollection();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgStringCollection();
    /// \htmlinclude SyntaxBottom.html
    ///
    MgStringCollection();

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
    /// Gets the item in the collection at the specified index.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual string GetItem(int index) const;
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual String GetItem(int index) const;
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual string GetItem(int index) const;
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param index (int)
    /// Input index
    ///
    /// \return
    /// Returns the item in the collection at the specified index.
    ///
    /// \exception MgIndexOutOfRangeException if index is out of range.
    ///
    virtual STRING GetItem(INT32 index) const;

    //////////////////////////////////////////////////////////
    /// \brief
    /// Sets the item in the collection at the specified index position to the specified value.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual void SetItem(int index, string value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual void SetItem(int index, String value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual void SetItem(int index, string value);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param index (int)
    /// Input index
    /// \param value (String/string)
    /// Input value
    ///
    /// \return
    /// Returns nothing.
    ///
    /// \exception MgIndexOutOfRangeException if the index is out of range.
    ///
    virtual void SetItem(INT32 index, CREFSTRING value);

    //////////////////////////////////////////////////////////
    /// \brief
    /// Adds the specified item to the end of the collection.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual int Add(string value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual int Add(String value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual int Add(string value);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param value (String/string)
    /// Input value
    ///
    /// \return
    /// Returns nothing.
    ///
    virtual void Add(CREFSTRING value);

    //////////////////////////////////////////////////////////
    /// \brief
    /// Inserts the specified item at the specified index position within the collection.
    /// Items following the insertion point are moved down to accommodate the new item.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual void Insert(int index, string value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual void Insert(int index, String value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual void Insert(int index, string value);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param index (int)
    /// Input index
    /// \param value (String/string)
    /// Input value
    ///
    /// \return
    /// Returns nothing.
    ///
    /// \exception MgIndexOutOfRangeException if the index is out of range.
    ///
    virtual void Insert(INT32 index, CREFSTRING value);

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
    /// virtual bool Remove(string value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual bool Remove(String value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual bool Remove(string value);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param value (String/string)
    /// Input value
    ///
    /// \return
    /// Returns true if removal was successful.
    ///
    ///
    virtual bool Remove(CREFSTRING value);

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
    /// Returns nothing.
    ///
    /// \exception MgIndexOutOfRangeException if the index is out of range.
    ///
    virtual void RemoveAt(INT32 index);

    //////////////////////////////////////////////////////////
    /// \brief
    /// Returns true if the collection contains the specified item, false otherwise.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual bool Contains(string value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual boolean Contains(String value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual bool Contains(string value);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param value (String/string)
    /// Input value
    ///
    /// \return
    /// Returns true if the collection contains the specified item, false otherwise.
    ///
    virtual bool Contains(CREFSTRING value);

    //////////////////////////////////////////////////////////
    /// \brief
    /// Returns the index of the specified item in the collection or -1 if the item does not exist.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual int IndexOf(string value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual int IndexOf(String value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual int IndexOf(string value);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param value (String/string)
    /// Input value
    ///
    /// \return
    /// Returns the index of the specified item in the collection or -1 if the item does not exist.
    ///
    virtual INT32 IndexOf(CREFSTRING value);

EXTERNAL_API:
    //////////////////////////////////////////////////////////
    /// \brief
    /// Creates an XML document representing the collection.
    /// The order of the properties in the XML is the same as the order of properties in the collection.
    /// It starts with item zero to length of collection.
    /// TODO: Add a snippet of XML here?
    ///
    /// \return
    /// A pointer to an MgByteReader object.
    ///
    virtual MgByteReader* ToXml();

    //////////////////////////////////////////////////////////
    /// \brief
    /// Deletes the collection and the strings contained in it.
    ///
    /// \return
    /// Returns nothing
    ///
    virtual ~MgStringCollection();

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
    /// Creates a string collection by parsing a string.
    /// Each element in the string is separated by the specified
    /// delimiter. If no delimiter is specified, a comma is used.
    ///
    /// \param coll
    /// String encoded collection
    /// \param delim
    /// String delimiter. A comma is used if delim is NULL or empty.
    ///
    /// \return
    /// A collection object.
    ///
    static MgStringCollection* ParseCollection(CREFSTRING coll, CREFSTRING delim);

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

    /// performs index validation
    void ValidateIndex(INT32 index) const;

    std::vector<STRING> m_strProperty;

CLASS_ID:
    static const INT32 m_cls_id = Foundation_Collection_StringCollection;
};
/// \}

#endif
