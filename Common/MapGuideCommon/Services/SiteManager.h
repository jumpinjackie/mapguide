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

#ifndef MG_SITEMANAGER_H_
#define MG_SITEMANAGER_H_

class MgSiteInfo;
typedef std::vector<MgSiteInfo*> MgSiteVector;

// Data structure which is passed to thread
struct SMThreadData
{
    ACE_thread_t threadId;
    INT32 failoverRetryTime;
};

class MG_MAPGUIDE_API MgSiteManager : public MgGuardDisposable
{
DECLARE_CLASSNAME(MgSiteManager)

public:

/// Constructors/Destructor

    virtual ~MgSiteManager();

private:

    // Unimplemented copy constructor and assignment operator.

    MgSiteManager();
    MgSiteManager(const MgSiteManager&);
    MgSiteManager& operator=(const MgSiteManager&);

/// Methods

public:

    virtual void Dispose();
    static MgSiteManager* GetInstance();

    MgConnectionProperties* GetConnectionProperties(
        MgUserInformation* userInfo,
        MgSiteInfo* siteInfo,
        MgSiteInfo::MgPortType portType);

    MgConnectionProperties* GetConnectionProperties(
        MgUserInformation* userInfo,
        MgSiteInfo::MgPortType portType,
        bool useSessionIp);

    MgConnectionProperties* GetSupportServerConnectionProperties(
        CREFSTRING supportServer,
        MgUserInformation* userInfo,
        MgSiteInfo::MgPortType portType);

    MgSiteInfo* GetSiteInfo(INT32 index);
    MgSiteInfo* GetSiteInfo(CREFSTRING target, INT32 port);
    MgSiteInfo* GetSiteInfo(CREFSTRING hexString);

    INT32 GetSiteCount();

    MgSiteVector* GetSites();
    ACE_Recursive_Thread_Mutex m_mutex;

    bool GetCheckServersThreadDone();
    void SetCheckServersThreadDone(bool bDone);
    void StartCheckServersThread();
    void StopCheckServersThread();

private:

    void Initialize();
    void ClearSiteInfo();

    MgSiteInfo* GetNextSite();

/// Data Members

private:

    static Ptr<MgSiteManager> sm_siteManager;

    INT32 m_index;
    MgSiteVector m_sites;
    SMThreadData m_threadData;
    bool m_bCheckServersThreadDone;
};

#endif // MG_SITEMANAGER_H_
