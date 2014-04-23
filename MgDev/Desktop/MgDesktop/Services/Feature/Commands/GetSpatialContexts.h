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

#ifndef MG_SERVERGETSPATIALCONTEXTS_H_
#define MG_SERVERGETSPATIALCONTEXTS_H_

#include "MgDesktop.h"
#include "System/XmlDefs.h"
#include "System/XmlUtil.h"
#include "Fdo.h"

class MgdFeatureServiceCache;

class MgdGetSpatialContexts
{
public:
    MgdGetSpatialContexts();
    ~MgdGetSpatialContexts();
    MgSpatialContextReader* GetSpatialContexts(MgResourceIdentifier* resId);

private:
    MgSpatialContextData* GetSpatialContextData(FdoISpatialContextReader* spatialReader,
        MgSpatialContextInfo* spatialContextInfo);

    // ==========================================================================
    // ===================== EPSG RELATED UTILITY FUNCTIONS =====================
    // ==========================================================================
    // Checks whether or not the coordinate system name represents an EPSG code.
    // If this is the case the routine returns TRUE, FALSE otherwise.
    bool IsEpsgCodeRepresentation (FdoString *coordSysName);

    STRING m_providerName;
    MgdFeatureServiceCache* m_featureServiceCache;
};

#endif
