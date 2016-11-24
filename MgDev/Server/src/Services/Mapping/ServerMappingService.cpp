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

#include "ServerMappingServiceDefs.h"
#include "ServerMappingService.h"

#include "Bounds.h"
#include "Renderer.h"

#include "StylizationUtil.h"
#include "MappingUtil.h"

#include "EPlotRenderer.h"

#include "DefaultStylizer.h"

#include "FeatureTypeStyleVisitor.h"
#include "SymbolInstance.h"

#include "RSMgSymbolManager.h"
#include "SEMgSymbolManager.h"

#include "LegendPlotUtil.h"
#include "MgCSTrans.h"

#include "icons.h"

#define LEGEND_BITMAP_SIZE 16

#define REQUEST_LAYER_STRUCTURE         1 /* Request layer and group structure */
#define REQUEST_LAYER_ICONS             2 /* Request layer scale and icon information */
#define REQUEST_LAYER_FEATURE_SOURCE    4 /* Request information about a layer's feature source */

typedef std::map<STRING, MdfModel::LayerDefinition*> LayerDefinitionMap;

//for use by observation mesh transformation
const STRING SRS_LL84 = L"GEOGCS[\"LL84\",DATUM[\"WGS84\",SPHEROID[\"WGS84\",6378137,298.25722293287],TOWGS84[0,0,0,0,0,0,0]],PRIMEM[\"Greenwich\",0],UNIT[\"Degrees\",0.01745329252]]";


IMPLEMENT_CREATE_SERVICE(MgServerMappingService)

///----------------------------------------------------------------------------
/// <summary>
/// Constructs the object.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------
MgServerMappingService::MgServerMappingService() :
    MgMappingService()
{
    m_svcFeature = NULL;
    m_svcResource = NULL;
    m_svcDrawing = NULL;
    m_pCSFactory = new MgCoordinateSystemFactory();

    // TODO: This code to read the RasterGridSize settings is replicated from MgServerRenderingService.
    // Ideally, we would have both places share this chunk of code.
    // One possibility is to move it to MappingUtil. This may require changes to MappingUtil
    // (like making it a static instance so we can read the settings once and store them).
    // Hopefully we can re-visit this in the future.
    //
    MgConfiguration* pConf = MgConfiguration::GetInstance();

    pConf->GetIntValue(MgConfigProperties::RenderingServicePropertiesSection,
                          MgConfigProperties::RenderingServicePropertyRasterGridSize,
                          m_rasterGridSize,
                          MgConfigProperties::DefaultRenderingServicePropertyRasterGridSize);

    pConf->GetIntValue(MgConfigProperties::RenderingServicePropertiesSection,
                          MgConfigProperties::RenderingServicePropertyMinRasterGridSize,
                          m_minRasterGridSize,
                          MgConfigProperties::DefaultRenderingServicePropertyMinRasterGridSize);

    pConf->GetDoubleValue(MgConfigProperties::RenderingServicePropertiesSection,
                          MgConfigProperties::RenderingServicePropertyRasterGridSizeOverrideRatio,
                          m_rasterGridSizeOverrideRatio,
                          MgConfigProperties::DefaultRenderingServicePropertyRasterGridSizeOverrideRatio);

    pConf->GetIntValue(MgConfigProperties::RenderingServicePropertiesSection,
                          MgConfigProperties::RenderingServicePropertyRasterGridSizeForPlot,
                          m_rasterGridSizeForPlot,
                          MgConfigProperties::DefaultRenderingServicePropertyRasterGridSizeForPlot);

    pConf->GetIntValue(MgConfigProperties::RenderingServicePropertiesSection,
                          MgConfigProperties::RenderingServicePropertyMinRasterGridSizeForPlot,
                          m_minRasterGridSizeForPlot,
                          MgConfigProperties::DefaultRenderingServicePropertyMinRasterGridSizeForPlot);

    pConf->GetDoubleValue(MgConfigProperties::RenderingServicePropertiesSection,
                          MgConfigProperties::RenderingServicePropertyRasterGridSizeOverrideRatioForPlot,
                          m_rasterGridSizeOverrideRatioForPlot,
                          MgConfigProperties::DefaultRenderingServicePropertyRasterGridSizeOverrideRatioForPlot);

    pConf->GetIntValue(MgConfigProperties::RenderingServicePropertiesSection,
                          MgConfigProperties::RenderingServicePropertyMaxRasterImageWidth,
                          m_maxRasterImageWidth,
                          MgConfigProperties::DefaultRenderingServicePropertyMaxRasterImageWidth);

    pConf->GetIntValue(MgConfigProperties::RenderingServicePropertiesSection,
                          MgConfigProperties::RenderingServicePropertyMaxRasterImageHeight,
                          m_maxRasterImageHeight,
                          MgConfigProperties::DefaultRenderingServicePropertyMaxRasterImageHeight);
}


///----------------------------------------------------------------------------
/// <summary>
/// Destructs the object.
/// </summary>
///----------------------------------------------------------------------------
MgServerMappingService::~MgServerMappingService()
{
    MG_SERVER_MAPPING_SERVICE_TRY()

    MG_SERVER_MAPPING_SERVICE_CATCH(L"MgServerMappingService.~MgServerMappingService")
}


///----------------------------------------------------------------------------
/// <summary>
/// Plot The map to an EPlot DWF using the center and scale from the map.  The
/// extents will be computed to fill the space within the page margins.  If the
/// layout paramter is null, no adornments will be added to the page.
/// </summary>
/// <param name="map">Map object describing current state of map.</param>
/// <param name="plotSpec">Plotting specifications for the generated plot.</param>
/// <param name="layout">Layout specifications to use for the generated plot.</param>
/// <param name="dwfVersion">DWF version required by the client. This
///                          determines the schema and file versions used to
///                          generate the DWFs sent back to the client. </param>
/// <returns>
/// Returns an MgByteReader object containing a DWF in ePlot format.
/// </returns>
/// <exception>
/// MgArgumentNullException
/// </exception>
///----------------------------------------------------------------------------
MgByteReader* MgServerMappingService::GeneratePlot(
    MgMap* map,
    MgPlotSpecification* plotSpec,
    MgLayout* layout,
    MgDwfVersion* dwfVersion)
{
    Ptr<MgByteReader> byteReader;

    MG_SERVER_MAPPING_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerMappingService::GeneratePlot()");

    CHECKARGUMENTNULL(map, L"MgServerMappingService.GeneratePlot");
    CHECKARGUMENTNULL(dwfVersion, L"MgServerMappingService.GeneratePlot");
    CHECKARGUMENTNULL(plotSpec, L"MgServerMappingService.GeneratePlot");

    // Create a MgMapPlot which will be passed to GenerateMultiPlot
    Ptr<MgMapPlot> mapPlot = new MgMapPlot(map, plotSpec, layout);

    // Add it to a MgMapPlotCollecion
    Ptr<MgMapPlotCollection> mapPlots = new MgMapPlotCollection();
    mapPlots->Add(mapPlot);

    // Create the plot
    byteReader = GenerateMultiPlot(mapPlots, dwfVersion);

    MG_SERVER_MAPPING_SERVICE_CATCH_AND_THROW(L"MgServerMappingService.GeneratePlot")

    return byteReader.Detach();
}


///----------------------------------------------------------------------------
/// <summary>
/// Plot the map to an EPlot DWF using the specified center and scale.  The extents
/// will be computed to fill the space within the page margins.  If the layout
/// paramter is null, no adornments will be added to the page.
/// </summary>
/// <param name="map">Map object describing current state of map.</param>
/// <param name="center">The coordinates of the center of the map for the plot.</param>
/// <param name="scale">The scale to use for the plot.</param>
/// <param name="plotSpec">Plotting specifications for the generated plot.</param>
/// <param name="layout">Layout specifications to use for the generated plot.</param>
/// <param name="dwfVersion">DWF version required by the client. This
///                          determines the schema and file versions used to
///                          generate the DWFs sent back to the client. </param>
/// <returns>
/// Returns an MgByteReader object containing a DWF in ePlot format.
/// </returns>
/// <exception>
/// MgArgumentNullException
/// </exception>
///----------------------------------------------------------------------------
MgByteReader* MgServerMappingService::GeneratePlot(
    MgMap* map,
    MgCoordinate* center,
    double scale,
    MgPlotSpecification* plotSpec,
    MgLayout* layout,
    MgDwfVersion* dwfVersion)
{
    Ptr<MgByteReader> byteReader;

    MG_SERVER_MAPPING_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerMappingService::GeneratePlot()");

    CHECKARGUMENTNULL(map, L"MgServerMappingService.GeneratePlot");
    CHECKARGUMENTNULL(center, L"MgServerMappingService.GeneratePlot");
    CHECKARGUMENTNULL(dwfVersion, L"MgServerMappingService.GeneratePlot");
    CHECKARGUMENTNULL(plotSpec, L"MgServerMappingService.GeneratePlot");

    // Create a MgMapPlot which will be passed to GenerateMultiPlot
    Ptr<MgMapPlot> mapPlot = new MgMapPlot(map, center, scale, plotSpec, layout);

    // Add it to a MgMapPlotCollecion
    Ptr<MgMapPlotCollection> mapPlots = new MgMapPlotCollection();
    mapPlots->Add(mapPlot);

    // Create the plot
    byteReader = GenerateMultiPlot(mapPlots, dwfVersion);

    MG_SERVER_MAPPING_SERVICE_CATCH_AND_THROW(L"MgServerMappingService.GeneratePlot")

    return byteReader.Detach();
}


///----------------------------------------------------------------------------
/// <summary>
/// Plot the map to an EPlot DWF using the specified center and scale.  The extents
/// will be computed to fill the space within the page margins.  If the layout
/// paramter is null, no adornments will be added to the page.
/// </summary>
/// <param name="map">Map object describing current state of map.</param>
/// <param name="extents">The extents of the map for the plot.</param>
/// <param name="expandToFit">The flag used to determine if the extents should be
///                           expanded for fill the space withing page margins.</param>
/// <param name="plotSpec">Plotting specifications for the generated plot.</param>
/// <param name="layout">Layout specifications to use for the generated plot.</param>
/// <param name="dwfVersion">DWF version required by the client. This
///                          determines the schema and file versions used to
///                          generate the DWFs sent back to the client. </param>
/// <returns>
/// Returns an MgByteReader object containing a DWF in ePlot format.
/// </returns>
/// <exception>
/// MgArgumentNullException
/// </exception>
///----------------------------------------------------------------------------
MgByteReader* MgServerMappingService::GeneratePlot(
    MgMap* map,
    MgEnvelope* extents,
    bool expandToFit,
    MgPlotSpecification* plotSpec,
    MgLayout* layout,
    MgDwfVersion* dwfVersion)
{
    Ptr<MgByteReader> byteReader;

    MG_SERVER_MAPPING_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerMappingService::GeneratePlot()");

    CHECKARGUMENTNULL(map, L"MgServerMappingService.GeneratePlot");
    CHECKARGUMENTNULL(extents, L"MgServerMappingService.GeneratePlot");
    CHECKARGUMENTNULL(plotSpec, L"MgServerMappingService.GeneratePlot");
    CHECKARGUMENTNULL(dwfVersion, L"MgServerMappingService.GeneratePlot");

    // Make a copy of the extents
    Ptr<MgCoordinate> oldll = extents->GetLowerLeftCoordinate();
    Ptr<MgCoordinate> oldur = extents->GetUpperRightCoordinate();
    Ptr<MgCoordinate> ll = new MgCoordinateXY(oldll->GetX(), oldll->GetY());
    Ptr<MgCoordinate> ur = new MgCoordinateXY(oldur->GetX(), oldur->GetY());
    CHECKNULL(ll, L"MgServerMappingService.GeneratePlot");
    CHECKNULL(ur, L"MgServerMappingService.GeneratePlot");
    Ptr<MgEnvelope> env = new MgEnvelope(ll, ur);

    // Create a MgMapPlot which will be passed to GenerateMultiPlot
    Ptr<MgMapPlot> mapPlot = new MgMapPlot(map, env, expandToFit, plotSpec, layout);

    // Add it to a MgMapPlotCollecion
    Ptr<MgMapPlotCollection> mapPlots = new MgMapPlotCollection();
    CHECKNULL(mapPlot, L"MgServerMappingService.GeneratePlot");
    CHECKNULL(mapPlots, L"MgServerMappingService.GeneratePlot");
    mapPlots->Add(mapPlot);

    // Create the plot
    byteReader = GenerateMultiPlot(mapPlots, dwfVersion);

    MG_SERVER_MAPPING_SERVICE_CATCH_AND_THROW(L"MgServerMappingService.GeneratePlot")

    return byteReader.Detach();
}


///----------------------------------------------------------------------------
/// <summary>
/// Generates an <see cref="ePlot"/> containing one sheet per
/// specified map.
/// </summary>
/// <param name="mapPlots">A collection of MapPlot objects. </param>
/// <param name="dwfVersion">DWF specification required by the client. This
///                          determines the schema and file versions used to
///                          generate the DWFs sent back to the client. </param>
/// <returns>
/// Returns an MgByteReader object containing a DWF
/// in ePlot format with multiple sheets, one for each
/// map.
/// </returns>
/// <exception>
/// MgArgumentNullException
/// </exception>
///----------------------------------------------------------------------------
MgByteReader* MgServerMappingService::GenerateMultiPlot(
    MgMapPlotCollection* mapPlots,
    MgDwfVersion* dwfVersion)
{
    Ptr<MgByteReader> byteReader;

    MG_SERVER_MAPPING_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerMappingService::GenerateMultiPlot()");

    CHECKARGUMENTNULL(mapPlots, L"MgServerMappingService.GenerateMultiPlot");
    CHECKARGUMENTNULL(dwfVersion, L"MgServerMappingService.GenerateMultiPlot");

    if (m_svcResource == NULL)
        InitializeResourceService();
    if (m_svcFeature == NULL)
        InitializeFeatureService();
    if (m_svcDrawing == NULL)
        InitializeDrawingService();

    // get a temporary file to write out EPlot DWF to
    // TODO: clean the temp file name prefix
    STRING dwfName = MgFileUtil::GenerateTempFileName(false, L"default_prefix");

    EPlotRenderer dr(dwfName.c_str(), 0, 0, L"inches");  // NOXLATE

    dr.SetRasterGridSize(m_rasterGridSizeForPlot);
    dr.SetMinRasterGridSize(m_minRasterGridSizeForPlot);
    dr.SetRasterGridSizeOverrideRatio(m_rasterGridSizeOverrideRatioForPlot);
    dr.SetMaxRasterImageWidth(m_maxRasterImageWidth);
    dr.SetMaxRasterImageHeight(m_maxRasterImageHeight);

    RSMgSymbolManager mgr(m_svcResource);
    dr.SetSymbolManager(&mgr);

    // process the MapPlot collection
    for (int nMapPlotIndex = 0; nMapPlotIndex < mapPlots->GetCount(); nMapPlotIndex++)
    {
        Ptr<MgMapPlot> mapPlot = mapPlots->GetItem(nMapPlotIndex);
        CHECKNULL((MgMapPlot*)mapPlot, L"MgServerMappingService.GenerateMultiPlot");

        Ptr<MgMap> map = mapPlot->GetMap();
        CHECKNULL((MgMap*)map, L"MgServerMappingService.GenerateMultiPlot");

        Ptr<MgPlotSpecification> plotSpec = mapPlot->GetPlotSpecification();
        CHECKNULL((MgPlotSpecification*)plotSpec, L"MgServerMappingService.GenerateMultiPlot");

        Ptr<MgLayout> layout = mapPlot->GetLayout();

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
        CHECKNULL((MgEnvelope*)env, L"MgServerMappingService.GenerateMultiPlot");

        Ptr<MgCoordinate> ll = env->GetLowerLeftCoordinate();
        Ptr<MgCoordinate> ur = env->GetUpperRightCoordinate();
        CHECKNULL((MgCoordinate*)ll, L"MgServerMappingService.GenerateMultiPlot");
        CHECKNULL((MgCoordinate*)ur, L"MgServerMappingService.GenerateMultiPlot");
        RS_Bounds b(ll->GetX(), ll->GetY(), ur->GetX(), ur->GetY());

        //if requested data extent is not valid, use map definition extent
        // this may be removed eventually, but is a good sanity check nonetheless
        if (!b.IsValid())
        {
            Ptr<MgEnvelope> env2 = map->GetMapExtent();
            CHECKNULL((MgEnvelope*)env2, L"MgServerMappingService.GenerateMultiPlot");

            Ptr<MgCoordinate> ll2 = env2->GetLowerLeftCoordinate();
            Ptr<MgCoordinate> ur2 = env2->GetUpperRightCoordinate();
            CHECKNULL((MgCoordinate*)ll2, L"MgServerMappingService.GenerateMultiPlot");
            CHECKNULL((MgCoordinate*)ur2, L"MgServerMappingService.GenerateMultiPlot");

            b.minx = ll2->GetX();
            b.miny = ll2->GetY();
            b.maxx = ur2->GetX();
            b.maxy = ur2->GetY();
        }

        // Create a simple print layout containing only the map
        Ptr<MgPrintLayout> printLayout = new MgPrintLayout();
        CHECKNULL((MgPrintLayout*)printLayout, L"MgServerMappingService.GenerateMultiPlot");

        if (NULL != layout)
        {
            Ptr<MgResourceIdentifier> resID = layout->GetLayout();
            printLayout->Create(m_svcResource, resID);
            printLayout->SetPlotSize(plotSpec);
            printLayout->SetScaleBarUnits(layout->GetUnitType());
            printLayout->SetPlotTitle(layout->GetTitle());

            Ptr<MgPlotSpecification> spec = printLayout->GetPlotSize();

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
        CHECKNULL((MgCoordinate*)center, L"MgServerMappingService.GenerateMultiPlot");
        CHECKNULL((MgEnvelope*)extents, L"MgServerMappingService.GenerateMultiPlot");

        switch (mapPlot->GetMapPlotInstruction())
        {
        case MgMapPlotInstruction::UseMapCenterAndScale:
            {
                dMapScale = map->GetViewScale();
                if (dMapScale <= 0)
                {
                    Ptr<MgEnvelope> extents = map->GetDataExtent();
                    CHECKNULL((MgEnvelope*)extents, L"MgServerMappingService.GenerateMultiPlot");
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
        case MgMapPlotInstruction::UseOverriddenCenterAndScale:
            {
                dMapScale = mapPlot->GetScale();
                center = mapPlot->GetCenter();
                break;
            }
        case MgMapPlotInstruction::UseOverriddenExtent:
            {
                // Compute the plotCenter and plotScale from the extents
                extents = mapPlot->GetExtent();
                //...plotCenter
                Ptr<MgCoordinate> plotll = extents->GetLowerLeftCoordinate();
                Ptr<MgCoordinate> plotur = extents->GetUpperRightCoordinate();
                CHECKNULL((MgCoordinate*)plotll, L"MgServerMappingService.GenerateMultiPlot");
                CHECKNULL((MgCoordinate*)plotur, L"MgServerMappingService.GenerateMultiPlot");

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
                CHECKNULL((MgCoordinate*)center, L"MgServerMappingService.GenerateMultiPlot");
                Ptr<MgPoint> centerpt = new MgPoint(center);
                CHECKNULL((MgPoint*)centerpt, L"MgServerMappingService.GenerateMultiPlot");
                map->SetViewCenter(centerpt);
                break;
            }
        default:
            {
                MgStringCollection arguments;
                arguments.Add(L"1");
                arguments.Add(L"MgMapPlotCollection");

                throw new MgInvalidArgumentException(L"MgServerMappingService.GenerateMultiPlot",
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
        Ptr<MgUserInformation> userInfo = MgUserInformation::GetCurrentUserInfo();
        if (userInfo != NULL)
            sessionId = userInfo->GetMgSessionId();

        RS_MapUIInfo mapInfo(sessionId, map->GetName(), L"", srs, units, layoutColor, center->GetX(), center->GetY(), dMapScale);

        // Dynamically adjust the width and height of the map
        printLayout->ComputeMapOffsetAndSize(dMapScale, extents, metersPerUnit, dr.mapOffsetX(), dr.mapOffsetY(), dr.mapWidth(), dr.mapHeight(), mapPlot->GetExpandToFit());

        Ptr<MgCoordinate> newll;
        Ptr<MgCoordinate> newur;
        if (mapPlot->GetMapPlotInstruction() != MgMapPlotInstruction::UseOverriddenExtent || mapPlot->GetExpandToFit())
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
        else if (mapPlot->GetMapPlotInstruction() == MgMapPlotInstruction::UseOverriddenExtent && !mapPlot->GetExpandToFit())
        {
            newll = extents->GetLowerLeftCoordinate();
            newur = extents->GetUpperRightCoordinate();
        }

        b.minx = newll->GetX();
        b.miny = newll->GetY();
        b.maxx = newur->GetX();
        b.maxy = newur->GetY();

        SEMgSymbolManager semgr(m_svcResource);
        DefaultStylizer ds(&semgr);

        double dpi = map->GetDisplayDpi();
        dr.StartMap(&mapInfo, b, dMapScale, dpi, metersPerUnit);

        Ptr<MgLayerCollection> layers = map->GetLayers();
        CHECKNULL((MgLayerCollection*)layers, L"MgServerMappingService.GenerateMultiPlot");

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
        MgMappingUtil::StylizeLayers(m_svcResource, m_svcFeature, m_svcDrawing, m_pCSFactory, map,
                                     rolc, NULL, &ds, &dr, dstCs, false, false, dMapScale);

        // Finish adding the map to the page
        // Calculate the the height of the map bounds on the page (in page units)
        dr.mapBoundsHeight() = b.height() * dr.mapWidth()/b.width();

        //construct one every time -- not really a bottleneck
        MgLegendPlotUtil lu(m_svcResource);

        // Now add the rest of the layout element to the page
        lu.AddLayoutElements(printLayout, (STRING)mapInfo.name(), mapResId->ToString(), map, layers, b, dMapScale, metersPerUnit, dr);

        dr.EndMap();
    }

    dr.Done();

    // write out the file
    Ptr<MgByteSource> bs = new MgByteSource(dwfName, true);

    bs->SetMimeType(MgMimeType::Dwf);
    byteReader = bs->GetReader();

    MG_SERVER_MAPPING_SERVICE_CATCH_AND_THROW(L"MgServerMappingService.GenerateMultiPlot")

    return byteReader.Detach();
}


///----------------------------------------------------------------------------
/// <summary>
/// Generates an EPlot DWF containing a legend for the specified
/// map based upon its current view scale.
/// </summary>
/// <param name="map">
/// Map object for the current view state for the legend
/// </param>
/// <param name="scale">
/// The map scale for generating the legend.
/// </param>
/// <param name="plotSpec">
/// Plotting specifications for the legend describing the plot size and margins.
/// </param>
/// <param name="dwfVersion">
/// DWF specification supported/required by the client.  The DWF spec determines the
/// schema and file versions used to generate the DWFs sent back to the client.
/// </param>
/// <returns>
/// An EPlot DWF for the legend
/// </returns>
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------
MgByteReader* MgServerMappingService::GenerateLegendPlot(
    MgMap* map,
    double scale,
    MgPlotSpecification* plotSpec,
    MgDwfVersion* dwfVersion)
{
    Ptr<MgByteReader> byteReader;

    MG_SERVER_MAPPING_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerMappingService::GenerateLegendPlot()");

    CHECKARGUMENTNULL(map, L"MgServerMappingService.GenerateLegendPlot");
    CHECKARGUMENTNULL(dwfVersion, L"MgServerMappingService.GenerateLegendPlot");
    CHECKARGUMENTNULL(plotSpec, L"MgServerMappingService.GenerateLegendPlot");

    if (m_svcResource == NULL)
        InitializeResourceService();

    // Here is where we have to get the layers, rules, etc.
    // But first let's generate a box for the legend border...

    // request extenst
    Ptr<MgEnvelope> env = map->GetDataExtent();
    CHECKNULL((MgEnvelope*)env, L"MgServerMappingService.GenerateMultiPlot");
    Ptr<MgCoordinate> ll = env->GetLowerLeftCoordinate();
    Ptr<MgCoordinate> ur = env->GetUpperRightCoordinate();
    CHECKNULL((MgCoordinate*)ll, L"MgServerMappingService.GenerateMultiPlot");
    CHECKNULL((MgCoordinate*)ur, L"MgServerMappingService.GenerateMultiPlot");
    RS_Bounds b(ll->GetX(), ll->GetY(), ur->GetX(), ur->GetY());

    // get a temporary file to write out EPlot DWF to
    STRING dwfName = MgFileUtil::GenerateTempFileName(false, L"legendplot");

    EPlotRenderer dr(dwfName, plotSpec->GetPaperWidth(), plotSpec->GetPaperHeight(), plotSpec->GetPageSizeUnits());

    dr.SetRasterGridSize(m_rasterGridSize);
    dr.SetMinRasterGridSize(m_minRasterGridSize);
    dr.SetRasterGridSizeOverrideRatio(m_rasterGridSizeOverrideRatio);
    dr.SetMaxRasterImageWidth(m_maxRasterImageWidth);
    dr.SetMaxRasterImageHeight(m_maxRasterImageHeight);

    RSMgSymbolManager mgr(m_svcResource);
    dr.SetSymbolManager(&mgr);

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

    Ptr<MgPoint> ptCenter = map->GetViewCenter();
    Ptr<MgCoordinate> coord = ptCenter->GetCoordinate();
    double dMapScale = map->GetViewScale();

    RS_Color mapBgColor(255, 255, 255, 0);
    RS_MapUIInfo mapInfo(L"", map->GetName(), L"", srs, units, mapBgColor, coord->GetX(), coord->GetY(), dMapScale);

    double dpi = map->GetDisplayDpi();
    dr.StartMap(&mapInfo, b, scale, dpi, metersPerUnit);

    RS_Bounds pageBounds(0, 0, plotSpec->GetPaperWidth(), plotSpec->GetPaperHeight());
    dr.StartLayout(pageBounds);

    double startX = 0;
    double startY = 0;
    double legendOffsetX = 0;
    double legendOffsetY = 0;

    // Draw legend boundary
    LineBuffer lb(4);
    lb.MoveTo(0, 0);
    lb.LineTo(0, plotSpec->GetPaperHeight());
    lb.LineTo(plotSpec->GetPaperWidth(), plotSpec->GetPaperHeight());
    lb.LineTo(plotSpec->GetPaperWidth(), 0);
    lb.Close();

    RS_LineStroke lineStroke;
    dr.ProcessPolyline(&lb, lineStroke);

    double convertUnits = 1.0;
    STRING pageUnits = plotSpec->GetPageSizeUnits();
    if (_wcsnicmp(pageUnits.c_str(), L"mm", 3) == 0 ||
        _wcsnicmp(pageUnits.c_str(), L"millimeters", 12) == 0 )  // NOXLATE
    {
        convertUnits = IN_TO_MM;
    }

    MgLegendPlotUtil lu(m_svcResource);
    lu.BuildLegendContent(map, scale, plotSpec, startX, startY, dr, convertUnits);

    dr.EndLayout();

    dr.EndMap();
    dr.Done();

    Ptr<MgByteSource> bs = new MgByteSource(dwfName, true);

    bs->SetMimeType(MgMimeType::Dwf);
    byteReader = bs->GetReader();

    MG_SERVER_MAPPING_SERVICE_CATCH_AND_THROW(L"MgServerMappingService.GenerateLegendPlot")

    return byteReader.Detach();
}


///----------------------------------------------------------------------------
/// <summary>
/// Queries and returns each of the visible features on the specified layer of the
/// specified map.  For feature layers the properties to be included are defined
/// in the Feature Layer definition. For DWF layers the properties of each of the objects
/// in the DWF source for the layer should be included as the feature properties. The
/// geometry of each feature can optionally be returned in either the coordinate space
/// of the map or the coordinate space of the display. This operation returns the feature
/// properties in XML format, which can be used to construct an HTML Image Map for a previously
/// rendered view of the map.
/// </summary>
/// <param name="map">Input
/// map object containing current viewed state of map.
/// </param>
/// <param name="layerName">Input
/// name of the layer to query properties for
/// </param>
/// <param name="coordinateSpace">Input
/// specifies the coordinate system the geometry should be returned in.  See MgCoordinateSpace
/// </param>
/// <returns>
/// A byte reader containing an XML document with the feature information
/// </returns>
/// <remarks>
/// This API as defined does not allow to support WMS GetFeatureInfo
/// </remarks>
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------
MgByteReader* MgServerMappingService::QueryFeatures(MgMap* map,
    CREFSTRING layerName, CREFSTRING coordinateSpace)
{
    throw new MgNotImplementedException(L"MgServerMappingService.QueryFeatures", __LINE__, __WFILE__, NULL, L"", NULL);
    return NULL;
}


///----------------------------------------------------------------------------
/// <summary>
/// Queries feature information for each feature visible at the specified point of interest, on a given set of layers.
/// This is a multi-layer version of QueryFeatureProperties.
/// </summary>
/// <param name="map">
/// Map object containing current view state of the map.
/// </param>
/// <param name="layerNames">
/// List of layers to return information from.  Each layer must be in the specified MgMap object
/// </param>
/// <param name="x">
/// X coordinate for point of interest
/// </param>
/// <param name="y">
/// Y coordinate for point of interest
/// </param>
/// <param name="maxFeatures">
/// Maximum number of feature that should be returned for a given layer
/// greater than 0, usually 1
/// </param>
/// <param name="coordinateSpace">Input
/// specifies the coordinate system the geometry should be returned in.  See MgCoordinateSpace
/// </param>
/// <returns>
/// A byte reader containing an XML document with the feature information
/// </returns>
/// <remarks>
/// This API is for use by the WMS agent, to support of the GetFeatureInfo WMS request
/// </remarks>
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------
MgByteReader* MgServerMappingService::QueryFeatures(MgMap* map,
    MgStringCollection* layerNames, INT32 x, INT32 y,
    INT32 maxFeatures, CREFSTRING coordinateSpace)
{
    throw new MgNotImplementedException(L"MgServerMappingService.QueryFeatures", __LINE__, __WFILE__, NULL, L"", NULL);
    return NULL;
}


///----------------------------------------------------------------------------
/// <summary>
/// Returns the legend image for the specified layer.
/// </summary>
/// <param name="scale">
/// The scale at which the symbolization is requested.
/// </param>
/// <param name="imgWidth">
/// The requested image width in pixels.
/// </param>
/// <param name="imgHeight">
/// The requested image height in pixels.
/// </param>
/// <param name="format">
/// Image format, from MgImageFormats. Example: PNG, JPG, PNG8, etc …
/// </param>
/// <param name="geomType">
/// The type of symbolization required: -1=any, 1=Point, 2=Line, 3=Area, 4=Composite
/// </param>
/// <param name="themeCategory">
/// The value indicating which theme category swatch to return.
/// Used when there is a theme defined at this scale. An exception will be
/// thrown if a requested them category doesn't exist.
/// </param>
/// <returns>
/// A stream representing the legend image.  The default returned image format
/// will be PNG8 unless a different supported format is requested. An exception
/// will be thrown if an unsupported image format is requested.
/// </returns>
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------
MgByteReader* MgServerMappingService::GenerateLegendImage(MgResourceIdentifier* resource,
    double     scale,
    INT32      imgWidth,
    INT32      imgHeight,
    CREFSTRING format,
    INT32      geomType,
    INT32      themeCategory)
{
    Ptr<MgByteReader> byteReader;

    MG_SERVER_MAPPING_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerMappingService::GenerateLegendImage");

    CHECKARGUMENTNULL(resource, L"MgServerMappingService.GenerateLegendImage");

    if (m_svcResource == NULL)
        InitializeResourceService();

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
                byteReader = MgMappingUtil::DrawFTS(m_svcResource, fts, imgWidth, imgHeight, themeCategory, format);
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

    MG_SERVER_MAPPING_SERVICE_CATCH_AND_THROW(L"MgServerMappingService.GenerateLegendImage")

    return byteReader.Detach();
}


//---------------------------------------------------------
// Private helper methods
//---------------------------------------------------------


//gets an instance of the feature service using the service manager
void MgServerMappingService::InitializeFeatureService()
{
    MgServiceManager* serviceMan = MgServiceManager::GetInstance();
    assert(NULL != serviceMan);

    m_svcFeature = dynamic_cast<MgFeatureService*>(
        serviceMan->RequestService(MgServiceType::FeatureService));
    assert(m_svcFeature != NULL);
}


//gets an instance of the resource service using the service manager
void MgServerMappingService::InitializeResourceService()
{
    MgServiceManager* serviceMan = MgServiceManager::GetInstance();
    assert(NULL != serviceMan);

    m_svcResource = dynamic_cast<MgResourceService*>(
        serviceMan->RequestService(MgServiceType::ResourceService));
    assert(m_svcResource != NULL);
}


//gets an instance of the drawing service using the service manager
void MgServerMappingService::InitializeDrawingService()
{
    MgServiceManager* serviceMan = MgServiceManager::GetInstance();
    assert(NULL != serviceMan);

    m_svcDrawing = dynamic_cast<MgDrawingService*>(
        serviceMan->RequestService(MgServiceType::DrawingService));
    assert(m_svcDrawing != NULL);
}

//gets an instance of the tile service using the service manager
void MgServerMappingService::InitializeTileService()
{
    MgServiceManager* serviceMan = MgServiceManager::GetInstance();
    assert(NULL != serviceMan);

    m_svcTile = dynamic_cast<MgTileService*>(
        serviceMan->RequestService(MgServiceType::TileService));
    assert(m_svcTile != NULL);
} 

// Returns true if the supplied feature type style is compatible with the
// supplied geometry type: 1=Point, 2=Line, 3=Area, 4=Composite
bool MgServerMappingService::FeatureTypeStyleSupportsGeomType(MdfModel::FeatureTypeStyle* fts, INT32 geomType)
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


void MgServerMappingService::MakeUIGraphicsForScaleRange(std::list<RS_UIGraphic>& uiGraphics, std::vector<MgByte*>& uiGraphicSources, MdfModel::VectorScaleRange* sr)
{
    //make a list of the theming bitmaps
    //TODO: for now, if there are multiple feature type styles
    //in the feature layer, we will pick the bitmaps for the first one
    //since the viewer cannot handle legend bitmaps for multiple
    //feature types in the same layer.

    MdfModel::FeatureTypeStyleCollection* ftscol = sr->GetFeatureTypeStyles();

    for (int k=0; k<ftscol->GetCount(); k++)
    {
        MdfModel::FeatureTypeStyle* fts = ftscol->GetAt(k);

        FeatureTypeStyleVisitor::eFeatureTypeStyle st = FeatureTypeStyleVisitor::DetermineFeatureTypeStyle(fts);

        switch (st)
        {
        case FeatureTypeStyleVisitor::ftsPoint:
        case FeatureTypeStyleVisitor::ftsLine:
        case FeatureTypeStyleVisitor::ftsArea:
        case FeatureTypeStyleVisitor::ftsComposite:
            {
                if (!fts->IsShowInLegend())
                    break;

                MdfModel::RuleCollection* rules = fts->GetRules();

                for (int i=0; i<rules->GetCount(); i++)
                {
                    MdfModel::Rule* rule = rules->GetAt(i);

                    RS_UIGraphic gr(NULL, 0, rule->GetLegendLabel());

                    Ptr<MgByteReader> rdr = MgMappingUtil::DrawFTS(m_svcResource, fts, LEGEND_BITMAP_SIZE, LEGEND_BITMAP_SIZE, i);

                    if (rdr.p)
                    {
                        MgByteSink sink(rdr);
                        MgByte* bytes = sink.ToBuffer();

                        //remember the bytes so that we release them when done
                        uiGraphicSources.push_back(bytes);

                        int sz = bytes->GetLength();
                        const unsigned char* bmp = bytes->Bytes();

                        gr.data() = (unsigned char*)bmp;
                        gr.length() = sz;
                    }

                    uiGraphics.push_back(gr);
                }
            }
            break;
        }
    }
}

void MgServerMappingService::SetConnectionProperties(MgConnectionProperties*)
{
    // Do nothing.  No connection properties are required for Server-side service objects.
}

MgByteReader* MgServerMappingService::DescribeRuntimeMap(MgMap* map,
                                                         INT32 requestedFeatures,
                                                         INT32 iconsPerScaleRange)
{
    CHECKARGUMENTNULL(map, L"MgServerMappingService.DescribeRuntimeMap");
    return DescribeRuntimeMap(map, MgImageFormats::Png, LEGEND_BITMAP_SIZE, LEGEND_BITMAP_SIZE, requestedFeatures, iconsPerScaleRange);
}

MgByteReader* MgServerMappingService::DescribeRuntimeMap(MgMap* map,
                                                         CREFSTRING iconFormat,
                                                         INT32 iconWidth,
                                                         INT32 iconHeight,
                                                         INT32 requestedFeatures,
                                                         INT32 iconsPerScaleRange)
{
    return DescribeRuntimeMap(map, iconFormat, iconWidth, iconHeight, requestedFeatures, iconsPerScaleRange, MG_API_VERSION(3, 0, 0) /* Latest and greatest */);
}

MgByteReader* MgServerMappingService::DescribeRuntimeMap(MgMap* map,
                                                         CREFSTRING iconFormat,
                                                         INT32 iconWidth,
                                                         INT32 iconHeight,
                                                         INT32 requestedFeatures,
                                                         INT32 iconsPerScaleRange,
                                                         INT32 schemaVersion)
{
    LayerDefinitionMap layerDefinitionMap;
    if (MgImageFormats::Png != iconFormat &&
        MgImageFormats::Gif != iconFormat &&
        MgImageFormats::Png8 != iconFormat &&
        MgImageFormats::Jpeg != iconFormat)
    {
        MgStringCollection args;
        args.Add(iconFormat);
        throw new MgInvalidArgumentException(L"MgServerMappingService.DescribeRuntimeMap", __LINE__, __WFILE__, NULL, L"MgInvalidImageFormat", &args);
    }
    Ptr<MgByteReader> byteReader;

    MG_SERVER_MAPPING_SERVICE_TRY()

    if (m_svcResource == NULL)
        InitializeResourceService();

    STRING sessionId = map->GetSessionId();
    STRING targetMapName = map->GetName();
    Ptr<MgResourceIdentifier> mapDefinition = map->GetMapDefinition();
    Ptr<MgResourceIdentifier> tileSetDefinition = map->GetTileSetDefinition();

    //TODO: Possible future caching opportunity?
    std::string xml = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";  // NOXLATE
    if (schemaVersion == MG_API_VERSION(2, 6, 0))
        xml.append("<RuntimeMap xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"RuntimeMap-2.6.0.xsd\">\n");
    else
        xml.append("<RuntimeMap xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"RuntimeMap-3.0.0.xsd\" version=\"3.0.0\">\n");
    // ------------------------------ Site Version ----------------------------------//
    xml.append("<SiteVersion>");
    MgServerManager* serverMgr = MgServerManager::GetInstance();
    xml.append(MgUtil::WideCharToMultiByte(serverMgr->GetSiteVersion()));
    xml.append("</SiteVersion>\n");
    // ------------------------------ Session ID --------------------------------- //
    xml.append("<SessionId>");
    xml.append(MgUtil::WideCharToMultiByte(sessionId));
    xml.append("</SessionId>\n");
    // ------------------------------ Map Name --------------------------------- //
    xml.append("<Name>");
    xml.append(MgUtil::WideCharToMultiByte(targetMapName));
    xml.append("</Name>\n");
    // ------------------------------ Map Definition ID --------------------------------- //
    xml.append("<MapDefinition>");
    xml.append(MgUtil::WideCharToMultiByte(mapDefinition->ToString()));
    xml.append("</MapDefinition>\n");
    //Write tile set definition if requesting a v3.0.0 response
    if (NULL != (MgResourceIdentifier*)tileSetDefinition && schemaVersion == MG_API_VERSION(3, 0, 0))
    {
        if (m_svcTile == NULL)
            InitializeTileService();

        std::string sWidth;
        std::string sHeight;
        MgUtil::Int32ToString(m_svcTile->GetDefaultTileSizeX(tileSetDefinition), sWidth);
        MgUtil::Int32ToString(m_svcTile->GetDefaultTileSizeY(tileSetDefinition), sHeight);

        // ------------------------------ Tile Set Definition ID --------------------------------- //
        xml.append("<TileSetDefinition>");
        xml.append(MgUtil::WideCharToMultiByte(tileSetDefinition->ToString()));
        xml.append("</TileSetDefinition>\n");
        // ------------------------------ Tile Width ---------------------------------- //
        xml.append("<TileWidth>");
        xml.append(sWidth);
        xml.append("</TileWidth>\n");
        // ------------------------------ Tile Height --------------------------------- //
        xml.append("<TileHeight>");
        xml.append(sHeight);
        xml.append("</TileHeight>\n");
    }
    // ------------------------------ Background Color ---------------------------------- //
    xml.append("<BackgroundColor>");
    xml.append(MgUtil::WideCharToMultiByte(map->GetBackgroundColor()));
    xml.append("</BackgroundColor>\n");
    // ------------------------------ Display DPI --------------------------------------- //
    xml.append("<DisplayDpi>");
    std::string sDpi;
    MgUtil::Int32ToString(map->GetDisplayDpi(), sDpi);
    xml.append(sDpi);
    xml.append("</DisplayDpi>\n");
    // ------------------------------ Icon Image Format --------------------------------- //
    if ((requestedFeatures & REQUEST_LAYER_ICONS) == REQUEST_LAYER_ICONS)
    {
        xml.append("<IconMimeType>");
        if (iconFormat == MgImageFormats::Gif)
            xml.append(MgUtil::WideCharToMultiByte(MgMimeType::Gif));
        else if (iconFormat == MgImageFormats::Jpeg)
            xml.append(MgUtil::WideCharToMultiByte(MgMimeType::Jpeg));
        else if (iconFormat == MgImageFormats::Png || iconFormat == MgImageFormats::Png8)
            xml.append(MgUtil::WideCharToMultiByte(MgMimeType::Png));
        xml.append("</IconMimeType>\n");
    }
    // ------------------------------ Coordinate System --------------------------------- //
    xml.append("<CoordinateSystem>\n");
    xml.append("<Wkt>");
    STRING srs = map->GetMapSRS();
    xml.append(MgUtil::WideCharToMultiByte(MgUtil::ReplaceEscapeCharInXml(srs)));
    xml.append("</Wkt>\n");
    Ptr<MgCoordinateSystemFactory> fact = new MgCoordinateSystemFactory();
    std::string sEpsg;
    std::string sMentor;
    std::string sMpu;
    try
    {
        Ptr<MgCoordinateSystem> cs = fact->Create(srs);
        if (!srs.empty())
            MgUtil::DoubleToString(cs->ConvertCoordinateSystemUnitsToMeters(1.0), sMpu);
        else
            sMpu = "1.0";
        sMentor = MgUtil::WideCharToMultiByte(cs->GetCsCode());
        INT32 epsg = cs->GetEpsgCode();
        MgUtil::Int32ToString(epsg, sEpsg);
    }
    catch (MgException* ex)
    {
        SAFE_RELEASE(ex);
    }
    xml.append("<MentorCode>");
    xml.append(sMentor);
    xml.append("</MentorCode>\n");
    xml.append("<EpsgCode>");
    xml.append(sEpsg);
    xml.append("</EpsgCode>\n");
    xml.append("<MetersPerUnit>");
    xml.append(sMpu);
    xml.append("</MetersPerUnit>\n");
    xml.append("</CoordinateSystem>");
    // ------------------------------ Extents --------------------------------- //
    std::string sExtents;
    Ptr<MgEnvelope> extents = map->GetMapExtent();
    extents->ToXml(sExtents);
    xml.append("<Extents>\n");
    xml.append(sExtents);
    xml.append("</Extents>");
    // ---------------------- Optional things if requested -------------------- //
    if ((requestedFeatures & REQUEST_LAYER_STRUCTURE) == REQUEST_LAYER_STRUCTURE)
    {
        //Build our LayerDefinition map for code below that requires it
        if ((requestedFeatures & REQUEST_LAYER_ICONS) == REQUEST_LAYER_ICONS)
        {
            Ptr<MgStringCollection> layerIds = new MgStringCollection();
            Ptr<MgLayerCollection> layers = map->GetLayers();
            for (INT32 i = 0; i < layers->GetCount(); i++)
            {
                Ptr<MgLayerBase> layer = layers->GetItem(i);
                Ptr<MgResourceIdentifier> ldfId = layer->GetLayerDefinition();
                layerIds->Add(ldfId->ToString());
            }

            Ptr<MgStringCollection> layerContents = m_svcResource->GetResourceContents(layerIds, NULL);
            for (INT32 i = 0; i < layerIds->GetCount(); i++)
            {
                STRING ldfId = layerIds->GetItem(i);
                STRING content = layerContents->GetItem(i);
                MdfModel::LayerDefinition* ldf = MgLayerBase::GetLayerDefinition(content);
                layerDefinitionMap[ldfId] = ldf;
            }
        }

        // ----------- Some pre-processing before we do groups/layers ------------- //
        Ptr<MgLayerGroupCollection> groups = map->GetLayerGroups();
        Ptr<MgLayerCollection> layers = map->GetLayers();
        for (INT32 i = 0; i < groups->GetCount(); i++)
        {
            Ptr<MgLayerGroup> group = groups->GetItem(i);
            Ptr<MgLayerGroup> parent = group->GetGroup();
            CreateGroupItem(group, parent, xml);
        }
        for (INT32 i = 0; i < layers->GetCount(); i++)
        {
            Ptr<MgLayerBase> layer = layers->GetItem(i);
            Ptr<MgLayerGroup> parent = layer->GetGroup();
            
            MdfModel::LayerDefinition* layerDef = NULL;
            Ptr<MgResourceIdentifier> layerid = layer->GetLayerDefinition();
            LayerDefinitionMap::iterator it = layerDefinitionMap.find(layerid->ToString());
            if (it != layerDefinitionMap.end())
                layerDef = it->second;

            CreateLayerItem(requestedFeatures, iconsPerScaleRange, iconFormat, iconWidth, iconHeight, layer, parent, layerDef, xml);
        }
    }
    else //Base Layer Groups need to be outputted regardless, otherwise a client application doesn't have enough information to build GETTILEIMAGE requests
    {
        Ptr<MgLayerGroupCollection> groups = map->GetLayerGroups();
        for (INT32 i = 0; i < groups->GetCount(); i++)
        {
            Ptr<MgLayerGroup> group = groups->GetItem(i);
            if (group->GetLayerGroupType() != MgLayerGroupType::BaseMap && group->GetLayerGroupType() != MgLayerGroupType::BaseMapFromTileSet)
                continue;

            Ptr<MgLayerGroup> parent = group->GetGroup();
            CreateGroupItem(group, parent, xml);
        }
    }
    // ------------------------ Finite Display Scales (if any) ------------------------- //
    INT32 fsCount = map->GetFiniteDisplayScaleCount();
    if (fsCount > 0)
    {
        for (INT32 i = 0; i < fsCount; i++)
        {
            xml.append("<FiniteDisplayScale>");
            double dScale = map->GetFiniteDisplayScaleAt(i);
            std::string sScale;
            MgUtil::DoubleToString(dScale, sScale);
            xml.append(sScale);
            xml.append("</FiniteDisplayScale>\n");
        }
    }
    xml.append("</RuntimeMap>");

    Ptr<MgByteSource> byteSource = new MgByteSource((BYTE_ARRAY_IN)xml.c_str(), (INT32)xml.length());
    byteSource->SetMimeType(MgMimeType::Xml);
    byteReader = byteSource->GetReader();

    MG_SERVER_MAPPING_SERVICE_CATCH(L"MgServerMappingService.DescribeRuntimeMap")

    //Cleanup our LayerDefinition pointers. Do it here so we don't leak on any exception
    for (LayerDefinitionMap::iterator it = layerDefinitionMap.begin(); it != layerDefinitionMap.end(); it++)
    {
        MdfModel::LayerDefinition* ldf = it->second;
        delete ldf;
    }
    layerDefinitionMap.clear();

    MG_SERVER_MAPPING_SERVICE_THROW()

    return byteReader.Detach();
}

MgByteReader* MgServerMappingService::CreateRuntimeMap(MgResourceIdentifier* mapDefinition,
                                                       CREFSTRING sessionId,
                                                       INT32 requestedFeatures,
                                                       INT32 iconsPerScaleRange)
{
    CHECKARGUMENTNULL(mapDefinition, L"MgServerMappingService.CreateRuntimeMap");
    STRING mapName = mapDefinition->GetName();
    return CreateRuntimeMap(mapDefinition, mapName, sessionId, MgImageFormats::Png, LEGEND_BITMAP_SIZE, LEGEND_BITMAP_SIZE, requestedFeatures, iconsPerScaleRange);
}

MgByteReader* MgServerMappingService::CreateRuntimeMap(MgResourceIdentifier* mapDefinition,
                                                       CREFSTRING targetMapName,
                                                       CREFSTRING sessionId,
                                                       CREFSTRING iconFormat,
                                                       INT32 iconWidth,
                                                       INT32 iconHeight,
                                                       INT32 requestedFeatures,
                                                       INT32 iconsPerScaleRange)
{
    return CreateRuntimeMap(mapDefinition, targetMapName, sessionId, iconFormat, iconWidth, iconHeight, requestedFeatures, iconsPerScaleRange, MG_API_VERSION(3, 0, 0) /* Latest and greatest */);
}

MgByteReader* MgServerMappingService::CreateRuntimeMap(MgResourceIdentifier* mapDefinition,
                                                       CREFSTRING targetMapName,
                                                       CREFSTRING sessionId,
                                                       CREFSTRING iconFormat,
                                                       INT32 iconWidth,
                                                       INT32 iconHeight,
                                                       INT32 requestedFeatures,
                                                       INT32 iconsPerScaleRange,
                                                       INT32 schemaVersion)
{
    CHECKARGUMENTNULL(mapDefinition, L"MgServerMappingService.CreateRuntimeMap");
    LayerDefinitionMap layerDefinitionMap;
    if (MgImageFormats::Png != iconFormat &&
        MgImageFormats::Gif != iconFormat &&
        MgImageFormats::Png8 != iconFormat &&
        MgImageFormats::Jpeg != iconFormat)
    {
        MgStringCollection args;
        args.Add(iconFormat);
        throw new MgInvalidArgumentException(L"MgServerMappingService.CreateRuntimeMap", __LINE__, __WFILE__, NULL, L"MgInvalidImageFormat", &args);
    }
    Ptr<MgByteReader> byteReader;

    MG_SERVER_MAPPING_SERVICE_TRY()

    if (m_svcResource == NULL)
        InitializeResourceService();

    Ptr<MgSiteConnection> siteConn = new MgSiteConnection();
    Ptr<MgUserInformation> userInfo = new MgUserInformation(sessionId);
    siteConn->Open(userInfo);
    Ptr<MgMap> map = new MgMap(siteConn);
    map->Create(mapDefinition, targetMapName);
    
    STRING sStateId = L"Session:";
    sStateId += sessionId;
    sStateId += L"//";
    sStateId += targetMapName;
    sStateId += L".";
    sStateId += MgResourceType::Map;

    Ptr<MgResourceIdentifier> mapStateId = new MgResourceIdentifier(sStateId);
    Ptr<MgSelection> sel = new MgSelection(map);
    //Call our special Save() API that doesn't try to look for a MgUserInformation that's not
    //there
    sel->Save(m_svcResource, sessionId, targetMapName);
    map->Save(m_svcResource, mapStateId);

    byteReader = DescribeRuntimeMap(map, iconFormat, iconWidth, iconHeight, requestedFeatures, iconsPerScaleRange, schemaVersion);

    MG_SERVER_MAPPING_SERVICE_CATCH_AND_THROW(L"MgServerMappingService.CreateRuntimeMap")

    return byteReader.Detach();
}

void MgServerMappingService::CreateGroupItem(MgLayerGroup* group, MgLayerGroup* parent, std::string& xml)
{
    MG_SERVER_MAPPING_SERVICE_TRY()

    STRING groupName = group->GetName();
    xml.append("<Group>\n");
    xml.append("<Name>");
    xml.append(MgUtil::WideCharToMultiByte(groupName));
    xml.append("</Name>\n");
    xml.append("<Type>");
    INT32 gType = group->GetLayerGroupType();
    std::string sType;
    MgUtil::Int32ToString(gType, sType);
    xml.append(sType);
    xml.append("</Type>\n");
    xml.append("<LegendLabel>");
    xml.append(MgUtil::WideCharToMultiByte(MgUtil::ReplaceEscapeCharInXml(group->GetLegendLabel())));
    xml.append("</LegendLabel>\n");
    xml.append("<ObjectId>");
    xml.append(MgUtil::WideCharToMultiByte(group->GetObjectId()));
    xml.append("</ObjectId>\n");
    if (NULL != parent)
    {
        xml.append("<ParentId>");
        xml.append(MgUtil::WideCharToMultiByte(parent->GetObjectId()));
        xml.append("</ParentId>\n");
    }
    xml.append("<DisplayInLegend>");
    xml.append(group->GetDisplayInLegend() ? "true" : "false");
    xml.append("</DisplayInLegend>\n");
    xml.append("<ExpandInLegend>");
    xml.append(group->GetExpandInLegend() ? "true" : "false");
    xml.append("</ExpandInLegend>\n");
    xml.append("<Visible>");
    xml.append(group->GetVisible() ? "true" : "false");
    xml.append("</Visible>\n");
    xml.append("<ActuallyVisible>");
    xml.append(group->IsVisible() ? "true" : "false");
    xml.append("</ActuallyVisible>\n");
    xml.append("</Group>");

    MG_SERVER_MAPPING_SERVICE_CATCH_AND_THROW(L"MgServerMappingService.CreateGroupItem")
}

void MgServerMappingService::CreateLayerItem(INT32 requestedFeatures, INT32 iconsPerScaleRange, CREFSTRING iconFormat, INT32 iconWidth, INT32 iconHeight, MgLayerBase* layer, MgLayerGroup* parent, MdfModel::LayerDefinition* ldf, std::string& xml)
{
    MG_SERVER_MAPPING_SERVICE_TRY()

    xml.append("<Layer>\n");
    xml.append("<Name>");
    xml.append(MgUtil::WideCharToMultiByte(layer->GetName()));
    xml.append("</Name>\n");
    xml.append("<Type>");
    std::string sLayerType;
    MgUtil::Int32ToString(layer->GetLayerType(), sLayerType);
    xml.append(sLayerType);
    xml.append("</Type>\n");
    xml.append("<LegendLabel>");
    xml.append(MgUtil::WideCharToMultiByte(MgUtil::ReplaceEscapeCharInXml(layer->GetLegendLabel())));
    xml.append("</LegendLabel>\n");
    xml.append("<ObjectId>");
    xml.append(MgUtil::WideCharToMultiByte(layer->GetObjectId()));
    xml.append("</ObjectId>\n");
    if (NULL != parent)
    {
        xml.append("<ParentId>");
        xml.append(MgUtil::WideCharToMultiByte(parent->GetObjectId()));
        xml.append("</ParentId>\n");
    }
    xml.append("<Selectable>");
    xml.append(layer->GetSelectable() ? "true" : "false");
    xml.append("</Selectable>\n");
    xml.append("<DisplayInLegend>");
    xml.append(layer->GetDisplayInLegend() ? "true" : "false");
    xml.append("</DisplayInLegend>\n");
    xml.append("<ExpandInLegend>");
    xml.append(layer->GetExpandInLegend() ? "true" : "false");
    xml.append("</ExpandInLegend>\n");
    xml.append("<Visible>");
    xml.append(layer->GetVisible() ? "true" : "false");
    xml.append("</Visible>\n");
    xml.append("<ActuallyVisible>");
    xml.append(layer->IsVisible() ? "true" : "false");
    xml.append("</ActuallyVisible>\n");
    xml.append("<LayerDefinition>");
    Ptr<MgResourceIdentifier> layerDefId = layer->GetLayerDefinition();
    STRING ldfId = layerDefId->ToString();
    xml.append(MgUtil::WideCharToMultiByte(ldfId));
    xml.append("</LayerDefinition>\n");
    // ----------------------- Optional things if requested ------------------------- //
    if ((requestedFeatures & REQUEST_LAYER_FEATURE_SOURCE) == REQUEST_LAYER_FEATURE_SOURCE)
    {
        xml.append("<FeatureSource>\n");
        xml.append("<ResourceId>");
        xml.append(MgUtil::WideCharToMultiByte(layer->GetFeatureSourceId()));
        xml.append("</ResourceId>\n");
        xml.append("<ClassName>");
        xml.append(MgUtil::WideCharToMultiByte(layer->GetFeatureClassName()));
        xml.append("</ClassName>\n");
        xml.append("<Geometry>");
        xml.append(MgUtil::WideCharToMultiByte(layer->GetFeatureGeometryName()));
        xml.append("</Geometry>\n");
        xml.append("</FeatureSource>\n");
    }
    
    if (NULL != ldf)
    {
        MdfModel::VectorLayerDefinition* vl = dynamic_cast<MdfModel::VectorLayerDefinition*>(ldf);
        MdfModel::GridLayerDefinition* gl = dynamic_cast<MdfModel::GridLayerDefinition*>(ldf);
        MdfModel::DrawingLayerDefinition* dl = dynamic_cast<MdfModel::DrawingLayerDefinition*>(ldf);
        if (NULL != vl)
        {
            MdfModel::VectorScaleRangeCollection* vsrs = vl->GetScaleRanges();
            for (INT32 i = 0; i < vsrs->GetCount(); i++)
            {
                MdfModel::VectorScaleRange* vsr = vsrs->GetAt(i);
                xml.append("<ScaleRange>\n");
                xml.append("<MinScale>");
                std::string sMinScale;
                MgUtil::DoubleToString(vsr->GetMinScale(), sMinScale);
                xml.append(sMinScale);
                xml.append("</MinScale>\n");
                xml.append("<MaxScale>");
                std::string sMaxScale;
                MgUtil::DoubleToString(vsr->GetMaxScale(), sMaxScale);
                xml.append(sMaxScale);
                xml.append("</MaxScale>\n");

                double dScale = (vsr->GetMaxScale() + vsr->GetMinScale()) / 2.0;
                MdfModel::FeatureTypeStyleCollection* ftsc = vsr->GetFeatureTypeStyles();

                INT32 nIconCount = 0;
                for (INT32 j = 0; j < ftsc->GetCount(); j++)
                {
                    MdfModel::FeatureTypeStyle* fts = ftsc->GetAt(j);
                    if (fts->IsShowInLegend())
                    {
                        MdfModel::RuleCollection* rules = fts->GetRules();
                        nIconCount += rules->GetCount();
                    }
                }

                //This theme will be compressed if we're over the specified limit for this scale range
                bool bCompress = (nIconCount > iconsPerScaleRange);

                for (INT32 j = 0; j < ftsc->GetCount(); j++)
                {
                    MdfModel::FeatureTypeStyle* fts = ftsc->GetAt(j);
                    if (!fts->IsShowInLegend())
                        continue;

                    MdfModel::PointTypeStyle*       pts = dynamic_cast<MdfModel::PointTypeStyle*>(fts);
                    MdfModel::LineTypeStyle*        lts = dynamic_cast<MdfModel::LineTypeStyle*>(fts);
                    MdfModel::AreaTypeStyle*        ats = dynamic_cast<MdfModel::AreaTypeStyle*>(fts);
                    MdfModel::CompositeTypeStyle*   cts = dynamic_cast<MdfModel::CompositeTypeStyle*>(fts);
                        
                    INT32 geomType = 0;
                    if (pts)
                        geomType = 1;
                    else if (lts)
                        geomType = 2;
                    else if (ats)
                        geomType = 3;
                    else if (cts)
                        geomType = 4;

                    //Geometry type and Theme Category are required so that deferred icon requests can be made
                    //back to the GetLegendImage(), especially if we exceed the icons-per-scale-range limit
                    //Geometry type is specified here, theme category is inferred from rule item positioning.
                    xml.append("<FeatureStyle>\n");
                    xml.append("<Type>");
                    std::string sGeomType;
                    MgUtil::Int32ToString(geomType, sGeomType);
                    xml.append(sGeomType);
                    xml.append("</Type>\n");
                    MdfModel::RuleCollection* rules = fts->GetRules();
                    for (INT32 r = 0; r < rules->GetCount(); r++)
                    {
                        MdfModel::Rule* rule = rules->GetAt(r);
                        bool bRequestIcon = false;
                        if (!bCompress)
                        {
                            bRequestIcon = true;
                        }
                        else //This is a compressed theme
                        {
                            bRequestIcon = (r == 0 || r == rules->GetCount() - 1); //Only first and last rule
                        }

                        xml.append("<Rule>\n");
                        xml.append("<LegendLabel>");
                        xml.append(MgUtil::WideCharToMultiByte(MgUtil::ReplaceEscapeCharInXml(rule->GetLegendLabel())));
                        xml.append("</LegendLabel>\n");
                        xml.append("<Filter>");
                        xml.append(MgUtil::WideCharToMultiByte(MgUtil::ReplaceEscapeCharInXml(rule->GetFilter())));
                        xml.append("</Filter>\n");
                        if (bRequestIcon)
                        {
                            xml.append("<Icon>");
                            Ptr<MgByteReader> iconReader = MgMappingUtil::DrawFTS(m_svcResource, fts, iconWidth, iconHeight, r, iconFormat);
                            Ptr<MgByteSink> sink = new MgByteSink(iconReader);
                            Ptr<MgByte> bytes = sink->ToBuffer();
                            Ptr<MgMemoryStreamHelper> streamHelper = new MgMemoryStreamHelper((INT8*) bytes->Bytes(), bytes->GetLength(), false);
                            std::string b64 = streamHelper->ToBase64();
                            xml.append(b64);
                            xml.append("</Icon>");
                        }
                        xml.append("</Rule>\n");
                    }
                    xml.append("</FeatureStyle>\n");
                }

                xml.append("</ScaleRange>\n");
            }
        }
        else if (NULL != gl)
        {
            MdfModel::GridScaleRangeCollection* gsrs = gl->GetScaleRanges();
            for (INT32 i = 0; i < gsrs->GetCount(); i++)
            {
                MdfModel::GridScaleRange* gsr = gsrs->GetAt(i);
                xml.append("<ScaleRange>\n");
                xml.append("<MinScale>");
                std::string sMinScale;
                MgUtil::DoubleToString(gsr->GetMinScale(), sMinScale);
                xml.append(sMinScale);
                xml.append("</MinScale>\n");
                xml.append("<MaxScale>");
                std::string sMaxScale;
                MgUtil::DoubleToString(gsr->GetMaxScale(), sMaxScale);
                xml.append(sMaxScale);
                xml.append("</MaxScale>\n");
                xml.append("</ScaleRange>\n");
            }
        }
        else if (NULL != dl)
        {
            xml.append("<ScaleRange>\n");
            xml.append("<MinScale>");
            std::string sMinScale;
            MgUtil::DoubleToString(dl->GetMinScale(), sMinScale);
            xml.append(sMinScale);
            xml.append("</MinScale>\n");
            xml.append("<MaxScale>");
            std::string sMaxScale;
            MgUtil::DoubleToString(dl->GetMaxScale(), sMaxScale);
            xml.append(sMaxScale);
            xml.append("</MaxScale>\n");
            xml.append("</ScaleRange>\n");
        }
        else
        {
            xml.append("<ScaleRange/>");
        }
    }
    else
        xml.append("<ScaleRange/>");

    xml.append("</Layer>");

    MG_SERVER_MAPPING_SERVICE_CATCH_AND_THROW(L"MgServerMappingService.CreateLayerItem")
}