//
//  Copyright (C) 2004-2006  Autodesk, Inc.
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

#include "Common.h"
#include "Fdo.h"
#include "ServerFeatureServiceDefs.h"
#include "ServerFeatureReaderIdentifier.h"

MgServerFeatureReaderIdentifier::MgServerFeatureReaderIdentifier(FdoIFeatureReader* featureReader)
{
    m_featureReader = GIS_SAFE_ADDREF(featureReader);
}

MgServerFeatureReaderIdentifier::~MgServerFeatureReaderIdentifier()
{
    GIS_SAFE_RELEASE(m_featureReader);
}

FdoIFeatureReader* MgServerFeatureReaderIdentifier::GetFeatureReader()
{
    return GIS_SAFE_ADDREF(m_featureReader);
}
