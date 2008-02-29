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

/// \defgroup MgMapPlotCollection MgMapPlotCollection
/// \ingroup Mapping_Service_Module
/// \{

#ifndef MG_MAP_PLOT_COLLECTION_H_
#define MG_MAP_PLOT_COLLECTION_H_

class MgMapPlotCollection;
template class MG_MAPGUIDE_API Ptr<MgMapPlotCollection>;

class MgMapPlot;

/////////////////////////////////////////////////////////////////
/// \brief
/// Manipulates collections of MgMapPlot objects.
class MG_MAPGUIDE_API MgMapPlotCollection : public MgSerializable
{
    MG_DECL_DYNCREATE()
    DECLARE_CLASSNAME(MgMapPlotCollection)

PUBLISHED_API:
    ///////////////////////////////////////////////
    /// \brief
    /// Gets the number of MapPlots in the collection.
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
    /// Returns the number of MapPlots.
    ///
    INT32 GetCount();

    ////////////////////////////////////////////////////////
    /// \brief
    /// Gets the MapPlot at the specified index.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgMapPlot GetItem(int index);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgMapPlot GetItem(int index);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgMapPlot GetItem(int index);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param index (int)
    /// Index of the map to get.
    ///
    /// \return
    /// Returns the specified MgMapPlot object.
    ///
    /// \exception MgOutOfRangeException if the index is invalid.
    ///
    MgMapPlot* GetItem(INT32 index);

    //////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the MapPlot with the specified name.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgMapPlot GetItem(string name);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgMapPlot GetItem(String name);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgMapPlot GetItem(string name);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param name (String/string)
    /// Name of the MapPlot to get.
    ///
    /// \return
    /// Returns the specified MgMapPlot object.
    ///
    /// \exception MgInvalidArgumentException if the name does not exist in
    ///  the collection.
    ///
    MgMapPlot* GetItem(CREFSTRING name);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the MapPlot in the collection at the specified index to a
    /// specified value.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void SetItem(int index, MgMapPlot value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void SetItem(int index, MgMapPlot value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void SetItem(int index, MgMapPlot value);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param index (int)
    /// Position in the collection to set the
    /// MgMapPot.
    /// \param value (MgMapPlot)
    /// MgMapPlot to be set in the collection.
    ///
    /// \return
    /// Returns nothing.
    ///
    /// \exception MgOutOfRangeException if the index is out of range.
    ///
    void SetItem(INT32 index, MgMapPlot* value);

    ////////////////////////////////////////////////////////////////////
    /// \brief
    /// Adds the specified MapPlot to the end of the collection.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void Add(MgMapPlot value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void Add(MgMapPlot value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void Add(MgMapPlot value);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param value (MgMapPlot)
    /// MgMapPlot to be added to the collection.
    ///
    /// \return
    /// Returns nothing.
    ///
    void Add(MgMapPlot* value);

    ///////////////////////////////////////////////////////////////////////
    /// \brief
    /// Inserts a MapPlot at a specified index within the collection.
    ///
    /// \remarks
    /// MapPlots following the insertion point are moved down to
    /// accommodate the new MapPlot.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void Insert(int index, MgMapPlot value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void Insert(int index, MgMapPlot value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void Insert(int index, MgMapPlot value);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param index (int)
    /// Position in the collection to insert the
    /// MgMapPlot.
    /// \param value (MgMapPlot)
    /// MgMapPlot to be inserted in the collection.
    ///
    /// \return
    /// Returns nothing.
    ///
    /// \exception MgOutOfRangeException if the specified index is out of
    ///  range.
    ///
    void Insert(INT32 index, MgMapPlot* value);

    /////////////////////////////////////////
    /// \brief
    /// Removes all MapPlots from the collection.
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
    /// Removes a MapPlot from the collection.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// bool Remove(MgMapPlot value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// bool Remove(MgMapPlot value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// bool Remove(MgMapPlot value);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param value (MgMapPlot)
    /// MgMapPlot to be removed
    ///
    /// \return
    /// Returns true if removal was successful.
    ///
    ///
    bool Remove(MgMapPlot* value);

    ///////////////////////////////////////////////////////////////
    /// \brief
    /// Removes a MapPlot from the collection at the specified index.
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
    /// \exception MgOutOfRangeException if the MapPlot does not exist in the
    ///  collection.
    ///
    void RemoveAt(INT32 index);

    ///////////////////////////////////////////////////////////////
    /// \brief
    /// Determines if the collection contains the specified MapPlot.
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
    /// Name of the MapPlot.
    ///
    /// \return
    /// Returns true if the collection contains the specified map,
    /// or false otherwise.
    ///
    bool Contains(CREFSTRING name);

    ///////////////////////////////////////////////////////////////
    /// \brief
    /// Determines if the collection contains the specified MapPlot.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// bool Contains(MgMapPlot value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// boolean Contains(MgMapPlot value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// bool Contains(MgMapPlot value);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param value (MgMapPlot)
    /// The MgMapPlot to look for.
    ///
    /// \return
    /// Returns true if the collection contains the specified MapPlot,
    /// or false otherwise.
    ///
    bool Contains(MgMapPlot* value);

    ///////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the index of the specified MapPlot in the collection.
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
    /// Name of the MapPlot to get the index of.
    ///
    /// \return
    /// Returns the MapPlot's index if the collection contains the
    /// specified MapPlot, or false (-1) otherwise.
    ///
    INT32 IndexOf(CREFSTRING name);

    //////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the index of the specified MapPlot in the collection.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// int IndexOf(MgMapPlot value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// int IndexOf(MgMapPlot value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// int IndexOf(MgMapPlot value);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param value (MgMapPlot)
    /// Value of the item to get the index of.
    ///
    /// \return
    /// Returns the MapPlot's index if the collection contains the
    /// specified MapPlot, or false (-1) otherwise.
    ///
    INT32 IndexOf(MgMapPlot* value);

    /// \brief
    /// Constructs an empty MgMapPlotCollection object.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgMapPlotCollection();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgMapPlotCollection();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgMapPlotCollection();
    /// \htmlinclude SyntaxBottom.html
    ///
    MgMapPlotCollection();

INTERNAL_API:
    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Serialize data to a stream
    ///
    /// \param stream
    /// Stream
    ///
    virtual void Serialize(MgStream* stream);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Deserialize data from a stream
    ///
    /// \param stream
    /// Stream
    ///
    virtual void Deserialize(MgStream* stream);

protected:
    /// \brief
    /// Destruct a MgMapPlotCollection object
    ///
    /// \return
    /// Nothing
    ///
    virtual ~MgMapPlotCollection();

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
    static const INT32 m_cls_id = MapGuide_MappingService_MapPlotCollection;

private:
    MgNamedCollection*   m_mapPlots;
};
/// \}

#endif
