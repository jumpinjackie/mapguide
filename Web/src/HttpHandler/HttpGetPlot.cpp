//
//  Copyright (C) 2004-2009 by Autodesk, Inc.
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
#include "HttpGetPlot.h"

const double IN_TO_MM = 25.4;  // inches to millimeters conversion

HTTP_IMPLEMENT_CREATE_OBJECT(MgHttpGetPlot)

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
MgHttpGetPlot::MgHttpGetPlot(MgHttpRequest *hRequest)
{
    InitializeCommonParameters(hRequest);

    Ptr<MgHttpRequestParam> params = hRequest->GetRequestParam();
    STRING mapName = params->GetParameterValue(MgHttpResourceStrings::reqMappingMapName);
    STRING sessionId = this->m_userInfo->GetMgSessionId();
    m_map = new MgMap();
    Ptr<MgResourceService> resourceService = dynamic_cast<MgResourceService*>(this->CreateService(MgServiceType::ResourceService));
    if (!mapName.empty() && !sessionId.empty())
    {
        m_map->Open(resourceService, mapName);
    }
    else
    {
        Ptr<MgResourceIdentifier> resId = new MgResourceIdentifier(params->GetParameterValue(MgHttpResourceStrings::reqMappingMapDefinition));
        m_map->Create(resourceService, resId, resId->GetName());
    }

    STRING printLayout = params->GetParameterValue(MgHttpResourceStrings::reqMappingLayout);
    if(printLayout.length() > 0)
    {
        STRING layoutTitle = params->GetParameterValue(MgHttpResourceStrings::reqMappingLayoutTitle);
        STRING layoutUnits = params->GetParameterValue(MgHttpResourceStrings::reqMappingLayoutUnits);
        Ptr<MgResourceIdentifier> layoutId = new MgResourceIdentifier(printLayout);
        m_layout = new MgLayout(layoutId, layoutTitle, layoutUnits);
    }

    float width = (float) MgUtil::StringToDouble(params->GetParameterValue(MgHttpResourceStrings::reqMappingWidth));
    float height = (float) MgUtil::StringToDouble(params->GetParameterValue(MgHttpResourceStrings::reqMappingHeight));
    STRING units = params->GetParameterValue(MgHttpResourceStrings::reqMappingUnits);
    STRING margins = params->GetParameterValue(MgHttpResourceStrings::reqMappingMargins);

    float leftMargin = 1.0;
    float topMargin = 1.0;
    float rightMargin = 1.0;
    float bottomMargin = 1.0;

    if (_wcsnicmp(units.c_str(), L"mm", 3) == 0 ||
        _wcsnicmp(units.c_str(), L"millimeters", 12) == 0 )
    {
        leftMargin *= (float)IN_TO_MM;
        bottomMargin *= (float)IN_TO_MM;
        rightMargin *= (float)IN_TO_MM;
        topMargin *= (float)IN_TO_MM;
    }

    if (margins.length() > 0)
    {
        STRING token = L",";
        STRING::size_type start = 0;
        STRING::size_type end = margins.find(token);
        STRING substr;
        if (end > start && end < margins.length())
        {
            substr = margins.substr(start, end-start);
            leftMargin = (float) MgUtil::StringToDouble(substr);
        }

        start = end+1;
        end = margins.find(token, start);

        if (end > start && end < margins.length())
        {
            substr = margins.substr(start, end-start);
            topMargin = (float) MgUtil::StringToDouble(substr);
        }

        start = end+1;
        end = margins.find(token, start);

        if (end > start && end < margins.length())
        {
            substr = margins.substr(start, end-start);
            rightMargin = (float) MgUtil::StringToDouble(substr);

            substr = margins.substr(end+1);
            bottomMargin = (float) MgUtil::StringToDouble(substr);
        }

    }

    m_plotSpec  = new MgPlotSpecification(width, height, units, leftMargin, topMargin, rightMargin, bottomMargin);

    m_dwfVersion = params->GetParameterValue(MgHttpResourceStrings::reqMappingDwfVersion);
    m_eplotVersion = params->GetParameterValue(MgHttpResourceStrings::reqMappingEplotVersion);
}

/// <summary>
/// Executes the specific request.
/// </summary>
/// <returns>
/// MgHttpResponse
/// This contains the response (including MgHttpResult and StatusCode) from the server.
/// </returns>
void MgHttpGetPlot::Execute(MgHttpResponse& hResponse)
{
    Ptr<MgHttpResult> hResult = hResponse.GetResult();
    Ptr<MgHttpRequestParam> params = m_hRequest->GetRequestParam();

    MG_HTTP_HANDLER_TRY()

    //do not perform usual validation of common parameters, since for this request only version is
    //the mandatory parameter to verify
    if (params->GetParameterValue(MgHttpResourceStrings::reqVersion) == L"")
    {
        MgStringCollection arguments;
        arguments.Add(L"0");
        arguments.Add(MgResources::BlankArgument);

        throw new MgInvalidArgumentException(L"MgHttpGetPlot.Execute",
            __LINE__, __WFILE__, &arguments, L"MgStringEmpty", NULL);
    }

    //get the commands
    Ptr<MgPropertyCollection> commands = params->GetParameters()->GetPropertyCollection();

    //execute the request through a dwf controller
    MgDwfController controller(m_siteConn,m_hRequest->GetAgentUri());
    Ptr<MgByteReader> reader = controller.GetPlot(m_map, m_plotSpec, m_layout, m_dwfVersion, m_eplotVersion, commands);

    hResult->SetResultObject(reader, reader->GetMimeType());

    MG_HTTP_HANDLER_CATCH_AND_THROW_EX(L"MgHttpGetPlot.Execute")
}
