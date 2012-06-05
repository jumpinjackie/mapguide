#ifndef DESKTOP_MAPPING_SERVICE_H
#define DESKTOP_MAPPING_SERVICE_H

#include "MgDesktop.h"

class MgLayout;
class MgMapPlotCollection;
class MgPlotSpecification;
class MgDwfVersion;
class MgdMap;

namespace MdfModel
{
    class FeatureTypeStyle;
}

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
    virtual MgByteReader* GenerateLegendImage(MgResourceIdentifier* resource,
                                               double scale,
                                               INT32 width,
                                               INT32 height,
                                               CREFSTRING format,
                                               INT32 geomType,
                                               INT32 themeCategory);

    virtual MgByteReader* GeneratePlot(MgdMap* map,
                                       MgPlotSpecification* plotSpec,
                                       MgLayout* layout,
                                       MgDwfVersion* dwfVersion);

    virtual MgByteReader* GeneratePlot(MgdMap* map,
                                       MgCoordinate* center,
                                       double scale,
                                       MgPlotSpecification* plotSpec,
                                       MgLayout* layout,
                                       MgDwfVersion* dwfVersion);

    virtual MgByteReader* GeneratePlot(MgdMap* map,
                                       MgEnvelope* extents,
                                       bool expandToFit,
                                       MgPlotSpecification* plotSpec,
                                       MgLayout* layout,
                                       MgDwfVersion* dwfVersion);

    virtual MgByteReader* GenerateMultiPlot(MgMapPlotCollection* mapPlots, MgDwfVersion* dwfVersion);

private:
    virtual MgByteReader* GenerateMultiPlotInternal(MgMapPlotCollection* mapPlots, MgDwfVersion* dwfVersion);
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

#endif