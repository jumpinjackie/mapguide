//
//  Copyright (C) 2004-2007 by Autodesk, Inc.
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

//////////////////////////////////////////////////////////////////
/// <summary>
/// Constructor.
/// </summary>
MgHttpRequestResponseHandler::MgHttpRequestResponseHandler() { }

/// <summary>
/// Initializes the common parameters of the request.
/// </summary>
/// <param name="name">Input
/// MgHttpRequest
/// This contains all the parameters of the request.
/// </param>
void MgHttpRequestResponseHandler::InitializeCommonParameters(MgHttpRequest *hRequest)
{
    MG_HTTP_HANDLER_TRY()

    // Check that this HTTP operation is enabled in webconfig.ini
    STRING disableProperty;
    switch (GetRequestClassification())
    {
        case MgHttpRequestResponseHandler::mrcViewer:
            // Always enable Viewer requests
            break;

        case MgHttpRequestResponseHandler::mrcAuthor:
            disableProperty = MgConfigProperties::AgentDisableAuthoring;
            break;


        case MgHttpRequestResponseHandler::mrcWms:
            disableProperty = MgConfigProperties::AgentDisableWms;
            break;

        case MgHttpRequestResponseHandler::mrcWfs:
            disableProperty = MgConfigProperties::AgentDisableWfs;
            break;

        default:
            break;
    }

    if (disableProperty.length() > 0)
    {
        bool bDisable = false;
        MgConfiguration* config = MgConfiguration::GetInstance();
        config->GetBoolValue(MgConfigProperties::AgentPropertiesSection, disableProperty, bDisable, false);
        if (bDisable)
        {
            throw new MgInvalidOperationException(L"HttpRequestResponseHandler.InitializeCommonParamters", __LINE__, __WFILE__, NULL, L"", NULL);
        }
    }

    m_hRequest = SAFE_ADDREF((MgHttpRequest*)hRequest);

    m_userInfo = new MgUserInformation();

    Ptr<MgHttpRequestParam> hrParam = m_hRequest->GetRequestParam();

    // Get version
    m_version = hrParam->GetParameterValue(MgHttpResourceStrings::reqVersion);

    // Get sesssion
    STRING session = hrParam->GetParameterValue(MgHttpResourceStrings::reqSession);
    if (session.length() > 0)
    {
        m_userInfo->SetMgSessionId(session);
    }

    // Get username and password
    STRING userName = hrParam->GetParameterValue(MgHttpResourceStrings::reqUsername);
    STRING password = hrParam->GetParameterValue(MgHttpResourceStrings::reqPassword);

    if (userName.length() > 0)
    {
        m_userInfo->SetMgUsernamePassword(userName, password);
    }
    else
    {
        // If no username/password or session supplied, default to Anonymous.
        if (session.length() <= 0)
        {
            userName = MgUser::Anonymous;
            password = L"";
            m_userInfo->SetMgUsernamePassword(userName, password);
        }
    }

    // Get locale
    STRING locale = hrParam->GetParameterValue(MgHttpResourceStrings::reqLocale);
    if (locale.length() > 0)
    {
        m_userInfo->SetLocale(locale);
    }

    // Get client agent (CLIENT)
    STRING clientAgent = hrParam->GetParameterValue(MgHttpResourceStrings::reqClientAgent);
    if (clientAgent.length() > 0)
    {
        m_userInfo->SetClientAgent(clientAgent);
    }

    // Get client ip address (CLIENTIP)
    STRING clientIp = hrParam->GetParameterValue(MgHttpResourceStrings::reqClientIp);
    if (clientIp.length() > 0)
    {
        m_userInfo->SetClientIp(clientIp);
    }

    // And create the site connection
    m_siteConn = new MgSiteConnection();
    m_siteConn->Open(m_userInfo);

    MG_HTTP_HANDLER_CATCH_AND_THROW(L"MgHttpRequestResponseHandler.InitializeCommonParameters")
}

/// <summary>
/// Validates the common parameters of the request.
/// </summary>
void MgHttpRequestResponseHandler::ValidateCommonParameters()
{
    MG_HTTP_HANDLER_TRY()

    if (m_version == L"")
    {
        MgStringCollection arguments;
        arguments.Add(L"0");
        arguments.Add(MgResources::BlankArgument);

        throw new MgInvalidArgumentException(L"MgHttpRequestResponseHandler.ValidateCommonParameters",
            __LINE__, __WFILE__, &arguments, L"MgStringEmpty", NULL);
    }

    ValidateOperationVersion();

    MG_HTTP_HANDLER_CATCH_AND_THROW(L"MgHttpRequestResponseHandler.ValidateCommonParameters")
}

/// <summary>
/// This method is responsible for checking if
/// a valid version was given
/// </summary>
/// <returns>Returns nothing</returns>
void MgHttpRequestResponseHandler::ValidateOperationVersion()
{
    MG_HTTP_HANDLER_TRY()

    if (m_version != L"1.0.0")
    {
        throw new MgInvalidOperationVersionException(
        L"MgHttpRequestResponseHandler.ValidateOperationVersion", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MG_HTTP_HANDLER_CATCH_AND_THROW(L"MgHttpRequestResponseHandler.ValidateOperationVersion");
}

/// <summary>
/// Validates the common parameters of the request.
/// </summary>
/// <param name="serviceType">Input
/// This defines the service type to create.
/// </param>
/// <returns>
/// Pointer to MgService based on serviceType.
/// </returns>
MgService* MgHttpRequestResponseHandler::CreateService(INT16 serviceType)
{
    MG_HTTP_HANDLER_TRY()

    Ptr<MgService> service = m_siteConn->CreateService(serviceType);

    return SAFE_ADDREF((MgService*)service);

    MG_HTTP_HANDLER_CATCH_AND_THROW(L"MgHttpRequestResponseHandler.CreateService")

    return NULL;
}

MgHttpRequestResponseHandler::~MgHttpRequestResponseHandler()
{
}
