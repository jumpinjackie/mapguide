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

#ifndef DESKTOP_PRINTLAYOUT_H_
#define DESKTOP_PRINTLAYOUT_H_

#include <vector>

/// \cond INTERNAL

class MgLayerCollection;
class MgResourceService;
class MgResourceIdentifier;
class MgXmlUtil;

typedef std::vector<MgdCustomLogoInfo> MgdCustomLogoInfoVector;
typedef std::vector<MgdCustomTextInfo> MgdCustomTextInfoVector;

////////////////////////////////////////////////////////////
/// \brief
/// Specifies the layout format of the \link ePlot ePlot \endlink
/// sheet to be created by the Mapping Service GeneratePlot()
/// operations.
///
/// This class is internal only, and should not be exposed.
///
/// Future directions for this class (time permitting) are to
/// remove and/or rewrite the functionality of this class
/// in more appropriate utility class.
class MG_DESKTOP_API MgdPrintLayout : public MgSerializable
{
    MG_DECL_DYNCREATE()
    DECLARE_CLASSNAME(MgdPrintLayout)

INTERNAL_API:

    ///////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Construct an MgdPrintLayout object.
    ///
    /// \return
    /// Nothing
    ///
    MgdPrintLayout();

    ///////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Initializes a new MgdPrintLayout object given a resource service,
    /// and print layout definition.
    ///
    /// \param resourceService
    /// An MgResourceService that can be used to
    /// retrieve the print layout definition.
    /// \param layoutDefinition
    /// An MgResourceIdentifier that specifies the
    /// location of the print layout definition in a resource
    /// repository.
    ///
    /// \return
    /// Nothing
    ///
    void Create(MgResourceService* resourceService, MgResourceIdentifier* layoutDefinition);

    ///////////////////////////////////////////////////////
    /// \brief
    /// Gets the plot title.
    ///
    /// \return
    /// Returns the title for the plot.
    ///
    STRING GetPlotTitle();

    //////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the plot title.  This overrides the default plot title (the map title)
    /// with a user specified title.
    ///
    /// \param plotTitle
    /// The title for the plot.
    ///
    /// \return
    /// Returns nothing.
    ///
    void SetPlotTitle(CREFSTRING plotTitle);

    ///////////////////////////////////////////////////////
    /// \brief
    /// Gets the map scale to use for generating the layout plot.
    ///
    /// \return
    /// Returns the map scale for the plot.
    ///
    double GetPlotScale();

    ////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the map scale for the plot.  This will override the
    /// current map scale with the specified scale.
    ///
    /// \param dMapScale
    /// The map scale to use for the plot.
    ///
    /// \return
    /// Returns nothing.
    ///
    void SetPlotScale(double dMapScale);

    ///////////////////////////////////////////////////////
    /// \brief
    /// Gets the map center to use for generating the layout plot.
    ///
    /// \return
    /// Returns the map center for the plot.
    ///
    MgCoordinate* GetPlotCenter();

    ////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the map center for the plot.  This will override the
    /// current map center with the specified center.
    ///
    /// \param center
    /// The map center to use for the plot.
    ///
    /// \return
    /// Returns nothing.
    ///
    void SetPlotCenter(MgCoordinate* center);

    ////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the plot size.
    ///
    /// \return
    /// Returns the size of the plot as MgdPlotSpecification
    ///
    MgdPlotSpecification* GetPlotSize();

    ////////////////////////////////////////////////////////////////////
    /// \brief
    /// Set the plot size.  This will override the page size specified
    /// in the PrintLayout resource.
    ///
    /// \param plotSpec
    /// The plot size as a MgdPlotSpecification
    ///
    /// \return
    /// Returns nothing.
    ///
    void SetPlotSize(MgdPlotSpecification* plotSpec);

    ////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the system of measurement (US/English or metric units) for the scale bar.
    ///
    /// \return
    /// The system of measurement for scale bar.
    ///
    STRING GetScaleBarUnits();

    ////////////////////////////////////////////////////////////////////
    /// \brief
    /// Set the system of measurement (US/English or metric units) for the scale bar.
    ///
    /// \param units
    /// The system of measurement for scale bar
    ///
    /// \return
    /// Returns nothing.
    ///
    void SetScaleBarUnits(CREFSTRING units);

protected:

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

CLASS_ID:
    static const INT32 m_cls_id = MapGuide_Desktop_MappingService_PrintLayout;

INTERNAL_API:
    /// Layout element constants
    static const double LegendWidth;
    static const double LegendPadding;
    static const double HeaderHeight;
    static const double ScalebarHeight;
    static const double ScalebarPadding;
    static const double FooterHeight;
    static const double GroupIndent;

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Destroy a MgdPrintLayout object
    ///
    ~MgdPrintLayout();

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

    /// Accessors
    inline double& PageWidth() {return m_dPageWidth;}
    inline double& PageHeight() {return m_dPageHeight;}
    inline STRING& Units() {return m_units;}

    inline bool& ShowTitle() {return m_bShowTitle;}
    inline bool& ShowLegend() {return m_bShowLegend;}
    inline bool& ShowScalebar() {return m_bShowScalebar;}
    inline bool& ShowNorthArrow() {return m_bShowNorthArrow;}
    inline bool& ShowUrl() {return m_bShowUrl;}
    inline bool& ShowDateTime() {return m_bShowDateTime;}
    inline bool& ShowCustomLogos() {return m_bShowCustomLogos;}
    inline bool& ShowCustomText() {return m_bShowCustomText;}

    inline MgdCustomLogoInfoVector& CustomLogos() {return m_logos;}
    inline MgdCustomTextInfoVector& CustomText() {return m_text;}

    MgColor* GetBackgroundColor();

    void ComputeMapOffsetAndSize(double mapScale, MgEnvelope* mapBounds, double metersPerUnit,
                                 double& mapOffsetX, double& mapOffsetY, double& mapWidth, double& mapHeight, bool expandToFit = true);
    MgEnvelope* DetermineLayoutMapExtents(MgdMap* map, double metersPerUnit, double mapWidth, double mapHeight);

private:

    STRING m_plotTitle;
    Ptr<MgdPlotSpecification> m_plotSpec;
    STRING m_scaleBarUnits;
    double m_dPlotScale;
    Ptr<MgCoordinate> m_plotCenter;

    /// members to hold properties from PrintLayout resource document
    double m_dPageWidth;
    double m_dPageHeight;
    STRING m_units;
    Ptr<MgColor> m_bgColor;

    bool m_bShowTitle;
    bool m_bShowLegend;
    bool m_bShowScalebar;
    bool m_bShowNorthArrow;
    bool m_bShowUrl;
    bool m_bShowDateTime;
    bool m_bShowCustomLogos;
    bool m_bShowCustomText;

    MgdCustomLogoInfoVector m_logos;
    MgdCustomTextInfoVector m_text;

    void GetLayoutPropertiesFromXml(MgXmlUtil* pXmlUtil);
};

/// \endcond

#endif // MG_PRINT_LAYOUT_H


