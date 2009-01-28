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

#include "MapGuideCommon.h"
#include "IpUtil.h"

ACE_Recursive_Thread_Mutex MgIpUtil::sm_mutex;

///----------------------------------------------------------------------------
/// <summary>
/// Constructs the object.
/// </summary>
///----------------------------------------------------------------------------

MgIpUtil::MgIpUtil()
{
}

///----------------------------------------------------------------------------
/// <summary>
/// Destructs the object.
/// </summary>
///----------------------------------------------------------------------------

MgIpUtil::~MgIpUtil()
{
}

///----------------------------------------------------------------------------
/// <summary>
/// Detemines whether or not the specified host is an IP address.
/// </summary>
///
/// <param name="address">
/// IP address (or host name).
/// </param>
/// </param>
/// <param name="strict">
/// Flag indicating if the specified address must be looked up.
/// </param>
///
/// EXCEPTIONS:
/// MgNullArgumentException
/// MgInvalidArgumentException
/// MgInvalidIpAddressException
///----------------------------------------------------------------------------

bool MgIpUtil::IsIpAddress(CREFSTRING address, bool strict)
{
    ValidateAddress(address, strict);

    unsigned int n1, n2, n3, n4;

    return (4 == ::swscanf(address.c_str(), L"%u.%u.%u.%u", &n1, &n2, &n3, &n4));
}

///----------------------------------------------------------------------------
/// <summary>
/// Detemines whether or not the specified host is local.
/// </summary>
///
/// <param name="address">
/// IP address (or host name).
/// </param>
/// <param name="strict">
/// Flag indicating if the specified address must be looked up.
/// </param>
///
/// EXCEPTIONS:
/// MgNullArgumentException
/// MgInvalidArgumentException
///----------------------------------------------------------------------------

bool MgIpUtil::IsLocalHost(CREFSTRING address, bool strict)
{
    bool isLocalHost = false;

    if (0 == ::wcscmp(address.c_str(), L"127.0.0.1")
     || 0 == _wcsnicmp(address.c_str(), L"localhost", ::wcslen(L"localhost")))
    {
        isLocalHost = true;
    }
    else if (strict)
    {
        // This is a workaround for the special case where the domain and the
        // DNS suffix are different, e.g.:
        //      name1 = calpc220.amer.ads.autodesk.com
        //      name2 = calpc220.ads.autodesk.com

        STRING address1, address2;

        HostNameToAddress(address, address1, strict);
        HostNameToAddress(L"localhost", address2, strict);

        if (0 == _wcsicmp(address1.c_str(), address2.c_str()))
        {
            isLocalHost = true;
        }
        else
        {
            STRING name1, name2;

            HostAddressToName(address, name1, strict);
            HostAddressToName(L"localhost", name2, strict);

            isLocalHost = (0 == _wcsnicmp(name1.c_str(), name2.c_str(),
                ACE_MIN(name1.length(), name2.length())));
        }
    }

    return isLocalHost;
}

///----------------------------------------------------------------------------
/// <summary>
/// Returns the fully qualified domain name of the local host.
/// </summary>
///
/// <returns>
/// The fully qualified domain name of the local host.
/// </returns>
///
/// EXCEPTIONS:
/// MgDomainException
///----------------------------------------------------------------------------

STRING MgIpUtil::GetLocalHostName()
{
    STRING localHostName;

#ifdef _WIN32

    // On Windows, use ACE_OS::gethostbyname because ACE_OS::hostname may
    // return the correct host name but not in a fully qualified form.

    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex, STRING(L"")));
    // This is not thread safe - requires guard.
    const struct hostent* hostEnt = ACE_OS::gethostbyname("localhost");

    if (NULL != hostEnt && NULL != hostEnt->h_name)
    {
        MgUtil::MultiByteToWideChar(hostEnt->h_name, localHostName);
    }

#else

    // On Linux, use ACE_OS::hostname because ACE_OS::gethostbyname may
    // just return "localhost".

    char buf[MAXHOSTNAMELEN + 1];

    ::memset(buf, 0, sizeof buf);

    if (0 == ACE_OS::hostname(buf, sizeof buf))
    {
        MgUtil::MultiByteToWideChar(buf, localHostName);
    }

#endif

    if (localHostName.empty())
    {
        throw new MgDomainException(L"MgIpUtil.GetLocalHostName",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    return localHostName;
}

///----------------------------------------------------------------------------
/// <summary>
/// Returns the IP address of the local host.
/// </summary>
///
/// <returns>
/// The IP address of the local host.
/// </returns>
///
/// EXCEPTIONS:
/// MgDomainException
///----------------------------------------------------------------------------

STRING MgIpUtil::GetLocalHostAddress()
{
    STRING localHostAddress;

    HostNameToAddress(L"localhost", localHostAddress);

    return localHostAddress;
}

///----------------------------------------------------------------------------
/// <summary>
/// Detemines whether or not the specified address is valid.
/// </summary>
///
/// <param name="address">
/// IP address (or host name).
/// </param>
/// <param name="strict">
/// Flag indicating if the specified address must be looked up.
/// </param>
///
/// EXCEPTIONS:
/// MgNullArgumentException
/// MgInvalidArgumentException
/// MgInvalidIpAddressException
///----------------------------------------------------------------------------

void MgIpUtil::ValidateAddress(CREFSTRING address, bool strict)
{
    if (address.empty())
    {
        throw new MgNullArgumentException(L"MgIpUtil.ValidateAddress",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    if (STRING::npos != address.rfind(L':'))
    {
        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(address);

        MgStringCollection whyArguments;
        whyArguments.Add(L":");

        throw new MgInvalidArgumentException(L"MgIpUtil.ValidateAddress",
            __LINE__, __WFILE__, &arguments,
            L"MgStringContainsReservedCharacters", &whyArguments);
    }

    if (strict)
    {
        ACE_INET_Addr inetAddr;

        if (0 != inetAddr.set((u_short)0, address.c_str()) ||
            NULL == inetAddr.get_host_name())
        {
            MgStringCollection arguments;
            arguments.Add(address);

            throw new MgInvalidIpAddressException(L"MgIpUtil.ValidateAddress",
                __LINE__, __WFILE__, &arguments, L"", NULL);
        }
    }
}

///----------------------------------------------------------------------------
/// <summary>
/// Converts a host name to an IP address.
/// </summary>
///
/// <param name="name">
/// Input host name (or IP address).
/// </param>
/// <param name="address">
/// Output IP address.
/// </param>
/// <param name="strict">
/// When the input host name is invalid or unreachable, if this value is
/// false, then the output IP address is set to the input host name.
/// Otherwise, an exception will be thrown.
/// </param>
///
/// <returns>
/// true if successful, false otherwise.
/// </returns>
///
/// EXCEPTIONS:
/// MgNullArgumentException
/// MgInvalidArgumentException
/// MgInvalidIpAddressException
///----------------------------------------------------------------------------

bool MgIpUtil::HostNameToAddress(CREFSTRING name, REFSTRING address,
    bool strict)
{
    // No conversion is needed if the input parameter already contains a host address.

    if (IsIpAddress(name, false))
    {
        ValidateAddress(name, !IsLocalHost(name, false));
        address = name;

        return true;
    }

    // Note that the following code still works even if the input parameter is
    // an IP address instead of a host name.
    // It converts "127.0.0.1", "localhost" or "localhost.localdomain" to
    // the actual IP address if DNS entries are set up correctly.

    ACE_INET_Addr inetAddr;

    address.clear();

    if (0 == inetAddr.set((u_short)0, name.c_str()))
    {
        const char* hostName = inetAddr.get_host_name();

        if (NULL != hostName)
        {
            if (0 == _strnicmp(hostName, "localhost", ::strlen("localhost")))
            {
                address = GetLocalHostName();
            }
            else
            {
                MgUtil::MultiByteToWideChar(hostName, address);
            }

            if (0 == inetAddr.set((u_short)0, address.c_str()))
            {
                const char* hostAddress = inetAddr.get_host_addr();

                if (NULL != hostAddress)
                {
                    MgUtil::MultiByteToWideChar(hostAddress, address);
                }
            }
        }
    }

    bool valid = !address.empty();

    if (!valid)
    {
        if (strict)
        {
            MgStringCollection arguments;
            arguments.Add(name);

            throw new MgInvalidIpAddressException(L"MgIpUtil.HostNameToAddress",
                __LINE__, __WFILE__, &arguments, L"", NULL);
        }
        else
        {
            address = name;
        }
    }

    return valid;
}

///----------------------------------------------------------------------------
/// <summary>
/// Converts an IP address to a host name.
/// </summary>
///
/// <param name="address">
/// Input IP address (or host name).
/// </param>
/// <param name="name">
/// Output host name.
/// </param>
/// <param name="strict">
/// When the input IP address is invalid or unreachable, if this value is
/// false, then the output host name is set to the input IP address.
/// Otherwise, an exception will be thrown.
/// </param>
///
/// <returns>
/// true if successful, false otherwise.
/// </returns>
///
/// EXCEPTIONS:
/// MgNullArgumentException
/// MgInvalidArgumentException
/// MgInvalidIpAddressException
///----------------------------------------------------------------------------

bool MgIpUtil::HostAddressToName(CREFSTRING address, REFSTRING name,
    bool strict)
{
    // No conversion is needed if the input parameter already contains a host name.

    if (!IsIpAddress(address, false))
    {
        ValidateAddress(address, !IsLocalHost(address, false));
        name = address;

        return true;
    }

    // Note that the following code still works even if the input parameter is
    // a host name instead of an IP address.
    // It converts "127.0.0.1", "localhost" or "localhost.localdomain" to
    // the actual host name if DNS entries are set up correctly.

    ACE_INET_Addr inetAddr;

    name.clear();

    if (0 == inetAddr.set((u_short)0, address.c_str()))
    {
        const char* hostName = inetAddr.get_host_name();

        if (NULL != hostName)
        {
            if (0 == _strnicmp(hostName, "localhost", ::strlen("localhost")))
            {
                name = GetLocalHostName();
            }
            else // if (0 != _stricmp(hostName, "<unknown>"))
            {
                MgUtil::MultiByteToWideChar(hostName, name);
            }
        }
    }

    bool valid = !name.empty();

    if (!valid)
    {
        if (strict)
        {
            MgStringCollection arguments;
            arguments.Add(address);

            throw new MgInvalidIpAddressException(L"MgIpUtil.HostAddressToName",
                __LINE__, __WFILE__, &arguments, L"", NULL);
        }
        else
        {
            name = address;
        }
    }

    return valid;
}

///----------------------------------------------------------------------------
/// <summary>
/// Compares two IP addresses (or host names) for equality.
/// </summary>
///
/// <returns>
/// < 0, address1 is less than address2.
/// = 0, address1 is equal or equivalvent to address2.
/// > 0, address1 is greater than address2.
/// </returns>
///
/// EXCEPTIONS:
/// MgNullArgumentException
/// MgInvalidArgumentException
/// MgInvalidIpAddressException
///----------------------------------------------------------------------------

int MgIpUtil::CompareAddresses(CREFSTRING address1, CREFSTRING address2)
{
    int compareFlag = _wcsicmp(address1.c_str(), address2.c_str());

    if (0 == compareFlag)
    {
        ValidateAddress(address1, !IsLocalHost(address1, false));
        ValidateAddress(address2, !IsLocalHost(address2, false));
    }
    else if (IsIpAddress(address1, false) && IsIpAddress(address2, false) &&
            !IsLocalHost(address1, false) && !IsLocalHost(address2, false))
    {
        ValidateAddress(address1);
        ValidateAddress(address2);
    }
    else
    {
        ACE_INET_Addr inetAddr1((u_short)0, address1.c_str());
        ACE_INET_Addr inetAddr2((u_short)0, address2.c_str());

        if (inetAddr1 == inetAddr2)
        {
            // This is a workaround for the problem where the ACE_INET_Addr
            // equality test above returns true when two different invalid
            // addresses are specified.

            STRING name1, name2;

            if (HostAddressToName(address1, name1) ||
                HostAddressToName(address2, name2))
            {
                compareFlag = 0;
            }
            else
            {
                compareFlag = _wcsicmp(name1.c_str(), name2.c_str());
            }
        }
        else
        {
            // This is a workaround for the problem where the ACE_INET_Addr
            // equality test above returns false when address #1 is the fully
            // qualified domain name of the local host and address #2 is the real
            // IP address of the local host, and vice versa.

            if (IsLocalHost(address1) && IsLocalHost(address2))
            {
                compareFlag = 0;
            }
            else if (inetAddr1 < inetAddr2)
            {
                compareFlag = -1;
            }
            else
            {
                compareFlag = 1;
            }
        }
    }

    return compareFlag;
}
