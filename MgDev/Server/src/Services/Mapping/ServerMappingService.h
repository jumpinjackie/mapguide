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

#ifndef MG_SERVER_MAPPING_SERVICE_H
#define MG_SERVER_MAPPING_SERVICE_H

#include "ServerMappingDllExport.h"
#include <list>

namespace MdfModel
{
    class MapDefinition;
    class LayerDefinition;
    class VectorLayerDefinition;
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
class RS_UIGraphic;

class MG_SERVER_MAPPING_API MgServerMappingService : public MgMappingService
{
    DECLARE_CLASSNAME(MgServerMappingService)

// Constructors/Destructor

    public:
        MgServerMappingService();
        virtual ~MgServerMappingService();

        DECLARE_CREATE_SERVICE()

// Methods

    public:
        virtual MgByteReader* GenerateMap(MgMap* map, CREFSTRING sessionId,
            CREFSTRING mapAgentUri, MgDwfVersion* dwfVersion);
        virtual MgByteReader* GenerateMapUpdate(MgMap* map, INT32 seqNo,
            MgDwfVersion* dwfVersion);

        virtual MgByteReader* GeneratePlot(
            MgMap* map,
            MgPlotSpecification* plotSpec,
            MgLayout* layout,
            MgDwfVersion* dwfVersion);
        virtual MgByteReader* GeneratePlot(
            MgMap* map,
            MgCoordinate* center,
            double scale,
            MgPlotSpecification* plotSpec,
            MgLayout* layout,
            MgDwfVersion* dwfVersion);
        virtual MgByteReader* GeneratePlot(
            MgMap* map,
            MgEnvelope* extents,
            bool expandToFit,
            MgPlotSpecification* plotSpec,
            MgLayout* layout,
            MgDwfVersion* dwfVersion);

        virtual MgByteReader* GenerateMultiPlot(
            MgMapPlotCollection* mapPlots,
            MgDwfVersion* dwfVersion);
        virtual MgByteReader* GenerateLegendPlot(
            MgMap* map,
            double scale,
            MgPlotSpecification* plotSpec,
            MgDwfVersion* dwfVersion);

        virtual MgByteReader* QueryFeatures(MgMap* map, CREFSTRING layerName,
            CREFSTRING coordinateSpace);
        virtual MgByteReader* QueryFeatures(MgMap* map,
            MgStringCollection* layerNames, INT32 x, INT32 y,
            INT32 maxFeatures, CREFSTRING coordinateSpace);

        virtual MgByteReader* GenerateLegendImage(
            MgResourceIdentifier* resource,
            double scale,
            INT32 imgWidth,
            INT32 imgHeight,
            CREFSTRING format,
            INT32 geomType,
            INT32 themeCategory);

        void SetConnectionProperties(MgConnectionProperties* connProp);

// Data Members

    private:
        bool FeatureTypeStyleSupportsGeomType(MdfModel::FeatureTypeStyle* fts, INT32 geomType);
        void MakeUIGraphicsForScaleRange(std::list<RS_UIGraphic>& uiGraphics, std::vector<MgByte*>& uiGraphicSources, MdfModel::VectorScaleRange* sr);

        void InitializeFeatureService();
        void InitializeResourceService();
        void InitializeDrawingService();

        Ptr<MgFeatureService> m_svcFeature;
        Ptr<MgResourceService> m_svcResource;
        Ptr<MgDrawingService> m_svcDrawing;
        Ptr<MgCoordinateSystemFactory> m_pCSFactory;

        INT32 m_rasterGridSize;
        INT32 m_minRasterGridSize;
        double m_rasterGridSizeOverrideRatio;

        INT32 m_rasterGridSizeForPlot;
        INT32 m_minRasterGridSizeForPlot;
        double m_rasterGridSizeOverrideRatioForPlot;

        INT32 m_maxRasterImageWidth;
        INT32 m_maxRasterImageHeight;
};

#endif
