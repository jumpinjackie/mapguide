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
#include "HttpGetSiteInfo.h"
#include "MapGuideCommon.h"

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

    STRING xml;
    xml += BeginXml();

    if (m_userInfo->GetApiVersion() >= MG_API_VERSION(2,2,0))
    {
        MgSiteManager* siteManager = MgSiteManager::GetInstance();
        if(siteManager)
        {
            MgSiteVector* sites = siteManager->GetSites();
            if(sites)
            {
                for(size_t i=0;i<sites->size();i++)
                {
                    MgSiteInfo* siteInfo = sites->at(i);

                    // Check the server status - though this status could be out of date and an exception might be thrown
                    bool bHaveSiteInfo = false;
                    STRING message = MgResources::Unknown;

                    if (MgSiteInfo::Ok == siteInfo->GetStatus())
                    {
                        MG_HTTP_HANDLER_TRY()

                        // Create ServerAdmin object
                        Ptr<MgServerAdmin> serverAdmin = new MgServerAdmin();
                        serverAdmin->Open(siteInfo->GetTarget(), m_userInfo);

                        // call the C++ APIs
                        Ptr<MgPropertyCollection> properties = serverAdmin->GetInformationProperties();
                        xml += GetXml(properties);
                        bHaveSiteInfo = true;

                        MG_HTTP_HANDLER_CATCH(L"MgHttpGetSiteInfo.Execute")
                        if (mgException != NULL)
                        {
                            message = mgException->GetExceptionMessage();
                        }
                    }

                    if(!bHaveSiteInfo)
                    {
                        // This server is not available
                        xml += L"\t<Server>\n";

                        xml += L"\t\t<IpAddress>";
                        xml += siteInfo->GetTarget();
                        xml += L"</IpAddress>\n";
                        xml += L"\t\t<DisplayName></DisplayName>\n";
                        xml += L"\t\t<Status>";
                        xml += message;
                        xml += L"</Status>\n";
                        xml += L"\t\t<Version></Version>\n";
                        xml += L"\t\t<OperatingSystem></OperatingSystem>\n";
                        xml += L"\t\t<Statistics></Statistics>\n";

                        xml += L"\t</Server>\n";
                    }
                }
            }
        }
    }
    else
    {
        // Create ServerAdmin object
        Ptr<MgServerAdmin> serverAdmin = new MgServerAdmin();
        serverAdmin->Open(m_userInfo);

        // call the C++ APIs
        Ptr<MgPropertyCollection> properties = serverAdmin->GetInformationProperties();
        xml += GetXml(properties);
    }
    xml += EndXml();

    Ptr<MgHttpPrimitiveValue> value = new MgHttpPrimitiveValue(xml);
    if(!value)
        throw new MgOutOfMemoryException(L"", __LINE__, __WFILE__, NULL, L"", NULL);

    hResult->SetResultObject(value, MgMimeType::Xml);

    MG_HTTP_HANDLER_CATCH_AND_THROW_EX(L"MgHttpGetSiteInfo.Execute")
}

STRING MgHttpGetSiteInfo::BeginXml()
{
    STRING xml = L"";

    xml += L"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    if (m_userInfo->GetApiVersion() >= MG_API_VERSION(2,2,0))
    {
        xml += L"<SiteInformation xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"SiteInformation-2.2.0.xsd\">\n";
    }
    else
    {
        xml += L"<SiteInformation xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"SiteInformation-1.0.0.xsd\">\n";
    }

    return xml;
}

STRING MgHttpGetSiteInfo::EndXml()
{
    STRING xml = L"";

    xml = L"</SiteInformation>\n";

    return xml;
}

STRING MgHttpGetSiteInfo::GetXml(MgPropertyCollection* properties)
{
    STRING xml;
    std::string tmpStr;
    Ptr<MgStringProperty> strProp;
    Ptr<MgInt64Property> int64Prop;
    Ptr<MgInt32Property> int32Prop;
    Ptr<MgBooleanProperty> boolProp;

    if (m_userInfo->GetApiVersion() >= MG_API_VERSION(2,2,0))
    {
        xml += L"\t<Server>\n";
    }
    else
    {
        xml += L"\t<SiteServer>\n";
    }

    if (m_userInfo->GetApiVersion() >= MG_API_VERSION(2,2,0))
    {
        xml += L"\t\t<IpAddress>";
        strProp = (MgStringProperty*)properties->GetItem(MgServerInformationProperties::MachineIp);
        xml += strProp->GetValue();
        xml += L"</IpAddress>\n";
    }

    xml += L"\t\t<DisplayName>";
    strProp = (MgStringProperty*)properties->GetItem(MgServerInformationProperties::DisplayName);
    xml += strProp->GetValue();
    xml += L"</DisplayName>\n";

    xml += L"\t\t<Status>";
    boolProp = (MgBooleanProperty*)properties->GetItem(MgServerInformationProperties::Status);
    xml += boolProp->GetValue() ? L"Online" : L"Offline";
    xml += L"</Status>\n";

    xml += L"\t\t<Version>";
    strProp = (MgStringProperty*)properties->GetItem(MgServerInformationProperties::ServerVersion);
    xml += strProp->GetValue();
    xml += L"</Version>\n";

    xml += L"\t\t<OperatingSystem>\n";

    xml += L"\t\t\t<AvailablePhysicalMemory>";
    int64Prop = (MgInt64Property*)properties->GetItem(MgServerInformationProperties::AvailablePhysicalMemory);
    MgUtil::Int64ToString(int64Prop->GetValue(), tmpStr);
    xml += MgUtil::MultiByteToWideChar(tmpStr);
    xml += L"</AvailablePhysicalMemory>\n";

    xml += L"\t\t\t<TotalPhysicalMemory>";
    int64Prop = (MgInt64Property*)properties->GetItem(MgServerInformationProperties::TotalPhysicalMemory);
    MgUtil::Int64ToString(int64Prop->GetValue(), tmpStr);
    xml += MgUtil::MultiByteToWideChar(tmpStr);
    xml += L"</TotalPhysicalMemory>\n";

    xml += L"\t\t\t<AvailableVirtualMemory>";
    int64Prop = (MgInt64Property*)properties->GetItem(MgServerInformationProperties::AvailableVirtualMemory);
    MgUtil::Int64ToString(int64Prop->GetValue(), tmpStr);
    xml += MgUtil::MultiByteToWideChar(tmpStr);
    xml += L"</AvailableVirtualMemory>\n";

    xml += L"\t\t\t<TotalVirtualMemory>";
    int64Prop = (MgInt64Property*)properties->GetItem(MgServerInformationProperties::TotalVirtualMemory);
    MgUtil::Int64ToString(int64Prop->GetValue(), tmpStr);
    xml += MgUtil::MultiByteToWideChar(tmpStr);
    xml += L"</TotalVirtualMemory>\n";

    xml += L"\t\t\t<Version>";
    strProp = (MgStringProperty*)properties->GetItem(MgServerInformationProperties::OperatingSystemVersion);
    xml += strProp->GetValue();
    xml += L"</Version>\n";

    xml += L"\t\t</OperatingSystem>\n";

    if (m_userInfo->GetApiVersion() == MG_API_VERSION(1,0,0))
    {
        xml += L"\t</SiteServer>\n";
    }

    xml += L"\t<Statistics>\n";

    xml += L"\t\t<AdminOperationsQueueCount>";
    int32Prop = (MgInt32Property*)properties->GetItem(MgServerInformationProperties::AdminOperationsQueueCount);
    MgUtil::Int32ToString(int32Prop->GetValue(), tmpStr);
    xml += MgUtil::MultiByteToWideChar(tmpStr);
    xml += L"</AdminOperationsQueueCount>\n";

    xml += L"\t\t<ClientOperationsQueueCount>";
    int32Prop = (MgInt32Property*)properties->GetItem(MgServerInformationProperties::ClientOperationsQueueCount);
    MgUtil::Int32ToString(int32Prop->GetValue(), tmpStr);
    xml += MgUtil::MultiByteToWideChar(tmpStr);
    xml += L"</ClientOperationsQueueCount>\n";

    xml += L"\t\t<SiteOperationsQueueCount>";
    int32Prop = (MgInt32Property*)properties->GetItem(MgServerInformationProperties::SiteOperationsQueueCount);
    MgUtil::Int32ToString(int32Prop->GetValue(), tmpStr);
    xml += MgUtil::MultiByteToWideChar(tmpStr);
    xml += L"</SiteOperationsQueueCount>\n";

    xml += L"\t\t<AverageOperationTime>";
    int32Prop = (MgInt32Property*)properties->GetItem(MgServerInformationProperties::AverageOperationTime);
    MgUtil::Int32ToString(int32Prop->GetValue(), tmpStr);
    xml += MgUtil::MultiByteToWideChar(tmpStr);
    xml += L"</AverageOperationTime>\n";

    xml += L"\t\t<CpuUtilization>";
    int32Prop = (MgInt32Property*)properties->GetItem(MgServerInformationProperties::CpuUtilization);
    MgUtil::Int32ToString(int32Prop->GetValue(), tmpStr);
    xml += MgUtil::MultiByteToWideChar(tmpStr);
    xml += L"</CpuUtilization>\n";

    if (m_userInfo->GetApiVersion() >= MG_API_VERSION(2,2,0))
    {
        xml += L"\t\t<WorkingSet>";
        int64Prop = (MgInt64Property*)properties->GetItem(MgServerInformationProperties::WorkingSet);
        MgUtil::Int64ToString(int64Prop->GetValue(), tmpStr);
        xml += MgUtil::MultiByteToWideChar(tmpStr);
        xml += L"</WorkingSet>\n";

        xml += L"\t\t<VirtualMemory>";
        int64Prop = (MgInt64Property*)properties->GetItem(MgServerInformationProperties::VirtualMemory);
        MgUtil::Int64ToString(int64Prop->GetValue(), tmpStr);
        xml += MgUtil::MultiByteToWideChar(tmpStr);
        xml += L"</VirtualMemory>\n";
    }

    xml += L"\t\t<TotalOperationTime>";
    int32Prop = (MgInt32Property*)properties->GetItem(MgServerInformationProperties::TotalOperationTime);
    MgUtil::Int32ToString(int32Prop->GetValue(), tmpStr);
    xml += MgUtil::MultiByteToWideChar(tmpStr);
    xml += L"</TotalOperationTime>\n";

    xml += L"\t\t<ActiveConnections>";
    int32Prop = (MgInt32Property*)properties->GetItem(MgServerInformationProperties::TotalActiveConnections);
    MgUtil::Int32ToString(int32Prop->GetValue(), tmpStr);
    xml += MgUtil::MultiByteToWideChar(tmpStr);
    xml += L"</ActiveConnections>\n";

    xml += L"\t\t<TotalConnections>";
    int32Prop = (MgInt32Property*)properties->GetItem(MgServerInformationProperties::TotalConnections);
    MgUtil::Int32ToString(int32Prop->GetValue(), tmpStr);
    xml += MgUtil::MultiByteToWideChar(tmpStr);
    xml += L"</TotalConnections>\n";

    xml += L"\t\t<TotalOperationsProcessed>";
    int32Prop = (MgInt32Property*)properties->GetItem(MgServerInformationProperties::TotalProcessedOperations);
    MgUtil::Int32ToString(int32Prop->GetValue(), tmpStr);
    xml += MgUtil::MultiByteToWideChar(tmpStr);
    xml += L"</TotalOperationsProcessed>\n";

    xml += L"\t\t<TotalOperationsReceived>";
    int32Prop = (MgInt32Property*)properties->GetItem(MgServerInformationProperties::TotalReceivedOperations);
    MgUtil::Int32ToString(int32Prop->GetValue(), tmpStr);
    xml += MgUtil::MultiByteToWideChar(tmpStr);
    xml += L"</TotalOperationsReceived>\n";

    xml += L"\t\t<Uptime>";
    int32Prop = (MgInt32Property*)properties->GetItem(MgServerInformationProperties::Uptime);
    MgUtil::Int32ToString(int32Prop->GetValue(), tmpStr);
    xml += MgUtil::MultiByteToWideChar(tmpStr);
    xml += L"</Uptime>\n";

    if (m_userInfo->GetApiVersion() >= MG_API_VERSION(2,2,0))
    {
        xml += L"\t\t<CacheSize>";
        int32Prop = (MgInt32Property*)properties->GetItem(MgServerInformationProperties::CacheSize);
        MgUtil::Int32ToString(int32Prop->GetValue(), tmpStr);
        xml += MgUtil::MultiByteToWideChar(tmpStr);
        xml += L"</CacheSize>\n";

        xml += L"\t\t<CacheDroppedEntries>";
        int32Prop = (MgInt32Property*)properties->GetItem(MgServerInformationProperties::CacheDroppedEntries);
        MgUtil::Int32ToString(int32Prop->GetValue(), tmpStr);
        xml += MgUtil::MultiByteToWideChar(tmpStr);
        xml += L"</CacheDroppedEntries>\n";
    }

    xml += L"\t</Statistics>\n";

    if (m_userInfo->GetApiVersion() >= MG_API_VERSION(2,2,0))
    {
        xml += L"\t</Server>\n";
    }

    return xml;
}
