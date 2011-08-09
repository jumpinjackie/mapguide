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
/// Determines whether or not the specified host is an IP address.
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
/// MgInvalidIpAddressException
///----------------------------------------------------------------------------

bool MgIpUtil::IsIpAddress(CREFSTRING address, bool strict)
{
    return IsIpv4Address(address, strict) || IsIpv6Address(address, strict);
}

///----------------------------------------------------------------------------
/// <summary>
/// Determines whether or not the specified host is an IP address in IPv4 format.
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
/// MgInvalidIpAddressException
///----------------------------------------------------------------------------

bool MgIpUtil::IsIpv4Address(CREFSTRING address, bool strict)
{
    if (address.empty())
    {
        throw new MgNullArgumentException(L"MgIpUtil.IsIpv4Address",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    unsigned int n1, n2, n3, n4;
    bool isv4Address = 4 == ::swscanf(address.c_str(), L"%u.%u.%u.%u", &n1, &n2, &n3, &n4);

    if (isv4Address && strict)
    {
        ACE_INET_Addr inetAddr;

        if (0 != inetAddr.set((u_short)0, address.c_str()) ||
            NULL == inetAddr.get_host_name())
        {
            MgStringCollection arguments;
            arguments.Add(address);

            throw new MgInvalidIpAddressException(L"MgIpUtil.IsIpv4Address",
                __LINE__, __WFILE__, &arguments, L"", NULL);
        }

        isv4Address = AF_INET == inetAddr.get_type();
    }

    return isv4Address;
}

///----------------------------------------------------------------------------
/// <summary>
/// Determines whether or not the specified host is an IP address in IPv6 format.
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
/// MgInvalidIpAddressException
///----------------------------------------------------------------------------

bool MgIpUtil::IsIpv6Address(CREFSTRING address, bool strict)
{
    if (address.empty())
    {
        throw new MgNullArgumentException(L"MgIpUtil.IsIpv6Address",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // An valid IPv6 address must have ":"
    bool isv6Address = address.find(L":") != -1;

    if (isv6Address)
    {
        // Check if the address is expressed
        int expressed = (int) address.find(L"::");
        if (address.rfind(L"::") != expressed)
        {
            // There are more than 1 "::", it's an invalid address
            return false;
        }
        expressed = expressed == -1 ? 0 : 1;

        int conlonNumber = 0;
        const wchar_t *addr = address.c_str();
        int len = (int) wcslen(addr);

        if (addr[len] == L':')
        {
            // If the last char is :, it's an invalid address
            return false;
        }

        if (addr[0] == L':' && addr[1] != L':')
        {
            // Single ":" cannot be the leading character for an valid IPv6 address
            return false;
        }

        // Check how many ":" the address has
        for (int i = 0; i < len; ++i)
        {
            if (addr[i] == L':')
            {
                ++conlonNumber;
            }
        }
        
        if (conlonNumber > 7 || conlonNumber < 1)
        {
            // If there are more than 8 groups or less than 1 group, it's an invalid address
            return false;
        }

        int totalGroups = 8;
        // Get how many groups are expressed as ::
        // It's possible that "::" is the leading character, then 1 more group is expressed
        int compressedGroups = totalGroups - conlonNumber + (addr[0] == L':' ? 1 : 0);

        wchar_t fullAddress[40] = {0};
        wcscpy(fullAddress, L"0000:0000:0000:0000:0000:0000:0000:0000");

        int digits = 0;
        int index = (int) wcslen(fullAddress) - 1;
        wchar_t lastChar = L' ';
        for (int i = len - 1; i >= 0 && index >= 0; --i, --index)
        {
            if (addr[i] == L':')
            {
                if (lastChar == L':')
                {
                    // Restore the compressed groups
                    index -= compressedGroups * 5 - 1;
                }
                else
                {
                    // Restore the addtional leading 0s
                    index -= 4 - digits;
                }

                // A new group will start, then reset the digits counter
                digits = 0;
            }
            else
            {
                fullAddress[index] = addr[i];

                // The max number of digits in a group is 4
                if (++digits > 4)
                {
                    return false;
                }
            }

            lastChar = addr[i];
        }

        // Check if the digits in each group are valid hex numbers
        int n1, n2, n3, n4, n5, n6, n7, n8;
        isv6Address = 8 == ::swscanf(fullAddress, L"%4X:%4X:%4X:%4X:%4X:%4X:%4X:%4X:", &n1, &n2, &n3, &n4, &n5, &n6, &n7, &n8);

        if (isv6Address & strict)
        {
            ACE_INET_Addr inetAddr;

            if (0 != inetAddr.set((u_short)0, address.c_str()) ||
                NULL == inetAddr.get_host_name())
            {
                MgStringCollection arguments;
                arguments.Add(address);

                throw new MgInvalidIpAddressException(L"MgIpUtil.IsIpv6Address",
                    __LINE__, __WFILE__, &arguments, L"", NULL);
            }

            isv6Address = AF_INET6 == inetAddr.get_type();
        }
    }

    return isv6Address;
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
        || 0 == ::wcscmp(address.c_str(), L"::1")
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
/// MgInvalidIpAddressException
///----------------------------------------------------------------------------

void MgIpUtil::ValidateAddress(CREFSTRING address, bool strict)
{
    if (address.empty())
    {
        throw new MgNullArgumentException(L"MgIpUtil.ValidateAddress",
            __LINE__, __WFILE__, NULL, L"", NULL);
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
            // If one of the addresses is IPv6 and the other one is IPv4, then even the two addresses 
            // are for a same host, inetAddr1 == inetAddr2 is still false.
            // And the case of both addresses are IPv6 and for a same host are already handled before
            else if (inetAddr1.get_type() == AF_INET6 || inetAddr2.get_type() == AF_INET6)
            {
                // Convert them to addresses first then convert them to names.
                // This is a workaround for the special case where the domain and the
                // DNS suffix are different, e.g.:
                //      name1 = calpc220.amer.ads.autodesk.com
                //      name2 = calpc220.ads.autodesk.com
                STRING addr1, addr2;
                if (HostNameToAddress(address1, addr1) &&
                    HostNameToAddress(address2, addr2))
                {
                    STRING name1, name2;

                    if (HostAddressToName(addr1, name1) &&
                        HostAddressToName(addr2, name2))
                    {
                        compareFlag = _wcsnicmp(name1.c_str(), name2.c_str(),
                                                ACE_MIN(name1.length(), name2.length()));
                    }
                    // else, MgInvalidIpAddressException will be thrown
                }
                // else, MgInvalidIpAddressException will be thrown
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
