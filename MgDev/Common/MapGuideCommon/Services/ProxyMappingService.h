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

#ifndef _MG_PROXY_MAPPING_SERVICE_H_
#define _MG_PROXY_MAPPING_SERVICE_H_

/// \cond INTERNAL
////////////////////////////////////////////////////////////////////////////////////////
/// \brief
/// This class contains methods to allow MapGuide users to return feature data from
/// MapGuide.
class MG_MAPGUIDE_API MgProxyMappingService : public MgMappingService
{
    DECLARE_CLASSNAME(MgProxyMappingService)

EXTERNAL_API:
    ///////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Plot the map to a EPlot DWF using the center and scale from the map.  The
    /// extents will be computed to fill the space wihing the page margins.  If the
    /// layout parameter is null, no adonments will be added to the page.
    ///
    /// \param map
    /// Map object describing current state of map
    /// \param plotSpec
    /// MgPlotSpecification for generated plot describing page size and margins.
    /// \param layout
    /// MgLayout object describing layout elements to add to the plot.
    /// \param dwfVersion
    /// DWF specification supported/required by the client.  The DWF spec determines the
    /// schema and file versions used to generate the DWFs sent back to the client.
    ///
    /// \return
    /// An EPlot DWF.
    ///
    /// \exception To be documented
    ///
    virtual MgByteReader* GeneratePlot(
        MgMap* map,
        MgPlotSpecification* plotSpec,
        MgLayout* layout,
        MgDwfVersion* dwfVersion);

    ///////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Plot the map to an EPlot DWF using the specified center and scale.  The extents
    /// will be computed to fill the space within the page margins.  If the layout
    /// parameter is null, no adonments will be added to the page.
    ///
    /// \param map
    /// Map object describing current state of map
    /// \param center
    /// MgCoordinate object describing center of the map in the generated plot.
    /// \param scale
    /// The scale to use in  the generated plot.
    /// \param plotSpec
    /// MgPlotSpecification for generated plot describing page size and margins.
    /// \param layout
    /// MgLayout object describing layout elements to add to the plot.
    /// \param dwfVersion
    /// DWF specification supported/required by the client.  The DWF spec determines the
    /// schema and file versions used to generate the DWFs sent back to the client.
    ///
    /// \return
    /// An EPlot DWF.
    ///
    /// \exception To be documented
    ///
    virtual MgByteReader* GeneratePlot(
        MgMap* map,
        MgCoordinate* center,
        double scale,
        MgPlotSpecification* plotSpec,
        MgLayout* layout,
        MgDwfVersion* dwfVersion);

    ///////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Plot the map to an EPlot DWF using the extent specified. in the mapping
    /// coordinate space.  The center and scale will be computed based on the extent
    /// specified.  If expandToFit is true, then the extent specified will be expanded
    /// in the X or Y direction to fill the space within the page margins.  If the
    /// layout parameter is null, no adornments will be added to the page.
    ///
    /// \param map
    /// Map object describing current state of map
    /// \param extents
    /// MgEnvelope object describing extents of the map in the generated plot.
    /// \param expandToFit
    /// The flag to indicate whether the extents are expanded to fill the  space
    /// within the page margins.
    /// \param plotSpec
    /// MgPlotSpecification for generated plot describing page size and margins.
    /// \param layout
    /// MgLayout object describing layout elements to add to the plot.
    /// \param dwfVersion
    /// DWF specification supported/required by the client.  The DWF spec determines the
    /// schema and file versions used to generate the DWFs sent back to the client.
    ///
    /// \return
    /// An EPlot DWF.
    ///
    /// \exception To be documented
    ///
    virtual MgByteReader* GeneratePlot(
        MgMap* map,
        MgEnvelope* extents,
        bool expandToFit,
        MgPlotSpecification* plotSpec,
        MgLayout* layout,
        MgDwfVersion* dwfVersion);

    ///////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Plot multiple maps into an EPlot DWF as separate sheets.  The parameters allow
    /// the caller to control the map, the view, the size, orientation, and margins of
    /// each page, and the layout of each page.
    ///
    /// \param mapPlots
    /// A collection of MapPlot objects.
    /// \param dwfVersion
    /// DWF specification supported/required by the client.  The DWF spec determines the
    /// schema and file versions used to generate the DWFs sent back to the client.
    ///
    /// \return
    /// An EPlot DWF with multiple sheets, one for each map.
    ///
    /// \exception To be documented
    ///
    virtual MgByteReader* GenerateMultiPlot(
        MgMapPlotCollection* mapPlots,
        MgDwfVersion* dwfVersion);

    /// \brief
    /// Queries and returns each of the visible features on the specified layer of the
    /// specified map.  For feature layers the properties to be included are defined
    /// in the Feature Layer definition. For DWF layers the properties of each of the objects
    /// in the DWF source for the layer should be included as the feature properties. The
    /// geometry of each feature can optionally be returned in either the coordinate space
    /// of the map or the coordinate space of the display. This operation returns the feature
    /// properties in XML format, which can be used to construct an HTML Image Map for a previously
    /// rendered view of the map.
    ///
    /// \remarks
    /// This API does not support WMS GetFeatureInfo
    ///
    /// \param map
    /// Input
    /// map object containing current viewed state of map.
    /// \param layerName
    /// Input
    /// name of the layer to query properties for
    /// \param coordinateSpace
    /// Input
    /// specifies the coordinate system the geometry should be returned in.  See MgCoordinateSpace
    ///
    /// \return
    /// A byte reader containing an XML document with the feature information
    ///
    virtual MgByteReader* QueryFeatures(MgMap* map, CREFSTRING layerName, CREFSTRING coordinateSpace);

    /// \brief
    /// Queries feature information for each feature visible at the specified point of interest, on a given set of layers.
    /// This is a multi-layer version of QueryFeatures.
    ///
    /// \remarks
    /// This API is used to support WMS GetFeatureInfo.
    ///
    /// \param map
    /// Map object containing current view state of the map.
    /// \param layerNames
    /// List of layers to return information from.  Each layer must be in the specified MgMap object
    /// \param x
    /// X coordinate for point of interest
    /// \param y
    /// Y coordinate for point of interest
    /// \param maxFeatures
    /// Maximum number of feature that should be returned for a given layer
    /// greater than 0, usually 1
    /// \param coordinateSpace
    /// Input
    /// specifies the coordinate system the geometry should be returned in.  See MgCoordinateSpace
    ///
    /// \return
    /// A byte reader containing an XML document with the feature information
    ///
    virtual MgByteReader* QueryFeatures(MgMap* map, MgStringCollection* layerNames, INT32 x, INT32 y, INT32 maxFeatures, CREFSTRING coordinateSpace);

    /// \brief
    /// GenerateLegendImage() returns the legend image for the specified layer.
    ///
    /// \param resource
    /// Input
    /// MgResourceIdentifier object identifying the layer definition resource.
    /// \param scale
    /// Input
    /// The scale at which the symbolization is requested.
    /// \param width
    /// Input
    /// The requested image width in pixels.
    /// \param height
    /// Input
    /// The requested image height in pixels.
    /// \param format
    /// Input
    /// Image format, from MgImageFormats. Example: PNG, JPG, PNG8, etc …
    /// \param geomType
    /// Input
    /// The type of symbolization required: 1=Point, 2=Line, 3=Area, 4=Composite
    /// \param themeCategory
    /// Input
    /// The value indicating which theme category swatch to return.
    /// Used when there is a theme defined at this scale. An exception will be
    /// thrown if a requested them category doesn't exist.
    ///
    /// \return
    /// Returns a stream representing the legend image.
    /// The default returned image format will be PNG8 unless a different supported
    /// format is requested. An exception will be thrown if an unsupported image
    /// format is requested.
    ///
    /// \exception MgArgumentOutOfRangeException
    /// \exception MgInvalidResourceTypeException
    /// \exception MgNullArgumentException
    /// \exception MgInvalidImageFormatException
    ///
    virtual MgByteReader* GenerateLegendImage(MgResourceIdentifier* resource,
                                              double scale,
                                              INT32 width,
                                              INT32 height,
                                              CREFSTRING format,
                                              INT32 geomType,
                                              INT32 themeCategory);

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Creates a new runtime map from the specified Map Definition resource id and returns an XML-based description of the runtime
    /// map
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgByteReader CreateRuntimeMap(MgResourceIdentifier mapDefinition, int requestedFeatures, int iconsPerScaleRange);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgByteReader CreateRuntimeMap(MgResourceIdentifier mapDefinition, int requestedFeatures, int iconsPerScaleRange);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgByteReader CreateRuntimeMap(MgResourceIdentifier mapDefinition, int requestedFeatures, int iconsPerScaleRange);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param mapDefinition (MgResourceIdentifier)
    /// MgResourceIdentifier object identifying the map definition resource.
    /// \param sessionId (String/string)
    /// The session ID
    /// \param requestedFeatures (int)
    /// A bitmask representing the desired information to return in the XML response: 
    /// 1=Layer/Group structure, 2=Layer Icons, 4=Layer Feature Source Information
    /// \param iconsPerScaleRange (int)
    /// The number of legend icons per scale range to render inline in the XML response as base64 strings. 
    /// If a scale range contains a number of rules that exceeds this value, only the first and
    /// last rules of a type style in the scale range will have inline icons
    ///
    /// \remarks
    /// The bitmask values of 2 (Layer Icons) and 4 (Layer Feature Source Information) have no effect if 1 (Layer/Group structure)
    /// is not specified
    ///
    /// \return
    /// Returns an XML-based description of the runtime map
    ///
    virtual MgByteReader* CreateRuntimeMap(MgResourceIdentifier* mapDefinition,
                                           CREFSTRING sessionId,
                                           INT32 requestedFeatures,
                                           INT32 iconsPerScaleRange);

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Creates a new runtime map from the specified Map Definition resource id and returns an XML-based description of the runtime
    /// map
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgByteReader CreateRuntimeMap(MgResourceIdentifier mapDefinition, int iconWidth, int iconHeight, int requestedFeatures, int iconsPerScaleRange);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgByteReader CreateRuntimeMap(MgResourceIdentifier mapDefinition, int iconWidth, int iconHeight, int requestedFeatures, int iconsPerScaleRange);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgByteReader CreateRuntimeMap(MgResourceIdentifier mapDefinition, int iconWidth, int iconHeight, int requestedFeatures, int iconsPerScaleRange);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param mapDefinition (MgResourceIdentifier)
    /// MgResourceIdentifier object identifying the map definition resource.
    /// \param targetMapName (String/string)
    /// The desired name of the runtime map
    /// \param sessionId (String/string)
    /// The session ID
    /// \param iconFormat (String/string)
    /// The desired image format for icons (from MgImageFormats)
    /// \param iconWidth (int)
    /// The width of each individual inline legend icons. Has no effect if icons was not requested in the response.
    /// \param iconHeight (int)
    /// The height of each individual inline legend icons. Has no effect if icons was not requested in the response.
    /// \param requestedFeatures (int)
    /// A bitmask representing the desired information to return in the XML response: 
    /// 1=Layer/Group structure, 2=Layer Icons, 4=Layer Feature Source Information
    /// \param iconsPerScaleRange (int)
    /// The number of legend icons per scale range to render inline in the XML response as base64 strings. 
    /// If a scale range contains a number of rules that exceeds this value, only the first and
    /// last rules of a type style in the scale range will have inline icons
    ///
    /// \remarks
    /// The bitmask values of 2 (Layer Icons) and 4 (Layer Feature Source Information) have no effect if 1 (Layer/Group structure)
    /// is not specified
    ///
    /// \return
    /// Returns an XML-based description of the runtime map
    ///
    virtual MgByteReader* CreateRuntimeMap(MgResourceIdentifier* mapDefinition,
                                           CREFSTRING targetMapName,
                                           CREFSTRING sessionId,
                                           CREFSTRING iconFormat,
                                           INT32 iconWidth,
                                           INT32 iconHeight,
                                           INT32 requestedFeatures,
                                           INT32 iconsPerScaleRange);

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns an XML-based description of the runtime map
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgByteReader DescribeRuntimeMap(MgMap map, int requestedFeatures, int iconsPerScaleRange);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgByteReader DescribeRuntimeMap(MgMap map, int requestedFeatures, int iconsPerScaleRange);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgByteReader DescribeRuntimeMap(MgMap map, int requestedFeatures, int iconsPerScaleRange);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param map (MgMap)
    /// The map to describe
    /// \param requestedFeatures (int)
    /// A bitmask representing the desired information to return in the XML response: 
    /// 1=Layer/Group structure, 2=Layer Icons, 4=Layer Feature Source Information
    /// \param iconsPerScaleRange (int)
    /// The number of legend icons per scale range to render inline in the XML response as base64 strings. 
    /// If a scale range contains a number of rules that exceeds this value, only the first and
    /// last rules of a type style in the scale range will have inline icons
    ///
    /// \remarks
    /// The bitmask values of 2 (Layer Icons) and 4 (Layer Feature Source Information) have no effect if 1 (Layer/Group structure)
    /// is not specified
    ///
    /// \return
    /// Returns an XML-based description of the runtime map
    ///
    virtual MgByteReader* DescribeRuntimeMap(MgMap* map,
                                             INT32 requestedFeatures,
                                             INT32 iconsPerScaleRange);

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns an XML-based description of the given runtime map
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgByteReader DescribeRuntimeMap(MgMap map, string iconFormat, int iconWidth, int iconHeight, int requestedFeatures, int iconsPerScaleRange);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgByteReader DescribeRuntimeMap(MgMap map, String iconFormat, int iconWidth, int iconHeight, int requestedFeatures, int iconsPerScaleRange);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgByteReader DescribeRuntimeMap(MgMap map, string iconFormat, int iconWidth, int iconHeight, int requestedFeatures, int iconsPerScaleRange);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param map (MgMap)
    /// The map to describe
    /// \param iconFormat (String/string)
    /// The desired image format for icons (from MgImageFormats)
    /// \param iconWidth (int)
    /// The width of each individual inline legend icons. Has no effect if icons was not requested in the response.
    /// \param iconHeight (int)
    /// The height of each individual inline legend icons. Has no effect if icons was not requested in the response.
    /// \param requestedFeatures (int)
    /// A bitmask representing the desired information to return in the XML response: 
    /// 1=Layer/Group structure, 2=Layer Icons, 4=Layer Feature Source Information
    /// \param iconsPerScaleRange (int)
    /// The number of legend icons per scale range to render inline in the XML response as base64 strings. 
    /// If a scale range contains a number of rules that exceeds this value, only the first and
    /// last rules of a type style in the scale range will have inline icons
    ///
    /// \remarks
    /// The bitmask values of 2 (Layer Icons) and 4 (Layer Feature Source Information) have no effect if 1 (Layer/Group structure)
    /// is not specified
    ///
    /// \return
    /// Returns an XML-based description of the runtime map
    ///
    /// \exception MgInvalidArgumentException
    /// \exception MgNullArgumentException
    ///
    virtual MgByteReader* DescribeRuntimeMap(MgMap* map,
                                             CREFSTRING iconFormat,
                                             INT32 iconWidth,
                                             INT32 iconHeight,
                                             INT32 requestedFeatures,
                                             INT32 iconsPerScaleRange);

INTERNAL_API:
    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Construct an MgProxyMappingService object.
    ///
    /// \param connection
    /// Connection to server
    ///
    MgProxyMappingService();

    DECLARE_CREATE_SERVICE()

    virtual MgByteReader* DescribeRuntimeMap(MgMap* map,
                                             CREFSTRING iconFormat,
                                             INT32 iconWidth,
                                             INT32 iconHeight,
                                             INT32 requestedFeatures,
                                             INT32 iconsPerScaleRange,
                                             INT32 schemaVersion);

    virtual MgByteReader* CreateRuntimeMap(MgResourceIdentifier* mapDefinition,
                                           CREFSTRING targetMapName,
                                           CREFSTRING sessionId,
                                           CREFSTRING iconFormat,
                                           INT32 iconWidth,
                                           INT32 iconHeight,
                                           INT32 requestedFeatures,
                                           INT32 iconsPerScaleRange,
                                           INT32 schemaVersion);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the connection properties for the Proxy Service.  This
    /// information tells the proxy object where to connect.
    ///
    /// \param connProp
    /// Connection properties for server
    ///
    void SetConnectionProperties(MgConnectionProperties* connProp);

    ///////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Generates an EPlot DWF containing a legend for the specified
    /// map based upon a specified scale.
    ///
    /// \param map
    /// Map object describing the current view state for the legend
    /// \param scale
    /// The scale of the map when generating the legend.
    /// \param plotSpec
    /// Plotting specifications for the legend describing plot size and margins.
    /// \param dwfVersion
    /// DWF specification supported/required by the client.  The DWF spec determines the
    /// schema and file versions used to generate the DWFs sent back to the client.
    ///
    /// \return
    /// An EPlot DWF for the legend
    ///
    /// \exception To be documented
    ///
    virtual MgByteReader* GenerateLegendPlot(
        MgMap* map,
        double scale,
        MgPlotSpecification* plotSpec,
        MgDwfVersion* dwfVersion);

protected:

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Dispose this object
    ///
    /// \return
    /// Nothing
    ///
    virtual void Dispose();

private:

    void SetWarning(MgWarnings* warning);

    Ptr<MgConnectionProperties> m_connProp;
};
/// \endcond

#endif
