//
//  Copyright (C) 2004-2006  Autodesk, Inc.
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
#include "HttpGetSiteInfo.h"

HTTP_IMPLEMENT_CREATE_OBJECT(MgHttpGetSiteInfo)

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
MgHttpGetSiteInfo::MgHttpGetSiteInfo(MgHttpRequest *hRequest)
{
    InitializeCommonParameters(hRequest);
}

/// <summary>
/// Executes the specific request.
/// </summary>
/// <returns>
/// MgHttpResponse
/// This contains the response (including MgHttpResult and StatusCode) from the server.
/// </returns>
void MgHttpGetSiteInfo::Execute(MgHttpResponse& hResponse)
{
    Ptr<MgHttpResult> hResult = hResponse.GetResult();

    MG_HTTP_HANDLER_TRY()

    // Check common parameters
    ValidateCommonParameters();

    // Get user information
    MgUserInformation* userInfo = MgUserInformation::GetCurrentUserInfo();

    // Open connection to Mg server
    Ptr<MgSite> mgSite;
    mgSite = new MgSite();
    mgSite->Open(userInfo);
    STRING serverAddress = mgSite->GetSiteServerAddress();

    // Create ServerAdmin object
    Ptr<MgServerAdmin> serverAdmin = new MgServerAdmin();
    serverAdmin->Open(serverAddress, userInfo);

    // call the C++ APIs
    Ptr<MgPropertyCollection> properties = serverAdmin->GetInformationProperties();
    STRING xml = GetXml(properties, serverAdmin->IsOnline());

    Ptr<MgHttpPrimitiveValue> value = new MgHttpPrimitiveValue(xml);
    if(!value)
        throw new MgOutOfMemoryException(L"", __LINE__, __WFILE__, NULL, L"", NULL);

    hResult->SetResultObject(value, MgMimeType::Xml);

    MG_HTTP_HANDLER_CATCH_AND_THROW_EX(L"MgHttpGetSiteInfo.Execute")
}

STRING MgHttpGetSiteInfo::GetXml(MgPropertyCollection* properties, bool isOnline)
{
    STRING xml = L"";
    std::string tmpStr = "";
    INT64 longVal = 0;
    INT32 intVal = 0;

    // this XML follows the SiteInformation-1.0.0.xsd schema
    xml += L"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    xml += L"<SiteInformation xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"SiteInformation-1.0.0.xsd\">\n";
    xml += L"\t<SiteServer>\n";

    xml += L"\t\t<DisplayName>";
    xml += ((MgStringProperty*)properties->GetItem(L"DisplayName"))->GetValue();
    xml += L"</DisplayName>\n";

    xml += L"\t\t<Status>";
    xml += isOnline? L"Online" : L"Offline";
    xml += L"</Status>\n";

    xml += L"\t\t<Version>";
    xml += ((MgStringProperty*)properties->GetItem(L"ServerVersion"))->GetValue();
    xml += L"</Version>\n";

    xml += L"\t\t<OperatingSystem>\n";

    xml += L"\t\t\t<AvailablePhysicalMemory>";
    longVal = ((MgInt64Property*)properties->GetItem(L"AvailablePhysicalMemory"))->GetValue();
    MgUtil::Int64ToString(longVal, tmpStr);
    xml += MgUtil::MultiByteToWideChar(tmpStr);
    xml += L"</AvailablePhysicalMemory>\n";

    xml += L"\t\t\t<TotalPhysicalMemory>";
    longVal = ((MgInt64Property*)properties->GetItem(L"TotalPhysicalMemory"))->GetValue();
    MgUtil::Int64ToString(longVal, tmpStr);
    xml += MgUtil::MultiByteToWideChar(tmpStr);
    xml += L"</TotalPhysicalMemory>\n";

    xml += L"\t\t\t<AvailableVirtualMemory>";
    longVal = ((MgInt64Property*)properties->GetItem(L"AvailableVirtualMemory"))->GetValue();
    MgUtil::Int64ToString(longVal, tmpStr);
    xml += MgUtil::MultiByteToWideChar(tmpStr);
    xml += L"</AvailableVirtualMemory>\n";

    xml += L"\t\t\t<TotalVirtualMemory>";
    longVal = ((MgInt64Property*)properties->GetItem(L"TotalVirtualMemory"))->GetValue();
    MgUtil::Int64ToString(longVal, tmpStr);
    xml += MgUtil::MultiByteToWideChar(tmpStr);
    xml += L"</TotalVirtualMemory>\n";

    xml += L"\t\t\t<Version>";
    xml += ((MgStringProperty*)properties->GetItem(L"OperatingSystemVersion"))->GetValue();
    xml += L"</Version>\n";

    xml += L"\t\t</OperatingSystem>\n";

    xml += L"\t</SiteServer>\n";

    xml += L"\t<Statistics>\n";

    xml += L"\t\t<AdminOperationsQueueCount>";
    intVal = ((MgInt32Property*)properties->GetItem(L"AdminOperationsQueueCount"))->GetValue();
    MgUtil::Int32ToString(intVal, tmpStr);
    xml += MgUtil::MultiByteToWideChar(tmpStr);
    xml += L"</AdminOperationsQueueCount>\n";

    xml += L"\t\t<ClientOperationsQueueCount>";
    intVal = ((MgInt32Property*)properties->GetItem(L"ClientOperationsQueueCount"))->GetValue();
    MgUtil::Int32ToString(intVal, tmpStr);
    xml += MgUtil::MultiByteToWideChar(tmpStr);
    xml += L"</ClientOperationsQueueCount>\n";

    xml += L"\t\t<SiteOperationsQueueCount>";
    intVal = ((MgInt32Property*)properties->GetItem(L"SiteOperationsQueueCount"))->GetValue();
    MgUtil::Int32ToString(intVal, tmpStr);
    xml += MgUtil::MultiByteToWideChar(tmpStr);
    xml += L"</SiteOperationsQueueCount>\n";

    xml += L"\t\t<AverageOperationTime>";
    intVal = ((MgInt32Property*)properties->GetItem(L"AverageOperationTime"))->GetValue();
    MgUtil::Int32ToString(intVal, tmpStr);
    xml += MgUtil::MultiByteToWideChar(tmpStr);
    xml += L"</AverageOperationTime>\n";

    xml += L"\t\t<CpuUtilization>";
    intVal = ((MgInt32Property*)properties->GetItem(L"CpuUtilization"))->GetValue();
    MgUtil::Int32ToString(intVal, tmpStr);
    xml += MgUtil::MultiByteToWideChar(tmpStr);
    xml += L"</CpuUtilization>\n";

    xml += L"\t\t<TotalOperationTime>";
    intVal = ((MgInt32Property*)properties->GetItem(L"TotalOperationTime"))->GetValue();
    MgUtil::Int32ToString(intVal, tmpStr);
    xml += MgUtil::MultiByteToWideChar(tmpStr);
    xml += L"</TotalOperationTime>\n";

    xml += L"\t\t<TotalConnections>";
    intVal = ((MgInt32Property*)properties->GetItem(L"TotalConnections"))->GetValue();
    MgUtil::Int32ToString(intVal, tmpStr);
    xml += MgUtil::MultiByteToWideChar(tmpStr);
    xml += L"</TotalConnections>\n";

    xml += L"\t\t<TotalOperationsProcessed>";
    intVal = ((MgInt32Property*)properties->GetItem(L"TotalProcessedOperations"))->GetValue();
    MgUtil::Int32ToString(intVal, tmpStr);
    xml += MgUtil::MultiByteToWideChar(tmpStr);
    xml += L"</TotalOperationsProcessed>\n";

    xml += L"\t\t<TotalOperationsReceived>";
    intVal = ((MgInt32Property*)properties->GetItem(L"TotalReceivedOperations"))->GetValue();
    MgUtil::Int32ToString(intVal, tmpStr);
    xml += MgUtil::MultiByteToWideChar(tmpStr);
    xml += L"</TotalOperationsReceived>\n";

    xml += L"\t\t<Uptime>";
    intVal = ((MgInt32Property*)properties->GetItem(L"Uptime"))->GetValue();
    MgUtil::Int32ToString(intVal, tmpStr);
    xml += MgUtil::MultiByteToWideChar(tmpStr);
    xml += L"</Uptime>\n";

    xml += L"\t</Statistics>\n";


    xml += L"</SiteInformation>\n";

    return xml;
}
