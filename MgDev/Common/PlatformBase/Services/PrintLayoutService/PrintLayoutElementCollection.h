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

/// \defgroup MgPrintLayoutElementCollection MgPrintLayoutElementCollection
/// \ingroup Print_Layout_Service_Module
/// \{

#ifndef MGPRINTLAYOUTELEMENTCOLLECTION_H_
#define MGPRINTLAYOUTELEMENTCOLLECTION_H_

class MgPrintLayoutBase;
class MgPrintLayoutElementCollection;
template class MG_PLATFORMBASE_API Ptr<MgPrintLayoutElementCollection>;

/////////////////////////////////////////////////////////////////
/// \brief
/// Manipulates collections of MgPrintLayoutElementBase objects.
///
class MG_PLATFORMBASE_API MgPrintLayoutElementCollection : public MgCollection
{
    MG_DECL_DYNCREATE();
    DECLARE_CLASSNAME(MgPrintLayoutElementCollection)

PUBLISHED_API:
    //////////////////////////////////////////////////////////
    /// \brief
    /// Gets the number of print layout elements in the collection.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// int GetCount();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// int GetCount();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// int GetCount();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the number of print layout elements.
    ///
    INT32 GetCount() const;

    //////////////////////////////////////////////////////////
    /// \brief
    /// Gets the print layout element at the specified index.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgPrintLayoutElementBase GetItem(int index);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgPrintLayoutElementBase GetItem(int index);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgPrintLayoutElementBase GetItem(int index);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param index (int)
    /// Index of the print layout element to get.
    ///
    /// \return
    /// Returns the print layout element.
    ///
    /// \exception MgIndexOutOfRangeException if the specified index is out of range.
    ///
    MgPrintLayoutElementBase* GetItem(INT32 index);

    //////////////////////////////////////////////////////////
    /// \brief
    /// Returns the print layout element with the specified name.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgPrintLayoutElementBase GetItem(string name);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgPrintLayoutElementBase GetItem(String name);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgPrintLayoutElementBase GetItem(string name);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param name (String/string)
    /// Name of the print layout element to get.
    ///
    /// \return
    /// Returns the print layout element.
    ///
    /// \exception MgObjectNotFoundException if the name does not exist.
    ///
    MgPrintLayoutElementBase* GetItem(CREFSTRING name);

    //////////////////////////////////////////////////////////
    /// \brief
    /// Sets the print layout element in the collection at the specified index to
    /// the specified value.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void SetItem(int index, MgPrintLayoutElementBase value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void SetItem(int index, MgPrintLayoutElementBase value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void SetItem(int index, MgPrintLayoutElementBase value);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param index (int)
    /// Position in the collection to set the print layout element.
    /// \param value (MgPrintLayoutElementBase)
    /// MgPrintLayoutElementBase to be set in the collection.
    ///
    /// \return
    /// Returns nothing.
    ///
    /// \exception MgIndexOutOfRangeException if the specified index is out of range.
    ///
    void SetItem(INT32 index, MgPrintLayoutElementBase* value);

    //////////////////////////////////////////////////////////
    /// \brief
    /// Adds a print layout element to the end of the collection.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void Add(MgPrintLayoutElementBase value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void Add(MgPrintLayoutElementBase value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void Add(MgPrintLayoutElementBase value);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param value (MgPrintLayoutElementBase)
    /// Print layout element to be added.
    ///
    /// \return
    /// Returns nothing.
    ///
    void Add(MgPrintLayoutElementBase* value);

    //////////////////////////////////////////////////////////
    /// \brief
    /// Inserts a print layout element at a specified location in the collection.
    ///
    /// \remarks
    /// Print layout elements following the insertion point are moved down to
    /// accommodate the new print layout element.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void Insert(int index, MgPrintLayoutElementBase value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void Insert(int index, MgPrintLayoutElementBase value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void Insert(int index, MgPrintLayoutElementBase value);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param index (int)
    /// Position in the collection to insert the print layout element.
    /// \param value (MgPrintLayoutElementBase)
    /// Print layout element to be inserted in the collection.
    ///
    /// \return
    /// Returns nothing.
    ///
    /// \exception MgIndexOutOfRangeException if the specified index is out of range.
    ///
    void Insert(INT32 index, MgPrintLayoutElementBase* value);

    //////////////////////////////////////////////////////////
    /// \brief
    /// Removes all print layout elements from the collection.
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

    //////////////////////////////////////////////////////////
    /// \brief
    /// Removes a print layout element from the collection.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// bool Remove(MgPrintLayoutElementBase value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// bool Remove(MgPrintLayoutElementBase value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// bool Remove(MgPrintLayoutElementBase value);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param value (MgPrintLayoutElementBase)
    /// Print layout element to be removed.
    ///
    /// \return
    /// Returns true if removal was successful.
    ///
    ///
    bool Remove(MgPrintLayoutElementBase* value);

    //////////////////////////////////////////////////////////
    /// \brief
    /// Removes a print layout element from the collection at the specified index.
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
    /// Index of the print layout element to be removed.
    ///
    /// \return
    /// Returns nothing.
    ///
    /// \exception MgIndexOutOfRangeException if the specified index is out of range.
    ///
    void RemoveAt(INT32 index);

    //////////////////////////////////////////////////////////
    /// \brief
    /// Determines if the collection contains the specified print layout element.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// bool Contains(string name);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// boolean Contains(String name);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// bool Contains(string name);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param name (String/string)
    /// Name of the print layout element to search for.
    ///
    /// \return
    /// Returns true if the collection contains the specified item,
    /// or false otherwise.
    ///
    bool Contains(CREFSTRING name);

    //////////////////////////////////////////////////////////
    /// \brief
    /// Determines if the collection contains the specified print layout element.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// bool Contains(MgPrintLayoutElementBase value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// boolean Contains(MgPrintLayoutElementBase value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// bool Contains(MgPrintLayoutElementBase value);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param value (MgPrintLayoutElementBase)
    /// MgPrintLayoutElementBase to search for.
    ///
    /// \return
    /// Returns true if the collection contains the specified item,
    /// or false otherwise.
    ///
    bool Contains(MgPrintLayoutElementBase* value);

    //////////////////////////////////////////////////////////
    /// \brief
    /// Gets the index of the specified print layout element in the collection.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// int IndexOf(string name);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// int IndexOf(String name);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// int IndexOf(string name);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param name (String/string)
    /// Name of the print layout element to search for.
    ///
    /// \return
    /// Returns the print layout element's index if the collection contains the
    /// specified print layout element, or false (-1) otherwise.
    ///
    INT32 IndexOf(CREFSTRING name);

    //////////////////////////////////////////////////////////
    /// \brief
    /// Gets the index of the specified print layout element in the collection.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// int IndexOf(MgPrintLayoutElementBase value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// int IndexOf(MgPrintLayoutElementBase value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// int IndexOf(MgPrintLayoutElementBase value);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param value (MgPrintLayoutElementBase)
    /// MgPrintLayoutElementBase to search for.
    ///
    /// \return
    /// Returns the print layout element's index if the collection contains the
    /// specified print layout element, or false (-1) otherwise.
    ///
    INT32 IndexOf(MgPrintLayoutElementBase* value);

INTERNAL_API:

    //////////////////////////////////////////////////////////
    /// \brief
    /// Creates an XML document representing the collection.
    ///
    /// \return
    /// A pointer to an MgByteReader object.
    ///
    MgByteReader* ToXml();

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

    /// \brief
    /// Constructs an empty MgPrintLayoutElementCollection object.
    ///
    MgPrintLayoutElementCollection(MgPrintLayoutBase* owner);

    /// \brief
    /// Gets the print layout owning this collection
    ///
    MgPrintLayoutBase* GetPrintLayout();

protected:
    /// \brief
    /// To shut Ptr<> up
    ///
    MgPrintLayoutElementCollection();

    /// \brief
    /// Destruct a MgPrintLayoutElementCollection object
    ///
    /// \return
    /// Nothing
    ///
    virtual ~MgPrintLayoutElementCollection();

    /// \brief
    /// Get the unique identifier for the class
    ///
    /// \return
    /// Class Identifider.
    ///
    virtual INT32 GetClassId()
    {
        return m_cls_id;
    }

    /// \brief
    /// Dispose this object.
    ///
    /// \return
    /// Returns nothing
    ///
    virtual void Dispose()
    {
        delete this;
    }

CLASS_ID:
    static const INT32 m_cls_id = PlatformBase_PrintLayoutService_PrintLayoutElementCollection;

private:
    Ptr<MgNamedCollection> m_elements;
    Ptr<MgPrintLayoutBase> m_owner;
};
/// \}

#endif
