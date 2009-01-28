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

/// \defgroup MgLayerCollection MgLayerCollection
/// \ingroup Maps_and_Layers_Module
/// \{

#ifndef _MG_LAYER_COLLECTION_H_
#define _MG_LAYER_COLLECTION_H_

class MgMapBase;
class MgLayerCollection;
template class MG_PLATFORMBASE_API Ptr<MgLayerCollection>;

/////////////////////////////////////////////////////////////////
/// \brief
/// Manipulates collections of MgLayerBase objects.
///
/// \remarks
/// A layer's draw priority is controlled by the order of the
/// layers in this collection. To change a layer's draw priority,
/// remove the layer from the collection and insert it at the
/// desired position.
///
class MG_PLATFORMBASE_API MgLayerCollection : public MgGuardDisposable
{
    DECLARE_CLASSNAME(MgLayerCollection)

PUBLISHED_API:
    //////////////////////////////////////////////////////////
    /// \brief
    /// Gets the number of layers in the collection.
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
    /// Returns the number of layers.
    ///
    INT32 GetCount();

    //////////////////////////////////////////////////////////
    /// \brief
    /// Gets the layer at the specified index.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgLayerBase GetItem(int index);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgLayerBase GetItem(int index);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgLayerBase GetItem(int index);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param index (int)
    /// Index of the layer to get.
    ///
    /// \return
    /// Returns the layer.
    ///
    /// \exception MgIndexOutOfRangeException if the specified index is out of range.
    ///
    MgLayerBase* GetItem(INT32 index);

    //////////////////////////////////////////////////////////
    /// \brief
    /// Returns the layer with the specified name.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgLayerBase GetItem(string name);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgLayerBase GetItem(String name);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgLayerBase GetItem(string name);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param name (String/string)
    /// Name of the layer to get.
    ///
    /// \return
    /// Returns the layer.
    ///
    /// \exception MgObjectNotFoundException if the name does not exist.
    ///
    MgLayerBase* GetItem(CREFSTRING name);

    //////////////////////////////////////////////////////////
    /// \brief
    /// Sets the layer in the collection at the specified index to
    /// the specified value.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void SetItem(int index, MgLayerBase value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void SetItem(int index, MgLayerBase value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void SetItem(int index, MgLayerBase value);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param index (int)
    /// Position in the collection to set the layer.
    /// \param value (MgLayerBase)
    /// MgLayerBase to be set in the collection.
    ///
    /// \return
    /// Returns nothing.
    ///
    /// \exception MgIndexOutOfRangeException if the specified index is out of range.
    ///
    void SetItem(INT32 index, MgLayerBase* value);

    //////////////////////////////////////////////////////////
    /// \brief
    /// Adds a layer to the end of the collection.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void Add(MgLayerBase value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void Add(MgLayerBase value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void Add(MgLayerBase value);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param value (MgLayerBase)
    /// Layer to be added.
    ///
    /// \return
    /// Returns nothing.
    ///
    void Add(MgLayerBase* value);

    //////////////////////////////////////////////////////////
    /// \brief
    /// Inserts a layer at a specified location in the collection.
    ///
    /// \remarks
    /// Layers following the insertion point are moved down to
    /// accommodate the new layer.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void Insert(int index, MgLayerBase value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void Insert(int index, MgLayerBase value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void Insert(int index, MgLayerBase value);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param index (int)
    /// Position in the collection to insert the layer.
    /// \param value (MgLayerBase)
    /// Layer to be inserted in the collection.
    ///
    /// \return
    /// Returns nothing.
    ///
    /// \exception MgIndexOutOfRangeException if the specified index is out of range.
    ///
    void Insert(INT32 index, MgLayerBase* value);

    //////////////////////////////////////////////////////////
    /// \brief
    /// Removes all layers from the collection.
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
    /// Removes a layer from the collection.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// bool Remove(MgLayerBase value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// bool Remove(MgLayerBase value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// bool Remove(MgLayerBase value);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param value (MgLayerBase)
    /// Layer to be removed.
    ///
    /// \return
    /// Returns true if removal was successful.
    ///
    ///
    bool Remove(MgLayerBase* value);

    //////////////////////////////////////////////////////////
    /// \brief
    /// Removes a layer from the collection at the specified index.
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
    /// Index of the layer to be removed.
    ///
    /// \return
    /// Returns nothing.
    ///
    /// \exception MgIndexOutOfRangeException if the specified index is out of range.
    ///
    void RemoveAt(INT32 index);

    //////////////////////////////////////////////////////////
    /// \brief
    /// Determines if the collection contains the specified layer.
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
    /// Name of the layer to search for.
    ///
    /// \return
    /// Returns true if the collection contains the specified item,
    /// or false otherwise.
    ///
    bool Contains(CREFSTRING name);

    //////////////////////////////////////////////////////////
    /// \brief
    /// Determines if the collection contains the specified layer.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// bool Contains(MgLayerBase value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// boolean Contains(MgLayerBase value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// bool Contains(MgLayerBase value);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param value (MgLayerBase)
    /// MgLayerBase to search for.
    ///
    /// \return
    /// Returns true if the collection contains the specified item,
    /// or false otherwise.
    ///
    bool Contains(MgLayerBase* value);

    //////////////////////////////////////////////////////////
    /// \brief
    /// Gets the index of the specified layer in the collection.
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
    /// Name of the layer to search for.
    ///
    /// \return
    /// Returns the layer's index if the collection contains the
    /// specified layer, or false (-1) otherwise.
    ///
    INT32 IndexOf(CREFSTRING name);

    //////////////////////////////////////////////////////////
    /// \brief
    /// Gets the index of the specified layer in the collection.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// int IndexOf(MgLayerBase value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// int IndexOf(MgLayerBase value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// int IndexOf(MgLayerBase value);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param value (MgLayerBase)
    /// MgLayerBase to search for.
    ///
    /// \return
    /// Returns the layer's index if the collection contains the
    /// specified layer, or false (-1) otherwise.
    ///
    INT32 IndexOf(MgLayerBase* value);

INTERNAL_API:

    /// \brief
    /// Constructs an empty MgLayerCollection object.
    ///
    MgLayerCollection(MgMapBase* owner);

    /// \brief
    /// Gets the map owning this collection
    ///
    MgMapBase* GetMap();

    /// \brief
    /// Enable/disable duplicates check.  Used during Deserialization
    void SetCheckForDuplicates(bool bDup);

protected:
    /// \brief
    /// To shut Ptr<> up
    ///
    MgLayerCollection();

    /// \brief
    /// Destruct a MgLayerCollection object
    ///
    /// \return
    /// Nothing
    ///
    virtual ~MgLayerCollection();

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
    static const INT32 m_cls_id = PlatformBase_MapLayer_LayerCollection;

private:
    MgNamedCollection*  m_layers;
    MgMapBase*              m_owner;
};
/// \}

#endif
