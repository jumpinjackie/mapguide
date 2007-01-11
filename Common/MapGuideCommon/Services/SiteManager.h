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
#include "MapGuideCommon.h"

#ifndef MG_SITEMANAGER_H_
#define MG_SITEMANAGER_H_

class MgSiteInfo;
typedef std::vector<MgSiteInfo*> MgSiteVector;

class MG_MAPGUIDE_API MgSiteManager : public MgGuardDisposable
{
DECLARE_CLASSNAME(MgSiteManager)

public:

/// Constructors/Destructor

    virtual ~MgSiteManager();

private:

    MgSiteManager();

/// Methods

public:

    virtual void Dispose();
    static MgSiteManager* GetInstance();

    MgConnectionProperties* GetConnectionProperties(MgUserInformation* userInfo, 
        MgSiteInfo* siteInfo,
        MgSiteInfo::MgPortType type);

    MgConnectionProperties* GetConnectionProperties(MgUserInformation* userInfo, 
        MgSiteInfo::MgPortType type,
        bool useSessionIP);

    MgConnectionProperties* GetSupportServerConnectionProperties(CREFSTRING supportServer,
        MgUserInformation* userInfo, 
        MgSiteInfo::MgPortType type);

    MgSiteInfo* GetSiteInfo(INT32 index);
    MgSiteInfo* GetSiteInfo(STRING target, INT32 port);
    MgSiteInfo* GetNextSite();
    INT32 GetSiteCount();

private:
    void Initialize();
    void ClearSiteInfo();

/// Data Members
 
private:
    MgConfiguration* m_configuration;
    static Ptr<MgSiteManager> sm_siteManager;
    static ACE_Recursive_Thread_Mutex sm_siteManagerMutex;

    UINT16 m_index;
    MgSiteVector m_sites;
};

#endif // MG_SITEMANAGER_H_
