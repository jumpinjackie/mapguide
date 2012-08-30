#ifndef DESKTOP_MAPPING_UTIL_H
#define DESKTOP_MAPPING_UTIL_H

#include "MgDesktop.h"
#include "MapLayer/Layer.h"
#include "MapLayer/Map.h"
#include "RendererStyles.h"
#include "ProfileRenderLayersResult.h"

typedef bool (*CancelStylization)(void* userData);

//fwd declare
class MgResourceService;
class MgFeatureService;
class MgFeatureReader;
class MgdDrawingService;
class MgCoordinateSystemFactory;
class MgResourceIdentifier;
class MgReadOnlyLayerCollection;
class MgStringCollection;
class MgMapBase;
class MgCoordinateSystem;
class RSMgdFeatureReader;
class MgdTransformCache;
class SE_SymbolManager;
class Stylizer;
class Renderer;

struct RS_Bounds;

namespace MdfModel
{
    class SymbolDefinition;
    class CompoundSymbolDefinition;
    class SimpleSymbolDefinition;
    class FeatureTypeStyle;
    class VectorScaleRange;
    class VectorLayerDefinition;
    class GridLayerDefinition;
}

//Common stylization utility code -- used by both the mapping and rendering services
class MgdMappingUtil
{
public:
    static void StylizeLayers(MgResourceService* svcResource,
                              MgFeatureService* svcFeature,
                              MgdDrawingService* svcDrawing,
                              MgCoordinateSystemFactory* csFactory,
                              MgdMap* map,
                              MgReadOnlyLayerCollection* layers,
                              MgStringCollection* overrideFilters,
                              Stylizer* ds,
                              Renderer* dr,
                              MgCoordinateSystem* dstCs,
                              bool expandExtents,
                              bool checkRefreshFlag,
                              double scale,
                              bool selection = false,
                              bool extractColors = false,
							  ProfileRenderLayersResultBase* = NULL,
                              CancelStylization cancel = NULL);

    static RSMgdFeatureReader* ExecuteFeatureQuery(MgFeatureService* svcFeature,
                                                 RS_Bounds& extent,
                                                 MdfModel::VectorLayerDefinition* vl,
                                                 const wchar_t* overrideFilter,
                                                 MgCoordinateSystem* mapCs,
                                                 MgCoordinateSystem* layerCs,
                                                 MgdTransformCache* cache);

    static RSMgdFeatureReader* ExecuteRasterQuery(MgFeatureService* svcFeature,
                                                RS_Bounds& extent,
                                                MdfModel::GridLayerDefinition* gl,
                                                const wchar_t* overrideFilter,
                                                MgCoordinateSystem* mapCs,
                                                MgCoordinateSystem* layerCs,
                                                int width,
                                                int height);

    static MdfModel::MapDefinition* GetMapDefinition(MgResourceService* svcResource, MgResourceIdentifier* resId);

    static MgByteReader* DrawFTS(MgResourceService* svcResource, MdfModel::FeatureTypeStyle* fts, INT32 imgWidth, INT32 imgHeight, INT32 themeCategory);
    static double ComputeStylizationOffset(MgdMap* map, MdfModel::VectorScaleRange* scaleRange, double scale);

    static MgEnvelope* TransformExtent(MgEnvelope* extent, MgCoordinateSystemTransform* xform);

    static void InitializeStylizerCallback();
    static MgPolygon* GetPolygonFromEnvelope(MgEnvelope* extent);

    // RFC60 addition
    static void ExtractColors(MgdMap* map, MdfModel::VectorScaleRange* scaleRange, Stylizer* stylizer);
    static void GetUsedColorsFromScaleRange(ColorStringList& usedColorList, MdfModel::VectorScaleRange* scaleRange, SE_SymbolManager* sman);

    // helpers
    static void FindColorInSymDefHelper(ColorStringList& colorList, MdfModel::SymbolDefinition* symdef);
    static void FindColorInSymDefHelper(ColorStringList& colorList, MdfModel::CompoundSymbolDefinition* symdef);
    static void FindColorInSymDefHelper(ColorStringList& colorList, MdfModel::SimpleSymbolDefinition* symdef);

    // Parse the expressions collected from xml definitions of all layers.  The map
    // object has a list from all color entries found in the most recent layer stylization.
    // TODO: currently they are interpreted as ffffffff 32 bit RGBA string values.
    // The color palette passed to the renderer is a std::vector<RS_Color>
    static void ParseColorStrings(RS_ColorVector* tileColorPalette, MgdMap* map);
};


#endif