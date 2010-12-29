//
//  Copyright (C) 2004-2010 by Autodesk, Inc.
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

/// \brief
/// Default constructor
///
MgConnectionProperties::MgConnectionProperties()
{
}

/// \brief
/// Constructor for HTTP connection properties
///
MgConnectionProperties::MgConnectionProperties(MgUserInformation* userInfo, CREFSTRING url)
{
    if (NULL == userInfo)
    {
        throw new MgNullArgumentException(
            L"MgConnectionProperties.MgConnectionProperties",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    if (url.empty())
    {
        MgStringCollection arguments;
        arguments.Add(L"2");
        arguments.Add(MgResources::BlankArgument);

        throw new MgInvalidArgumentException(
            L"MgConnectionProperties.MgConnectionProperties",
            __LINE__, __WFILE__, &arguments, L"MgStringEmpty", NULL);
    }

    m_userInfo = SAFE_ADDREF(userInfo);
    m_url = url;
    m_port = 0;
}

/// \brief
/// Constructor for TCP/IP connection properties
///
MgConnectionProperties::MgConnectionProperties(MgUserInformation* userInfo, CREFSTRING target, INT32 port)
{
    if (NULL == userInfo)
    {
        throw new MgNullArgumentException(
            L"MgConnectionProperties.MgConnectionProperties",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    m_userInfo = SAFE_ADDREF(userInfo);
    m_target = target; // may be empty
    m_port = port;
}


/// <summary>
/// Destructor
/// </summary>
MgConnectionProperties::~MgConnectionProperties()
{
}

bool MgConnectionProperties::IsLocal()
{
    return m_url.empty() && m_target.empty();
}

/// <summary>
/// Returns the URL for HTTP connection
/// </summary>
/// <returns>
/// returns URL string
/// </returns>
STRING MgConnectionProperties::GetUrl()
{
    return m_url;
}

/// <summary>
/// Returns the target for TCP/IP connection
/// </summary>
/// <returns>
/// returns target string
/// </returns>
STRING MgConnectionProperties::GetTarget()
{
    return m_target;
}

/// <summary>
/// Returns the port for TCP/IP connection
/// </summary>
/// <returns>
/// returns port#
/// </returns>
INT32 MgConnectionProperties::GetPort()
{
    return m_port;
}

/// <summary>
/// Compares data members of this object and the object supplied
/// </summary>
/// <param name="connProp">
/// Connection properties object to be compared
/// <param>
/// <returns>
/// true - if properties match
/// false - if properties do not match
/// </returns>
bool MgConnectionProperties::Equals(MgConnectionProperties* connProp)
{
    bool isequal = false;

    if ( (this->m_port == connProp->GetPort())               &&
         ((this->m_url).compare(connProp->GetUrl()) == 0)    &&
         ((this->m_target).compare(connProp->GetTarget()) == 0)
       )
    {
        isequal = true;
    }

    return isequal;
}

wstring MgConnectionProperties::Hash()
{
    wchar_t wsPort[20];
    #ifdef _WIN32
    _itow(m_port, wsPort, 10);
    #else
    swprintf(wsPort, 20, L"%d", m_port);
    #endif

    wstring tmp = m_url;
    tmp.append(m_target);
    tmp.append(wsPort);
    return tmp;
}


/// <summary>
/// Self destructor
/// </summary>
void MgConnectionProperties::Dispose()
{
    delete this;
}

/// <summary>
/// Returns class id
/// </summary>
INT32 MgConnectionProperties::GetClassId()
{
    return m_cls_id;
}

/// <summary>
/// Serializes data over TCP/IP connection
/// </summary>
void MgConnectionProperties::Serialize(MgStream* stream)
{
    MG_UNUSED_ARG(stream);
}

/// <summary>
/// Deserializes data from TCP/IP stream
/// </summary>
void MgConnectionProperties::Deserialize(MgStream* stream)
{
    MG_UNUSED_ARG(stream);
}

/// \brief
/// Gets user information for connection
///
MgUserInformation* MgConnectionProperties::GetUserInfo()
{
    return SAFE_ADDREF(m_userInfo.p);
}
