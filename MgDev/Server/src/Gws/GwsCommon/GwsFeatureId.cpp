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

/////////////////////////////////////////////////////////////////////
//
// Includes
//
/////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#include "GwsCommonImp.h"


GWSFeatureId::GWSFeatureId()
{
    m_values = NULL;
}

GWSFeatureId::GWSFeatureId(const GWSFeatureId & other)
{
    * this = other;
}

GWSFeatureId::GWSFeatureId(FdoDataValueCollection * vals)
{
    m_values = vals;
    if (vals != NULL)
        vals->AddRef ();
}

GWSFeatureId::GWSFeatureId(int id)
{
    m_values = CreateDataValueCollection ();
    FdoPtr<FdoDataValue> idval = FdoDataValue::Create (id);
    m_values->Add (idval);
}


GWSFeatureId::~GWSFeatureId()
{
}

FdoDataValueCollection * GWSFeatureId::CreateDataValueCollection()
{
    FdoDataValueCollection * coll = CGwsDataValueCollection::Create ();
    return coll;
}

FdoDataValue * GWSFeatureId::GetItem(FdoInt32 idx) const
{
    if (m_values == NULL)
        return NULL;
    return m_values->GetItem (idx);
}

FdoInt32 GWSFeatureId::GetCount() const
{
    if (m_values == NULL)
        return 0;
    return m_values->GetCount ();
}

GWSFeatureId & GWSFeatureId::operator=(const GWSFeatureId& other)
{
    m_values = other.m_values;
    return *this;
}

GWSFeatureId & GWSFeatureId::operator=(FdoDataValueCollection * vals)
{
    m_values = vals;
    if (vals != NULL)
        vals->AddRef ();
    return *this;
}


bool GWSFeatureId::operator<(const GWSFeatureId& other) const
{
    return Compare (other) < 0;
}

bool GWSFeatureId::operator==(const GWSFeatureId& other) const
{
    return Compare (other) == 0;
}

int GWSFeatureId::Compare(const GWSFeatureId& other) const
{
    int iCount1;
    int iCount2;
    if ((iCount1 = GetCount ()) < (iCount2 = other.GetCount ()))
        return -1;
    if (iCount1 > iCount2)
        return 1;

    int res = 0;
    for (int i = 0; res == 0 && i < iCount1; i ++) {
        FdoPtr<FdoDataValue> val1 = GetItem (i);
        FdoPtr<FdoDataValue> val2 = other.GetItem (i);
        res = GWSFdoUtilities::CompareDataValues (val1, val2);

    }
    return res;
}

bool GWSFeatureId::IsNull() const
{
    for (int i = 0; i < GetCount (); i ++) {
        FdoPtr<FdoDataValue> val1 = GetItem (i);
        if (val1 == NULL || val1->IsNull ()) {
            return true;
        }
    }
    return false;
}


int GWSFeatureId::ToString(wchar_t * str, int numchars) const
{
    WSTR wstr;
    for (int i = 0; i < GetCount (); i ++) {
        FdoPtr<FdoDataValue> val1 = GetItem (i);
        if (i > 0)
            wstr.append (L",");
        if (val1 == NULL || val1->IsNull ()) {
            wstr.append (L"NULL");
        } else {
            wstr.append (val1->ToString ());
        }
    }
    int len = gws_min (numchars - 1, (int)wcslen (wstr.c_str ()));
    wcsncpy (str, wstr.c_str (), len);
    *(str + len) = 0;
    return len;
}
