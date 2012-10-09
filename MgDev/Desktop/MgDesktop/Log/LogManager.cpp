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

#include "MgDesktop.h"
#include "LogEntryData.h"
#include "LogManager.h"
#include "LogThread.h"

const int MAX_BUF       = 16384;
const int MAX_LINES     = 512;
const int SECONDSINDAY  = 86400;

// Process-wide MgResources
Ptr<MgdLogManager> MgdLogManager::m_logManager          = (MgdLogManager*)NULL;

// Default values
STRING MgdLogManager::m_path                           = L"./";                    // Default path
INT32 MgdLogManager::m_maxLogSize                      = 64;
STRING MgdLogManager::m_delimiter                      = L"\t";
bool MgdLogManager::m_useMaxLogSize                    = false;
const STRING MgdLogManager::DefaultAccessLogFileName         = L"Access.log";
const STRING MgdLogManager::DefaultAdminLogFileName          = L"Admin.log";
const STRING MgdLogManager::DefaultAuthenticationLogFileName = L"Authentication.log";
const STRING MgdLogManager::DefaultErrorLogFileName          = L"Error.log";
const STRING MgdLogManager::DefaultPerformanceLogFileName    = L"Performance.log";
const STRING MgdLogManager::DefaultSessionLogFileName        = L"Session.log";
const STRING MgdLogManager::DefaultTraceLogFileName          = L"Trace.log";

// Log parameters
const STRING MgdLogManager::AverageOpTimeParam   = L"AVERAGEOPTIME";
const STRING MgdLogManager::ClientParam          = L"CLIENT";
const STRING MgdLogManager::ClientIpParam        = L"CLIENTIP";
const STRING MgdLogManager::DurationParam        = L"DURATION";
const STRING MgdLogManager::EndTimeParam         = L"ENDTIME";
const STRING MgdLogManager::ErrorParam           = L"ERROR";
const STRING MgdLogManager::InfoParam            = L"INFO";
const STRING MgdLogManager::OpIdParam            = L"OPID";
const STRING MgdLogManager::OpsFailedParam       = L"OPSFAILED";
const STRING MgdLogManager::OpsProcessedParam    = L"OPSPROCESSED";
const STRING MgdLogManager::OpsReceivedParam     = L"OPSRECEIVED";
const STRING MgdLogManager::StackTraceParam      = L"STACKTRACE";
const STRING MgdLogManager::StartTimeParam       = L"STARTTIME";
const STRING MgdLogManager::UserParam            = L"USER";

// Performance Log parameters
const STRING MgdLogManager::PerformanceAdminOperationsQueueCount  = L"ADMINOPQCOUNT";
const STRING MgdLogManager::PerformanceClientOperationsQueueCount = L"CLIENTOPQCOUNT";
const STRING MgdLogManager::PerformanceSiteOperationsQueueCount   = L"SITEOPQCOUNT";
const STRING MgdLogManager::PerformanceAverageOperationTime       = L"AVGOPTIME";
const STRING MgdLogManager::PerformanceCpuUtilization             = L"CPU";
const STRING MgdLogManager::PerformanceWorkingSet                 = L"WORKINGSET";
const STRING MgdLogManager::PerformanceVirtualMemory              = L"VIRTUALMEMORY";
const STRING MgdLogManager::PerformanceTotalOperationTime         = L"TOTALOPTIME";
const STRING MgdLogManager::PerformanceTotalActiveConnections     = L"TOTALACTIVECONNECTIONS";
const STRING MgdLogManager::PerformanceTotalConnections           = L"TOTALCONNECTIONS";
const STRING MgdLogManager::PerformanceTotalProcessedOperations   = L"TOTALPROCESSEDOP";
const STRING MgdLogManager::PerformanceTotalReceivedOperations    = L"TOTALRECEIVEDOP";
const STRING MgdLogManager::PerformanceUptime                     = L"UPTIME";
const STRING MgdLogManager::PerformanceCacheSize                  = L"CACHESIZE";
const STRING MgdLogManager::PerformanceCacheDroppedEntries        = L"CACHEDROPPEDENTRIES";

// Header line prefix strings
const STRING MgdLogManager::HeaderLine1          = L"# Log Type:";
const STRING MgdLogManager::HeaderLine2          = L"# Log Parameters:";

// Log type strings
const STRING MgdLogManager::AccessLog            = L"Access Log";
const STRING MgdLogManager::AdminLog             = L"Admin Log";
const STRING MgdLogManager::AuthenticationLog    = L"Authentication Log";
const STRING MgdLogManager::ErrorLog             = L"Error Log";
const STRING MgdLogManager::PerformanceLog       = L"Performance Log";
const STRING MgdLogManager::SessionLog           = L"Session Log";
const STRING MgdLogManager::TraceLog             = L"Trace Log";
const STRING MgdLogManager::UnspecifiedLog       = L"Unspecified";

// Log file properties
const STRING MgdLogManager::LogNameProperty      = L"LogNameProperty";
const STRING MgdLogManager::LogTypeProperty      = L"LogTypeProperty";
const STRING MgdLogManager::LogStatusProperty    = L"LogStatusProperty";

// Log status strings
const STRING MgdLogManager::LogStatusActive      = L"Active";
const STRING MgdLogManager::LogStatusArchive     = L"Archive";

// Constructor
MgdLogManager::MgdLogManager() :
    m_outputStream(NULL),
    m_bAccessLogEnabled(true),
    m_bAccessLogHeader(false),
    m_AccessLogFileName(MgdLogManager::DefaultAccessLogFileName),
    m_bAdminLogEnabled(true),
    m_bAdminLogHeader(false),
    m_AdminLogFileName(MgdLogManager::DefaultAdminLogFileName),
    m_bAuthenticationLogEnabled(true),
    m_bAuthenticationLogHeader(false),
    m_AuthenticationLogFileName(MgdLogManager::DefaultAuthenticationLogFileName),
    m_bErrorLogEnabled(true),
    m_bErrorLogHeader(false),
    m_ErrorLogFileName(MgdLogManager::DefaultErrorLogFileName),
    m_bPerformanceLogEnabled(false),
    m_bPerformanceLogHeader(false),
    m_PerformanceLogFileName(MgdLogManager::DefaultPerformanceLogFileName),
    m_bSessionLogEnabled(true),
    m_bSessionLogHeader(false),
    m_SessionLogFileName(MgdLogManager::DefaultSessionLogFileName),
    m_bTraceLogEnabled(false),      // Disabled by default
    m_bTraceLogHeader(false),
    m_TraceLogFileName(MgdLogManager::DefaultTraceLogFileName),
    m_pLogThread(NULL),
    m_writeCount(0)
{
}

// Destructor
MgdLogManager::~MgdLogManager()
{
    ACE_DEBUG ((LM_DEBUG, ACE_TEXT("(%t) MgdLogManager::~MgdLogManager()\n")));

    // Close the logs
    if(m_accessLogStream.is_open())
    {
        m_accessLogStream.close();
    }

    if(m_adminLogStream.is_open())
    {
        m_adminLogStream.close();
    }

    if(m_authenticationLogStream.is_open())
    {
        m_authenticationLogStream.close();
    }

    if(m_errorLogStream.is_open())
    {
        m_errorLogStream.close();
    }

    if(m_performanceLogStream.is_open())
    {
        m_performanceLogStream.close();
    }

    if(m_sessionLogStream.is_open())
    {
        m_sessionLogStream.close();
    }

    if(m_traceLogStream.is_open())
    {
        m_traceLogStream.close();
    }

    delete m_pLogThread;
    m_pLogThread = NULL;
}

void MgdLogManager::Dispose()
{
    delete this;
}

// Get pointer to a process-wide MgdLogManager.
MgdLogManager* MgdLogManager::GetInstance()
{
    MG_LOGMANAGER_TRY()

    ACE_TRACE ("MgdLogManager::GetInstance");

    if (MgdLogManager::m_logManager == NULL)
    {
        // Perform Double-Checked Locking Optimization.
        ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, *ACE_Static_Object_Lock::instance (), 0));
        if (MgdLogManager::m_logManager == NULL)
        {
            MgdLogManager::m_logManager = new MgdLogManager;
        }
    }

    MG_LOGMANAGER_CATCH_AND_THROW(L"MgdLogManager.GetInstance")

    // To avoid overheads and maintain thread safety,
    // do not assign this returned static singleton to a Ptr object.
    return MgdLogManager::m_logManager;
}

// Initialization
void MgdLogManager::Initialize()
{
    MG_LOGMANAGER_TRY()

    m_applicationName = MgResources::ServerServiceDisplayName;

    MgConfiguration* pConfiguration = MgConfiguration::GetInstance();

    // Get the logs path
    pConfiguration->GetStringValue(MgdConfigProperties::GeneralPropertiesSection, MgdConfigProperties::GeneralPropertyLogsPath, m_path, MgdConfigProperties::DefaultGeneralPropertyLogsPath);

    // Check if path ends with a '/' if not, add one if needed
    MgFileUtil::AppendSlashToEndOfPath(m_path);

    // Try and create the directory. If it already exists this will error and not a problem
    ACE_OS::mkdir(MG_WCHAR_TO_TCHAR(m_path));

    // Load the configuration properties
    LoadConfigurationProperties();

    // Create the logging thread
    m_pLogThread = new MgdLogThread(m_threadManager, 1);
    m_pLogThread->Activate();

    MG_LOGMANAGER_CATCH_AND_THROW(L"MgdLogManager.Initialize")
}

void MgdLogManager::LoadConfigurationProperties()
{
    MG_LOGMANAGER_TRY()

    ACE_MT (ACE_GUARD (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex));

    MgConfiguration* pConfiguration = MgConfiguration::GetInstance();

    bool bLogEnabled;
    STRING logFileName;
    STRING logParameters;
    STRING logDetail;

    // Maximum log file size
    pConfiguration->GetBoolValue(MgdConfigProperties::GeneralPropertiesSection, MgdConfigProperties::GeneralPropertyMaxLogFileSizeEnabled, m_useMaxLogSize, MgdConfigProperties::DefaultGeneralPropertyMaxLogFileSizeEnabled);
    pConfiguration->GetIntValue(MgdConfigProperties::GeneralPropertiesSection, MgdConfigProperties::GeneralPropertyMaxLogFileSize, m_maxLogSize, MgdConfigProperties::DefaultGeneralPropertyMaxLogFileSize);
    // Log data delimiter
    pConfiguration->GetStringValue(MgdConfigProperties::GeneralPropertiesSection, MgdConfigProperties::GeneralPropertyLogsDelimiter, m_delimiter, MgdConfigProperties::DefaultGeneralPropertyLogsDelimiter);
    TranslateDelimiter();

    // Logs detail level
    pConfiguration->GetStringValue(MgdConfigProperties::GeneralPropertiesSection, MgdConfigProperties::GeneralPropertyLogsDetail, logDetail, MgdConfigProperties::DefaultGeneralPropertyLogsDetail);
    //m_logsDetail.resize(MgServerInformation::sm_knMaxNumberServices,0);
    m_logsDetail.resize(11,0);
    ParseLogService(MgServiceType::ResourceService, logDetail);
    ParseLogService(MgServiceType::DrawingService, logDetail);
    ParseLogService(MgServiceType::FeatureService, logDetail);
    ParseLogService(MgServiceType::MappingService, logDetail);
    ParseLogService(MgServiceType::RenderingService, logDetail);
    ParseLogService(MgServiceType::TileService, logDetail);
    ParseLogService(MgServiceType::KmlService, logDetail);
    ParseLogService(MgServiceType::ServerAdminService, logDetail);
    ParseLogService(MgServiceType::SiteService, logDetail);
    ParseLogService(MgServiceType::ProfilingService, logDetail);

    // Access Log
    pConfiguration->GetBoolValue(MgdConfigProperties::AccessLogPropertiesSection, MgdConfigProperties::AccessLogPropertyEnabled, bLogEnabled, MgdConfigProperties::DefaultAccessLogPropertyEnabled);
    pConfiguration->GetStringValue(MgdConfigProperties::AccessLogPropertiesSection, MgdConfigProperties::AccessLogPropertyFilename, logFileName, MgdConfigProperties::DefaultAccessLogPropertyFilename);
    pConfiguration->GetStringValue(MgdConfigProperties::AccessLogPropertiesSection, MgdConfigProperties::AccessLogPropertyParameters, logParameters, MgdConfigProperties::DefaultAccessLogPropertyParameters);
    m_AccessLogParameters = logParameters;
    m_AccessLogFileName = ValidateLogFileName(logFileName);
    SetAccessLogEnabled(bLogEnabled);

    // Admin Log
    pConfiguration->GetBoolValue(MgdConfigProperties::AdminLogPropertiesSection, MgdConfigProperties::AdminLogPropertyEnabled, bLogEnabled, MgdConfigProperties::DefaultAdminLogPropertyEnabled);
    pConfiguration->GetStringValue(MgdConfigProperties::AdminLogPropertiesSection, MgdConfigProperties::AdminLogPropertyFilename, logFileName, MgdConfigProperties::DefaultAdminLogPropertyFilename);
    pConfiguration->GetStringValue(MgdConfigProperties::AdminLogPropertiesSection, MgdConfigProperties::AdminLogPropertyParameters, logParameters, MgdConfigProperties::DefaultAdminLogPropertyParameters);
    m_AdminLogParameters = logParameters;
    m_AdminLogFileName = ValidateLogFileName(logFileName);
    SetAdminLogEnabled(bLogEnabled);

    // Authentication Log
    pConfiguration->GetBoolValue(MgdConfigProperties::AuthenticationLogPropertiesSection, MgdConfigProperties::AuthenticationLogPropertyEnabled, bLogEnabled, MgdConfigProperties::DefaultAuthenticationLogPropertyEnabled);
    pConfiguration->GetStringValue(MgdConfigProperties::AuthenticationLogPropertiesSection, MgdConfigProperties::AuthenticationLogPropertyFilename, logFileName, MgdConfigProperties::DefaultAuthenticationLogPropertyFilename);
    pConfiguration->GetStringValue(MgdConfigProperties::AuthenticationLogPropertiesSection, MgdConfigProperties::AuthenticationLogPropertyParameters, logParameters, MgdConfigProperties::DefaultAuthenticationLogPropertyParameters);
    m_AuthenticationLogParameters = logParameters;
    m_AuthenticationLogFileName = ValidateLogFileName(logFileName);
    SetAuthenticationLogEnabled(bLogEnabled);

    // Error Log
    pConfiguration->GetBoolValue(MgdConfigProperties::ErrorLogPropertiesSection, MgdConfigProperties::ErrorLogPropertyEnabled, bLogEnabled, MgdConfigProperties::DefaultErrorLogPropertyEnabled);
    pConfiguration->GetStringValue(MgdConfigProperties::ErrorLogPropertiesSection, MgdConfigProperties::ErrorLogPropertyFilename, logFileName, MgdConfigProperties::DefaultErrorLogPropertyFilename);
    pConfiguration->GetStringValue(MgdConfigProperties::ErrorLogPropertiesSection, MgdConfigProperties::ErrorLogPropertyParameters, logParameters, MgdConfigProperties::DefaultErrorLogPropertyParameters);
    m_ErrorLogParameters = logParameters;
    m_ErrorLogFileName = ValidateLogFileName(logFileName);
    SetErrorLogEnabled(bLogEnabled);

    // Performance Log
    pConfiguration->GetBoolValue(MgdConfigProperties::PerformanceLogPropertiesSection, MgdConfigProperties::PerformanceLogPropertyEnabled, bLogEnabled, MgdConfigProperties::DefaultPerformanceLogPropertyEnabled);
    pConfiguration->GetStringValue(MgdConfigProperties::PerformanceLogPropertiesSection, MgdConfigProperties::PerformanceLogPropertyFilename, logFileName, MgdConfigProperties::DefaultPerformanceLogPropertyFilename);
    pConfiguration->GetStringValue(MgdConfigProperties::PerformanceLogPropertiesSection, MgdConfigProperties::PerformanceLogPropertyParameters, logParameters, MgdConfigProperties::DefaultPerformanceLogPropertyParameters);
    m_PerformanceLogParameters = logParameters;
    m_PerformanceLogFileName = ValidateLogFileName(logFileName);
    SetPerformanceLogEnabled(bLogEnabled);

    // Session Log
    pConfiguration->GetBoolValue(MgdConfigProperties::SessionLogPropertiesSection, MgdConfigProperties::SessionLogPropertyEnabled, bLogEnabled, MgdConfigProperties::DefaultSessionLogPropertyEnabled);
    pConfiguration->GetStringValue(MgdConfigProperties::SessionLogPropertiesSection, MgdConfigProperties::SessionLogPropertyFilename, logFileName, MgdConfigProperties::DefaultSessionLogPropertyFilename);
    pConfiguration->GetStringValue(MgdConfigProperties::SessionLogPropertiesSection, MgdConfigProperties::SessionLogPropertyParameters, logParameters, MgdConfigProperties::DefaultSessionLogPropertyParameters);
    m_SessionLogParameters = logParameters;
    m_SessionLogFileName = ValidateLogFileName(logFileName);
    SetSessionLogEnabled(bLogEnabled);

    // Trace Log
    pConfiguration->GetBoolValue(MgdConfigProperties::TraceLogPropertiesSection, MgdConfigProperties::TraceLogPropertyEnabled, bLogEnabled, MgdConfigProperties::DefaultTraceLogPropertyEnabled);
    pConfiguration->GetStringValue(MgdConfigProperties::TraceLogPropertiesSection, MgdConfigProperties::TraceLogPropertyFilename, logFileName, MgdConfigProperties::DefaultTraceLogPropertyFilename);
    pConfiguration->GetStringValue(MgdConfigProperties::TraceLogPropertiesSection, MgdConfigProperties::TraceLogPropertyParameters, logParameters, MgdConfigProperties::DefaultTraceLogPropertyParameters);
    m_TraceLogParameters = logParameters;
    m_TraceLogFileName = ValidateLogFileName(logFileName);
    SetTraceLogEnabled(bLogEnabled);

    MG_LOGMANAGER_CATCH_AND_THROW(L"MgdLogManager.LoadConfigurationProperties")
}

STRING MgdLogManager::GetLogsPath()
{
    return m_path;
}

STRING MgdLogManager::ValidateLogFileName(CREFSTRING filename)
{
    if (filename.empty())
    {
        throw new MgNullArgumentException(L"MgdLogManager.ValidateLogFileName", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    if (STRING::npos != filename.find(L"\\") ||
        STRING::npos != filename.find(L"/"))
    {
        // Does not support paths.  Must be a strict filename only.
        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(filename);

        MgStringCollection whyArguments;
        whyArguments.Add(L"\\/");

        throw new MgInvalidArgumentException(L"MgdLogManager.ValidateLogFileName",
            __LINE__, __WFILE__, &arguments, L"MgStringContainsReservedCharacters", &whyArguments);
    }

    return (STRING)filename.c_str();
}

void MgdLogManager::SetAccessLogInfo(bool bEnabled, CREFSTRING filename, CREFSTRING parameters)
{
    MG_LOGMANAGER_TRY()

    ACE_MT (ACE_GUARD (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex));

    // Disable existing log in use if there is one
    DisableLog(mltAccess);

    SetAccessLogParameters(parameters);
    SetAccessLogFileName(filename);
    SetAccessLogEnabled(bEnabled);

    MG_LOGMANAGER_CATCH_AND_THROW(L"MgdLogManager.SetAccessLogInfo");
}

bool MgdLogManager::IsAccessLogEnabled()
{
    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex, false));

    return m_bAccessLogEnabled;
}

void MgdLogManager::SetAccessLogEnabled(bool bEnabled)
{
    MG_LOGMANAGER_TRY()

    ACE_MT (ACE_GUARD (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex));

    m_bAccessLogEnabled = bEnabled;
    if(m_bAccessLogEnabled)
    {
        ValidateLogHeaders(mltAccess);
        EnableLog(mltAccess);
    }
    else
    {
        DisableLog(mltAccess);
    }

    MG_LOGMANAGER_CATCH_AND_THROW(L"MgdLogManager.SetAccessLogEnabled")
}

STRING MgdLogManager::GetAccessLogFileName()
{
    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex, STRING(L"")));

    return (STRING)m_AccessLogFileName.c_str();
}

void MgdLogManager::SetAccessLogFileName(CREFSTRING filename)
{
    ACE_MT (ACE_GUARD (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex));

    DisableLog(mltAccess);
    m_AccessLogFileName = ValidateLogFileName(filename);
    EnableLog(mltAccess);
}

STRING MgdLogManager::GetAccessLogParameters()
{
    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex, STRING(L"")));

    return (STRING)m_AccessLogParameters.c_str();
}

void MgdLogManager::SetAccessLogParameters(CREFSTRING parameters)
{
    MG_LOGMANAGER_TRY()

    ACE_MT (ACE_GUARD (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex));

    if (0 != parameters.compare(m_AccessLogParameters))
    {
        ArchiveLog(mltAccess);
    }
    m_AccessLogParameters = parameters.c_str();

    MG_LOGMANAGER_CATCH_AND_THROW(L"MgdLogManager.SetAccessLogParameters")
}

bool MgdLogManager::ClearAccessLog()
{
    bool bResult = false;

    MG_LOGMANAGER_TRY()

    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex, false));

    // Disable the log which closes the log for us
    DisableLog(mltAccess);

    // Get the access log filename and path
    STRING filename = BuildFileName(m_AccessLogFileName);
    bResult = RemoveLogFile(filename);

    // Enable the log which opens the log for us
    EnableLog(mltAccess);

    MG_LOGMANAGER_CATCH_AND_THROW(L"MgdLogManager.ClearAccessLog")

    return bResult;
}

MgByteReader* MgdLogManager::GetAccessLog()
{
    Ptr<MgByteReader> byteReader;

    MG_LOGMANAGER_TRY()

    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex, NULL));

    // Disable the log which closes the log for us
    DisableLog(mltAccess);

    // Get the access log filename and path
    STRING filename = BuildFileName(m_AccessLogFileName);
    byteReader = GetLogContents(filename);

    // Enable the log which opens the log for us
    EnableLog(mltAccess);

    MG_LOGMANAGER_CATCH_AND_THROW(L"MgdLogManager.GetAccessLog")

    return byteReader.Detach();
}

MgByteReader* MgdLogManager::GetAccessLog(INT32 numEntries)
{
    Ptr<MgByteReader> byteReader;

    MG_LOGMANAGER_TRY()

    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex, NULL));

    // Disable the log which closes the log for us
    DisableLog(mltAccess);

    // Get the access log filename and path
    STRING filename = BuildFileName(m_AccessLogFileName);
    byteReader = GetLogContents(filename, numEntries);

    // Enable the log which opens the log for us
    EnableLog(mltAccess);

    MG_LOGMANAGER_CATCH_AND_THROW(L"MgdLogManager.GetAccessLog")

    return byteReader.Detach();
}

MgByteReader* MgdLogManager::GetAccessLog(MgDateTime* fromDate, MgDateTime* toDate)
{
    Ptr<MgByteReader> byteReader;

    MG_LOGMANAGER_TRY()

    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex, NULL));

    // Disable the log which closes the log for us
    DisableLog(mltAccess);

    byteReader = GetLogContents(mltAccess, fromDate, toDate);

    // Enable the log which opens the log for us
    EnableLog(mltAccess);

    MG_LOGMANAGER_CATCH_AND_THROW(L"MgdLogManager.GetAccessLog")

    return byteReader.Detach();
}

void MgdLogManager::SetAdminLogInfo(bool bEnabled, CREFSTRING filename, CREFSTRING parameters)
{
    MG_LOGMANAGER_TRY()

    ACE_MT (ACE_GUARD (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex));

    // Disable existing log in use if there is one
    DisableLog(mltAdmin);

    SetAdminLogParameters(parameters);
    SetAdminLogFileName(filename);
    SetAdminLogEnabled(bEnabled);

    MG_LOGMANAGER_CATCH_AND_THROW(L"MgdLogManager.SetAdminLogInfo");
}

bool MgdLogManager::IsAdminLogEnabled()
{
    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex, false));

    return m_bAdminLogEnabled;
}

void MgdLogManager::SetAdminLogEnabled(bool bEnabled)
{
    MG_LOGMANAGER_TRY()

    ACE_MT (ACE_GUARD (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex));

    m_bAdminLogEnabled = bEnabled;
    if(m_bAdminLogEnabled)
    {
        ValidateLogHeaders(mltAdmin);
        EnableLog(mltAdmin);
    }
    else
    {
        DisableLog(mltAdmin);
    }

    MG_LOGMANAGER_CATCH_AND_THROW(L"MgdLogManager.SetAdminLogEnabled")
}

STRING MgdLogManager::GetAdminLogFileName()
{
    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex, STRING(L"")));

    return (STRING)m_AdminLogFileName.c_str();
}

void MgdLogManager::SetAdminLogFileName(CREFSTRING filename)
{
    ACE_MT (ACE_GUARD (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex));

    DisableLog(mltAdmin);
    m_AdminLogFileName = ValidateLogFileName(filename);
    EnableLog(mltAdmin);
}

STRING MgdLogManager::GetAdminLogParameters()
{
    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex, STRING(L"")));

    return (STRING)m_AdminLogParameters.c_str();
}

void MgdLogManager::SetAdminLogParameters(CREFSTRING parameters)
{
    MG_LOGMANAGER_TRY()

    ACE_MT (ACE_GUARD (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex));

    if (0 != parameters.compare(m_AdminLogParameters))
    {
        ArchiveLog(mltAdmin);
    }
    m_AdminLogParameters = parameters.c_str();

    MG_LOGMANAGER_CATCH_AND_THROW(L"MgdLogManager.SetAdminLogParameters")
}

bool MgdLogManager::ClearAdminLog()
{
    bool bResult = false;

    MG_LOGMANAGER_TRY()

    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex, false));

    // Disable the log which closes the log for us
    DisableLog(mltAdmin);

    // Get the admin log filename and path
    STRING filename = BuildFileName(m_AdminLogFileName);
    bResult = RemoveLogFile(filename);

    // Enable the log which opens the log for us
    EnableLog(mltAdmin);

    MG_LOGMANAGER_CATCH_AND_THROW(L"MgdLogManager.ClearAdminLog")

    return bResult;
}

MgByteReader* MgdLogManager::GetAdminLog()
{
    Ptr<MgByteReader> byteReader;

    MG_LOGMANAGER_TRY()

    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex, NULL));

    // Disable the log which closes the log for us
    DisableLog(mltAdmin);

    // Get the admin log filename and path
    STRING filename = BuildFileName(m_AdminLogFileName);
    byteReader = GetLogContents(filename);

    // Enable the log which opens the log for us
    EnableLog(mltAdmin);

    MG_LOGMANAGER_CATCH_AND_THROW(L"MgdLogManager.GetAdminLog")

    return byteReader.Detach();
}

MgByteReader* MgdLogManager::GetAdminLog(INT32 numEntries)
{
    Ptr<MgByteReader> byteReader;

    MG_LOGMANAGER_TRY()

    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex, NULL));

    // Disable the log which closes the log for us
    DisableLog(mltAdmin);

    // Get the admin log filename and path
    STRING filename = BuildFileName(m_AdminLogFileName);
    byteReader = GetLogContents(filename, numEntries);

    // Enable the log which opens the log for us
    EnableLog(mltAdmin);

    MG_LOGMANAGER_CATCH_AND_THROW(L"MgdLogManager.GetAdminLog")

    return byteReader.Detach();
}

MgByteReader* MgdLogManager::GetAdminLog(MgDateTime* fromDate, MgDateTime* toDate)
{
    Ptr<MgByteReader> byteReader;

    MG_LOGMANAGER_TRY()

    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex, NULL));

    // Disable the log which closes the log for us
    DisableLog(mltAdmin);

    byteReader = GetLogContents(mltAdmin, fromDate, toDate);

    // Enable the log which opens the log for us
    EnableLog(mltAdmin);

    MG_LOGMANAGER_CATCH_AND_THROW(L"MgdLogManager.GetAdminLog")

    return byteReader.Detach();
}

void MgdLogManager::SetAuthenticationLogInfo(bool bEnabled, CREFSTRING filename, CREFSTRING parameters)
{
    MG_LOGMANAGER_TRY()

    ACE_MT (ACE_GUARD (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex));

    // Disable existing log in use if there is one
    DisableLog(mltAuthentication);

    SetAuthenticationLogParameters(parameters);
    SetAuthenticationLogFileName(filename);
    SetAuthenticationLogEnabled(bEnabled);

    MG_LOGMANAGER_CATCH_AND_THROW(L"MgdLogManager.SetAuthenticationLogInfo");
}

bool MgdLogManager::IsAuthenticationLogEnabled()
{
    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex, false));

    return m_bAuthenticationLogEnabled;
}

void MgdLogManager::SetAuthenticationLogEnabled(bool bEnabled)
{
    MG_LOGMANAGER_TRY()

    ACE_MT (ACE_GUARD (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex));

    m_bAuthenticationLogEnabled = bEnabled;
    if(m_bAuthenticationLogEnabled)
    {
        ValidateLogHeaders(mltAuthentication);
        EnableLog(mltAuthentication);
    }
    else
    {
        DisableLog(mltAuthentication);
    }

    MG_LOGMANAGER_CATCH_AND_THROW(L"MgdLogManager.SetAuthenticationLogEnabled")
}

STRING MgdLogManager::GetAuthenticationLogFileName()
{
    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex, STRING(L"")));

    return (STRING)m_AuthenticationLogFileName.c_str();
}

void MgdLogManager::SetAuthenticationLogFileName(CREFSTRING filename)
{
    ACE_MT (ACE_GUARD (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex));

    DisableLog(mltAuthentication);
    m_AuthenticationLogFileName = ValidateLogFileName(filename);
    EnableLog(mltAuthentication);
}

STRING MgdLogManager::GetAuthenticationLogParameters()
{
    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex, STRING(L"")));

    return (STRING)m_AuthenticationLogParameters.c_str();
}

void MgdLogManager::SetAuthenticationLogParameters(CREFSTRING parameters)
{
    MG_LOGMANAGER_TRY()

    ACE_MT (ACE_GUARD (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex));

    if (0 != parameters.compare(m_AuthenticationLogParameters))
    {
        ArchiveLog(mltAuthentication);
    }
    m_AuthenticationLogParameters = parameters.c_str();

    MG_LOGMANAGER_CATCH_AND_THROW(L"MgdLogManager.SetAuthenticationLogParameters")
}

bool MgdLogManager::ClearAuthenticationLog()
{
    bool bResult = false;

    MG_LOGMANAGER_TRY()

    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex, false));

    // Disable the log which closes the log for us
    DisableLog(mltAuthentication);

    // Get the authentication log filename and path
    STRING filename = BuildFileName(m_AuthenticationLogFileName);
    bResult = RemoveLogFile(filename);

    // Enable the log which opens the log for us
    EnableLog(mltAuthentication);

    MG_LOGMANAGER_CATCH_AND_THROW(L"MgdLogManager.ClearAuthenticationLog")

    return bResult;
}

MgByteReader* MgdLogManager::GetAuthenticationLog()
{
    Ptr<MgByteReader> byteReader;

    MG_LOGMANAGER_TRY()

    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex, NULL));

    // Disable the log which closes the log for us
    DisableLog(mltAuthentication);

    // Get the authentication log filename and path
    STRING filename = BuildFileName(m_AuthenticationLogFileName);
    byteReader = GetLogContents(filename);

    // Enable the log which opens the log for us
    EnableLog(mltAuthentication);

    MG_LOGMANAGER_CATCH_AND_THROW(L"MgdLogManager.GetAuthenticationLog")

    return byteReader.Detach();
}

MgByteReader* MgdLogManager::GetAuthenticationLog(INT32 numEntries)
{
    Ptr<MgByteReader> byteReader;

    MG_LOGMANAGER_TRY()

    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex, NULL));

    // Disable the log which closes the log for us
    DisableLog(mltAuthentication);

    // Get the authentication log filename and path
    STRING filename = BuildFileName(m_AuthenticationLogFileName);
    byteReader = GetLogContents(filename, numEntries);

    // Enable the log which opens the log for us
    EnableLog(mltAuthentication);

    MG_LOGMANAGER_CATCH_AND_THROW(L"MgdLogManager.GetAuthenticationLog")

    return byteReader.Detach();
}

MgByteReader* MgdLogManager::GetAuthenticationLog(MgDateTime* fromDate, MgDateTime* toDate)
{
    Ptr<MgByteReader> byteReader;

    MG_LOGMANAGER_TRY()

    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex, NULL));

    // Disable the log which closes the log for us
    DisableLog(mltAuthentication);

    byteReader = GetLogContents(mltAuthentication, fromDate, toDate);

    // Enable the log which opens the log for us
    EnableLog(mltAuthentication);

    MG_LOGMANAGER_CATCH_AND_THROW(L"MgdLogManager.GetAuthenticationLog")

    return byteReader.Detach();
}


void MgdLogManager::SetErrorLogInfo(bool bEnabled, CREFSTRING filename, CREFSTRING parameters)
{
    MG_LOGMANAGER_TRY()

    ACE_MT (ACE_GUARD (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex));

    // Disable existing log in use if there is one
    DisableLog(mltError);

    SetErrorLogParameters(parameters);
    SetErrorLogFileName(filename);
    SetErrorLogEnabled(bEnabled);

    MG_LOGMANAGER_CATCH_AND_THROW(L"MgdLogManager.SetErrorLogInfo");
}

bool MgdLogManager::IsErrorLogEnabled()
{
    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex, false));

    return m_bErrorLogEnabled;
}

void MgdLogManager::SetErrorLogEnabled(bool bEnabled)
{
    MG_LOGMANAGER_TRY()

    ACE_MT (ACE_GUARD (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex));

    m_bErrorLogEnabled = bEnabled;
    if(m_bErrorLogEnabled)
    {
        ValidateLogHeaders(mltError);
        EnableLog(mltError);
    }
    else
    {
        DisableLog(mltError);
    }

    MG_LOGMANAGER_CATCH_AND_THROW(L"MgdLogManager.SetErrorLogEnabled")
}

STRING MgdLogManager::GetErrorLogFileName()
{
    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex, STRING(L"")));

    return (STRING)m_ErrorLogFileName.c_str();
}

void MgdLogManager::SetErrorLogFileName(CREFSTRING filename)
{
    ACE_MT (ACE_GUARD (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex));

    DisableLog(mltError);
    m_ErrorLogFileName = ValidateLogFileName(filename);
    EnableLog(mltError);
}

STRING MgdLogManager::GetErrorLogParameters()
{
    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex, STRING(L"")));

    return (STRING)m_ErrorLogParameters.c_str();
}

void MgdLogManager::SetErrorLogParameters(CREFSTRING parameters)
{
    MG_LOGMANAGER_TRY()

    ACE_MT (ACE_GUARD (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex));

    if (0 != parameters.compare(m_ErrorLogParameters))
    {
        ArchiveLog(mltError);
    }
    m_ErrorLogParameters = parameters.c_str();

    MG_LOGMANAGER_CATCH_AND_THROW(L"MgdLogManager.SetErrorLogParameters")
}

bool MgdLogManager::ClearErrorLog()
{
    bool bResult = false;

    MG_LOGMANAGER_TRY()

    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex, false));

    // Disable the log which closes the log for us
    DisableLog(mltError);

    // Get the error log filename and path
    STRING filename = BuildFileName(m_ErrorLogFileName);
    bResult = RemoveLogFile(filename);

    // Enable the log which opens the log for us
    EnableLog(mltError);

    MG_LOGMANAGER_CATCH_AND_THROW(L"MgdLogManager.ClearErrorLog")

    return bResult;
}

MgByteReader* MgdLogManager::GetErrorLog()
{
    Ptr<MgByteReader> byteReader;

    MG_LOGMANAGER_TRY()

    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex, NULL));

    // Disable the log which closes the log for us
    DisableLog(mltError);

    // Get the error log filename and path
    STRING filename = BuildFileName(m_ErrorLogFileName);
    byteReader = GetLogContents(filename);

    // Enable the log which opens the log for us
    EnableLog(mltError);

    MG_LOGMANAGER_CATCH_AND_THROW(L"MgdLogManager.GetErrorLog")

    return byteReader.Detach();
}

MgByteReader* MgdLogManager::GetErrorLog(INT32 numEntries)
{
    Ptr<MgByteReader> byteReader;

    MG_LOGMANAGER_TRY()

    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex, NULL));

    // Disable the log which closes the log for us
    DisableLog(mltError);

    // Get the error log filename and path
    STRING filename = BuildFileName(m_ErrorLogFileName);
    byteReader = GetLogContents(filename, numEntries);

    // Enable the log which opens the log for us
    EnableLog(mltError);

    MG_LOGMANAGER_CATCH_AND_THROW(L"MgdLogManager.GetErrorLog")

    return byteReader.Detach();
}

MgByteReader* MgdLogManager::GetErrorLog(MgDateTime* fromDate, MgDateTime* toDate)
{
    Ptr<MgByteReader> byteReader;

    MG_LOGMANAGER_TRY()

    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex, NULL));

    // Disable the log which closes the log for us
    DisableLog(mltError);

    byteReader = GetLogContents(mltError, fromDate, toDate);

    // Enable the log which opens the log for us
    EnableLog(mltError);

    MG_LOGMANAGER_CATCH_AND_THROW(L"MgdLogManager.GetErrorLog")

    return byteReader.Detach();
}

void MgdLogManager::SetPerformanceLogInfo(bool bEnabled, CREFSTRING filename, CREFSTRING parameters)
{
    MG_LOGMANAGER_TRY()

    ACE_MT (ACE_GUARD (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex));

    // Disable existing log in use if there is one
    DisableLog(mltError);

    SetPerformanceLogParameters(parameters);
    SetPerformanceLogFileName(filename);
    SetPerformanceLogEnabled(bEnabled);

    MG_LOGMANAGER_CATCH_AND_THROW(L"MgdLogManager.SetPerformanceLogInfo");
}

bool MgdLogManager::IsPerformanceLogEnabled()
{
    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex, false));

    return m_bPerformanceLogEnabled;
}

void MgdLogManager::SetPerformanceLogEnabled(bool bEnabled)
{
    MG_LOGMANAGER_TRY()

    ACE_MT (ACE_GUARD (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex));

    m_bPerformanceLogEnabled = bEnabled;
    if(m_bPerformanceLogEnabled)
    {
        ValidateLogHeaders(mltPerformance);
        EnableLog(mltPerformance);
    }
    else
    {
        DisableLog(mltPerformance);
    }

    MG_LOGMANAGER_CATCH_AND_THROW(L"MgdLogManager.SetPerformanceLogEnabled")
}

STRING MgdLogManager::GetPerformanceLogFileName()
{
    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex, STRING(L"")));

    return (STRING)m_PerformanceLogFileName.c_str();
}

void MgdLogManager::SetPerformanceLogFileName(CREFSTRING filename)
{
    ACE_MT (ACE_GUARD (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex));

    DisableLog(mltPerformance);
    m_PerformanceLogFileName = ValidateLogFileName(filename);
    EnableLog(mltPerformance);
}

STRING MgdLogManager::GetPerformanceLogParameters()
{
    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex, STRING(L"")));

    return (STRING)m_PerformanceLogParameters.c_str();
}

void MgdLogManager::SetPerformanceLogParameters(CREFSTRING parameters)
{
    MG_LOGMANAGER_TRY()

    ACE_MT (ACE_GUARD (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex));

    if (0 != parameters.compare(m_PerformanceLogParameters))
    {
        ArchiveLog(mltPerformance);
    }
    m_PerformanceLogParameters = parameters.c_str();

    MG_LOGMANAGER_CATCH_AND_THROW(L"MgdLogManager.SetPerformanceLogParameters")
}

bool MgdLogManager::ClearPerformanceLog()
{
    bool bResult = false;

    MG_LOGMANAGER_TRY()

    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex, false));

    // Disable the log which closes the log for us
    DisableLog(mltPerformance);

    // Get the performance log filename and path
    STRING filename = BuildFileName(m_PerformanceLogFileName);
    bResult = RemoveLogFile(filename);

    // Enable the log which opens the log for us
    EnableLog(mltPerformance);

    MG_LOGMANAGER_CATCH_AND_THROW(L"MgdLogManager.ClearPerformanceLog")

    return bResult;
}

MgByteReader* MgdLogManager::GetPerformanceLog()
{
    Ptr<MgByteReader> byteReader;

    MG_LOGMANAGER_TRY()

    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex, NULL));

    // Disable the log which closes the log for us
    DisableLog(mltPerformance);

    // Get the performance log filename and path
    STRING filename = BuildFileName(m_PerformanceLogFileName);
    byteReader = GetLogContents(filename);

    // Enable the log which opens the log for us
    EnableLog(mltPerformance);

    MG_LOGMANAGER_CATCH_AND_THROW(L"MgdLogManager.GetPerformanceLog")

    return byteReader.Detach();
}

MgByteReader* MgdLogManager::GetPerformanceLog(INT32 numEntries)
{
    Ptr<MgByteReader> byteReader;

    MG_LOGMANAGER_TRY()

    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex, NULL));

    // Disable the log which closes the log for us
    DisableLog(mltPerformance);

    // Get the performance log filename and path
    STRING filename = BuildFileName(m_PerformanceLogFileName);
    byteReader = GetLogContents(filename, numEntries);

    // Enable the log which opens the log for us
    EnableLog(mltPerformance);

    MG_LOGMANAGER_CATCH_AND_THROW(L"MgdLogManager.GetPerformanceLog")

    return byteReader.Detach();
}

MgByteReader* MgdLogManager::GetPerformanceLog(MgDateTime* fromDate, MgDateTime* toDate)
{
    Ptr<MgByteReader> byteReader;

    MG_LOGMANAGER_TRY()

    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex, NULL));

    // Disable the log which closes the log for us
    DisableLog(mltPerformance);

    byteReader = GetLogContents(mltPerformance, fromDate, toDate);

    // Enable the log which opens the log for us
    EnableLog(mltPerformance);

    MG_LOGMANAGER_CATCH_AND_THROW(L"MgdLogManager.GetPerformanceLog")

    return byteReader.Detach();
}

void MgdLogManager::SetSessionLogInfo(bool bEnabled, CREFSTRING filename, CREFSTRING parameters)
{
    MG_LOGMANAGER_TRY()

    ACE_MT (ACE_GUARD (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex));

    // Disable existing log in use if there is one
    DisableLog(mltSession);

    SetSessionLogParameters(parameters);
    SetSessionLogFileName(filename);
    SetSessionLogEnabled(bEnabled);

    MG_LOGMANAGER_CATCH_AND_THROW(L"MgdLogManager.SetSessionLogInfo");
}

bool MgdLogManager::IsSessionLogEnabled()
{
    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex, false));

    return m_bSessionLogEnabled;
}

void MgdLogManager::SetSessionLogEnabled(bool bEnabled)
{
    MG_LOGMANAGER_TRY()

    ACE_MT (ACE_GUARD (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex));

    m_bSessionLogEnabled = bEnabled;
    if(m_bSessionLogEnabled)
    {
        ValidateLogHeaders(mltSession);
        EnableLog(mltSession);
    }
    else
    {
        DisableLog(mltSession);
    }

    MG_LOGMANAGER_CATCH_AND_THROW(L"MgdLogManager.SetSessionLogEnabled")
}

STRING MgdLogManager::GetSessionLogFileName()
{
    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex, STRING(L"")));

    return (STRING)m_SessionLogFileName.c_str();
}

void MgdLogManager::SetSessionLogFileName(CREFSTRING filename)
{
    ACE_MT (ACE_GUARD (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex));

    DisableLog(mltSession);
    m_SessionLogFileName = ValidateLogFileName(filename);
    EnableLog(mltSession);
}

STRING MgdLogManager::GetSessionLogParameters()
{
    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex, STRING(L"")));

    return (STRING)m_SessionLogParameters.c_str();
}

void MgdLogManager::SetSessionLogParameters(CREFSTRING parameters)
{
    MG_LOGMANAGER_TRY()

    ACE_MT (ACE_GUARD (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex));

    if (0 != parameters.compare(m_SessionLogParameters))
    {
        ArchiveLog(mltSession);
    }
    m_SessionLogParameters = parameters.c_str();

    MG_LOGMANAGER_CATCH_AND_THROW(L"MgdLogManager.SetSessionLogParameters")
}

bool MgdLogManager::ClearSessionLog()
{
    bool bResult = false;

    MG_LOGMANAGER_TRY()

    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex, false));

    // Disable the log which closes the log for us
    DisableLog(mltSession);

    // Get the session log filename and path
    STRING filename = BuildFileName(m_SessionLogFileName);
    bResult = RemoveLogFile(filename);

    // Enable the log which opens the log for us
    EnableLog(mltSession);

    MG_LOGMANAGER_CATCH_AND_THROW(L"MgdLogManager.ClearSessionLog")

    return bResult;
}

MgByteReader* MgdLogManager::GetSessionLog()
{
    Ptr<MgByteReader> byteReader;

    MG_LOGMANAGER_TRY()

    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex, NULL));

    // Disable the log which closes the log for us
    DisableLog(mltSession);

    // Get the session log filename and path
    STRING filename = BuildFileName(m_SessionLogFileName);
    byteReader = GetLogContents(filename);

    // Enable the log which opens the log for us
    EnableLog(mltSession);

    MG_LOGMANAGER_CATCH_AND_THROW(L"MgdLogManager.GetSessionLog")

    return byteReader.Detach();
}

MgByteReader* MgdLogManager::GetSessionLog(INT32 numEntries)
{
    Ptr<MgByteReader> byteReader;

    MG_LOGMANAGER_TRY()

    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex, NULL));

    // Disable the log which closes the log for us
    DisableLog(mltSession);

    // Get the session log filename and path
    STRING filename = BuildFileName(m_SessionLogFileName);
    byteReader = GetLogContents(filename, numEntries);

    // Enable the log which opens the log for us
    EnableLog(mltSession);

    MG_LOGMANAGER_CATCH_AND_THROW(L"MgdLogManager.GetSessionLog")

    return byteReader.Detach();
}

MgByteReader* MgdLogManager::GetSessionLog(MgDateTime* fromDate, MgDateTime* toDate)
{
    Ptr<MgByteReader> byteReader;

    MG_LOGMANAGER_TRY()

    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex, NULL));

    // Disable the log which closes the log for us
    DisableLog(mltSession);

    byteReader = GetLogContents(mltSession, fromDate, toDate);

    // Enable the log which opens the log for us
    EnableLog(mltSession);

    MG_LOGMANAGER_CATCH_AND_THROW(L"MgdLogManager.GetSessionLog")

    return byteReader.Detach();
}

void MgdLogManager::SetTraceLogInfo(bool bEnabled, CREFSTRING filename, CREFSTRING parameters)
{
    MG_LOGMANAGER_TRY()

    ACE_MT (ACE_GUARD (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex));

    // Disable existing log in use if there is one
    DisableLog(mltTrace);

    SetTraceLogParameters(parameters);
    SetTraceLogFileName(filename);
    SetTraceLogEnabled(bEnabled);

    MG_LOGMANAGER_CATCH_AND_THROW(L"MgdLogManager.SetTraceLogInfo")
}

bool MgdLogManager::IsTraceLogEnabled()
{
    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex, false));

    return m_bTraceLogEnabled;
}

void MgdLogManager::SetTraceLogEnabled(bool bEnabled)
{
    MG_LOGMANAGER_TRY()

    ACE_MT (ACE_GUARD (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex));

    m_bTraceLogEnabled = bEnabled;
    if(m_bTraceLogEnabled)
    {
        ValidateLogHeaders(mltTrace);
        EnableLog(mltTrace);
    }
    else
    {
        DisableLog(mltTrace);
    }

    MG_LOGMANAGER_CATCH_AND_THROW(L"MgdLogManager.SetTraceLogEnabled")
}

STRING MgdLogManager::GetTraceLogFileName()
{
    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex, STRING(L"")));

    return (STRING)m_TraceLogFileName.c_str();
}

void MgdLogManager::SetTraceLogFileName(CREFSTRING filename)
{
    ACE_MT (ACE_GUARD (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex));

    DisableLog(mltTrace);
    m_TraceLogFileName = ValidateLogFileName(filename);
    EnableLog(mltTrace);
}

STRING MgdLogManager::GetTraceLogParameters()
{
    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex, STRING(L"")));

    return(STRING) m_TraceLogParameters.c_str();
}

void MgdLogManager::SetTraceLogParameters(CREFSTRING parameters)
{
    MG_LOGMANAGER_TRY()

    ACE_MT (ACE_GUARD (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex));

    if (0 != parameters.compare(m_TraceLogParameters))
    {
        ArchiveLog(mltTrace);
    }
    m_TraceLogParameters = parameters.c_str();

    MG_LOGMANAGER_CATCH_AND_THROW(L"MgdLogManager.SetTraceLogParameters")
}

INT8 MgdLogManager::GetDetailLevelForService(INT16 serviceNum)
{
    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex, 0));

    return m_logsDetail[serviceNum];
}

bool MgdLogManager::ClearTraceLog()
{
    bool bResult = false;

    MG_LOGMANAGER_TRY()

    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex, false));

    // Disable the log which closes the log for us
    DisableLog(mltTrace);

    // Get the trace log filename and path
    STRING filename = BuildFileName(m_TraceLogFileName);
    bResult = RemoveLogFile(filename);

    // Enable the log which opens the log for us
    EnableLog(mltTrace);

    MG_LOGMANAGER_CATCH_AND_THROW(L"MgdLogManager.ClearTraceLog")

    return bResult;
}

MgByteReader* MgdLogManager::GetTraceLog()
{
    Ptr<MgByteReader> byteReader;

    MG_LOGMANAGER_TRY()

    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex, NULL));

    // Disable the log which closes the log for us
    DisableLog(mltTrace);

    // Get the trace log filename and path
    STRING filename = BuildFileName(m_TraceLogFileName);
    byteReader = GetLogContents(filename);

    // Enable the log which opens the log for us
    EnableLog(mltTrace);

    MG_LOGMANAGER_CATCH_AND_THROW(L"MgdLogManager.GetTraceLog")

    return byteReader.Detach();
}

MgByteReader* MgdLogManager::GetTraceLog(INT32 numEntries)
{
    Ptr<MgByteReader> byteReader;

    MG_LOGMANAGER_TRY()

    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex, NULL));

    // Disable the log which closes the log for us
    DisableLog(mltTrace);

    // Get the trace log filename and path
    STRING filename = BuildFileName(m_TraceLogFileName);
    byteReader = GetLogContents(filename, numEntries);

    // Enable the log which opens the log for us
    EnableLog(mltTrace);

    MG_LOGMANAGER_CATCH_AND_THROW(L"MgdLogManager.GetTraceLog")

    return byteReader.Detach();
}

MgByteReader* MgdLogManager::GetTraceLog(MgDateTime* fromDate, MgDateTime* toDate)
{
    Ptr<MgByteReader> byteReader;

    MG_LOGMANAGER_TRY()

    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex, NULL));

    // Disable the log which closes the log for us
    DisableLog(mltTrace);

    byteReader = GetLogContents(mltTrace, fromDate, toDate);

    // Enable the log which opens the log for us
    EnableLog(mltTrace);

    MG_LOGMANAGER_CATCH_AND_THROW(L"MgdLogManager.GetTraceLog")

    return byteReader.Detach();
}

MgByteReader* MgdLogManager::GetLogFile( CREFSTRING filename )
{
    Ptr<MgByteReader> byteReader;

    MG_LOGMANAGER_TRY()

    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex, NULL));

    enum MgLogType logType = mltSystem; // Assign to log type not associated with a file

    bool bInUse = IsLogFileInUse(filename, logType);
    if(bInUse)
    {
        DisableLog(logType);
    }

    // Get the trace log filename and path
    STRING fullPath = BuildFileName(filename);
    byteReader = GetLogContents(fullPath);

    if(bInUse)
    {
        EnableLog(logType);
    }

    MG_LOGMANAGER_CATCH_AND_THROW(L"MgdLogManager.GetLogFile")

    return byteReader.Detach();
}

void MgdLogManager::LogToSysLog(ACE_Log_Msg* pAce, char* application)
{
    ACE_MT (ACE_GUARD (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex));

    pAce->open(ACE_TEXT_CHAR_TO_TCHAR(application), ACE_Log_Msg::SYSLOG, ACE_TEXT_CHAR_TO_TCHAR(application));
}

void MgdLogManager::LogToOStream(ACE_Log_Msg* pAce, ACE_OSTREAM_TYPE* output)
{
    ACE_MT (ACE_GUARD (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex));

    m_outputStream = output;
    pAce->msg_ostream(m_outputStream);
    pAce->clr_flags(ACE_Log_Msg::STDERR | ACE_Log_Msg::LOGGER | ACE_Log_Msg::SYSLOG);
    pAce->set_flags(ACE_Log_Msg::OSTREAM);
}

void MgdLogManager::LogToStderr(ACE_Log_Msg* pAce)
{
    ACE_MT (ACE_GUARD (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex));

    pAce->clr_flags(ACE_Log_Msg::OSTREAM | ACE_Log_Msg::LOGGER | ACE_Log_Msg::SYSLOG);
    pAce->set_flags(ACE_Log_Msg::STDERR);
}

void MgdLogManager::LogError(CREFSTRING entry, CREFSTRING client, CREFSTRING clientIp, CREFSTRING userName, CREFSTRING stackTrace)
{
    // Errors are always logged to both the error and trace logs if enabled.
    if(IsErrorLogEnabled())
    {
        // Check if this is a startup/shutdown entry
        if((MgResources::ServerStarted == entry) ||
           (MgResources::ServerStopped == entry))
        {
            LogErrorEntry(entry, client, clientIp, userName, stackTrace, MgResources::Success);
        }
        else
        {
            LogErrorEntry(entry, client, clientIp, userName, stackTrace, MgResources::Error);
        }
    }
    if(IsTraceLogEnabled())
    {
        LogTraceEntry(entry, client, clientIp, userName, stackTrace, MgResources::Error);
    }
}

void MgdLogManager::LogWarning(INT16 service, CREFSTRING entry, CREFSTRING client, CREFSTRING clientIp, CREFSTRING userName, CREFSTRING stackTrace)
{
    // Warnings are only logged if the detail level for the service is high enough.
    INT8 detailLevel = GetDetailLevelForService(service);

    if (detailLevel >= MgdLogDetail::Warning)
    {
        // Log entries to both error log and trace log, if applicable
        if(IsErrorLogEnabled())
        {
            LogErrorEntry(entry, client, clientIp, userName, stackTrace, MgResources::Warnings);
        }
        if(IsTraceLogEnabled())
        {
            LogTraceEntry(entry, client, clientIp, userName, stackTrace, MgResources::Warnings);
        }
    }
}

void MgdLogManager::LogSystemEntry(ACE_Log_Priority priority, CREFSTRING entry)
{
    QueueLogEntry(mltSystem, entry, priority);
}

void MgdLogManager::LogAccessEntry(CREFSTRING opId, CREFSTRING client, CREFSTRING clientIp, CREFSTRING userName)
{
    // Message to be entered into the log
    STRING logEntry;

    MG_LOGMANAGER_TRY()

    // Current threadid is first parameter
    AddThreadId(logEntry);

    // Parse parameter string into an MgStringCollection
    Ptr<MgStringCollection> paramList = MgStringCollection::ParseCollection(
        GetAccessLogParameters().c_str(), L",");

    // Go through parameter list and add the information appropriately
    if (paramList != NULL)
    {
        INT32 numParams = paramList->GetCount();
        STRING param;

        for (INT32 i = 0; i < numParams; ++i)
        {
            param = paramList->GetItem(i);

            if (MgdLogManager::ClientParam == param)
            {
                AddClient(logEntry, client);
            }
            else if (MgdLogManager::ClientIpParam == param)
            {
                AddClientIp(logEntry, clientIp);
            }
            else if (MgdLogManager::UserParam == param)
            {
                AddUserName(logEntry, userName);
            }
            else if (MgdLogManager::OpIdParam == param)
            {
                AddOpId(logEntry, opId);
            }
        }
    }
    else
    {
        // No parameters specified
        // Use default message with just opId
        AddOpId(logEntry, opId);
    }

    MG_LOGMANAGER_CATCH(L"MgdLogManager.LogAccessEntry")

    if (mgException != NULL)
    {
        // Use default message with just opId
        logEntry.clear();
        AddOpId(logEntry, opId);
    }

    QueueLogEntry(mltAccess, logEntry, LM_INFO);
}

void MgdLogManager::LogAdminEntry(CREFSTRING opId, CREFSTRING client, CREFSTRING clientIp, CREFSTRING userName)
{
    // Message to be entered into the log
    STRING logEntry;

    MG_LOGMANAGER_TRY()

    // Parse parameter string into an MgStringCollection
    Ptr<MgStringCollection> paramList = MgStringCollection::ParseCollection(
        GetAdminLogParameters().c_str(), L",");

    // Go through parameter list and add the information appropriately
    if (paramList != NULL)
    {
        INT32 numParams = paramList->GetCount();
        STRING param;

        for (INT32 i = 0; i < numParams; ++i)
        {
            param = paramList->GetItem(i);

            if (MgdLogManager::ClientParam == param)
            {
                AddClient(logEntry, client);
            }
            else if (MgdLogManager::ClientIpParam == param)
            {
                AddClientIp(logEntry, clientIp);
            }
            else if (MgdLogManager::UserParam == param)
            {
                AddUserName(logEntry, userName);
            }
            else if (MgdLogManager::OpIdParam == param)
            {
                AddOpId(logEntry, opId);
            }
        }
    }
    else
    {
        // No parameters specified
        // Use default message with just opId
        AddOpId(logEntry, opId);
    }

    MG_LOGMANAGER_CATCH(L"MgdLogManager.LogAdminEntry")

    if (mgException != NULL)
    {
        // Use default message with just opId
        logEntry.clear();
        AddOpId(logEntry, opId);
    }

    QueueLogEntry(mltAdmin, logEntry, LM_INFO);
}

void MgdLogManager::LogAuthenticationEntry(CREFSTRING entry, CREFSTRING client, CREFSTRING clientIp, CREFSTRING userName)
{
    // Message to be entered into the log
    STRING logEntry;

    MG_LOGMANAGER_TRY()

    // Parse parameter string into an MgStringCollection
    Ptr<MgStringCollection> paramList = MgStringCollection::ParseCollection(
        GetAuthenticationLogParameters().c_str(), L",");

    // Go through parameter list and add the information appropriately
    if (paramList != NULL)
    {
        INT32 numParams = paramList->GetCount();
        STRING param;

        for (INT32 i = 0; i < numParams; ++i)
        {
            param = paramList->GetItem(i);

            if (MgdLogManager::ClientParam == param)
            {
                AddClient(logEntry, client);
            }
            else if (MgdLogManager::ClientIpParam == param)
            {
                AddClientIp(logEntry, clientIp);
            }
            else if (MgdLogManager::UserParam == param)
            {
                AddUserName(logEntry, userName);
            }
        }
    }

    // Add the given info.
    AddDelimiter(logEntry);
    logEntry += entry;

    MG_LOGMANAGER_CATCH(L"MgdLogManager.LogAuthenticationEntry")

    if (mgException != NULL)
    {
        // Use default message with just the given info.
        // TODO: Create actual default message when it's known what will go in this log
        logEntry = entry;
    }

    QueueLogEntry(mltAuthentication, logEntry, LM_INFO);
}

void MgdLogManager::LogErrorEntry(CREFSTRING entry, CREFSTRING client, CREFSTRING clientIp, CREFSTRING userName, CREFSTRING stackTrace, CREFSTRING type)
{
    // Message to be entered into the log
    STRING logEntry;

    MG_LOGMANAGER_TRY()

    // Current threadid is first parameter
    AddThreadId(logEntry);

    // Parse parameter string into an MgStringCollection
    Ptr<MgStringCollection> paramList = MgStringCollection::ParseCollection(
        GetErrorLogParameters().c_str(), L",");

    // Go through parameter list and add the information appropriately
    if (paramList != NULL)
    {
        INT32 numParams = paramList->GetCount();
        STRING param;

        for (INT32 i = 0; i < numParams; ++i)
        {
            param = paramList->GetItem(i);

            if (MgdLogManager::ClientParam == param)
            {
                AddClient(logEntry, client);
            }
            else if (MgdLogManager::ClientIpParam == param)
            {
                AddClientIp(logEntry, clientIp);
            }
            else if (MgdLogManager::UserParam == param)
            {
                AddUserName(logEntry, userName);
            }
            else if (MgdLogManager::ErrorParam == param)
            {
                AddError(logEntry, entry, type);
            }
            else if (MgdLogManager::StackTraceParam == param)
            {
                AddStackTrace(logEntry, stackTrace);
            }
        }
    }

    // Add the given info.
    AddDelimiter(logEntry);

    MG_LOGMANAGER_CATCH(L"MgdLogManager.LogErrorEntry")

    if (mgException != NULL)
    {
        // Use default message with just the given info.
        // TODO: Create actual default message when it's known what will go in this log
        logEntry = entry;
    }

    QueueLogEntry(mltError, logEntry, LM_ERROR);
}

void MgdLogManager::LogTraceEntry(CREFSTRING entry, CREFSTRING client, CREFSTRING clientIp, CREFSTRING userName, CREFSTRING stackTrace, CREFSTRING type)
{
    // Message to be entered into the log
    STRING logEntry;

    MG_LOGMANAGER_TRY()

    // Current threadid is first parameter
    AddThreadId(logEntry);

    // Parse parameter string into an MgStringCollection
    Ptr<MgStringCollection> paramList = MgStringCollection::ParseCollection(
        GetTraceLogParameters().c_str(), L",");

    // Go through parameter list and add the information appropriately
    if (paramList != NULL)
    {
        INT32 numParams = paramList->GetCount();
        STRING param;

        for (INT32 i = 0; i < numParams; ++i)
        {
            param = paramList->GetItem(i);

            if (MgdLogManager::ClientParam == param)
            {
                AddClient(logEntry, client);
            }
            else if (MgdLogManager::ClientIpParam == param)
            {
                AddClientIp(logEntry, clientIp);
            }
            else if (MgdLogManager::UserParam == param)
            {
                AddUserName(logEntry, userName);
            }
            else if (MgdLogManager::InfoParam == param)
            {
                if (type.compare(L"") == 0)
                {
                    AddInfo(logEntry, entry);
                }
                else
                {
                    AddError(logEntry, entry, type);
                }
             }
            else if (MgdLogManager::StackTraceParam == param)
            {
                if (!logEntry.empty())
                {
                    AddStackTrace(logEntry, stackTrace);
                }
            }
        }
    }

    MG_LOGMANAGER_CATCH(L"MgdLogManager.LogTraceEntry")

    if (mgException != NULL)
    {
        // Use default message with just the given info.
        // TODO: Create actual default message when it's known what will go in this log
        logEntry = entry;
    }

    QueueLogEntry(mltTrace, logEntry, LM_INFO);
}

void MgdLogManager::LogSystemErrorEntry(MgException* except)
{
    if (NULL != except)
    {
        //MgServerManager* serverManager = MgServerManager::GetInstance();
        //ACE_ASSERT(NULL != serverManager);

        //STRING locale = serverManager->GetDefaultMessageLocale();
        STRING locale = MgResources::DefaultMessageLocale;
        STRING message = except->GetExceptionMessage(locale);
        STRING details = except->GetDetails(locale);
        STRING stackTrace = except->GetStackTrace(locale);

        ACE_DEBUG((LM_ERROR, ACE_TEXT("(%t) %W\n"), details.c_str()));
        MG_LOG_SYSTEM_ENTRY(LM_ERROR, details.c_str());
        MG_LOG_EXCEPTION_ENTRY(message.c_str(), stackTrace.c_str());
    }
}

MgPropertyCollection* MgdLogManager::EnumerateLogs()
{
    Ptr<MgPropertyCollection> logs;
    ACE_DIR* directory = NULL;

    MG_LOGMANAGER_TRY()

    logs = new MgPropertyCollection(true, true);

    // Open the logs directory
    directory = ACE_OS::opendir(MG_WCHAR_TO_TCHAR(m_path));
    if (directory == NULL)
    {
        MgStringCollection arguments;
        arguments.Add(m_path);
        throw new MgFileIoException(L"MgdLogManager.EnumerateLogs", __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    dirent* direntry = NULL;

    wstring name;
    STRING path;
    int statResult;
#ifdef _WIN32
    struct _stat statBuf;
#else
    struct stat statBuf;
#endif

    // Go through the directory entries
    Ptr<MgStringProperty> pProperty;
    while ((direntry = ACE_OS::readdir(directory)) != NULL)
    {
        name = MG_TCHAR_TO_WCHAR(direntry->d_name);
        path = m_path + name;

#ifdef _WIN32
        statResult = ::_wstat(path.c_str(), &statBuf);
#else
        statResult = ::stat(MgUtil::WideCharToMultiByte(path).c_str(),
            &statBuf);
#endif
        // Add to list of log files if it is a file and not a folder
        if (statResult == 0 && (statBuf.st_mode & S_IFREG))
        {
            pProperty = new MgStringProperty(MgdLogManager::LogNameProperty, name);
            logs->Add(pProperty);

            // Is the log in use?
            enum MgLogType logType = mltSystem;
            bool bInUse = IsLogFileInUse(name, logType);
            if(bInUse)
            {
                DisableLog(logType);
            }

            // Add the log type
            wstring type = ReadLogTypeFromLogFile(path);
            pProperty = new MgStringProperty(MgdLogManager::LogTypeProperty, type);
            logs->Add(pProperty);

            if(bInUse)
            {
                EnableLog(logType);
            }

            // Add the log status
            wstring status = DetermineLogFileStatus(name, type);
            pProperty = new MgStringProperty(MgdLogManager::LogStatusProperty, status);
            logs->Add(pProperty);
        }
    }

    ACE_OS::closedir(directory);

    MG_LOGMANAGER_CATCH(L"MgdLogManager.EnumerateLogs")
    if (mgException != NULL)
    {
        if (directory != NULL)
        {
            ACE_OS::closedir(directory);
        }
        (*mgException).AddRef();
        mgException->Raise();
    }

    return logs.Detach();
}


void MgdLogManager::RenameLog(CREFSTRING oldFileName, CREFSTRING newFileName)
{
    if (oldFileName.empty() || newFileName.empty())
    {
        throw new MgNullArgumentException(L"MgdLogManager.RenameLog", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MG_LOGMANAGER_TRY()

    ACE_MT (ACE_GUARD (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex));

    enum MgLogType logType = mltSystem;
    bool bInUse = IsLogFileInUse(oldFileName, logType);
    if(bInUse)
    {
        DisableLog(logType);
    }

    // Rename the log file
    MgFileUtil::RenameFile(m_path, oldFileName, newFileName);

    if(bInUse)
    {
        EnableLog(logType);
    }

    MG_LOGMANAGER_CATCH_AND_THROW(L"MgdLogManager.RenameLog")
}

void MgdLogManager::DeleteLog(CREFSTRING fileName)
{
    if (fileName.empty())
    {
        throw new MgNullArgumentException(L"MgdLogManager.DeleteLog", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    if (STRING::npos != fileName.find(L"\\") ||
        STRING::npos != fileName.find(L"/"))
    {
        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(fileName);

        MgStringCollection whyArguments;
        whyArguments.Add(L"\\/");

        throw new MgInvalidArgumentException(L"MgdLogManager.DeleteLog",
            __LINE__, __WFILE__, &arguments, L"MgStringContainsReservedCharacters", &whyArguments);
    }

    MG_LOGMANAGER_TRY()

    ACE_MT (ACE_GUARD (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex));

    enum MgLogType logType = mltSystem;
    bool bInUse = IsLogFileInUse(fileName, logType);
    if(bInUse)
    {
        DisableLog(logType);
    }

    MgFileUtil::DeleteFile((STRING)m_path.c_str()+(STRING)fileName.c_str());

    if(bInUse)
    {
        EnableLog(logType);
    }

    MG_LOGMANAGER_CATCH_AND_THROW(L"MgdLogManager.DeleteLog")
}

void MgdLogManager::QueueLogEntry(enum MgLogType logType, CREFSTRING message, ACE_Log_Priority logPriority)
{
    // Do NOT queue the log entry if the Log Manager has not been initialized.
    // This is likely due to problems with the server configuration.
    if (NULL == m_pLogThread)
    {
        return;
    }

    MG_LOGMANAGER_TRY()

    // We want the log thread to handle the log entry for us
    MgdLogEntryData* led = NULL;
    ACE_Allocator* allocator = ACE_Allocator::instance();
    ACE_NEW_MALLOC_NORETURN (led,
        static_cast<MgdLogEntryData*> (allocator->malloc(sizeof(MgdLogEntryData))),
        MgdLogEntryData(logType, message, logPriority) );

    //ACE_NEW_NORETURN( led, MgdLogEntryData(logType, message, logPriority) );

    ACE_Message_Block* mb;
    ACE_NEW_NORETURN( mb, ACE_Message_Block( led ) );
    if(mb)
    {
        mb->msg_type(ACE_Message_Block::MB_DATA);
        int nResult = m_pLogThread->putq(mb);
        if(nResult == -1)
        {
            // Failed to queue the message
            STRING messageId;
            MgStringCollection arguments;

            arguments.Add(L"Failed to queue ACE_Message_Block.");
            messageId = L"MgFormatInnerExceptionMessage";

            MgException* mgException = new MgRuntimeException(L"MgdLogManager.QueueLogEntry", __LINE__, __WFILE__, NULL, messageId, &arguments);
            throw mgException;
        }
    }

    MG_LOGMANAGER_CATCH_AND_THROW(L"MgdLogManager.QueueLogEntry")
}

void MgdLogManager::StopLogThread()
{
    // Do NOT stop the log thread if the Log Manager has not been initialized.
    // This is likely due to problems with the server configuration.
    if (NULL != m_pLogThread)
    {
        // Tell the log thread to stop
        ACE_Message_Block* mb = new ACE_Message_Block(4);
        if(mb)
        {
            mb->msg_type(ACE_Message_Block::MB_STOP);
            m_pLogThread->putq(mb);
        }

        // Wait for thread to process STOP
        m_pLogThread->wait();
    }

    m_threadManager.wait(0,1);
    m_threadManager.close();
}

void MgdLogManager::WriteLogMessage(enum MgLogType logType, CREFSTRING message, ACE_Log_Priority logPriority)
{
    ACE_MT (ACE_GUARD (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex));

    ACE_Log_Msg* pAce = ACE_Log_Msg::instance();

    // System message gets logged to event log
    if (logType == mltSystem)
    {
        pAce->acquire();

        MG_LOGMANAGER_TRY()

        string appName = MgUtil::WideCharToMultiByte(m_applicationName);
        wchar_t* messageStr = (wchar_t*) message.c_str();

        LogToSysLog(pAce, (char *)appName.c_str());
#ifdef _WIN32
        pAce->log(logPriority, ACE_TEXT("%Z\r\n"), message.c_str()); // WAS %W
#else
        pAce->log(logPriority, ACE_TEXT("%Z\n"), messageStr); // WAS %W
#endif
        LogToStderr(pAce);

        MG_LOGMANAGER_CATCH(L"MgdLogManager.WriteLogMessage")

        if (mgException != NULL)
        {
            STRING entry = L"Unable to log system message";
            LogError(entry, L"", L"", L"");
        }

        pAce->release();
    }
    // All other messages will get logged to their respective files
    else
    {
        std::ofstream* pLogStream = NULL;

        bool bEnabled = false;
        STRING filename;
        STRING logTypeName;
        STRING logParameters;

        switch (logType)
        {
        case mltAccess:
            // Get the access log filename and path
            filename = BuildFileName(m_AccessLogFileName);
            logTypeName = MgdLogManager::AccessLog;
            logParameters = GetAccessLogParameters();
            pLogStream = &m_accessLogStream;
            bEnabled = m_bAccessLogEnabled;
            break;
        case mltAdmin:
            // Get the admin log filename and path
            filename = BuildFileName(m_AdminLogFileName);
            logTypeName = MgdLogManager::AdminLog;
            logParameters = GetAdminLogParameters();
            pLogStream = &m_adminLogStream;
            bEnabled = m_bAdminLogEnabled;
            break;
        case mltAuthentication:
            // Get the authentication log filename and path
            filename = BuildFileName(m_AuthenticationLogFileName);
            logTypeName = MgdLogManager::AuthenticationLog;
            logParameters = GetAuthenticationLogParameters();
            pLogStream = &m_authenticationLogStream;
            bEnabled = m_bAuthenticationLogEnabled;
            break;
        case mltError:
            // Get the error log filename and path
            filename = BuildFileName(m_ErrorLogFileName);
            logTypeName = MgdLogManager::ErrorLog;
            logParameters = GetErrorLogParameters();
            pLogStream = &m_errorLogStream;
            bEnabled = m_bErrorLogEnabled;
            break;
        case mltPerformance:
            // Get the performance log filename and path
            filename = BuildFileName(m_PerformanceLogFileName);
            logTypeName = MgdLogManager::PerformanceLog;
            logParameters = GetPerformanceLogParameters();
            pLogStream = &m_performanceLogStream;
            bEnabled = m_bPerformanceLogEnabled;
            break;
        case mltSession:
            // Get the session log filename and path
            filename = BuildFileName(m_SessionLogFileName);
            logTypeName = MgdLogManager::SessionLog;
            logParameters = GetSessionLogParameters();
            pLogStream = &m_sessionLogStream;
            bEnabled = m_bSessionLogEnabled;
            break;
        case mltTrace:
            // Get the trace log file and path
            filename = BuildFileName(m_TraceLogFileName);
            logTypeName = MgdLogManager::TraceLog;
            logParameters = GetTraceLogParameters();
            pLogStream = &m_traceLogStream;
            bEnabled = m_bTraceLogEnabled;
            break;
        default:
            {
                STRING buffer;
                MgUtil::Int32ToString(logType, buffer);

                MgStringCollection arguments;
                arguments.Add(L"1");
                arguments.Add(buffer);

                throw new MgInvalidArgumentException(L"MgdLogManager.WriteLogMessage",
                    __LINE__, __WFILE__, &arguments, L"MgInvalidLogType", NULL);
            }
        }

        // Check to see if the log is still open
        if(bEnabled)
        {
            if(pLogStream)
            {
                pAce->acquire();

                MG_LOGMANAGER_TRY()

                // This is an expensive check, only do it occasionally
                // First time, cache all the error log time stamps, then cache is updated when archive is created.
                if (0 == m_writeCount)
                {
                    UpdateLogFilesTimestampCache();
                }

                // CheckArchiveFrequency for every write
                m_writeCount++;
                if (false == CheckArchiveFrequency(logType, filename))
                {
                    ArchiveLog(logType);
                }

                if (!pLogStream->is_open())
                {
                    MgStringCollection arguments;
                    arguments.Add(filename);
                    throw new MgFileIoException(L"MgdLogManager.WriteLogMessage", __LINE__, __WFILE__, &arguments, L"", NULL);
                }

                LogToOStream(pAce, pLogStream);

                if(!LogHasHeader(logType))
                {
                    // Write file header information if the log is empty
                    bool bLogFileEmpty = true;
                    bLogFileEmpty =  (MgFileUtil::GetFileSize(filename) ? false : true);
                    if (bLogFileEmpty)
                    {
                        STRING headerLine1 = MgdLogManager::HeaderLine1 + L" ";
                        headerLine1 += logTypeName;
                        STRING headerLine2 = MgdLogManager::HeaderLine2 + L" ";
                        headerLine2 += logParameters;
#ifdef _WIN32
                        pAce->log(logPriority, ACE_TEXT("%W\r\n"), headerLine1.c_str());
                        pAce->log(logPriority, ACE_TEXT("%W\r\n"), headerLine2.c_str());
#else
                        pAce->log(logPriority, ACE_TEXT("%W\n"), headerLine1.c_str());
                        pAce->log(logPriority, ACE_TEXT("%W\n"), headerLine2.c_str());
#endif
                        SetLogHasHeader(logType, true);
                    }  // end write file header
                }

                // Get the current time in XML standard format.
                MgDateTime currTime;
                if (mltTrace != logType)
                {
                    // Generate sub-second timing only for trace log
                    currTime.SetMicrosecond(0);
                }
                STRING logTime = currTime.ToXmlString(false);

#ifdef _WIN32
                pAce->log(logPriority, ACE_TEXT("<%W> %W\r\n"), logTime.c_str(), message.c_str());
#else
                pAce->log(logPriority, ACE_TEXT("<%W> %W\n"), logTime.c_str(), message.c_str());
#endif
                pLogStream->flush();
                LogToStderr(pAce);

                MG_LOGMANAGER_CATCH(L"MgdLogManager.WriteLogMessage")

                if (mgException != 0 && logType != mltError)
                {
                    STRING entry = L"Unable to log message to " + filename;
                    LogError(entry, L"", L"", L"");
                }

                // Check if log size has exceeded the maximum size
                if (IsMaximumLogSizeEnabled() && IsMaxSizeExceeded(filename))
                {
                    // Archive the current log
                    ArchiveLog(logType);
                }

                pAce->release();
            }
        }
    } // end else
}

bool MgdLogManager::RemoveLogFile(CREFSTRING filename)
{
    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex, false));

    // Does the file exist? If so remove it.
    int nResult = ACE_OS::unlink(MG_WCHAR_TO_TCHAR(filename));
    if(nResult == -1)
    {
        // Check to see if the error is a result of the file not found.
        if(ENOENT == errno)
        {
            // File not found, so this method was successful as there was no log to remove
            nResult = 0;
        }
    }

    return (nResult == 0) ? true : false;
}

MgByteReader* MgdLogManager::GetLogHeader(enum MgLogType logType)
{
    Ptr<MgByteReader> byteReader;
    byteReader = NULL;

    MG_LOGMANAGER_TRY()

    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex, NULL));

    STRING filename = L"";

    switch(logType)
    {
    case mltAccess:
        filename = BuildFileName(m_AccessLogFileName);
        break;
    case mltAdmin:
        filename = BuildFileName(m_AdminLogFileName);
        break;
    case mltAuthentication:
        filename = BuildFileName(m_AuthenticationLogFileName);
        break;
    case mltError:
        filename = BuildFileName(m_ErrorLogFileName);
        break;
    case mltPerformance:
        filename = BuildFileName(m_PerformanceLogFileName);
        break;
    case mltSession:
        filename = BuildFileName(m_SessionLogFileName);
        break;
    case mltTrace:
        filename = BuildFileName(m_TraceLogFileName);
        break;
    default:
        {
            STRING buffer;
            MgUtil::Int32ToString(logType, buffer);

            MgStringCollection arguments;
            arguments.Add(L"1");
            arguments.Add(buffer);

            throw new MgInvalidArgumentException(L"MgdLogManager.GetLogHeader",
                __LINE__, __WFILE__, &arguments, L"MgInvalidLogType", NULL);
        }
    }

    bool bInUse = IsLogInUse(logType);
    if(bInUse)
    {
        DisableLog(logType);
    }

    byteReader = GetLogHeader(filename);

    if(bInUse)
    {
        EnableLog(logType);
    }

    MG_LOGMANAGER_CATCH(L"MgdLogManager.GetLogHeader")

    return byteReader.Detach();
}

MgByteReader* MgdLogManager::GetLogHeader(CREFSTRING filename)
{
    Ptr<MgByteReader> byteReader;
    byteReader = NULL;
    FILE* pReadFile = NULL;
    string contents = "";

    MG_LOGMANAGER_TRY()

    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex, NULL));

    pReadFile = ACE_OS::fopen(MG_WCHAR_TO_TCHAR(filename), ACE_TEXT("rb"));

    if (pReadFile != NULL)
    {
        const int size = 2048;
        char buffer[2048] = {0};

        ACE_OS::fread(buffer, sizeof(char), size-1, pReadFile);
        contents = buffer;

        ACE_OS::fclose(pReadFile);
    }

    byteReader = MgUtil::GetByteReader(contents);

    MG_LOGMANAGER_CATCH(L"MgdLogManager.GetLogHeader")
    if (mgException != NULL)
    {
        if (pReadFile != NULL)
        {
            ACE_OS::fclose(pReadFile);
        }
        (*mgException).AddRef();
        mgException->Raise();
    }

    return byteReader.Detach();
}

MgByteReader* MgdLogManager::GetLogContents(CREFSTRING filename)
{
    Ptr<MgByteReader> byteReader;
    byteReader = NULL;
    FILE* pReadFile = NULL;
    string contents = "";

    MG_LOGMANAGER_TRY()

    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex, NULL));

    pReadFile = ACE_OS::fopen(MG_WCHAR_TO_TCHAR(filename), ACE_TEXT("rb"));

    if (pReadFile != NULL)
    {
        ACE_OS::fseek(pReadFile, 0, SEEK_END);
        INT32 length = ACE_OS::ftell(pReadFile);
        ACE_OS::fseek(pReadFile, 0, SEEK_SET);

        //TODO: do we need a maximum size restriction?
        char* buffer = new char[length+1];

        if (buffer == NULL)
        {
            ACE_OS::fclose(pReadFile);
            throw new MgOutOfMemoryException(L"MgdLogManager.GetLogContents", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        // Clear buffer
        memset(buffer, 0, (length+1)*sizeof(char));

        ACE_OS::fread(buffer, sizeof(char), length*sizeof(char), pReadFile);
        contents = buffer;

        // Clean up buffer
        delete [] buffer;
        buffer = NULL;

        ACE_OS::fclose(pReadFile);
    }

    byteReader = MgUtil::GetByteReader(contents);

    MG_LOGMANAGER_CATCH(L"MgdLogManager.GetLogContents")
    if (mgException != NULL)
    {
        if (pReadFile != NULL)
        {
            ACE_OS::fclose(pReadFile);
        }
        (*mgException).AddRef();
        mgException->Raise();
    }

    return byteReader.Detach();
}

MgByteReader* MgdLogManager::GetLogContents(CREFSTRING filename, INT32 numEntries)
{
    Ptr<MgByteReader> byteReader;
    byteReader = NULL;
    FILE* pReadFile = NULL;
    Ptr<MgByteSource> byteSource;
    byteSource = NULL;
    string contents = "";

    MG_LOGMANAGER_TRY()

    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex, NULL));

    if (numEntries < 0)
    {
        throw new  MgArgumentOutOfRangeException(L"MgdLogManager.GetLogContents", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    INT32 lastPos;      // Keeps track of the position in the file
    INT32 numNewLines;  // Keep track of the number of entries found so far
    char buffer[MAX_BUF+1];
    bool bReadMore;

    pReadFile = ACE_OS::fopen(MG_WCHAR_TO_TCHAR(filename), ACE_TEXT("rb"));
    if (pReadFile != NULL)
    {
        ACE_OS::fseek(pReadFile, 0, SEEK_END);
        lastPos = ACE_OS::ftell(pReadFile);

        INT32 loopCount = 1;
        numNewLines = 0;
        bReadMore = true;

        bool lookForNewLine = false;
        INT32 strPos = 0;

        while (bReadMore)
        {
            // Clear buffer
            memset(buffer, 0, (MAX_BUF+1)*sizeof(char));

            // Place file pointer to fill an array with MAX_BUF characters of file starting from
            // the end of the file.
            if (lastPos > MAX_BUF)
            {
                ACE_OS::fseek(pReadFile, -(loopCount*MAX_BUF), SEEK_END);
                lastPos = ACE_OS::ftell(pReadFile);
                ACE_OS::fread(buffer, sizeof(char), MAX_BUF*sizeof(char), pReadFile);

                // Search through file backwards to count entries
                // Each entry starts with a '<' after a new line
                for (strPos = MAX_BUF; strPos >= 0; strPos--)
                {
                    if (lookForNewLine && '\n' == buffer[strPos])
                    {
                        numNewLines++;
                    }
                    lookForNewLine = false;

                    if ('<' == buffer[strPos])
                    {
                        lookForNewLine = true;
                    }
                    // found the number of entries wanted
                    if (numNewLines == numEntries)
                    {
                        bReadMore = false;
                        break;
                    }
                }
            }
            // Place file pointer at beginning of file and fill the buffer with as
            // much as needed.  Also ensure that each part of the file is not read more than
            // once.
            else
            {
                ACE_OS::fseek(pReadFile, 0, SEEK_SET);
                ACE_OS::fread(buffer, sizeof(char), lastPos*sizeof(char), pReadFile);

                for (strPos = lastPos; strPos >= 0; strPos--)
                {
                    if (lookForNewLine && '\n' == buffer[strPos])
                    {
                        numNewLines++;
                    }
                    lookForNewLine = false;

                    if ('<' == buffer[strPos])
                    {
                        lookForNewLine = true;
                    }
                    // found the number of entries wanted
                    if (numNewLines == numEntries)
                    {
                        break;
                    }
                }

                // set readmore to false since there is nothing left to read
                bReadMore = false;
            }

            contents = &buffer[++strPos] + contents;
            loopCount++;
        }
        ACE_OS::fclose(pReadFile);
    }

    byteReader = MgUtil::GetByteReader(contents);

    MG_LOGMANAGER_CATCH(L"MgdLogManager.GetLogContents")
    if (mgException != NULL)
    {
        if (pReadFile != NULL)
        {
            ACE_OS::fclose(pReadFile);
        }
        (*mgException).AddRef();
        mgException->Raise();
    }

    return byteReader.Detach();
}

MgByteReader* MgdLogManager::GetLogContents(enum MgLogType logType, MgDateTime* fromDate, MgDateTime* toDate)
{
    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex, NULL));

    if (fromDate == NULL || toDate == NULL)
    {
        throw new MgNullArgumentException(L"MgdLogManager.GetLogContents", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    if (*fromDate > *toDate)
    {
        MgStringCollection arguments;
        arguments.Add(L"2");
        arguments.Add(fromDate->ToString());

        throw new MgInvalidArgumentException(L"MgdLogManager.GetLogContents",
            __LINE__, __WFILE__, &arguments, L"MgInvalidFromDate", NULL);
    }

    if (IsMoreThan24HourDiff(fromDate, toDate))
    {
        MgStringCollection arguments;
        arguments.Add(L"2");
        arguments.Add(fromDate->ToString());
        arguments.Add(L"3");
        arguments.Add(toDate->ToString());

        throw new MgInvalidArgumentException(L"MgdLogManager.GetLogContents",
            __LINE__, __WFILE__, &arguments, L"MgInvalidDateDifference", NULL);
    }

    Ptr<MgByteReader> byteReader;
    byteReader = NULL;
    FILE* pReadFile = NULL;
    string contents = "";

    MG_LOGMANAGER_TRY()

    Ptr<MgStringCollection> filenames;
    filenames = NULL;
    Ptr<MgStringCollection> entries;
    entries = NULL;

    STRING currentFilename;

    // Get the potential file names
    filenames = DeterminePotentialFileNames(logType, fromDate, toDate);
    if (filenames->GetCount() < 1)
    {
        MgStringCollection arguments;
        arguments.Add(L"2");
        arguments.Add(fromDate->ToString());
        arguments.Add(L"3");
        arguments.Add(toDate->ToString());

        throw new MgInvalidArgumentException(L"MgdLogManager.GetLogContents",
             __LINE__, __WFILE__, &arguments, L"MgFailedToGetFileNameForDates", NULL);
    }

    entries = new MgStringCollection();
    if (entries == NULL)
    {
        throw new MgOutOfMemoryException(L"MgdLogManager.GetLogContents", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    bool fromDateFound = false;
    bool toDateFound = false;

    INT32 fromLine = 0;
    INT32 toLine = 0;
    INT32 i = 0;
    INT32 j = 0;

    char buffer[MAX_BUF+1];
    string logEntry = "";                // current entry read from file
    STRING wEntry = L"";

    INT32 nSearchedFiles = 0;
    while (nSearchedFiles < filenames->GetCount())
    {
        currentFilename = filenames->GetItem(nSearchedFiles);
        nSearchedFiles++;   // increment the number of searched files

        pReadFile = ACE_OS::fopen(MG_WCHAR_TO_TCHAR(currentFilename), ACE_TEXT("rb"));
        if (pReadFile != NULL)
        {
            INT32 nNumRead = 0;

            // Keep reading lines from the file in a chunk until no more to read or
            // both dates have been found
            bool bReadMore = true;
            while (bReadMore)
            {
                // Clear buffer
                memset(buffer, 0, (MAX_BUF+1)*sizeof(char));
                entries->Clear();

                fromLine = 0;
                toLine = 0;

                size_t charsRead = ACE_OS::fread(buffer, sizeof(char), MAX_BUF*sizeof(char), pReadFile);
                if (charsRead < MAX_BUF)
                {
                    bReadMore = false;
                    nNumRead = (INT32)charsRead;
                }
                else
                {
                    nNumRead = MAX_BUF;
                }

                char* entryBegin = buffer;
                for (i = 0; i < nNumRead; i++)
                {
                    if (i == 0)
                    {
                        if (logEntry.size() > 0 && logEntry[logEntry.size()-1] == '\n' && buffer[i] == '<')
                        {
                            wEntry = MgUtil::MultiByteToWideChar(logEntry);
                            logEntry.erase();
                            entries->Add(wEntry);
                        }
                    }
                    else if (buffer[i-1] == '\n' && buffer[i] == '<')
                    {
                        char tmp = buffer[i];
                        buffer[i] = '\0';

                        logEntry += entryBegin;
                        wEntry = MgUtil::MultiByteToWideChar(logEntry);
                        logEntry.erase();

                        // Skip over the header entry if it is present.  Header lines start with the '#' character.
                        if (wEntry.find_first_of('#') != 0)
                        {
                            entries->Add(wEntry);
                        }

                        buffer[i] = tmp;
                        entryBegin = buffer + i;
                    }
                }
                logEntry += entryBegin;

                if (!bReadMore)
                {
                    wEntry = MgUtil::MultiByteToWideChar(logEntry);
                    logEntry = "";
                    entries->Add(wEntry);
                }

                if (entries->GetCount() != 0)
                {
                    // fromDate hasn't been found yet so we must search for it
                    if (!fromDateFound)
                    {
                        fromLine = SearchClosestDateAfter(entries, fromDate);
                        if (fromLine != -1)
                        {
                            fromDateFound = true;
                        }
                    }

                    // fromDate has been found so we must search for the toDate now
                    if (fromDateFound && !toDateFound)
                    {
                        toLine = SearchClosestDateBefore(entries, toDate);
                        if (toLine == -1)
                        {
                            toDateFound = true;
                        }
                    }

                    if (fromLine != -1 && toLine != -1)
                    {
                        // Put this chunk into the content string.
                        for (j = fromLine; j <= toLine; j++)
                        {
                            contents += MgUtil::WideCharToMultiByte(entries->GetItem(j));
                        }
                    }
                }

                // Both dates have been found so we can now stop
                if (toDateFound && fromDateFound)
                {
                    bReadMore = false;
                }
            } // End of file reading while loop

            ACE_OS::fclose(pReadFile);
        } // End if file open

        // Both dates have been found so we can now stop
        if (toDateFound && fromDateFound)
        {
            break;
        }
    } // End of file selecting while loop

    // Put string into the byteReader that will be sent back
    byteReader = MgUtil::GetByteReader(contents);

    MG_LOGMANAGER_CATCH(L"MgdLogManager.GetLogContents");
    if (mgException != NULL)
    {
        if (pReadFile != NULL)
        {
            ACE_OS::fclose(pReadFile);
        }
        (*mgException).AddRef();
        mgException->Raise();
    }

    return byteReader.Detach();
}

MgStringCollection* MgdLogManager::DeterminePotentialFileNames(enum MgLogType logType, MgDateTime* fromDate, MgDateTime* toDate)
{
    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex, NULL));

    Ptr<MgStringCollection> filenames;

    filenames = new MgStringCollection();

    if (filenames == NULL)
    {
        throw new MgOutOfMemoryException(L"MgdLogManager.GetLogContents", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    STRING rawFilename;

    // Determine the possible filenames that the logs might have
    switch (logType)
    {
    case mltAccess:
        rawFilename = m_AccessLogFileName;
        break;
    case mltAdmin:
        rawFilename = m_AdminLogFileName;
        break;
    case mltAuthentication:
        rawFilename = m_AuthenticationLogFileName;
        break;
    case mltError:
        // Get the error log filename and path
        rawFilename = m_ErrorLogFileName;
        break;
    case mltPerformance:
        rawFilename = m_PerformanceLogFileName;
        break;
    case mltSession:
        rawFilename = m_SessionLogFileName;
        break;
    case mltTrace:
        rawFilename = m_TraceLogFileName;
        break;
    default:
        {
            STRING buffer;
            MgUtil::Int32ToString(logType, buffer);

            MgStringCollection arguments;
            arguments.Add(L"1");
            arguments.Add(buffer);

            throw new MgInvalidArgumentException(L"MgdLogManager.DeterminePotentialFileNames",
                __LINE__, __WFILE__, &arguments, L"MgInvalidLogType", NULL);
        }
    }

    // Name is static so can be used directly
    if (rawFilename.find_first_of(L'%') == string::npos)
    {
        filenames->Add(m_path + rawFilename);
    }
    // Name is dynamic so there may be up to two files that contain the
    // entriest requested.
    else
    {
        STRING filename = BuildFileNameFromDateTime(rawFilename, fromDate);
        filenames->Add(filename);

        // Request spans across multiple days so more than one file may be needed
        INT8 day1 = fromDate->GetDay();
        INT8 day2 = toDate->GetDay();
        if (day1 != day2 && rawFilename.find(L"%d", 0) != string::npos)
        {
            filename = BuildFileNameFromDateTime(rawFilename, toDate);
            filenames->Add(filename);
        }
    } // End of file name determination

    return filenames.Detach();
}

STRING MgdLogManager::BuildFileName(CREFSTRING filename)
{
    STRING newFilename = filename.c_str();

    newFilename = RemoveArchiveFrequencySpecifier(newFilename);

    return ((STRING)m_path.c_str() + (STRING)newFilename.c_str());
}

STRING MgdLogManager::BuildFileNameFromDateTime(CREFSTRING filename, MgDateTime* date)
{
    STRING newFilename = filename.c_str();
    STRING replacer;
    wchar_t buff[3];

    // replace %y with last 2 digits of year
    INT16 year = date->GetYear();
    year %= 100;
    ACE_OS::itoa(year, buff, 10);
    replacer = buff;
    if (year < 10)
    {
        replacer = L'0' + replacer;
    }
    else
    {
        replacer = buff;
    }
    newFilename = MgUtil::ReplaceString(newFilename, L"%y", replacer.c_str());

    // replace %m with month
    INT8 month = date->GetMonth();
    ACE_OS::itoa(month, buff, 10);
    replacer = buff;
    if (month < 10)
    {
        replacer = L'0' + replacer;
    }
    else
    {
        replacer = buff;
    }
    newFilename = MgUtil::ReplaceString(newFilename, L"%m", replacer.c_str());

    // replace %d with day
    INT8 day = date->GetDay();
    ACE_OS::itoa(day, buff, 10);
    replacer = buff;
    if (month < 10)
    {
        replacer = L'0' + replacer;
    }
    else
    {
        replacer = buff;
    }
    newFilename = MgUtil::ReplaceString(newFilename, L"%d", replacer.c_str());

    return ((STRING)m_path.c_str() + (STRING)newFilename.c_str());
}

bool MgdLogManager::IsMoreThan24HourDiff(MgDateTime* fromDate, MgDateTime* toDate)
{
    bool result = false;
    double timeDiff = ACE_OS::difftime(toDate->ToTimeValue(), fromDate->ToTimeValue());

    if (abs((int)timeDiff) > SECONDSINDAY)
    {
        result = true;
    }
    else if (abs((int)timeDiff) == SECONDSINDAY)
    {
        if (abs(toDate->GetMicrosecond() - fromDate->GetMicrosecond()) > 0)
        {
            result = true;
        }
        else
        {
            result = false;
        }
    }

    return result;
}

INT32 MgdLogManager::SearchClosestDateAfter(MgStringCollection* lines, MgDateTime* searchDate)
{
    INT32 result = -1;

    if (lines == NULL || searchDate == NULL)
    {
        throw new MgNullArgumentException(L"MgdLogManager.SearchClosestDateAfter", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    Ptr<MgDateTime> dateTime;

    MG_LOGMANAGER_TRY()

    // If nothing is in the collection, the date cannot be found
    if (lines->GetCount() > 0)
    {
        INT32 start = 0;
        INT32 end = lines->GetCount() - 1;
        INT32 key = start + (end-start)/2;
        bool keepGoing = true;

        while (keepGoing)
        {
            if (start == end)
            {
                keepGoing = false;
            }

            dateTime = GetDateTimeFromEntry(lines->GetItem(key));

            if(dateTime)
            {
                // Found date
                if (*dateTime == *searchDate)
                {
                    keepGoing = false;
                }
                else if (*dateTime < *searchDate)
                {
                    start = key + 1;
                }
                else
                {
                    end = key;
                }

                key = start + (end-start)/2;
            }
        }

        if(dateTime)
        {
            // Check to ensure that a date after was actually found
            if (*dateTime >= *searchDate)
            {
                // check to ensure that there are no previous equal entries in date
                Ptr<MgDateTime> dateTime2;
                if (key > 0)
                {
                    dateTime2 = GetDateTimeFromEntry(lines->GetItem(key-1));
                    while (*dateTime == *dateTime2)
                    {
                        key--;
                        if (key == 0)
                        {
                            break;
                        }
                        dateTime2 = GetDateTimeFromEntry(lines->GetItem(key-1));
                    }
                }
                result = key;
            }
        }
    }

    MG_LOGMANAGER_CATCH_AND_THROW(L"MgdLogManager.SearchClosestDateAfter");

    return result;
}

INT32 MgdLogManager::SearchClosestDateBefore(MgStringCollection* lines, MgDateTime* searchDate)
{
    INT32 result = -1;

    if (lines == NULL || searchDate == NULL)
    {
        throw new MgNullArgumentException(L"MgdLogManager.SearchClosestDateBefore", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    Ptr<MgDateTime> dateTime;

    MG_LOGMANAGER_TRY()

    // If nothing is in the collection, the date cannot be found
    if (lines->GetCount() > 0)
    {
        INT32 start = 0;
        INT32 end = lines->GetCount() - 1;

        INT32 key = end - (end-start)/2;
        bool keepGoing = true;

        while (keepGoing)
        {
            if (start == end)
            {
                keepGoing = false;
            }

            dateTime = GetDateTimeFromEntry(lines->GetItem(key));

            if(dateTime)
            {
                // Found date
                if (*dateTime == *searchDate)
                {
                    keepGoing = false;
                }
                else if (*dateTime < *searchDate)
                {
                    start = key;
                }
                else
                {
                    end = key - 1;
                }

                key = end - (end-start)/2;
            }
        }

        if(dateTime)
        {
            // Check to ensure that a date before was actually found
            if (*dateTime <= *searchDate)
            {
                // Check to ensure that the next entries are not equal in date
                Ptr<MgDateTime> dateTime2;
                if (key < (lines->GetCount() - 1))
                {
                    dateTime2 = GetDateTimeFromEntry(lines->GetItem(key+1));
                    while (*dateTime == *dateTime2)
                    {
                        key++;
                        if (key == (lines->GetCount() - 1))
                        {
                            break;
                        }
                        dateTime2 = GetDateTimeFromEntry(lines->GetItem(key+1));
                    }
                }
                result = key;
            }
        }
    }

    MG_LOGMANAGER_CATCH_AND_THROW(L"MgdLogManager.SearchClosestDateBefore");

    return result;
}

MgDateTime* MgdLogManager::GetDateTimeFromEntry(CREFSTRING entry)
{
    Ptr<MgDateTime> dateTime;

    MG_LOGMANAGER_TRY()

    if(entry.size() == 0)
    {
        return NULL;
    }

    // Each entry starts with the log time that conforms to XML standard and
    // is enclosed by angle brackets, i.e. <CCYY-MM-DDThh:mm:ss>
    if (entry.find_first_of(L'<') != 0  || entry.find_first_of(L'>') != 20)
    {
        throw new MgdInvalidLogEntryException(
            L"MgdLogManager.GetDateTimeFromEntry",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    string logTime;
    MgUtil::WideCharToMultiByte(entry.substr(1, 19), logTime);
    dateTime = new MgDateTime(logTime);

    MG_LOGMANAGER_CATCH_AND_THROW(L"MgdLogManager.GetDateTimeFromEntry");

    return dateTime.Detach();
}

void MgdLogManager::AddDateTime(REFSTRING entry, const MgDateTime& value)
{
    AddDelimiter(entry);
    entry += const_cast<MgDateTime&>(value).ToXmlString(false);
}

void MgdLogManager::AddDouble(REFSTRING entry, double value)
{
    STRING buffer;

    AddDelimiter(entry);
    MgUtil::DoubleToString(value, buffer);
    entry += buffer;
}

void MgdLogManager::AddInt32(REFSTRING entry, INT32 value)
{
    STRING buffer;

    AddDelimiter(entry);
    MgUtil::Int32ToString(value, buffer);
    entry += buffer;
}

void MgdLogManager::AddString(REFSTRING entry, CREFSTRING value)
{
    AddDelimiter(entry);
    entry += value.c_str();
}

void MgdLogManager::AddClient(REFSTRING entry, CREFSTRING client)
{
    AddDelimiter(entry);
    entry += client.c_str();
}

void MgdLogManager::AddClientIp(REFSTRING entry, CREFSTRING clientIp)
{
    AddDelimiter(entry);
    entry += clientIp.c_str();
}

void MgdLogManager::AddError(REFSTRING entry, CREFSTRING error, CREFSTRING type)
{
#ifdef _WIN32
    entry += L"\r\n " + type + L": ";
#else
    entry += L"\n " + type + L": ";
#endif

    size_t size = error.size();

    if (size > 0)
    {
        STRING str = error.c_str();

        // Get rid of extra \n at the end of the stack trace
        if (str[size-1] == L'\n')
        {
            str.erase((size-1), 1);
        }

        // Make it look "pretty"
#ifdef _WIN32
        // Windows uses \r\n for a new line
        str = MgUtil::ReplaceString(str, L"\n", L"\r\n        ");
        entry += str;
#else
        // Linux just uses \n for a new line
        str = MgUtil::ReplaceString(str, L"\n", L"\n        ");
        entry += str;
#endif
    }

}

void MgdLogManager::AddStackTrace(REFSTRING entry, CREFSTRING stackTrace)
{
    // Do not log empty stack traces
    if (stackTrace.empty())
    {
        return;
    }

#ifdef _WIN32
    entry += L"\r\n " + MgResources::StackTrace + L":";
#else
    entry += L"\n " + MgResources::StackTrace + L":";
#endif

    size_t size = stackTrace.size();

    if (size > 0)
    {
        STRING trace = stackTrace.c_str();

        // Get rid of extra \n at the end of the stack trace
        if (trace[size-1] == L'\n')
        {
            trace.erase((size-1), 1);
        }

        // Make it look "pretty"
#ifdef _WIN32
        // Windows uses \r\n for a new line
        trace = MgUtil::ReplaceString(trace, L"\n", L"\r\n  ");
        entry += L"\r\n  " + trace;
#else
        // Linux just uses \n for a new line
        trace = MgUtil::ReplaceString(trace, L"\n", L"\n  ");
        entry += L"\n  " + trace;
#endif
    }
}

void MgdLogManager::AddInfo(REFSTRING entry, CREFSTRING info)
{
    AddDelimiter(entry);
    entry += info.c_str();
}

void MgdLogManager::AddOpId(REFSTRING entry, CREFSTRING opId)
{
    AddDelimiter(entry);
    entry += MgUtil::EncodeXss(opId);
}

void MgdLogManager::AddThreadId(REFSTRING entry)
{
    AddDelimiter(entry);

    ACE_thread_t threadId = ACE_OS::thr_self();
    STRING threadString;
    MgUtil::Int32ToString(threadId, threadString);
    entry += threadString;
}

void MgdLogManager::AddUserName(REFSTRING entry, CREFSTRING userName)
{
    AddDelimiter(entry);
    entry += userName.c_str();
}

bool MgdLogManager::IsMaxSizeExceeded(CREFSTRING logFileName)
{
    bool bMaxSizeReached = false;

    INT64 nFileSize = MgFileUtil::GetFileSize(logFileName);

    if (nFileSize >= (MgdLogManager::m_maxLogSize * 1024))
    {
        bMaxSizeReached = true;
    }

    return bMaxSizeReached;
}

void MgdLogManager::ArchiveLog(enum MgLogType logType)
{
    ACE_MT (ACE_GUARD (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex));

    STRING logFileName = L"";
    std::ofstream* pLogStream = NULL;

    switch(logType)
    {
        case mltAccess:
            logFileName = BuildFileName(m_AccessLogFileName);
            pLogStream = &m_accessLogStream;
            break;
        case mltAdmin:
            logFileName = BuildFileName(m_AdminLogFileName);
            pLogStream = &m_adminLogStream;
            break;
        case mltAuthentication:
            logFileName = BuildFileName(m_AuthenticationLogFileName);
            pLogStream = &m_authenticationLogStream;
            break;
        case mltError:
            logFileName = BuildFileName(m_ErrorLogFileName);
            pLogStream = &m_errorLogStream;
            break;
        case mltPerformance:
            logFileName = BuildFileName(m_PerformanceLogFileName);
            pLogStream = &m_performanceLogStream;
            break;
        case mltSession:
            logFileName = BuildFileName(m_SessionLogFileName);
            pLogStream = &m_sessionLogStream;
            break;
        case mltTrace:
            logFileName = BuildFileName(m_TraceLogFileName);
            pLogStream = &m_traceLogStream;
            break;
        default:
            {
                STRING buffer;
                MgUtil::Int32ToString(logType, buffer);

                MgStringCollection arguments;
                arguments.Add(L"1");
                arguments.Add(buffer);

                throw new MgInvalidArgumentException(L"MgdLogManager.ArchiveLog",
                    __LINE__, __WFILE__, &arguments, L"MgInvalidLogType", NULL);
            }
    }

    if (MgFileUtil::PathnameExists(logFileName))
    {
        if(pLogStream)
        {
            // Is the stream open?
            if(pLogStream->is_open())
            {
                // Close the log file
                pLogStream->close();
            }
        }

        // The archived named will be the current filename suffixed with the current date of the archive + a unique ID
        STRING archiveName = L"";

        // Cache the current extension
        size_t index = logFileName.rfind(L".");
        STRING logFileExt = L"";
        logFileExt = logFileName.substr(index);

        archiveName.clear();
        archiveName = logFileName.substr(0, index);

        // Add the date and time suffix to the archive name.
        ACE_Date_Time date_time;
        INT32 year = (INT32)date_time.year();
        INT32 month = (INT32)date_time.month();
        INT32 day = (INT32)date_time.day();

        STRING uniqueId;
        MgUtil::GenerateUuid(uniqueId);

        STRING strYear;
        STRING strMonth;
        STRING strDay;
        MgUtil::Int32ToString(year, strYear);
        MgUtil::Int32ToString(month, strMonth);
        MgUtil::Int32ToString(day, strDay);

        archiveName.append(L"_");
        archiveName.append(strYear);
        archiveName.append(L"-");
        archiveName.append(strMonth);
        archiveName.append(L"-");
        archiveName.append(strDay);
        archiveName.append(L"_");
        archiveName.append(uniqueId);
        archiveName.append(logFileExt);

        // Create the archive
        MgFileUtil::RenameFile(logFileName, archiveName);

        if(pLogStream)
        {
            // Open the new log file
            pLogStream->open(MgUtil::WideCharToMultiByte(logFileName).c_str(), ios::out | ios::app | ios::binary);

            SetLogHasHeader(logType, false);
        }

        UpdateLogFilesTimestampCache();
    }
}

// If the parameters in the header do not match the current logging parameters, the log is archived
// so that new log will be created based on the current logging parameters.
//
void MgdLogManager::ValidateLogHeaders(enum MgLogType logType)
{
    ACE_MT (ACE_GUARD (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex));

    switch(logType)
    {
       case mltAccess:
            // Check Access Log
            if (!ValidateAccessLogHeader())
            {
                ArchiveLog(logType);
            }
            break;
        case mltAdmin:
            // Check Admin Log
            if (!ValidateAdminLogHeader())
            {
                ArchiveLog(logType);
            }
            break;
        case mltAuthentication:
            // Check Authentication Log
            if (!ValidateAuthenticationLogHeader())
            {
                ArchiveLog(logType);
            }
            break;
        case mltError:
            // Check Error Log
            if (!ValidateErrorLogHeader())
            {
                ArchiveLog(logType);
            }
            break;
        case mltPerformance:
            // Check Performance Log
            if (!ValidatePerformanceLogHeader())
            {
                ArchiveLog(logType);
            }
            break;
        case mltSession:
            // Check Session Log
            if (!ValidateSessionLogHeader())
            {
                ArchiveLog(logType);
            }
            break;
        case mltTrace:
            // Check Trace Log
            if (!ValidateTraceLogHeader())
            {
                ArchiveLog(logType);
            }
            break;
        default:
            {
                STRING buffer;
                MgUtil::Int32ToString(logType, buffer);

                MgStringCollection arguments;
                arguments.Add(L"1");
                arguments.Add(buffer);

                throw new MgInvalidArgumentException(L"MgdLogManager.ValidateLogHeaders",
                    __LINE__, __WFILE__, &arguments, L"MgInvalidLogType", NULL);
            }
    }
}

bool MgdLogManager::ValidateAccessLogHeader()
{
    bool bValid = false;

    MG_LOGMANAGER_TRY()

    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex, false));

    // Compare the log file parameters list with the current logging parameters
    STRING logFileParameters = ReadParametersFromLogFile(mltAccess);
    STRING currentLogParams = GetAccessLogParameters();
    if (0 == logFileParameters.compare(currentLogParams))
    {
        bValid = true;
    }

    MG_LOGMANAGER_CATCH(L"MgdLogManager.ValidateAccessLogHeader")

    return bValid;
}

bool MgdLogManager::ValidateAdminLogHeader()
{
    bool bValid = false;

    MG_LOGMANAGER_TRY()

    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex, false));

    // Compare the log file parameters list with the current logging parameters
    STRING logFileParameters = ReadParametersFromLogFile(mltAdmin);
    STRING currentLogParams = GetAdminLogParameters();
    if (0 == logFileParameters.compare(currentLogParams))
    {
        bValid = true;
    }

    MG_LOGMANAGER_CATCH(L"MgdLogManager.ValidateAdminLogHeader")

    return bValid;
}

bool MgdLogManager::ValidateAuthenticationLogHeader()
{
    bool bValid = false;

    MG_LOGMANAGER_TRY()

    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex, false));

    // Compare the log file parameters list with the current logging parameters
    STRING logFileParameters = ReadParametersFromLogFile(mltAuthentication);
    STRING currentLogParams = GetAuthenticationLogParameters();
    if (0 == logFileParameters.compare(currentLogParams))
    {
        bValid = true;
    }

    MG_LOGMANAGER_CATCH(L"MgdLogManager.ValidateAuthenticationLogHeader")

    return bValid;
}

bool MgdLogManager::ValidateErrorLogHeader()
{
    bool bValid = false;

    MG_LOGMANAGER_TRY()

    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex, false));

    // Compare the log file parameters list with the current logging parameters
    STRING logFileParameters = ReadParametersFromLogFile(mltError);
    STRING currentLogParams = GetErrorLogParameters();
    if (0 == logFileParameters.compare(currentLogParams))
    {
        bValid = true;
    }

    MG_LOGMANAGER_CATCH(L"MgdLogManager.ValidateErrorLogHeader")

    return bValid;
}

bool MgdLogManager::ValidatePerformanceLogHeader()
{
    bool bValid = false;

    MG_LOGMANAGER_TRY()

    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex, false));

    // Compare the log file parameters list with the current logging parameters
    STRING logFileParameters = ReadParametersFromLogFile(mltPerformance);
    STRING currentLogParams = GetPerformanceLogParameters();
    if (0 == logFileParameters.compare(currentLogParams))
    {
        bValid = true;
    }

    MG_LOGMANAGER_CATCH(L"MgdLogManager.ValidatePerformanceLogHeader")

    return bValid;
}

bool MgdLogManager::ValidateSessionLogHeader()
{
    bool bValid = false;

    MG_LOGMANAGER_TRY()

    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex, false));

    // Compare the log file parameters list with the current logging parameters
    STRING logFileParameters = ReadParametersFromLogFile(mltSession);
    STRING currentLogParams = GetSessionLogParameters();
    if (0 == logFileParameters.compare(currentLogParams))
    {
        bValid = true;
    }

    MG_LOGMANAGER_CATCH(L"MgdLogManager.ValidateSessionLogHeader")

    return bValid;
}

bool MgdLogManager::ValidateTraceLogHeader()
{
    bool bValid = false;

    MG_LOGMANAGER_TRY()

    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex, false));

    // Compare the log file parameters list with the current logging parameters
    STRING logFileParameters = ReadParametersFromLogFile(mltTrace);
    STRING currentLogParams = GetTraceLogParameters();
    if (0 == logFileParameters.compare(currentLogParams))
    {
        bValid = true;
    }

    MG_LOGMANAGER_CATCH(L"MgdLogManager.ValidateTraceLogHeader")

    return bValid;
}

STRING MgdLogManager::ReadParametersFromLogFile(enum MgLogType logType)
{
    STRING logParameters = L"";

    MG_LOGMANAGER_TRY()

    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex, NULL));

    Ptr<MgByteReader> bytes;
    bytes = GetLogHeader(logType);

    if (NULL != bytes.p && bytes->GetLength() > 0)
    {
        char buffer[4096] = {0};
        bytes->Read((BYTE_ARRAY_OUT)&buffer, 4095);
        string strBuffer;
        strBuffer.assign(buffer);

        // Find the second line in the log
        size_t startLine2 = strBuffer.find("\n") + 1;
        size_t endLine2 = strBuffer.find("\n", startLine2);
        if (0 != startLine2)
        {
            string strLineBuf = strBuffer.substr(startLine2, endLine2 - startLine2);

            // Check if there is a carriage return (\r) at the end of the line
            size_t endLineBuf = strLineBuf.rfind("\r");
            string strLine2 = strLineBuf.substr(0, endLineBuf);

            // Attempt to extract the parameters list
            string strHeaderPrefix = MgUtil::WideCharToMultiByte(MgdLogManager::HeaderLine2);
            if (0 == strLine2.compare(0, strHeaderPrefix.length(), strHeaderPrefix))
            {
                // The prefix for the header line is OK.  Now lets get the parameters
                string strParams = strLine2.substr(strHeaderPrefix.length() + 1);
                logParameters = MgUtil::MultiByteToWideChar(strParams);

                SetLogHasHeader(logType, true);
            }
        }
    }
    else
    {
        // The file was empty so the parameters have not been written yet
        SetLogHasHeader(logType, false);

        switch (logType)
        {
        case mltAccess:
            logParameters = m_AccessLogParameters;
            break;
        case mltAdmin:
            logParameters = m_AdminLogParameters;
            break;
        case mltAuthentication:
            logParameters = m_AuthenticationLogParameters;
            break;
        case mltError:
            logParameters = m_ErrorLogParameters;
            break;
        case mltPerformance:
            logParameters = m_PerformanceLogParameters;
            break;
        case mltSession:
            logParameters = m_SessionLogParameters;
            break;
        case mltTrace:
            logParameters = m_TraceLogParameters;
            break;
        default:
            break;
        }
    }

    MG_LOGMANAGER_CATCH(L"MgdLogManager.ReadParametersFromLogFile")

    return logParameters;
}

STRING MgdLogManager::ReadLogTypeFromLogFile(CREFSTRING logFilename)
{
    STRING logType = MgdLogManager::UnspecifiedLog;

    MG_LOGMANAGER_TRY()

    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex, NULL));

    Ptr<MgByteReader> bytes;
    bytes = GetLogHeader(logFilename);

    char buffer[4096] = {0};
    bytes->Read((BYTE_ARRAY_OUT)&buffer, 4095);
    string contents;
    contents.assign(buffer);

    if (contents.size() > 0)
    {
        // Find the first line in the log
        size_t startLine1 = 0;
        size_t endLine1 = contents.find("\n");
        if (0 != endLine1)
        {
            string strLineBuf = contents.substr(startLine1, endLine1);

            // Check if there is a carriage return (\r) at the end of the line
            size_t endLineBuf = strLineBuf.rfind("\r");
            string strLine1 = strLineBuf.substr(0, endLineBuf);

            // Attempt to extract the log type
            string strHeaderPrefix = MgUtil::WideCharToMultiByte(MgdLogManager::HeaderLine1);
            if (0 == strLine1.compare(0, strHeaderPrefix.length(), strHeaderPrefix))
            {
                // The prefix for the header line is OK.  Now lets get the type
                string strType = strLine1.substr(strHeaderPrefix.length() + 1);
                logType = MgUtil::MultiByteToWideChar(strType);
            }
        }
    }

    MG_LOGMANAGER_CATCH(L"MgdLogManager.ReadLogTypeFromLogFile")

    return logType;
}

STRING MgdLogManager::DetermineLogFileStatus(CREFSTRING logFilename, CREFSTRING logFileType)
{
    STRING logStatus = L"";

    MG_LOGMANAGER_TRY()

    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex, NULL));

    STRING currentLogName = L"";

    // Get the name being used for the current log
    if (0 == logFileType.compare(MgdLogManager::AccessLog))
    {
        currentLogName = GetAccessLogFileName();
    }
    else if (0 == logFileType.compare(MgdLogManager::AdminLog))
    {
        currentLogName = GetAdminLogFileName();
    }
    else if (0 == logFileType.compare(MgdLogManager::AuthenticationLog))
    {
        currentLogName = GetAuthenticationLogFileName();
    }
    else if (0 == logFileType.compare(MgdLogManager::ErrorLog))
    {
        currentLogName = GetErrorLogFileName();
    }
    else if (0 == logFileType.compare(MgdLogManager::PerformanceLog))
    {
        currentLogName = GetPerformanceLogFileName();
    }
    else if (0 == logFileType.compare(MgdLogManager::SessionLog))
    {
        currentLogName= GetSessionLogFileName();
    }
    else if (0 == logFileType.compare(MgdLogManager::TraceLog))
    {
        currentLogName = GetTraceLogFileName();
    }
    else if (0 == logFileType.compare(MgdLogManager::UnspecifiedLog))
    {
        // The log type is unspecified.  This is most likely an
        // archived log because the header information is not found.
        //
        // Nothing to do here, because the log status will be set to "Archive".
    }
    else
    {
        MgStringCollection arguments;
        arguments.Add(L"2");
        arguments.Add(logFileType);

        throw new MgInvalidArgumentException(L"MgdLogManager.DetermineLogFileStatus",
            __LINE__, __WFILE__, &arguments, L"MgInvalidLogType", NULL);
    }

    currentLogName = RemoveArchiveFrequencySpecifier(currentLogName);

    // Compare the current log name with the filename to determine its status.
    if (0 == logFilename.compare(currentLogName))
    {
        logStatus = MgdLogManager::LogStatusActive;
    }
    else
    {
        logStatus = MgdLogManager::LogStatusArchive;
    }

    MG_LOGMANAGER_CATCH_AND_THROW(L"MgdLogManager.DetermineLogFileStatus")

    return logStatus;
}

void MgdLogManager::AddDelimiter(REFSTRING entry)
{
    entry += m_delimiter;
}

// Check if the delimiter is any one of the reserved characters ( \t \r \n \f \v ).
// If found, it will need to be translated to actually write the character.
void MgdLogManager::TranslateDelimiter()
{
    if (0 == m_delimiter.compare(0, 2, L"\\t"))
    {
        m_delimiter = '\t';
    }
    else if (0 == m_delimiter.compare(0, 2, L"\\r"))
    {
        m_delimiter = '\r';
    }
    else if (0 == m_delimiter.compare(0, 2, L"\\n"))
    {
        m_delimiter = '\n';
    }
    else if (0 == m_delimiter.compare(0, 2, L"\\f"))
    {
        m_delimiter = '\f';
    }
    else if (0 == m_delimiter.compare(0, 2, L"\\v"))
    {
        m_delimiter = '\v';
    }
}

void MgdLogManager::SetMaximumLogSize(INT32 size)
{
    m_maxLogSize = size;
}

void MgdLogManager::SetLogDelimiter(CREFSTRING delimiter)
{
    ACE_MT (ACE_GUARD (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex));

    m_delimiter = delimiter;
}

STRING MgdLogManager::GetLogDelimiter()
{
    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex, STRING(L"")));

    return m_delimiter;
}

void MgdLogManager::EnableMaximumLogSize(bool useMaxSize)
{
    m_useMaxLogSize = useMaxSize;
}

bool MgdLogManager::IsMaximumLogSizeEnabled()
{
    return m_useMaxLogSize;
}

bool MgdLogManager::CheckArchiveFrequency(enum MgLogType logType, CREFSTRING logFilename)
{
    bool bCurrentLog = true;

    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex, false));

    // Get the frequency specifier from the filename, and the filetimestamp from the cache
    STRING rawFilename;
    MgDateTime logTimestamp;

    switch (logType)
    {
    case mltAccess:
        rawFilename = m_AccessLogFileName;
        logTimestamp = m_cacheAccessLogTimestamp;
        break;
    case mltAdmin:
        rawFilename = m_AdminLogFileName;
        logTimestamp = m_cacheAdminLogTimestamp;
        break;
    case mltAuthentication:
        rawFilename = m_AuthenticationLogFileName;
        logTimestamp = m_cacheAuthenticationLogTimestamp;
        break;
    case mltError:
        rawFilename = m_ErrorLogFileName;
        logTimestamp = m_cacheErrorLogTimestamp;
        break;
    case mltPerformance:
        rawFilename = m_PerformanceLogFileName;
        logTimestamp = m_cachePerformanceLogTimestamp;
        break;
    case mltSession:
        rawFilename = m_SessionLogFileName;
        logTimestamp = m_cacheSessionLogTimestamp;
        break;
    case mltTrace:
        rawFilename = m_TraceLogFileName;
        logTimestamp = m_cacheTraceLogTimestamp;
        break;
    default:
        {
            STRING buffer;
            MgUtil::Int32ToString(logType, buffer);

            MgStringCollection arguments;
            arguments.Add(L"1");
            arguments.Add(buffer);

            throw new MgInvalidArgumentException(L"MgdLogManager.CheckArchiveFrequency",
                __LINE__, __WFILE__, &arguments, L"MgInvalidLogType", NULL);
        }
    }

    STRING specifier;
    STRING::size_type specifierIndex = rawFilename.find_first_of(L'%');
    if (specifierIndex != string::npos)
    {
        specifier = rawFilename[specifierIndex + 1];
    }


    // Compare the timestamp to the current time
    MgDateTime currentTime;

    // If the frequency boundary has been crossed, the file needs to be archived
    if (specifier == L"d")
    {
        if ( logTimestamp < currentTime )
        {
            if ( logTimestamp.GetDay() != currentTime.GetDay() )
            {
                bCurrentLog = false;
            }
        }
    }
    else if (specifier == L"m")
    {
        if ( logTimestamp.GetYear() <= currentTime.GetYear() )
        {
            if (logTimestamp.GetMonth() != currentTime.GetMonth())
            {
                bCurrentLog = false;
            }
        }
    }
    else if (specifier == L"y")
    {
        if ( logTimestamp.GetYear() < currentTime.GetYear() )
        {
            bCurrentLog = false;
        }
    }

    return bCurrentLog;
}

STRING MgdLogManager::RemoveArchiveFrequencySpecifier(CREFSTRING logFilename)
{
    STRING newFileName;

    // Remove the archive frequency specifier from the filename, if it is present
    newFileName = MgUtil::ReplaceString(logFilename, L"%y", L"");     // NOXLATE
    newFileName = MgUtil::ReplaceString(newFileName, L"%m", L"");     // NOXLATE
    newFileName = MgUtil::ReplaceString(newFileName, L"%d", L"");     // NOXLATE

    return newFileName;
}

bool MgdLogManager::IsLogFileInUse(CREFSTRING filename, enum MgLogType& logType)
{
    bool bResult = false;

    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex, false));

    logType = mltSystem; // Assign to log type not associated with a file

    // Check if the filename matches any of the log filenames in use
    if(filename.compare(m_AccessLogFileName) == 0)
    {
        // Match
        logType = mltAccess;
    }
    else if(filename.compare(m_AdminLogFileName) == 0)
    {
        // Match
        logType = mltAdmin;
    }
    else if(filename.compare(m_AuthenticationLogFileName) == 0)
    {
        // Match
        logType = mltAuthentication;
    }
    else if(filename.compare(m_ErrorLogFileName) == 0)
    {
        // Match
        logType = mltError;
    }
    else if(filename.compare(m_PerformanceLogFileName) == 0)
    {
        // Match
        logType = mltPerformance;
    }
    else if(filename.compare(m_SessionLogFileName) == 0)
    {
        // Match
        logType = mltSession;
    }
    else if(filename.compare(m_TraceLogFileName) == 0)
    {
        // Match
        logType = mltTrace;
    }

    // Was there a match?
    if(mltSystem != logType)
    {
        bResult = IsLogInUse(logType);
    }

    return bResult;
}

bool MgdLogManager::IsLogInUse(enum MgLogType& logType)
{
    bool bResult = false;

    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex, false));

    switch(logType)
    {
        case mltAccess:
            bResult = m_bAccessLogEnabled;
            break;
        case mltAdmin:
            bResult = m_bAdminLogEnabled;
            break;
        case mltAuthentication:
            bResult = m_bAuthenticationLogEnabled;
            break;
        case mltError:
            bResult = m_bErrorLogEnabled;
            break;
        case mltPerformance:
            bResult = m_bPerformanceLogEnabled;
            break;
        case mltSession:
            bResult = m_bSessionLogEnabled;
            break;
        case mltTrace:
            bResult = m_bTraceLogEnabled;
            break;
        default:
            break;
    }

    return bResult;
}

void MgdLogManager::DisableLog(enum MgLogType logType)
{
    ACE_MT (ACE_GUARD (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex));

    switch(logType)
    {
        case mltAccess:
            if(m_accessLogStream.is_open())
            {
                // Close the file
                m_accessLogStream.close();
            }
            break;
        case mltAdmin:
            if(m_adminLogStream.is_open())
            {
                // Close the file
                m_adminLogStream.close();
            }
            break;
        case mltAuthentication:
            if(m_authenticationLogStream.is_open())
            {
                // Close the file
                m_authenticationLogStream.close();
            }
            break;
        case mltError:
            if(m_errorLogStream.is_open())
            {
                // Close the file
                m_errorLogStream.close();
            }
            break;
        case mltPerformance:
            if(m_performanceLogStream.is_open())
            {
                // Close the file
                m_performanceLogStream.close();
            }
            break;
        case mltSession:
            if(m_sessionLogStream.is_open())
            {
                // Close the file
                m_sessionLogStream.close();
            }
            break;
        case mltTrace:
            if(m_traceLogStream.is_open())
            {
                // Close the file
                m_traceLogStream.close();
            }
            break;
        default:
            break;
    }
}

void MgdLogManager::EnableLog(enum MgLogType logType)
{
    ACE_MT (ACE_GUARD (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex));

    switch(logType)
    {
        case mltAccess:
            if(!m_accessLogStream.is_open())
            {
                // Open the file
                STRING filename = BuildFileName(m_AccessLogFileName);
                m_accessLogStream.open(MgUtil::WideCharToMultiByte(filename).c_str(), ios::out | ios::app | ios::binary);
            }
            break;
        case mltAdmin:
            if(!m_adminLogStream.is_open())
            {
                // Open the file
                STRING filename = BuildFileName(m_AdminLogFileName);
                m_adminLogStream.open(MgUtil::WideCharToMultiByte(filename).c_str(), ios::out | ios::app | ios::binary);
            }
            break;
        case mltAuthentication:
            if(!m_authenticationLogStream.is_open())
            {
                // Open the file
                STRING filename = BuildFileName(m_AuthenticationLogFileName);
                m_authenticationLogStream.open(MgUtil::WideCharToMultiByte(filename).c_str(), ios::out | ios::app | ios::binary);
            }
            break;
        case mltError:
            if(!m_errorLogStream.is_open())
            {
                // Open the file
                STRING filename = BuildFileName(m_ErrorLogFileName);
                m_errorLogStream.open(MgUtil::WideCharToMultiByte(filename).c_str(), ios::out | ios::app | ios::binary);
            }
            break;
        case mltPerformance:
            if(!m_performanceLogStream.is_open())
            {
                // Open the file
                STRING filename = BuildFileName(m_PerformanceLogFileName);
                m_performanceLogStream.open(MgUtil::WideCharToMultiByte(filename).c_str(), ios::out | ios::app | ios::binary);
            }
            break;
        case mltSession:
            if(!m_sessionLogStream.is_open())
            {
                // Open the file
                STRING filename = BuildFileName(m_SessionLogFileName);
                m_sessionLogStream.open(MgUtil::WideCharToMultiByte(filename).c_str(), ios::out | ios::app | ios::binary);
            }
            break;
        case mltTrace:
            if(!m_traceLogStream.is_open())
            {
                // Open the file
                STRING filename = BuildFileName(m_TraceLogFileName);
                m_traceLogStream.open(MgUtil::WideCharToMultiByte(filename).c_str(), ios::out | ios::app | ios::binary);
            }
            break;
        default:
            break;
    }

    SetLogHasHeader(logType, false);
}

void MgdLogManager::SetLogHasHeader(enum MgLogType logType, bool bHeader)
{
    ACE_MT (ACE_GUARD (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex));

    switch(logType)
    {
        case mltAccess:
            m_bAccessLogHeader = bHeader;
            break;
        case mltAdmin:
            m_bAdminLogHeader = bHeader;
            break;
        case mltAuthentication:
            m_bAuthenticationLogHeader = bHeader;
            break;
        case mltError:
            m_bErrorLogHeader = bHeader;
            break;
        case mltPerformance:
            m_bPerformanceLogHeader = bHeader;
            break;
        case mltSession:
            m_bSessionLogHeader = bHeader;
            break;
        case mltTrace:
            m_bTraceLogHeader = bHeader;
            break;
        default:
            break;
    }
}

bool MgdLogManager::LogHasHeader(enum MgLogType logType)
{
    bool bResult = false;

    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex, false));

    switch(logType)
    {
        case mltAccess:
            bResult = m_bAccessLogHeader;
            break;
        case mltAdmin:
            bResult = m_bAdminLogHeader;
            break;
        case mltAuthentication:
            bResult = m_bAuthenticationLogHeader;
            break;
        case mltError:
            bResult = m_bErrorLogHeader;
            break;
        case mltPerformance:
            bResult = m_bPerformanceLogHeader;
            break;
        case mltSession:
            bResult = m_bSessionLogHeader;
            break;
        case mltTrace:
            bResult = m_bTraceLogHeader;
            break;
        default:
            break;
    }

    return bResult;
}

void MgdLogManager::UpdateLogFilesTimestampCache()
{
    // mltAccess:
    if (IsAccessLogEnabled())
    {
        STRING accessLogFileName = BuildFileName(m_AccessLogFileName);
        if (MgFileUtil::PathnameExists(accessLogFileName))
        {
            m_cacheAccessLogTimestamp = MgFileUtil::GetFileModificationTime(accessLogFileName);
        }
    }

    // mltAdmin:
    if (IsAdminLogEnabled())
    {
        STRING adminLogFileName = BuildFileName(m_AdminLogFileName);
        if (IsAdminLogEnabled() && MgFileUtil::PathnameExists(adminLogFileName))
        {
            m_cacheAdminLogTimestamp = MgFileUtil::GetFileModificationTime(adminLogFileName);
        }
    }

    // mltAuthentication:
    if (IsAuthenticationLogEnabled())
    {
        STRING authenticationLogFileName = BuildFileName(m_AuthenticationLogFileName);
        if (MgFileUtil::PathnameExists(authenticationLogFileName))
        {
            m_cacheAuthenticationLogTimestamp = MgFileUtil::GetFileModificationTime(authenticationLogFileName);
        }
    }

    // mltError:
    if (IsErrorLogEnabled())
    {
        STRING errorLogFileName = BuildFileName(m_ErrorLogFileName);
        if (MgFileUtil::PathnameExists(errorLogFileName))
        {
            m_cacheErrorLogTimestamp = MgFileUtil::GetFileModificationTime(errorLogFileName);
        }
    }

    // mltPerformance:
    if (IsPerformanceLogEnabled())
    {
        STRING performanceLogFileName = BuildFileName(m_PerformanceLogFileName);
        if (MgFileUtil::PathnameExists(performanceLogFileName))
        {
            m_cachePerformanceLogTimestamp = MgFileUtil::GetFileModificationTime(performanceLogFileName);
        }
    }

    // mltSession:
    if (IsSessionLogEnabled())
    {
        STRING sessionLogFileName = BuildFileName(m_SessionLogFileName);
        if (MgFileUtil::PathnameExists(sessionLogFileName))
        {
            m_cacheSessionLogTimestamp = MgFileUtil::GetFileModificationTime(sessionLogFileName);
        }
    }

    // mltTrace:
    if (IsTraceLogEnabled())
    {
        STRING traceLogFileName = BuildFileName(m_TraceLogFileName);
        if (MgFileUtil::PathnameExists(traceLogFileName))
        {
            m_cacheTraceLogTimestamp = MgFileUtil::GetFileModificationTime(traceLogFileName);
        }
    }
}

void MgdLogManager::ParseLogService(INT16 serviceType, CREFSTRING configString)
{
    STRING serviceString;
    switch (serviceType)
    {
    case MgServiceType::ResourceService:
        serviceString = MgResources::ResourceService;
        break;
    case MgServiceType::DrawingService:
        serviceString = MgResources::DrawingService;
        break;
    case MgServiceType::FeatureService:
        serviceString = MgResources::FeatureService;
        break;
    case MgServiceType::MappingService:
        serviceString = MgResources::MappingService;
        break;
    case MgServiceType::RenderingService:
        serviceString = MgResources::RenderingService;
        break;
    case MgServiceType::TileService:
        serviceString = MgResources::TileService;
        break;
    case MgServiceType::KmlService:
        serviceString = MgResources::KmlService;
        break;
    case MgServiceType::ServerAdminService:
        serviceString = MgResources::ServerAdminService;
        break;
    case MgServiceType::SiteService:
        serviceString = MgResources::SiteService;
        break;
    case MgServiceType::ProfilingService:
        serviceString = MgResources::ProfilingService;
        break;
    default:
        break;
    }

    if (!serviceString.empty())
    {
        STRING::size_type pos = configString.find(serviceString);
        if (pos != configString.npos)
        {
            pos = configString.find(L":",pos);
            m_logsDetail[serviceType] = (INT8) MgUtil::StringToInt32(configString.substr(pos+1,1));
        }
    }
}
