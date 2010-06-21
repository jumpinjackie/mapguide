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

/// \defgroup MgReadOnlyLayerCollection MgReadOnlyLayerCollection
/// \ingroup Collections_Module
/// \{

#ifndef _MG_READ_ONLY_LAYER_COLLECTION_H_
#define _MG_READ_ONLY_LAYER_COLLECTION_H_

class MgReadOnlyLayerCollection;
template class MG_PLATFORMBASE_API Ptr<MgReadOnlyLayerCollection>;

/////////////////////////////////////////////////////////////////
/// \brief
/// Provides read only access to a collection of MgLayerBase objects.
///
/// \todo
///   * [[TO DO: add samples for this section.]]
///
class MG_PLATFORMBASE_API MgReadOnlyLayerCollection : public MgGuardDisposable
{
    DECLARE_CLASSNAME(MgReadOnlyLayerCollection)

PUBLISHED_API:
    ////////////////////////////////////////////////
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
    /// \exception MgOutOfRangeException if the index is invalid.
    ///
    MgLayerBase* GetItem(INT32 index);

    ////////////////////////////////////////////////////////////////
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

    /////////////////////////////////////////////////////////////
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
    /////////////////////////////////////////////////////////////////
    /// \brief
    /// To shut Ptr<> up
    ///
    MgReadOnlyLayerCollection();

    //////////////////////////////////////////////////
    /// \brief
    /// Adds a layer to the end of the collection.
    ///
    ///
    void Add(MgLayerBase* value);

protected:
    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Destruct a MgLayerCollection object
    ///
    /// \return
    /// Nothing
    ///
    virtual ~MgReadOnlyLayerCollection();

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Get the unique identifier for the class
    ///
    /// \return
    /// Class Identifider.
    ///
    virtual INT32 GetClassId() { return m_cls_id; }

    /////////////////////////////////////////////////////////////////
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
    static const INT32 m_cls_id = PlatformBase_MapLayer_ReadOnlyLayerCollection;

private:
    Ptr<MgDisposableCollection> m_layers;
};
/// \}

#endif
