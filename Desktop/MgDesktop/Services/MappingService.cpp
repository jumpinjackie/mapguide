#include "MappingService.h"
#include "EPlotRenderer.h"
#include "FeatureTypeStyleVisitor.h"
#include "Rendering/RSMgSymbolManager.h"
#include "Rendering/icons.h"
#include "Rendering/LegendPlotUtil.h"
#include "StylizationUtil.h"
#include "DefaultStylizer.h"
#include "Stylization/SEMgSymbolManager.h"

///////////////////////////////////////////////////////////////////////////////
/// Mg Server Mapping Service try/catch/throw macros.
///
#define MG_SERVER_MAPPING_SERVICE_TRY()                                       \
    MG_TRY()                                                                  \

// For the Mapping service, gracefully handle and remap any Fdo exceptions to MapGuide exceptions.
// Some Fdo providiers like WMS return useful error strings when a request fails.
// Without the remap, this information is lost and MapGuide returns an unclassified exception.
#define MG_SERVER_MAPPING_SERVICE_CATCH(methodName)                         \
    }                                                                       \
    catch (MgException* e)                                                  \
                                                                            \
    {                                                                       \
        mgException = e;                                                    \
        mgException->AddStackTraceInfo(methodName, __LINE__, __WFILE__);    \
    }                                                                       \
    catch (exception& e)                                                    \
    {                                                                       \
        mgException = MgSystemException::Create(e, methodName, __LINE__, __WFILE__); \
    }                                                                       \
    catch (FdoException* e)                                                 \
    {                                                                       \
        STRING messageId;                                                   \
        MgStringCollection arguments;                                       \
        wchar_t* buf = (wchar_t*)e->GetExceptionMessage();                  \
        INT64 nativeErrorCode = e->GetNativeErrorCode();                    \
        if (NULL != buf)                                                    \
        {                                                                   \
            messageId = L"MgFormatInnerExceptionMessage";                   \
            arguments.Add(buf);                                             \
        }                                                                   \
        FDO_SAFE_RELEASE(e);                                                \
        mgException = new MgFdoException(methodName, __LINE__, __WFILE__, NULL, messageId, &arguments, nativeErrorCode); \
    }                                                                       \
    catch (...)                                                             \
    {                                                                       \
        mgException = new MgUnclassifiedException(methodName, __LINE__, __WFILE__, NULL, L"", NULL); \
    }                                                                       \

#define MG_SERVER_MAPPING_SERVICE_THROW()                                     \
    MG_THROW()                                                                \

#define MG_SERVER_MAPPING_SERVICE_CATCH_AND_THROW(methodName)                 \
    MG_SERVER_MAPPING_SERVICE_CATCH(methodName)                               \
                                                                              \
    MG_SERVER_MAPPING_SERVICE_THROW()                                         \


MgdMappingService::MgdMappingService() : MgService() 
{ 
    m_pCSFactory = new MgCoordinateSystemFactory();

    // store references to the various services we use
    //MgServiceManager* serviceMan = MgServiceManager::GetInstance();
    //assert(NULL != serviceMan);

    Ptr<MgdServiceFactory> fact = new MgdServiceFactory();

    m_svcResource = dynamic_cast<MgResourceService*>(fact->CreateService(MgServiceType::ResourceService));
    assert(m_svcResource != NULL);

    m_svcFeature = dynamic_cast<MgFeatureService*>(fact->CreateService(MgServiceType::FeatureService));
    assert(m_svcFeature != NULL);

    m_svcDrawing = dynamic_cast<MgdDrawingService*>(fact->CreateService(MgServiceType::DrawingService));
    assert(m_svcDrawing != NULL);

    MgConfiguration* pConf = MgConfiguration::GetInstance();
    pConf->GetIntValue(MgdConfigProperties::RenderingServicePropertiesSection,
                          MgdConfigProperties::RenderingServicePropertyMaxRasterImageWidth,
                          m_maxRasterImageWidth,
                          MgdConfigProperties::DefaultRenderingServicePropertyMaxRasterImageWidth);

    pConf->GetIntValue(MgdConfigProperties::RenderingServicePropertiesSection,
                          MgdConfigProperties::RenderingServicePropertyMaxRasterImageHeight,
                          m_maxRasterImageHeight,
                          MgdConfigProperties::DefaultRenderingServicePropertyMaxRasterImageHeight);

    // Set Mapping Service related properties
	pConf->GetIntValue(MgdConfigProperties::RenderingServicePropertiesSection,
                          MgdConfigProperties::RenderingServicePropertyRasterGridSizeForPlot,
                          m_rasterGridSizeForPlot,
                          MgdConfigProperties::DefaultRenderingServicePropertyRasterGridSizeForPlot);

    pConf->GetIntValue(MgdConfigProperties::RenderingServicePropertiesSection,
                          MgdConfigProperties::RenderingServicePropertyMinRasterGridSizeForPlot,
                          m_minRasterGridSizeForPlot,
                          MgdConfigProperties::DefaultRenderingServicePropertyMinRasterGridSizeForPlot);

    pConf->GetDoubleValue(MgdConfigProperties::RenderingServicePropertiesSection,
                          MgdConfigProperties::RenderingServicePropertyRasterGridSizeOverrideRatioForPlot,
                          m_rasterGridSizeOverrideRatioForPlot,
                          MgdConfigProperties::DefaultRenderingServicePropertyRasterGridSizeOverrideRatioForPlot);
}

MgdMappingService::~MgdMappingService() { }

// Returns true if the supplied feature type style is compatible with the
// supplied geometry type: 1=Point, 2=Line, 3=Area, 4=Composite
bool MgdMappingService::FeatureTypeStyleSupportsGeomType(MdfModel::FeatureTypeStyle* fts, INT32 geomType)
{
    if (fts == NULL)
        return false;

    // if ShowInLegend is false, the legend doesn't need to be shown
    if(!fts->IsShowInLegend())
        return false;

    // a value of -1 matches any
    if (geomType == -1)
        return true;

    FeatureTypeStyleVisitor::eFeatureTypeStyle ftsType = FeatureTypeStyleVisitor::DetermineFeatureTypeStyle(fts);

    switch (ftsType)
    {
        case FeatureTypeStyleVisitor::ftsPoint:
            return (geomType == 1);

        case FeatureTypeStyleVisitor::ftsLine:
            return (geomType == 2);

        case FeatureTypeStyleVisitor::ftsArea:
            return (geomType == 3);

        case FeatureTypeStyleVisitor::ftsComposite:
            return (geomType == 4);
    }

    return false;
}

MgByteReader* MgdMappingService::GenerateLegendImage(MgResourceIdentifier* resource,
                                                      double     scale,
                                                      INT32      imgWidth,
                                                      INT32      imgHeight,
                                                      CREFSTRING format,
                                                      INT32      geomType,
                                                      INT32      themeCategory)
{
    Ptr<MgByteReader> byteReader;

    MG_LOG_OPERATION_MESSAGE(L"GenerateLegendImage");

    MG_SERVER_MAPPING_SERVICE_TRY()

    MG_LOG_OPERATION_MESSAGE_INIT(MG_API_VERSION(1, 0, 0), 7);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == resource) ? L"MgResourceIdentifier" : resource->ToString().c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_DOUBLE(scale);
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_INT32(imgWidth);
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_INT32(imgHeight);
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(format.c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_INT32(geomType);
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_INT32(themeCategory);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

    MG_LOG_TRACE_ENTRY(L"MgdMappingService::GenerateLegendImage()");

    if (0 == resource)
    {
        throw new MgNullArgumentException(
            L"MgdMappingService::GenerateLegendImage", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    if (m_svcResource == NULL)
    {
        Ptr<MgdServiceFactory> fact = new MgdServiceFactory();
        m_svcResource = static_cast<MgResourceService*>(fact->CreateService(MgServiceType::ResourceService));
    }

    auto_ptr<MdfModel::LayerDefinition> ldf(MgLayerBase::GetLayerDefinition(m_svcResource, resource));
    MdfModel::VectorLayerDefinition* vl = dynamic_cast<MdfModel::VectorLayerDefinition*>(ldf.get());
    MdfModel::DrawingLayerDefinition* dl = dynamic_cast<MdfModel::DrawingLayerDefinition*>(ldf.get());
    MdfModel::GridLayerDefinition* gl = dynamic_cast<MdfModel::GridLayerDefinition*>(ldf.get());

    if (vl) // vector layer
    {
        MdfModel::VectorScaleRangeCollection* src = vl->GetScaleRanges();

        MdfModel::VectorScaleRange* range = NULL;

        //find the right scale range
        for (int i=0; i<src->GetCount(); i++)
        {
            MdfModel::VectorScaleRange* r = src->GetAt(i);

            if (scale >= r->GetMinScale() && scale < r->GetMaxScale())
            {
                range = r;
                break;
            }
        }

        //we have the scale range... find the feature style
        if (range)
        {
            MdfModel::FeatureTypeStyle* fts = NULL;

            // The "geometry type" specifies which flavor of feature type style to use,
            // while the theme category is a 0-based value which specifies the actual
            // type style to use.  So if I have a scale range with two composite type
            // styles, the first with 3 rules and the second with 2 rules, then the
            // theme category can range from 0 to 4: 0,1,2 refer to the 3 rules in the
            // 1st type style, while 3,4 refer to the 2 rules in the 2nd type style.
            MdfModel::FeatureTypeStyleCollection* ftsc = range->GetFeatureTypeStyles();
            int numFTS = ftsc->GetCount();
            for (int j=0; j<numFTS; ++j)
            {
                // filter the type styles
                MdfModel::FeatureTypeStyle* temp = ftsc->GetAt(j);
                if (!FeatureTypeStyleSupportsGeomType(temp, geomType))
                    continue;

                // make sure we have rules
                RuleCollection* rules = temp->GetRules();
                if (!rules)
                    continue;

                // check if the theme category refers to one of these rules (note: a
                // value of -1 matches anything)
                int numRules = rules->GetCount();
                if (themeCategory >= -1 && themeCategory < numRules)
                {
                    // found it
                    fts = temp;
                    break;
                }

                // adjust the theme category, and move to the next type style
                themeCategory -= numRules;
            }

            if (fts)
                byteReader = MgdMappingUtil::DrawFTS(m_svcResource, fts, imgWidth, imgHeight, themeCategory);
            else
            {
                //return the fixed array
                //MgByteSource will make its own copy of the data
                Ptr<MgByteSource> src = new MgByteSource((BYTE_ARRAY_IN)BLANK_LAYER_ICON, sizeof(BLANK_LAYER_ICON));
                byteReader = src->GetReader();
            }
        }
    }
    else if (dl) // drawing layer
    {
        //return the fixed array
        //MgByteSource will make its own copy of the data
        Ptr<MgByteSource> src = new MgByteSource((BYTE_ARRAY_IN)DWF_LAYER_ICON, sizeof(DWF_LAYER_ICON));
        byteReader = src->GetReader();
    }
    else if (gl) //grid (raster) layer
    {
        //return the fixed array
        //MgByteSource will make its own copy of the data
        Ptr<MgByteSource> src = new MgByteSource((BYTE_ARRAY_IN)RASTER_LAYER_ICON, sizeof(RASTER_LAYER_ICON));
        byteReader = src->GetReader();
    }

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_SERVER_MAPPING_SERVICE_CATCH(L"MgdMappingService::GenerateLegendImage")

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
        MG_DESKTOP_LOG_EXCEPTION();
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_SERVER_MAPPING_SERVICE_THROW()

    return byteReader.Detach();
}

MgByteReader* MgdMappingService::GeneratePlot(MgdMap* map,
                                               MgdPlotSpecification* plotSpec,
                                               MgdLayout* layout,
                                               MgdDwfVersion* dwfVersion)
{
    Ptr<MgByteReader> byteReader;

    MG_LOG_OPERATION_MESSAGE(L"GeneratePlot");

    MG_SERVER_MAPPING_SERVICE_TRY()

    Ptr<MgResourceIdentifier> mapId;
    if (NULL != map)
        mapId = map->GetResourceId();
    Ptr<MgResourceIdentifier> layoutId;
    if (NULL != layoutId)
        layoutId = layout->GetLayout();
    MG_LOG_OPERATION_MESSAGE_INIT(MG_API_VERSION(1, 0, 0), 4);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == mapId) ? L"MgResourceIdentifier" : mapId->ToString().c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(L"MgdPlotSpecification");
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == layoutId) ? L"MgResourceIdentifier" : layoutId->ToString().c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == dwfVersion) ? L"MgdDwfVersion" : dwfVersion->GetLogString());
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

    MG_LOG_TRACE_ENTRY(L"MgdMappingService::GeneratePlot()");

    if (NULL == map  || NULL == dwfVersion || NULL == plotSpec )
    {
        throw new MgNullArgumentException(
            L"MgdMappingService::GeneratePlot", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // Create a MgdMapPlot which will be passed to GenerateMultiPlot
    Ptr<MgdMapPlot> mapPlot = new MgdMapPlot(map, plotSpec, layout);

    // Add it to a MgdMapPlotCollecion
    Ptr<MgdMapPlotCollection> mapPlots = new MgdMapPlotCollection();
    mapPlots->Add(mapPlot);

    // Create the plot
    byteReader = GenerateMultiPlotInternal(mapPlots, dwfVersion);

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_SERVER_MAPPING_SERVICE_CATCH(L"MgdMappingService::GeneratePlot")

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
        MG_DESKTOP_LOG_EXCEPTION();
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_SERVER_MAPPING_SERVICE_THROW()

    return byteReader.Detach();
}

MgByteReader* MgdMappingService::GeneratePlot(MgdMap* map,
                                               MgCoordinate* center,
                                               double scale,
                                               MgdPlotSpecification* plotSpec,
                                               MgdLayout* layout,
                                               MgdDwfVersion* dwfVersion)
{
    Ptr<MgByteReader> byteReader;

    MG_LOG_OPERATION_MESSAGE(L"GeneratePlot");

    MG_SERVER_MAPPING_SERVICE_TRY()

    Ptr<MgResourceIdentifier> mapId;
    if (NULL != map)
        mapId = map->GetResourceId();
    Ptr<MgResourceIdentifier> layoutId;
    if (NULL != layoutId)
        layoutId = layout->GetLayout();
    MG_LOG_OPERATION_MESSAGE_INIT(MG_API_VERSION(1, 0, 0), 6);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == mapId) ? L"MgResourceIdentifier" : mapId->ToString().c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(L"MgCoordinate")
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_DOUBLE(scale)
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(L"MgdPlotSpecification");
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == layoutId) ? L"MgResourceIdentifier" : layoutId->ToString().c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == dwfVersion) ? L"MgdDwfVersion" : dwfVersion->GetLogString());
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

    MG_LOG_TRACE_ENTRY(L"MgdMappingService::GeneratePlot()");

    if (NULL == map  || NULL == center || NULL == dwfVersion || NULL == plotSpec )
    {
        throw new MgNullArgumentException(
            L"MgdMappingService::GeneratePlot", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // Create a MgdMapPlot which will be passed to GenerateMultiPlot
    Ptr<MgdMapPlot> mapPlot = new MgdMapPlot(map, center, scale, plotSpec, layout);

    // Add it to a MgdMapPlotCollecion
    Ptr<MgdMapPlotCollection> mapPlots = new MgdMapPlotCollection();
    mapPlots->Add(mapPlot);

    // Create the plot
    byteReader = GenerateMultiPlotInternal(mapPlots, dwfVersion);

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_SERVER_MAPPING_SERVICE_CATCH(L"MgdMappingService::GeneratePlot")

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
        MG_DESKTOP_LOG_EXCEPTION();
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_SERVER_MAPPING_SERVICE_THROW()

    return byteReader.Detach();
}

MgByteReader* MgdMappingService::GeneratePlot(MgdMap* map,
                                               MgEnvelope* extents,
                                               bool expandToFit,
                                               MgdPlotSpecification* plotSpec,
                                               MgdLayout* layout,
                                               MgdDwfVersion* dwfVersion)
{
    Ptr<MgByteReader> byteReader;

    MG_LOG_OPERATION_MESSAGE(L"GeneratePlot");
    MG_SERVER_MAPPING_SERVICE_TRY()

    Ptr<MgResourceIdentifier> mapId;
    if (NULL != map)
        mapId = map->GetResourceId();
    Ptr<MgResourceIdentifier> layoutId;
    if (NULL != layoutId)
        layoutId = layout->GetLayout();

    MG_LOG_OPERATION_MESSAGE_INIT(MG_API_VERSION(1, 0, 0), 6);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == mapId) ? L"MgResourceIdentifier" : mapId->ToString().c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(L"MgEnvelope")
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_BOOL(expandToFit)
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(L"MgdPlotSpecification");
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == layoutId) ? L"MgResourceIdentifier" : layoutId->ToString().c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == dwfVersion) ? L"MgdDwfVersion" : dwfVersion->GetLogString());
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

    MG_LOG_TRACE_ENTRY(L"MgdMappingService::GeneratePlot()");

    if (NULL == map  || NULL == extents || NULL == plotSpec || NULL == dwfVersion )
    {
        throw new MgNullArgumentException(
            L"MgdMappingService::GeneratePlot", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // Make a copy of the extents
    Ptr<MgCoordinate> oldll = extents->GetLowerLeftCoordinate();
    Ptr<MgCoordinate> oldur = extents->GetUpperRightCoordinate();
    Ptr<MgCoordinate> ll = new MgCoordinateXY(oldll->GetX(), oldll->GetY());
    Ptr<MgCoordinate> ur = new MgCoordinateXY(oldur->GetX(), oldur->GetY());
    if (ll == NULL || ur == NULL)
    {
        throw new MgNullArgumentException(L"MgdMappingService::GeneratePlot", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    Ptr<MgEnvelope> env = new MgEnvelope(ll, ur);

    // Create a MgdMapPlot which will be passed to GenerateMultiPlot
    Ptr<MgdMapPlot> mapPlot = new MgdMapPlot(map, env, expandToFit, plotSpec, layout);

    // Add it to a MgdMapPlotCollecion
    Ptr<MgdMapPlotCollection> mapPlots = new MgdMapPlotCollection();
    if (mapPlot == NULL || mapPlots == NULL)
    {
        throw new MgNullArgumentException(L"MgdMappingService::GeneratePlot", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    mapPlots->Add(mapPlot);

    // Create the plot
    byteReader = GenerateMultiPlotInternal(mapPlots, dwfVersion);

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_SERVER_MAPPING_SERVICE_CATCH(L"MgdMappingService::GeneratePlot")

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
        MG_DESKTOP_LOG_EXCEPTION();
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_SERVER_MAPPING_SERVICE_THROW()

    return byteReader.Detach();
}

MgByteReader* MgdMappingService::GenerateMultiPlot(MgdMapPlotCollection* mapPlots, MgdDwfVersion* dwfVersion)
{
    Ptr<MgByteReader> ret;
    MG_LOG_OPERATION_MESSAGE(L"GenerateMultiPlot");

    MG_SERVER_MAPPING_SERVICE_TRY()

    MG_LOG_OPERATION_MESSAGE_INIT(MG_API_VERSION(1, 0, 0), 2);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(L"MgdMapPlotCollection");
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == dwfVersion) ? L"MgdDwfVersion" : dwfVersion->GetLogString().c_str());
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

    MG_LOG_TRACE_ENTRY(L"MgdMappingService::GenerateMultiPlot()");

    ret = GenerateMultiPlotInternal(mapPlots, dwfVersion);

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_SERVER_MAPPING_SERVICE_CATCH(L"MgdMappingService::GenerateMultiPlot")

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
        MG_DESKTOP_LOG_EXCEPTION();
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_SERVER_MAPPING_SERVICE_THROW()
    return ret.Detach();
}

MgByteReader* MgdMappingService::GenerateMultiPlotInternal(MgdMapPlotCollection* mapPlots, MgdDwfVersion* dwfVersion)
{
    Ptr<MgByteReader> byteReader;

    MG_SERVER_MAPPING_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgdMappingService::GenerateMultiPlot()");

    if (0 == mapPlots || 0 == dwfVersion)
    {
        throw new MgNullArgumentException(
            L"MgdMappingService::GenerateMultiPlot", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // get a temporary file to write out EPlot DWF to
    // TODO: clean the temp file name prefix
    STRING dwfName = MgFileUtil::GenerateTempFileName(false, L"default_prefix");

    EPlotRenderer dr(dwfName.c_str(), 0, 0, L"inches");  // NOXLATE

    dr.SetRasterGridSize(m_rasterGridSizeForPlot);
    dr.SetMinRasterGridSize(m_minRasterGridSizeForPlot);
    dr.SetRasterGridSizeOverrideRatio(m_rasterGridSizeOverrideRatioForPlot);
    dr.SetMaxRasterImageWidth(m_maxRasterImageWidth);
    dr.SetMaxRasterImageHeight(m_maxRasterImageHeight);

    RSMgdSymbolManager mgr(m_svcResource);
    dr.SetSymbolManager(&mgr);

    // process the MapPlot collection
    for (int nMapPlotIndex = 0; nMapPlotIndex < mapPlots->GetCount(); nMapPlotIndex++)
    {
        Ptr<MgdMapPlot> mapPlot = mapPlots->GetItem(nMapPlotIndex);
        if (NULL == mapPlot)
        {
            throw new MgNullReferenceException(L"MgdMappingService::GenerateMultiPlot", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgdMap> map = mapPlot->GetMap();
        if (NULL == map)
        {
            throw new MgNullReferenceException(L"MgdMappingService::GenerateMultiPlot", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgdPlotSpecification> plotSpec = mapPlot->GetPlotSpecification();
        if (NULL == plotSpec)
        {
            throw new MgNullReferenceException(L"MgdMappingService::GenerateMultiPlot", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgdLayout> layout = mapPlot->GetLayout();

        double width = plotSpec->GetPaperWidth();
        double height = plotSpec->GetPaperHeight();
        STRING pageUnits = plotSpec->GetPageSizeUnits();

        dr.SetPageWidth(width);
        dr.SetPageHeight(height);
        dr.SetPageSizeUnits(pageUnits);
        dr.SetMapWidth(width);
        dr.SetMapHeight(height);
        dr.mapBoundsHeight() = height;

        // temporary place for the resId of the Map
        Ptr<MgResourceIdentifier> mapResId = map->GetMapDefinition();

        // request extenst
        Ptr<MgEnvelope> env = map->GetDataExtent();
        if (env == NULL)
        {
            throw new MgNullReferenceException(L"MgdMappingService::GenerateMultiPlot", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgCoordinate> ll = env->GetLowerLeftCoordinate();
        Ptr<MgCoordinate> ur = env->GetUpperRightCoordinate();
        if (ll == NULL || ur == NULL)
        {
            throw new MgNullReferenceException(L"MgdMappingService::GenerateMultiPlot", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        RS_Bounds b(ll->GetX(), ll->GetY(), ur->GetX(), ur->GetY());

        //if requested data extent is not valid, use map definition extent
        // this may be removed eventually, but is a good sanity check nonetheless
        if (!b.IsValid())
        {
            Ptr<MgEnvelope> env2 = map->GetMapExtent();
            if (env2 == NULL)
            {
                throw new MgNullReferenceException(L"MgdMappingService::GenerateMultiPlot", __LINE__, __WFILE__, NULL, L"", NULL);
            }

            Ptr<MgCoordinate> ll2 = env2->GetLowerLeftCoordinate();
            Ptr<MgCoordinate> ur2 = env2->GetUpperRightCoordinate();
            if (ll2 == NULL || ur2 == NULL)
            {
                throw new MgNullReferenceException(L"MgdMappingService::GenerateMultiPlot", __LINE__, __WFILE__, NULL, L"", NULL);
            }

            b.minx = ll2->GetX();
            b.miny = ll2->GetY();
            b.maxx = ur2->GetX();
            b.maxy = ur2->GetY();
        }

        // Create a simple print layout containing only the map
        Ptr<MgdPrintLayout> printLayout = new MgdPrintLayout();
        if (printLayout == NULL)
        {
            throw new MgNullReferenceException(L"MgdMappingService::GenerateMultiPlot", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        if (NULL != layout)
        {
            Ptr<MgResourceIdentifier> resID = layout->GetLayout();
            printLayout->Create(m_svcResource, resID);
            printLayout->SetPlotSize(plotSpec);
            printLayout->SetScaleBarUnits(layout->GetUnitType());
            printLayout->SetPlotTitle(layout->GetTitle());

            Ptr<MgdPlotSpecification> spec = printLayout->GetPlotSize();

            width = spec->GetPaperWidth();
            height = spec->GetPaperHeight();
            pageUnits = spec->GetPageSizeUnits();
        }
        else
        {
            printLayout->SetPlotSize(plotSpec);
        }

        printLayout->PageWidth() = width;
        printLayout->PageHeight() = height;
        printLayout->Units() = pageUnits;

        dr.EnableLayoutPlot();

        //get the map coordinate system
        MdfModel::MdfString srs = map->GetMapSRS();
        Ptr<MgCoordinateSystem> dstCs;
        if (!srs.empty())
        {
            //let's not fail here if coord sys conversion fails for the
            //map's coordinate system. Instead we will fail per layer at a later stage
            try
            {
                dstCs = m_pCSFactory->Create(srs);
            }
            catch (MgInvalidCoordinateSystemException* e)
            {
                e->Release();
            }
        }

        double metersPerUnit = (dstCs.p) ? dstCs->ConvertCoordinateSystemUnitsToMeters(1.0) : 1.0;
        RS_String units = (dstCs.p) ? dstCs->GetUnits() : L"";

        double dMapScale = 0.0;
        Ptr<MgCoordinate> center = new MgCoordinateXY(0, 0);
        Ptr<MgEnvelope> extents = map->GetMapExtent();
        if (center == NULL || extents == NULL)
        {
            throw new MgNullReferenceException(L"MgdMappingService::GenerateMultiPlot", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        switch (mapPlot->GetMapPlotInstruction())
        {
        case MgdMapPlotInstruction::UseMapCenterAndScale:
            {
                dMapScale = map->GetViewScale();
                if (dMapScale <= 0)
                {
                    Ptr<MgEnvelope> extents = map->GetDataExtent();
                    if (extents == NULL)
                    {
                        throw new MgNullReferenceException(L"MgdMappingService::GenerateMultiPlot", __LINE__, __WFILE__, NULL, L"", NULL);
                    }
                    printLayout->ComputeMapOffsetAndSize(dMapScale, extents, metersPerUnit, dr.mapOffsetX(), dr.mapOffsetY(), dr.mapWidth(), dr.mapHeight());
                    double mapWidth = dr.mapWidth();
                    double mapHeight = dr.mapHeight();
                    if (_wcsicmp(pageUnits.c_str(), L"mm") == 0 || _wcsicmp(pageUnits.c_str(), L"millimeters") == 0) // NOXLATE
                    {
                        mapWidth *= MM_TO_IN;
                        mapHeight *= MM_TO_IN;
                    }

                    double scaleToFitX = (extents->GetWidth() * metersPerUnit * M_TO_IN) / mapWidth;
                    double scaleToFitY = (extents->GetHeight() * metersPerUnit * M_TO_IN) / mapHeight;
                    dMapScale = rs_max(scaleToFitX, scaleToFitY);
                }

                Ptr<MgPoint> pt = map->GetViewCenter();
                center = pt->GetCoordinate();
                break;
            }
        case MgdMapPlotInstruction::UseOverriddenCenterAndScale:
            {
                dMapScale = mapPlot->GetScale();
                center = mapPlot->GetCenter();
                break;
            }
        case MgdMapPlotInstruction::UseOverriddenExtent:
            {
                // Compute the plotCenter and plotScale from the extents
                extents = mapPlot->GetExtent();
                //...plotCenter
                Ptr<MgCoordinate> plotll = extents->GetLowerLeftCoordinate();
                Ptr<MgCoordinate> plotur = extents->GetUpperRightCoordinate();
                if (plotll == NULL || plotur == NULL)
                {
                    throw new MgNullReferenceException(L"MgdMappingService::GenerateMultiPlot", __LINE__, __WFILE__, NULL, L"", NULL);
                }
                double minX = plotll->GetX();
                double minY = plotll->GetY();
                double maxX = plotur->GetX();
                double maxY = plotur->GetY();
                double centerX = minX + (maxX - minX) * 0.5;
                double centerY = minY + (maxY - minY) * 0.5;
                //
                //...plotScale
                printLayout->ComputeMapOffsetAndSize(dMapScale, extents, metersPerUnit, dr.mapOffsetX(), dr.mapOffsetY(), dr.mapWidth(), dr.mapHeight());
                double mapWidth = dr.mapWidth();
                double mapHeight = dr.mapHeight();
                if (_wcsicmp(pageUnits.c_str(), L"mm") == 0 || _wcsicmp(pageUnits.c_str(), L"millimeters") == 0)  // NOXLATE
                {
                    mapWidth *= MM_TO_IN;
                    mapHeight *= MM_TO_IN;
                }

                double scaleToFitX = (extents->GetWidth() * metersPerUnit * M_TO_IN) / mapWidth;
                double scaleToFitY = (extents->GetHeight() * metersPerUnit * M_TO_IN) / mapHeight;
                dMapScale = rs_max(scaleToFitX, scaleToFitY);

                map->SetViewScale(dMapScale);
                center = new MgCoordinateXY(centerX, centerY);
                if (center == NULL)
                {
                    throw new MgNullReferenceException(L"MgdMappingService::GenerateMultiPlot", __LINE__, __WFILE__, NULL, L"", NULL);
                }
                Ptr<MgPoint> centerpt = new MgPoint(center);
                if (centerpt == NULL)
                {
                    throw new MgNullReferenceException(L"MgdMappingService::GenerateMultiPlot", __LINE__, __WFILE__, NULL, L"", NULL);
                }
                map->SetViewCenter(centerpt);
                break;
            }
        default:
            {
                MgStringCollection arguments;
                arguments.Add(L"1");
                arguments.Add(L"MgdMapPlotCollection");

                throw new MgInvalidArgumentException(L"MgdMappingService::GenerateMultiPlot",
                    __LINE__, __WFILE__, &arguments, L"MgInvalidMapPlotCollectionMapPlotInstruction", NULL);
            }
            break;
        }

        printLayout->SetPlotCenter(center);
        printLayout->SetPlotScale(dMapScale);

        // Get the map background color
        RS_Color bgcolor;
        StylizationUtil::ParseColor( map->GetBackgroundColor(), bgcolor);

        // Get the layout background color
        RS_Color layoutColor;
        Ptr<MgColor> bgColor = printLayout->GetBackgroundColor();
        layoutColor.red() = bgColor->GetRed();
        layoutColor.green() = bgColor->GetGreen();
        layoutColor.blue() = bgColor->GetBlue();

        // Get the session ID
        STRING sessionId;
        /*Ptr<MgUserInformation> userInfo = MgUserInformation::GetCurrentUserInfo();
        if (userInfo != NULL)
            sessionId = userInfo->GetMgSessionId();*/

        RS_MapUIInfo mapInfo(sessionId, map->GetName(), L"", srs, units, layoutColor);

        // Dynamically adjust the width and height of the map
        printLayout->ComputeMapOffsetAndSize(dMapScale, extents, metersPerUnit, dr.mapOffsetX(), dr.mapOffsetY(), dr.mapWidth(), dr.mapHeight(), mapPlot->GetExpandToFit());

        Ptr<MgCoordinate> newll;
        Ptr<MgCoordinate> newur;
        if (mapPlot->GetMapPlotInstruction() != MgdMapPlotInstruction::UseOverriddenExtent || mapPlot->GetExpandToFit())
        {
            double mapWidth = dr.mapWidth();
            double mapHeight = dr.mapHeight();
            if (_wcsicmp(pageUnits.c_str(), L"mm") == 0 || _wcsicmp(pageUnits.c_str(), L"millimeters") == 0)  // NOXLATE
            {
                mapWidth *= MM_TO_IN;
                mapHeight *= MM_TO_IN;
            }

            env = printLayout->DetermineLayoutMapExtents(map, metersPerUnit, mapWidth, mapHeight);

            newll = env->GetLowerLeftCoordinate();
            newur = env->GetUpperRightCoordinate();
        }
        else if (mapPlot->GetMapPlotInstruction() == MgdMapPlotInstruction::UseOverriddenExtent && !mapPlot->GetExpandToFit())
        {
            newll = extents->GetLowerLeftCoordinate();
            newur = extents->GetUpperRightCoordinate();
        }

        b.minx = newll->GetX();
        b.miny = newll->GetY();
        b.maxx = newur->GetX();
        b.maxy = newur->GetY();

        SEMgdSymbolManager semgr(m_svcResource);
        DefaultStylizer ds(&semgr);

        double dpi = map->GetDisplayDpi();
        dr.StartMap(&mapInfo, b, dMapScale, dpi, metersPerUnit);

        Ptr<MgLayerCollection> layers = map->GetLayers();
        if (layers == NULL)
        {
            throw new MgNullReferenceException(L"MgdMappingService::GenerateMultiPlot", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        // Define a polygon to represent the map extents and fill it with the map background color
        dr.StartLayer(NULL, NULL);
        LineBuffer lb(5);
        lb.MoveTo(b.minx, b.miny);
        lb.LineTo(b.maxx, b.miny);
        lb.LineTo(b.maxx, b.maxy);
        lb.LineTo(b.minx, b.maxy);
        lb.Close();

        RS_LineStroke lineStroke;
        RS_FillStyle fillStyle(lineStroke, bgcolor, layoutColor, L"Solid");  // NOXLATE
        dr.ProcessPolygon(&lb, fillStyle);
        dr.EndLayer();

        //transfer layers to a temporary collection
        //here it doesn't matter but in the rendering service it does
        Ptr<MgReadOnlyLayerCollection> rolc = new MgReadOnlyLayerCollection();

        for (int u=0; u<layers->GetCount(); u++)
        {
            Ptr<MgLayerBase> lr = layers->GetItem(u);
            rolc->Add(lr);
        }

        //stylize all the map layers
        MgdMappingUtil::StylizeLayers(m_svcResource, m_svcFeature, m_svcDrawing, m_pCSFactory, map,
                                     rolc, NULL, &ds, &dr, dstCs, false, false, dMapScale);

        // Finish adding the map to the page
        // Calculate the the height of the map bounds on the page (in page units)
        dr.mapBoundsHeight() = b.height() * dr.mapWidth()/b.width();

        //construct one every time -- not really a bottleneck
        MgdLegendPlotUtil lu(m_svcResource);

        // Now add the rest of the layout element to the page
        lu.AddLayoutElements(printLayout, (STRING)mapInfo.name(), mapResId->ToString(), map, layers, b, dMapScale, metersPerUnit, dr);

        dr.EndMap();
    }

    dr.Done();

    // write out the file
    Ptr<MgByteSource> bs = new MgByteSource(dwfName, true);

    bs->SetMimeType(MgMimeType::Dwf);
    byteReader = bs->GetReader();

    MG_SERVER_MAPPING_SERVICE_CATCH_AND_THROW(L"MgdMappingService::GenerateMultiPlot")

    return byteReader.Detach();
}