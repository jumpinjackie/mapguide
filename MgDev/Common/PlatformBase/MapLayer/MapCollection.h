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

/// \defgroup MgMapCollection MgMapCollection
/// \ingroup Maps_and_Layers_Module
/// \{

#ifndef _MG_MAP_COLLECTION_H_
#define _MG_MAP_COLLECTION_H_

class MgMapCollection;
template class MG_PLATFORMBASE_API Ptr<MgMapCollection>;

class MgMap;

/////////////////////////////////////////////////////////////////
/// \brief
/// Manipulates collections of MgMapBase objects.
///
class MG_PLATFORMBASE_API MgMapCollection : public MgGuardDisposable
{
    DECLARE_CLASSNAME(MgMapCollection)

PUBLISHED_API:
    ///////////////////////////////////////////////
    /// \brief
    /// Gets the number of map in the collection.
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
    /// Returns the number of maps.
    ///
    INT32 GetCount();

    ////////////////////////////////////////////////////////
    /// \brief
    /// Gets the map at the specified index.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgMapBase GetItem(int index);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgMapBase GetItem(int index);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgMapBase GetItem(int index);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param index (int)
    /// Index of the map to get.
    ///
    /// \return
    /// Returns the specified MgMapBase object.
    ///
    /// \exception MgOutOfRangeException if the index is invalid.
    ///
    MgMapBase* GetItem(INT32 index);

    //////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the map with the specified name.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgMapBase GetItem(string name);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgMapBase GetItem(String name);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgMapBase GetItem(string name);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param name (String/string)
    /// Name of the map to get.
    ///
    /// \return
    /// Returns the specified MgMapBase object.
    ///
    /// \exception MgInvalidArgumentException if the name does not exist in
    ///  the collection.
    ///
    MgMapBase* GetItem(CREFSTRING name);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the map in the collection at the specified index to a
    /// specified value.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void SetItem(int index, MgMapBase value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void SetItem(int index, MgMapBase value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void SetItem(int index, MgMapBase value);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param index (int)
    /// Position in the collection to set the
    /// MgMapBase.
    /// \param value (MgMapBase)
    /// MgMapBase to be set in the collection.
    ///
    /// \return
    /// Returns nothing.
    ///
    /// \exception MgOutOfRangeException if the index is out of range.
    ///
    void SetItem(INT32 index, MgMapBase* value);

    ////////////////////////////////////////////////////////////////////
    /// \brief
    /// Adds the specified map to the end of the collection.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void Add(MgMapBase value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void Add(MgMapBase value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void Add(MgMapBase value);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param value (MgMapBase)
    /// MgMapBase to be added to the collection.
    ///
    /// \return
    /// Returns nothing.
    ///
    void Add(MgMapBase* value);

    ///////////////////////////////////////////////////////////////////////
    /// \brief
    /// Inserts a map at a specified index within the collection.
    ///
    /// \remarks
    /// Maps following the insertion point are moved down to
    /// accommodate the new map.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void Insert(int index, MgMapBase value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void Insert(int index, MgMapBase value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void Insert(int index, MgMapBase value);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param index (int)
    /// Position in the collection to insert the
    /// MgMapBase.
    /// \param value (MgMapBase)
    /// MgMapBase to be inserted in the collection.
    ///
    /// \return
    /// Returns nothing.
    ///
    /// \exception MgOutOfRangeException if the specified index is out of
    ///  range.
    ///
    void Insert(INT32 index, MgMapBase* value);

    /////////////////////////////////////////
    /// \brief
    /// Removes all maps from the collection.
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

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Removes a map from the collection.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// bool Remove(MgMapBase value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// bool Remove(MgMapBase value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// bool Remove(MgMapBase value);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param value (MgMapBase)
    /// MgMapBase to be removed
    ///
    /// \return
    /// Returns true if removal was successful.
    ///
    ///
    bool Remove(MgMapBase* value);

    ///////////////////////////////////////////////////////////////
    /// \brief
    /// Removes a map from the collection at the specified index.
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
    /// Index of the map to be removed.
    ///
    /// \return
    /// Returns nothing.
    ///
    /// \exception MgOutOfRangeException if the map does not exist in the
    ///  collection.
    ///
    void RemoveAt(INT32 index);

    ///////////////////////////////////////////////////////////////
    /// \brief
    /// Determines if the collection contains the specified map.
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
    /// Name of the map.
    ///
    /// \return
    /// Returns true if the collection contains the specified map,
    /// or false otherwise.
    ///
    bool Contains(CREFSTRING name);

    ///////////////////////////////////////////////////////////////
    /// \brief
    /// Determines if the collection contains the specified map.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// bool Contains(MgMapBase value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// boolean Contains(MgMapBase value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// bool Contains(MgMapBase value);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param value (MgMapBase)
    /// The MgMapBase to look for.
    ///
    /// \return
    /// Returns true if the collection contains the specified map,
    /// or false otherwise.
    ///
    bool Contains(MgMapBase* value);

    ///////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the index of the specified map in the collection.
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
    /// Name of the map to get the index of.
    ///
    /// \return
    /// Returns the map's index if the collection contains the
    /// specified map, or false (-1) otherwise.
    ///
    INT32 IndexOf(CREFSTRING name);

    //////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the index of the specified map in the collection.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// int IndexOf(MgMapBase value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// int IndexOf(MgMapBase value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// int IndexOf(MgMapBase value);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param value (MgMapBase)
    /// Value of the item to get the index of.
    ///
    /// \return
    /// Returns the map's index if the collection contains the
    /// specified map, or false (-1) otherwise.
    ///
    INT32 IndexOf(MgMapBase* value);

    /// \brief
    /// Constructs an empty MgMapCollection object.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgMapCollection();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgMapCollection();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgMapCollection();
    /// \htmlinclude SyntaxBottom.html
    ///
    MgMapCollection();

protected:
    /// \brief
    /// Destruct a MgMapCollection object
    ///
    /// \return
    /// Nothing
    ///
    virtual ~MgMapCollection();

    /// \brief
    /// Get the unique identifier for the class
    ///
    /// \return
    /// Class Identifider.
    ///
    virtual INT32 GetClassId() { return m_cls_id; }

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
    static const INT32 m_cls_id = PlatformBase_MapLayer_MapCollection;

private:
    MgNamedCollection*   m_maps;
};
/// \}

#endif
