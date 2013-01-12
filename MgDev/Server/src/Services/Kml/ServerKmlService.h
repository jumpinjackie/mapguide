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

#ifndef MGSERVERKMLSERVICE_H
#define MGSERVERKMLSERVICE_H

#include "ServerKmlDllExport.h"

namespace MdfModel
{
    class MapDefinition;
    class LayerDefinition;
    class VectorLayerDefinition;
    class DrawingLayerDefinition;
    class GridLayerDefinition;
    class FeatureTypeStyle;
    class VectorScaleRange;
}
class FdoFilter;
struct RS_Bounds;
class MgFeatureService;
class MgCoordinateSystem;
class MgCoordinateSystemFactory;
class MgResourceService;
class MgDrawingService;
class MgLayerCollection;
class MgFeatureReader;
class Stylizer;
class Renderer;
class CSysToCSysTrans;

class EPlotRenderer;
class RS_LineStroke;
class RS_Color;
class MgXmlUtil;
class MgPrintLayout;
class RS_TextDef;

class KmlContent;

class MG_SERVER_KML_API MgServerKmlService : public MgKmlService
{
    DECLARE_CLASSNAME(MgServerKmlService)

public:
    MgServerKmlService();
    ~MgServerKmlService();

    DECLARE_CREATE_SERVICE()

    void SetConnectionProperties(MgConnectionProperties* connProp);

    virtual MgByteReader* GetMapKml(MgMap* map, double dpi, CREFSTRING agentUri, CREFSTRING format);

    virtual MgByteReader* GetLayerKml(MgLayer* layer, MgEnvelope* extents, INT32 width, INT32 height, double dpi, INT32 drawOrder, CREFSTRING agentUri, CREFSTRING format);

    virtual MgByteReader* GetFeaturesKml(MgLayer* layer, MgEnvelope* extents, INT32 width, INT32 height, double dpi, INT32 drawOrder, CREFSTRING format);

    virtual MgByteReader* GetFeaturesKml(MgLayer* layer, MgEnvelope* extents, INT32 width, INT32 height, double dpi, INT32 drawOrder, CREFSTRING agentUri, CREFSTRING format);

private:

    void AppendLayer(MgLayer* layer,
        MgEnvelope* extent,
        INT32 drawOrder,
        CREFSTRING agentUri,
        CREFSTRING format,
        CREFSTRING sessionId,
        KmlContent& kmlContent);

    void AppendScaleRange(MgLayer* layer,
        MgEnvelope* extent,
        CREFSTRING agentUri,
        double dimension,
        double minScale,
        double maxScale,
        double dpi,
        INT32 drawOrder,
        CREFSTRING format,
        CREFSTRING sessionId,
        KmlContent& kmlContent);

    void AppendRasterScaleRange(MgLayer* layer,
        MgEnvelope* extent,
        CREFSTRING agentUri,
        double dimension,
        double minScale,
        double maxScale,
        double dpi,
        INT32 drawOrder,
        CREFSTRING format,
        CREFSTRING sessionId,
        KmlContent& kmlContent);

    void AppendFeatures(MgLayer* layer,
        MdfModel::LayerDefinition* layerDef,
        MgEnvelope* extents,
        double scale,
        double dpi,
        INT32 drawOrder,
        KmlContent& kmlContent);

    void AppendFeatures(MgLayer* layer,
        MdfModel::LayerDefinition* layerDef,
        MgEnvelope* extents,
        CREFSTRING agentUri,
        double scale,
        double dpi,
        INT32 drawOrder,
        CREFSTRING sessionId,
        KmlContent& kmlContent);

    MgByteSource* GetByteSource(KmlContent& kmlContent, CREFSTRING format);

    void InitializeResourceService();
    void InitializeFeatureService();
    void InitializeDrawingService();
    void InitializeRenderingService();

    void WriteRegion(MgEnvelope* extent, KmlContent& kmlContent, double dpi, double dimension = 0, double minScale = 0, double maxScale = 0);

    MgCoordinateSystem* GetCoordinateSystem(MgResourceIdentifier* featureSourceResId);

    STRING ReadElement(STRING input, STRING elementName, size_t& offset);
    MgEnvelope* GetLayerExtent(MdfModel::LayerDefinition* layerDef, MgCoordinateSystem* destCs);
    double GetScale(MgEnvelope* llExtents, int width, int height, double dpi);
    STRING GetSessionId();

    STRING GetPointStyleImageUrl(STRING agentUri, MgLayer* layer, double scale, CREFSTRING sessionId);

    Ptr<MgResourceService> m_svcResource;
    Ptr<MgFeatureService> m_svcFeature;
    Ptr<MgDrawingService> m_svcDrawing;
    Ptr<MgRenderingService> m_svcRendering;
    Ptr<MgCoordinateSystemFactory> m_csFactory;
};

#endif
