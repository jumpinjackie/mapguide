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

#ifndef _MG_SERVER_FEATURE_CONNECTION_H
#define _MG_SERVER_FEATURE_CONNECTION_H

#include "MapGuideCommon.h"
#include "Fdo.h"
#include "FdoConnectionManager.h"

class MgServerFeatureConnection : public MgGuardDisposable
{
public:

    MgServerFeatureConnection(MgResourceIdentifier* featureSourceIdentifier);
    MgServerFeatureConnection(CREFSTRING providerName, CREFSTRING connectionString);
    virtual ~MgServerFeatureConnection();

private:

    MgServerFeatureConnection();

public:

    virtual void Dispose();

    FdoIConnection* GetConnection();
    STRING GetProviderName();
    MgSpatialContextInfoMap* GetSpatialContextInfoMap();

    bool IsConnectionOpen();
    bool IsConnectionBusy();
    bool IsConnectionClosed();
    bool IsConnectionPending();

    bool SupportsCommand(INT32 commandType);

private:

    void Initialize();

    FdoIConnection* m_fdoConn;
    Ptr<MgResourceIdentifier> m_resourceId;
};

#endif
