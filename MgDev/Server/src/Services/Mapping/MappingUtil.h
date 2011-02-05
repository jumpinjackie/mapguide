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

#ifndef MAPPINGUTIL_H
#define MAPPINGUTIL_H

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
class RSMgFeatureReader;
class TransformCache;
class SE_SymbolManager;
namespace MdfModel
{
    class SymbolDefinition;
    class CompoundSymbolDefinition;
    class SimpleSymbolDefinition;
}

//Common stylization utility code -- used by both the mapping and rendering services
class MG_SERVER_MAPPING_API MgMappingUtil
{
public:
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
                              double scale,
                              bool selection = false,
                              bool extractColors = false);

    static RSMgFeatureReader* ExecuteFeatureQuery(MgFeatureService* svcFeature,
                                                 RS_Bounds& extent,
                                                 MdfModel::VectorLayerDefinition* vl,
                                                 const wchar_t* overrideFilter,
                                                 MgCoordinateSystem* mapCs,
                                                 MgCoordinateSystem* layerCs,
                                                 TransformCache* cache);

    static RSMgFeatureReader* ExecuteRasterQuery(MgFeatureService* svcFeature,
                                                RS_Bounds& extent,
                                                MdfModel::GridLayerDefinition* gl,
                                                const wchar_t* overrideFilter,
                                                MgCoordinateSystem* mapCs,
                                                MgCoordinateSystem* layerCs,
                                                int width,
                                                int height);

    static MdfModel::MapDefinition* GetMapDefinition(MgResourceService* svcResource, MgResourceIdentifier* resId);

    static MgByteReader* DrawFTS(MgResourceService* svcResource, MdfModel::FeatureTypeStyle* fts, INT32 imgWidth, INT32 imgHeight, INT32 themeCategory);
    static double ComputeStylizationOffset(MgMap* map, MdfModel::VectorScaleRange* scaleRange, double scale);

    static MgEnvelope* TransformExtent(MgEnvelope* extent, MgCoordinateSystemTransform* xform);

    static void InitializeStylizerCallback();
    static MgPolygon* GetPolygonFromEnvelope(MgEnvelope* extent);

    // RFC60 addition
    static void ExtractColors(MgMap* map, MdfModel::VectorScaleRange* scaleRange, Stylizer* stylizer);
    static void GetUsedColorsFromScaleRange(ColorStringList& usedColorList, MdfModel::VectorScaleRange* scaleRange, SE_SymbolManager* sman);

    // helpers
    static void FindColorInSymDefHelper(ColorStringList& colorList, MdfModel::SymbolDefinition* symdef);
    static void FindColorInSymDefHelper(ColorStringList& colorList, MdfModel::CompoundSymbolDefinition* symdef);
    static void FindColorInSymDefHelper(ColorStringList& colorList, MdfModel::SimpleSymbolDefinition* symdef);

    // Parse the expressions collected from xml definitions of all layers.  The map
    // object has a list from all color entries found in the most recent layer stylization.
    // TODO: currently they are interpreted as ffffffff 32 bit RGBA string values.
    // The color palette passed to the renderer is a std::vector<RS_Color>
    static void ParseColorStrings(RS_ColorVector* tileColorPalette, MgMap* map);
};

#endif
