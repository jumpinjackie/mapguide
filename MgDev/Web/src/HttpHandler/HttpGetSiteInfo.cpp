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

    // Create ServerAdmin object
    Ptr<MgServerAdmin> serverAdmin = new MgServerAdmin();
    serverAdmin->Open(m_userInfo);

    // call the C++ APIs
    Ptr<MgPropertyCollection> properties = serverAdmin->GetInformationProperties();
    STRING xml = GetXml(properties);

    Ptr<MgHttpPrimitiveValue> value = new MgHttpPrimitiveValue(xml);
    if(!value)
        throw new MgOutOfMemoryException(L"", __LINE__, __WFILE__, NULL, L"", NULL);

    hResult->SetResultObject(value, MgMimeType::Xml);

    MG_HTTP_HANDLER_CATCH_AND_THROW_EX(L"MgHttpGetSiteInfo.Execute")
}

STRING MgHttpGetSiteInfo::GetXml(MgPropertyCollection* properties)
{
    STRING xml;
    std::string tmpStr;
    Ptr<MgStringProperty> strProp;
    Ptr<MgInt64Property> int64Prop;
    Ptr<MgInt32Property> int32Prop;
    Ptr<MgBooleanProperty> boolProp;

    // this XML follows the SiteInformation-1.0.0.xsd schema
    xml += L"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    xml += L"<SiteInformation xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"SiteInformation-1.0.0.xsd\">\n";
    xml += L"\t<SiteServer>\n";

    xml += L"\t\t<DisplayName>";
    strProp = (MgStringProperty*)properties->GetItem(L"DisplayName");
    xml += strProp->GetValue();
    xml += L"</DisplayName>\n";

    xml += L"\t\t<Status>";
    boolProp = (MgBooleanProperty*)properties->GetItem(L"Status");
    xml += boolProp->GetValue() ? L"Online" : L"Offline";
    xml += L"</Status>\n";

    xml += L"\t\t<Version>";
    strProp = (MgStringProperty*)properties->GetItem(L"ServerVersion");
    xml += strProp->GetValue();
    xml += L"</Version>\n";

    xml += L"\t\t<OperatingSystem>\n";

    xml += L"\t\t\t<AvailablePhysicalMemory>";
    int64Prop = (MgInt64Property*)properties->GetItem(L"AvailablePhysicalMemory");
    MgUtil::Int64ToString(int64Prop->GetValue(), tmpStr);
    xml += MgUtil::MultiByteToWideChar(tmpStr);
    xml += L"</AvailablePhysicalMemory>\n";

    xml += L"\t\t\t<TotalPhysicalMemory>";
    int64Prop = (MgInt64Property*)properties->GetItem(L"TotalPhysicalMemory");
    MgUtil::Int64ToString(int64Prop->GetValue(), tmpStr);
    xml += MgUtil::MultiByteToWideChar(tmpStr);
    xml += L"</TotalPhysicalMemory>\n";

    xml += L"\t\t\t<AvailableVirtualMemory>";
    int64Prop = (MgInt64Property*)properties->GetItem(L"AvailableVirtualMemory");
    MgUtil::Int64ToString(int64Prop->GetValue(), tmpStr);
    xml += MgUtil::MultiByteToWideChar(tmpStr);
    xml += L"</AvailableVirtualMemory>\n";

    xml += L"\t\t\t<TotalVirtualMemory>";
    int64Prop = (MgInt64Property*)properties->GetItem(L"TotalVirtualMemory");
    MgUtil::Int64ToString(int64Prop->GetValue(), tmpStr);
    xml += MgUtil::MultiByteToWideChar(tmpStr);
    xml += L"</TotalVirtualMemory>\n";

    xml += L"\t\t\t<Version>";
    strProp = (MgStringProperty*)properties->GetItem(L"OperatingSystemVersion");
    xml += strProp->GetValue();
    xml += L"</Version>\n";

    xml += L"\t\t</OperatingSystem>\n";

    xml += L"\t</SiteServer>\n";

    xml += L"\t<Statistics>\n";

    xml += L"\t\t<AdminOperationsQueueCount>";
    int32Prop = (MgInt32Property*)properties->GetItem(L"AdminOperationsQueueCount");
    MgUtil::Int32ToString(int32Prop->GetValue(), tmpStr);
    xml += MgUtil::MultiByteToWideChar(tmpStr);
    xml += L"</AdminOperationsQueueCount>\n";

    xml += L"\t\t<ClientOperationsQueueCount>";
    int32Prop = (MgInt32Property*)properties->GetItem(L"ClientOperationsQueueCount");
    MgUtil::Int32ToString(int32Prop->GetValue(), tmpStr);
    xml += MgUtil::MultiByteToWideChar(tmpStr);
    xml += L"</ClientOperationsQueueCount>\n";

    xml += L"\t\t<SiteOperationsQueueCount>";
    int32Prop = (MgInt32Property*)properties->GetItem(L"SiteOperationsQueueCount");
    MgUtil::Int32ToString(int32Prop->GetValue(), tmpStr);
    xml += MgUtil::MultiByteToWideChar(tmpStr);
    xml += L"</SiteOperationsQueueCount>\n";

    xml += L"\t\t<AverageOperationTime>";
    int32Prop = (MgInt32Property*)properties->GetItem(L"AverageOperationTime");
    MgUtil::Int32ToString(int32Prop->GetValue(), tmpStr);
    xml += MgUtil::MultiByteToWideChar(tmpStr);
    xml += L"</AverageOperationTime>\n";

    xml += L"\t\t<CpuUtilization>";
    int32Prop = (MgInt32Property*)properties->GetItem(L"CpuUtilization");
    MgUtil::Int32ToString(int32Prop->GetValue(), tmpStr);
    xml += MgUtil::MultiByteToWideChar(tmpStr);
    xml += L"</CpuUtilization>\n";

    xml += L"\t\t<TotalOperationTime>";
    int32Prop = (MgInt32Property*)properties->GetItem(L"TotalOperationTime");
    MgUtil::Int32ToString(int32Prop->GetValue(), tmpStr);
    xml += MgUtil::MultiByteToWideChar(tmpStr);
    xml += L"</TotalOperationTime>\n";

    xml += L"\t\t<TotalConnections>";
    int32Prop = (MgInt32Property*)properties->GetItem(L"TotalConnections");
    MgUtil::Int32ToString(int32Prop->GetValue(), tmpStr);
    xml += MgUtil::MultiByteToWideChar(tmpStr);
    xml += L"</TotalConnections>\n";

    xml += L"\t\t<TotalOperationsProcessed>";
    int32Prop = (MgInt32Property*)properties->GetItem(L"TotalProcessedOperations");
    MgUtil::Int32ToString(int32Prop->GetValue(), tmpStr);
    xml += MgUtil::MultiByteToWideChar(tmpStr);
    xml += L"</TotalOperationsProcessed>\n";

    xml += L"\t\t<TotalOperationsReceived>";
    int32Prop = (MgInt32Property*)properties->GetItem(L"TotalReceivedOperations");
    MgUtil::Int32ToString(int32Prop->GetValue(), tmpStr);
    xml += MgUtil::MultiByteToWideChar(tmpStr);
    xml += L"</TotalOperationsReceived>\n";

    xml += L"\t\t<Uptime>";
    int32Prop = (MgInt32Property*)properties->GetItem(L"Uptime");
    MgUtil::Int32ToString(int32Prop->GetValue(), tmpStr);
    xml += MgUtil::MultiByteToWideChar(tmpStr);
    xml += L"</Uptime>\n";

    xml += L"\t</Statistics>\n";

    xml += L"</SiteInformation>\n";

    return xml;
}
