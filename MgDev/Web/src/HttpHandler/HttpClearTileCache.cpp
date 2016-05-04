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

#include "HttpHandler.h"
#include "HttpClearTileCache.h"

HTTP_IMPLEMENT_CREATE_OBJECT(MgHttpClearTileCache)

/// <summary>
/// Initializes the common parameters and parameters specific to this request.
/// </summary>
/// <param name="name">Input
/// MgHttpRequest
/// This contains all the parameters of the request.
/// </param>
/// <returns>
/// nothing
/// </returns>
MgHttpClearTileCache::MgHttpClearTileCache(MgHttpRequest *hRequest)
{
    InitializeCommonParameters(hRequest);

    Ptr<MgHttpRequestParam> params = hRequest->GetRequestParam();

    // Get the map definition
    m_mapDefinition = params->GetParameterValue(MgHttpResourceStrings::reqTileMapDefinition);
}

/// <summary>
/// Executes the specific request.
/// </summary>
/// <returns>
/// MgHttpResponse
/// This contains the response (including MgHttpResult and StatusCode) from the server.
/// </returns>
void MgHttpClearTileCache::Execute(MgHttpResponse& hResponse)
{
    Ptr<MgHttpResult> hResult = hResponse.GetResult();

    MG_HTTP_HANDLER_TRY()

    // Check common parameters
    ValidateCommonParameters();

    if (m_mapDefinition.empty())
    {
        MgStringCollection arguments;
        arguments.Add(L"0");
        arguments.Add(MgResources::BlankArgument);

        throw new MgInvalidArgumentException(L"MgHttpClearTileCache.Execute",
            __LINE__, __WFILE__, &arguments, L"MgStringEmpty", NULL);
    }

    // Need a resource identifier
    Ptr<MgResourceIdentifier> resource = new MgResourceIdentifier(m_mapDefinition);

    if (resource->IsResourceTypeOf(MgResourceType::MapDefinition))
    {
        // Get Resource Service instance
        Ptr<MgResourceService> resourceService = (MgResourceService*)CreateService(MgServiceType::ResourceService);

        // Create MgMap
        Ptr<MgMap> map = new MgMap();
        map->Create(resourceService, resource, resource->GetName());

        // Get Proxy Tile Service instance
        Ptr<MgTileService> service = (MgTileService*)(CreateService(MgServiceType::TileService));

        // Call the C++ API
        service->ClearCache(map);
    }
    else if (resource->IsResourceTypeOf(MgResourceType::TileSetDefinition))
    {
        // Get Proxy Tile Service instance
        Ptr<MgTileService> service = (MgTileService*)(CreateService(MgServiceType::TileService));

        // Call the C++ API
        service->ClearCache(resource);
    }
    MG_HTTP_HANDLER_CATCH_AND_THROW_EX(L"MgHttpClearTileCache.Execute")
}
