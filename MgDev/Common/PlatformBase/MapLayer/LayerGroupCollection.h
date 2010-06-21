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

/// \defgroup MgLayerGroupCollection MgLayerGroupCollection
/// \ingroup Maps_and_Layers_Module
/// \{

#ifndef _MG_LAYER_GROUP_COLLECTION_H_
#define _MG_LAYER_GROUP_COLLECTION_H_

class MgMapBase;
class MgLayerGroupCollection;
template class MG_PLATFORMBASE_API Ptr<MgLayerGroupCollection>;

////////////////////////////////////////////////////
/// \brief
/// Manipulates collections of MgLayerGroup objects.
///
class MG_PLATFORMBASE_API MgLayerGroupCollection : public MgGuardDisposable
{
    DECLARE_CLASSNAME(MgLayerGroupCollection)

PUBLISHED_API:
    //////////////////////////////////////////////////////////
    /// \brief
    /// Gets the number of layer groups in the collection.
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
    /// Returns the number of layer groups.
    ///
    INT32 GetCount();

    //////////////////////////////////////////////////////////
    /// \brief
    /// Gets the layer group at the specified index.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgLayerGroup GetItem(int index);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgLayerGroup GetItem(int index);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgLayerGroup GetItem(int index);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param index (int)
    /// Index of the layer group to get.
    ///
    /// \return
    /// Returns the layer group.
    ///
    /// \exception MgIndexOutOfRangeException if the specified index is out of range.
    ///
    MgLayerGroup* GetItem(INT32 index);

    //////////////////////////////////////////////////////////
    /// \brief
    /// Returns the layer group with the specified name.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgLayerGroup GetItem(string name);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgLayerGroup GetItem(String name);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgLayerGroup GetItem(string name);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param name (String/string)
    /// Name of the layer group to get.
    ///
    /// \return
    /// Returns the layer group.
    ///
    /// \exception MgObjectNotFoundException if the name does not exist.
    ///
    MgLayerGroup* GetItem(CREFSTRING name);

    //////////////////////////////////////////////////////////
    /// \brief
    /// Sets the layer group in the collection at the specified index to
    /// the specified value.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void SetItem(int index, MgLayerGroup value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void SetItem(int index, MgLayerGroup value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void SetItem(int index, MgLayerGroup value);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param index (int)
    /// Position in the collection to set the layer group.
    /// \param value (MgLayerGroup)
    /// MgLayerGroup to be set in the collection.
    ///
    /// \return
    /// Returns nothing.
    ///
    /// \exception MgIndexOutOfRangeException if the specified index is out of range.
    ///
    void SetItem(INT32 index, MgLayerGroup* value);

    //////////////////////////////////////////////////////////
    /// \brief
    /// Adds a layer group to the end of the collection.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void Add(MgLayerGroup value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void Add(MgLayerGroup value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void Add(MgLayerGroup value);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param value (MgLayerGroup)
    /// Layer group to be added.
    ///
    /// \return
    /// Returns nothing.
    ///
    void Add(MgLayerGroup* value);

    //////////////////////////////////////////////////////////
    /// \brief
    /// Inserts a layer group at a specified location in the collection.
    ///
    /// \remarks
    /// Layer groups following the insertion point are moved down to
    /// accommodate the new layer group.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void Insert(int index, MgLayerGroup value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void Insert(int index, MgLayerGroup value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void Insert(int index, MgLayerGroup value);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param index (int)
    /// Position in the collection to insert the layer group.
    /// \param value (MgLayerGroup)
    /// Layer group to be inserted in the collection.
    ///
    /// \return
    /// Returns nothing.
    ///
    /// \exception MgIndexOutOfRangeException if the specified index is out of range.
    ///
    void Insert(INT32 index, MgLayerGroup* value);

    //////////////////////////////////////////////////////////
    /// \brief
    /// Removes all layer groups from the collection.
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
    /// Removes a layer group from the collection.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// bool Remove(MgLayerGroup value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// bool Remove(MgLayerGroup value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// bool Remove(MgLayerGroup value);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param value (MgLayerGroup)
    /// Layer group to be removed.
    ///
    /// \return
    /// Returns true if removal was successful.
    ///
    ///
    bool Remove(MgLayerGroup* value);

    //////////////////////////////////////////////////////////
    /// \brief
    /// Removes a layer group from the collection at the specified index.
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
    /// Index of the layer group to be removed.
    ///
    /// \return
    /// Returns nothing.
    ///
    /// \exception MgIndexOutOfRangeException if the specified index is out of range.
    ///
    void RemoveAt(INT32 index);

    //////////////////////////////////////////////////////////
    /// \brief
    /// Determines if the collection contains the specified layer group.
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
    /// Name of the layer group to search for.
    ///
    /// \return
    /// Returns true if the collection contains the specified item,
    /// or false otherwise.
    ///
    bool Contains(CREFSTRING name);

    //////////////////////////////////////////////////////////
    /// \brief
    /// Determines if the collection contains the specified layer group.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// bool Contains(MgLayerGroup value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// boolean Contains(MgLayerGroup value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// bool Contains(MgLayerGroup value);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param value (MgLayerGroup)
    /// MgLayerGroup to search for.
    ///
    /// \return
    /// Returns true if the collection contains the specified item,
    /// or false otherwise.
    ///
    bool Contains(MgLayerGroup* value);

    //////////////////////////////////////////////////////////
    /// \brief
    /// Gets the index of the specified layer group in the collection.
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
    /// Name of the layer group to search for.
    ///
    /// \return
    /// Returns the layer group's index if the collection contains the
    /// specified layer group, or false (-1) otherwise.
    ///
    INT32 IndexOf(CREFSTRING name);

    //////////////////////////////////////////////////////////
    /// \brief
    /// Gets the index of the specified layer group in the collection.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// int IndexOf(MgLayerGroup value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// int IndexOf(MgLayerGroup value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// int IndexOf(MgLayerGroup value);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param value (MgLayerBase)
    /// MgLayerGroup to search for.
    ///
    /// \return
    /// Returns the layer group's index if the collection contains the
    /// specified layer group, or false (-1) otherwise.
    ///
    INT32 IndexOf(MgLayerGroup* value);

INTERNAL_API:

    /// \brief
    /// Constructs an empty MgLayerGroupCollection object.
    ///
    MgLayerGroupCollection(MgMapBase* owner);

    /// \brief
    /// Gets the map owning this collection
    ///
    MgMapBase* GetMap();

protected:
    /// \brief
    /// To shut Ptr<> up
    ///
    MgLayerGroupCollection();

    /// \brief
    /// Destruct a MgLayerGroupCollection object
    ///
    /// \return
    /// Nothing
    ///
    virtual ~MgLayerGroupCollection();

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
    static const INT32 m_cls_id = PlatformBase_MapLayer_LayerGroupCollection;

private:
    MgNamedCollection*  m_groups;
    MgMapBase*              m_owner;
};
/// \}

#endif
