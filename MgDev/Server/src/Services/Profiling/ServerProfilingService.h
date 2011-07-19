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

#ifndef MGSERVERPROFILINGSERVICE_H
#define MGSERVERPROFILINGSERVICE_H

#include "ServerProfilingDllExport.h"

class MG_SERVER_PROFILING_API MgServerProfilingService : public MgProfilingService
{
    DECLARE_CLASSNAME(MgServerProfilingService)

public:
    MgServerProfilingService();
    ~MgServerProfilingService();

    DECLARE_CREATE_SERVICE()

    void SetConnectionProperties(MgConnectionProperties* connProp);

    virtual MgByteReader* ProfileRenderDynamicOverlay(MgMap* map,
                                               MgSelection* selection,
                                               MgRenderingOptions* options);

    virtual MgByteReader* ProfileRenderMap(MgMap* map,
                                    MgSelection* selection,
                                    MgCoordinate* center,
                                    double scale,
                                    INT32 width,
                                    INT32 height,
                                    MgColor* backgroundColor,
                                    CREFSTRING format,
                                    bool bKeepSelection);

private:

    // member data
    Ptr<MgFeatureService> m_svcFeature;
    Ptr<MgResourceService> m_svcResource;
    Ptr<MgRenderingService> m_svcRendering;
};

#endif
