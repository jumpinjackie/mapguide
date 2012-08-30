#ifndef DESKTOP_RENDERING_SERVICE_H
#define DESKTOP_RENDERING_SERVICE_H

class MgdMap;
class MgdFeatureInformation;
class SE_Renderer;
class MgdRenderingOptions;
class MgdFeatureInfoRenderer;
class MgdDrawingService;
struct RS_Bounds;
class RS_Color;
class Stylizer;
class Renderer;

namespace MdfModel
{
    class FeatureTypeStyle;
	class ProfileRenderMapResult;
}

template class Ptr<MgdDrawingService>;

/// \ingroup Desktop_Service_Module
/// \{
class MG_DESKTOP_API MgdRenderingService : public MgService
{
    DECLARE_CLASSNAME(MgdRenderingService)

public:
    MgdRenderingService();
    ~MgdRenderingService();

    DECLARE_CREATE_SERVICE()

    //void SetConnectionProperties(MgConnectionProperties* connProp);

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
                                               MgdRenderingOptions* options);

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

    virtual MgdFeatureInformation* QueryFeatures(MgdMap* map,
                                                 MgStringCollection* layerNames,
                                                 MgGeometry* filterGeometry,
                                                 INT32 selectionVariant,
                                                 INT32 maxFeatures);

    virtual MgdFeatureInformation* QueryFeatures(MgdMap* map,
                                                 MgStringCollection* layerNames,
                                                 MgGeometry* filterGeometry,
                                                 INT32 selectionVariant,
                                                 CREFSTRING featureFilter,
                                                 INT32 maxFeatures,
                                                 INT32 layerAttributeFilter);

INTERNAL_API:

    virtual MgByteReader* RenderDynamicOverlay(MgdMap* map,
                                               MgdSelection* selection,
                                               MgdRenderingOptions* options,
                                               MdfModel::ProfileRenderMapResult* pPRMResult);

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
                                    MdfModel::ProfileRenderMapResult* pPRMResult);

    virtual MgByteReader* RenderMap(MgdMap* map,
                                    MgdSelection* selection,
                                    MgCoordinate* center,
                                    double scale,
                                    INT32 width,
                                    INT32 height,
                                    MgColor* backgroundColor,
                                    CREFSTRING format,
                                    bool bKeepSelection,
                                    bool bClip,
                                    MdfModel::ProfileRenderMapResult* pPRMResult = NULL);

private:
    MgdFeatureInformation* QueryFeaturesInternal(MgdMap* map,
                                                 MgStringCollection* layerNames,
                                                 MgGeometry* filterGeometry,
                                                 INT32 selectionVariant,
                                                 CREFSTRING featureFilter,
                                                 INT32 maxFeatures,
                                                 INT32 layerAttributeFilter);

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

    // Internal help called by our PUBLISHED_API versions of RenderDynamicOverlay
    MgByteReader* RenderDynamicOverlayInternal(MgdMap* map,
                                               MgdSelection* selection,
                                               MgdRenderingOptions* options,
                                               MdfModel::ProfileRenderMapResult* pPRMResult);

    // Internal help called by our PUBLISHED_API versions of RenderMap
    MgByteReader* RenderMapPublished(MgdMap* map,
                                     MgdSelection* selection,
                                     CREFSTRING format,
                                     bool bKeepSelection,
                                     bool bClip);

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
                                    bool renderWatermark,
                                    MdfModel::ProfileRenderMapResult* pPRMResult = NULL);
	
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
                                    MgdRenderingOptions* options,
                                    bool renderWatermark,
                                    MdfModel::ProfileRenderMapResult* pPRMResult = NULL);

    void RenderForSelection(MgdMap* map,
                            MgStringCollection* layerNames,
                            MgGeometry* geometry,
                            INT32 selectionVariant,
                            CREFSTRING featureFilter,
                            INT32 maxFeatures,
                            INT32 layerAttributeFilter,
                            MgdFeatureInfoRenderer* selRenderer);

    SE_Renderer* CreateRenderer(int width,
                                int height,
                                RS_Color& bgColor,
                                bool requiresClipping,
                                bool localOverposting = false,
                                double tileExtentOffset = 0.0);

    void RenderLayers(MgdMap* map,
                      MgReadOnlyLayerCollection* layers,
                      Stylizer* ds,
                      Renderer* dr,
                      MgCoordinateSystem* dstCs,
                      bool expandExtents,
                      double scale,
                      CREFSTRING format,
                      MdfModel::ProfileRenderMapResult* pPRMResult);

    void RenderSelection(MgdMap* map,
                         MgdSelection* selection,
                         MgReadOnlyLayerCollection* layers,
                         MgdRenderingOptions* options,
                         Stylizer* ds,
                         Renderer* dr,
                         MgCoordinateSystem* dstCs,
                         double scale,
                         INT32 behavior,
                         MdfModel::ProfileRenderMapResult* pPRMResult);

    void RenderWatermarks(MgdMap* map,
                          MgReadOnlyLayerCollection* layers,
                          Stylizer* ds,
                          Renderer* dr,
                          int drawWidth,
                          int drawHeight,
                          INT32 saveWidth,
                          INT32 saveHeight,
                          MdfModel::ProfileRenderMapResult* pPRMResult);

    MgByteReader* CreateImage(MgdMap* map,
                              Renderer* dr,
                              INT32 saveWidth,
                              INT32 saveHeight,
                              CREFSTRING format,
                              MdfModel::ProfileRenderMapResult* pPRMResult);

    // member data
    Ptr<MgFeatureService> m_svcFeature;
    Ptr<MgResourceService> m_svcResource;
    Ptr<MgdDrawingService> m_svcDrawing;
    Ptr<MgCoordinateSystemFactory> m_pCSFactory;

    // this will eventually be removed
    STRING m_rendererName;

    INT32 m_rasterGridSize;
    INT32 m_minRasterGridSize;
    double m_rasterGridSizeOverrideRatio;
    INT32 m_renderSelectionBatchSize;
    INT32 m_maxRasterImageWidth;
    INT32 m_maxRasterImageHeight;

protected:
    virtual INT32 GetClassId() { return m_cls_id; }

    virtual void Dispose() { delete this; }

CLASS_ID:
    static const INT32 m_cls_id = MapGuide_Desktop_MappingService_MappingService;
};
/// \}
#endif