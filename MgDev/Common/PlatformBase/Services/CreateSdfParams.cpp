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

#include "PlatformBase.h"

#define SDF_PROVIDER_NAME L"OSGeo.SDF"

MG_IMPL_DYNCREATE(MgCreateSdfParams);


MgCreateSdfParams::MgCreateSdfParams()
:
MgFileFeatureSourceParams(SDF_PROVIDER_NAME)
{
}

MgCreateSdfParams::MgCreateSdfParams(CREFSTRING spatialContextName, CREFSTRING srsWkt, MgFeatureSchema* featureSchema)
:
MgFileFeatureSourceParams(SDF_PROVIDER_NAME, spatialContextName, srsWkt, featureSchema)
{
}

MgCreateSdfParams::~MgCreateSdfParams()
{
}

void MgCreateSdfParams::SetProviderName(CREFSTRING name)
{
    throw new MgInvalidOperationException(L"MgCreateSdfParams.SetProviderName",
        __LINE__, __WFILE__, NULL, L"", NULL);
}

INT32 MgCreateSdfParams::GetClassId()
{
    return m_cls_id;
}

void MgCreateSdfParams::Dispose()
{
    delete this;
}
