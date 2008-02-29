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
#include "MapGuideCommon.h"

MG_IMPL_DYNCREATE(MgSiteInfo);

///----------------------------------------------------------------------------
/// <summary>
/// Constructs the object.
/// </summary>
///----------------------------------------------------------------------------

MgSiteInfo::MgSiteInfo(CREFSTRING target, INT32 sitePort, INT32 clientPort, INT32 adminPort) :
    m_clientPort(clientPort),
    m_sitePort(sitePort),
    m_adminPort(adminPort),
    m_target(target),
    m_status(Ok)
{
}

///----------------------------------------------------------------------------
/// <summary>
/// Constructs the object.
/// </summary>
///----------------------------------------------------------------------------

MgSiteInfo::MgSiteInfo() :
    m_clientPort(0),
    m_sitePort(0),
    m_adminPort(0),
    m_target(L""),
    m_status(Uninitialized)
{
}

///----------------------------------------------------------------------------
/// <summary>
/// Constructs the object.
/// </summary>
///----------------------------------------------------------------------------

MgSiteInfo::MgSiteInfo(CREFSTRING hexString) :
    m_clientPort(0),
    m_sitePort(0),
    m_adminPort(0),
    m_target(L""),
    m_status(Uninitialized)
{
    if(hexString.length() >= HexStringLength)
    {
        UINT32 n1, n2, n3, n4;

        // Read the IP parts into their variables
        if(::swscanf(hexString.c_str(), L"%2X%2X%2X%2X%4X%4X%4X", &n1, &n2, &n3, &n4, &m_sitePort, &m_clientPort, &m_adminPort) == 7)
        {
            // Write the 4 'n' values into an IP string
            wchar_t buffer[20];
            swprintf(buffer, 20, L"%u.%u.%u.%u", n1, n2, n3, n4);
            m_target = buffer;
            m_status = Ok;
        }
    }
}

///----------------------------------------------------------------------------
/// <summary>
/// Destructs the object.
/// </summary>
///----------------------------------------------------------------------------

MgSiteInfo::~MgSiteInfo()
{
}

///----------------------------------------------------------------------------
/// <summary>
/// Disposes the object.
/// </summary>
///----------------------------------------------------------------------------

void MgSiteInfo::Dispose()
{
    delete this;
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the classId.
/// </summary>
INT32 MgSiteInfo::GetClassId()
{
    return m_cls_id;
}

///////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Serialize data to TCP/IP stream.
/// </summary>
void MgSiteInfo::Serialize(MgStream* stream)
{
    stream->WriteString(m_target);
    stream->WriteInt32(m_sitePort);
    stream->WriteInt32(m_clientPort);
    stream->WriteInt32(m_adminPort);
    stream->WriteInt32((INT32)m_status);
}

///////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Deserialize data from TCP/IP stream.
/// </summary>
void MgSiteInfo::Deserialize(MgStream* stream)
{
    stream->GetString(m_target);
    stream->GetInt32(m_sitePort);
    stream->GetInt32(m_clientPort);
    stream->GetInt32(m_adminPort);
    INT32 statusVal;
    stream->GetInt32(statusVal);
    m_status = (MgSiteStatus)statusVal;
}

///////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the hex string representation of the target and ports
/// </summary>
STRING MgSiteInfo::ToHexString()
{
    STRING hexString;
    UINT32 n1, n2, n3, n4;
    wchar_t buffer[30];
    if (4 == ::swscanf(m_target.c_str(), L"%u.%u.%u.%u", &n1, &n2, &n3, &n4))
    {
        swprintf(buffer, 30, L"%.2X%.2X%.2X%.2X%.4X%.4X%.4X", n1, n2, n3, n4, m_sitePort, m_clientPort, m_adminPort);
        hexString = buffer;
    }
    return hexString;
}

///////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the IP target
/// </summary>
STRING MgSiteInfo::GetTarget()
{
    return m_target;
}

///////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the port for the specified connection type
/// </summary>
INT32 MgSiteInfo::GetPort(MgPortType type)
{
    INT32 port = 0;
    switch(type)
    {
        case Client:
        {
            port = m_clientPort;
            break;
        }
        case Site:
        {
            port = m_sitePort;
            break;
        }
        case Admin:
        {
            port = m_adminPort;
            break;
        }
    };
    return port;
}

///////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the status of this site server connection
/// </summary>
MgSiteInfo::MgSiteStatus MgSiteInfo::GetStatus()
{
    return m_status;
}

///////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Sets the status of this site server connection
/// </summary>
void MgSiteInfo::SetStatus(MgSiteStatus status)
{
    m_status = status;
}


