//
//  Copyright (C) 2004-2007 by Autodesk, Inc.
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

#ifndef STYLIZATIONUTIL_H
#define STYLIZATIONUTIL_H

#include "ServerMappingDllExport.h"
#include "MapGuideCommon.h"

//fwd declare
class MgResourceService;
class MgFeatureService;
class MgFeatureReader;
class MgDrawingService;
class MgCoordinateSystemFactory;
class MgResourceIdentifier;
class MgReadOnlyLayerCollection;
class MgStringCollection;
class MgMap;
class MgCoordinateSystem;
class MgCSTrans;

//Common stylization utility code -- used by both the mapping and rendering services
class MG_SERVER_MAPPING_API MgStylizationUtil
{
public:

    // Class to cache coordinate systems and transforms during processing.  Many layers
    // will use the same coordinate system so this is an effective way to reduce significant overhead.
    class TransformCache
    {
    public:
        TransformCache(MgCSTrans* transform, MgCoordinateSystem* coordinateSystem);
        virtual ~TransformCache();
        MgCSTrans* GetTransform();
        MgCoordinateSystem* GetCoordSys();
        void SetMgTransform(MgCoordinateSystemTransform* mgTransform);
        MgCoordinateSystemTransform* GetMgTransform();
        void SetEnvelope(MgEnvelope* extent);
        MgEnvelope* GetEnvelope();

    private:
        MgCSTrans* m_xform;
        Ptr<MgCoordinateSystem> m_coordSys;
        Ptr<MgCoordinateSystemTransform> m_transform;
        Ptr<MgEnvelope> m_envelope;
    };

    typedef std::map<STRING, TransformCache*> TransformCacheMap;

    static void StylizeLayers(MgResourceService* svcResource,
                              MgFeatureService* svcFeature,
                              MgDrawingService* svcDrawing,
                              MgCoordinateSystemFactory* csFactory,
                              MgMap* map,
                              MgReadOnlyLayerCollection* layers,
                              MgStringCollection* overrideFilters,
                              Stylizer* ds,
                              Renderer* dr,
                              MgCoordinateSystem* dstCs,
                              bool expandExtents,
                              bool checkRefreshFlag,
                              double scale);

    static MgFeatureReader * ExecuteFeatureQuery(MgFeatureService* svcFeature,
                                                 RS_Bounds& extent,
                                                 MdfModel::VectorLayerDefinition* vl,
                                                 const wchar_t* overrideFilter,
                                                 MgCoordinateSystem* mapCs,
                                                 MgCoordinateSystem* layerCs,
                                                 TransformCache* cache);

    static MgFeatureReader * ExecuteRasterQuery(MgFeatureService* svcFeature,
                                                RS_Bounds& extent,
                                                MdfModel::GridLayerDefinition* gl,
                                                const wchar_t* overrideFilter,
                                                MgCoordinateSystem* mapCs,
                                                MgCoordinateSystem* layerCs,
                                                int width,
                                                int height);


    static MdfModel::MapDefinition* GetMapDefinition(MgResourceService* svcResource, MgResourceIdentifier* resId);
    static MdfModel::LayerDefinition* GetLayerDefinition(MgResourceService* svcResource, MgResourceIdentifier* resId);
    static void ParseColor(CREFSTRING scolor, RS_Color& rscol);
    static double ParseDouble(CREFSTRING valstr);
    static bool ParseDouble(CREFSTRING valstr, double& val);

    static MgByteReader* DrawFTS(MgResourceService* svcResource, MdfModel::FeatureTypeStyle* fts, INT32 imgWidth, INT32 imgHeight, INT32 themeCategory);

    static double ComputeStylizationOffset(MgMap* map, MdfModel::VectorScaleRange* scaleRange, double scale);
};

#endif
