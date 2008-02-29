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

GWSExtendedFeatureId::GWSExtendedFeatureId ()
{
}


GWSExtendedFeatureId::GWSExtendedFeatureId (
    const GWSQualifiedName & qname,
    const GWSFeatureId     & id
)
{
    m_qname = qname;
    m_fid   = id;
}

GWSExtendedFeatureId::GWSExtendedFeatureId (const GWSExtendedFeatureId & other)
{
    * this = other;
}

GWSExtendedFeatureId::~GWSExtendedFeatureId()
{
}


const GWSQualifiedName & GWSExtendedFeatureId::ClassName () const
{
    return m_qname;
}

const GWSFeatureId & GWSExtendedFeatureId::FeatureId () const
{
    return m_fid;
}



FdoInt32 GWSExtendedFeatureId::GetCount() const
{
    return m_fid.GetCount ();
}

FdoDataValue * GWSExtendedFeatureId::GetItem(FdoInt32 index) const
{
    return m_fid.GetItem (index);
}

// this function returns true if one name is less than the other,
// and returns false if this name is greater.  If the names are the
// same then it returns the relation between the fids.
// The comparison is case sensitive.
bool GWSExtendedFeatureId::operator< (const GWSExtendedFeatureId & other) const
{
    if (m_qname < other.m_qname)
        return true;
    if (m_qname == other.m_qname)
        return m_fid < other.m_fid;
    return false;
}

bool GWSExtendedFeatureId::operator== (const GWSExtendedFeatureId & other) const
{
     if (! (m_qname == other.m_qname))
        return false;

    return m_fid == other.m_fid;

}

void GWSExtendedFeatureId::operator= (const GWSExtendedFeatureId & other)
{
    m_qname = other.m_qname;
    m_fid   = other.m_fid;
}

int GWSExtendedFeatureId::ToString (wchar_t * str, int numchars) const
{
    int len;
    len = m_qname.ToString (str, numchars);
    if (len < numchars - 3) {
        wcscat (str, L"(");
        int len2 = m_fid.ToString (str + len + 1, numchars-len-3);
        len += len2;
        wcscat (str, L")");
        len += 3;
    }
    return len;
}
