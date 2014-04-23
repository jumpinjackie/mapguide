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

#ifndef DESKTOP_MAP_PLOT_H_
#define DESKTOP_MAP_PLOT_H_

class MgdMap;
class MgdLayer;
class MgdPlotSpecification;
class MgdLayout;
/// \ingroup Desktop_Misc_Module
/// \{
/////////////////////////////////////////////////////////////////
/// \brief
/// The MgdMapPlot class allows the developer to specify the map,
/// the view of the map, the plot specification, and the layout
/// for one sheet within a call to GenerateMultiPlot.
class MG_DESKTOP_API MgdMapPlot : public MgNamedSerializable
{
    DECLARE_CLASSNAME(MgdMapPlot)

PUBLISHED_API:
    ///////////////////////////////////////////////
    /// \brief
    /// Create and initialize an MgdMapPlot instance using
    /// the map center and scale. Sets the MapPlotInstruction value
    /// appropriately.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgdMapPlot(MgdMap map, MgdPlotSpecification plotSpec, MgdLayout layout);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgdMapPlot(MgdMap map, MgdPlotSpecification plotSpec, MgdLayout layout);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgdMapPlot(MgdMap map, MgdPlotSpecification plotSpec, MgdLayout layout);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Nothing.
    ///
    MgdMapPlot(
        MgdMap* map,
        MgdPlotSpecification* plotSpec,
        MgdLayout* layout);

    ///////////////////////////////////////////////
    /// \brief
    /// Create and initialize an MgdMapPlot instance using
    /// the specified center and scale.  Sets the MapPlotInstruction value
    /// appropriately.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgdMapPlot(MgdMap map, MgCoordinate center, double scale, MgdPlotSpecification plotSpec, MgdLayout layout);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgdMapPlot(MgdMap map, MgCoordinate center, double scale, MgdPlotSpecification plotSpec, MgdLayout layout);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgdMapPlot(MgdMap map, MgCoordinate center, double scale, MgdPlotSpecification plotSpec, MgdLayout layout);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Nothing.
    ///
    MgdMapPlot(
        MgdMap* map,
        MgCoordinate* center,
        double scale,
        MgdPlotSpecification* plotSpec,
        MgdLayout* layout);

    ///////////////////////////////////////////////
    /// \brief
    /// Create and initialize an MgdMapPlot instance using
    /// the specified extents  Sets the MapPlotInstruction value
    /// appropriately.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgdMapPlot(MgdMap map, MgEnvelope extent, bool expandToFit, MgdPlotSpecification plotSpec, MgdLayout layout);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgdMapPlot(MgdMap map, MgEnvelope extent, boolean expandToFit, MgdPlotSpecification plotSpec, MgdLayout layout);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgdMapPlot(MgdMap map, MgEnvelope extent, bool expandToFit, MgdPlotSpecification plotSpec, MgdLayout layout);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Nothing.
    ///
    MgdMapPlot(
        MgdMap* map,
        MgEnvelope* extent,
        bool expandToFit,
        MgdPlotSpecification* plotSpec,
        MgdLayout* layout);

    ///////////////////////////////////////////////
    /// \brief
    /// Get the MgdMap object to plot.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgdMap GetMap();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgdMap GetMap();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgdMap GetMap();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// The MgdMap object.
    ///
    MgdMap* GetMap();

    ///////////////////////////////////////////////
    /// \brief
    /// Set the MgdMap object to plot.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void SetMap(MgdMap map);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void SetMap(MgdMap map);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void SetMap(MgdMap map);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param map (MgdMap)
    /// The MgdMap object to plot.
    ///
    /// \return
    /// Nothing
    ///
    void SetMap(MgdMap* map);

    ///////////////////////////////////////////////
    /// \brief
    /// Get the MapPlotInstruction, which defines how the view of the map
    /// to be plotted is defined.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// int GetMapPlotInstruction();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// int GetMapPlotInstruction();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// int GetMapPlotInstruction();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// The MapPlotInstruction value.
    ///
    INT32 GetMapPlotInstruction();

    ///////////////////////////////////////////////
    /// \brief
    /// Set the MapPlotInstruction, which defines how the view of the map
    /// to be plotted is defined.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void SetMapPlotInstruction(int plotInstruction);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void SetMapPlotInstruction(int plotInstruction);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void SetMapPlotInstruction(int plotInstruction);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param plotInstruction (int)
    /// The enumeration value defining
    /// how the view of the map is defined.
    ///
    /// \return
    /// Nothing
    ///
    void SetMapPlotInstruction(INT32 plotInstruction);

    ///////////////////////////////////////////////
    /// \brief
    /// Get view center
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgCoordinate GetCenter();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgCoordinate GetCenter();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgCoordinate GetCenter();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// The coordinates of the center of the view of map
    ///
    MgCoordinate* GetCenter();

    ///////////////////////////////////////////////
    /// \brief
    /// Get view scale
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// double GetScale();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// double GetScale();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// double GetScale();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// The scale of the view of map
    ///
    double GetScale();

    ///////////////////////////////////////////////
    /// \brief
    /// Set the view center and scale of the map.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void SetCenterAndScale(MgCoordinate center, double scale);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void SetCenterAndScale(MgCoordinate center, double scale);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void SetCenterAndScale(MgCoordinate center, double scale);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param center (MgCoordinate)
    /// The coordinates of the center of the view of the map.
    /// \param scale (double)
    /// The scale of the view of the map.
    ///
    /// \return
    /// Nothing
    ///
    void SetCenterAndScale(MgCoordinate* center, double scale);

    ///////////////////////////////////////////////
    /// \brief
    /// Get extents of the view of the map.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgEnvelope GetExtent();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgEnvelope GetExtent();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgEnvelope GetExtent();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// The extents of the view of the map.
    ///
    MgEnvelope* GetExtent();

    ///////////////////////////////////////////////
    /// \brief
    /// Gets flag indicating whether extents of the view of the map
    /// should be expanded to fill the space within the page margins.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// bool GetExpandToFit();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// boolean GetExpandToFit();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// bool GetExpandToFit();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// The expand to fit flag.
    ///
    bool GetExpandToFit();

    ///////////////////////////////////////////////
    /// \brief
    /// Set the extents of the map.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void SetExtent(MgEnvelope extent, bool expandToFit);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void SetExtent(MgEnvelope extent, boolean expandToFit);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void SetExtent(MgEnvelope extent, bool expandToFit);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param extent (MgEnvelope)
    /// The extents of the map.
    /// \param expandToFit (boolean/bool)
    /// Flag indicating whether the extents should be
    /// expanded to fill the space within the page margins.
    ///
    /// \return
    /// Nothing
    ///
    void SetExtent(MgEnvelope* extent, bool expandToFit);

    ///////////////////////////////////////////////
    /// \brief
    /// Gets the plot specification.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgdPlotSpecification GetPlotSpecification();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgdPlotSpecification GetPlotSpecification();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgdPlotSpecification GetPlotSpecification();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// The MgdPlotSpecification.
    ///
    MgdPlotSpecification* GetPlotSpecification();

    ///////////////////////////////////////////////
    /// \brief
    /// Set the plot specification.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void SetPlotSpecification(MgdPlotSpecification plotSpec);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void SetPlotSpecification(MgdPlotSpecification plotSpec);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void SetPlotSpecification(MgdPlotSpecification plotSpec);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param plotSpec (MgdPlotSpecification)
    /// The plot specification.
    ///
    /// \return
    /// Nothing
    ///
    void SetPlotSpecification(MgdPlotSpecification* plotSpec);

    ///////////////////////////////////////////////
    /// \brief
    /// Get the layout.  This value is optional; if null the map will fill
    /// the entire page area within the margins.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgdLayout GetLayout();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgdLayout GetLayout();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgdLayout GetLayout();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// The MgdLayout.
    ///
    MgdLayout* GetLayout();

    ///////////////////////////////////////////////
    /// \brief
    /// Set the layout.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void SetLayout(MgdLayout layout);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void SetLayout(MgdLayout layout);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void SetLayout(MgdLayout layout);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param layout (MgdLayout)
    /// The object defining the layout for the plot.
    ///
    /// \return
    /// Nothing
    ///
    void SetLayout(MgdLayout* layout);

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

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Get the name of object
    ///
    /// \return
    /// Returns the name of object
    ///
    STRING GetName();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Does it allow to set the name
    ///
    /// \return
    /// true - allows to set the name
    /// false - can not set the name
    ///
    bool CanSetName();

    //////////////////////////////////////////////////////////
    /// \brief
    /// Construct and empty MgdMapPlot object
    ///
    MgdMapPlot();

    //////////////////////////////////////////////////////////
    /// \brief
    /// Destruct a MgdMapPlot object
    ///
    /// \return
    /// Nothing
    ///
    virtual ~MgdMapPlot();

protected:

    //////////////////////////////////////////////////////////
    /// \brief
    /// Get the unique identifier for the class
    ///
    /// \return
    /// Class Identifider.
    ///
    virtual INT32 GetClassId() { return m_cls_id; }

    //////////////////////////////////////////////////////////
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
    static const INT32 m_cls_id = MapGuide_Desktop_MappingService_MapPlot;

private:

    /// \brief
    /// Initalize class members which may not be set by the constructors
    ///
    void Initialize();

    MgdMap* m_map;
    MgdPlotSpecification* m_plotSpec;
    MgdLayout* m_layout;

    MgCoordinate* m_center;
    double m_scale;

    MgEnvelope* m_extent;

    INT32 m_plotInstruction;

    bool m_bExpandToFit;
};
/// \}

#endif
