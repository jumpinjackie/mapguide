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

#include "MapGuideCommon.h"
#include "ServerTileService.h"
#include "TileSetDefinition.h"
#include "UnmanagedDataManager.h"
#include "SAX2Parser.h"

IMPLEMENT_CREATE_SERVICE(MgServerTileService)

MgServerTileService::MgServerTileService() : MgTileService()
{
    MgTileCacheDefault::Initialize();
}

MgServerTileService::~MgServerTileService()
{

}

MgByteReader* MgServerTileService::GetTile(
    MgMap* map,
    CREFSTRING baseMapLayerGroupName,
    INT32 tileColumn,
    INT32 tileRow)
{
    Ptr<MgByteReader> ret;

    MG_TRY()

    CHECKARGUMENTNULL(map, L"MgServerTileService.GetTile");
    CHECKARGUMENTEMPTYSTRING(baseMapLayerGroupName, L"MgServerTileService.GetTile");

    // find the finite display scale closest to the requested map scale
    double scale = map->GetViewScale();
    INT32 scaleIndex = map->FindNearestFiniteDisplayScaleIndex(scale);

    // if we don't find a nearest scale then something is wrong with the map
    if (scaleIndex < 0)
    {
        throw new MgInvalidMapDefinitionException(L"MgServerTileService.GetTile",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    Ptr<MgTileCacheDefault> cache = new MgTileCacheDefault(map);
    ret = cache->GetTile(baseMapLayerGroupName, tileColumn, tileRow, scaleIndex);

    MG_CATCH_AND_THROW(L"MgServerTileService.GetTile")

    return ret.Detach();
}

MgByteReader* MgServerTileService::GetTile(
    MgResourceIdentifier* resource,
    CREFSTRING baseMapLayerGroupName,
    INT32 tileColumn,
    INT32 tileRow,
    INT32 scaleIndex)
{
    Ptr<MgByteReader> ret;

    MG_TRY()

    CHECKARGUMENTNULL(resource, L"MgServerTileService.GetTile");
    CHECKARGUMENTEMPTYSTRING(baseMapLayerGroupName, L"MgServerTileService.GetTile");
        
    Ptr<MgTileCache> cache = GetTileCache(resource);
    ret = cache->GetTile(baseMapLayerGroupName, tileColumn, tileRow, scaleIndex);

    MG_CATCH_AND_THROW(L"MgServerTileService.GetTile")

    return ret.Detach();
}

void MgServerTileService::ClearCache(MgMap* map)
{
    MG_TRY()

    CHECKARGUMENTNULL(map, L"MgServerTileService.ClearCache");

    Ptr<MgTileCacheDefault> cache = new MgTileCacheDefault(map);
    cache->Clear();

    MG_CATCH_AND_THROW(L"MgServerTileService.ClearCache")
}

void MgServerTileService::ClearCache(MgResourceIdentifier* tileSet)
{
    MG_TRY()

    CHECKARGUMENTNULL(tileSet, L"MgServerTileService.ClearCache");

    Ptr<MgTileCache> cache = GetTileCache(tileSet);
    cache->Clear();

    MG_CATCH_AND_THROW(L"MgServerTileService.ClearCache")
}

INT32 MgServerTileService::GetDefaultTileSizeX()
{
    return MgTileParameters::tileWidth;
}

INT32 MgServerTileService::GetDefaultTileSizeY()
{
    return MgTileParameters::tileHeight;
}

INT32 MgServerTileService::GetDefaultTileSizeX(MgResourceIdentifier* tileSet)
{
    INT32 ret = 0;

    MG_TRY()

    Ptr<MgTileCache> cache = GetTileCache(tileSet);
    ret = cache->GetDefaultTileSizeX();

    MG_CATCH_AND_THROW(L"MgServerTileService.GetDefaultTileSizeX")
    
    return ret;
}

INT32 MgServerTileService::GetDefaultTileSizeY(MgResourceIdentifier* tileSet)
{
    INT32 ret = 0;

    MG_TRY()

    Ptr<MgTileCache> cache = GetTileCache(tileSet);
    ret = cache->GetDefaultTileSizeX();

    MG_CATCH_AND_THROW(L"MgServerTileService.GetDefaultTileSizeY")

    return ret;
}

MgByteReader* MgServerTileService::GetTileProviders()
{
    Ptr<MgByteReader> ret;

    MG_TRY()

    //This response is obviously pre-canned right now. If we do want to FDO-ize tile provider support
    //we obviously should revisit this.

    std::string xml = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
    xml.append("<TileProviderList xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"TileProviderList-3.0.0.xsd\">\n");

    //Default Tile Provider
    {
        xml.append("<TileProvider>\n");
        xml.append("<Name>");
        std::string mbName;
        MgUtil::WideCharToMultiByte(MG_TILE_PROVIDER_DEFAULT, mbName);
        xml.append(mbName);
        xml.append("</Name>\n");
        xml.append("<DisplayName>");
        std::string mbDisplayName;
        STRING wDisplayName = MgUtil::GetResourceMessage(MgResources::TileService, L"MgTileProvider_Default_DisplayName");
        MgUtil::WideCharToMultiByte(wDisplayName, mbDisplayName);
        xml.append(mbDisplayName);
        xml.append("</DisplayName>\n");
        xml.append("<Description>");
        std::string mbDescription;
        STRING wDescription = MgUtil::GetResourceMessage(MgResources::TileService, L"MgTileProvider_Default_Description");
        MgUtil::WideCharToMultiByte(wDescription, mbDescription);
        xml.append(mbDescription);
        xml.append("</Description>\n");
        xml.append("<ConnectionProperties>\n");
    
        //Property: TilePath
        xml.append("<ConnectionProperty Enumerable=\"false\" Protected=\"false\" Required=\"true\">\n");
        xml.append("<Name>");
        std::string mbTilePath;
        MgUtil::WideCharToMultiByte(MG_TILE_PROVIDER_COMMON_PARAM_TILEPATH, mbTilePath);
        xml.append(mbTilePath);
        xml.append("</Name>\n");
        xml.append("<LocalizedName>");
        std::string mbLocName;
        STRING wLocName = MgUtil::GetResourceMessage(MgResources::TileService, L"MgTileProvider_Common_Property_TilePath_LocalizedName");
        MgUtil::WideCharToMultiByte(wLocName, mbLocName);
        xml.append(mbLocName);
        xml.append("</LocalizedName>\n");
        xml.append("<DefaultValue>");
        std::string mbDefaultTag;
        MgUtil::WideCharToMultiByte(MgResourceTag::TileCachePath, mbDefaultTag);
        xml.append(mbDefaultTag);
        xml.append("</DefaultValue>\n");
        xml.append("</ConnectionProperty>\n");
    
        //Property: TileWidth
        xml.append("<ConnectionProperty Enumerable=\"false\" Protected=\"false\" Required=\"true\">\n");
        xml.append("<Name>");
        std::string mbTileWidth;
        MgUtil::WideCharToMultiByte(MG_TILE_PROVIDER_COMMON_PARAM_TILEWIDTH, mbTileWidth);
        xml.append(mbTileWidth);
        xml.append("</Name>\n");
        xml.append("<LocalizedName>");
        std::string mbLocTileWidth;
        STRING wLocTileWidth = MgUtil::GetResourceMessage(MgResources::TileService, L"MgTileProvider_Common_Property_TileWidth_LocalizedName");
        MgUtil::WideCharToMultiByte(wLocTileWidth, mbLocTileWidth);
        xml.append(mbLocTileWidth);
        xml.append("</LocalizedName>\n");
        xml.append("<DefaultValue>");
        std::string mbDefaultTileWidth;
        MgUtil::Int32ToString(MgConfigProperties::DefaultTileServicePropertyTileSizeX, mbDefaultTileWidth);
        xml.append(mbDefaultTileWidth);
        xml.append("</DefaultValue>\n");
        xml.append("</ConnectionProperty>\n");

        //Property: TileHeight
        xml.append("<ConnectionProperty Enumerable=\"false\" Protected=\"false\" Required=\"true\">\n");
        xml.append("<Name>");
        std::string mbTileHeight;
        MgUtil::WideCharToMultiByte(MG_TILE_PROVIDER_COMMON_PARAM_TILEHEIGHT, mbTileHeight);
        xml.append(mbTileHeight);
        xml.append("</Name>\n");
        xml.append("<LocalizedName>");
        std::string mbLocTileHeight;
        STRING wLocTileHeight = MgUtil::GetResourceMessage(MgResources::TileService, L"MgTileProvider_Common_Property_TileHeight_LocalizedName");
        MgUtil::WideCharToMultiByte(wLocTileHeight, mbLocTileHeight);
        xml.append(mbLocTileHeight);
        xml.append("</LocalizedName>\n");
        xml.append("<DefaultValue>");
        std::string mbDefaultTileHeight;
        MgUtil::Int32ToString(MgConfigProperties::DefaultTileServicePropertyTileSizeY, mbDefaultTileHeight);
        xml.append(mbDefaultTileHeight);
        xml.append("</DefaultValue>\n");
        xml.append("</ConnectionProperty>\n");

        //Property: TileFormat
        xml.append("<ConnectionProperty Enumerable=\"true\" Protected=\"false\" Required=\"true\">\n");
        xml.append("<Name>");
        std::string mbTileFormat;
        MgUtil::WideCharToMultiByte(MG_TILE_PROVIDER_COMMON_PARAM_TILEFORMAT, mbTileFormat);
        xml.append(mbTileFormat);
        xml.append("</Name>\n");
        xml.append("<LocalizedName>");
        std::string mbLocTileFormat;
        STRING wLocTileFormat = MgUtil::GetResourceMessage(MgResources::TileService, L"MgTileProvider_Common_Property_TileFormat_LocalizedName");
        MgUtil::WideCharToMultiByte(wLocTileFormat, mbLocTileFormat);
        xml.append(mbLocTileFormat);
        xml.append("</LocalizedName>\n");
        //Image formats supported by our tile service
        xml.append("<DefaultValue>PNG</DefaultValue>\n");
        xml.append("<Value>PNG</Value>");
        xml.append("<Value>PNG8</Value>");
        xml.append("<Value>JPG</Value>");
        xml.append("<Value>GIF</Value>");

        xml.append("</ConnectionProperty>\n");

        //Property: RenderOnly
        xml.append("<ConnectionProperty Enumerable=\"false\" Protected=\"false\" Required=\"false\">\n");
        xml.append("<Name>");
        std::string mbRenderOnly;
        MgUtil::WideCharToMultiByte(MG_TILE_PROVIDER_COMMON_PARAM_RENDERONLY, mbRenderOnly);
        xml.append(mbRenderOnly);
        xml.append("</Name>\n");
        xml.append("<LocalizedName>");
        std::string mbLocRenderOnly;
        STRING wLocRenderOnly = MgUtil::GetResourceMessage(MgResources::TileService, L"MgTileProvider_Common_Property_RenderOnly_LocalizedName");
        MgUtil::WideCharToMultiByte(wLocRenderOnly, mbLocRenderOnly);
        xml.append(mbLocRenderOnly);
        xml.append("</LocalizedName>\n");
        xml.append("<DefaultValue>false</DefaultValue>\n");
        xml.append("</ConnectionProperty>\n");

        //Property: CoordinateSystem
        xml.append("<ConnectionProperty Enumerable=\"false\" Protected=\"false\" Required=\"true\">\n");
        xml.append("<Name>");
        std::string mbCoordSys;
        MgUtil::WideCharToMultiByte(MG_TILE_PROVIDER_COMMON_PARAM_COORDINATESYSTEM, mbCoordSys);
        xml.append(mbCoordSys);
        xml.append("</Name>\n");
        xml.append("<LocalizedName>");
        std::string mbLocCoordSys;
        STRING wLocCoordSys = MgUtil::GetResourceMessage(MgResources::TileService, L"MgTileProvider_Common_Property_CoordinateSystem_LocalizedName");
        MgUtil::WideCharToMultiByte(wLocCoordSys, mbLocCoordSys);
        xml.append(mbLocCoordSys);
        xml.append("</LocalizedName>\n");
        xml.append("<DefaultValue></DefaultValue>\n");
        xml.append("</ConnectionProperty>\n");

        //Property: FiniteScaleList
        xml.append("<ConnectionProperty Enumerable=\"false\" Protected=\"false\" Required=\"true\">\n");
        xml.append("<Name>");
        std::string mbFiniteScaleList;
        MgUtil::WideCharToMultiByte(MG_TILE_PROVIDER_COMMON_PARAM_FINITESCALELIST, mbFiniteScaleList);
        xml.append(mbFiniteScaleList);
        xml.append("</Name>\n");
        xml.append("<LocalizedName>");
        std::string mbLocFiniteScaleList;
        STRING wLocFiniteScaleList = MgUtil::GetResourceMessage(MgResources::TileService, L"MgTileProvider_Common_Property_FiniteScaleList_LocalizedName");
        MgUtil::WideCharToMultiByte(wLocFiniteScaleList, mbLocFiniteScaleList);
        xml.append(mbLocFiniteScaleList);
        xml.append("</LocalizedName>\n");
        xml.append("<DefaultValue></DefaultValue>\n");
        xml.append("</ConnectionProperty>\n");

        xml.append("</ConnectionProperties>\n");
        xml.append("</TileProvider>\n");
    }
    //XYZ Tile Provider
    {
        xml.append("<TileProvider>\n");
        xml.append("<Name>");
        std::string mbName;
        STRING wName = MG_TILE_PROVIDER_XYZ;
        MgUtil::WideCharToMultiByte(wName, mbName);
        xml.append(mbName);
        xml.append("</Name>\n");
        xml.append("<DisplayName>");
        std::string mbDisplayName;
        STRING wDisplayName = MgUtil::GetResourceMessage(MgResources::TileService, L"MgTileProvider_XYZ_DisplayName");
        MgUtil::WideCharToMultiByte(wDisplayName, mbDisplayName);
        xml.append(mbDisplayName);
        xml.append("</DisplayName>\n");
        xml.append("<Description>");
        std::string mbDescription;
        STRING wDescription = MgUtil::GetResourceMessage(MgResources::TileService, L"MgTileProvider_XYZ_Description");
        MgUtil::WideCharToMultiByte(wDescription, mbDescription);
        xml.append(mbDescription);
        xml.append("</Description>\n");
        xml.append("<ConnectionProperties>\n");
    
        //Property: TilePath
        xml.append("<ConnectionProperty Enumerable=\"false\" Protected=\"false\" Required=\"true\">\n");
        xml.append("<Name>");
        std::string mbTilePath;
        MgUtil::WideCharToMultiByte(MG_TILE_PROVIDER_COMMON_PARAM_TILEPATH, mbTilePath);
        xml.append(mbTilePath);
        xml.append("</Name>\n");
        xml.append("<LocalizedName>");
        std::string mbLocName;
        STRING wLocName = MgUtil::GetResourceMessage(MgResources::TileService, L"MgTileProvider_Common_Property_TilePath_LocalizedName");
        MgUtil::WideCharToMultiByte(wLocName, mbLocName);
        xml.append(mbLocName);
        xml.append("</LocalizedName>\n");
        xml.append("<DefaultValue>");
        std::string mbDefaultTag;
        MgUtil::WideCharToMultiByte(MgResourceTag::TileCachePath, mbDefaultTag);
        xml.append(mbDefaultTag);
        xml.append("</DefaultValue>\n");
        xml.append("</ConnectionProperty>\n");

        //Property: TileFormat
        xml.append("<ConnectionProperty Enumerable=\"true\" Protected=\"false\" Required=\"true\">\n");
        xml.append("<Name>");
        std::string mbTileFormat;
        MgUtil::WideCharToMultiByte(MG_TILE_PROVIDER_COMMON_PARAM_TILEFORMAT, mbTileFormat);
        xml.append(mbTileFormat);
        xml.append("</Name>\n");
        xml.append("<LocalizedName>");
        std::string mbLocTileFormat;
        STRING wLocTileFormat = MgUtil::GetResourceMessage(MgResources::TileService, L"MgTileProvider_Common_Property_TileFormat_LocalizedName");
        MgUtil::WideCharToMultiByte(wLocTileFormat, mbLocTileFormat);
        xml.append(mbLocTileFormat);
        xml.append("</LocalizedName>\n");
        //Image formats supported by our tile service
        xml.append("<DefaultValue>PNG</DefaultValue>\n");
        xml.append("<Value>PNG</Value>");
        xml.append("<Value>PNG8</Value>");
        xml.append("<Value>JPG</Value>");
        xml.append("<Value>GIF</Value>");

        xml.append("</ConnectionProperty>\n");

        //Property: RenderOnly
        xml.append("<ConnectionProperty Enumerable=\"false\" Protected=\"false\" Required=\"false\">\n");
        xml.append("<Name>");
        std::string mbRenderOnly;
        MgUtil::WideCharToMultiByte(MG_TILE_PROVIDER_COMMON_PARAM_RENDERONLY, mbRenderOnly);
        xml.append(mbRenderOnly);
        xml.append("</Name>\n");
        xml.append("<LocalizedName>");
        std::string mbLocRenderOnly;
        STRING wLocRenderOnly = MgUtil::GetResourceMessage(MgResources::TileService, L"MgTileProvider_Common_Property_RenderOnly_LocalizedName");
        MgUtil::WideCharToMultiByte(wLocRenderOnly, mbLocRenderOnly);
        xml.append(mbLocRenderOnly);
        xml.append("</LocalizedName>\n");
        xml.append("<DefaultValue>false</DefaultValue>\n");
        xml.append("</ConnectionProperty>\n");

        xml.append("</ConnectionProperties>\n");
        xml.append("</TileProvider>\n");
    }

    xml.append("</TileProviderList>");

    Ptr<MgByteSource> source = new MgByteSource((BYTE_ARRAY_IN)xml.c_str(), (INT32)xml.length());
    source->SetMimeType(MgMimeType::Xml);
    ret = source->GetReader();

    MG_CATCH_AND_THROW(L"MgServerTileService.GetTileProviders")

    return ret.Detach();
}

void MgServerTileService::SetTile(
    MgByteReader* img,
    MgMap* map,
    INT32 scaleIndex,
    CREFSTRING baseMapLayerGroupName,
    INT32 tileColumn,
    INT32 tileRow)
{
    MG_TRY()

    Ptr<MgTileCacheDefault> cache = new MgTileCacheDefault(map);
    cache->SetTile(baseMapLayerGroupName, tileColumn, tileRow, scaleIndex, img);

    MG_CATCH_AND_THROW(L"MgServerTileService.SetTile")
}

MgTileCache* MgServerTileService::GetTileCache(MgResourceIdentifier* resource)
{
    Ptr<MgTileCache> cache;

    MG_TRY()

    if (resource->GetResourceType() == MgResourceType::MapDefinition)
    {
        cache = new MgTileCacheDefault(resource);
    }
    else if (resource->GetResourceType() == MgResourceType::TileSetDefinition)
    {
        // get service manager
        MgServiceManager* serviceMan = MgServiceManager::GetInstance();
        assert(NULL != serviceMan);

        // Get the service from service manager
        Ptr<MgResourceService> resourceService = dynamic_cast<MgResourceService*>(
            serviceMan->RequestService(MgServiceType::ResourceService));
        assert(NULL != resourceService);

        if (!resourceService->HasPermission(resource, MgResourcePermission::ReadOnly))
        {
            MG_LOG_AUTHENTICATION_ENTRY(MgResources::PermissionDenied.c_str());

            MgStringCollection arguments;
            arguments.Add(resource->ToString());

            throw new MgPermissionDeniedException(L"MgServerTileService.GetTileCache", __LINE__, __WFILE__, &arguments, L"", NULL);
        }

        Ptr<MgByteReader> content = resourceService->GetResourceContent(resource);
        Ptr<MgByteSink> sink = new MgByteSink(content);
        std::string xml;
        sink->ToStringUtf8(xml);

        // Before parsing, do any substitutions on the content
        MgUnmanagedDataManager::SubstituteDataPathAliases(xml);

        // parse the tile set definition
        MdfParser::SAX2Parser parser;
        parser.ParseString(xml.c_str(), xml.size());

        if (!parser.GetSucceeded())
        {
            STRING errorMsg = parser.GetErrorMessage();
            MgStringCollection arguments;
            arguments.Add(errorMsg);
            throw new MgXmlParserException(L"MgServerTileService.GetTileCache", __LINE__, __WFILE__, &arguments, L"", NULL);
        }

        MdfModel::TileSetDefinition* tileset = parser.DetachTileSetDefinition();
        assert(NULL != tileset);
        
        cache = GetTileCache(resource, tileset);
    }

    MG_CATCH_AND_THROW(L"MgServerTileService.GetTileCache")

    return cache.Detach();
}

MgTileCache* MgServerTileService::GetTileCache(MgResourceIdentifier* tileSetId, MdfModel::TileSetDefinition* tileset)
{
    Ptr<MgTileCache> cache;

    MG_TRY()

    MdfModel::TileStoreParameters* tilesetParams = tileset->GetTileStoreParameters();
    const MdfModel::MdfString& provider = tilesetParams->GetTileProvider();
    if (provider == MG_TILE_PROVIDER_DEFAULT)
    {
        MdfModel::NameStringPairCollection* parameters = tilesetParams->GetParameters();
        INT32 width = 300;
        INT32 height = 300;
        STRING format = L"PNG";
        STRING path;
        bool bRenderOnly = false;
        for (INT32 i = 0; i < parameters->GetCount(); i++)
        {
            MdfModel::NameStringPair* pair = parameters->GetAt(i);
            if (pair->GetName() == MG_TILE_PROVIDER_COMMON_PARAM_TILEPATH)
            {
                path = pair->GetValue();
            }
            else if (pair->GetName() == MG_TILE_PROVIDER_COMMON_PARAM_TILEWIDTH)
            {
                width = MgUtil::StringToInt32(pair->GetValue());
            }
            else if (pair->GetName() == MG_TILE_PROVIDER_COMMON_PARAM_TILEHEIGHT)
            {
                height = MgUtil::StringToInt32(pair->GetValue());
            }
            else if (pair->GetName() == MG_TILE_PROVIDER_COMMON_PARAM_TILEFORMAT)
            {
                format = pair->GetValue();
            }
            else if (pair->GetName() == MG_TILE_PROVIDER_COMMON_PARAM_RENDERONLY)
            {
                bRenderOnly = MgUtil::StringToBoolean(pair->GetValue());
            }
        }

        //If we find the cache path substitution tag, replace it with the default path from the configuration
        if (path == MgResourceTag::TileCachePath)
        {
            path = MgTileParameters::tileCachePath;
        }

        cache = new MgTileCacheDefaultProvider(tileSetId, path, width, height, format, bRenderOnly);
    }
    else if (provider == MG_TILE_PROVIDER_XYZ)
    {
        MdfModel::NameStringPairCollection* parameters = tilesetParams->GetParameters();
        STRING format = L"PNG";
        STRING path;
        bool bRenderOnly = false;
        for (INT32 i = 0; i < parameters->GetCount(); i++)
        {
            MdfModel::NameStringPair* pair = parameters->GetAt(i);
            if (pair->GetName() == MG_TILE_PROVIDER_COMMON_PARAM_TILEPATH)
            {
                path = pair->GetValue();
            }
            else if (pair->GetName() == MG_TILE_PROVIDER_COMMON_PARAM_TILEFORMAT)
            {
                format = pair->GetValue();
            }
            else if (pair->GetName() == MG_TILE_PROVIDER_COMMON_PARAM_RENDERONLY)
            {
                bRenderOnly = MgUtil::StringToBoolean(pair->GetValue());
            }
        }

        //If we find the cache path substitution tag, replace it with the default path from the configuration
        if (path == MgResourceTag::TileCachePath)
        {
            path = MgTileParameters::tileCachePath;
        }

        cache = new MgTileCacheXYZProvider(tileSetId, path, format, bRenderOnly);
    }
    else 
    {
        MgStringCollection arguments;
        arguments.Add(provider);
        throw new MgUnknownTileProviderException(L"MgServerTileService.GetTileCache", __LINE__, __WFILE__, &arguments, L"UnknownTileProvider", NULL);
    }

    MG_CATCH_AND_THROW(L"MgServerTileService.GetTileCache")

    return cache.Detach();
}

bool MgServerTileService::IsTileCacheEmpty() const
{
    return MgTileCacheDefault::IsTileCacheEmpty();
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Handle the Resource Change Notification event.
/// Any tile cache associated with the specified Map Definition resources
/// will be cleared.
///
bool MgServerTileService::NotifyResourcesChanged(MgSerializableCollection* resources, bool strict)
{
    bool success = true;

    if (NULL != resources)
    {
        INT32 numResources = resources->GetCount();

        if (numResources > 0)
        {
            for (INT32 i = 0; i < numResources; ++i)
            {
                Ptr<MgSerializable> serializableObj = resources->GetItem(i);
                MgResourceIdentifier* resource =
                    dynamic_cast<MgResourceIdentifier*>(serializableObj.p);
                ACE_ASSERT(NULL != resource);

                if (NULL != resource && (resource->IsResourceTypeOf(MgResourceType::MapDefinition) 
                                     || resource->IsResourceTypeOf(MgResourceType::TileSetDefinition)))
                {
                    MG_TRY()

                    // clear any tile cache associated with this map
                    Ptr<MgTileCache> cache = GetTileCache(resource);
                    cache->Clear();

                    //ACE_DEBUG ((LM_INFO, ACE_TEXT("(%t) Cleared tile cache for %W\n"), resource->ToString().c_str()));

                    MgTileCacheDefault* cacheDefault = dynamic_cast<MgTileCacheDefault*>(cache.p);
                    if (NULL != cacheDefault)
                    {
                        // clear any cached mgmap objects
                        cacheDefault->ClearMapCache(resource->ToString());
                        //ACE_DEBUG ((LM_INFO, ACE_TEXT("(%t) Cleared mem-cached MgMap instances for %W\n"), resource->ToString().c_str()));
                    }

                    MG_CATCH(L"MgServerTileService.NotifyResourcesChanged")

                    if (NULL != mgException.p)
                    {
                        success = false;

                        if (strict)
                        {
                            MG_THROW();
                        }
                        else
                        {
                            MgLogManager* logManager = MgLogManager::GetInstance();
                            ACE_ASSERT(NULL != logManager);
                            logManager->LogSystemErrorEntry(mgException.p);
                        }
                    }
                }
            }
        }
    }

    return success;
}

///////////////////////////////////////////////////////////////////////////////
void MgServerTileService::SetConnectionProperties(MgConnectionProperties*)
{
    // Do nothing.  No connection properties are required for Server-side service objects.
}
