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

/////////////////////////////////////////////////////////////////////
//
// Includes
//
/////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#include "GwsCommonImp.h"
#include <string>
/////////////////////////////////////////////////////////////////////
//
// class GWSQualifiedName
//
/////////////////////////////////////////////////////////////////////
GWSQualifiedName::GWSQualifiedName ()
{
}


GWSQualifiedName::GWSQualifiedName (
    FdoString * FeatureSource,
    FdoString * schema,
    FdoString * name
)
{
    if (FeatureSource != NULL)
        m_featuresource = FeatureSource;
    if (schema != NULL)
        m_schema = schema;
    if (name != NULL)
        m_name = name;
}

GWSQualifiedName::GWSQualifiedName (
    FdoString * schema,
    FdoString * name
)
{
    if (schema != NULL)
        m_schema = schema;
    if (name != NULL)
        m_name = name;
}

GWSQualifiedName::GWSQualifiedName (FdoString * name)
{
    if (name == NULL)
        return;

    std::wstring qualifiedName = name;
    size_t location = qualifiedName.find_last_of(L"@");
    if(location == std::wstring::npos)
    {
        location = qualifiedName.find_last_of(L":");
        if(location == std::wstring::npos)
        {
            //assume its just the classname
            m_name = name;
        }
        else
        {
            m_schema = qualifiedName.substr(0, location).c_str();
            m_name = qualifiedName.substr(location + 1).c_str();
        }
    }
    else
    {
        std::wstring scname = qualifiedName.substr(0, location).c_str();
        m_featuresource = qualifiedName.substr(location + 1).c_str();
        location = scname.find_last_of(L":");
        if(location == std::wstring::npos)
        {
            //assume its just the classname
            m_name = name;
        }
        else
        {
            m_schema = scname.substr(0, location).c_str();
            m_name = scname.substr(location + 1).c_str();
        }
    }
}

GWSQualifiedName::GWSQualifiedName (const GWSQualifiedName & other)
{
    * this = other;
}

GWSQualifiedName::~GWSQualifiedName ()
{
}

FdoString * GWSQualifiedName::FeatureSource () const
{
    return m_featuresource;
}

FdoString * GWSQualifiedName::Schema () const
{
    return m_schema;
}

FdoString * GWSQualifiedName::Name () const
{
    return m_name;
}

void GWSQualifiedName::operator=(const GWSQualifiedName & other)
{
    m_featuresource = other.m_featuresource;
    m_schema = other.m_schema;
    m_name = other.m_name;
}
bool GWSQualifiedName::operator==(const GWSQualifiedName & other) const
{
    if (_wcsicmp ((FdoString *)m_featuresource, (FdoString *) other.m_featuresource) == 0 &&
        m_schema == other.m_schema &&
        m_name == other.m_name)
        return true;
    return false;

}
bool GWSQualifiedName::operator>(const GWSQualifiedName & other) const
{
    int iCmp = _wcsicmp ((FdoString *)m_featuresource,
                        (FdoString *)other.m_featuresource);
    if (iCmp > 0)
        return true;
    if (iCmp < 0)
        return false;

    if (m_schema > other.m_schema)
        return true;
    if (m_schema < other.m_schema)
        return false;

    if (m_name > other.m_name)
        return true;

    return false;
}

bool GWSQualifiedName::operator<(const GWSQualifiedName & other) const
{
    int iCmp = _wcsicmp ((FdoString *) m_featuresource, (FdoString *) other.m_featuresource);

    if (iCmp < 0)
        return true;
    if (iCmp > 0)
        return false;

    if (m_schema < other.m_schema)
        return true;
    if (m_schema > other.m_schema)
        return false;

    if (m_name < other.m_name)
        return true;

    return false;
}

int GWSQualifiedName::ToString (wchar_t * res, int len) const
{
    size_t retlen = 0;
    if (res != NULL) {
        FdoStringP str;
        str = m_schema;
        str += L":";
        str += m_name;
        retlen = gws_min(str.GetLength (), (size_t)len);
        wcsncpy (res, str, retlen);
        if ((int)retlen < len)
            * (res + retlen)  = 0;
    }
    return (int)retlen;
}

int GWSQualifiedName::ToFullyQualifedString(wchar_t * res, int len) const
{
    size_t retlen = 0;
    if (res != NULL) {
        FdoStringP str;
        str = m_schema;
        str += L":";
        str += m_name;
        str += L"@";
        str += m_featuresource;
        retlen = gws_min(str.GetLength (), (size_t)len);
        wcsncpy (res, str, retlen);
        if ((int)retlen < len)
            * (res + retlen)  = 0;
    }
    return (int)retlen;
}
