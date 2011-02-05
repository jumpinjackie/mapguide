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

//////////////////////////////////////////////////////////////////
/// <summary>
/// Constructor. Initialize string property collection.
/// </summary>
MgHttpHeader::MgHttpHeader()
{
    // Nothing here yet
}

/// <summary>
/// Adds the header name and value to the collection.
/// header name  -    case-insensitive
/// header value -    case-insensitive
/// </summary>
/// <param name="name">Input
/// Name of the header
/// Two header can have same name.
/// </param>
/// <param name="value">Input
/// Value corresponding to the header.
/// NULL value and empty string is NOT allowed.
/// </param>
/// <returns>
/// TRUE  -  Header name is successfully added.
/// FALSE -  Header could not be added.
/// Possible cause - header value is NULL.
/// </returns>
bool MgHttpHeader::AddHeader(CREFSTRING name, CREFSTRING value)
{
    if (value.length()!=0 && !m_headerCollection.Contains(name))
    {
        m_headerCollection.Add(name, value);
        return true;
    }

    return false;
}

/// <summary>
/// Removes the header name and value from the collection.
/// header name - case-insensitive
/// </summary>
/// <param name="name">Input
/// Name of the header to be removed.
/// </param>
/// <returns>
/// TRUE  -  header name is successfully removed.
/// FALSE -  header name could not be removed.
/// Possible cause is header name does not exist.
/// </returns>
bool MgHttpHeader::RemoveHeader(CREFSTRING name)
{
    if (m_headerCollection.Contains(name))
    {
        m_headerCollection.Remove(name);
        return true;
    }

    return false;
}

/// <summary>
/// Retrieve the value for the specified header.
/// header name  - case-insensitive
/// </summary>
/// <param name="name">Input
/// Name of the header for which value to be retrieved.
/// </param>
/// <returns>
/// Value for the specified header or empty string if header does not exist.
/// </returns>
STRING MgHttpHeader::GetHeaderValue(CREFSTRING name)
{
    if (m_headerCollection.Contains(name))
    {
        return m_headerCollection.GetValue(name);
    }

    return L"";
}

/// <summary>
/// Update the value for the specified header.
/// header name  - case-insensitive
/// header value - case-insensitive
/// </summary>
/// <param name="name">Input
/// Name of the header to be updated.
/// </param>
/// <param name="value">Input
/// Value corresponding to the header.
/// NULL value and empty string is NOT allowed.
/// </param>
/// <returns>
/// TRUE  -  header is successfully updated.
/// FALSE -  header could not be updated.
/// Possible cause is header does not exist or value is NULL.
/// </returns>
bool MgHttpHeader::SetHeaderValue(CREFSTRING name, CREFSTRING value)
{
    if (m_headerCollection.Contains(name))
    {
        m_headerCollection.SetValue(name, value);
        return true;
    }

    return false;
}

/// <summary>
/// Returns the value of the specified header as a long value that represents a Date object.
/// Use this method with headers that contain dates, such as If-Modified-Since.
/// The date is returned as the number of milliseconds since January 1, 1970 GMT.
/// </summary>
/// <param name="name">Input
/// Name of the header (case insensitive).
/// </param>
/// <returns>
/// long - The date is returned as the number of milliseconds since January 1, 1970 GMT
/// S_MISSING_HEADER -   request did not have a header of the specified name
/// S_CANNOT_CONVERT -   header can't be converted to a date
/// </returns>
long MgHttpHeader::GetDateHeader(CREFSTRING name)
{
    // TODO: Figure out which Date manipulation functions to use
    if (m_headerCollection.Contains(name))
    {
        return 1;
    }
    else
        return 2;
}

/// <summary>
/// Returns the value of the specified request header as an int.
/// </summary>
/// <param name="name">Input
/// Name of the header (case-insensitive).
/// </param>
/// <returns>
/// INT32 - An integer value
/// S_MISSING_HEADER -   request did not have a header of the specified name
/// S_CANNOT_CONVERT -   header can't be converted to an integer.
/// </returns>
INT32 MgHttpHeader::GetIntHeader(CREFSTRING name)
{
    if (m_headerCollection.Contains(name))
    {
        string headerIntString = MgUtil::WideCharToMultiByte(m_headerCollection.GetValue(name));
        const char* headerIntCString = headerIntString.c_str();

        // Check for non-digits in string
        for (unsigned int i=0; i < strlen(headerIntCString); i++)
            if (!isdigit(headerIntCString[i]))
                return S_CANNOT_CONVERT;

        return atoi(headerIntCString);
    }
    else
        return S_MISSING_HEADER;
}

/// <summary>
/// Retrieve the list of all headers.
/// </summary>
/// <returns>
/// A string collection containing names of all headers.
/// </returns>
MgStringCollection* MgHttpHeader::GetHeaderNames()
{
    Ptr<MgStringCollection> mgsCollection;
    mgsCollection = new MgStringCollection();

    for (int i = 0; i < m_headerCollection.GetCount(); i++)
    {
        mgsCollection->Add(m_headerCollection.GetName(i));
    }

    return SAFE_ADDREF((MgStringCollection*)mgsCollection);
}

/// <summary>
/// Retrieve the list of all possible values for a header.
/// </summary>
/// <returns>
/// A string collection containing possible values of a header.
/// </returns>
MgStringCollection* MgHttpHeader::GetHeaders(CREFSTRING name)
{
    MG_UNUSED_ARG(name);

    // TODO: Return proper string collection here
    Ptr<MgStringCollection> mgsCollection;
    mgsCollection = new MgStringCollection();
    return SAFE_ADDREF((MgStringCollection*)mgsCollection);
}

void MgHttpHeader::Dispose()
{
    delete this;
}

INT32 MgHttpHeader::GetClassId()
{
    return m_cls_id;
}

MgHttpHeader::~MgHttpHeader()
{
}
