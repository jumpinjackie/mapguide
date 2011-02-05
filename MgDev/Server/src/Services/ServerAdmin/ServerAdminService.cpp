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

#include "MapGuideCommon.h"
#include "ServerAdminService.h"
#include "PackageManager.h"
#include "ServerManager.h"
#include "LoadBalanceManager.h"
#include "LogManager.h"

IMPLEMENT_CREATE_SERVICE(MgServerAdminService)

//////////////////////////////////////////////////////////////////
/// <summary>
/// Constructor
/// </summary>
MgServerAdminService::MgServerAdminService() :
    MgService()
{
    m_loadBalanceMan = MgLoadBalanceManager::GetInstance();
    assert(NULL != m_loadBalanceMan);
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Dispose this object
/// </summary>
/// <returns>
/// Nothing
/// </returns>
void MgServerAdminService::Dispose()
{
    delete this;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Destructor
/// </summary>
MgServerAdminService::~MgServerAdminService()
{
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Allows the server to process client operations.
/// </summary>
/// <returns>
/// Nothing
/// </returns>
///
/// EXCEPTIONS:
/// MgConnectionNotOpenException
void MgServerAdminService::BringOnline()
{
    MG_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerAdminService::BringOnline()");

    MgServerManager::GetInstance()->BringOnline();

    MG_CATCH_AND_THROW(L"MgServerAdminService.BringOnline");
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Prevents the server from processing client operations.  When offline, the
/// adminstrator can access the server via "Admin" operations without worrying
/// about Mg clients using the server.
/// </summary>
/// <returns>
/// Nothing
/// </returns>
///
/// EXCEPTIONS:
/// MgConnectionNotOpenException
void MgServerAdminService::TakeOffline()
{
    MG_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerAdminService::TakeOffline()");

    MgServerManager::GetInstance()->TakeOffline();

    MG_CATCH_AND_THROW(L"MgServerAdminService.TakeOffline");
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the online status of the server.
/// </summary>
/// <returns>
/// True for online, False for offline.
/// </returns>
///
/// EXCEPTIONS:
/// MgConnectionNotOpenException
bool MgServerAdminService::IsOnline()
{
    bool bOnline = false;

    MG_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerAdminService::IsOnline()");

    bOnline = MgServerManager::GetInstance()->IsOnline();

    MG_CATCH_AND_THROW(L"MgServerAdminService.IsOnline");

    return bOnline;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Clears the specified log.
/// </summary>
bool MgServerAdminService::ClearLog(CREFSTRING log)
{
    bool bResult = false;

    MG_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerAdminService::ClearLog()");

    MgLogManager* pMan = MgLogManager::GetInstance();
    if (NULL == pMan)
    {
        throw new MgNullReferenceException(L"MgServerAdminService::ClearLog", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    if (log == MgLogFileType::Access)
    {
        bResult = pMan->ClearAccessLog();
    }
    else if (log == MgLogFileType::Admin)
    {
        bResult = pMan->ClearAdminLog();
    }
    else if (log == MgLogFileType::Authentication)
    {
        bResult = pMan->ClearAuthenticationLog();
    }
    else if (log == MgLogFileType::Error)
    {
        bResult = pMan->ClearErrorLog();
    }
    else if (log == MgLogFileType::Session)
    {
        bResult = pMan->ClearSessionLog();
    }
    else if (log == MgLogFileType::Trace)
    {
        bResult = pMan->ClearTraceLog();
    }
    else
    {
        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(log);

        throw new MgInvalidArgumentException(L"MgServerAdminService.ClearLog",
            __LINE__, __WFILE__, &arguments, L"MgInvalidLogType", NULL);
    }

    MG_CATCH_AND_THROW(L"MgServerAdminService.ClearLog")

    return bResult;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Deletes the file from the Logs folder
/// </summary>
void MgServerAdminService::DeleteLog(CREFSTRING fileName)
{
    MG_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerAdminService::DeleteLog()");

    MgLogManager* pMan = MgLogManager::GetInstance();
    pMan->DeleteLog(fileName);

    MG_CATCH_AND_THROW(L"MgServerAdminService.DeleteLog")
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Renames a log file.
/// </summary>
void MgServerAdminService::RenameLog(CREFSTRING oldFileName, CREFSTRING newFileName)
{
    MG_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerAdminService::RenameLog()");

    MgLogManager* pMan = MgLogManager::GetInstance();
    pMan->RenameLog(oldFileName, newFileName);

    MG_CATCH_AND_THROW(L"MgServerAdminService.RenameLog")
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets all of the contents of the specified log.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
MgByteReader* MgServerAdminService::GetLog(CREFSTRING log)
{
    Ptr<MgByteReader> byteReader;
    byteReader = NULL;

    MG_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerAdminService::GetLog()");

    MgLogManager* pMan = MgLogManager::GetInstance();
    if (NULL == pMan)
    {
        throw new MgNullReferenceException(L"MgServerAdminService::GetLog", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    if (log == MgLogFileType::Access)
    {
        byteReader = pMan->GetAccessLog();
    }
    else if (log == MgLogFileType::Admin)
    {
        byteReader = pMan->GetAdminLog();
    }
    else if (log == MgLogFileType::Authentication)
    {
        byteReader = pMan->GetAuthenticationLog();
    }
    else if (log == MgLogFileType::Error)
    {
        byteReader = pMan->GetErrorLog();
    }
    else if (log == MgLogFileType::Session)
    {
        byteReader = pMan->GetSessionLog();
    }
    else if (log == MgLogFileType::Trace)
    {
        byteReader = pMan->GetTraceLog();
    }
    else
    {
        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(log);

        throw new MgInvalidArgumentException(L"MgServerAdminService.GetLog",
            __LINE__, __WFILE__, &arguments, L"MgInvalidLogType", NULL);
    }

    MG_CATCH_AND_THROW(L"MgServerAdminService.GetLog")

    return byteReader.Detach();
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the last numEntries of the current access log.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
MgByteReader* MgServerAdminService::GetLog(CREFSTRING log, INT32 numEntries)
{
    Ptr<MgByteReader> byteReader;
    byteReader = NULL;

    MG_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerAdminService::GetLog()");

    MgLogManager* pMan = MgLogManager::GetInstance();
    if (NULL == pMan)
    {
        throw new MgNullReferenceException(L"MgServerAdminService::GetLog", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    if (log == MgLogFileType::Access)
    {
        byteReader = pMan->GetAccessLog(numEntries);
    }
    else if (log == MgLogFileType::Admin)
    {
        byteReader = pMan->GetAdminLog(numEntries);
    }
    else if (log == MgLogFileType::Authentication)
    {
        byteReader = pMan->GetAuthenticationLog(numEntries);
    }
    else if (log == MgLogFileType::Error)
    {
        byteReader = pMan->GetErrorLog(numEntries);
    }
    else if (log == MgLogFileType::Session)
    {
        byteReader = pMan->GetSessionLog(numEntries);
    }
    else if (log == MgLogFileType::Trace)
    {
        byteReader = pMan->GetTraceLog(numEntries);
    }
    else
    {
        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(log);

        throw new MgInvalidArgumentException(L"MgServerAdminService.GetLog",
            __LINE__, __WFILE__, &arguments, L"MgInvalidLogType", NULL);
    }

    MG_CATCH_AND_THROW(L"MgServerAdminService.GetLog")

    return byteReader.Detach();
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the contents of the specified log between the given dates.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
MgByteReader* MgServerAdminService::GetLog(CREFSTRING log, MgDateTime* fromDate, MgDateTime* toDate)
{
    Ptr<MgByteReader> byteReader;
    byteReader = NULL;

    MG_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerAdminService::GetLog()");

    MgLogManager* pMan = MgLogManager::GetInstance();
    if (NULL == pMan)
    {
        throw new MgNullReferenceException(L"MgServerAdminService::GetLog", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    if (log == MgLogFileType::Access)
    {
        byteReader = pMan->GetAccessLog(fromDate, toDate);
    }
    else if (log == MgLogFileType::Admin)
    {
        byteReader = pMan->GetAdminLog(fromDate, toDate);
    }
    else if (log == MgLogFileType::Authentication)
    {
        byteReader = pMan->GetAuthenticationLog(fromDate, toDate);
    }
    else if (log == MgLogFileType::Error)
    {
        byteReader = pMan->GetErrorLog(fromDate, toDate);
    }
    else if (log == MgLogFileType::Session)
    {
        byteReader = pMan->GetSessionLog(fromDate, toDate);
    }
    else if (log == MgLogFileType::Trace)
    {
        byteReader = pMan->GetTraceLog(fromDate, toDate);
    }
    else
    {
        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(log);

        throw new MgInvalidArgumentException(L"MgServerAdminService.GetLog",
            __LINE__, __WFILE__, &arguments, L"MgInvalidLogType", NULL);
    }

    MG_CATCH_AND_THROW(L"MgServerAdminService.GetLog")

    return byteReader.Detach();
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets all of the contents of the specified log.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
MgByteReader* MgServerAdminService::GetLogFile(CREFSTRING logFile)
{
    Ptr<MgByteReader> byteReader;
    byteReader = NULL;

    MG_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerAdminService::GetLogFile()");

    MgLogManager* pMan = MgLogManager::GetInstance();
    if (NULL == pMan)
    {
        throw new MgNullReferenceException(L"MgServerAdminService::GetLogFile", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    byteReader = pMan->GetLogFile( logFile );

    MG_CATCH_AND_THROW(L"MgServerAdminService.GetLogFile")

    return byteReader.Detach();
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Returns a string collection of the logs currently in the log folder.
/// </summary>
MgPropertyCollection* MgServerAdminService::EnumerateLogs()
{
    Ptr<MgPropertyCollection> logs;
    logs = NULL;

    MG_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerAdminService::EnumerateLogs()");

    MgLogManager* pMan = MgLogManager::GetInstance();
    if (NULL == pMan)
    {
        throw new MgNullReferenceException(L"MgServerAdminService::GetLog", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    logs = pMan->EnumerateLogs();

    MG_CATCH_AND_THROW(L"MgServerAdminService.EnumerateLogs")

    return logs.Detach();
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the configuration properties for the specified property section.
/// </summary>
MgPropertyCollection* MgServerAdminService::GetConfigurationProperties(CREFSTRING propertySection)
{
    Ptr<MgPropertyCollection> pProperties;
    pProperties = NULL;

    MG_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerAdminService::GetConfigurationProperties()");

    MgServerManager* pMan = MgServerManager::GetInstance();
    if (NULL == pMan)
    {
        throw new MgNullReferenceException(L"MgServerAdminService::GetConfigurationProperties", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    pProperties = pMan->GetConfigurationProperties(propertySection);

    MG_CATCH_AND_THROW(L"MgServerAdminService.GetConfigurationProperties");

    return pProperties.Detach();
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Sets the configuration properties for the specified property section.
/// </summary>
void MgServerAdminService::SetConfigurationProperties(CREFSTRING propertySection, MgPropertyCollection* properties)
{
    MG_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerAdminService::SetConfigurationProperties()");

    MgServerManager* pMan = MgServerManager::GetInstance();
    if (NULL == pMan)
    {
        throw new MgNullReferenceException(L"MgServerAdminService::SetConfigurationProperties", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    pMan->SetConfigurationProperties(propertySection, properties);

    MG_CATCH_AND_THROW(L"MgServerAdminService.SetConfigurationProperties");
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Removes the configuration properties for the specified property section.
/// If the properties are not specified, then the entire section will be removed.
/// </summary>
void MgServerAdminService::RemoveConfigurationProperties(CREFSTRING propertySection, MgPropertyCollection* properties)
{
    MG_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerAdminService::SetConfigurationProperties()");

    MgServerManager* pMan = MgServerManager::GetInstance();
    if (NULL == pMan)
    {
        throw new MgNullReferenceException(L"MgServerAdminService::SetConfigurationProperties", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    pMan->RemoveConfigurationProperties(propertySection, properties);

    MG_CATCH_AND_THROW(L"MgServerAdminService.SetConfigurationProperties");
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the information properties for the server.
/// </summary>
MgPropertyCollection* MgServerAdminService::GetInformationProperties()
{
    Ptr<MgPropertyCollection> pProperties;
    pProperties = NULL;

    MG_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerAdminService::GetInformationProperties()");

    MgServerManager* pMan = MgServerManager::GetInstance();
    if (NULL == pMan)
    {
        throw new MgNullReferenceException(L"MgServerAdminService::GetInformationProperties", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    pProperties = pMan->GetInformationProperties();

    MG_CATCH_AND_THROW(L"MgServerAdminService.GetInformationProperties");

    return pProperties.Detach();
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the site version.
/// </summary>
STRING MgServerAdminService::GetSiteVersion()
{
    STRING version = L"";

    MG_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerAdminService::GetSiteVersion()");

    MgServerManager* pMan = MgServerManager::GetInstance();
    if (NULL == pMan)
    {
        throw new MgNullReferenceException(L"MgServerAdminService::GetSiteVersion", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    version = pMan->GetSiteVersion();

    MG_CATCH_AND_THROW(L"MgServerAdminService.GetSiteVersion");

    return version;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the status properties for the server.
/// </summary>
MgPropertyCollection* MgServerAdminService::GetSiteStatus()
{
    Ptr<MgPropertyCollection> pProperties;
    pProperties = NULL;

    MG_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerAdminService::GetSiteStatus()");

    MgServerManager* pMan = MgServerManager::GetInstance();
    if (NULL == pMan)
    {
        throw new MgNullReferenceException(L"MgServerAdminService::GetSiteStatus", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    pProperties = pMan->GetSiteStatus();

    MG_CATCH_AND_THROW(L"MgServerAdminService.GetSiteStatus");

    return pProperties.Detach();
}

///----------------------------------------------------------------------------
/// <summary>
/// Registers services on the specified servers.
/// </summary>
///
/// <param name="serverInfoList">
/// List of server information such as name, description, IP address, and
/// available services.
/// </param>
///
/// <returns>
/// List of server information from all other servers within a site.
/// The returned list may be NULL.
/// </returns>
///
/// EXCEPTIONS:
/// MgConnectionNotOpenException
/// MgNullArgumentException
/// MgInvalidArgumentException
///----------------------------------------------------------------------------
MgSerializableCollection* MgServerAdminService::RegisterServicesOnServers(
    MgSerializableCollection* serverInfoList)
{
    Ptr<MgSerializableCollection> feedbackList;

    MG_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerAdminService::RegisterServicesOnServers()");

    feedbackList = m_loadBalanceMan->RegisterServicesOnServers(serverInfoList);

    MG_CATCH_AND_THROW(L"MgServerAdminService.RegisterServicesOnServers")

    return feedbackList.Detach();
}

///----------------------------------------------------------------------------
/// <summary>
/// Un-registers services on the specified servers.
/// </summary>
///
/// <param name="serverInfoList">
/// List of server information such as name, description, IP address, and
/// available services.
/// </param>
///
/// <returns>
/// Nothing.
/// </returns>
///
/// EXCEPTIONS:
/// MgConnectionNotOpenException
/// MgNullArgumentException
/// MgInvalidArgumentException
///----------------------------------------------------------------------------
void MgServerAdminService::UnregisterServicesOnServers(
    MgSerializableCollection* serverInfoList)
{
    MG_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerAdminService::UnregisterServicesOnServers()");

    m_loadBalanceMan->UnregisterServicesOnServers(serverInfoList);

    MG_CATCH_AND_THROW(L"MgServerAdminService.UnregisterServicesOnServers")
}

//////////////////////////////////////////////////////////////////
/// \brief
/// Send a resource change notification.
///
void MgServerAdminService::NotifyResourcesChanged(MgSerializableCollection* resources)
{
    MG_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerAdminService::NotifyResourcesChanged()");

    MgServiceManager* serviceMan = MgServiceManager::GetInstance();
    assert(NULL != serviceMan);

    serviceMan->NotifyResourcesChanged(resources);

    MG_CATCH_AND_THROW(L"MgServerAdminService.NotifyResourcesChanged")
}

///----------------------------------------------------------------------------
/// <summary>
/// Tells the server to delete the specified package if able.
/// </summary>
///
/// <param name="packageName">
/// The name of the package to be deleted.  Available packages can be found by
/// using EnumeratePackages().
/// </param>
///
/// <returns>
/// Nothing.
/// </returns>
///
/// EXCEPTIONS:
/// MgFileIoException
/// MgInvalidArgumentException
/// MgNullArgumentException
///----------------------------------------------------------------------------
void MgServerAdminService::DeletePackage(CREFSTRING packageName)
{
    MG_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerAdminService::DeletePackage()");

    MgPackageManager::GetInstance()->DeletePackage(packageName);

    MG_CATCH_AND_THROW(L"MgServerAdminService.DeletePackage");
}

///----------------------------------------------------------------------------
/// <summary>
/// Enumerates the packages available in the package directory
/// </summary>
///
/// <returns>
/// A list of packages in the packages directory
/// </returns>
///
/// EXCEPTIONS:
/// MgFileIoException
/// MgFileNotFoundException
/// MgOutOfMemoryException
///----------------------------------------------------------------------------
MgStringCollection* MgServerAdminService::EnumeratePackages()
{
    Ptr<MgStringCollection> packages;
    packages = NULL;

    MG_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerAdminService::EnumeratePackages()");

    packages = MgPackageManager::GetInstance()->EnumeratePackages();

    MG_CATCH_AND_THROW(L"MgServerAdminService.EnumeratePackages");

    return packages.Detach();
}

///----------------------------------------------------------------------------
/// <summary>
/// Gets the current log of the specified package
/// </summary>
///
/// <param name="packageName">
/// The name of the package to get the status for.  Available packages can be
/// found by using EnumeratePackages().
/// </param>
///
/// <returns>
/// An MgByteReader containing the contents of the package's log.
/// </returns>
///
/// EXCEPTIONS:
/// MgFileNotFoundException
/// MgFileIoException
/// MgInvalidArgumentException
/// MgOutOfMemoryException
///----------------------------------------------------------------------------
MgByteReader* MgServerAdminService::GetPackageLog(CREFSTRING packageName)
{
    Ptr<MgByteReader> byteReader;
    byteReader = NULL;

    MG_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerAdminService::GetPackageLog()");

    byteReader= MgPackageManager::GetInstance()->GetPackageLog(packageName);

    MG_CATCH_AND_THROW(L"MgServerAdminService.GetPackageLog");

    return byteReader.Detach();
}

///----------------------------------------------------------------------------
/// <summary>
/// Tells the server to get the current status of the specified package
/// </summary>
///
/// <param name="packageName">
/// The name of the package to get the status for.  Available packages can be
/// found by using EnumeratePackages().
/// </param>
///
/// <returns>
/// The status of the package.
/// </returns>
///
/// EXCEPTIONS:
/// None.
///----------------------------------------------------------------------------
MgPackageStatusInformation* MgServerAdminService::GetPackageStatus(CREFSTRING packageName)
{
    Ptr<MgPackageStatusInformation> statusInfo;

    MG_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerAdminService::GetPackageStatus()");

    statusInfo = MgPackageManager::GetInstance()->GetPackageStatus(packageName);

    MG_CATCH_AND_THROW(L"MgServerAdminService.GetPackageStatus");

    return statusInfo.Detach();
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Loads the specified package into a resource repository.
///
void MgServerAdminService::LoadPackage(CREFSTRING packageName)
{
    MG_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerAdminService::LoadPackage()");

    MgPackageManager::GetInstance()->LoadPackage(packageName);

    MG_CATCH_AND_THROW(L"MgServerAdminService.LoadPackage");
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Creates a package from the specified resource, and then saves it into
/// the specified name.
///
void MgServerAdminService::MakePackage(MgResourceIdentifier* resource,
    CREFSTRING packageName, CREFSTRING packageDescription)
{
    MG_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerAdminService::MakePackage()");

    MgPackageManager::GetInstance()->MakePackage(resource, packageName,
        packageDescription);

    MG_CATCH_AND_THROW(L"MgServerAdminService.MakePackage");
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Specifies the maximum size in kilobytes for the log files.  When the maximum
/// size is exceeded, the current log will be archived, and a new log will be created.
///
/// </summary>
/// <returns>
/// Nothing
/// </returns>
///
/// EXCEPTIONS:
/// MgConnectionNotOpenException
void MgServerAdminService::SetMaximumLogSize(INT32 size)
{
    MG_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerAdminService::SetMaximumLogSize()");

    MgLogManager* pMan = MgLogManager::GetInstance();
    pMan->SetMaximumLogSize(size);

    MG_CATCH_AND_THROW(L"MgServerAdminService.SetMaximumLogSize")
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Specifies the delimiter to use for separating the data fields in the logs.
///
/// </summary>
/// <returns>
/// Nothing
/// </returns>
///
/// EXCEPTIONS:
/// MgConnectionNotOpenException
void MgServerAdminService::SetLogDelimiter(CREFSTRING delimiter)
{
    MG_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerAdminService::SetLogDelimiter()");

    MgLogManager* pMan = MgLogManager::GetInstance();
    pMan->SetLogDelimiter(delimiter);

    MG_CATCH_AND_THROW(L"MgServerAdminService.SetLogDelimiter")
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Enables/disables maximum log file size restriction.  Maximum log file size is
/// set with MgServerAdmin::SetMaximumLogSize().
///
/// </summary>
/// <param name="useMaxSize">
/// Log file size restriction is enforced if useMaxSize = true.
/// using EnumeratePackages().
/// </param>
/// <returns>
/// Nothing
/// </returns>
///
/// EXCEPTIONS:
/// MgConnectionNotOpenException
void MgServerAdminService::EnableMaximumLogSize(bool useMaxSize)
{
    MG_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerAdminService::EnableMaximumLogSize()");

    MgLogManager* pMan = MgLogManager::GetInstance();
    pMan->EnableMaximumLogSize(useMaxSize);

    MG_CATCH_AND_THROW(L"MgServerAdminService.EnableMaximumLogSize")
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Determines if log file size is restricted to the maximum size
/// set with MgServerAdmin::SetMaximumLogSize().
///
/// </summary>
/// <returns>
/// The status of the use of the maximum log size restriction
/// </returns>
///
/// EXCEPTIONS:
/// MgConnectionNotOpenException
bool MgServerAdminService::IsMaximumLogSizeEnabled()
{
    bool useMaxSize = false;

    MG_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerAdminService::IsMaximumLogSizeEnabled()");

    MgLogManager* pMan = MgLogManager::GetInstance();
    useMaxSize = pMan->IsMaximumLogSizeEnabled();

    MG_CATCH_AND_THROW(L"MgServerAdminService.IsMaximumLogSizeEnabled")

    return useMaxSize;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the specified document.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
MgByteReader* MgServerAdminService::GetDocument(CREFSTRING identifier)
{
    Ptr<MgByteReader> byteReader;
    byteReader = NULL;

    MG_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerAdminService::GetDocument()");

    MgServerManager* pMan = MgServerManager::GetInstance();
    if (NULL == pMan)
    {
        throw new MgNullReferenceException(L"MgServerAdminService::GetDocument", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    byteReader = pMan->GetDocument(identifier);

    MG_CATCH_AND_THROW(L"MgServerAdminService.GetDocument")

    return byteReader.Detach();
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Sets the specified document.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
void MgServerAdminService::SetDocument(CREFSTRING identifier, MgByteReader* data)
{
    MG_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerAdminService::SetDocument()");

    MgServerManager* pMan = MgServerManager::GetInstance();
    if (NULL == pMan)
    {
        throw new MgNullReferenceException(L"MgServerAdminService::SetDocument", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    pMan->SetDocument(identifier, data);

    MG_CATCH_AND_THROW(L"MgServerAdminService.SetDocument")
}

void MgServerAdminService::SetConnectionProperties(MgConnectionProperties*)
{
    // Do nothing.  No connection properties are required for Server-side service objects.
}
