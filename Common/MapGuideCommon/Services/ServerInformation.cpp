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
#include "ServerInformation.h"

MG_IMPL_DYNCREATE(MgServerInformation);

// Notes:
//
// 1. The order of these must match the MgServiceType enumeration.
// 2. The number of supported services must match the value of
//    MgServerInformation::sm_knMaxNumberServices.
// 3. The service names must be in sync with the those defined in class
//    MgConfigProperties.
// 4. Referencing the service names with MgConfigProperties::HostProperty???'s
//    will not work well because these strings may have not been initialized
//    yet (and this will result in a segmentation fault on Linux).

const MgServerInformation::MgServiceInfoMap MgServerInformation::sm_serviceInfoMap[] =
{
    { MgServiceType::ResourceService   , L"ResourceService"   , MgServiceFlag::ResourceService    },
    { MgServiceType::DrawingService    , L"DrawingService"    , MgServiceFlag::DrawingService     },
    { MgServiceType::FeatureService    , L"FeatureService"    , MgServiceFlag::FeatureService     },
    { MgServiceType::MappingService    , L"MappingService"    , MgServiceFlag::MappingService     },
    { MgServiceType::RenderingService  , L"RenderingService"  , MgServiceFlag::RenderingService   },
    { MgServiceType::TileService       , L"TileService"       , MgServiceFlag::TileService        },
    { MgServiceType::KmlService        , L"KmlService"        , MgServiceFlag::KmlService         },
    { MgServiceType::ServerAdminService, L"ServerAdminService", MgServiceFlag::ServerAdminService },
    { MgServiceType::SiteService       , L"SiteService"       , MgServiceFlag::SiteService        },
    { MgServiceType::ProfilingService  , L"ProfilingService"  , MgServiceFlag::ProfilingService   },
};

//////////////////////////////////////////////////////////////
/// <summary>
/// Constructs an uninitialized object.
/// </summary>
MgServerInformation::MgServerInformation() :
    m_serviceFlags(MgServiceFlag::ServerAdminService)
{
}

//////////////////////////////////////////////////////////////
/// <summary>
/// Constructs the object with the specified information.
/// </summary>
MgServerInformation::MgServerInformation(CREFSTRING identifier, CREFSTRING name,
    CREFSTRING description, CREFSTRING address) :
    m_identifier(identifier),
    m_name(name),
    m_description(description),
    m_address(address),
    m_serviceFlags(MgServiceFlag::ServerAdminService)
{
    Validate();
}

//////////////////////////////////////////////////////////////
/// <summary>
/// Copy constructor.
/// </summary>
MgServerInformation::MgServerInformation(const MgServerInformation& serverInfo)
{
    *this = serverInfo;
}

//////////////////////////////////////////////////////////////
/// <summary>
/// Destructs the object.
/// </summary>
MgServerInformation::~MgServerInformation()
{
}

//////////////////////////////////////////////////////////////
/// <summary>
/// Disposes the object.
/// </summary>
void MgServerInformation::Dispose()
{
    delete this;
}

//////////////////////////////////////////////////////////////
/// <summary>
/// Assignment operator.
/// </summary>
MgServerInformation& MgServerInformation::operator=(const MgServerInformation& serverInfo)
{
    if (&serverInfo != this)
    {
        m_identifier   = serverInfo.m_identifier;
        m_name         = serverInfo.m_name;
        m_description  = serverInfo.m_description;
        m_address      = serverInfo.m_address;
        m_serviceFlags = serverInfo.m_serviceFlags;
    }

    return *this;
}

//////////////////////////////////////////////////////////////
/// <summary>
/// Tests two objects for equality.
/// </summary>
bool MgServerInformation::operator==(const MgServerInformation &serverInfo) const
{
    return (m_identifier   == serverInfo.m_identifier
         && m_name         == serverInfo.m_name
         && m_description  == serverInfo.m_description
         && m_address      == serverInfo.m_address
         && m_serviceFlags == serverInfo.m_serviceFlags);
}

//////////////////////////////////////////////////////////////
/// <summary>
/// Tests two objects for inequality.
/// </summary>
bool MgServerInformation::operator!=(const MgServerInformation &serverInfo) const
{
    return (!(serverInfo == *this));
}

//////////////////////////////////////////////////////////////
/// <summary>
/// Returns the class ID.
/// </summary>
///
/// <returns>
/// Class ID.
/// </returns>
INT32 MgServerInformation::GetClassId()
{
    return m_cls_id;
}

//////////////////////////////////////////////////////////////
/// <summary>
/// Serializes data to TCP/IP stream.
/// </summary>
///
/// <param name="stream">
/// Stream
/// </param>
void MgServerInformation::Serialize(MgStream* stream)
{
    stream->WriteString(m_identifier);
    stream->WriteString(m_name);
    stream->WriteString(m_description);
    stream->WriteString(m_address);
    stream->WriteInt32(m_serviceFlags);
}

//////////////////////////////////////////////////////////////
/// <summary>
/// Deserializes data from TCP/IP stream.
/// </summary>
///
/// <param name="stream">
/// Stream
/// </param>
void MgServerInformation::Deserialize(MgStream* stream)
{
    stream->GetString(m_identifier);
    stream->GetString(m_name);
    stream->GetString(m_description);
    stream->GetString(m_address);
    stream->GetInt32(m_serviceFlags);
}

//////////////////////////////////////////////////////////////
/// <summary>
/// Does it allow to set the name
/// </summary>
bool MgServerInformation::CanSetName()
{
    return true;
}

//////////////////////////////////////////////////////////////
/// \brief
/// Determine if this is a Site server.
///
/// \return
/// true if this is a Site server, false otherwise.
///
bool MgServerInformation::IsSiteServer() const
{
    return IsServiceEnabled(MgServiceType::SiteService);
}

//////////////////////////////////////////////////////////////
/// <summary>
/// Checks whether or not the server name is valid.
/// </summary>
///
/// <param name="name">
/// Name of the server.
/// </param>
void MgServerInformation::CheckName(CREFSTRING name)
{
    if (name.empty())
    {
        throw new MgNullArgumentException(L"MgServerInformation.CheckName",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    if (wstring::npos != name.find_first_of(L"[]="))
    {
        MgStringCollection arguments;
        arguments.Add(name);

        throw new MgInvalidServerNameException(
            L"MgServerInformation.CheckName",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
}

//////////////////////////////////////////////////////////////
/// <summary>
/// Checks whether or not the server description is valid.
/// </summary>
///
/// <param name="description">
/// Description of the server.
/// </param>
void MgServerInformation::CheckDescription(CREFSTRING description)
{
    if (STRING::npos != description.find_first_of(L"[]="))
    {
        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(description);

        MgStringCollection whyArguments;
        whyArguments.Add(L"[]=");

        throw new MgInvalidArgumentException(L"MgServerInformation.CheckDescription",
            __LINE__, __WFILE__, &arguments, L"MgStringContainsReservedCharacters", &whyArguments);
    }
}

//////////////////////////////////////////////////////////////
/// <summary>
/// Checks whether or not the server IP address is valid.
/// </summary>
///
/// <param name="address">
/// IP address of the server.
/// </param>
void MgServerInformation::CheckAddress(CREFSTRING address)
{
    if (address.empty())
    {
        throw new MgNullArgumentException(L"MgServerInformation.CheckAddress",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    if (wstring::npos != address.find_first_of(L"[]="))
    {
        MgStringCollection arguments;
        arguments.Add(address);

        throw new MgInvalidIpAddressException(
            L"MgServerInformation.CheckAddress",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
}

//////////////////////////////////////////////////////////////
/// <summary>
/// Converts the specified service name to a service type.
/// </summary>
///
/// <param name="serviceName">
/// Name of service.
/// </param>
///
/// <returns>
/// Type of service.
/// </returns>
INT32 MgServerInformation::ToServiceType(CREFSTRING serviceName, bool strict)
{
    for (INT32 i = 0; i < sm_knMaxNumberServices; ++i)
    {
        if (sm_serviceInfoMap[i].m_serviceName == serviceName)
        {
            return i;
        }
    }

    if (strict)
    {
        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(serviceName);

        throw new MgInvalidArgumentException(L"MgServerInformation.ToServiceType",
            __LINE__, __WFILE__, &arguments, L"MgNameNotFound", NULL);
    }

    return -1;
}

//////////////////////////////////////////////////////////////
/// <summary>
/// Converts the specified service type to a service name.
/// </summary>
///
/// <param name="serviceType">
/// Type of service.
/// </param>
///
/// <returns>
/// Name of service.
/// </returns>
STRING MgServerInformation::ToServiceName(INT32 serviceType, bool strict)
{
    if (serviceType >= 0 && serviceType < sm_knMaxNumberServices)
    {
        return sm_serviceInfoMap[serviceType].m_serviceName;
    }
    else if (strict)
    {
        STRING buffer;
        MgUtil::Int32ToString(serviceType, buffer);

        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(buffer);

        throw new MgInvalidArgumentException(L"MgServerInformation.ToServiceName",
            __LINE__, __WFILE__, &arguments, L"MgInvalidServiceType", NULL);
    }

    return L"";
}

//////////////////////////////////////////////////////////////
/// <summary>
/// Converts the specified service type to a service flag.
/// </summary>
///
/// <param name="serviceType">
/// Type of service.
/// </param>
///
/// <returns>
/// Service flag.
/// </returns>
INT32 MgServerInformation::ToServiceFlag(INT32 serviceType, bool strict)
{
    if (serviceType >= 0 && serviceType < sm_knMaxNumberServices)
    {
        return sm_serviceInfoMap[serviceType].m_serviceFlag;
    }
    else if (strict)
    {
        STRING buffer;
        MgUtil::Int32ToString(serviceType, buffer);

        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(buffer);

        throw new MgInvalidArgumentException(L"MgServerInformation.ToServiceFlag",
            __LINE__, __WFILE__, &arguments, L"MgInvalidServiceType", NULL);
    }

    return 0;
}

//////////////////////////////////////////////////////////////
/// <summary>
/// Converts the specified host properties to service flags.
/// </summary>
///
/// <param name="hostProps">
/// Host properties.
/// </param>
/// <param name="serviceFlags">
/// Initial service flags.
/// </param>
///
/// <returns>
/// Service flags.
/// </returns>
INT32 MgServerInformation::ToServiceFlags(MgPropertyCollection* hostProps,
    INT32 initialFlags)
{
    if (NULL == hostProps)
    {
        throw new MgNullReferenceException(
            L"MgServerInformation.ToServiceFlags", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    INT32 serviceFlags = initialFlags;

    for (INT32 i = 0; i < hostProps->GetCount(); ++i)
    {
        Ptr<MgProperty> property = hostProps->GetItem(i);
        INT32 serviceType = ToServiceType(property->GetName(), false);

        if (serviceType >= 0)
        {
            MgStringProperty* host =
                dynamic_cast<MgStringProperty*>(property.p);

            if (NULL == host)
            {
                // Everything in the config file is treated as a string
                // so we should not be here.

                throw new MgInvalidPropertyTypeException(
                    L"MgServerInformation::ToServiceFlags", __LINE__, __WFILE__, NULL, L"", NULL);
            }

            if (0 == ACE_OS::atoi(host->GetValue().c_str()))
            {
                serviceFlags &= ~sm_serviceInfoMap[serviceType].m_serviceFlag;
            }
            else
            {
                serviceFlags |= sm_serviceInfoMap[serviceType].m_serviceFlag;
            }
        }
    }

    return serviceFlags;
}

//////////////////////////////////////////////////////////////
/// <summary>
/// Gets the identifier of the server.
/// </summary>
///
/// <returns>
/// The server identifier.
/// </returns>
STRING MgServerInformation::GetIdentifier()
{
    return m_identifier;
}

//////////////////////////////////////////////////////////////
/// <summary>
/// Sets the identifier of the server.
/// </summary>
///
/// <param name="identifier">
/// Identifier of the server.
/// </param>
///
/// <returns>
/// Nothing
/// </returns>
void MgServerInformation::SetIdentifier(CREFSTRING identifier)
{
    m_identifier = identifier;
}

//////////////////////////////////////////////////////////////
/// <summary>
/// Gets the name of the server.
/// </summary>
///
/// <returns>
/// The server name.
/// </returns>
STRING MgServerInformation::GetName()
{
    return m_name;
}

//////////////////////////////////////////////////////////////
/// <summary>
/// Sets the name of the server.
/// </summary>
///
/// <param name="name">
/// Name of the server.
/// </param>
///
/// <returns>
/// Nothing
/// </returns>
void MgServerInformation::SetName(CREFSTRING name)
{
    m_name = name;
}

//////////////////////////////////////////////////////////////
/// <summary>
/// Gets the description of the server.
/// </summary>
///
/// <returns>
/// The server description.
/// </returns>
STRING MgServerInformation::GetDescription()
{
    return m_description;
}

//////////////////////////////////////////////////////////////
/// <summary>
/// Sets the description of the server.
/// </summary>
///
/// <param name="description">
/// Description of the server.
/// </param>
///
/// <returns>
/// Nothing
/// </returns>
void MgServerInformation::SetDescription(CREFSTRING description)
{
    m_description = description;
}

//////////////////////////////////////////////////////////////
/// <summary>
/// Gets the IP address of the server.
/// </summary>
///
/// <returns>
/// The server IP address.
/// </returns>
STRING MgServerInformation::GetAddress()
{
    return m_address;
}

//////////////////////////////////////////////////////////////
/// <summary>
/// Sets the IP address of the server.
/// </summary>
///
/// <param name="address">
/// IP address of the server.
/// </param>
///
/// <returns>
/// Nothing
/// </returns>
void MgServerInformation::SetAddress(CREFSTRING address)
{
    m_address = address;
}

//////////////////////////////////////////////////////////////
/// <summary>
/// Validates the server information.
/// </summary>
///
/// <returns>
/// Nothing.
/// </returns>
void MgServerInformation::Validate()
{
    CheckName(m_name);
    CheckDescription(m_description);
    CheckAddress(m_address);
}

//////////////////////////////////////////////////////////////
/// <summary>
/// Determines if the specified service is currently enabled.
/// </summary>
///
/// <param name="serviceType">
/// Type of service.
/// </param>
///
/// <returns>
/// true if the specified service is enabled, and false otherwise.
/// </returns>
bool MgServerInformation::IsServiceEnabled(INT32 serviceType) const
{
    if (serviceType < 0 || serviceType >= sm_knMaxNumberServices)
    {
        STRING buffer;
        MgUtil::Int32ToString(serviceType, buffer);

        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(buffer);

        throw new MgInvalidArgumentException(L"MgServerInformation.IsServiceEnabled",
            __LINE__, __WFILE__, &arguments, L"MgInvalidServiceType", NULL);
    }

    return (sm_serviceInfoMap[serviceType].m_serviceFlag & m_serviceFlags);
}

//////////////////////////////////////////////////////////////
/// <summary>
/// Gets the bit-wise flags that specify which services should be enabled/disabled.
/// </summary>
///
/// <returns>
/// The service flags.
/// </returns>
INT32 MgServerInformation::GetServiceFlags()
{
    return m_serviceFlags;
}

//////////////////////////////////////////////////////////////
/// <summary>
/// Sets the service flags.
/// </summary>
///
/// <param name="serviceFlags">
/// Bit-wise flags that specify which services should be enabled/disabled.
/// </param>
///
/// <returns>
/// Nothing
/// </returns>
void MgServerInformation::SetServiceFlags(INT32 serviceFlags)
{
    // Ensure the internal Server Admin Service is always enabled.
    m_serviceFlags = (serviceFlags | MgServiceFlag::ServerAdminService);
}

//////////////////////////////////////////////////////////////
/// <summary>
/// Copies the specified server information into this object.
/// </summary>
void MgServerInformation::CopyFrom(const MgServerInformation& serverInfo,
    bool sourceFromSiteServer)
{
    if (&serverInfo != this)
    {
        if (m_address != serverInfo.m_address &&
            0 != MgIpUtil::CompareAddresses(m_address, serverInfo.m_address))
        {
            throw new MgInvalidOperationException(L"MgServerInformation.CopyFrom",
                __LINE__, __WFILE__, NULL, L"", NULL);
        }

        if (sourceFromSiteServer)
        {
            m_identifier   = serverInfo.m_identifier;
            m_name         = serverInfo.m_name;
            m_description  = serverInfo.m_description;
            m_address      = serverInfo.m_address;
        }
        else
        {
            m_serviceFlags = serverInfo.m_serviceFlags;
        }
    }
}
