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

IGWSQualifiedNames * IGWSQualifiedNames::Create ()
{
    return CGwsQualifiedNames::CreateInstance<CGwsQualifiedNames>();

}

IGWSLongFeatureIdSet* IGWSLongFeatureIdSet::Create(long* cacheIds, long count)
{
    CGwsFeatureIdSet<long, IGWSLongFeatureIdSet>* pIdSet = new CGwsFeatureIdSet<long, IGWSLongFeatureIdSet>();

    for(long i=0;i<count;i++)
        pIdSet->Add(cacheIds[i]);

    FDO_SAFE_ADDREF(pIdSet);
    return pIdSet;
}

IGWSLongFeatureIdSet* IGWSLongFeatureIdSet::Create()
{
    CGwsFeatureIdSet<long, IGWSLongFeatureIdSet>* pIdSet = new CGwsFeatureIdSet<long, IGWSLongFeatureIdSet>();

    FDO_SAFE_ADDREF(pIdSet);
    return pIdSet;
}

IGWSFeatureIdSet* IGWSFeatureIdSet::Create()
{
    CGwsFeatureIdSet<GWSFeatureId, IGWSFeatureIdSet>* pIdSet = new CGwsFeatureIdSet<GWSFeatureId, IGWSFeatureIdSet>();

    FDO_SAFE_ADDREF(pIdSet);
    return pIdSet;
}
