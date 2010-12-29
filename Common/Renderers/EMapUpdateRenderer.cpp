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
#include "EMapUpdateRenderer.h"

#include "dwfcore/File.h"
#include "dwfcore/String.h"
#include "dwfcore/MIME.h"
#include "dwfcore/StreamFileDescriptor.h"
#include "dwfcore/FileInputStream.h"
#include "dwfcore/UUID.h"
#include "dwfcore/BufferOutputStream.h"
#include "dwfcore/BufferInputStream.h"

#include "dwf/package/writer/PackageWriter.h"
#include "dwf/package/writer/extensions/6.01/PackageVersionExtension.h"
#include "dwf/package/GraphicResource.h"

#include "dwfemap/EMapVersion.h"
#include "dwfemap/package/EMapSection.h"
#include "dwfemap/package/EMapConstants.h"
#include "dwfemap/package/Background.h"
#include "dwfemap/package/CoordinateSpace.h"
#include "dwfemap/package/Bounds.h"
#include "dwfemap/package/LinearUnit.h"
#include "dwfemap/package/EMapInterface.h"
#include "dwfemap/package/Layer.h"
#include "dwfemap/package/LayerGroup.h"
#include "dwfemap/package/transaction/TransactionSection.h"
#include "dwfemap/package/transaction/UpdateLayer.h"
#include "dwfemap/package/transaction/AddLayer.h"
#include "dwfemap/package/transaction/RemoveLayer.h"
#include "dwfemap/package/transaction/UpdateLayerGroup.h"
#include "dwfemap/package/transaction/AddLayerGroup.h"
#include "dwfemap/package/transaction/RemoveLayerGroup.h"
#include "dwfemap/package/transaction/UpdateUIGraphic.h"
#include "dwfemap/package/ScaleRange.h"
#include "dwfemap/package/UIGraphic.h"

#include "ObservationMesh.h"
#include "RSDWFOutputStream.h"

#define TOP_LABEL_W2D_ZORDER 2000000000.0


EMapUpdateRenderer::EMapUpdateRenderer(const RS_String& filename,
                                       unsigned int     sequenceId)
                                       : DWFRenderer()
{
    m_uuid = new DWFUUID;
    m_sequenceId = sequenceId;
    m_dwfFilename = filename;
}


EMapUpdateRenderer::~EMapUpdateRenderer()
{
    delete m_uuid;
}


void EMapUpdateRenderer::StartMap(RS_MapUIInfo*    mapInfo,
                                  RS_Bounds&       extents,
                                  double           mapScale,
                                  double           dpi,
                                  double           metersPerUnit,
                                  CSysTransformer* xformToLL)
{
    //init super
    DWFRenderer::StartMap(mapInfo, extents, mapScale, dpi, metersPerUnit, xformToLL);

    m_bFirst = true;

    DWFSource oSource( mapInfo->name().c_str(), MAP_PRODUCT_NAME, L"" );

    DWFBackground bg(mapInfo->bgcolor().argb());

    DWFBounds mapextent(extents.minx,
                        extents.maxx,
                        extents.miny,
                        extents.maxy);

    m_pPage = DWFCORE_ALLOC_OBJECT( DWFEMapTransactionSection(
                        mapInfo->name().c_str(),    //zTitle
                        m_uuid->uuid(false), //zObjectId
                        oSource,    //rSource
                        m_sequenceId,
                        mapInfo->guid().c_str()
                        ));
}


void EMapUpdateRenderer::EndMap()
{
    //find W2D to Map coords transform
    //We need to send this to the viewer in the descriptor

    RS_Bounds b = GetBounds();

    //W2D to page transform
    double anTransform[4][4] = {
        { 1.0 / GetMapToW2DScale(), 0, 0, 0 },
        { 0, 1.0 / GetMapToW2DScale(), 0, 0 },
        { 0, 0, 1, 0 },
        { b.minx-b.width()*0.5, b.miny-b.height()*0.5, 0, 1 }
    };

    //W2D clip
    //optional -- don't send for now
    //double anClip[4] = { b.minx, b.miny, b.maxx, b.maxy };
    double* anClip = NULL;

    //////////////////////////////////////////////////////////////////////
    // Feature Data resource W2D (one W2D per each map layer)
    //////////////////////////////////////////////////////////////////////

    objdefres_list::iterator oditer = m_lAttributeResources.begin();
    layerinfo_list::iterator iditer = m_lLayerInfos.begin();
    for (stream_list::iterator iter = m_lLayerStreams.begin();
        iter != m_lLayerStreams.end(); ++iter, ++iditer, ++oditer)
    {
        std::wstring layerid = (*iditer).guid();
        if (m_bFirst)
        {
            AddW2DResource( *iter,
                *oditer,
                DWFXML::kzRole_Graphics2d,
                layerid.c_str(),
                (const double*)anTransform,
                (const double*)anClip,
                (*iditer).zorder());
            m_bFirst = false;
        }
        else
        {
            AddW2DResource(*iter,
                *oditer,
                DWFXML::kzRole_Graphics2dOverlay,
                layerid.c_str(),
                (const double*)anTransform,
                (const double*)anClip,
                (*iditer).zorder());
        }
    }

    //////////////////////////////////////////////////////////////////////
    // Label resource W2D
    //////////////////////////////////////////////////////////////////////

    iditer = m_lLayerInfos.begin();
    for (stream_list::iterator iter = m_lLabelStreams.begin();
        iter != m_lLabelStreams.end(); ++iter, ++iditer)
    {
        if (*iter)
        {
            std::wstring layerid = (*iditer).guid();
            AddW2DResource( *iter,
                NULL, //no attribute data for labels
                DWFXML::kzRole_Graphics2dOverlay,
                layerid.c_str(),
                (const double*)anTransform,
                (const double*)anClip,
                TOP_LABEL_W2D_ZORDER - (*iditer).zorder());
        }
    }

    //IMPORTANT
    //Since we added the object definition resources to the EMapTransaction
    //section, it now owns them. We need to clear the object resources
    //collection so that DWFRenderer does not clean them out.
    //TODO: this will be cleaned up some day
    m_lAttributeResources.clear();

    m_lLayerInfos.clear();

    DWFBufferOutputStream* dwfout = NULL;

    ////////////////////////////////////////////////////////////////////
    //now save out the observation mesh, if there is one
    //Note: m_obsMesh is protected member of DWFRenderer (the base class)
    if (m_obsMesh)
    {
        dwfout = DWFCORE_ALLOC_OBJECT( DWFBufferOutputStream(16384));
        RSDWFOutputStream rsos(dwfout);

        //serialize to XML
        m_obsMesh->WriteXml(&rsos);

        //need to define an observation mesh role string constant
        DWFResource* meshRes = DWFCORE_ALLOC_OBJECT( DWFResource(L"", DWFEMAPXML::kzRole_ObservationMesh, DWFMIME::kzMIMEType_XML));

        //create an input stream out of the observation mesh stream
        DWFBufferInputStream* is = DWFCORE_ALLOC_OBJECT(DWFBufferInputStream(
            dwfout->buffer(), dwfout->bytes()));

        //the resource now owns the input stream
        meshRes->setInputStream(is);
        meshRes->setParentObjectID(m_pPage->objectID());

        m_pPage->addResource(meshRes, true);
    }

    ///////////////////////////////////////////////////////////////////
    // Write out the DWF

    DWFFile oDWF( m_dwfFilename.c_str() );

    DWFPackageVersionExtension* pVersionExtension = DWFCORE_ALLOC_OBJECT(
        DWFPackageVersionTypeInfoExtension(DWFEMapInterface::kzEMapTransaction_ID));

    DWFPackageWriter oWriter( oDWF, L"", pVersionExtension );

    DWFInterface* pEMapInterface =
        DWFCORE_ALLOC_OBJECT( DWFEMapInterface( DWFEMapInterface::kzEMapTransaction_Name,
                                                DWFEMapInterface::kzEMapTransaction_HRef,
                                                DWFEMapInterface::kzEMapTransaction_ID) );

    //the writer takes ownership of the pEMapInterface
    oWriter.addSection( m_pPage, pEMapInterface );

    //TODO: extract strings
    oWriter.write( MAP_PRODUCT_AUTHOR,
                   MAP_PRODUCT_NAME,
                   MAP_PRODUCT_VERSION,
                   MAP_PRODUCT_AUTHOR,
                   _DWF_FORMAT_EMAP_VERSION_CURRENT_STRING,
                   DWFZipFileDescriptor::eZipFastest);

    //free the observation mesh stream if any
    DWFCORE_FREE_OBJECT(dwfout);

    //call super to free the W2D Streams
    DWFRenderer::EndMap();
}


void EMapUpdateRenderer::StartLayer(RS_LayerUIInfo*      layerInfo,
                                    RS_FeatureClassInfo* classInfo
                                   )
{
    //call super to do W2D stuff
    DWFRenderer::StartLayer(layerInfo, classInfo);

    m_lLayerInfos.push_back(*layerInfo);
}


//TODO: take this out if it does nothing.
void EMapUpdateRenderer::EndLayer()
{
    //call super to clean up W2D stuff
    DWFRenderer::EndLayer();
}


void EMapUpdateRenderer::AddW2DResource(DWFCore::DWFBufferOutputStream* w2dStream,
                                        DWFToolkit::DWFObjectDefinitionResource* objdefs,
                                        const wchar_t*  role,
                                        const wchar_t*  parentId,
                                        const double*   transform,
                                        const double*   clip,
                                        double          zorder)
{
    //
    // define the resource - this must be allocated on the heap
    //
    DWFGraphicResource* p2Dgfx =
        DWFCORE_ALLOC_OBJECT( DWFGraphicResource(L"",               // title
                                                 role,               //DWFXML::kzRole_Graphics2d, // role
                                                 DWFMIME::kzMIMEType_W2D,  // MIME type
                                                 L"",                // author
                                                 L"",                // description
                                                 L"",                // creation time
                                                 L"") );             // modification time

    if (p2Dgfx == NULL)
    {
        _DWFCORE_THROW( DWFMemoryException, L"Failed to allocate resource" );
    }

    //TODO: parentID should be the scale range ID instead.
    p2Dgfx->setParentObjectID(parentId);

    p2Dgfx->configureGraphic( (const double*)transform,
                            NULL,
                            (const double*)clip,
                            true,
                            (int)zorder);

    //create an input stream out of the W2D output stream
    //Note the output stream will be freed by DWFRenderer::EndMap
    //which happens after the DWF package is created so everything is cool
    DWFBufferInputStream* is = DWFCORE_ALLOC_OBJECT(DWFBufferInputStream(
        w2dStream->buffer(), w2dStream->bytes()));

    if (is == NULL)
    {
        DWFCORE_FREE_OBJECT( p2Dgfx );

        _DWFCORE_THROW( DWFMemoryException, L"Failed to allocate file stream" );
    }

    p2Dgfx->setInputStream( is );

    p2Dgfx->setObjectID(m_uuid->next(false));

    //
    // finally, drop the resource into the page
    //
    m_pPage->addResource( p2Dgfx, true );

    ///////////////////////////////////////////////////////////////////
    // Add the object attribute data to the section
    ///////////////////////////////////////////////////////////////////
    if (objdefs)
    {
        objdefs->setParentObjectID(p2Dgfx->objectID());
        m_pPage->addResource(objdefs, true);
    }
}


void EMapUpdateRenderer::CmdAddLayer(const RS_String& guid)
{
    //get the previously accumulated layer information
    RS_LayerUIInfo layerInfo = m_hLayerInfoMap[guid];

    //see if we actually got a valid structure
    _ASSERT(!layerInfo.guid().empty());

    //correlate the UIGraphic object ID with the layer object ID
    DWFString uigGuid = GetUIGraphicObjectIdFromLayerObjectId(layerInfo.guid().c_str());
    DWFUIGraphic* pGraphic = DWFCORE_ALLOC_OBJECT(
        DWFUIGraphic( uigGuid,
                    layerInfo.graphic().label().c_str(),
                    layerInfo.show(),
                    ! layerInfo.expand()
                    ));

    //Construct a Layer to be added to the Map
    DWFEMapLayer* pLayer = DWFCORE_ALLOC_OBJECT( DWFEMapLayer(
                                        layerInfo.name().c_str(),
                                        layerInfo.guid().c_str(), //m_uuid->next(false)
                                        layerInfo.groupguid().c_str(),
                                        layerInfo.visible(),
                                        layerInfo.selectable(),
                                        layerInfo.editable(),
                                        pGraphic //UI graphic
                                        ));

    DWFEMapAddLayerCommand* cmd = DWFCORE_ALLOC_OBJECT(
                                    DWFEMapAddLayerCommand( pLayer,
                                                            L"")); //TODO: insert after guid

    //keep track added layers so that we can also get scale ranges for them
    //via CmdAddScaleRange
    m_hAddedLayers[guid] = pLayer;

    ((DWFEMapTransactionSection*)m_pPage)->addCommand(cmd);
}


void EMapUpdateRenderer::CmdRemoveLayer(const RS_String& guid)
{
    //verify the guid is valid
    _ASSERT(!guid.empty());

    DWFEMapRemoveLayerCommand* cmd = DWFCORE_ALLOC_OBJECT(
                                    DWFEMapRemoveLayerCommand( guid.c_str()));

    ((DWFEMapTransactionSection*)m_pPage)->addCommand(cmd);
}


void EMapUpdateRenderer::CmdUpdateLayer(const RS_String& guid)
{
    //get the previously accumulated layer information
    RS_LayerUIInfo layerInfo = m_hLayerInfoMap[guid];

    //see if we actually got a valid structure
    _ASSERT(!layerInfo.guid().empty());

    DWFEMapUpdateLayerCommand* cmd = DWFCORE_ALLOC_OBJECT(
                                    DWFEMapUpdateLayerCommand( layerInfo.guid().c_str(),
                                                               layerInfo.groupguid().c_str(),
                                                               layerInfo.visible(),
                                                               layerInfo.selectable()));

    ((DWFEMapTransactionSection*)m_pPage)->addCommand(cmd);
}


void EMapUpdateRenderer::CmdAddLayerGroup(const RS_String& guid)
{
    //get the previously accumulated layer group information
    RS_LayerUIInfo layerInfo = m_hGroups[guid];

    //see if we actually got a valid structure
    _ASSERT(!layerInfo.guid().empty());

    //correlate the UIGraphic object ID with the layer group object ID
    DWFString uigGuid = GetUIGraphicObjectIdFromLayerObjectId(layerInfo.guid().c_str());
    DWFUIGraphic* pGraphic = DWFCORE_ALLOC_OBJECT(
        DWFUIGraphic( uigGuid,
                    layerInfo.graphic().label().c_str(),
                    layerInfo.show(),
                    ! layerInfo.expand()
                    ));

    //Construct a Layer to be added to the Map
    DWFEMapLayerGroup* pLayer = DWFCORE_ALLOC_OBJECT( DWFEMapLayerGroup(
                                        layerInfo.name().c_str(),
                                        layerInfo.guid().c_str(),
                                        layerInfo.groupguid().c_str(),
                                        layerInfo.visible(),
                                        pGraphic //UI graphic
                                        ));

    DWFEMapAddLayerGroupCommand* cmd = DWFCORE_ALLOC_OBJECT(
                                    DWFEMapAddLayerGroupCommand( pLayer,
                                                                 L"" )); //TODO: insert after guid

    ((DWFEMapTransactionSection*)m_pPage)->addCommand(cmd);
}


void EMapUpdateRenderer::CmdRemoveLayerGroup(const RS_String& guid)
{
    //verify the guid is valid
    _ASSERT(!guid.empty());

    DWFEMapRemoveLayerGroupCommand* cmd = DWFCORE_ALLOC_OBJECT(
                                    DWFEMapRemoveLayerGroupCommand( guid.c_str()));

    ((DWFEMapTransactionSection*)m_pPage)->addCommand(cmd);
}


void EMapUpdateRenderer::CmdUpdateLayerGroup(const RS_String& guid)
{
    //get the previously accumulated layer group information
    RS_LayerUIInfo layerInfo = m_hGroups[guid];

    //see if we actually got a valid structure
    _ASSERT(!layerInfo.guid().empty());

    DWFEMapUpdateLayerGroupCommand* cmd = DWFCORE_ALLOC_OBJECT(
                                    DWFEMapUpdateLayerGroupCommand( layerInfo.guid().c_str(),
                                                               layerInfo.groupguid().c_str(),
                                                               layerInfo.visible()));

    ((DWFEMapTransactionSection*)m_pPage)->addCommand(cmd);
}


void EMapUpdateRenderer::CmdUpdateUIGraphicForLayer(const RS_String& guid)
{
    //get the previously accumulated layer information
    RS_LayerUIInfo layerInfo = m_hLayerInfoMap[guid];

    //see if we actually got a valid structure
    _ASSERT(!layerInfo.guid().empty());

    //get the UIGraphic object ID correlated with the layer object ID
    DWFString uigGuid = GetUIGraphicObjectIdFromLayerObjectId(layerInfo.guid().c_str());
    DWFEMapUpdateUIGraphicCommand* cmd = DWFCORE_ALLOC_OBJECT(
                                    DWFEMapUpdateUIGraphicCommand( uigGuid,
                                                               layerInfo.graphic().label().c_str(),
                                                               layerInfo.show()));

    ((DWFEMapTransactionSection*)m_pPage)->addCommand(cmd);
}


void EMapUpdateRenderer::CmdUpdateUIGraphicForLayerGroup(const RS_String& guid)
{
    //get the previously accumulated layer group information
    RS_LayerUIInfo layerInfo = m_hGroups[guid];

    //see if we actually got a valid structure
    _ASSERT(!layerInfo.guid().empty());

    //get the UIGraphic object ID correlated with the layer group object ID
    DWFString uigGuid = GetUIGraphicObjectIdFromLayerObjectId(layerInfo.guid().c_str());
    DWFEMapUpdateUIGraphicCommand* cmd = DWFCORE_ALLOC_OBJECT(
                                    DWFEMapUpdateUIGraphicCommand( uigGuid,
                                                               layerInfo.graphic().label().c_str(),
                                                               layerInfo.show()));

    ((DWFEMapTransactionSection*)m_pPage)->addCommand(cmd);

}


void EMapUpdateRenderer::AddLayerGroupInfo(RS_LayerUIInfo& layerInfo)
{
    //In GetMapUpdate, unlike GetMap, we index by GUID, not name
    //in order to deal with transaction commands easier
    m_hGroups[std::wstring(layerInfo.guid())] = layerInfo;
}


void EMapUpdateRenderer::AddLayerInfo(RS_LayerUIInfo& layerInfo)
{
    //In GetMapUpdate, unlike GetMap, we index by GUID, not name
    //in order to deal with transaction commands easier
    m_hLayerInfoMap[std::wstring(layerInfo.guid())] = layerInfo;
}


//note this has to be called after we know about the layer added command
//since it relies on the EMap layer already existing -- it is confusing
//and ambiguous unfortunately
//This is because we do the layer command processing outside of the normal
//StartLayer->EndLayer loop, which the EMapRenderer has the luxury of using,
//since EMapRenderer does not need to worry about generating layer graphics also
void EMapUpdateRenderer::AddScaleRange(RS_String& layerGuid,
                                       double min,
                                       double max,
                                       std::list<RS_UIGraphic>* uiGraphics)
{
    DWFEMapLayer* pLayer = m_hAddedLayers[layerGuid];

    // Construct two scaleRanges to add to the Layer
    DWFScaleRange* pScaleR = DWFCORE_ALLOC_OBJECT( DWFScaleRange(
                                                    m_uuid->next(false), // ObjectId
                                                    min,                 // Min
                                                    max));               // Max

    // Add the Scale Range to the Layer
    pLayer->addScaleRange(pScaleR);

    //now add the theming ui graphics for this scale range
    if (uiGraphics)
    {
        std::list<RS_UIGraphic>::iterator iter = uiGraphics->begin();
        for (; iter != uiGraphics->end(); ++iter)
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
                    ((DWFEMapTransactionSection*)m_pPage)->addResource(imgRes, true);
                }
            }
        }
    }
}


DWFImageResource* EMapUpdateRenderer::CreateImageResource(unsigned char* data, int len)
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
