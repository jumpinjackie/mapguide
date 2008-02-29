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

#ifndef MG_CONNECTION_PROPERTIES_H_
#define MG_CONNECTION_PROPERTIES_H_


class MG_MAPGUIDE_API MgConnectionProperties;

#ifdef _WIN32
template class MG_MAPGUIDE_API Ptr<MgConnectionProperties>;
#endif

class MgStream;
class MgSerializable;

/// \cond INTERNAL
class MG_MAPGUIDE_API MgConnectionProperties : public MgSerializable
{
    DECLARE_CLASSNAME(MgConnectionProperties)

INTERNAL_API:
    /// \brief
    /// Constructor for HTTP connection properties
    ///
    /// \param userInfo
    /// User information
    /// \param url
    /// URL for HTTP connection
    ///
    /// \exception MgInvalidArgumentException
    ///  if URL is empty
    ///
    MgConnectionProperties(MgUserInformation* userInfo, CREFSTRING url);

    /// \brief
    /// Constructor for TCP/IP connection properties
    ///
    /// \param userInfo
    /// User information
    /// \param target
    /// Target machine's IP address
    /// \param port
    /// Target machine's port number
    ///
    MgConnectionProperties(MgUserInformation* userInfo, CREFSTRING target, INT32 port);

    /// \brief
    /// Destructor
    ///
    ~MgConnectionProperties();

    /// \brief
    /// Determines whether host is local
    ///
    /// \return
    /// true if local
    ///
    bool IsLocal();

    /// \brief
    /// Gets user information for connection
    ///
    MgUserInformation* GetUserInfo();

    /// \brief
    /// Returns the URL for HTTP connection
    ///
    /// \return
    /// returns URL string
    ///
    STRING GetUrl();

    /// \brief
    /// Returns the target for TCP/IP connection
    ///
    /// \return
    /// returns target string
    ///
    STRING GetTarget();

    /// \brief
    /// Returns the port for TCP/IP connection
    ///
    /// \return
    /// returns port#
    ///
    INT32 GetPort();

    /// \brief
    /// Compares data members of this object and the object supplied
    ///
    /// \param connProp
    /// Connection properties object to be compared
    ///
    /// \return
    /// true - if properties match
    /// false - if properties do not match
    ///
    bool Equals(MgConnectionProperties* connProp);

    /// \brief
    /// Creates a hash key for the connection props
    ///
    /// \return
    /// string based hash
    ///
    wstring Hash();

    /// \brief
    /// Serializes data over TCP/IP connection
    ///
    void Serialize(MgStream* stream);

    /// \brief
    /// Deserializes data from TCP/IP stream
    ///
    void Deserialize(MgStream* stream);

protected:

    /// \brief
    /// Self destructor
    ///
    void Dispose();

    /// \brief
    /// Returns class id
    ///
    INT32 GetClassId();

private:

    /// \brief
    /// Default constructor
    ///
    MgConnectionProperties();

private:

    Ptr<MgUserInformation> m_userInfo;
    STRING m_url;
    STRING m_target;
    INT32 m_port;

CLASS_ID:
    static const INT32 m_cls_id = MapGuide_Service_ConnectionProperties;
};

#endif // MG_CONNECTION_PROPERTIES_H_

/// \endcond
