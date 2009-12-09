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

#ifndef MGSERVERADMINSERVICE_H_
#define MGSERVERADMINSERVICE_H_

#include "Services/ServerAdminDefs.h"
#include "ServerAdminDllExport.h"

class MgLoadBalanceManager;

class MG_SERVER_SERVERADMIN_API MgServerAdminService : public MgService
{
DECLARE_CLASSNAME(MgServerAdminService)

/// Constructors/Destructor

public:
    explicit MgServerAdminService();
    ~MgServerAdminService();
    DECLARE_CREATE_SERVICE()

/// Methods

public:
    MgPropertyCollection* GetConfigurationProperties(CREFSTRING propertySection);
    void SetConfigurationProperties(CREFSTRING propertySection, MgPropertyCollection* properties);
    void RemoveConfigurationProperties(CREFSTRING propertySection, MgPropertyCollection* properties);

    void BringOnline();
    void TakeOffline();
    bool IsOnline();

    bool ClearLog(CREFSTRING log);
    void DeleteLog(CREFSTRING fileName);
    void RenameLog(CREFSTRING oldFileName, CREFSTRING newFileName);
    MgByteReader* GetLog(CREFSTRING log);
    MgByteReader* GetLog(CREFSTRING log, INT32 numEntries);
    MgByteReader* GetLog(CREFSTRING log, MgDateTime* fromDate, MgDateTime* toDate);
    MgPropertyCollection* EnumerateLogs();
    MgByteReader* GetLogFile(CREFSTRING logFile);
    void SetMaximumLogSize(INT32 size);
    void SetLogDelimiter(CREFSTRING delimiter);
    void EnableMaximumLogSize(bool useMaxSize);
    bool IsMaximumLogSizeEnabled();

    MgByteReader* GetDocument(CREFSTRING identifier);
    void SetDocument(CREFSTRING identifier, MgByteReader* data);

    MgPropertyCollection* GetInformationProperties();
    MgPropertyCollection* GetSiteStatus();
    STRING GetSiteVersion();

    // Service Management Methods

    MgSerializableCollection* RegisterServicesOnServers(
        MgSerializableCollection* serverInfoList);
    void UnregisterServicesOnServers(
        MgSerializableCollection* serverInfoList);

    // Notification Methods

    void NotifyResourcesChanged(MgSerializableCollection* resources);

    // Resource Package Management Methods

    MgStringCollection* EnumeratePackages();
    void LoadPackage(CREFSTRING packageName);
    void MakePackage(MgResourceIdentifier* resource, CREFSTRING packageName,
        CREFSTRING packageDescription);
    void DeletePackage(CREFSTRING packageName);
    MgPackageStatusInformation* GetPackageStatus(CREFSTRING packageName);
    MgByteReader* GetPackageLog(CREFSTRING packageName);

    // Internal use methods
    void SetConnectionProperties(MgConnectionProperties* connProp);

protected:
    //  Inherited from MgDisposable
    virtual void Dispose();

private:
    INT32 ComputeWIN32CpuLoad();
    INT32 ComputeLinuxCpuLoad();

    MgLoadBalanceManager* m_loadBalanceMan;
};

#endif
