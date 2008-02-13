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

#include "OgcFramework.h"
#include "NameStringValueCollection.h"

/////////////////////////////////////////////////////////////////////////
// This operator is just syntactic sugar, offering associative-array
// semantics to this class.
//
CPSZ MgUtilNameStringValueCollection::operator [](CPSZ pszCandidate)
{
    int i = FindName(pszCandidate);
    return i == -1? NULL : (CPSZ)Value(i);
}

/////////////////////////////////////////////////////////////////////////
// Case-insensitive comparisons are offered.
//
bool MgUtilNameStringValueCollection::CompareSame(CPSZ pszName,CPSZ pszCandidate)
{
    return szicmp(pszName,pszCandidate) == 0;
}
