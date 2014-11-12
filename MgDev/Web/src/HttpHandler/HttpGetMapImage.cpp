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
#include "HttpGetMapImage.h"

HTTP_IMPLEMENT_CREATE_OBJECT(MgHttpGetMapImage)

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
MgHttpGetMapImage::MgHttpGetMapImage(MgHttpRequest *hRequest)
{
    InitializeCommonParameters(hRequest);

    Ptr<MgHttpRequestParam> params = hRequest->GetRequestParam();

    // Get any map name
    m_mapName = params->GetParameterValue(MgHttpResourceStrings::reqRenderingMapName);

    // Get any map definition
    m_mapDefinition = params->GetParameterValue(MgHttpResourceStrings::reqRenderingMapDefinition);

    // Get the requested image format
    m_mapFormat = params->GetParameterValue(MgHttpResourceStrings::reqRenderingFormat);

    // Get the keep selection flag
    m_bKeepSelection = true; // default
    STRING keepSelection = params->GetParameterValue(MgHttpResourceStrings::reqRenderingKeepSelection);
    if(!keepSelection.empty())
    {
        m_bKeepSelection = (keepSelection == L"1");
    }

    // Get the requires clipping flag
    m_bClip = false; // default
    STRING clip = params->GetParameterValue(MgHttpResourceStrings::reqRenderingClip);
    if(!clip.empty())
    {
        m_bClip = (clip == L"1");
    }
}

/// <summary>
/// Executes the specific request.
/// </summary>
/// <returns>
/// MgHttpResponse
/// This contains the response (including MgHttpResult and StatusCode) from the server.
/// </returns>
void MgHttpGetMapImage::Execute(MgHttpResponse& hResponse)
{
    Ptr<MgHttpResult> hResult = hResponse.GetResult();
    Ptr<MgHttpRequestParam> params = m_hRequest->GetRequestParam();

    MG_HTTP_HANDLER_TRY()

    // Check common parameters
    ValidateCommonParameters();

    STRING sessionId = m_userInfo->GetMgSessionId();

    Ptr<MgResourceService> resourceService = dynamic_cast<MgResourceService*>(CreateService(MgServiceType::ResourceService));

    bool openedMap = false;

    // Create MgMap and selection
    Ptr<MgMap> map = new MgMap();
    Ptr<MgSelection> selection;
    bool bLayersAndGroupsAreIds = true;
    if (!m_mapName.empty() && !sessionId.empty())
    {
        openedMap = true;

        map->Open(resourceService, m_mapName);

        // Get the selection
        selection = new MgSelection(map);
        selection->Open(resourceService, m_mapName);
    }
    else
    {
        Ptr<MgResourceIdentifier> resId = new MgResourceIdentifier(m_mapDefinition);
        map->Create(resourceService, resId, resId->GetName());

        // No selection in this case
        bLayersAndGroupsAreIds = false;
    }

    // Get the commands
    Ptr<MgPropertyCollection> commands = params->GetParameters()->GetPropertyCollection();

    // Call the HTML controller to render the map image
    MgHtmlController controller(m_siteConn);

    // v3.0 supports a selection color parameter
    Ptr<MgColor> selColor;
    INT32 version = m_userInfo->GetApiVersion();
    if (version == MG_API_VERSION(3,0,0))
    {
        STRING strSelColor = params->GetParameterValue(MgHttpResourceStrings::reqRenderingSelectionColor);
        if (!strSelColor.empty())
        {
            selColor = new MgColor(strSelColor);
        }
    }

    Ptr<MgByteReader> reader = controller.GetMapImage(map, selection, m_mapFormat, commands, m_bKeepSelection, m_bClip, selColor, bLayersAndGroupsAreIds);

    // If we opened the map from the repository then save it back to ensure
    // any track changes are removed from the persisted version, since these
    // are not applicable for AJAX.
    if (openedMap)
        map->Save(resourceService);

    // Set the result
    hResult->SetResultObject(reader, reader->GetMimeType());

    MG_HTTP_HANDLER_CATCH_AND_THROW_EX(L"MgHttpGetMapImage.Execute")
}

/// <summary>
/// This method is responsible for checking if
/// a valid version was given
/// </summary>
/// <returns>Returns nothing</returns>
void MgHttpGetMapImage::ValidateOperationVersion()
{
    MG_HTTP_HANDLER_TRY()

    // There are multiple supported versions
    INT32 version = m_userInfo->GetApiVersion();
    if (version != MG_API_VERSION(1,0,0) &&
        version != MG_API_VERSION(3,0,0))
    {
        throw new MgInvalidOperationVersionException(
        L"MgHttpGetMapImage.ValidateOperationVersion", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MG_HTTP_HANDLER_CATCH_AND_THROW(L"MgHttpGetMapImage.ValidateOperationVersion");
}
