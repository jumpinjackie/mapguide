//
//  Copyright (C) 2004-2010 by Autodesk, Inc.
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

#include "stdafx.h"
#include "EMapRenderer.h"

#include "dwfcore/UUID.h"
#include "dwfcore/MIME.h"
#include "dwf/package/GraphicResource.h"
#include "dwf/package/writer/PackageWriter.h"
#include "dwf/package/writer/extensions/6.01/PackageVersionExtension.h"
#include "dwfemap/EMapVersion.h"
#include "dwfemap/package/EMapConstants.h"
#include "dwfemap/package/EMapSection.h"
#include "dwfemap/package/Background.h"
#include "dwfemap/package/CoordinateSpace.h"
#include "dwfemap/package/Bounds.h"
#include "dwfemap/package/LinearUnit.h"
#include "dwfemap/package/EMapInterface.h"
#include "dwfemap/package/Layer.h"
#include "dwfemap/package/ScaleRange.h"
#include "dwfemap/package/UIGraphic.h"
#include "dwfemap/package/LayerGroup.h"

EMapRenderer::EMapRenderer(const RS_String& filename,
                           const RS_String& agentUri)
                           : DWFRenderer()
{
    m_pPage = NULL;
    m_uuid = new DWFUUID;
    m_agentUri = agentUri;
    m_dwfFilename = filename;
    m_pLayer = NULL;
}


EMapRenderer::~EMapRenderer()
{
    delete m_uuid;
}


void EMapRenderer::StartMap(RS_MapUIInfo*    mapInfo,
                            RS_Bounds&       extents,
                            double           mapScale,
                            double           dpi,
                            double           metersPerUnit,
                            CSysTransformer* xformToLL)
{
    //init super first
    DWFRenderer::StartMap(mapInfo, extents, mapScale, dpi, metersPerUnit, xformToLL);

    DWFSource oSource( mapInfo->name().c_str(), L"MapGuide Server", L"" );

    DWFBackground bg(mapInfo->bgcolor().argb());

    //we want to be able to pan around everywhere
    DWFBounds fullView( -DBL_MAX, //minx
                         DBL_MAX, //maxx
                        -DBL_MAX, //miny
                         DBL_MAX);//maxy

    //...but the initial view is equal to the
    //user specified map extent
    DWFBounds initialView(extents.minx,
                          extents.maxx,
                          extents.miny,
                          extents.maxy);

    DWFLinearUnit unit(mapInfo->units().c_str(), metersPerUnit);
    DWFCoordinateSpace cs(mapInfo->coordsys().c_str(), unit, fullView, &initialView);

    m_pPage = DWFCORE_ALLOC_OBJECT( DWFEMapSection(
                        mapInfo->name().c_str(), //zTitle
                        mapInfo->guid().c_str(), //zObjectId //here we use GUID given by web tier!!!
                        oSource,         //rSource
                        bg,              //background
                        cs,              //coord sys
                        m_agentUri.c_str()  //map agent url
                        ));

    // _AW_Session and _AW_MapName are required map properties
    // DO NOT CHANGE THESE PROPERTY NAMES - the DWF Viewer
    DWFProperty propM1(L"_AW_Session", mapInfo->session().c_str());
    DWFProperty propM2(L"_AW_MapName", mapInfo->name().c_str());

    // add properties to map
    m_pPage->addProperty(&propM1, false);
    m_pPage->addProperty(&propM2, false);

    m_pLayer = NULL;
}


void EMapRenderer::StartLayer(RS_LayerUIInfo*      layerInfo,
                              RS_FeatureClassInfo* /*classInfo*/)
{
    // correlate the UIGraphic object ID with the layer object ID
    DWFString uigGuid = GetUIGraphicObjectIdFromLayerObjectId(layerInfo->guid().c_str());
    DWFUIGraphic* pGraphic = DWFCORE_ALLOC_OBJECT(
        DWFUIGraphic( uigGuid,
                    layerInfo->graphic().label().c_str(),
                    layerInfo->show(),
                    !layerInfo->expand()
                    ));

    // Construct a Layer to be added to the Map
    DWFEMapLayer* pLayer = DWFCORE_ALLOC_OBJECT( DWFEMapLayer(
                                        layerInfo->name().c_str(),
                                        layerInfo->guid().c_str(),
                                        layerInfo->groupguid().c_str(),
                                        layerInfo->visible(),
                                        layerInfo->selectable(),
                                        layerInfo->editable(),
                                        pGraphic //UI graphic
                                        ));

    DWFImageResource* imgRes = CreateImageResource(layerInfo->graphic().data(), layerInfo->graphic().length());
    if (imgRes)
    {
        imgRes->setParentObjectID(pGraphic->objectID());
        ((DWFEMapSection*)m_pPage)->addResource(imgRes, true);
    }

    // Add the Layer and LayerGroup to the Map
    ((DWFEMapSection*)m_pPage)->addLayer(pLayer);

    m_pLayer = pLayer;

    //no need to call superclass StartLayer since we are not stylizing
}


//adds a scale range to current layer
void EMapRenderer::AddScaleRange(double min,
                                 double max,
                                 std::list<RS_UIGraphic>* uiGraphics)
{
    // Construct two scaleRanges to add to the Layer
    DWFScaleRange* pScaleR = DWFCORE_ALLOC_OBJECT( DWFScaleRange(
                                                    m_uuid->next(false), // ObjectId
                                                    min,                 // Min
                                                    max));               // Max

    // Add the Scale Range to the Layer
    m_pLayer->addScaleRange(pScaleR);

    //now add the theming ui graphics for this scale range
    if (uiGraphics)
    {
        std::list<RS_UIGraphic>::iterator iter = uiGraphics->begin();
        for (; iter != uiGraphics->end(); iter++)
        {
            // no need to correlate the UIGraphic object ID with any other object ID
            DWFUIGraphic* pGraphic = DWFCORE_ALLOC_OBJECT(
                DWFUIGraphic( m_uuid->next(false), (*iter).label().c_str(), true, false));

            pScaleR->addUIGraphic(pGraphic);

            if ((*iter).length() > 0 && (*iter).data())
            {
                DWFImageResource* imgRes = CreateImageResource((*iter).data(), (*iter).length());
                if (imgRes)
                {
                    imgRes->setParentObjectID(pGraphic->objectID());
                    ((DWFEMapSection*)m_pPage)->addResource(imgRes, true);
                }
            }
        }
    }
}


void EMapRenderer::EndLayer()
{
    //NOTE: we do not have to free m_pLayer since the EMap owns it
    m_pLayer = NULL;

    //no need to call superclass EndLayer since we are not stylizing
}


void EMapRenderer::EndMap()
{
    //first we need to replace layer group parent names by the
    //parents' uuids. This is done as post-processing step
    //so that we know all groups are defined first
    for (layerinfo_map::iterator iter = m_hGroups.begin();
        iter != m_hGroups.end(); iter++)
    {
        RS_LayerUIInfo layerinfo = iter->second;

        // correlate the UIGraphic object ID with the layer group object ID
        DWFString uigGuid = GetUIGraphicObjectIdFromLayerObjectId(layerinfo.guid().c_str());
        DWFUIGraphic* pGraphic =
            DWFCORE_ALLOC_OBJECT( DWFUIGraphic(uigGuid,
                                               layerinfo.graphic().label().c_str(),
                                               layerinfo.show(),
                                               !layerinfo.expand()) );

        //create a DWF layer group with the correct id
        DWFEMapLayerGroup* pGroup =
            DWFCORE_ALLOC_OBJECT( DWFEMapLayerGroup(layerinfo.name().c_str(),
                                                    layerinfo.guid().c_str(),
                                                    layerinfo.groupguid().c_str(),
                                                    layerinfo.visible(),
                                                    pGraphic) );

        // Add the LayerGroup to the Map
        ((DWFEMapSection*)m_pPage)->addLayerGroup(pGroup);
    }

    try
    {
        ///////////////////////////////////////////////////////////////////
        // Write out the DWF

        DWFFile oDWF( m_dwfFilename.c_str() );

        DWFPackageVersionExtension* pVersionExtension =
            DWFCORE_ALLOC_OBJECT( DWFPackageVersionTypeInfoExtension(DWFEMapInterface::kzEMap_ID) );

        DWFPackageWriter oWriter( oDWF, L"", pVersionExtension );

        DWFInterface* pEMapInterface =
            DWFCORE_ALLOC_OBJECT( DWFEMapInterface(DWFEMapInterface::kzEMap_Name,
                                                   DWFEMapInterface::kzEMap_HRef,
                                                   DWFEMapInterface::kzEMap_ID) );

        //the writer takes ownership of the pEMapInterface
        oWriter.addSection( m_pPage, pEMapInterface );

        //TODO: extract strings
        oWriter.write( MAP_PRODUCT_AUTHOR,
                       MAP_PRODUCT_NAME,
                       MAP_PRODUCT_VERSION,
                       MAP_PRODUCT_AUTHOR,
                       _DWF_FORMAT_EMAP_VERSION_CURRENT_STRING,
                       DWFZipFileDescriptor::eZipFastest);

    }
    catch (DWFException& )
    {
        _ASSERT(false);
        //TODO: should we just let the mapping service catch that?
    }

    //NOTE: we should not free the EMapSection since the DWF owns it
    m_pPage = NULL;

    //call super so that temp files are deleted
    DWFRenderer::EndMap();
}


void EMapRenderer::AddLayerGroup(RS_LayerUIInfo& layerInfo)
{
    //Note we cannot add the layer group to the map here.
    //Layer groups can have otehr layer groups as parents
    //and these may not be available yet so we do not necessarily
    //know their uuid. We need to keep track of all groups and
    //do some post-processing in EndMap

    m_hGroups[std::wstring(layerInfo.name())] = layerInfo;
}


DWFImageResource* EMapRenderer::CreateImageResource(unsigned char* data, int len)
{
    if (len == 0 || data == NULL)
        return NULL;

    //create an image resource with the icon and add it to the section
    DWFImageResource* imgRes = DWFCORE_ALLOC_OBJECT( DWFImageResource (
                                                        L"",
                                                        DWFEMAPXML::kzRole_LegendImage, //role
                                                        DWFMIME::kzMIMEType_PNG //mime type
                                                        ));

    DWFBufferInputStream* pImageStream = DWFCORE_ALLOC_OBJECT(
                                                DWFBufferInputStream
                                                (
                                                data,
                                                len
                                                ));

    // configure the resource
    double anTransform[4][4] = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
    double anExtents[4] = { 0, 0, 31, 31 };
    imgRes->configureGraphic( (const double*)anTransform, (const double*)anExtents );

    imgRes->setInputStream(pImageStream);

    return imgRes;
}
