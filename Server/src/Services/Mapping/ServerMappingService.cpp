//
//  Copyright (C) 2004-2008 by Autodesk, Inc.
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
#include "EMapRenderer.h"
#include "EMapUpdateRenderer.h"

#include "DefaultStylizer.h"

#include "FeatureTypeStyleVisitor.h"
#include "SymbolInstance.h"

#include "RSMgSymbolManager.h"
#include "SEMgSymbolManager.h"

#include "LegendPlotUtil.h"
#include "MgCSTrans.h"

#include "icons.h"

#define LEGEND_BITMAP_SIZE 16


//for use by observation mesh transformation
const STRING SRS_LL84 = L"GEOGCS[\"LL84\",DATUM[\"WGS 84\",SPHEROID[\"WGS 84\",6378137,298.25722293287],TOWGS84[0,0,0,0,0,0,0]],PRIMEM[\"Greenwich\",0],UNIT[\"Degrees\",0.01745329252]]";


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
/// Generates an EMap DWF that describes the specified map
/// </summary>
/// <param name="map">
/// Map object describing current state of map
/// </param>
/// <param name="sessionId">
/// Session identifier for client
/// </param>
/// <param name="mapAgentUri">
/// Http Uri for map agent
/// </param>
/// <param name="dwfVersion">
/// DWF specification supported/required by the client.  The DWF spec determines the
/// schema and file versions used to generate the DWFs sent back to the client.
/// </param>
/// <returns>
/// An EMap DWF
/// </returns>
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------
MgByteReader* MgServerMappingService::GenerateMap(MgMap* map,
    CREFSTRING sessionId, CREFSTRING mapAgentUri, MgDwfVersion* dwfVersion)
{
    Ptr<MgByteReader> byteReader;
    std::vector<MgByte*> uiGraphicSources;

    MG_SERVER_MAPPING_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerMappingService::GenerateMap()");

    //TODO: uncomment once dwf version is serializable and comes in
    if (0 == map || sessionId.empty() || mapAgentUri.empty() || 0 == dwfVersion)
    {
        throw new MgNullArgumentException(
            L"MgServerMappingService.GenerateMap", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    if (m_svcResource == NULL)
        InitializeResourceService();

    //temporary place for the resId of the Map
    Ptr<MgResourceIdentifier> mapResId = map->GetMapDefinition();

    //get the map definition
    auto_ptr<MdfModel::MapDefinition> mdf(MgMappingUtil::GetMapDefinition(m_svcResource, mapResId));

    MdfModel::Box2D box = mdf->GetExtents();
    RS_Bounds b(box.GetMinX(),
                box.GetMinY(),
                box.GetMaxX(),
                box.GetMaxY());

    //get a temporary file to write out EMap DWF to
    STRING dwfName = MgFileUtil::GenerateTempFileName(false, mdf->GetName());

    EMapRenderer dr(dwfName, mapAgentUri);

    //get the coordinate system unit scale
    MdfModel::MdfString srs = map->GetMapSRS();

    Ptr<MgCoordinateSystem> cs;

    if (!srs.empty())
    {
        //let's not fail here if coord sys conversion fails for the
        //map's coordinate system. Instead we will fail per layer at a later stage
        try
        {
            cs = m_pCSFactory->Create(srs);
        }
        catch (MgInvalidCoordinateSystemException* e)
        {
            e->Release();
        }
    }

    double metersPerUnit = (cs.p) ? cs->ConvertCoordinateSystemUnitsToMeters(1.0) : 1.0;
    RS_String units = (cs.p) ? cs->GetUnits() : L"";

    RS_Color bgcolor;
    StylizationUtil::ParseColor( mdf->GetBackgroundColor(), bgcolor);

    //These should not matter for GenerateMap
    double dMapScale = map->GetViewScale();
    double dpi = map->GetDisplayDpi();

    RS_MapUIInfo mapInfo(sessionId, mapResId->GetName(), map->GetObjectId(), srs, units, bgcolor);

    dr.StartMap(&mapInfo, b, dMapScale, dpi, metersPerUnit);

    //do layer groups
    Ptr<MgLayerGroupCollection> mggroups = map->GetLayerGroups();

    //add the groups first.
    //This is important to do before we do any layers
    //so that the layers can later find the guids for the groups.
    for (int k=0; k<mggroups->GetCount(); k++)
    {
        Ptr<MgLayerGroup> mggroup = mggroups->GetItem(k);
        Ptr<MgLayerGroup> mgparent = mggroup->GetGroup();

        //layer legend and ui specific information
        RS_UIGraphic uig(NULL, 0, mggroup->GetLegendLabel());
        RS_LayerUIInfo layerInfo( mggroup->GetName(),
                                  mggroup->GetObjectId(),
                                  false,   // selectable - does not apply to groups
                                  mggroup->GetVisible(),
                                  true,    // editable - does not apply to groups
                                  (mgparent.p) ? mgparent->GetName() : L"",
                                  (mgparent.p) ? mgparent->GetObjectId() : L"",
                                  mggroup->GetDisplayInLegend(),
                                  mggroup->GetExpandInLegend(),
                                  0.0,     // zorder - does not apply to groups
                                  uig);

        dr.AddLayerGroup(layerInfo);
    }

    //now do the layers
    Ptr<MgLayerCollection> mglayers = map->GetLayers();

    //iterate over layers
    for (int i = 0; i < mglayers->GetCount(); i++)
    {
        Ptr<MgLayerBase> mglayer = mglayers->GetItem(i);
        Ptr<MgLayerGroup> mgparent = mglayer->GetGroup();

        //get layer definition
        Ptr<MgResourceIdentifier> resId = mglayer->GetLayerDefinition();
        auto_ptr<MdfModel::LayerDefinition> ldf(MgMappingUtil::GetLayerDefinition(m_svcResource, resId));

        //base map layers are not editable
        bool bEditable = true;
        if (mglayer->GetLayerType() == MgLayerType::BaseMap)
            bEditable = false;

        //layer legend and ui specific information
        RS_UIGraphic uig(NULL, 0, mglayer->GetLegendLabel());
        RS_LayerUIInfo layerInfo( mglayer->GetName(),
                                  mglayer->GetObjectId(),
                                  mglayer->GetSelectable(),
                                  mglayer->GetVisible(),
                                  bEditable,
                                  (mgparent.p)? mgparent->GetName() : L"",
                                  (mgparent.p)? mgparent->GetObjectId() : L"",
                                  mglayer->GetDisplayInLegend(),
                                  mglayer->GetExpandInLegend(),
                                 -mglayer->GetDisplayOrder(),
                                  uig);

        MdfModel::VectorLayerDefinition* vl = dynamic_cast<MdfModel::VectorLayerDefinition*>(ldf.get());
        MdfModel::DrawingLayerDefinition* dl = dynamic_cast<MdfModel::DrawingLayerDefinition*>(ldf.get());
        MdfModel::GridLayerDefinition* gl = dynamic_cast<MdfModel::GridLayerDefinition*>(ldf.get());

        if (vl) // vector layer
        {
            dr.StartLayer(&layerInfo, NULL); //send null property mapping here
                                             //since it only matters when we stylize actual data

            MdfModel::VectorScaleRangeCollection* scr = vl->GetScaleRanges();

            for (int j=0; j < scr->GetCount(); j++)
            {
                MdfModel::VectorScaleRange* sr = scr->GetAt(j);
                std::list<RS_UIGraphic> uiGraphics;

                MakeUIGraphicsForScaleRange(uiGraphics, uiGraphicSources, sr);

                dr.AddScaleRange(sr->GetMinScale(), sr->GetMaxScale(), &uiGraphics);
            }

            dr.EndLayer();
        }
        else if (dl) // drawing layer
        {
            dr.StartLayer(&layerInfo, NULL);

            //it's a hardcoded icon
            std::list<RS_UIGraphic> uiGraphics;
            RS_UIGraphic gr((unsigned char*)DWF_LAYER_ICON, sizeof(DWF_LAYER_ICON), L"");
            uiGraphics.push_back(gr);

            dr.AddScaleRange( dl->GetMinScale(), dl->GetMaxScale(), &uiGraphics);

            dr.EndLayer();
        }
        else if (gl) // grid (raster) layer
        {
            dr.StartLayer(&layerInfo, NULL); //send null property mapping here
                                             //since it only matters when we stylize actual data

            MdfModel::GridScaleRangeCollection* scr = gl->GetScaleRanges();

            for (int j=0; j < scr->GetCount(); j++)
            {
                MdfModel::GridScaleRange* sr = scr->GetAt(j);

                //hardcoded graphic
                std::list<RS_UIGraphic> uiGraphics;
                RS_UIGraphic gr((unsigned char*)RASTER_LAYER_ICON, sizeof(RASTER_LAYER_ICON), L"");
                uiGraphics.push_back(gr);

                dr.AddScaleRange(sr->GetMinScale(), sr->GetMaxScale(), &uiGraphics);
            }

            dr.EndLayer();
        }
    }

    dr.EndMap();

    //write out the file
    Ptr<MgByteSource> bs = new MgByteSource(dwfName, true);

    bs->SetMimeType(MgMimeType::Dwf);
    byteReader = bs->GetReader();

    MG_SERVER_MAPPING_SERVICE_CATCH_AND_THROW(L"MgServerMappingService.GenerateMap")

    //release legend bitmap buffers
    for (std::vector<MgByte*>::iterator iter = uiGraphicSources.begin(); iter != uiGraphicSources.end(); iter++)
    {
        if (*iter)
            (*iter)->Release();
    }

    return byteReader.Detach();
}


///----------------------------------------------------------------------------
/// <summary>
/// Generates an EMapTransaction DWF that contains any required updates to the EMap
/// meta-data and layer graphics based on the state of the specified map
/// </summary>
/// <param name="map">
/// Map object specifying current state of map
/// </param>
/// <param name="seqNo">
/// Update sequence number
/// </param>
/// <param name="dwfVersion">
/// DWF specification supported/required by the client.  The DWF spec determines the
/// schema and file versions used to generate the DWFs sent back to the client.
/// </param>
/// <returns>
/// An EMap DWF
/// </returns>
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------
MgByteReader* MgServerMappingService::GenerateMapUpdate(MgMap* map,
    INT32 seqNo, MgDwfVersion* dwfVersion)
{
    Ptr<MgByteReader> byteReader;
    MgCSTrans* xformToLL = NULL;
    std::vector<MgByte*> uiGraphicSources;

    MG_SERVER_MAPPING_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerMappingService::GenerateMapUpdate()");

    if (0 == map || 0 == dwfVersion)
    {
        throw new MgNullArgumentException(
            L"MgServerMappingService.GenerateMapUpdate", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    if (m_svcResource == NULL)
        InitializeResourceService();
    if (m_svcFeature == NULL)
        InitializeFeatureService();
    if (m_svcDrawing == NULL)
        InitializeDrawingService();

    //temporary place for the resId of the Map
    Ptr<MgResourceIdentifier> mapResId = map->GetMapDefinition();

    Ptr<MgEnvelope> box = map->GetDataExtent();
    Ptr<MgCoordinate> ll = box->GetLowerLeftCoordinate();
    Ptr<MgCoordinate> ur = box->GetUpperRightCoordinate();
    RS_Bounds b(ll->GetX(),
                ll->GetY(),
                ur->GetX(),
                ur->GetY());

    //get a temporary file to write out EMapTransaction DWF to
    STRING dwfName = MgFileUtil::GenerateTempFileName(false, map->GetName());

    //convert the map coordinate system from srs wkt to a mentor cs structure
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

    //set up the map coord sys to lat lon transformation -- for use by the observation mesh
    if (dstCs)
    {
        //don't create an observation mesh for an aribtrary XY coordinate system
        if(dstCs->GetType() != MgCoordinateSystemType::Arbitrary)
        {
            try
            {
                Ptr<MgCoordinateSystem> ll84 = m_pCSFactory->Create(SRS_LL84);
                xformToLL = new MgCSTrans(dstCs, ll84);
            }
            catch(MgException* mge)
            {
                mge->Release();
                xformToLL = NULL;
            }
        }
    }

    //set up map-specific params, like bg color, scale, etc.
    //and initialize the appropriate map renderer
    RS_Color bgcolor;
    StylizationUtil::ParseColor( map->GetBackgroundColor(), bgcolor);

    double dMapScale = map->GetViewScale();
    double dpi       = map->GetDisplayDpi();

    // get the session ID
    STRING sessionId;
    MgUserInformation* userInfo = MgUserInformation::GetCurrentUserInfo();
    if (userInfo != NULL)
        sessionId = userInfo->GetMgSessionId();

    // prepare the renderer
    EMapUpdateRenderer dr(dwfName, seqNo);

    RSMgSymbolManager mgr(m_svcResource);
    dr.SetSymbolManager(&mgr);

    SEMgSymbolManager semgr(m_svcResource);
    DefaultStylizer ds(&semgr);

    RS_MapUIInfo mapInfo(sessionId, map->GetName(), map->GetObjectId(), srs, units, bgcolor);

    //begin map stylization
    dr.StartMap(&mapInfo, b, dMapScale, dpi, metersPerUnit, xformToLL);

        Ptr<MgLayerGroupCollection> mggroups = map->GetLayerGroups();

        //add the groups first.
        //This is important to do before we do any layers
        //so that the layers can later find the guids for the groups.
        for (int k=0; k<mggroups->GetCount(); k++)
        {
            Ptr<MgLayerGroup> mggroup = mggroups->GetItem(k);
            Ptr<MgLayerGroup> mgparent = mggroup->GetGroup();

            //layer legend and ui specific information
            RS_UIGraphic uig(NULL, 0, mggroup->GetLegendLabel());
            RS_LayerUIInfo layerInfo( mggroup->GetName(),
                                      mggroup->GetObjectId(),
                                      false,   // selectable - does not apply to groups
                                      mggroup->GetVisible(),
                                      true,    // editable - does not apply to groups
                                      (mgparent.p) ? mgparent->GetName() : L"",
                                      (mgparent.p) ? mgparent->GetObjectId() : L"",
                                      mggroup->GetDisplayInLegend(),
                                      mggroup->GetExpandInLegend(),
                                      0.0,     // zorder - does not apply to groups
                                      uig);

            dr.AddLayerGroupInfo(layerInfo);
        }

        //now add the layer information
        //we do this for all layers -- I'm not sure if
        //we could only do it for the ones that need update...
        //we'll see
        Ptr<MgLayerCollection> layers = map->GetLayers();

        for (int k=0; k<layers->GetCount(); k++)
        {
            Ptr<MgLayerBase> mglayer = layers->GetItem(k);
            Ptr<MgLayerGroup> mggroup = mglayer->GetGroup();

            //base map layers are not editable
            bool bEditable = true;
            if (mglayer->GetLayerType() == MgLayerType::BaseMap)
                bEditable = false;

            //layer legend and ui specific information
            RS_UIGraphic uig(NULL, 0, mglayer->GetLegendLabel());
            RS_LayerUIInfo layerInfo( mglayer->GetName(),
                                      mglayer->GetObjectId(),
                                      mglayer->GetSelectable(),
                                      mglayer->GetVisible(),
                                      bEditable,
                                      (mggroup.p) ? mggroup->GetName() : L"",
                                      (mggroup.p) ? mggroup->GetObjectId() : L"",
                                      mglayer->GetDisplayInLegend(),
                                      mglayer->GetExpandInLegend(),
                                     -mglayer->GetDisplayOrder(),
                                      uig);

            dr.AddLayerInfo(layerInfo);
        }

        //now process the changes to the Map
        //we need to go over the changelists sent us by the webtier
        //and describe them to the EMapUpdateRenderer so that it can
        //generate appropriate transaction commands into the EMap DWF
        Ptr<MgNamedCollection> changelists = map->GetChangeLists();
        for (int i=0; i<changelists->GetCount(); i++)
        {
            Ptr<MgChangeList> changelist = (MgChangeList*)changelists->GetItem(i);

            for (int j=0; j<changelist->GetChangeCount(); j++)
            {
                Ptr<MgObjectChange> change = changelist->GetChangeAt(j);

                MgObjectChange::ChangeType type = change->GetType();
                switch (type)
                {
                case MgObjectChange::removed:
                    {
                        if (changelist->IsLayer())
                            dr.CmdRemoveLayer(changelist->GetObjectId());
                        else
                            dr.CmdRemoveLayerGroup(changelist->GetObjectId());
                    }
                    break;

                case MgObjectChange::added:
                    {
                        if (changelist->IsLayer())
                        {
                            //add the layer
                            dr.CmdAddLayer(changelist->GetObjectId());

                            //and now we need to add its scale ranges
                            //TODO: may be we can move this to the layer iteration
                            //loop above, but doing it for all layers might affect performance
                            //and in most all cases it is not needed during map updates

                            //see also similar code in GenerateMap above

                            //find the corresponding MgLayer first
                            Ptr<MgLayerBase> layerToAdd;
                            for (int i=0; i<layers->GetCount(); i++)
                            {
                                Ptr<MgLayerBase> temp = layers->GetItem(i);
                                if (temp->GetObjectId() == changelist->GetObjectId())
                                {
                                    layerToAdd = temp;
                                    break;
                                }
                            }

                            if (layerToAdd.p)
                            {
                                //get layer definition
                                Ptr<MgResourceIdentifier> resId = layerToAdd->GetLayerDefinition();
                                auto_ptr<MdfModel::LayerDefinition> ldf(MgMappingUtil::GetLayerDefinition(m_svcResource, resId));
                                MdfModel::VectorLayerDefinition* vl = dynamic_cast<MdfModel::VectorLayerDefinition*>(ldf.get());
                                MdfModel::DrawingLayerDefinition* dl = dynamic_cast<MdfModel::DrawingLayerDefinition*>(ldf.get());
                                MdfModel::GridLayerDefinition* gl = dynamic_cast<MdfModel::GridLayerDefinition*>(ldf.get());

                                if (vl) // vector layer
                                {
                                    MdfModel::VectorScaleRangeCollection* scr = vl->GetScaleRanges();

                                    for (int j=0; j < scr->GetCount(); j++)
                                    {
                                        MdfModel::VectorScaleRange* sr = scr->GetAt(j);
                                        std::list<RS_UIGraphic> uiGraphics;

                                        MakeUIGraphicsForScaleRange(uiGraphics, uiGraphicSources, sr);

                                        STRING oid = changelist->GetObjectId();
                                        dr.AddScaleRange(oid, sr->GetMinScale(), sr->GetMaxScale(), &uiGraphics);
                                    }
                                }
                                else if (dl) // drawing layer
                                {
                                    //it's a hardcoded icon
                                    std::list<RS_UIGraphic> uiGraphics;
                                    RS_UIGraphic gr((unsigned char*)DWF_LAYER_ICON, sizeof(DWF_LAYER_ICON), L"");
                                    uiGraphics.push_back(gr);

                                    STRING oid = changelist->GetObjectId();
                                    dr.AddScaleRange(oid, dl->GetMinScale(), dl->GetMaxScale(), &uiGraphics);
                                }
                                else if (gl) // grid (raster) layer
                                {
                                    MdfModel::GridScaleRangeCollection* scr = gl->GetScaleRanges();

                                    for (int j=0; j < scr->GetCount(); j++)
                                    {
                                        MdfModel::GridScaleRange* sr = scr->GetAt(j);

                                        //hardcoded graphic
                                        std::list<RS_UIGraphic> uiGraphics;
                                        RS_UIGraphic gr((unsigned char*)RASTER_LAYER_ICON, sizeof(RASTER_LAYER_ICON), L"");
                                        uiGraphics.push_back(gr);

                                        STRING oid = changelist->GetObjectId();
                                        dr.AddScaleRange(oid, sr->GetMinScale(), sr->GetMaxScale(), &uiGraphics);
                                    }
                                }
                            }
                        }
                        else
                            dr.CmdAddLayerGroup(changelist->GetObjectId());
                    }
                    break;

                case MgObjectChange::visibilityChanged:
                case MgObjectChange::parentChanged:
                case MgObjectChange::selectabilityChanged:
                    {
                        if (changelist->IsLayer())
                            dr.CmdUpdateLayer(changelist->GetObjectId());
                        else
                            dr.CmdUpdateLayerGroup(changelist->GetObjectId());
                    }
                    break;

                case MgObjectChange::displayInLegendChanged:
                case MgObjectChange::legendLabelChanged:
                    {
                        if (changelist->IsLayer())
                            dr.CmdUpdateUIGraphicForLayer(changelist->GetObjectId());
                        else
                            dr.CmdUpdateUIGraphicForLayerGroup(changelist->GetObjectId());
                    }
                    break;

                }
            }
        }

        //transfer layers to a temporary collection
        //here it doesn't matter but in the rendering service it does
        Ptr<MgReadOnlyLayerCollection> rolc = new MgReadOnlyLayerCollection();

        for (int u=0; u<layers->GetCount(); u++)
        {
            Ptr<MgLayerBase> lr = layers->GetItem(u);
            rolc->Add(lr);
        }

        //finally generate graphics data for the layers that need it
        MgMappingUtil::StylizeLayers(m_svcResource, m_svcFeature, m_svcDrawing, m_pCSFactory, map,
                                     rolc, NULL, &ds, &dr, dstCs, false, true, map->GetViewScale());

    dr.EndMap();

    //write out the file
    Ptr<MgByteSource> bs = new MgByteSource(dwfName, true);

    bs->SetMimeType(MgMimeType::Dwf);
    byteReader = bs->GetReader();

    MG_SERVER_MAPPING_SERVICE_CATCH_AND_THROW(L"MgServerMappingService.GenerateMapUpdate")

    //release legend bitmap buffers
    for (std::vector<MgByte*>::iterator iter = uiGraphicSources.begin();
        iter != uiGraphicSources.end(); iter++)
        if (*iter) (*iter)->Release();

    //finally clean up if needed
    if (xformToLL)
        delete xformToLL;

    return byteReader.Detach();
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

    if (NULL == map  || NULL == dwfVersion || NULL == plotSpec )
    {
        throw new MgNullArgumentException(
            L"MgServerMappingService.GeneratePlot", __LINE__, __WFILE__, NULL, L"", NULL);
    }

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

    if (NULL == map  || NULL == center || NULL == dwfVersion || NULL == plotSpec )
    {
        throw new MgNullArgumentException(
            L"MgServerMappingService.GeneratePlot", __LINE__, __WFILE__, NULL, L"", NULL);
    }

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

    if (NULL == map  || NULL == extents || NULL == plotSpec || NULL == dwfVersion )
    {
        throw new MgNullArgumentException(
            L"MgServerMappingService.GeneratePlot", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // Make a copy of the extents
    Ptr<MgCoordinate> oldll = extents->GetLowerLeftCoordinate();
    Ptr<MgCoordinate> oldur = extents->GetUpperRightCoordinate();
    Ptr<MgCoordinate> ll = new MgCoordinateXY(oldll->GetX(), oldll->GetY());
    Ptr<MgCoordinate> ur = new MgCoordinateXY(oldur->GetX(), oldur->GetY());
    if (ll == NULL || ur == NULL)
    {
        throw new MgNullArgumentException(L"MgServerMappingService.GeneratePlot", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    Ptr<MgEnvelope> env = new MgEnvelope(ll, ur);

    // Create a MgMapPlot which will be passed to GenerateMultiPlot
    Ptr<MgMapPlot> mapPlot = new MgMapPlot(map, env, expandToFit, plotSpec, layout);

    // Add it to a MgMapPlotCollecion
    Ptr<MgMapPlotCollection> mapPlots = new MgMapPlotCollection();
    if (mapPlot == NULL || mapPlots == NULL)
    {
        throw new MgNullArgumentException(L"MgServerMappingService.GeneratePlot", __LINE__, __WFILE__, NULL, L"", NULL);
    }
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

    if (0 == mapPlots || 0 == dwfVersion)
    {
        throw new MgNullArgumentException(
            L"MgServerMappingService.GenerateMultiPlot", __LINE__, __WFILE__, NULL, L"", NULL);
    }

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

    RSMgSymbolManager mgr(m_svcResource);
    dr.SetSymbolManager(&mgr);

    // process the MapPlot collection
    for (int nMapPlotIndex = 0; nMapPlotIndex < mapPlots->GetCount(); nMapPlotIndex++)
    {
        Ptr<MgMapPlot> mapPlot = mapPlots->GetItem(nMapPlotIndex);
        if (NULL == mapPlot)
        {
            throw new MgNullReferenceException(L"MgServerMappingService.GenerateMultiPlot", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgMap> map = mapPlot->GetMap();
        if (NULL == map)
        {
            throw new MgNullReferenceException(L"MgServerMappingService.GenerateMultiPlot", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgPlotSpecification> plotSpec = mapPlot->GetPlotSpecification();
        if (NULL == plotSpec)
        {
            throw new MgNullReferenceException(L"MgServerMappingService.GenerateMultiPlot", __LINE__, __WFILE__, NULL, L"", NULL);
        }

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
        if (env == NULL)
        {
            throw new MgNullReferenceException(L"MgServerMappingService.GenerateMultiPlot", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgCoordinate> ll = env->GetLowerLeftCoordinate();
        Ptr<MgCoordinate> ur = env->GetUpperRightCoordinate();
        if (ll == NULL || ur == NULL)
        {
            throw new MgNullReferenceException(L"MgServerMappingService.GenerateMultiPlot", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        RS_Bounds b(ll->GetX(), ll->GetY(), ur->GetX(), ur->GetY());

        //if requested data extent is not valid, use map definition extent
        // this may be removed eventually, but is a good sanity check nonetheless
        if (!b.IsValid())
        {
            Ptr<MgEnvelope> env2 = map->GetMapExtent();
            if (env2 == NULL)
            {
                throw new MgNullReferenceException(L"MgServerMappingService.GenerateMultiPlot", __LINE__, __WFILE__, NULL, L"", NULL);
            }

            Ptr<MgCoordinate> ll2 = env2->GetLowerLeftCoordinate();
            Ptr<MgCoordinate> ur2 = env2->GetUpperRightCoordinate();
            if (ll2 == NULL || ur2 == NULL)
            {
                throw new MgNullReferenceException(L"MgServerMappingService.GenerateMultiPlot", __LINE__, __WFILE__, NULL, L"", NULL);
            }

            b.minx = ll2->GetX();
            b.miny = ll2->GetY();
            b.maxx = ur2->GetX();
            b.maxy = ur2->GetY();
        }

        // Create a simple print layout containing only the map
        Ptr<MgPrintLayout> printLayout = new MgPrintLayout();
        if (printLayout == NULL)
        {
            throw new MgNullReferenceException(L"MgServerMappingService.GenerateMultiPlot", __LINE__, __WFILE__, NULL, L"", NULL);
        }

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
        if (center == NULL || extents == NULL)
        {
            throw new MgNullReferenceException(L"MgServerMappingService.GenerateMultiPlot", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        switch (mapPlot->GetMapPlotInstruction())
        {
        case MgMapPlotInstruction::UseMapCenterAndScale:
            {
                dMapScale = map->GetViewScale();
                if (dMapScale <= 0)
                {
                    Ptr<MgEnvelope> extents = map->GetDataExtent();
                    if (extents == NULL)
                    {
                        throw new MgNullReferenceException(L"MgServerMappingService.GenerateMultiPlot", __LINE__, __WFILE__, NULL, L"", NULL);
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
                if (plotll == NULL || plotur == NULL)
                {
                    throw new MgNullReferenceException(L"MgServerMappingService.GenerateMultiPlot", __LINE__, __WFILE__, NULL, L"", NULL);
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
                    throw new MgNullReferenceException(L"MgServerMappingService.GenerateMultiPlot", __LINE__, __WFILE__, NULL, L"", NULL);
                }
                Ptr<MgPoint> centerpt = new MgPoint(center);
                if (centerpt == NULL)
                {
                    throw new MgNullReferenceException(L"MgServerMappingService.GenerateMultiPlot", __LINE__, __WFILE__, NULL, L"", NULL);
                }
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
        MgUserInformation* userInfo = MgUserInformation::GetCurrentUserInfo();
        if (userInfo != NULL)
            sessionId = userInfo->GetMgSessionId();

        RS_MapUIInfo mapInfo(sessionId, map->GetName(), L"", srs, units, layoutColor);

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
        if (layers == NULL)
        {
            throw new MgNullReferenceException(L"MgServerMappingService.GenerateMultiPlot", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        // Define a polygon to represent the map extents and fill it with the map background color
        dr.StartLayer(NULL, NULL);
        LineBuffer lb(4);
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

    if (0 == map || 0 == dwfVersion || 0 == plotSpec)
    {
        throw new MgNullArgumentException(
            L"MgServerMappingService.GenerateLegendPlot", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    if (m_svcResource == NULL)
        InitializeResourceService();

    // Here is where we have to get the layers, rules, etc.
    // But first let's generate a box for the legend border...

    // request extenst
    Ptr<MgEnvelope> env = map->GetDataExtent();
    if (env == NULL)
    {
        throw new MgNullReferenceException(L"MgServerMappingService.GenerateMultiPlot", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    Ptr<MgCoordinate> ll = env->GetLowerLeftCoordinate();
    Ptr<MgCoordinate> ur = env->GetUpperRightCoordinate();
    if (ll == NULL|| ur == NULL)
    {
        throw new MgNullReferenceException(L"MgServerMappingService.GenerateMultiPlot", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    RS_Bounds b(ll->GetX(), ll->GetY(), ur->GetX(), ur->GetY());

    // get a temporary file to write out EPlot DWF to
    STRING dwfName = MgFileUtil::GenerateTempFileName(false, L"legendplot");

    EPlotRenderer dr(dwfName, plotSpec->GetPaperWidth(), plotSpec->GetPaperHeight(), plotSpec->GetPageSizeUnits());

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

    RS_Color mapBgColor(255, 255, 255, 0);
    RS_MapUIInfo mapInfo(L"", map->GetName(), L"", srs, units, mapBgColor);

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

    if (0 == resource)
    {
        throw new MgNullArgumentException(
            L"MgServerMappingService.GenerateLegendImage", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    if (m_svcResource == NULL)
        InitializeResourceService();

    auto_ptr<MdfModel::LayerDefinition> ldf(MgMappingUtil::GetLayerDefinition(m_svcResource, resource));
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
            MdfModel::FeatureTypeStyleCollection* ftsc = range->GetFeatureTypeStyles();
            int numFTS = ftsc->GetCount();

            MdfModel::FeatureTypeStyle* fts = NULL;

            //if just one style use that
            if (numFTS == 1)
                fts = ftsc->GetAt(0);
            else
            {
                //find the right feature type style in the collection
                //by matching it against the requested geometry type
                for (int j=0; j<numFTS; ++j)
                {
                    MdfModel::FeatureTypeStyle* temp = ftsc->GetAt(j);
                    if (FeatureTypeStyleSupportsGeometry(temp, geomType, themeCategory))
                    {
                        fts = temp;
                        break;
                    }
                }
            }

            if (fts)
                byteReader = MgMappingUtil::DrawFTS(m_svcResource, fts, imgWidth, imgHeight, themeCategory);
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


// Returns true if the supplied feature type style is compatible with the supplied geometry type.
bool MgServerMappingService::FeatureTypeStyleSupportsGeometry(MdfModel::FeatureTypeStyle* fts, INT32 geomType, INT32 themeCategory)
{
    if (fts == NULL)
        return false;

    FeatureTypeStyleVisitor::eFeatureTypeStyle ftsType = FeatureTypeStyleVisitor::DetermineFeatureTypeStyle(fts);

    // composite type styles require some digging
    if (ftsType == FeatureTypeStyleVisitor::ftsComposite)
    {
        RuleCollection* rules = fts->GetRules();
        if (!rules)
            return false;

        // for composite type styles we need to pay attention to the category

        // if there's one rule then always use it
        if (rules->GetCount() == 1)
            themeCategory = 0;

        // validate category index
        if (themeCategory < 0 || themeCategory >= rules->GetCount())
            return false;

        // get correct theme rule
        CompositeRule* rule = (CompositeRule*)rules->GetAt(themeCategory);

        CompositeSymbolization* csym = rule->GetSymbolization();
        if (csym)
        {
            SymbolInstanceCollection* symbolInstances = csym->GetSymbolCollection();
            for (int i=0; i<symbolInstances->GetCount(); ++i)
            {
                SymbolInstance* symbolInstance = symbolInstances->GetAt(i);

                // use the geometry context to determine if the composite type style is compatible
                SymbolInstance::GeometryContext geomContext = symbolInstance->GetGeometryContext();

                if (geomContext == SymbolInstance::gcUnspecified)
                    return true;

                switch (geomType)
                {
                    case MgGeometryType::Point:
                    case MgGeometryType::MultiPoint:
                        if (geomContext == SymbolInstance::gcPoint)
                            return true;
                        break;

                    case MgGeometryType::LineString:
                    case MgGeometryType::MultiLineString:
                    case MgGeometryType::CurveString:
                    case MgGeometryType::MultiCurveString:
                        if (geomContext == SymbolInstance::gcLineString)
                            return true;
                        break;

                    case MgGeometryType::Polygon:
                    case MgGeometryType::MultiPolygon:
                    case MgGeometryType::CurvePolygon:
                    case MgGeometryType::MultiCurvePolygon:
                        if (geomContext == SymbolInstance::gcPolygon)
                            return true;
                        break;

                    case MgGeometryType::MultiGeometry:
                    default:
                        // just accept the first one
                        return true;
                }
            }
        }

        // composite type style is incompatible
        return false;
    }

    // non-composite type style - the category is irrelevant...
    switch (geomType)
    {
        case MgGeometryType::Point:
        case MgGeometryType::MultiPoint:
            if (ftsType == FeatureTypeStyleVisitor::ftsPoint)
                return true;
            break;

        case MgGeometryType::LineString:
        case MgGeometryType::MultiLineString:
        case MgGeometryType::CurveString:
        case MgGeometryType::MultiCurveString:
            if (ftsType == FeatureTypeStyleVisitor::ftsLine)
                return true;
            break;

        case MgGeometryType::Polygon:
        case MgGeometryType::MultiPolygon:
        case MgGeometryType::CurvePolygon:
        case MgGeometryType::MultiCurvePolygon:
            if (ftsType == FeatureTypeStyleVisitor::ftsArea)
                return true;
            break;

        case MgGeometryType::MultiGeometry:
        default:
            // just accept the first one
            return true;
    }

    // non-composite type style is incompatible
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
