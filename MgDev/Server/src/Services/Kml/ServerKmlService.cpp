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

#include "RendererStyles.h"
#include "MapGuideCommon.h"
#include "ServerKmlService.h"
#include "Services/KmlDefs.h"
#include "MappingUtil.h"
#include "SEMgSymbolManager.h"
#include "KmlRenderer.h"
#include "RSMgFeatureReader.h"
#include "RSMgInputStream.h"
#include "DefaultStylizer.h"
#include "Bounds.h"
#include "MgCSTrans.h"
#include "SAX2Parser.h"

//Zip support from DWF toolkit
#include "dwfcore/ZipFileDescriptor.h"

const STRING LL84_WKT = L"GEOGCS[\"LL84\",DATUM[\"WGS84\",SPHEROID[\"WGS84\",6378137,298.25722293287],TOWGS84[0,0,0,0,0,0,0]],PRIMEM[\"Greenwich\",0],UNIT[\"Degrees\",0.01745329252]]";
const STRING GOOGLE_EARTH_WKT = LL84_WKT;

IMPLEMENT_CREATE_SERVICE(MgServerKmlService)

MgServerKmlService::MgServerKmlService() : MgKmlService()
{
    m_csFactory = new MgCoordinateSystemFactory();
}


MgServerKmlService::~MgServerKmlService()
{
}


MgByteReader* MgServerKmlService::GetMapKml(MgMap* map, double dpi, CREFSTRING agentUri, CREFSTRING format)
{
    Ptr<MgByteReader> byteReader;

    MG_TRY()

    if (NULL == map)
    {
        throw new MgNullArgumentException(L"MgServerKmlService.GetMapKml", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    STRING sessionId = GetSessionId();

    KmlContent kmlContent;
    kmlContent.StartDocument();

    //write the map name
    STRING mapName = map->GetName();
    kmlContent.WriteString("<name><![CDATA[", false);
    kmlContent.WriteString(mapName, false);
    kmlContent.WriteString("]]></name>");
    kmlContent.WriteString("<open>1</open>");

    if(m_svcResource == NULL)
    {
        InitializeResourceService();
    }

    //write the map description, if any
    Ptr<MgResourceIdentifier> mapResId = map->GetMapDefinition();

    //get the map definition
    auto_ptr<MdfModel::MapDefinition> mdf(MgMappingUtil::GetMapDefinition(m_svcResource, mapResId));
    STRING metadata = mdf->GetMetadata();

    if(!metadata.empty())
    {
        size_t offset = 0;
        STRING description = ReadElement(metadata, L"Description", offset);
        if(!description.empty())
        {
            kmlContent.WriteString("<description><![CDATA[", false);
            kmlContent.WriteString(description, false);
            kmlContent.WriteString("]]></description>");
        }
    }
    kmlContent.WriteString("<visibility>1</visibility>");
    Ptr<MgLayerCollection> layers = map->GetLayers();
    Ptr<MgEnvelope> extent = map->GetMapExtent();
    if(extent != NULL)
    {
        STRING mapWkt = map->GetMapSRS();
        if(!mapWkt.empty())
        {
            Ptr<MgCoordinateSystem> mapCs = (mapWkt.empty()) ? NULL : m_csFactory->Create(mapWkt);
            Ptr<MgCoordinateSystem> llCs = m_csFactory->Create(GOOGLE_EARTH_WKT);
            Ptr<MgCoordinateSystemTransform> trans = m_csFactory->GetTransform(mapCs, llCs);
            trans->IgnoreDatumShiftWarning(true);
            trans->IgnoreOutsideDomainWarning(true);
            extent = trans->Transform(extent);
        }

        WriteRegion(extent, kmlContent, dpi);
    }
    int numLayers = layers->GetCount();
    for(int i = 0; i < numLayers; i++)
    {
        Ptr<MgLayer> layer = dynamic_cast<MgLayer*>(layers->GetItem(i));
        AppendLayer(layer, extent, numLayers - i, agentUri, format, sessionId, kmlContent);
    }
    kmlContent.EndDocument();

    Ptr<MgByteSource> byteSource = GetByteSource(kmlContent, format);
    if(byteSource != NULL)
    {
        byteReader = byteSource->GetReader();
    }

    MG_CATCH_AND_THROW(L"MgServerKmlService.GetMapKml")

    return SAFE_ADDREF(byteReader.p);
}

MgByteReader* MgServerKmlService::GetLayerKml(MgLayer* layer, MgEnvelope* extents,
    INT32 width, INT32 height, double dpi, INT32 drawOrder, CREFSTRING agentUri, CREFSTRING format)
{
    Ptr<MgByteReader> byteReader;

    MG_TRY()

    if (NULL == layer)
    {
        throw new MgNullArgumentException(L"MgServerKmlService.GetLayerKml", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    STRING sessionId = GetSessionId();

    if(m_svcResource == NULL)
    {
        InitializeResourceService();
    }

    double scale = GetScale(extents, width, height, dpi);

    //get layer definition
    Ptr<MgResourceIdentifier> resId = layer->GetLayerDefinition();
    auto_ptr<MdfModel::LayerDefinition> ldf(MgLayerBase::GetLayerDefinition(m_svcResource, resId));

    KmlContent kmlContent;
    kmlContent.StartDocument();
    kmlContent.WriteString("<visibility>1</visibility>");
    Ptr<MgCoordinateSystem> destCs = m_csFactory->Create(GOOGLE_EARTH_WKT);
    Ptr<MgEnvelope> destExtent = GetLayerExtent(ldf.get(), destCs);
    if(destExtent != NULL)
    {
        double widthMeters = destCs->ConvertCoordinateSystemUnitsToMeters(destExtent->GetWidth());
        double heightMeters = destCs->ConvertCoordinateSystemUnitsToMeters(destExtent->GetHeight());
        double dimension = sqrt(widthMeters * heightMeters);

        MdfModel::VectorLayerDefinition* vl = dynamic_cast<MdfModel::VectorLayerDefinition*>(ldf.get());
        MdfModel::DrawingLayerDefinition* dl = dynamic_cast<MdfModel::DrawingLayerDefinition*>(ldf.get());
        MdfModel::GridLayerDefinition* gl = dynamic_cast<MdfModel::GridLayerDefinition*>(ldf.get());
        if(vl != NULL)
        {
            //get the scale ranges
            MdfModel::VectorScaleRangeCollection* scaleRanges = vl->GetScaleRanges();
            MdfModel::VectorScaleRange* range = NULL;

            for (int i = 0; i < scaleRanges->GetCount(); i++)
            {
                range = scaleRanges->GetAt(i);
                double minScale = range->GetMinScale();
                double maxScale = range->GetMaxScale();
                if(scale > minScale && scale <= maxScale)
                {
                    AppendScaleRange(layer, destExtent, agentUri, dimension,
                        minScale, maxScale, dpi, drawOrder, format, sessionId, kmlContent);
                }
            }
        }
        else if(gl != NULL)
        {
            //get the scale ranges
            MdfModel::GridScaleRangeCollection* scaleRanges = gl->GetScaleRanges();
            MdfModel::GridScaleRange* range = NULL;

            for (int i = 0; i < scaleRanges->GetCount(); i++)
            {
                range = scaleRanges->GetAt(i);
                double minScale = range->GetMinScale();
                double maxScale = range->GetMaxScale();
                if(scale > minScale && scale <= maxScale)
                {
                    AppendRasterScaleRange(layer, destExtent, agentUri, dimension,
                        minScale, maxScale, dpi, drawOrder, format, sessionId, kmlContent);
                }
            }
        }
        /*else if(dl != NULL)
        {
            double minScale = dl->GetMinScale();
            double maxScale = dl->GetMaxScale();
            if(scale > minScale && scale <= maxScale)
            {
                AppendScaleRange(layer, destExtent, agentUri, dimension,
                    minScale, maxScale, dpi, drawOrder, format, sessionId, kmlContent);
            }
        }*/
    }
    kmlContent.EndDocument();

    Ptr<MgByteSource> byteSource = GetByteSource(kmlContent, format);
    if(byteSource != NULL)
    {
        byteReader = byteSource->GetReader();
    }

    MG_CATCH_AND_THROW(L"MgServerKmlService.GetLayerKml")

    return SAFE_ADDREF(byteReader.p);
}

MgByteReader* MgServerKmlService::GetFeaturesKml(MgLayer* layer, MgEnvelope* extents,
    INT32 width, INT32 height, double dpi, INT32 drawOrder, CREFSTRING format)
{
    Ptr<MgByteReader> byteReader;

    MG_TRY()

    if (NULL == layer)
    {
        throw new MgNullArgumentException(L"MgServerKmlService.GetFeaturesKml", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    double scale = GetScale(extents, width, height, dpi);

    if(m_svcResource == NULL)
    {
        InitializeResourceService();
    }
    Ptr<MgResourceIdentifier> resId = layer->GetLayerDefinition();
    auto_ptr<MdfModel::LayerDefinition> ldf(MgLayerBase::GetLayerDefinition(m_svcResource, resId));
    MdfModel::GridLayerDefinition* gl = dynamic_cast<MdfModel::GridLayerDefinition*>(ldf.get());
    if(gl != NULL)
    {
        // Create a new, empty map
        Ptr<MgSiteConnection> siteConn = new MgSiteConnection();
        Ptr<MgUserInformation> userInfo = MgUserInformation::GetCurrentUserInfo();
        siteConn->Open(userInfo);
        Ptr<MgMap> map = new MgMap(siteConn);
        map->Create(GOOGLE_EARTH_WKT, extents, L"Google Earth Map");
        map->SetDisplayWidth(width);
        map->SetDisplayHeight(height);
        map->SetDisplayDpi((int)dpi);
        map->SetViewScale(scale);
        Ptr<MgLayerCollection> layers = map->GetLayers();
        layers->Add(layer);
        layer->ForceRefresh();

        if(m_svcRendering == NULL)
        {
            InitializeRenderingService();
        }
        Ptr<MgSelection> selection;
        Ptr<MgColor> bkColor = new MgColor();
        byteReader = m_svcRendering->RenderMap(map, selection, extents, width, height, bkColor, L"PNG");
    }
    else
    {
        KmlContent kmlContent;
        kmlContent.StartDocument();
        kmlContent.WriteString("<visibility>1</visibility>");
        AppendFeatures(layer, ldf.get(), extents, scale, dpi, drawOrder, kmlContent);
        kmlContent.EndDocument();
        Ptr<MgByteSource> byteSource = GetByteSource(kmlContent, format);
        if(byteSource != NULL)
        {
            byteReader = byteSource->GetReader();
        }
    }

    MG_CATCH_AND_THROW(L"MgServerKmlService.GetFeaturesKml")

    return SAFE_ADDREF(byteReader.p);
}

void MgServerKmlService::AppendLayer(MgLayer* layer,
                                     MgEnvelope* extent,
                                     INT32 drawOrder,
                                     CREFSTRING agentUri,
                                     CREFSTRING format,
                                     CREFSTRING sessionId,
                                     KmlContent& kmlContent)
{
    char buffer[256];
    kmlContent.WriteString("<NetworkLink>");
    kmlContent.WriteString("<visibility>");
    kmlContent.WriteString(layer->GetVisible() ? "1" : "0");
    kmlContent.WriteString("</visibility>");
    kmlContent.WriteString("<name><![CDATA[", false);
    kmlContent.WriteString(MgUtil::WideCharToMultiByte(layer->GetLegendLabel()), false);
    kmlContent.WriteString("]]></name>");
    kmlContent.WriteString("<Link>");
    kmlContent.WriteString("<href>");
    kmlContent.WriteString(agentUri, false);
    kmlContent.WriteString("?OPERATION=GetLayerKml&amp;VERSION=1.0.0&amp;LAYERDEFINITION=", false);
    Ptr<MgResourceIdentifier> resource = layer->GetLayerDefinition();
    kmlContent.WriteString(MgUtil::WideCharToMultiByte(resource->ToString()), false);
    sprintf(buffer,"&amp;DRAWORDER=%d", drawOrder);
    kmlContent.WriteString(buffer, false);
    kmlContent.WriteString("&amp;FORMAT=", false);
    kmlContent.WriteString(MgUtil::WideCharToMultiByte(format), false);
    kmlContent.WriteString("&amp;SESSION=", false);
    kmlContent.WriteString(MgUtil::WideCharToMultiByte(sessionId));
    kmlContent.WriteString("</href>");
    kmlContent.WriteString("<viewRefreshMode>onStop</viewRefreshMode>");
    kmlContent.WriteString("<viewRefreshTime>1</viewRefreshTime>");
    kmlContent.WriteString("<viewFormat>BBOX=[bboxWest],[bboxSouth],[bboxEast],[bboxNorth]&amp;WIDTH=[horizPixels]&amp;HEIGHT=[vertPixels]</viewFormat>");
    kmlContent.WriteString("</Link>");
    kmlContent.WriteString("</NetworkLink>");
}

void MgServerKmlService::AppendScaleRange(MgLayer* layer,
                                          MgEnvelope* extent,
                                          CREFSTRING agentUri,
                                          double dimension,
                                          double minScale,
                                          double maxScale,
                                          double dpi,
                                          INT32 drawOrder,
                                          CREFSTRING format,
                                          CREFSTRING sessionId,
                                          KmlContent& kmlContent)
{
    char buffer[256];
    kmlContent.WriteString("<NetworkLink>");
    kmlContent.WriteString("<name><![CDATA[", false);
    sprintf(buffer,"%f - %f", minScale, maxScale);
    kmlContent.WriteString(buffer, false);
    kmlContent.WriteString("]]></name>");
    WriteRegion(extent, kmlContent, dpi, dimension, minScale, maxScale);
    kmlContent.WriteString("<open>1</open>");
    kmlContent.WriteString("<Link>");
    kmlContent.WriteString("<href>");
    kmlContent.WriteString(agentUri, false);
    kmlContent.WriteString("?OPERATION=GetFeaturesKml&amp;VERSION=1.0.0&amp;LAYERDEFINITION=", false);
    Ptr<MgResourceIdentifier> resource = layer->GetLayerDefinition();
    kmlContent.WriteString(MgUtil::WideCharToMultiByte(resource->ToString()), false);
    sprintf(buffer,"&amp;DPI=%f", dpi);
    kmlContent.WriteString(buffer, false);
    sprintf(buffer,"&amp;DRAWORDER=%d", drawOrder);
    kmlContent.WriteString(buffer, false);
    kmlContent.WriteString("&amp;FORMAT=", false);
    kmlContent.WriteString(MgUtil::WideCharToMultiByte(format), false);
    kmlContent.WriteString("&amp;SESSION=", false);
    kmlContent.WriteString(MgUtil::WideCharToMultiByte(sessionId));
    kmlContent.WriteString("</href>");
    kmlContent.WriteString("<viewRefreshMode>onStop</viewRefreshMode>");
    kmlContent.WriteString("<viewRefreshTime>1</viewRefreshTime>");
    kmlContent.WriteString("<viewFormat>BBOX=[bboxWest],[bboxSouth],[bboxEast],[bboxNorth]&amp;WIDTH=[horizPixels]&amp;HEIGHT=[vertPixels]</viewFormat>");
    kmlContent.WriteString("</Link>");
    kmlContent.WriteString("</NetworkLink>");
}

void MgServerKmlService::AppendRasterScaleRange(MgLayer* layer,
                                          MgEnvelope* extent,
                                          CREFSTRING agentUri,
                                          double dimension,
                                          double minScale,
                                          double maxScale,
                                          double dpi,
                                          INT32 drawOrder,
                                          CREFSTRING format,
                                          CREFSTRING sessionId,
                                          KmlContent& kmlContent)
{
    char buffer[1024];
    kmlContent.WriteString("<GroundOverlay>");
    sprintf(buffer,"<drawOrder>%d</drawOrder>", drawOrder);
    kmlContent.WriteString(buffer);
    kmlContent.WriteString("<name><![CDATA[", false);
    sprintf(buffer,"%f - %f", minScale, maxScale);
    kmlContent.WriteString(buffer, false);
    kmlContent.WriteString("]]></name>");
    //<!-- inherited from Overlay element -->
    //<color>ffffffff</color>                   <!-- kml:color -->
    //<drawOrder>0</drawOrder>                  <!-- int -->
    Ptr<MgCoordinate> ll = extent->GetLowerLeftCoordinate();
    Ptr<MgCoordinate> ur = extent->GetUpperRightCoordinate();
    sprintf(buffer, "<LatLonBox><north>%f</north><south>%f</south><east>%f</east><west>%f</west><rotation>0</rotation></LatLonBox>",
        ur->GetY(), ll->GetY(), ur->GetX(), ll->GetX());
    kmlContent.WriteString(buffer);
    WriteRegion(extent, kmlContent, dpi, dimension, minScale, maxScale);
    kmlContent.WriteString("<Icon>");
    kmlContent.WriteString("<href>");
    kmlContent.WriteString(agentUri, false);
    kmlContent.WriteString("?OPERATION=GetFeaturesKml&amp;VERSION=1.0.0&amp;LAYERDEFINITION=", false);
    Ptr<MgResourceIdentifier> resource = layer->GetLayerDefinition();
    kmlContent.WriteString(MgUtil::WideCharToMultiByte(resource->ToString()), false);
    sprintf(buffer,"&amp;DPI=%f", dpi);
    kmlContent.WriteString(buffer, false);
    kmlContent.WriteString("&amp;FORMAT=", false);
    kmlContent.WriteString(MgUtil::WideCharToMultiByte(format), false);
    kmlContent.WriteString("&amp;SESSION=", false);
    kmlContent.WriteString(MgUtil::WideCharToMultiByte(sessionId));
    kmlContent.WriteString("</href>");
    kmlContent.WriteString("<viewRefreshMode>onStop</viewRefreshMode>");
    kmlContent.WriteString("<viewRefreshTime>0</viewRefreshTime>");
    kmlContent.WriteString("<viewFormat>BBOX=[bboxWest],[bboxSouth],[bboxEast],[bboxNorth]&amp;WIDTH=[horizPixels]&amp;HEIGHT=[vertPixels]</viewFormat>");
    kmlContent.WriteString("</Icon>");
    kmlContent.WriteString("</GroundOverlay>");
}

void MgServerKmlService::AppendFeatures(MgLayer* layer,
                                        MdfModel::LayerDefinition* layerDef,
                                        MgEnvelope* extents,
                                        double scale,
                                        double dpi,
                                        INT32 drawOrder,
                                        KmlContent& kmlContent)
{
    MgCSTrans* csTrans = NULL;
    RSMgFeatureReader* rsReader = NULL;

    MG_TRY()

    RS_UIGraphic uig(NULL, 0, layer->GetLegendLabel());
    RS_LayerUIInfo layerInfo(layer->GetName(),
                             layer->GetObjectId(),
                             layer->GetSelectable(),
                             layer->GetVisible(),
                             false,
                             L"",
                             L"",
                             layer->GetDisplayInLegend(),
                             layer->GetExpandInLegend(),
                            -layer->GetDisplayOrder(),
                             uig);
    Ptr<MgCoordinateSystem> destCs = m_csFactory->Create(GOOGLE_EARTH_WKT);
    double metersPerUnit = (destCs.p != NULL)? destCs->ConvertCoordinateSystemUnitsToMeters(1.0) : 1.0;

    Ptr<MgCoordinate> ll = extents->GetLowerLeftCoordinate();
    Ptr<MgCoordinate> ur = extents->GetUpperRightCoordinate();
    RS_Bounds bounds(ll->GetX(), ll->GetY(), ur->GetX(), ur->GetY());

//  MdfModel::DrawingLayerDefinition* dl = dynamic_cast<MdfModel::DrawingLayerDefinition*>(ldf.get());
    MdfModel::VectorLayerDefinition* vl = dynamic_cast<MdfModel::VectorLayerDefinition*>(layerDef);
    if (vl != NULL)
    {
        if (m_svcFeature == NULL)
            InitializeFeatureService();

        Ptr<MgResourceIdentifier> resId = new MgResourceIdentifier(vl->GetResourceID());
        Ptr<MgCoordinateSystem> layerCs = GetCoordinateSystem(resId.p);
        if (layerCs != NULL)
            csTrans = new MgCSTrans(layerCs, destCs);

        rsReader = MgMappingUtil::ExecuteFeatureQuery(m_svcFeature, bounds, vl, NULL, destCs, layerCs, NULL);
        FdoPtr<FdoIFeatureReader> fdoReader = (NULL == rsReader) ? NULL : rsReader->GetInternalReader();

        if (NULL != fdoReader.p)
        {
            RS_FeatureClassInfo fcInfo(vl->GetFeatureName());
            MdfModel::NameStringPairCollection* pmappings = vl->GetPropertyMappings();
            for (int j=0; j<pmappings->GetCount(); j++)
            {
                MdfModel::NameStringPair* m = pmappings->GetAt(j);
                fcInfo.add_mapping(m->GetName(), m->GetValue());
            }

            KmlRenderer renderer(&kmlContent, bounds, scale, dpi, metersPerUnit, drawOrder);
            SEMgSymbolManager sman(m_svcResource);
            DefaultStylizer stylizer(&sman);
            renderer.StartLayer(&layerInfo, &fcInfo);
            stylizer.StylizeVectorLayer(vl, &renderer, rsReader, csTrans, scale, NULL, NULL);
            renderer.EndLayer();
        }
    }

    /*else if(dl != NULL)
    {
        if (m_svcDrawing == NULL)
            InitializeDrawingService();

        // make sure we have a valid scale range
        if (scale >= dl->GetMinScale() && scale < dl->GetMaxScale())
        {
            Ptr<MgResourceIdentifier> resId = new MgResourceIdentifier(dl->GetResourceID());

            //get the resource content to see if there is a coordinate system
            Ptr<MgByteReader> rdr = m_svcResource->GetResourceContent(resId);
            STRING st = rdr->ToString();

            //now look for a coordinate space tag and extract the contents
            size_t offset = 0;
            STRING cs = ReadElement(st, L"CoordinateSpace", offset);
            if (!cs.empty() && cs != destCs->ToString())
            {
                //construct cs transformer if needed
                Ptr<MgCoordinateSystem> srcCs = m_csFactory->Create(cs);
                csTrans = new MgCSTrans(srcCs, destCs);
            }

            //get DWF from drawing service
            Ptr<MgByteReader> reader = m_svcDrawing->GetSection(resId, dl->GetSheet());

            RSMgInputStream is(reader);

            renderer.StartLayer(&layerInfo, NULL);
            stylizer.StylizeDrawingLayer(dl, &renderer, &is, csTrans, scale);
            renderer.EndLayer();
        }
    }*/

    MG_CATCH(L"MgServerKmlService.AppendFeatures")

    delete rsReader;
    delete csTrans;

    MG_THROW()
}

double MgServerKmlService::GetScale(MgEnvelope* llExtents, int width, int height, double dpi)
{
    Ptr<MgCoordinateSystem> destCs = m_csFactory->Create(GOOGLE_EARTH_WKT);
    double mapWidth = destCs->ConvertCoordinateSystemUnitsToMeters(llExtents->GetWidth());
    double mapHeight = destCs->ConvertCoordinateSystemUnitsToMeters(llExtents->GetHeight());
    double screenWidth = width / dpi * METERS_PER_INCH;
    double screenHeight = height / dpi * METERS_PER_INCH;
    double xScale = mapWidth / screenWidth;
    double yScale = mapHeight / screenHeight;
    return rs_min(xScale, yScale);
}

MgEnvelope* MgServerKmlService::GetLayerExtent(MdfModel::LayerDefinition* layerDef, MgCoordinateSystem* destCs)
{
    Ptr<MgEnvelope> envelope;

    if(layerDef != NULL)
    {
        MdfModel::VectorLayerDefinition* vl = dynamic_cast<MdfModel::VectorLayerDefinition*>(layerDef);
        MdfModel::DrawingLayerDefinition* dl = dynamic_cast<MdfModel::DrawingLayerDefinition*>(layerDef);
        MdfModel::GridLayerDefinition* gl = dynamic_cast<MdfModel::GridLayerDefinition*>(layerDef);
        Ptr<MgCoordinateSystemTransform> csTrans;
        if(vl != NULL || gl != NULL)
        {
            Ptr<MgResourceIdentifier> featResId = new  MgResourceIdentifier(vl != NULL ? vl->GetResourceID() : gl->GetResourceID());

            if(m_svcFeature == NULL)
            {
                InitializeFeatureService();
            }
            Ptr<MgSpatialContextReader> scReader = m_svcFeature->GetSpatialContexts(featResId, false);
            if(scReader.p != NULL)
            {
                if(scReader->ReadNext())
                {
                    //get the layer coordinate system
                    STRING layerCoordSysWkt = scReader->GetCoordinateSystemWkt();
                    Ptr<MgCoordinateSystem> layerCs = (layerCoordSysWkt.empty()) ? NULL : m_csFactory->Create(layerCoordSysWkt);
                    if(layerCs != NULL)
                    {
                        csTrans = m_csFactory->GetTransform(layerCs, destCs);
                        csTrans->IgnoreDatumShiftWarning(true);
                        csTrans->IgnoreOutsideDomainWarning(true);

                        Ptr<MgByteReader> extentReader = scReader->GetExtent();
                        if(extentReader.p != NULL)
                        {
                            MgAgfReaderWriter agfReader;
                            Ptr<MgGeometry> geom = agfReader.Read(extentReader);
                            if (geom != NULL)
                            {
                                envelope = geom->Envelope();
                            }
                        }
                    }
                }
                scReader->Close();
            }
        }
        else if(dl != NULL)
        {
            if(m_svcResource == NULL)
            {
                InitializeResourceService();
            }

            Ptr<MgResourceIdentifier> resId = new MgResourceIdentifier(dl->GetResourceID());

            //get the resource content to see if there is a coordinate system
            Ptr<MgByteReader> rdr = m_svcResource->GetResourceContent(resId);
            STRING st = rdr->ToString();

            //now look for a coordinate space tag and extract the contents
            size_t offset = 0;
            STRING cs = ReadElement(st, L"CoordinateSpace", offset);
            if (!cs.empty() && cs != destCs->ToString())
            {
                //construct cs transformer if needed
                Ptr<MgCoordinateSystem> srcCs = m_csFactory->Create(cs);
                csTrans = m_csFactory->GetTransform(srcCs, destCs);
                csTrans->IgnoreDatumShiftWarning(true);
                csTrans->IgnoreOutsideDomainWarning(true);
            }

            offset = 0;
            do
            {
                STRING sheet = ReadElement(st, L"Sheet", offset);
                if(!sheet.empty())
                {
                    size_t sheetOffset = 0;
                    STRING sheetName = ReadElement(sheet, L"Name", sheetOffset);
                    if(sheetName.compare(dl->GetSheet()) == 0)
                    {
                        sheetOffset = 0;
                        STRING extent = ReadElement(sheet, L"Extent", sheetOffset);
                        if(!extent.empty())
                        {
                            size_t extentOffset = 0;
                            STRING minX = ReadElement(extent, L"MinX", extentOffset);
                            if(!minX.empty())
                            {
                                extentOffset = 0;
                                STRING minY = ReadElement(extent, L"MinY", extentOffset);
                                if(!minY.empty())
                                {
                                    extentOffset = 0;
                                    STRING maxX = ReadElement(extent, L"MaxX", extentOffset);
                                    if(!maxX.empty())
                                    {
                                        extentOffset = 0;
                                        STRING maxY = ReadElement(extent, L"MaxY", extentOffset);
                                        if(!maxY.empty())
                                        {
                                            double xMin = MgUtil::StringToDouble(minX);
                                            double yMin = MgUtil::StringToDouble(minY);
                                            double xMax = MgUtil::StringToDouble(maxX);
                                            double yMax = MgUtil::StringToDouble(maxY);
                                            envelope = new MgEnvelope(xMin, yMin, xMax, yMax);
                                        }
                                    }
                                }
                            }
                        }
                        break;
                    }
                }
            }
            while(offset != STRING::npos);
        }

        if(envelope != NULL && csTrans != NULL)
        {
            envelope = csTrans->Transform(envelope);
        }
    }
    return SAFE_ADDREF((MgEnvelope*)envelope);
}

STRING MgServerKmlService::ReadElement(STRING input, STRING elementName, size_t& offset)
{
    STRING content;
    STRING startTag = L"<" + elementName + L">";
    STRING endTag = L"</" + elementName + L">";
    size_t startPos = input.find(startTag, offset);
    if (startPos != STRING::npos)
    {
        size_t endPos = input.find(endTag, startPos);
        if(endPos != STRING::npos)
        {
            offset = endPos + endTag.length();
            startPos += startTag.length();
            content = input.substr(startPos, endPos - startPos);
        }
        else
        {
            offset = STRING::npos;
        }
    }
    else
    {
        offset = STRING::npos;
    }
    return content;
}

void MgServerKmlService::WriteRegion(MgEnvelope* extent, KmlContent& kmlContent, double dpi, double dimension, double minScale, double maxScale)
{
    if(extent != NULL)
    {
        char buffer[256];

        // Region Data
        Ptr<MgCoordinate> ll = extent->GetLowerLeftCoordinate();
        Ptr<MgCoordinate> ur = extent->GetUpperRightCoordinate();
        double north = ur->GetY();
        double south = ll->GetY();
        double east  = ur->GetX();
        double west  = ll->GetX();
        kmlContent.WriteString("<Region>");
        kmlContent.WriteString("<LatLonAltBox>");
        sprintf(buffer, "<north>%f</north><south>%f</south><east>%f</east><west>%f</west>",
            north, south, east, west);
        kmlContent.WriteString(buffer);
        kmlContent.WriteString("</LatLonAltBox>");
        if(dimension > 0)
        {
            double pixelSize = METERS_PER_INCH / dpi;
            int minPix = (int)(dimension / maxScale / pixelSize);
            int maxPix = minScale > 0 ? (int)(dimension / minScale / pixelSize) : -1;
            kmlContent.WriteString("<Lod>");
            sprintf(buffer, "<minLodPixels>%d</minLodPixels><maxLodPixels>%d</maxLodPixels>",
                minPix, maxPix);
            kmlContent.WriteString(buffer);
            kmlContent.WriteString("</Lod>");
        }
        kmlContent.WriteString("</Region>");
    }
}

//gets an instance of the resource service using the service manager
void MgServerKmlService::InitializeResourceService()
{
    MgServiceManager* serviceMan = MgServiceManager::GetInstance();
    assert(NULL != serviceMan);

    m_svcResource = dynamic_cast<MgResourceService*>(
        serviceMan->RequestService(MgServiceType::ResourceService));
    assert(m_svcResource != NULL);
}

//gets an instance of the feature service using the service manager
void MgServerKmlService::InitializeFeatureService()
{
    MgServiceManager* serviceMan = MgServiceManager::GetInstance();
    assert(NULL != serviceMan);

    m_svcFeature = dynamic_cast<MgFeatureService*>(
        serviceMan->RequestService(MgServiceType::FeatureService));
    assert(m_svcFeature != NULL);
}

//gets an instance of the drawing service using the service manager
void MgServerKmlService::InitializeDrawingService()
{
    MgServiceManager* serviceMan = MgServiceManager::GetInstance();
    assert(NULL != serviceMan);

    m_svcDrawing = dynamic_cast<MgDrawingService*>(
        serviceMan->RequestService(MgServiceType::DrawingService));
    assert(m_svcDrawing != NULL);
}

//gets an instance of the drawing service using the service manager
void MgServerKmlService::InitializeRenderingService()
{
    MgServiceManager* serviceMan = MgServiceManager::GetInstance();
    assert(NULL != serviceMan);

    m_svcRendering = dynamic_cast<MgRenderingService*>(
        serviceMan->RequestService(MgServiceType::RenderingService));
    assert(m_svcRendering != NULL);
}

MgCoordinateSystem* MgServerKmlService::GetCoordinateSystem(MgResourceIdentifier* featureSourceResId)
{
    Ptr<MgCoordinateSystem> layerCs;
    if(m_svcFeature == NULL)
    {
        InitializeFeatureService();
    }
    Ptr<MgSpatialContextReader> scReader = m_svcFeature->GetSpatialContexts(featureSourceResId, false);
    if(scReader.p != NULL)
    {
        if(scReader->ReadNext())
        {
            //get the layer coordinate system
            STRING layerCoordSysWkt = scReader->GetCoordinateSystemWkt();
            layerCs = (layerCoordSysWkt.empty()) ? NULL : m_csFactory->Create(layerCoordSysWkt);
        }
        scReader->Close();
    }
    return SAFE_ADDREF(layerCs.p);
}

void MgServerKmlService::SetConnectionProperties(MgConnectionProperties*)
{
    // Do nothing.  No connection properties are required for Server-side service objects.
}

MgByteSource* MgServerKmlService::GetByteSource(KmlContent& kmlContent, CREFSTRING format)
{
    MgByteSource* byteSource = NULL;
    string kmlString = kmlContent.GetString();
    const char* kmlBuf = kmlString.c_str();
    size_t kmlLen = kmlString.length();
    if(format.compare(L"KMZ") == 0)
    {
        STRING zipTmpFile = MgFileUtil::GenerateTempFileName();
        //ACE_OS::creat(MG_WCHAR_TO_TCHAR(zipTmpFile), 0);
        DWFString dwfFileName(zipTmpFile.c_str());
        DWFFile dwfFile(dwfFileName);
        DWFZipFileDescriptor zipFileDesc(dwfFile, DWFCore::DWFZipFileDescriptor::eZipFastest);
        zipFileDesc.open();
        DWFString innerFilename(L"doc.kml");
        DWFOutputStream* dwfOutputStream = zipFileDesc.zip(innerFilename);
        if(dwfOutputStream != NULL)
        {
            dwfOutputStream->write((const void*)kmlBuf, kmlLen);
            dwfOutputStream->flush();
            DWFCORE_FREE_OBJECT(dwfOutputStream);

            zipFileDesc.close();

            byteSource = new MgByteSource(zipTmpFile, true);
            byteSource->SetMimeType(MgMimeType::Kmz);
        }
    }
    else
    {
        byteSource = new MgByteSource( (unsigned char*)kmlBuf, (INT32)kmlLen);
        if(format.compare(L"XML") == 0)
        {
            byteSource->SetMimeType(MgMimeType::Xml);
        }
        else // default to KML
        {
            byteSource->SetMimeType(MgMimeType::Kml);
        }
    }

    return byteSource;
}

STRING MgServerKmlService::GetSessionId()
{
    STRING sessionId;
    Ptr<MgUserInformation> userInfo = MgUserInformation::GetCurrentUserInfo();
    if (userInfo != NULL)
    {
        sessionId = userInfo->GetMgSessionId();
        if(sessionId.empty())
        {
            Ptr<MgSiteConnection> conn = new MgSiteConnection();
            if(conn != NULL)
            {
                conn->Open(userInfo);
                Ptr<MgSite> site = conn->GetSite();
                if(site != NULL)
                {
                    sessionId = site->CreateSession();
                    if(!sessionId.empty())
                    {
                        userInfo->SetMgSessionId(sessionId);
                    }
                }
            }
        }
    }
    return sessionId;
}
