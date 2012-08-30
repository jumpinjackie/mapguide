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

#ifndef DESKTOP_MAP_PLOT_COLLECTION_H_
#define DESKTOP_MAP_PLOT_COLLECTION_H_

class MgdMapPlotCollection;
template class MG_DESKTOP_API Ptr<MgdMapPlotCollection>;

class MgdMapPlot;
/// \ingroup Desktop_Misc_Module
/// \{
/////////////////////////////////////////////////////////////////
/// \brief
/// Manipulates collections of MgdMapPlot objects.
class MG_DESKTOP_API MgdMapPlotCollection : public MgSerializable
{
    MG_DECL_DYNCREATE()
    DECLARE_CLASSNAME(MgdMapPlotCollection)

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
    /// MgdMapPlot GetItem(int index);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgdMapPlot GetItem(int index);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgdMapPlot GetItem(int index);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param index (int)
    /// Index of the map to get.
    ///
    /// \return
    /// Returns the specified MgdMapPlot object.
    ///
    /// \exception MgOutOfRangeException if the index is invalid.
    ///
    MgdMapPlot* GetItem(INT32 index);

    //////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the MapPlot with the specified name.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgdMapPlot GetItem(string name);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgdMapPlot GetItem(String name);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgdMapPlot GetItem(string name);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param name (String/string)
    /// Name of the MapPlot to get.
    ///
    /// \return
    /// Returns the specified MgdMapPlot object.
    ///
    /// \exception MgInvalidArgumentException if the name does not exist in
    ///  the collection.
    ///
    MgdMapPlot* GetItem(CREFSTRING name);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the MapPlot in the collection at the specified index to a
    /// specified value.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void SetItem(int index, MgdMapPlot value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void SetItem(int index, MgdMapPlot value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void SetItem(int index, MgdMapPlot value);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param index (int)
    /// Position in the collection to set the
    /// MgMapPot.
    /// \param value (MgdMapPlot)
    /// MgdMapPlot to be set in the collection.
    ///
    /// \return
    /// Returns nothing.
    ///
    /// \exception MgOutOfRangeException if the index is out of range.
    ///
    void SetItem(INT32 index, MgdMapPlot* value);

    ////////////////////////////////////////////////////////////////////
    /// \brief
    /// Adds the specified MapPlot to the end of the collection.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void Add(MgdMapPlot value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void Add(MgdMapPlot value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void Add(MgdMapPlot value);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param value (MgdMapPlot)
    /// MgdMapPlot to be added to the collection.
    ///
    /// \return
    /// Returns nothing.
    ///
    void Add(MgdMapPlot* value);

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
    /// void Insert(int index, MgdMapPlot value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void Insert(int index, MgdMapPlot value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void Insert(int index, MgdMapPlot value);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param index (int)
    /// Position in the collection to insert the
    /// MgdMapPlot.
    /// \param value (MgdMapPlot)
    /// MgdMapPlot to be inserted in the collection.
    ///
    /// \return
    /// Returns nothing.
    ///
    /// \exception MgOutOfRangeException if the specified index is out of
    ///  range.
    ///
    void Insert(INT32 index, MgdMapPlot* value);

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
    /// bool Remove(MgdMapPlot value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// bool Remove(MgdMapPlot value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// bool Remove(MgdMapPlot value);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param value (MgdMapPlot)
    /// MgdMapPlot to be removed
    ///
    /// \return
    /// Returns true if removal was successful.
    ///
    ///
    bool Remove(MgdMapPlot* value);

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
    /// bool Contains(MgdMapPlot value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// boolean Contains(MgdMapPlot value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// bool Contains(MgdMapPlot value);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param value (MgdMapPlot)
    /// The MgdMapPlot to look for.
    ///
    /// \return
    /// Returns true if the collection contains the specified MapPlot,
    /// or false otherwise.
    ///
    bool Contains(MgdMapPlot* value);

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
    /// int IndexOf(MgdMapPlot value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// int IndexOf(MgdMapPlot value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// int IndexOf(MgdMapPlot value);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param value (MgdMapPlot)
    /// Value of the item to get the index of.
    ///
    /// \return
    /// Returns the MapPlot's index if the collection contains the
    /// specified MapPlot, or false (-1) otherwise.
    ///
    INT32 IndexOf(MgdMapPlot* value);

    /// \brief
    /// Constructs an empty MgdMapPlotCollection object.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgdMapPlotCollection();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgdMapPlotCollection();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgdMapPlotCollection();
    /// \htmlinclude SyntaxBottom.html
    ///
    MgdMapPlotCollection();

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
    /// Destruct a MgdMapPlotCollection object
    ///
    /// \return
    /// Nothing
    ///
    virtual ~MgdMapPlotCollection();

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
    static const INT32 m_cls_id = MapGuide_Desktop_MappingService_MapPlotCollection;

private:
    MgNamedCollection*   m_mapPlots;
};
/// \}

#endif
