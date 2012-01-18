#ifndef DESKTOP_RENDERING_SERVICE_H
#define DESKTOP_RENDERING_SERVICE_H

class MgLayout;
class MgMapPlotCollection;
class MgPlotSpecification;
class MgDwfVersion;
class MgdMap;
class SE_Renderer;
class MgRenderingOptions;
class FeatureInfoRenderer;
class MgDrawingService;
struct RS_Bounds;
class RS_Color;

namespace MdfModel
{
    class FeatureTypeStyle;
}

template class Ptr<MgDrawingService>;

//NOTE: Despite MgRenderingService not defined in PlatformBase, we don't want a naming collision
//if this library and MapGuideCommon happend to be linked together

class MG_DESKTOP_API MgRenderingService : public MgService
{
    DECLARE_CLASSNAME(MgRenderingService)

public:
    MgRenderingService();
    ~MgRenderingService();

    DECLARE_CREATE_SERVICE()

    void SetConnectionProperties(MgConnectionProperties* connProp);

PUBLISHED_API:

    virtual MgByteReader* RenderTile(MgdMap* map,
                                     CREFSTRING baseMapLayerGroupName,
                                     INT32 tileColumn,
                                     INT32 tileRow);

    virtual MgByteReader* RenderDynamicOverlay(MgdMap* map,
                                               MgdSelection* selection,
                                               CREFSTRING format);

    virtual MgByteReader* RenderDynamicOverlay(MgdMap* map,
                                               MgdSelection* selection,
                                               CREFSTRING format,
                                               bool bKeepSelection);

    virtual MgByteReader* RenderDynamicOverlay(MgdMap* map,
                                               MgdSelection* selection,
                                               MgRenderingOptions* options);

    virtual MgByteReader* RenderMap(MgdMap* map,
                                    MgdSelection* selection,
                                    CREFSTRING format);

    virtual MgByteReader* RenderMap(MgdMap* map,
                                    MgdSelection* selection,
                                    CREFSTRING format,
                                    bool bKeepSelection);

    virtual MgByteReader* RenderMap(MgdMap* map,
                                    MgdSelection* selection,
                                    CREFSTRING format,
                                    bool bKeepSelection,
                                    bool bClip);

    virtual MgByteReader* RenderMapLegend(MgdMap* map,
                                          INT32 width,
                                          INT32 height,
                                          MgColor* backgroundColor,
                                          CREFSTRING format);

    virtual MgByteReader* GenerateLegendImage(MgResourceIdentifier* resource,
                                              double scale,
                                              INT32 width,
                                              INT32 height,
                                              CREFSTRING format,
                                              INT32 geomType,
                                              INT32 themeCategory);

    // --------------- DWF Rendering APIs from MgMappingService -----------------------//

    virtual MgByteReader* GeneratePlot(
        MgdMap* map,
        MgPlotSpecification* plotSpec,
        MgLayout* layout,
        MgDwfVersion* dwfVersion);

    virtual MgByteReader* GeneratePlot(
        MgdMap* map,
        MgCoordinate* center,
        double scale,
        MgPlotSpecification* plotSpec,
        MgLayout* layout,
        MgDwfVersion* dwfVersion);

    virtual MgByteReader* GeneratePlot(
        MgdMap* map,
        MgEnvelope* extents,
        bool expandToFit,
        MgPlotSpecification* plotSpec,
        MgLayout* layout,
        MgDwfVersion* dwfVersion);

    virtual MgByteReader* GenerateMultiPlot(
        MgMapPlotCollection* mapPlots,
        MgDwfVersion* dwfVersion);

INTERNAL_API:
    //These APIs are most likely relics from having to deal with a stateless
    //medium like HTTP. We won't remove them for now, but we won't make them
    //visible either
    virtual MgByteReader* RenderMap(MgdMap* map,
                                    MgdSelection* selection,
                                    MgEnvelope* extents,
                                    INT32 width,
                                    INT32 height,
                                    MgColor* backgroundColor,
                                    CREFSTRING format);

    virtual MgByteReader* RenderMap(MgdMap* map,
                                    MgdSelection* selection,
                                    MgEnvelope* extents,
                                    INT32 width,
                                    INT32 height,
                                    MgColor* backgroundColor,
                                    CREFSTRING format,
                                    bool bKeepSelection);

    virtual MgByteReader* RenderMap(MgdMap* map,
                                    MgdSelection* selection,
                                    MgCoordinate* center,
                                    double scale,
                                    INT32 width,
                                    INT32 height,
                                    MgColor* backgroundColor,
                                    CREFSTRING format);

    virtual MgByteReader* RenderMap(MgdMap* map,
                                    MgdSelection* selection,
                                    MgCoordinate* center,
                                    double scale,
                                    INT32 width,
                                    INT32 height,
                                    MgColor* backgroundColor,
                                    CREFSTRING format,
                                    bool bKeepSelection);

    virtual MgByteReader* RenderMap(MgdMap* map,
                                    MgdSelection* selection,
                                    MgCoordinate* center,
                                    double scale,
                                    INT32 width,
                                    INT32 height,
                                    MgColor* backgroundColor,
                                    CREFSTRING format,
                                    bool bKeepSelection,
                                    bool bClip);

private:
    bool FeatureTypeStyleSupportsGeomType(MdfModel::FeatureTypeStyle* fts, INT32 geomType);

    // used for tile generation
    MgByteReader* RenderTile(MgdMap* map,
                             MgLayerGroup* baseGroup,
                             INT32 scaleIndex,
                             INT32 width,
                             INT32 height,
                             double scale,
                             double mcsMinX,
                             double mcsMaxX,
                             double mcsMinY,
                             double mcsMaxY,
                             CREFSTRING format);

    // helper used by other methods
    MgByteReader* RenderMapInternal(MgdMap* map,
                                    MgdSelection* selection,
                                    MgReadOnlyLayerCollection* roLayers,
                                    SE_Renderer* dr,
                                    INT32 drawWidth,
                                    INT32 drawHeight,
                                    INT32 saveWidth,
                                    INT32 saveHeight,
                                    CREFSTRING format,
                                    double scale,
                                    RS_Bounds& b,
                                    bool expandExtents,
                                    bool bKeepSelection,
                                    bool renderWatermark);

    MgByteReader* RenderMapInternal(MgdMap* map,
                                    MgdSelection* selection,
                                    MgReadOnlyLayerCollection* roLayers,
                                    SE_Renderer* dr,
                                    INT32 drawWidth,
                                    INT32 drawHeight,
                                    INT32 saveWidth,
                                    INT32 saveHeight,
                                    double scale,
                                    RS_Bounds& b,
                                    bool expandExtents,
                                    MgRenderingOptions* options,
                                    bool renderWatermark);

    void RenderForSelection(MgdMap* map,
                         MgStringCollection* layerNames,
                         MgGeometry* geometry,
                         INT32 selectionVariant,
                         CREFSTRING featureFilter,
                         INT32 maxFeatures,
                         INT32 layerAttributeFilter,
                         FeatureInfoRenderer* selRenderer);

    SE_Renderer* CreateRenderer(int width,
                                int height,
                                RS_Color& bgColor,
                                bool requiresClipping,
                                bool localOverposting = false,
                                double tileExtentOffset = 0.0);

    // member data
    Ptr<MgFeatureService> m_svcFeature;
    Ptr<MgResourceService> m_svcResource;
    Ptr<MgDrawingService> m_svcDrawing;
    Ptr<MgCoordinateSystemFactory> m_pCSFactory;

    // this will eventually be removed
    STRING m_rendererName;

    INT32 m_rasterGridSize;
    INT32 m_minRasterGridSize;
    double m_rasterGridSizeOverrideRatio;
    INT32 m_renderSelectionBatchSize;
    INT32 m_maxRasterImageWidth;
    INT32 m_maxRasterImageHeight;

    INT32 m_rasterGridSizeForPlot;
    INT32 m_minRasterGridSizeForPlot;
    double m_rasterGridSizeOverrideRatioForPlot;

protected:

    virtual INT32 GetClassId() { return m_cls_id; }

    virtual void Dispose() { delete this; }

CLASS_ID:
    static const INT32 m_cls_id = MapGuide_Desktop_RenderingService_RenderingService;
};

#endif