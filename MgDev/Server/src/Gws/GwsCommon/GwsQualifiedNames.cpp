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

/////////////////////////////////////////////////////////////////////
//
// Includes
//
/////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#include "GwsCommonImp.h"


CGwsQualifiedNames::CGwsQualifiedNames ()
{
}
CGwsQualifiedNames::~CGwsQualifiedNames () throw()
{

}
int CGwsQualifiedNames::IndexOf (const GWSQualifiedName & qname) const
{
    std::vector<GWSQualifiedName>::const_iterator iter;
    int idx = 0;
    for (iter = m_names.begin (); iter != m_names.end (); iter ++, idx ++) {
        if (* iter == qname) {
            return idx;
        }
    }
    return -1;
}
bool  CGwsQualifiedNames::Insert(const GWSQualifiedName & qname)
{
    if (IndexOf (qname) == -1) {
        m_names.push_back (qname);
        return true;
    }
    return false;
}

const GWSQualifiedName & CGwsQualifiedNames::Get (int idx) const
{
    static GWSQualifiedName name;
    if (idx < (int) m_names.size () && idx >= 0)
        return m_names [idx];
    return name;

}
bool CGwsQualifiedNames::Remove(const GWSQualifiedName & qname)
{
    std::vector<GWSQualifiedName>::iterator iter;
    for (iter = m_names.begin (); iter != m_names.end (); iter ++) {
        if (* iter == qname) {
            m_names.erase (iter);
            return true;
        }
    }
    return false;
}
bool CGwsQualifiedNames::Remove(int idx)
{
    std::vector<GWSQualifiedName>::iterator iter;
    for (iter = m_names.begin (); idx >= 0 && iter != m_names.end (); iter ++, idx --) {
        if (idx == 0) {
            m_names.erase (iter);
            return true;
        }
    }
    return false;
}

int CGwsQualifiedNames::Count() const
{
    return (int) m_names.size ();
}
void CGwsQualifiedNames::Clear()
{
    m_names.clear ();
}
