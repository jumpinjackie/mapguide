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

#ifndef MGIPUTIL_H_
#define MGIPUTIL_H_

class MG_MAPGUIDE_API ACE_Recursive_Thread_Mutex;

/// \cond INTERNAL
class MG_MAPGUIDE_API MgIpUtil
{
    DECLARE_CLASSNAME(MgIpUtil)

INTERNAL_API:

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Constructs the object.
    ///

    MgIpUtil();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Destructs the object.
    ///

    virtual ~MgIpUtil();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Determines whether or not the specified host is an IP address.
    ///
    /// \param address
    /// IP address (or host name).
    /// \param strict
    /// Flag indicating if the specified address must be looked up.
    ///
    /// \exception MgNullArgumentException
    /// \exception MgInvalidIpAddressException
    ///
    ///

    static bool IsIpAddress(CREFSTRING address, bool strict = true);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Determines whether or not the specified host is an IP address in IPv4 format.
    ///
    /// \param address
    /// IP address (or host name).
    /// \param strict
    /// Flag indicating if the specified address must be looked up.
    /// \exception MgNullArgumentException
    /// \exception MgInvalidIpAddressException
    ///
    ///

    static bool IsIpv4Address(CREFSTRING address, bool strict = true);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Determines whether or not the specified host is an IP address in IPv6 format.
    ///
    /// \param address
    /// IP address (or host name).
    /// \param strict
    /// Flag indicating if the specified address must be looked up.
    /// \exception MgNullArgumentException
    /// \exception MgInvalidIpAddressException
    ///
    ///

    static bool IsIpv6Address(CREFSTRING address, bool strict = true);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Determines whether or not the specified host is local.
    ///
    /// \param address
    /// IP address (or host name).
    /// \param strict
    /// Flag indicating if the specified address must be looked up.
    ///
    /// \exception MgNullArgumentException
    /// \exception MgInvalidArgumentException
    ///
    ///

    static bool IsLocalHost(CREFSTRING address, bool strict = true);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the fully qualified domain name of the local host.
    ///
    /// \return
    /// The fully qualified domain name of the local host.
    ///
    /// \exception MgDomainException
    ///
    ///

    static STRING GetLocalHostName();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the IP address of the local host.
    ///
    /// \return
    /// The IP address of the local host.
    ///
    /// \exception MgDomainException
    ///
    ///

    static STRING GetLocalHostAddress();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Determines whether or not the specified address is valid.
    ///
    /// \param address
    /// IP address (or host name).
    /// \param strict
    /// Flag indicating if the specified address must be looked up.
    ///
    /// \exception MgNullArgumentException
    /// \exception MgInvalidArgumentException
    /// \exception MgInvalidIpAddressException
    ///
    ///

    static void ValidateAddress(CREFSTRING address, bool strict = true);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Converts a host name to an IP address.
    ///
    /// \param name
    /// Input host name (or IP address).
    /// \param address
    /// Output IP address.
    /// \param strict
    /// When the input host name is invalid or unreachable, if this value is
    /// false, then the output IP address is set to the input host name.
    /// Otherwise, an exception will be thrown.
    ///
    /// \return
    /// true if successful, false otherwise.
    ///
    /// \exception MgNullArgumentException
    /// \exception MgInvalidArgumentException
    /// \exception MgInvalidIpAddressException
    ///
    ///

    static bool HostNameToAddress(CREFSTRING name, REFSTRING address,
        bool strict = true);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Converts an IP address to a host name.
    ///
    /// \param address
    /// Input IP address (or host name).
    /// \param name
    /// Output host name.
    /// \param strict
    /// When the input IP address is invalid or unreachable, if this value is
    /// false, then the output host name is set to the input IP address.
    /// Otherwise, an exception will be thrown.
    ///
    /// \return
    /// true if successful, false otherwise.
    ///
    /// \exception MgNullArgumentException
    /// \exception MgInvalidArgumentException
    /// \exception MgInvalidIpAddressException
    ///
    ///

    static bool HostAddressToName(CREFSTRING address, REFSTRING name,
        bool strict = true);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Compares two IP addresses (or host names) for equality.
    ///
    /// \return
    /// < 0, address1 is less than address2.
    /// = 0, address1 is equal or equivalent to address2.
    /// > 0, address1 is greater than address2.
    ///
    /// \exception MgNullArgumentException
    /// \exception MgInvalidArgumentException
    /// \exception MgInvalidIpAddressException
    ///
    ///

    static int CompareAddresses(CREFSTRING address1, CREFSTRING address2);

private:

    /// Unimplemented Methods

    MgIpUtil(const MgIpUtil& util);
    MgIpUtil& operator=(const MgIpUtil& util);

    /// Helper Methods

private:

    static ACE_Recursive_Thread_Mutex sm_mutex;
};
/// \endcond

/// Inline Methods

#endif
