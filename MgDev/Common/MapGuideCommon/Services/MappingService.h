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

#ifndef MG_MAPPING_SERVICE_H
#define MG_MAPPING_SERVICE_H

#include "MappingDefs.h"
#include "DwfVersion.h"
#include "PlotSpecification.h"
#include "CustomLogoInfo.h"
#include "CustomTextInfo.h"
#include "Layout.h"
#include "PrintLayout.h"
#include "MapPlot.h"
#include "MapPlotCollection.h"
#include "MapPlotInstruction.h"
#include "UnitType.h"

class MgByteReader;
class MgMap;
class MgMapCollection;
class MgMapView;
class MgMapViewCollection;
class MgPlotSpecification;
class MgResourceIdentifier;
class MgEnvelope;

/// \defgroup MgMappingService MgMappingService
/// \ingroup Mapping_Service_Module
/// \{

////////////////////////////////////////////////////////////////
/// \brief
/// Performs map related operations such as getting an eMap DWF
/// representing a map or updates to a map, or querying features
/// from maps or layers of a map.
class MG_MAPGUIDE_API MgMappingService : public MgService
{
PUBLISHED_API:
    ////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Generates an \link eMap eMap \endlink that describes the specified
    /// map
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgByteReader GenerateMap(MgMap map, string mapAgentUri, MgDwfVersion dwfVersion);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgByteReader GenerateMap(MgMap map, String mapAgentUri, MgDwfVersion dwfVersion);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgByteReader GenerateMap(MgMap map, string mapAgentUri, MgDwfVersion dwfVersion);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param map (MgMap)
    /// Map object describing current state of map
    /// \param mapAgentUri (String/string)
    /// Http Uri for map agent
    /// \param dwfVersion (MgDwfVersion)
    /// DWF version required by the client. This
    /// determines the schema and file versions used
    /// to generate the DWFs sent back to the client.
    ///
    /// \return
    /// Returns an MgByteReader object containing a DWF of the
    /// legend in eMap format.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// \code
    /// // Assuming $resourceService and $mappingService have already been
    /// // initialized.
    /// // Get a runtime map from a map definition
    /// $resourceID = new  MgResourceIdentifier('Library://Calgary/Maps/Calgary.MapDefinition');
    /// $map = new MgMap();
    /// $map->Create($resourceService, $resourceID, 'Calgary');
    ///
    /// // Generate a DWF from the map
    /// $mapAgentURI = 'http://localhost/phpmapagent/mapAgent.php'; // Replace 'localhost' with your server name
    /// $dwfVersion = new MgDwfVersion('6.01','1.2');
    /// $byteReader = $mappingService->GenerateMap($map, $mapAgentURI, $dwfVersion);
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    /// \exception MgNullArgumentException
    ///
    virtual MgByteReader* GenerateMap(MgMap* map, CREFSTRING mapAgentUri, MgDwfVersion* dwfVersion);

    ///////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Generates an \link eMap eMap \endlink that contains any required
    /// updates to the metadata and layer graphics based on the state
    /// of the specified map.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgByteReader GenerateMapUpdate(MgMap map, int seqNo, MgDwfVersion dwfVersion);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgByteReader GenerateMapUpdate(MgMap map, int seqNo, MgDwfVersion dwfVersion);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgByteReader GenerateMapUpdate(MgMap map, int seqNo, MgDwfVersion dwfVersion);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param map (MgMap)
    /// Map object specifying current state of map.
    /// \param seqNo (int)
    /// Update sequence number. An arbitrary number which is
    /// set automatically by the DWF Viewer and used by a
    /// client to identify a map update request. The client
    /// passes this number as part of its request for data, and
    /// that number is returned here so that the client can
    /// match the response with the request. The MapGuide
    /// server and the web tier do not make any use of this
    /// number except sending it back as part of the response.
    /// \param dwfVersion (MgDwfVersion)
    /// DWF version required by the client. This determines the
    /// schema and file versions used to generate the DWFs sent
    /// back to the client.
    ///
    /// \return
    /// Returns an MgByteReader object containing a DWF of the
    /// legend in eMap format.
    ///
    /// \exception MgNullArgumentException
    ///
    ///
    virtual MgByteReader* GenerateMapUpdate(MgMap* map, INT32 seqNo, MgDwfVersion* dwfVersion) = 0;

    ///////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Plot The map to an EPlot DWF using the center and scale from the map.  The
    /// extents will be computed to fill the space within the page margins.  If the
    /// layout paramter is null, no adornments will be added to the page.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgByteReader GeneratePlot(MgMap map, MgPlotSpecification plotSpec, MgLayout layout, MgDwfVersion dwfVersion);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgByteReader GeneratePlot(MgMap map, MgPlotSpecification plotSpec, MgLayout layout, MgDwfVersion dwfVersion);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgByteReader GeneratePlot(MgMap map, MgPlotSpecification plotSpec, MgLayout layout, MgDwfVersion dwfVersion);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param map (MgMap)
    /// Map object describing current state of map.
    /// \param plotSpec (MgPlotSpecification)
    /// Plotting specifications for the generated plot.
    /// \param layout (MgLayout)
    /// Layout specifications to use for the generated plot.
    /// \param dwfVersion (MgDwfVersion)
    /// DWF version required by the client. This
    /// determines the schema and file versions used to
    /// generate the DWFs sent back to the client.
    ///
    /// \return
    /// Returns an MgByteReader object containing a DWF in ePlot format.
    ///
    /// \exception MgNullArgumentException
    ///
    virtual MgByteReader* GeneratePlot(
        MgMap* map,
        MgPlotSpecification* plotSpec,
        MgLayout* layout,
        MgDwfVersion* dwfVersion) = 0;

    ///////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Plot the map to an EPlot DWF using the specified center and scale.  The extents
    /// will be computed to fill the space within the page margins.  If the layout
    /// paramter is null, no adornments will be added to the page.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgByteReader GeneratePlot(MgMap map, MgCoordinate center, double scale, MgPlotSpecification plotSpec, MgLayout layout, MgDwfVersion dwfVersion);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgByteReader GeneratePlot(MgMap map, MgCoordinate center, double scale, MgPlotSpecification plotSpec, MgLayout layout, MgDwfVersion dwfVersion);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgByteReader GeneratePlot(MgMap map, MgCoordinate center, double scale, MgPlotSpecification plotSpec, MgLayout layout, MgDwfVersion dwfVersion);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param map (MgMap)
    /// Map object describing current state of map.
    /// \param center (MgCoordinate)
    /// The coordinates of the center of the map for the plot.
    /// \param scale (double)
    /// The scale to use for the plot.
    /// \param plotSpec (MgPlotSpecification)
    /// Plotting specifications for the generated plot.
    /// \param layout (MgLayout)
    /// Layout specifications to use for the generated plot.
    /// \param dwfVersion (MgDwfVersion)
    /// DWF version required by the client. This
    /// determines the schema and file versions used to
    /// generate the DWFs sent back to the client.
    ///
    /// \return
    /// Returns an MgByteReader object containing a DWF in ePlot format.
    ///
    /// \exception MgNullArgumentException
    ///
    virtual MgByteReader* GeneratePlot(
        MgMap* map,
        MgCoordinate* center,
        double scale,
        MgPlotSpecification* plotSpec,
        MgLayout* layout,
        MgDwfVersion* dwfVersion) = 0;

    ///////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Plot the map to an EPlot DWF using the extent specified in the mapping
    /// coordinate space.  The center and scale will be computed based on the extent
    /// specified.  If expandToFit is true, then the extent specified will be expanded
    /// in the X or Y direction to fill the space within the page margins.  If the
    /// layout paramter is null, no adornments will be added to the page.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgByteReader GeneratePlot(MgMap map, MgEnvelope extents, bool expandToFit, MgPlotSpecification plotSpec, MgLayout layout, MgDwfVersion dwfVersion);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgByteReader GeneratePlot(MgMap map, MgEnvelope extents, boolean expandToFit, MgPlotSpecification plotSpec, MgLayout layout, MgDwfVersion dwfVersion);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgByteReader GeneratePlot(MgMap map, MgEnvelope extents, bool expandToFit, MgPlotSpecification plotSpec, MgLayout layout, MgDwfVersion dwfVersion);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param map (MgMap)
    /// Map object describing current state of map.
    /// \param extents (MgEnvelope)
    /// The extents of the map to be plotted..
    /// \param expandToFit (boolean/bool)
    /// Flag indicating whether the extents should be expanded to
    /// fill the space withing the page margins.
    /// \param plotSpec (MgPlotSpecification)
    /// Plotting specifications for the generated plot.
    /// \param layout (MgLayout)
    /// Layout specifications to use for the generated plot.
    /// \param dwfVersion (MgDwfVersion)
    /// DWF version required by the client. This
    /// determines the schema and file versions used to
    /// generate the DWFs sent back to the client.
    ///
    /// \return
    /// Returns an MgByteReader object containing a DWF in ePlot format.
    ///
    /// \exception MgNullArgumentException
    ///
    virtual MgByteReader* GeneratePlot(
        MgMap* map,
        MgEnvelope* extents,
        bool expandToFit,
        MgPlotSpecification* plotSpec,
        MgLayout* layout,
        MgDwfVersion* dwfVersion) = 0;

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Generates an \link ePlot ePlot \endlink containing one sheet per
    /// specified map.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgByteReader GenerateMultiPlot(MgMapPlotCollection mapPlots, MgDwfVersion dwfVersion);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgByteReader GenerateMultiPlot(MgMapPlotCollection mapPlots, MgDwfVersion dwfVersion);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgByteReader GenerateMultiPlot(MgMapPlotCollection mapPlots, MgDwfVersion dwfVersion);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param mapPlots (MgMapPlotCollection)
    /// A collection of MapPlot objects.
    /// \param dwfVersion (MgDwfVersion)
    /// DWF specification required by the client. This
    /// determines the schema and file versions used to
    /// generate the DWFs sent back to the client.
    ///
    /// \return
    /// Returns an MgByteReader object containing a DWF
    /// in ePlot format with multiple sheets, one for each
    /// map.
    ///
    /// \exception MgNullArgumentException
    ///
    virtual MgByteReader* GenerateMultiPlot(
        MgMapPlotCollection* mapPlots,
        MgDwfVersion* dwfVersion) = 0;

    /// \brief
    /// GenerateLegendImage() returns the legend image for the specified layer.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgByteReader GenerateLegendImage(MgResourceIdentifier resource, double scale, int width, int height, string format, int geomType, int themeCategory);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgByteReader GenerateLegendImage(MgResourceIdentifier resource, double scale, int width, int height, String format, int geomType, int themeCategory);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgByteReader GenerateLegendImage(MgResourceIdentifier resource, double scale, int width, int height, string format, int geomType, int themeCategory);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param resource (MgResourceIdentifier)
    /// Input
    /// MgResourceIdentifier object identifying the layer definition resource.
    /// \param scale (double)
    /// Input
    /// The scale at which the symbolization is requested.
    /// \param width (int)
    /// Input
    /// The requested image width in pixels.
    /// \param height (int)
    /// Input
    /// The requested image height in pixels.
    /// \param format (String/string)
    /// Input
    /// Image format, from MgImageFormats. Example: PNG, JPG, PNG8, etc …
    /// \param geomType (int)
    /// Input
    /// The type of symbolization required: 1=Point, 2=Line, 3=Area, 4=Composite
    /// \param themeCategory (int)
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
                                              INT32 themeCategory) = 0;

EXTERNAL_API:

    //////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Queries and returns each of the visible features on the
    /// specified layer of the specified map.
    ///
    /// \remarks
    /// For feature layers the properties to be included are defined
    /// in the Feature Layer definition. For DWF layers the
    /// properties of each of the objects in the DWF source for the
    /// layer should be included as the feature properties. The
    /// geometry of each feature can optionally be returned in either
    /// the coordinate space of the map or the coordinate space of
    /// the display. This operation returns the feature properties in
    /// XML format, which can be used to construct an HTML Image Map
    /// for a previously rendered view of the map.
    /// \n
    /// This method does not support \link WMS WMS \endlink GetFeatureInfo
    ///
    /// \param map
    /// Input map object containing current viewed
    /// state of map.
    /// \param layerName
    /// Input name of the layer to query properties
    /// for
    /// \param coordinateSpace
    /// Input specifies the coordinate system the
    /// geometry should be returned in. See
    /// MgCoordinateSpace
    ///
    /// \return
    /// A byte reader containing an XML document with the feature
    /// information described in GML (Geography Markup Language). GML
    /// is defined by the OpenGIS® consortium. A specification for
    /// this language can be found at: /// https://portal.opengeospatial.org/files/?artifact_id=7174
    ///
    ///
    virtual MgByteReader* QueryFeatures(MgMap* map, CREFSTRING layerName, CREFSTRING coordinateSpace) = 0;

    //////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Queries feature information for each feature visible at the
    /// specified point of interest, on a given set of layers.
    ///
    /// \remarks
    /// This method is used to support \link WMS WMS \endlink
    /// GetFeatureInfo.
    ///
    /// \param map
    /// Map object containing current view state of the
    /// map.
    /// \param layerNames
    /// List of layers to return information from. Each
    /// layer must be in the specified MgMap object
    /// \param x
    /// X coordinate for point of interest
    /// \param y
    /// Y coordinate for point of interest
    /// \param maxFeatures
    /// Maximum number of feature that should be returned
    /// for a given layer greater than 0, usually 1. If
    /// the number of features sharing the requested
    /// location is greater than maxFeatures the subset of
    /// features returned depends on the provider.
    /// \param coordinateSpace
    /// Input specifies the coordinate system the
    /// geometry should be returned in. See
    /// MgCoordinateSpace
    ///
    /// \return
    /// Returns a byte reader containing an XML document with the feature
    /// information described in GML (Geography Markup Language). GML
    /// is defined by the OpenGIS® consortium. A specification for
    /// this language can be found at: /// https://portal.opengeospatial.org/files/?artifact_id=7174
    ///
    ///
    virtual MgByteReader* QueryFeatures(MgMap* map, MgStringCollection* layerNames, INT32 x, INT32 y, INT32 maxFeatures, CREFSTRING coordinateSpace) = 0;

INTERNAL_API:
    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Construct an MgMappingService object.
    ///
    MgMappingService();

    //TODO: Get rid of sessionId from API once user information is passed to server
    virtual MgByteReader* GenerateMap(MgMap* map, CREFSTRING sessionId, CREFSTRING mapAgentUri, MgDwfVersion* dwfVersion) = 0;

    ///////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Generates an \link ePlot ePlot \endlink containing a legend for the
    /// specified map based upon its current view scale.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgByteReader GenerateLegendPlot(MgMap map, double scale, MgPlotSpecification plotSpec, MgDwfVersion dwfVersion);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgByteReader GenerateLegendPlot(MgMap map, double scale, MgPlotSpecification plotSpec, MgDwfVersion dwfVersion);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgByteReader GenerateLegendPlot(MgMap map, double scale, MgPlotSpecification plotSpec, MgDwfVersion dwfVersion);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param map (MgMap)
    /// Map object describing the current view state for
    /// the legend.
    /// \param scale (double)
    /// The scale of the map, which determines the features that are
    /// displayed in the legend.
    /// \param plotSpec (MgPlotSpecification)
    /// Plotting specifications for the legend.
    /// \param dwfVersion (MgDwfVersion)
    /// DWF version required by the client. This
    /// determines the schema and file versions used to
    /// generate the DWFs sent back to the client.
    ///
    /// \return
    /// Returns an MgByteReader object containing a DWF of the
    /// legend in ePlot format.
    ///
    /// \exception MgNullArgumentException
    ///
    virtual MgByteReader* GenerateLegendPlot(
        MgMap* map,
        double scale,
        MgPlotSpecification* plotSpec,
        MgDwfVersion* dwfVersion) = 0;

protected:

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Get the class Id
    ///
    /// \return
    /// The integer value
    ///
    virtual INT32 GetClassId();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Dispose this object
    ///
    /// \return
    /// Nothing
    ///
    virtual void Dispose();

private:

CLASS_ID:
    static const INT32 m_cls_id = MapGuide_MappingService_MappingService;

public:

    //////////////////////////////////////////////////////////////////
    /// Enumerations
    enum OperationId
    {
        opIdGenerateMap             =  0x1111EE01,
        opIdGenerateMapUpdate       =  0x1111EE02,
        opIdGeneratePlot            =  0x1111EE03,
        opIdGeneratePlotWithLayout  =  0x1111EE04,
        opIdGenerateMultiPlot       =  0x1111EE05,
        opIdGenerateLegendPlot      =  0x1111EE06,
        opIdQueryFeatures           =  0x1111EE07,
        opIdQueryFeaturesWms        =  0x1111EE08,
        opIdGeneratePlot2           =  0x1111EE09,
        opIdGeneratePlot3           =  0x1111EE0A,
        opIdGenerateLegendImage     =  0x1111EE0D
    };
};
/// \}

#endif
