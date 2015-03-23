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

#ifndef MGSERVERRENDERINGSERVICE_H
#define MGSERVERRENDERINGSERVICE_H

#include "ServerRenderingDllExport.h"

class SE_Renderer;
class FeatureInfoRenderer;
class MgFeatureInformation;
class Stylizer;

namespace MdfModel
{
    class ProfileRenderMapResult;
}

class MG_SERVER_RENDERING_API MgServerRenderingService : public MgRenderingService
{
    DECLARE_CLASSNAME(MgServerRenderingService)

public:
    MgServerRenderingService();
    ~MgServerRenderingService();

    DECLARE_CREATE_SERVICE()

    void SetConnectionProperties(MgConnectionProperties* connProp);

    virtual MgByteReader* RenderTile(MgMap* map,
                                     CREFSTRING baseMapLayerGroupName,
                                     INT32 tileColumn,
                                     INT32 tileRow);

    virtual MgByteReader* RenderTile(MgMap* map,
                                     CREFSTRING baseMapLayerGroupName,
                                     INT32 tileColumn,
                                     INT32 tileRow,
                                     INT32 tileWidth,
                                     INT32 tileHeight,
                                     INT32 tileDpi,
                                     CREFSTRING tileImageFormat);

    virtual MgByteReader* RenderTileXYZ(MgMap* map,
                                        CREFSTRING baseMapLayerGroupName,
                                        INT32 x,
                                        INT32 y,
                                        INT32 z);

    virtual MgByteReader* RenderTileXYZ(MgMap* map,
                                        CREFSTRING baseMapLayerGroupName,
                                        INT32 x,
                                        INT32 y,
                                        INT32 z,
                                        INT32 dpi,
                                        CREFSTRING tileImageFormat);

    virtual MgByteReader* RenderDynamicOverlay(MgMap* map,
                                               MgSelection* selection,
                                               CREFSTRING format);

    virtual MgByteReader* RenderDynamicOverlay(MgMap* map,
                                               MgSelection* selection,
                                               CREFSTRING format,
                                               bool bKeepSelection);

    virtual MgByteReader* RenderDynamicOverlay(MgMap* map,
                                               MgSelection* selection,
                                               MgRenderingOptions* options);

    virtual MgByteReader* RenderDynamicOverlay(MgMap* map,
                                               MgSelection* selection,
                                               MgRenderingOptions* options,
                                               ProfileRenderMapResult* pPRMResult);

    virtual MgByteReader* RenderMap(MgMap* map,
                                    MgSelection* selection,
                                    CREFSTRING format);

    virtual MgByteReader* RenderMap(MgMap* map,
                                    MgSelection* selection,
                                    CREFSTRING format,
                                    bool bKeepSelection);

    virtual MgByteReader* RenderMap(MgMap* map,
                                    MgSelection* selection,
                                    CREFSTRING format,
                                    bool bKeepSelection,
                                    bool bClip);

    virtual MgByteReader* RenderMap(MgMap* map,
                                    MgSelection* selection,
                                    CREFSTRING format,
                                    bool bKeepSelection,
                                    bool bClip,
                                    MgColor* selectionColor);

    virtual MgByteReader* RenderMap(MgMap* map,
                                    MgSelection* selection,
                                    MgEnvelope* extents,
                                    INT32 width,
                                    INT32 height,
                                    MgColor* backgroundColor,
                                    CREFSTRING format);

    virtual MgByteReader* RenderMap(MgMap* map,
                                    MgSelection* selection,
                                    MgEnvelope* extents,
                                    INT32 width,
                                    INT32 height,
                                    MgColor* backgroundColor,
                                    CREFSTRING format,
                                    bool bKeepSelection);

    virtual MgByteReader* RenderMap(MgMap* map,
                                    MgSelection* selection,
                                    MgCoordinate* center,
                                    double scale,
                                    INT32 width,
                                    INT32 height,
                                    MgColor* backgroundColor,
                                    CREFSTRING format);

    virtual MgByteReader* RenderMap(MgMap* map,
                                    MgSelection* selection,
                                    MgCoordinate* center,
                                    double scale,
                                    INT32 width,
                                    INT32 height,
                                    MgColor* backgroundColor,
                                    CREFSTRING format,
                                    bool bKeepSelection);

    virtual MgByteReader* RenderMap(MgMap* map,
                                    MgSelection* selection,
                                    MgCoordinate* center,
                                    double scale,
                                    INT32 width,
                                    INT32 height,
                                    MgColor* backgroundColor,
                                    CREFSTRING format,
                                    bool bKeepSelection,
                                    MgColor* selectionColor);

    virtual MgByteReader* RenderMap(MgMap* map,
                                    MgSelection* selection,
                                    MgCoordinate* center,
                                    double scale,
                                    INT32 width,
                                    INT32 height,
                                    MgColor* backgroundColor,
                                    CREFSTRING format,
                                    bool bKeepSelection,
                                    ProfileRenderMapResult* pPRMResult);

    virtual MgByteReader* RenderMap(MgMap* map,
                                    MgSelection* selection,
                                    MgCoordinate* center,
                                    double scale,
                                    INT32 width,
                                    INT32 height,
                                    MgColor* backgroundColor,
                                    CREFSTRING format,
                                    bool bKeepSelection,
                                    bool bClip,
                                    MgColor* selectionColor,
                                    ProfileRenderMapResult* pPRMResult = NULL);

    virtual MgByteReader* RenderMapLegend(MgMap* map,
                                          INT32 width,
                                          INT32 height,
                                          MgColor* backgroundColor,
                                          CREFSTRING format);

    virtual MgFeatureInformation* QueryFeatures( MgMap* map,
                                        MgStringCollection* layerNames,
                                        MgGeometry* geometry,
                                        INT32 selectionVariant, // Within, Touching, Topmost
                                        INT32 maxFeatures);

    virtual MgFeatureInformation* QueryFeatures( MgMap* map,
                                        MgStringCollection* layerNames,
                                        MgGeometry* geometry,
                                        INT32 selectionVariant, // Within, Touching, Topmost
                                        CREFSTRING featureFilter,
                                        INT32 maxFeatures,
                                        INT32 layerAttributeFilter);

    virtual MgBatchPropertyCollection* QueryFeatureProperties( MgMap* map,
                                        MgStringCollection* layerNames,
                                        MgGeometry* filterGeometry,
                                        INT32 selectionVariant, // Within, Touching, Topmost
                                        INT32 maxFeatures);

    virtual MgBatchPropertyCollection* QueryFeatureProperties( MgMap* map,
                                        MgStringCollection* layerNames,
                                        MgGeometry* filterGeometry,
                                        INT32 selectionVariant, // Within, Touching, Topmost
                                        CREFSTRING featureFilter,
                                        INT32 maxFeatures,
                                        INT32 layerAttributeFilter);

    virtual MgBatchPropertyCollection* QueryFeatureProperties(
                                        MgMap* map,
                                        MgStringCollection* layerNames,
                                        MgGeometry* filterGeometry,
                                        INT32 selectionVariant,
                                        CREFSTRING featureFilter,
                                        INT32 maxFeatures,
                                        INT32 layerAttributeFilter,
                                        bool bIncludeFeatureBBOX);

private:
    // used for tile generation
    MgByteReader* RenderTile(MgMap* map,
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
    MgByteReader* RenderMapInternal(MgMap* map,
                                    MgSelection* selection,
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
                                    MgColor* selectionColor,
                                    ProfileRenderMapResult* pPRMResult = NULL);

    MgByteReader* RenderMapInternal(MgMap* map,
                                    MgSelection* selection,
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
                                    bool renderWatermark,
                                    ProfileRenderMapResult* pPRMResult = NULL);

    void RenderForSelection(MgMap* map,
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

    void RenderLayers(MgMap* map,
                      MgReadOnlyLayerCollection* layers,
                      Stylizer* ds,
                      Renderer* dr,
                      MgCoordinateSystem* dstCs,
                      bool expandExtents,
                      double scale,
                      CREFSTRING format,
                      ProfileRenderMapResult* pPRMResult);

    void RenderSelection(MgMap* map,
                         MgSelection* selection,
                         MgReadOnlyLayerCollection* layers,
                         MgRenderingOptions* options,
                         Stylizer* ds,
                         Renderer* dr,
                         MgCoordinateSystem* dstCs,
                         double scale,
                         INT32 behavior,
                         ProfileRenderMapResult* pPRMResult);

    void RenderWatermarks(MgMap* map,
                          MgReadOnlyLayerCollection* layers,
                          Stylizer* ds,
                          Renderer* dr,
                          int drawWidth,
                          int drawHeight,
                          INT32 saveWidth,
                          INT32 saveHeight,
                          ProfileRenderMapResult* pPRMResult);

    MgByteReader* CreateImage(MgMap* map,
                              Renderer* dr,
                              INT32 saveWidth,
                              INT32 saveHeight,
                              CREFSTRING format,
                              ProfileRenderMapResult* pPRMResult);

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
};

#endif
