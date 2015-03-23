#ifndef DESKTOP_MAPPING_SERVICE_H
#define DESKTOP_MAPPING_SERVICE_H

#include "MgDesktop.h"

class MgdLayout;
class MgdMapPlotCollection;
class MgdPlotSpecification;
class MgdDwfVersion;
class MgdMap;

namespace MdfModel
{
    class FeatureTypeStyle;
}

/// \ingroup Desktop_Service_Module
/// \{

////////////////////////////////////////////////////////////////
/// \brief
/// Performs map related operations such as getting an eMap DWF
/// representing a map or updates to a map, or querying features
/// from maps or layers of a map.
class MG_DESKTOP_API MgdMappingService : public MgService
{
    DECLARE_CLASSNAME(MgdMappingService)

public:
    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// Destructor
    /// </summary>
    virtual ~MgdMappingService();

    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// Construct an MgdDrawingService object
    /// </summary>
    MgdMappingService();

PUBLISHED_API:
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// GenerateLegendImage() returns the legend image for the specified layer.
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
    /// Image format, from MgImageFormats. Example: PNG, JPG, PNG8, etc 
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
                                               INT32 themeCategory);

    ///////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Plot The map to an EPlot DWF using the center and scale from the map.  The
    /// extents will be computed to fill the space within the page margins.  If the
    /// layout paramter is null, no adornments will be added to the page.
    ///
    /// \param map (MgdMap)
    /// Map object describing current state of map.
    /// \param plotSpec (MgdPlotSpecification)
    /// Plotting specifications for the generated plot.
    /// \param layout (MgdLayout)
    /// Layout specifications to use for the generated plot.
    /// \param dwfVersion (MgdDwfVersion)
    /// DWF version required by the client. This
    /// determines the schema and file versions used to
    /// generate the DWFs sent back to the client.
    ///
    /// \return
    /// Returns an MgByteReader object containing a DWF in ePlot format.
    ///
    /// \exception MgNullArgumentException
    ///
    virtual MgByteReader* GeneratePlot(MgdMap* map,
                                       MgdPlotSpecification* plotSpec,
                                       MgdLayout* layout,
                                       MgdDwfVersion* dwfVersion);

    ///////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Plot the map to an EPlot DWF using the specified center and scale.  The extents
    /// will be computed to fill the space within the page margins.  If the layout
    /// paramter is null, no adornments will be added to the page.
    ///
    /// \param map (MgdMap)
    /// Map object describing current state of map.
    /// \param center (MgCoordinate)
    /// The coordinates of the center of the map for the plot.
    /// \param scale (double)
    /// The scale to use for the plot.
    /// \param plotSpec (MgdPlotSpecification)
    /// Plotting specifications for the generated plot.
    /// \param layout (MgdLayout)
    /// Layout specifications to use for the generated plot.
    /// \param dwfVersion (MgdDwfVersion)
    /// DWF version required by the client. This
    /// determines the schema and file versions used to
    /// generate the DWFs sent back to the client.
    ///
    /// \return
    /// Returns an MgByteReader object containing a DWF in ePlot format.
    ///
    /// \exception MgNullArgumentException
    ///
    virtual MgByteReader* GeneratePlot(MgdMap* map,
                                       MgCoordinate* center,
                                       double scale,
                                       MgdPlotSpecification* plotSpec,
                                       MgdLayout* layout,
                                       MgdDwfVersion* dwfVersion);

    ///////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Plot the map to an EPlot DWF using the extent specified in the mapping
    /// coordinate space.  The center and scale will be computed based on the extent
    /// specified.  If expandToFit is true, then the extent specified will be expanded
    /// in the X or Y direction to fill the space within the page margins.  If the
    /// layout paramter is null, no adornments will be added to the page.
    ///
    /// \param map (MgdMap)
    /// Map object describing current state of map.
    /// \param extents (MgEnvelope)
    /// The extents of the map to be plotted..
    /// \param expandToFit (boolean/bool)
    /// Flag indicating whether the extents should be expanded to
    /// fill the space withing the page margins.
    /// \param plotSpec (MgdPlotSpecification)
    /// Plotting specifications for the generated plot.
    /// \param layout (MgdLayout)
    /// Layout specifications to use for the generated plot.
    /// \param dwfVersion (MgdDwfVersion)
    /// DWF version required by the client. This
    /// determines the schema and file versions used to
    /// generate the DWFs sent back to the client.
    ///
    /// \return
    /// Returns an MgByteReader object containing a DWF in ePlot format.
    ///
    /// \exception MgNullArgumentException
    ///
    virtual MgByteReader* GeneratePlot(MgdMap* map,
                                       MgEnvelope* extents,
                                       bool expandToFit,
                                       MgdPlotSpecification* plotSpec,
                                       MgdLayout* layout,
                                       MgdDwfVersion* dwfVersion);

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Generates an \link ePlot ePlot \endlink containing one sheet per
    /// specified map.
    ///
    /// \param mapPlots (MgdMapPlotCollection)
    /// A collection of MapPlot objects.
    /// \param dwfVersion (MgdDwfVersion)
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
    virtual MgByteReader* GenerateMultiPlot(MgdMapPlotCollection* mapPlots, MgdDwfVersion* dwfVersion);

private:
    virtual MgByteReader* GenerateMultiPlotInternal(MgdMapPlotCollection* mapPlots, MgdDwfVersion* dwfVersion);
    bool FeatureTypeStyleSupportsGeomType(MdfModel::FeatureTypeStyle* fts, INT32 geomType);

    // member data
    Ptr<MgFeatureService> m_svcFeature;
    Ptr<MgResourceService> m_svcResource;
    Ptr<MgdDrawingService> m_svcDrawing;
    Ptr<MgCoordinateSystemFactory> m_pCSFactory;
    
    // Mapping Service configuration properties
    INT32 m_rasterGridSizeForPlot;
    INT32 m_minRasterGridSizeForPlot;
    double m_rasterGridSizeOverrideRatioForPlot;
    INT32 m_maxRasterImageWidth;
    INT32 m_maxRasterImageHeight;

protected:

    virtual INT32 GetClassId() { return m_cls_id; }

    virtual void Dispose() { delete this; }

CLASS_ID:
    static const INT32 m_cls_id = MapGuide_Desktop_RenderingService_RenderingService;
};
/// \}
#endif