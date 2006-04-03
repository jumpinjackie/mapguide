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

#include "AceCommon.h"
#include "LogManager.h"
#include "Connection.h"

const int MAX_BUF       = 16384;
const int MAX_LINES     = 512;
const int SECONDSINDAY  = 86400;

// Process-wide MgResources
Ptr<MgLogManager> MgLogManager::m_logManager          = (MgLogManager*)NULL;

// Default values
STRING MgLogManager::m_path                           = L"./";                    // Default path
INT32 MgLogManager::m_maxLogSize                      = 64;
STRING MgLogManager::m_delimiter                      = L"\t";
bool MgLogManager::m_useMaxLogSize                    = false;
const STRING MgLogManager::DefaultAccessLogFileName         = L"Access.log";
const STRING MgLogManager::DefaultAdminLogFileName          = L"Admin.log";
const STRING MgLogManager::DefaultAuthenticationLogFileName = L"Authentication.log";
const STRING MgLogManager::DefaultErrorLogFileName          = L"Error.log";
const STRING MgLogManager::DefaultSessionLogFileName        = L"Session.log";
const STRING MgLogManager::DefaultTraceLogFileName          = L"Trace.log";

// Log parameters
const STRING MgLogManager::AverageOpTimeParam   = L"AVERAGEOPTIME";
const STRING MgLogManager::ClientParam          = L"CLIENT";
const STRING MgLogManager::ClientIpParam        = L"CLIENTIP";
const STRING MgLogManager::DurationParam        = L"DURATION";
const STRING MgLogManager::EndTimeParam         = L"ENDTIME";
const STRING MgLogManager::ErrorParam           = L"ERROR";
const STRING MgLogManager::InfoParam            = L"INFO";
const STRING MgLogManager::OpIdParam            = L"OPID";
const STRING MgLogManager::OpsFailedParam       = L"OPSFAILED";
const STRING MgLogManager::OpsProcessedParam    = L"OPSPROCESSED";
const STRING MgLogManager::OpsReceivedParam     = L"OPSRECEIVED";
const STRING MgLogManager::StackTraceParam      = L"STACKTRACE";
const STRING MgLogManager::StartTimeParam       = L"STARTTIME";
const STRING MgLogManager::UserParam            = L"USER";

// Header line prefix strings
const STRING MgLogManager::HeaderLine1          = L"# Log Type:";
const STRING MgLogManager::HeaderLine2          = L"# Log Parameters:";

// Log type strings
const STRING MgLogManager::AccessLog            = L"Access Log";
const STRING MgLogManager::AdminLog             = L"Admin Log";
const STRING MgLogManager::AuthenticationLog    = L"Authentication Log";
const STRING MgLogManager::ErrorLog             = L"Error Log";
const STRING MgLogManager::SessionLog           = L"Session Log";
const STRING MgLogManager::TraceLog             = L"Trace Log";
const STRING MgLogManager::UnspecifiedLog       = L"Unspecified";

// Log file properties
const STRING MgLogManager::LogNameProperty      = L"LogNameProperty";
const STRING MgLogManager::LogTypeProperty      = L"LogTypeProperty";
const STRING MgLogManager::LogStatusProperty    = L"LogStatusProperty";

// Log status strings
const STRING MgLogManager::LogStatusActive      = L"Active";
const STRING MgLogManager::LogStatusArchive     = L"Archive";

// Constructor
MgLogManager::MgLogManager(void) :
    m_outputStream(0),
    m_bAccessLogEnabled(true),
    m_AccessLogFileName(MgLogManager::DefaultAccessLogFileName),
    m_bAdminLogEnabled(true),
    m_AdminLogFileName(MgLogManager::DefaultAdminLogFileName),
    m_bAuthenticationLogEnabled(true),
    m_AuthenticationLogFileName(MgLogManager::DefaultAuthenticationLogFileName),
    m_bErrorLogEnabled(true),
    m_ErrorLogFileName(MgLogManager::DefaultErrorLogFileName),
    m_bSessionLogEnabled(true),
    m_SessionLogFileName(MgLogManager::DefaultSessionLogFileName),
    m_bTraceLogEnabled(false),      // Disabled by default
    m_TraceLogFileName(MgLogManager::DefaultTraceLogFileName)
{
}

// Destructor
MgLogManager::~MgLogManager(void)
{
    ACE_DEBUG ((LM_DEBUG, ACE_TEXT("(%P|%t) MgLogManager::~MgLogManager()\n")));

}

void MgLogManager::Dispose(void)
{
    delete this;
}

// Get pointer to a process-wide MgLogManager.
MgLogManager* MgLogManager::GetInstance()
{
    MG_LOGMANAGER_TRY()

    ACE_TRACE ("MgLogManager::GetInstance");

    if (MgLogManager::m_logManager == NULL)
    {
        // Perform Double-Checked Locking Optimization.
        ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, *ACE_Static_Object_Lock::instance (), 0));
        if (MgLogManager::m_logManager == NULL)
        {
            MgLogManager::m_logManager = new MgLogManager;
        }
    }

    MG_LOGMANAGER_CATCH_AND_THROW(L"MgLogManager.GetInstance")

    // To avoid overheads and maintain thread safety,
    // do not assign this returned static singleton to a Ptr object.
    return MgLogManager::m_logManager;
}

// Initialization
void MgLogManager::Initialize()
{
    MG_LOGMANAGER_TRY()

    m_applicationName = MgResources::ServerServiceDisplayName;

    MgConfiguration* pConfiguration = MgConfiguration::GetInstance();

    // Get the logs path
    pConfiguration->GetStringValue(MgConfigProperties::GeneralPropertiesSection, MgConfigProperties::GeneralPropertyLogsPath, m_path, MgConfigProperties::DefaultGeneralPropertyLogsPath);

    // Check if path ends with a '/' if not, add one if needed
    MgFileUtil::AppendSlashToEndOfPath(m_path);

    // Try and create the directory. If it already exists this will error and not a problem
    ACE_OS::mkdir(MG_WCHAR_TO_TCHAR(m_path));

    // Load the configuration properties
    LoadConfigurationProperties();

    // Check if any existing log files contain valid header information
    ValidateLogHeaders();

    MG_LOGMANAGER_CATCH_AND_THROW(L"MgLogManager.Initialize")
}

void MgLogManager::LoadConfigurationProperties()
{
    MG_LOGMANAGER_TRY()

    ACE_MT (ACE_GUARD (ACE_Recursive_Thread_Mutex, ace_mon, m_mutex));

    MgConfiguration* pConfiguration = MgConfiguration::GetInstance();

    bool bLogEnabled;
    STRING logFileName;
    STRING logParameters;

    // Maximum log file size
    pConfiguration->GetBoolValue(MgConfigProperties::GeneralPropertiesSection, MgConfigProperties::GeneralPropertyMaxLogFileSizeEnabled, m_useMaxLogSize, MgConfigProperties::DefaultGeneralPropertyMaxLogFileSizeEnabled);
    pConfiguration->GetIntValue(MgConfigProperties::GeneralPropertiesSection, MgConfigProperties::GeneralPropertyMaxLogFileSize, m_maxLogSize, MgConfigProperties::DefaultGeneralPropertyMaxLogFileSize);
    // Log data delimiter
    pConfiguration->GetStringValue(MgConfigProperties::GeneralPropertiesSection, MgConfigProperties::GeneralPropertyLogsDelimiter, m_delimiter, MgConfigProperties::DefaultGeneralPropertyLogsDelimiter);
    TranslateDelimiter();


    // Access Log
    pConfiguration->GetBoolValue(MgConfigProperties::AccessLogPropertiesSection, MgConfigProperties::AccessLogPropertyEnabled, bLogEnabled, MgConfigProperties::DefaultAccessLogPropertyEnabled);
    pConfiguration->GetStringValue(MgConfigProperties::AccessLogPropertiesSection, MgConfigProperties::AccessLogPropertyFilename, logFileName, MgConfigProperties::DefaultAccessLogPropertyFilename);
    pConfiguration->GetStringValue(MgConfigProperties::AccessLogPropertiesSection, MgConfigProperties::AccessLogPropertyParameters, logParameters, MgConfigProperties::DefaultAccessLogPropertyParameters);
    SetAccessLogInfo(bLogEnabled, logFileName, logParameters);

    // Admin Log
    pConfiguration->GetBoolValue(MgConfigProperties::AdminLogPropertiesSection, MgConfigProperties::AdminLogPropertyEnabled, bLogEnabled, MgConfigProperties::DefaultAdminLogPropertyEnabled);
    pConfiguration->GetStringValue(MgConfigProperties::AdminLogPropertiesSection, MgConfigProperties::AdminLogPropertyFilename, logFileName, MgConfigProperties::DefaultAdminLogPropertyFilename);
    pConfiguration->GetStringValue(MgConfigProperties::AdminLogPropertiesSection, MgConfigProperties::AdminLogPropertyParameters, logParameters, MgConfigProperties::DefaultAdminLogPropertyParameters);
    SetAdminLogInfo(bLogEnabled, logFileName, logParameters);

    // Authentication Log
    pConfiguration->GetBoolValue(MgConfigProperties::AuthenticationLogPropertiesSection, MgConfigProperties::AuthenticationLogPropertyEnabled, bLogEnabled, MgConfigProperties::DefaultAuthenticationLogPropertyEnabled);
    pConfiguration->GetStringValue(MgConfigProperties::AuthenticationLogPropertiesSection, MgConfigProperties::AuthenticationLogPropertyFilename, logFileName, MgConfigProperties::DefaultAuthenticationLogPropertyFilename);
    pConfiguration->GetStringValue(MgConfigProperties::AuthenticationLogPropertiesSection, MgConfigProperties::AuthenticationLogPropertyParameters, logParameters, MgConfigProperties::DefaultAuthenticationLogPropertyParameters);
    SetAuthenticationLogInfo(bLogEnabled, logFileName, logParameters);

    // Error Log
    pConfiguration->GetBoolValue(MgConfigProperties::ErrorLogPropertiesSection, MgConfigProperties::ErrorLogPropertyEnabled, bLogEnabled, MgConfigProperties::DefaultErrorLogPropertyEnabled);
    pConfiguration->GetStringValue(MgConfigProperties::ErrorLogPropertiesSection, MgConfigProperties::ErrorLogPropertyFilename, logFileName, MgConfigProperties::DefaultErrorLogPropertyFilename);
    pConfiguration->GetStringValue(MgConfigProperties::ErrorLogPropertiesSection, MgConfigProperties::ErrorLogPropertyParameters, logParameters, MgConfigProperties::DefaultErrorLogPropertyParameters);
    SetErrorLogInfo(bLogEnabled, logFileName, logParameters);

    // Session Log
    pConfiguration->GetBoolValue(MgConfigProperties::SessionLogPropertiesSection, MgConfigProperties::SessionLogPropertyEnabled, bLogEnabled, MgConfigProperties::DefaultSessionLogPropertyEnabled);
    pConfiguration->GetStringValue(MgConfigProperties::SessionLogPropertiesSection, MgConfigProperties::SessionLogPropertyFilename, logFileName, MgConfigProperties::DefaultSessionLogPropertyFilename);
    pConfiguration->GetStringValue(MgConfigProperties::SessionLogPropertiesSection, MgConfigProperties::SessionLogPropertyParameters, logParameters, MgConfigProperties::DefaultSessionLogPropertyParameters);
    SetSessionLogInfo(bLogEnabled, logFileName, logParameters);

    // Trace Log
    pConfiguration->GetBoolValue(MgConfigProperties::TraceLogPropertiesSection, MgConfigProperties::TraceLogPropertyEnabled, bLogEnabled, MgConfigProperties::DefaultTraceLogPropertyEnabled);
    pConfiguration->GetStringValue(MgConfigProperties::TraceLogPropertiesSection, MgConfigProperties::TraceLogPropertyFilename, logFileName, MgConfigProperties::DefaultTraceLogPropertyFilename);
    pConfiguration->GetStringValue(MgConfigProperties::TraceLogPropertiesSection, MgConfigProperties::TraceLogPropertyParameters, logParameters, MgConfigProperties::DefaultTraceLogPropertyParameters);
    SetTraceLogInfo(bLogEnabled, logFileName, logParameters);

    MG_LOGMANAGER_CATCH_AND_THROW(L"MgLogManager.LoadConfigurationProperties")
}

CREFSTRING MgLogManager::GetLogsPath()
{
    return m_path;
}

void MgLogManager::SetAccessLogInfo(bool bEnabled, CREFSTRING filename, CREFSTRING parameters)
{
    MG_LOGMANAGER_TRY()

    m_bAccessLogEnabled = bEnabled;
    m_AccessLogParameters = parameters;
    SetAccessLogFileName(filename);

    MG_LOGMANAGER_CATCH_AND_THROW(L"MgLogManager.SetAccessLogInfo");
}

bool MgLogManager::IsAccessLogEnabled()
{
    return m_bAccessLogEnabled;
}

void MgLogManager::SetAccessLogEnabled(bool bEnabled)
{
    m_bAccessLogEnabled = bEnabled;
}

STRING MgLogManager::GetAccessLogFileName()
{
    return m_AccessLogFileName;
}

void MgLogManager::SetAccessLogFileName(CREFSTRING filename)
{
    if (filename.empty())
    {
        throw new MgNullArgumentException(L"MgLogManager.SetAccessLogFileName", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    if (filename.find(L"/") != string::npos ||
        filename.find(L"\\") != string::npos)
    {
        // Does not support paths.  Must be a strict filename only.
        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(filename);

        throw new MgInvalidArgumentException(L"MgLogManager.SetAccessLogFileName",
            __LINE__, __WFILE__, &arguments, L"MgStringContainsReservedCharacters", NULL);
    }

    m_AccessLogFileName = filename;
}

STRING MgLogManager::GetAccessLogParameters()
{
    return m_AccessLogParameters;
}

void MgLogManager::SetAccessLogParameters(CREFSTRING parameters)
{
    if (0 != parameters.compare(m_AccessLogParameters))
    {
        STRING filename = BuildFileName(m_AccessLogFileName).c_str();
        ArchiveLog(filename);
    }
    m_AccessLogParameters = parameters;
}

bool MgLogManager::ClearAccessLog()
{
    // Get the access log filename and path
    STRING filename = BuildFileName(m_AccessLogFileName).c_str();
    return RemoveLogFile(filename);
}

MgByteReader* MgLogManager::GetAccessLog()
{
    Ptr<MgByteReader> byteReader;

    MG_LOGMANAGER_TRY()

    // Get the access log filename and path
    STRING filename = BuildFileName(m_AccessLogFileName).c_str();
    byteReader = GetLogContents(filename);

    MG_LOGMANAGER_CATCH_AND_THROW(L"MgLogManager.GetAccessLog")

    return byteReader.Detach();
}

MgByteReader* MgLogManager::GetAccessLog(INT32 numEntries)
{
    Ptr<MgByteReader> byteReader;

    MG_LOGMANAGER_TRY()

    // Get the access log filename and path
    STRING filename = BuildFileName(m_AccessLogFileName).c_str();
    byteReader = GetLogContents(filename, numEntries);

    MG_LOGMANAGER_CATCH_AND_THROW(L"MgLogManager.GetAccessLog")

    return byteReader.Detach();
}

MgByteReader* MgLogManager::GetAccessLog(MgDateTime* fromDate, MgDateTime* toDate)
{
    Ptr<MgByteReader> byteReader;

    MG_LOGMANAGER_TRY()

    byteReader = GetLogContents(mltAccess, fromDate, toDate);

    MG_LOGMANAGER_CATCH_AND_THROW(L"MgLogManager.GetAccessLog")

    return byteReader.Detach();
}

void MgLogManager::SetAdminLogInfo(bool bEnabled, CREFSTRING filename, CREFSTRING parameters)
{
    MG_LOGMANAGER_TRY()

    m_bAdminLogEnabled = bEnabled;
    m_AdminLogParameters = parameters;
    SetAdminLogFileName(filename);

    MG_LOGMANAGER_CATCH_AND_THROW(L"MgLogManager.SetAdminLogInfo");
}

bool MgLogManager::IsAdminLogEnabled()
{
    return m_bAdminLogEnabled;
}

void MgLogManager::SetAdminLogEnabled(bool bEnabled)
{
    m_bAdminLogEnabled = bEnabled;
}

STRING MgLogManager::GetAdminLogFileName()
{
    return m_AdminLogFileName;
}

void MgLogManager::SetAdminLogFileName(CREFSTRING filename)
{
    if (filename.empty())
    {
        throw new MgNullArgumentException(L"MgLogManager.SetAdminLogFileName", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    if (filename.find(L"/") != string::npos ||
        filename.find(L"\\") != string::npos)
    {
        // Does not support paths.  Must be a strict filename only.
        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(filename);

        throw new MgInvalidArgumentException(L"MgLogManager.SetAdminLogFileName",
            __LINE__, __WFILE__, &arguments, L"MgStringContainsReservedCharacters", NULL);
    }

    m_AdminLogFileName = filename;
}

STRING MgLogManager::GetAdminLogParameters()
{
    return m_AdminLogParameters;
}

void MgLogManager::SetAdminLogParameters(CREFSTRING parameters)
{
    if (0 != parameters.compare(m_AdminLogParameters))
    {
        STRING filename = BuildFileName(m_AdminLogFileName).c_str();
        ArchiveLog(filename);
    }
    m_AdminLogParameters = parameters;
}

bool MgLogManager::ClearAdminLog()
{
    // Get the admin log filename and path
    STRING filename = BuildFileName(m_AdminLogFileName).c_str();
    return RemoveLogFile(filename);
}

MgByteReader* MgLogManager::GetAdminLog()
{
    Ptr<MgByteReader> byteReader;

    MG_LOGMANAGER_TRY()

    // Get the admin log filename and path
    STRING filename = BuildFileName(m_AdminLogFileName).c_str();
    byteReader = GetLogContents(filename);

    MG_LOGMANAGER_CATCH_AND_THROW(L"MgLogManager.GetAdminLog")

    return byteReader.Detach();
}

MgByteReader* MgLogManager::GetAdminLog(INT32 numEntries)
{
    Ptr<MgByteReader> byteReader;

    MG_LOGMANAGER_TRY()

    // Get the admin log filename and path
    STRING filename = BuildFileName(m_AdminLogFileName).c_str();
    byteReader = GetLogContents(filename, numEntries);

    MG_LOGMANAGER_CATCH_AND_THROW(L"MgLogManager.GetAdminLog")

    return byteReader.Detach();
}

MgByteReader* MgLogManager::GetAdminLog(MgDateTime* fromDate, MgDateTime* toDate)
{
    Ptr<MgByteReader> byteReader;

    MG_LOGMANAGER_TRY()

    byteReader = GetLogContents(mltAdmin, fromDate, toDate);

    MG_LOGMANAGER_CATCH_AND_THROW(L"MgLogManager.GetAdminLog")

    return byteReader.Detach();
}

void MgLogManager::SetAuthenticationLogInfo(bool bEnabled, CREFSTRING filename, CREFSTRING parameters)
{
    MG_LOGMANAGER_TRY()

    m_bAuthenticationLogEnabled = bEnabled;
    m_AuthenticationLogParameters = parameters;
    SetAuthenticationLogFileName(filename);

    MG_LOGMANAGER_CATCH_AND_THROW(L"MgLogManager.SetAuthenticationLogInfo");
}

bool MgLogManager::IsAuthenticationLogEnabled()
{
    return m_bAuthenticationLogEnabled;
}

void MgLogManager::SetAuthenticationLogEnabled(bool bEnabled)
{
    m_bAuthenticationLogEnabled = bEnabled;
}

STRING MgLogManager::GetAuthenticationLogFileName()
{
    return m_AuthenticationLogFileName;
}

void MgLogManager::SetAuthenticationLogFileName(CREFSTRING filename)
{
    if (filename.empty())
    {
        throw new MgNullArgumentException(L"MgLogManager.SetAuthenticationLogFileName", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    if (filename.find(L"/") != string::npos ||
        filename.find(L"\\") != string::npos)
    {
        // Does not support paths.  Must be a strict filename only.
        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(filename);

        throw new MgInvalidArgumentException(L"MgLogManager.SetAuthenticationLogFileName",
            __LINE__, __WFILE__, &arguments, L"MgStringContainsReservedCharacters", NULL);
    }

    m_AuthenticationLogFileName = filename;
}

STRING MgLogManager::GetAuthenticationLogParameters()
{
    return m_AuthenticationLogParameters;
}

void MgLogManager::SetAuthenticationLogParameters(CREFSTRING parameters)
{
    if (0 != parameters.compare(m_AuthenticationLogParameters))
    {
        STRING filename = BuildFileName(m_AuthenticationLogFileName).c_str();
        ArchiveLog(filename);
    }
    m_AuthenticationLogParameters = parameters;
}

bool MgLogManager::ClearAuthenticationLog()
{
    // Get the authentication log filename and path
    STRING filename = BuildFileName(m_AuthenticationLogFileName).c_str();
    return RemoveLogFile(filename);
}

MgByteReader* MgLogManager::GetAuthenticationLog()
{
    Ptr<MgByteReader> byteReader;

    MG_LOGMANAGER_TRY()

    // Get the authentication log filename and path
    STRING filename = BuildFileName(m_AuthenticationLogFileName).c_str();
    byteReader = GetLogContents(filename);

    MG_LOGMANAGER_CATCH_AND_THROW(L"MgLogManager.GetAuthenticationLog")

    return byteReader.Detach();
}

MgByteReader* MgLogManager::GetAuthenticationLog(INT32 numEntries)
{
    Ptr<MgByteReader> byteReader;

    MG_LOGMANAGER_TRY()

    // Get the authentication log filename and path
    STRING filename = BuildFileName(m_AuthenticationLogFileName).c_str();
    byteReader = GetLogContents(filename, numEntries);

    MG_LOGMANAGER_CATCH_AND_THROW(L"MgLogManager.GetAuthenticationLog")

    return byteReader.Detach();
}

MgByteReader* MgLogManager::GetAuthenticationLog(MgDateTime* fromDate, MgDateTime* toDate)
{
    Ptr<MgByteReader> byteReader;

    MG_LOGMANAGER_TRY()

    byteReader = GetLogContents(mltAuthentication, fromDate, toDate);

    MG_LOGMANAGER_CATCH_AND_THROW(L"MgLogManager.GetAuthenticationLog")

    return byteReader.Detach();
}


void MgLogManager::SetErrorLogInfo(bool bEnabled, CREFSTRING filename, CREFSTRING parameters)
{
    MG_LOGMANAGER_TRY()

    m_bErrorLogEnabled = bEnabled;
    m_ErrorLogParameters = parameters;
    SetErrorLogFileName(filename);

    MG_LOGMANAGER_CATCH_AND_THROW(L"MgLogManager.SetErrorLogInfo");
}

bool MgLogManager::IsErrorLogEnabled()
{
    return m_bErrorLogEnabled;
}

void MgLogManager::SetErrorLogEnabled(bool bEnabled)
{
    m_bErrorLogEnabled = bEnabled;
}

STRING MgLogManager::GetErrorLogFileName()
{
    return m_ErrorLogFileName;
}

void MgLogManager::SetErrorLogFileName(CREFSTRING filename)
{
    if (filename.empty())
    {
        throw new MgNullArgumentException(L"MgLogManager.SetErrorLogFileName", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    if (filename.find(L"/") != string::npos ||
        filename.find(L"\\") != string::npos)
    {
        // Does not support paths.  Must be a strict filename only.
        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(filename);

        throw new MgInvalidArgumentException(L"MgLogManager.SetErrorLogFileName",
            __LINE__, __WFILE__, &arguments, L"MgStringContainsReservedCharacters", NULL);
    }

    m_ErrorLogFileName = filename;
}

STRING MgLogManager::GetErrorLogParameters()
{
    return m_ErrorLogParameters;
}

void MgLogManager::SetErrorLogParameters(CREFSTRING parameters)
{
    if (0 != parameters.compare(m_ErrorLogParameters))
    {
        STRING filename = BuildFileName(m_ErrorLogFileName).c_str();
        ArchiveLog(filename);
    }
    m_ErrorLogParameters = parameters;
}

bool MgLogManager::ClearErrorLog()
{
    // Get the error log filename and path
    STRING filename = BuildFileName(m_ErrorLogFileName).c_str();
    return RemoveLogFile(filename);
}

MgByteReader* MgLogManager::GetErrorLog()
{
    Ptr<MgByteReader> byteReader;

    MG_LOGMANAGER_TRY()

    // Get the error log filename and path
    STRING filename = BuildFileName(m_ErrorLogFileName).c_str();
    byteReader = GetLogContents(filename);

    MG_LOGMANAGER_CATCH_AND_THROW(L"MgLogManager.GetErrorLog")

    return byteReader.Detach();
}

MgByteReader* MgLogManager::GetErrorLog(INT32 numEntries)
{
    Ptr<MgByteReader> byteReader;

    MG_LOGMANAGER_TRY()

    // Get the error log filename and path
    STRING filename = BuildFileName(m_ErrorLogFileName).c_str();
    byteReader = GetLogContents(filename, numEntries);

    MG_LOGMANAGER_CATCH_AND_THROW(L"MgLogManager.GetErrorLog")

    return byteReader.Detach();
}

MgByteReader* MgLogManager::GetErrorLog(MgDateTime* fromDate, MgDateTime* toDate)
{
    Ptr<MgByteReader> byteReader;

    MG_LOGMANAGER_TRY()

    byteReader = GetLogContents(mltError, fromDate, toDate);

    MG_LOGMANAGER_CATCH_AND_THROW(L"MgLogManager.GetErrorLog")

    return byteReader.Detach();
}

void MgLogManager::SetSessionLogInfo(bool bEnabled, CREFSTRING filename, CREFSTRING parameters)
{
    MG_LOGMANAGER_TRY()

    m_bSessionLogEnabled = bEnabled;
    m_SessionLogParameters = parameters;
    SetSessionLogFileName(filename);

    MG_LOGMANAGER_CATCH_AND_THROW(L"MgLogManager.SetSessionLogInfo");
}

bool MgLogManager::IsSessionLogEnabled()
{
    return m_bSessionLogEnabled;
}

void MgLogManager::SetSessionLogEnabled(bool bEnabled)
{
    m_bSessionLogEnabled = bEnabled;
}

STRING MgLogManager::GetSessionLogFileName()
{
    return m_SessionLogFileName;
}

void MgLogManager::SetSessionLogFileName(CREFSTRING filename)
{
    if (filename.empty())
    {
        throw new MgNullArgumentException(L"MgLogManager.SetSessionLogFileName", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    if (filename.find(L"/") != string::npos ||
        filename.find(L"\\") != string::npos)
    {
        // Does not support paths.  Must be a strict filename only.
        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(filename);

        throw new MgInvalidArgumentException(L"MgLogManager.SetSessionLogFileName",
            __LINE__, __WFILE__, &arguments, L"MgStringContainsReservedCharacters", NULL);
    }

    m_SessionLogFileName = filename;
}

STRING MgLogManager::GetSessionLogParameters()
{
    return m_SessionLogParameters;
}

void MgLogManager::SetSessionLogParameters(CREFSTRING parameters)
{
    if (0 != parameters.compare(m_SessionLogParameters))
    {
        STRING filename = BuildFileName(m_SessionLogFileName).c_str();
        ArchiveLog(filename);
    }
    m_SessionLogParameters = parameters;
}

bool MgLogManager::ClearSessionLog()
{
    // Get the session log filename and path
    STRING filename = BuildFileName(m_SessionLogFileName).c_str();
    return RemoveLogFile(filename);
}

MgByteReader* MgLogManager::GetSessionLog()
{
    Ptr<MgByteReader> byteReader;

    MG_LOGMANAGER_TRY()

    // Get the session log filename and path
    STRING filename = BuildFileName(m_SessionLogFileName).c_str();
    byteReader = GetLogContents(filename);

    MG_LOGMANAGER_CATCH_AND_THROW(L"MgLogManager.GetSessionLog")

    return byteReader.Detach();
}

MgByteReader* MgLogManager::GetSessionLog(INT32 numEntries)
{
    Ptr<MgByteReader> byteReader;

    MG_LOGMANAGER_TRY()

    // Get the session log filename and path
    STRING filename = BuildFileName(m_SessionLogFileName).c_str();
    byteReader = GetLogContents(filename, numEntries);

    MG_LOGMANAGER_CATCH_AND_THROW(L"MgLogManager.GetSessionLog")

    return byteReader.Detach();
}

MgByteReader* MgLogManager::GetSessionLog(MgDateTime* fromDate, MgDateTime* toDate)
{
    Ptr<MgByteReader> byteReader;

    MG_LOGMANAGER_TRY()

    byteReader = GetLogContents(mltSession, fromDate, toDate);

    MG_LOGMANAGER_CATCH_AND_THROW(L"MgLogManager.GetSessionLog")

    return byteReader.Detach();
}

void MgLogManager::SetTraceLogInfo(bool bEnabled, CREFSTRING filename, CREFSTRING parameters)
{
    MG_LOGMANAGER_TRY()

    m_bTraceLogEnabled = bEnabled;
    m_TraceLogParameters = parameters;
    SetTraceLogFileName(filename);

    MG_LOGMANAGER_CATCH_AND_THROW(L"MgLogManager.SetTraceLogInfo")
}

bool MgLogManager::IsTraceLogEnabled()
{
    return m_bTraceLogEnabled;
}

void MgLogManager::SetTraceLogEnabled(bool bEnabled)
{
    m_bTraceLogEnabled = bEnabled;
}

STRING MgLogManager::GetTraceLogFileName()
{
    return m_TraceLogFileName;
}

void MgLogManager::SetTraceLogFileName(CREFSTRING filename)
{
    if (filename.empty())
    {
        throw new MgNullArgumentException(L"MgLogManager.SetTraceLogFileName", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    if (filename.find(L"/") != string::npos ||
        filename.find(L"\\") != string::npos)
    {
        // Does not support paths.  Must be a strict filename only.
        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(filename);

        throw new MgInvalidArgumentException(L"MgLogManager.SetTraceLogFileName",
            __LINE__, __WFILE__, &arguments, L"MgStringContainsReservedCharacters", NULL);
    }

    m_TraceLogFileName = filename;
}

STRING MgLogManager::GetTraceLogParameters()
{
    return m_TraceLogParameters;
}

void MgLogManager::SetTraceLogParameters(CREFSTRING parameters)
{
    if (0 != parameters.compare(m_TraceLogParameters))
    {
        STRING filename = BuildFileName(m_TraceLogFileName).c_str();
        ArchiveLog(filename);
    }
    m_TraceLogParameters = parameters;
}

bool MgLogManager::ClearTraceLog()
{
    // Get the trace log filename and path
    STRING filename = BuildFileName(m_TraceLogFileName).c_str();
    return RemoveLogFile(filename);
}

MgByteReader* MgLogManager::GetTraceLog()
{
    Ptr<MgByteReader> byteReader;

    MG_LOGMANAGER_TRY()

    // Get the trace log filename and path
    STRING filename = BuildFileName(m_TraceLogFileName).c_str();
    byteReader = GetLogContents(filename);

    MG_LOGMANAGER_CATCH_AND_THROW(L"MgLogManager.GetTraceLog")

    return byteReader.Detach();
}

MgByteReader* MgLogManager::GetTraceLog(INT32 numEntries)
{
    Ptr<MgByteReader> byteReader;

    MG_LOGMANAGER_TRY()

    // Get the trace log filename and path
    STRING filename = BuildFileName(m_TraceLogFileName).c_str();
    byteReader = GetLogContents(filename, numEntries);

    MG_LOGMANAGER_CATCH_AND_THROW(L"MgLogManager.GetTraceLog")

    return byteReader.Detach();
}

MgByteReader* MgLogManager::GetTraceLog(MgDateTime* fromDate, MgDateTime* toDate)
{
    Ptr<MgByteReader> byteReader;

    MG_LOGMANAGER_TRY()

    byteReader = GetLogContents(mltTrace, fromDate, toDate);

    MG_LOGMANAGER_CATCH_AND_THROW(L"MgLogManager.GetTraceLog")

    return byteReader.Detach();
}

MgByteReader* MgLogManager::GetLogFile( CREFSTRING filename )
{
    Ptr<MgByteReader> byteReader;

    MG_LOGMANAGER_TRY()

    // Get the trace log filename and path
    STRING fullPath = BuildFileName(filename).c_str();
    byteReader = GetLogContents(fullPath);

    MG_LOGMANAGER_CATCH_AND_THROW(L"MgLogManager.GetLogFile")

    return byteReader.Detach();
}

void MgLogManager::LogToSysLog(char* application)
{
    ACE_Log_Msg* pAce = ACE_Log_Msg::instance();
    pAce->open(ACE_TEXT_CHAR_TO_TCHAR(application), ACE_Log_Msg::SYSLOG, ACE_TEXT_CHAR_TO_TCHAR(application));
}

void MgLogManager::LogToOStream(ACE_OSTREAM_TYPE* output)
{
    m_outputStream = output;
    ACE_Log_Msg* pAce = ACE_Log_Msg::instance();
    pAce->msg_ostream(m_outputStream);
    pAce->clr_flags(ACE_Log_Msg::STDERR | ACE_Log_Msg::LOGGER | ACE_Log_Msg::SYSLOG);
    pAce->set_flags(ACE_Log_Msg::OSTREAM);
}

void MgLogManager::LogToStderr(void)
{
    ACE_Log_Msg* pAce = ACE_Log_Msg::instance();
    pAce->clr_flags(ACE_Log_Msg::OSTREAM | ACE_Log_Msg::LOGGER | ACE_Log_Msg::SYSLOG);
    pAce->set_flags(ACE_Log_Msg::STDERR);
}

void MgLogManager::LogSystemEntry(ACE_Log_Priority priority, CREFSTRING entry)
{
    WriteLogMessage(mltSystem, entry, priority);
}

void MgLogManager::LogAccessEntry(CREFSTRING opId)
{
    // Message to be entered into the log
    STRING logEntry;

    MG_LOGMANAGER_TRY()

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

            if (MgLogManager::ClientParam == param)
            {
                AddClient(logEntry);
            }
            else if (MgLogManager::ClientIpParam == param)
            {
                AddClientIp(logEntry);
            }
            else if (MgLogManager::UserParam == param)
            {
                AddUserName(logEntry);
            }
            else if (MgLogManager::OpIdParam == param)
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

    MG_LOGMANAGER_CATCH(L"MgLogManager.LogAccessEntry")

    if (mgException != NULL)
    {
        // Use default message with just opId
        logEntry.clear();
        AddOpId(logEntry, opId);
    }

    WriteLogMessage(mltAccess, logEntry, LM_INFO);
}

void MgLogManager::LogAdminEntry(CREFSTRING opId)
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

            if (MgLogManager::ClientParam == param)
            {
                AddClient(logEntry);
            }
            else if (MgLogManager::ClientIpParam == param)
            {
                AddClientIp(logEntry);
            }
            else if (MgLogManager::UserParam == param)
            {
                AddUserName(logEntry);
            }
            else if (MgLogManager::OpIdParam == param)
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

    MG_LOGMANAGER_CATCH(L"MgLogManager.LogAdminEntry")

    if (mgException != NULL)
    {
        // Use default message with just opId
        logEntry.clear();
        AddOpId(logEntry, opId);
    }

    WriteLogMessage(mltAdmin, logEntry, LM_INFO);
}

void MgLogManager::LogAuthenticationEntry(CREFSTRING entry)
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

            if (MgLogManager::ClientParam == param)
            {
                AddClient(logEntry);
            }
            else if (MgLogManager::ClientIpParam == param)
            {
                AddClientIp(logEntry);
            }
            else if (MgLogManager::UserParam == param)
            {
                AddUserName(logEntry);
            }
        }
    }

    // Add the given info.
    logEntry += entry;

    MG_LOGMANAGER_CATCH(L"MgLogManager.LogAuthenticationEntry")

    if (mgException != NULL)
    {
        // Use default message with just the given info.
        // TODO: Create actual default message when it's known what will go in this log
        logEntry = entry;
    }

    WriteLogMessage(mltAuthentication, logEntry, LM_INFO);
}

void MgLogManager::LogErrorEntry(CREFSTRING entry, CREFSTRING stackTrace)
{
    // Message to be entered into the log
    STRING logEntry;

    MG_LOGMANAGER_TRY()

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

            if (MgLogManager::ClientParam == param)
            {
                AddClient(logEntry);
            }
            else if (MgLogManager::ClientIpParam == param)
            {
                AddClientIp(logEntry);
            }
            else if (MgLogManager::UserParam == param)
            {
                AddUserName(logEntry);
            }
            else if (MgLogManager::ErrorParam == param)
            {
                AddError(logEntry, entry);
            }
            else if (MgLogManager::StackTraceParam == param)
            {
                AddStackTrace(logEntry, stackTrace);
            }
        }
    }

    // Add the given info.
    logEntry += entry;

    MG_LOGMANAGER_CATCH(L"MgLogManager.LogErrorEntry")

    if (mgException != NULL)
    {
        // Use default message with just the given info.
        // TODO: Create actual default message when it's known what will go in this log
        logEntry = entry;
    }

    WriteLogMessage(mltError, logEntry, LM_ERROR);
}

void MgLogManager::LogSessionEntry(const MgSessionInfo& sessionInfo)
{
    // Message to be entered into the log
    STRING logEntry;

    MG_LOGMANAGER_TRY()

    // Parse parameter string into an MgStringCollection
    Ptr<MgStringCollection> paramList = MgStringCollection::ParseCollection(
        GetSessionLogParameters().c_str(), L",");

    // Go through parameter list and add the information to the message appropriately
    if (paramList != NULL)
    {
        INT32 numParams = paramList->GetCount();
        STRING param;

        for (INT32 i = 0; i < numParams; ++i)
        {
            param = paramList->GetItem(i);

            if (MgLogManager::ClientParam == param)
            {
                AddString(logEntry, sessionInfo.GetClient());
            }
            else if (MgLogManager::ClientIpParam == param)
            {
                AddString(logEntry, sessionInfo.GetClientIp());
            }
            else if (MgLogManager::UserParam == param)
            {
                AddString(logEntry, sessionInfo.GetUser());
            }
            else if (MgLogManager::StartTimeParam == param)
            {
                AddDateTime(logEntry, sessionInfo.GetStartTime());
            }
            else if (MgLogManager::EndTimeParam == param)
            {
                AddDateTime(logEntry, sessionInfo.GetEndTime());
            }
            else if (MgLogManager::OpsFailedParam == param)
            {
                AddInt32(logEntry, sessionInfo.GetNumberOfOperationsFailed());
            }
            else if (MgLogManager::OpsReceivedParam == param)
            {
                AddInt32(logEntry, sessionInfo.GetNumberOfOperationsReceived());
            }
            else if (MgLogManager::AverageOpTimeParam == param)
            {
                AddDouble(logEntry, sessionInfo.GetAverageOperationTime());
            }
        }
    }

    MG_LOGMANAGER_CATCH(L"MgLogManager.LogSessionEntry")

    if (NULL == mgException)
    {
        WriteLogMessage(mltSession, logEntry, LM_INFO);
    }
    else
    {
        logEntry = L"Unable to log session message.";
        LogErrorEntry(logEntry, mgException->GetDetails());
    }
}

void MgLogManager::LogTraceEntry(CREFSTRING entry)
{
    // Message to be entered into the log
    STRING logEntry;

    MG_LOGMANAGER_TRY()

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

            if (MgLogManager::InfoParam == param)
            {
                AddInfo(logEntry, entry);
            }
        }
    }

    MG_LOGMANAGER_CATCH(L"MgLogManager.LogTraceEntry")

    if (mgException != NULL)
    {
        // Use default message with just the given info.
        // TODO: Create actual default message when it's known what will go in this log
        logEntry = entry;
    }

    WriteLogMessage(mltTrace, logEntry, LM_INFO);
}


MgPropertyCollection* MgLogManager::EnumerateLogs()
{
    Ptr<MgPropertyCollection> logs =  new MgPropertyCollection(true, true);
    Ptr<MgStringProperty> pProperty;

    // Open the logs directory
    ACE_DIR* directory = ACE_OS::opendir(MG_WCHAR_TO_TCHAR(m_path));
    if (directory == NULL)
    {
        MgStringCollection arguments;
        arguments.Add(m_path);
        throw new MgFileIoException(L"MgLogManager.EnumerateLogs", __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    MG_LOGMANAGER_TRY()
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
            pProperty = new MgStringProperty(MgLogManager::LogNameProperty, name);
            logs->Add(pProperty);

            // Add the log type
            wstring type = ReadLogTypeFromLogFile(path);
            pProperty = new MgStringProperty(MgLogManager::LogTypeProperty, type);
            logs->Add(pProperty);

            // Add the log status
            wstring status = this->DetermineLogFileStatus(name, type);
            pProperty = new MgStringProperty(MgLogManager::LogStatusProperty, status);
            logs->Add(pProperty);

        }
    }

    ACE_OS::closedir(directory);

    MG_LOGMANAGER_CATCH(L"MgLogManager.EnumerateLogs")
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


void MgLogManager::RenameLog(CREFSTRING oldFileName, CREFSTRING newFileName)
{
    if (oldFileName.empty() || newFileName.empty())
    {
        throw new MgNullArgumentException(L"MgLogManager.RenameLog", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MG_LOGMANAGER_TRY()
    MgFileUtil::RenameFile(m_path, oldFileName, newFileName);
    MG_LOGMANAGER_CATCH_AND_THROW(L"MgLogManager.RenameLog")
}

void MgLogManager::DeleteLog(CREFSTRING fileName)
{
    if (fileName.empty())
    {
        throw new MgNullArgumentException(L"MgLogManager.DeleteLog", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    if (fileName.find(L"/") != string::npos ||
        fileName.find(L"\\") != string::npos)
    {
        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(fileName);

        throw new MgInvalidArgumentException(L"MgLogManager.DeleteLog",
            __LINE__, __WFILE__, &arguments, L"MgStringContainsReservedCharacters", NULL);
    }

    MG_LOGMANAGER_TRY()
    MgFileUtil::DeleteFile(m_path+fileName);
    MG_LOGMANAGER_CATCH_AND_THROW(L"MgLogManager.DeleteLog")
}

void MgLogManager::WriteLogMessage(enum MgLogType logType, CREFSTRING message, ACE_Log_Priority logPriority)
{
    ACE_Log_Msg* pAce = ACE_Log_Msg::instance();

    // System message gets logged to event log
    if (logType == mltSystem)
    {
        pAce->acquire();

        MG_LOGMANAGER_TRY()

        LogToSysLog((char *)(MgUtil::WideCharToMultiByte(m_applicationName)).c_str());
#ifdef _WIN32
        pAce->log(logPriority, ACE_TEXT("%W\r\n"), message.c_str());
#else
        pAce->log(logPriority, ACE_TEXT("%W\n"), message.c_str());
#endif
        LogToStderr();

        MG_LOGMANAGER_CATCH(L"MgLogManager.WriteLogMessage")

        if (mgException != NULL)
        {
            STRING entry = L"Unable to log system message";
            LogErrorEntry(entry);
        }

        pAce->release();
    }
    // All other messages will get logged to their respective files
    else
    {
        STRING filename;
        STRING logTypeName;
        STRING logParameters;

        switch (logType)
        {
        case mltAccess:
            // Get the access log filename and path
            filename = BuildFileName(m_AccessLogFileName).c_str();
            logTypeName = MgLogManager::AccessLog;
            logParameters = GetAccessLogParameters();
            break;
        case mltAdmin:
            // Get the admin log filename and path
            filename = BuildFileName(m_AdminLogFileName).c_str();
            logTypeName = MgLogManager::AdminLog;
            logParameters = GetAdminLogParameters();
            break;
        case mltAuthentication:
            // Get the authentication log filename and path
            filename = BuildFileName(m_AuthenticationLogFileName).c_str();
            logTypeName = MgLogManager::AuthenticationLog;
            logParameters = GetAuthenticationLogParameters();
            break;
        case mltError:
            // Get the error log filename and path
            filename = BuildFileName(m_ErrorLogFileName).c_str();
            logTypeName = MgLogManager::ErrorLog;
            logParameters = GetErrorLogParameters();
            break;
        case mltSession:
            // Get the session log filename and path
            filename = BuildFileName(m_SessionLogFileName).c_str();
            logTypeName = MgLogManager::SessionLog;
            logParameters = GetSessionLogParameters();
            break;
        case mltTrace:
            // Get the trace log file and path
            filename = BuildFileName(m_TraceLogFileName).c_str();
            logTypeName = MgLogManager::TraceLog;
            logParameters = GetTraceLogParameters();
            break;
        }

        {
            ACE_MT (ACE_GUARD (ACE_Recursive_Thread_Mutex, ace_mon, m_mutex));
            pAce->acquire();

            MG_LOGMANAGER_TRY()

            ValidateLogHeaders();

            if (false == CheckArchiveFrequency(logType, filename))
            {
                ArchiveLog(filename);
            }

            m_logStream.open(MgUtil::WideCharToMultiByte(filename).c_str(), ios::out | ios::app | ios::binary);

            if (!m_logStream.is_open())
            {
                MgStringCollection arguments;
                arguments.Add(filename);
                throw new MgFileIoException(L"MgLogManager.WriteLogMessage", __LINE__, __WFILE__, &arguments, L"", NULL);
            }

            LogToOStream(&m_logStream);

            // Write file header information if the log is empty
            bool bLogFileEmpty = true;
            bLogFileEmpty =  (MgFileUtil::GetFileSize(filename) ? false : true);

            if (bLogFileEmpty)
            {
                STRING headerLine1 = MgLogManager::HeaderLine1 + L" ";
                headerLine1 += logTypeName;
                STRING headerLine2 = MgLogManager::HeaderLine2 + L" ";
                headerLine2 += logParameters;
#ifdef _WIN32
                pAce->log(logPriority, ACE_TEXT("%W\r\n"), headerLine1.c_str());
                pAce->log(logPriority, ACE_TEXT("%W\r\n"), headerLine2.c_str());
#else
                pAce->log(logPriority, ACE_TEXT("%W\n"), headerLine1.c_str());
                pAce->log(logPriority, ACE_TEXT("%W\n"), headerLine2.c_str());
#endif
            }  // end write file header

            // Get the current time in XML standard format.
            MgDateTime currTime;
            currTime.SetMicrosecond(0);
            STRING logTime = currTime.ToXmlString(false);

#ifdef _WIN32
            pAce->log(logPriority, ACE_TEXT("<%W> %W\r\n"), logTime.c_str(), message.c_str());
#else
            pAce->log(logPriority, ACE_TEXT("<%W> %W\n"), logTime.c_str(), message.c_str());
#endif
            m_logStream.close();
            LogToStderr();

            MG_LOGMANAGER_CATCH(L"MgLogManager.WriteLogMessage")

            if (mgException != 0 && logType != mltError)
            {
                STRING entry = L"Unable to log message to " + filename;
                LogErrorEntry(entry);
            }

            // Check if log size has exceeded the maximum size
            if (IsMaximumLogSizeEnabled() && IsMaxSizeExceeded(filename))
            {
                // Archive the current log
                ArchiveLog(filename);
            }

            pAce->release();
        }
    } // end else
}

bool MgLogManager::RemoveLogFile(CREFSTRING filename)
{
    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_mutex, false));

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

MgByteReader* MgLogManager::GetLogContents(CREFSTRING filename)
{
    Ptr<MgByteReader> byteReader;
    byteReader = NULL;
    FILE* pReadFile = NULL;
    string contents = "";

    MG_LOGMANAGER_TRY()

    {
        ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_mutex, NULL));

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
                throw new MgOutOfMemoryException(L"MgLogManager.GetLogContents", __LINE__, __WFILE__, NULL, L"", NULL);
            }

            // Clear buffer
            memset(buffer, 0, (length+1)*sizeof(char));

            ACE_OS::fread(buffer, sizeof(char), length*sizeof(char), pReadFile);
            contents = buffer;

            delete [] buffer;

            ACE_OS::fclose(pReadFile);
        }
    }

    byteReader = MgUtil::GetByteReader(contents);

    MG_LOGMANAGER_CATCH(L"MgLogManager.GetLogContents")
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

MgByteReader* MgLogManager::GetLogContents(CREFSTRING filename, INT32 numEntries)
{
    Ptr<MgByteReader> byteReader;
    byteReader = NULL;
    FILE* pReadFile = NULL;
    string contents = "";

    MG_LOGMANAGER_TRY()

    if (numEntries < 0)
    {
        throw new  MgArgumentOutOfRangeException(L"MgLogManager.GetLogContents", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    INT32 lastPos;      // Keeps track of the position in the file
    INT32 numNewLines;  // Keep track of the number of entries found so far
    char buffer[MAX_BUF+1];
    bool bReadMore;

    {
        ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_mutex, NULL));

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
    }


    byteReader = MgUtil::GetByteReader(contents);

    MG_LOGMANAGER_CATCH(L"MgLogManager.GetLogContents")
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

MgByteReader* MgLogManager::GetLogContents(enum MgLogType logType, MgDateTime* fromDate, MgDateTime* toDate)
{
    if (fromDate == NULL || toDate == NULL)
    {
        throw new MgNullArgumentException(L"MgLogManager.GetLogContents", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    if (*fromDate > *toDate)
    {
        MgStringCollection arguments;
        arguments.Add(L"2");
        arguments.Add(fromDate->ToString());

        throw new MgInvalidArgumentException(L"MgLogManager.GetLogContents",
            __LINE__, __WFILE__, &arguments, L"MgInvalidFromDate", NULL);
    }

    if (IsMoreThan24HourDiff(fromDate, toDate))
    {
        MgStringCollection arguments;
        arguments.Add(L"2");
        arguments.Add(fromDate->ToString());
        arguments.Add(L"3");
        arguments.Add(toDate->ToString());

        throw new MgInvalidArgumentException(L"MgLogManager.GetLogContents",
            __LINE__, __WFILE__, &arguments, L"MgInvalidDateDifference", NULL);
    }

    Ptr<MgByteReader> byteReader;
    byteReader = NULL;
    FILE* pReadFile = NULL;

    MG_LOGMANAGER_TRY()

    Ptr<MgStringCollection> filenames;
    filenames = NULL;
    Ptr<MgStringCollection> entries;
    entries = NULL;

    STRING currentFilename;
    string contents = "";

    // Get the potential file names
    filenames = DeterminePotentialFileNames(logType, fromDate, toDate);
    if (filenames->GetCount() < 1)
    {
        MgStringCollection arguments;
        arguments.Add(L"2");
        arguments.Add(fromDate->ToString());
        arguments.Add(L"3");
        arguments.Add(toDate->ToString());

        throw new MgInvalidArgumentException(L"MgLogManager.GetLogContents",
             __LINE__, __WFILE__, &arguments, L"MgFailedToGetFileNameForDates", NULL);
    }

    entries = new MgStringCollection();
    if (entries == NULL)
    {
        throw new MgOutOfMemoryException(L"MgLogManager.GetLogContents", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    bool fromDateFound = false;
    bool toDateFound = false;

    INT32 fromLine = 0;
    INT32 toLine = 0;
    INT32 i = 0;
    INT32 j = 0;

    char buffer[MAX_BUF+1];
    string logEntry = "";                // current entry read from file
    STRING wEntry = L"";               // current entry read from file made into a STRING

    INT32 nSearchedFiles = 0;
    while (nSearchedFiles < filenames->GetCount())
    {
        currentFilename = filenames->GetItem(nSearchedFiles);
        nSearchedFiles++;   // increment the number of searched files

        pReadFile = ACE_OS::fopen(MG_WCHAR_TO_TCHAR(currentFilename), ACE_TEXT("rb"));
        if (pReadFile != NULL)
        {
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
                }

                char* entryBegin = buffer;
                for (i = 0; i < MAX_BUF; i++)
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

    MG_LOGMANAGER_CATCH(L"MgLogManager.GetLogContents");
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

MgStringCollection* MgLogManager::DeterminePotentialFileNames(enum MgLogType logType, MgDateTime* fromDate, MgDateTime* toDate)
{
    Ptr<MgStringCollection> filenames;

    filenames = new MgStringCollection();

    if (filenames == NULL)
    {
        throw new MgOutOfMemoryException(L"MgLogManager.GetLogContents", __LINE__, __WFILE__, NULL, L"", NULL);
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

            throw new MgInvalidArgumentException(L"MgLogManager.GetLogContents",
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

STRING MgLogManager::BuildFileName(CREFSTRING filename)
{
    STRING newFilename = filename.c_str();

    newFilename = RemoveArchiveFrequencySpecifier(newFilename);

    return (m_path + newFilename);
}

STRING MgLogManager::BuildFileNameFromDateTime(CREFSTRING filename, MgDateTime* date)
{
    STRING newFilename = filename;
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

    return (m_path + newFilename);
}

bool MgLogManager::IsMoreThan24HourDiff(MgDateTime* fromDate, MgDateTime* toDate)
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

INT32 MgLogManager::SearchClosestDateAfter(MgStringCollection* lines, MgDateTime* searchDate)
{
    INT32 result = -1;

    if (lines == NULL || searchDate == NULL)
    {
        throw new MgNullArgumentException(L"MgLogManager.SearchClosestDateAfter", __LINE__, __WFILE__, NULL, L"", NULL);
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

    MG_LOGMANAGER_CATCH_AND_THROW(L"MgLogManager.SearchClosestDateAfter");

    return result;
}

INT32 MgLogManager::SearchClosestDateBefore(MgStringCollection* lines, MgDateTime* searchDate)
{
    INT32 result = -1;

    if (lines == NULL || searchDate == NULL)
    {
        throw new MgNullArgumentException(L"MgLogManager.SearchClosestDateBefore", __LINE__, __WFILE__, NULL, L"", NULL);
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

    MG_LOGMANAGER_CATCH_AND_THROW(L"MgLogManager.SearchClosestDateBefore");

    return result;
}

MgDateTime* MgLogManager::GetDateTimeFromEntry(CREFSTRING entry)
{
    Ptr<MgDateTime> dateTime;

    MG_LOGMANAGER_TRY()

    // Each entry starts with the log time that conforms to XML standard and
    // is enclosed by angle brackets, i.e. <CCYY-MM-DDThh:mm:ss>
    if (entry.find_first_of(L'<') != 0  || entry.find_first_of(L'>') != 20)
    {
        throw new MgInvalidLogEntryException(
            L"MgLogManager.GetDateTimeFromEntry",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    string logTime;
    MgUtil::WideCharToMultiByte(entry.substr(1, 19), logTime);
    dateTime = new MgDateTime(logTime);

    MG_LOGMANAGER_CATCH_AND_THROW(L"MgLogManager.GetDateTimeFromEntry");

    return dateTime.Detach();
}

void MgLogManager::AddDateTime(REFSTRING entry, const MgDateTime& value)
{
    AddDelimiter(entry);
    entry += const_cast<MgDateTime&>(value).ToXmlString(false);
}

void MgLogManager::AddDouble(REFSTRING entry, double value)
{
    STRING buffer;

    AddDelimiter(entry);
    MgUtil::DoubleToString(value, buffer);
    entry += buffer;
}

void MgLogManager::AddInt32(REFSTRING entry, INT32 value)
{
    STRING buffer;

    AddDelimiter(entry);
    MgUtil::Int32ToString(value, buffer);
    entry += buffer;
}

void MgLogManager::AddString(REFSTRING entry, CREFSTRING value)
{
    AddDelimiter(entry);
    entry += value;
}

void MgLogManager::AddClient(REFSTRING entry)
{
    AddDelimiter(entry);

    // Get client version. This needs to come from the web tier.
    // For logs involving operations, this will be stored in MgUserInformation.
    // Otherwise, for session logging, the info will be retrieved from the connection object.
    MgUserInformation* pUserInfo = MgUserInformation::GetCurrentUserInfo();

    if (NULL != pUserInfo && pUserInfo->GetClientAgent().length() > 0)
    {
        entry += pUserInfo->GetClientAgent();
    }
    else
    {
        MgConnection* pConnection = MgConnection::GetCurrentConnection();

        if (NULL != pConnection)
        {
            entry += pConnection->GetClientAgent();
        }
    }
}

void MgLogManager::AddClientIp(REFSTRING entry)
{
    AddDelimiter(entry);

    // Get client IP. This needs to come from the web tier.
    // For logs involving operations this will be stored in MgUserInformation.
    // Otherwise, for session logging, the info will be retrieved from the connection object.
    MgUserInformation* pUserInfo = MgUserInformation::GetCurrentUserInfo();

    if (NULL != pUserInfo && pUserInfo->GetClientIp().length() > 0)
    {
        entry += pUserInfo->GetClientIp();
    }
    else
    {
        MgConnection* pConnection = MgConnection::GetCurrentConnection();

        if (NULL != pConnection)
        {
            entry += pConnection->GetClientIp();
        }
    }
}

void MgLogManager::AddDuration(REFSTRING entry)
{
    AddDelimiter(entry);

    double connectionTime = 0;

    MgConnection* pConnection = MgConnection::GetCurrentConnection();
    if (pConnection != NULL)
    {
        // Get connection duration (milliseconds)
        connectionTime = pConnection->GetConnectionTime();
    }

    // Convert milliseconds to seconds
    char duration[256];
    duration[0] = 0;
    sprintf(duration, "%.6f", connectionTime/1000);

    entry += MgUtil::MultiByteToWideChar(string(duration));
}

void MgLogManager::AddError(REFSTRING entry, CREFSTRING error)
{
/*
    AddDelimiter(entry);

    STRING err = MgUtil::ReplaceString(error, L"\n", L" ");
    entry += MgResources::Error + L":" + err + L" ";
*/
#ifdef WIN32
    entry += L"\r\n " + MgResources::Error + L": ";
#else
    entry += L"\n " + MgResources::Error + L": ";
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
#ifdef WIN32
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

void MgLogManager::AddStackTrace(REFSTRING entry, CREFSTRING stackTrace)
{
#ifdef WIN32
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
#ifdef WIN32
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

void MgLogManager::AddInfo(REFSTRING entry, CREFSTRING info)
{
    AddDelimiter(entry);
    entry += info;
}

void MgLogManager::AddOpId(REFSTRING entry, CREFSTRING opId)
{
    AddDelimiter(entry);
    entry += opId;
}

void MgLogManager::AddOpsProcessed(REFSTRING entry)
{
    AddDelimiter(entry);

    INT32 nOperations = 0;

    MgConnection* pConnection = MgConnection::GetCurrentConnection();
    if (pConnection != NULL)
    {
        nOperations = pConnection->GetProcessedOperations();
    }

    string opsProcessed = "";
    MgUtil::Int32ToString(nOperations, opsProcessed);
    entry += MgUtil::MultiByteToWideChar(opsProcessed);
}

void MgLogManager::AddOpsReceived(REFSTRING entry)
{
    AddDelimiter(entry);

    INT32 nOperations = 0;

    MgConnection* pConnection = MgConnection::GetCurrentConnection();
    if (pConnection != NULL)
    {
        nOperations = pConnection->GetReceivedOperations();
    }

    string opsReceived = "";
    MgUtil::Int32ToString(nOperations, opsReceived);
    entry += MgUtil::MultiByteToWideChar(opsReceived);
}

void MgLogManager::AddUserName(REFSTRING entry)
{
    AddDelimiter(entry);

    // Get user.
    MgUserInformation* pUserInfo = MgUserInformation::GetCurrentUserInfo();
    MgConnection* pConnection = MgConnection::GetCurrentConnection();
    if (pUserInfo && pUserInfo->GetUserName().length() > 0)
    {
        entry += pUserInfo->GetUserName();
    }
    else if (pConnection!= NULL)
    {
        entry += pConnection->GetUserName();
    }
}

bool MgLogManager::IsMaxSizeExceeded(CREFSTRING logFileName)
{
    bool bMaxSizeReached = false;

    INT64 nFileSize = MgFileUtil::GetFileSize(logFileName);

    if (nFileSize >= (MgLogManager::m_maxLogSize * 1024))
    {
        bMaxSizeReached = true;
    }

    return bMaxSizeReached;
}

void MgLogManager::ArchiveLog(CREFSTRING logFileName)
{
    ACE_MT (ACE_GUARD (ACE_Recursive_Thread_Mutex, ace_mon, m_mutex));

    if (MgFileUtil::PathnameExists(logFileName))
    {
        // The archived named will be the current filename suffixed with the current date and time of the archive
        STRING archiveName = L"";

        // Cache the current extension
        size_t index = logFileName.rfind(L".");
        STRING logFileExt = L"";
        logFileExt = logFileName.substr(index);

        // Iterate until a unique archive name is available (maximum of 10 iterations).
        const int nMaxIterations = 10;
        int nIterations = 0;

        do
        {
            archiveName.clear();
            archiveName = logFileName.substr(0, index);

            // Add the date and time suffix to the archive name.
            ACE_Date_Time date_time;
            INT32 year = (INT32)date_time.year();
            INT32 month = (INT32)date_time.month();
            INT32 day = (INT32)date_time.day();
            INT32 hour = (INT32)date_time.hour();
            INT32 minute = (INT32)date_time.minute();
            INT32 second = (INT32)date_time.second();
            INT32 microsec = (INT32)date_time.microsec();

            string strYear;
            string strMonth;
            string strDay;
            string strHour;
            string strMinute;
            string strSecond;
            string strMicrosec;
            MgUtil::Int32ToString(year, strYear);
            MgUtil::Int32ToString(month, strMonth);
            MgUtil::Int32ToString(day, strDay);
            MgUtil::Int32ToString(hour, strHour);
            MgUtil::Int32ToString(minute, strMinute);
            MgUtil::Int32ToString(second, strSecond);
            MgUtil::Int32ToString(microsec, strMicrosec);

            archiveName.append(L"_");
            archiveName.append(MgUtil::MultiByteToWideChar(strYear));
            archiveName.append(MgUtil::MultiByteToWideChar(strMonth));
            archiveName.append(MgUtil::MultiByteToWideChar(strDay));
            archiveName.append(L"_");
            archiveName.append(MgUtil::MultiByteToWideChar(strHour));
            archiveName.append(MgUtil::MultiByteToWideChar(strMinute));
            archiveName.append(MgUtil::MultiByteToWideChar(strSecond));
            archiveName.append(MgUtil::MultiByteToWideChar(strMicrosec));
            archiveName.append(logFileExt);

            nIterations++;

        } while( MgFileUtil::PathnameExists(archiveName) && (nIterations < nMaxIterations) );

        // Create the archive
        MgFileUtil::RenameFile(logFileName, archiveName);
    }
}

// Walks the existing active log files and check if the parameters list.
// If the parameters in the header do not match the current logging parameters, the log is archived
// so that new log will be created based on the current logging parameters.
//
void MgLogManager::ValidateLogHeaders()
{
    STRING filename = L"";

    // Check Access Log
    if (!ValidateAccessLogHeader())
    {
        filename = BuildFileName(m_AccessLogFileName).c_str();
        ArchiveLog(filename);
    }

    // Check Admin Log
    if (!ValidateAdminLogHeader())
    {
        filename = BuildFileName(m_AdminLogFileName).c_str();
        ArchiveLog(filename);
    }

    // Check Authentication Log
    if (!ValidateAuthenticationLogHeader())
    {
        filename = BuildFileName(m_AuthenticationLogFileName).c_str();
        ArchiveLog(filename);
    }

    // Check Error Log
    if (!ValidateErrorLogHeader())
    {
        filename = BuildFileName(m_ErrorLogFileName).c_str();
        ArchiveLog(filename);
    }

    // Check Session Log
    if (!ValidateSessionLogHeader())
    {
        filename = BuildFileName(m_SessionLogFileName).c_str();
        ArchiveLog(filename);
    }

    // Check Trace Log
    if (!ValidateTraceLogHeader())
    {
        filename = BuildFileName(m_TraceLogFileName).c_str();
        ArchiveLog(filename);
    }
}

bool MgLogManager::ValidateAccessLogHeader()
{
    bool bValid = false;

    MG_LOGMANAGER_TRY()

    // Compare the log file parameters list with the current logging parameters
    STRING logFileParameters = ReadParametersFromLogFile(mltAccess);
    STRING currentLogParams = GetAccessLogParameters();
    if (0 == logFileParameters.compare(currentLogParams))
    {
        bValid = true;
    }

    MG_LOGMANAGER_CATCH(L"MgLogManager.ValidateAccessLogHeader")

    return bValid;
}

bool MgLogManager::ValidateAdminLogHeader()
{
    bool bValid = false;

    MG_LOGMANAGER_TRY()

    // Compare the log file parameters list with the current logging parameters
    STRING logFileParameters = ReadParametersFromLogFile(mltAdmin);
    STRING currentLogParams = GetAdminLogParameters();
    if (0 == logFileParameters.compare(currentLogParams))
    {
        bValid = true;
    }

    MG_LOGMANAGER_CATCH(L"MgLogManager.ValidateAdminLogHeader")

    return bValid;
}

bool MgLogManager::ValidateAuthenticationLogHeader()
{
    bool bValid = false;

    MG_LOGMANAGER_TRY()

    // Compare the log file parameters list with the current logging parameters
    STRING logFileParameters = ReadParametersFromLogFile(mltAuthentication);
    STRING currentLogParams = GetAuthenticationLogParameters();
    if (0 == logFileParameters.compare(currentLogParams))
    {
        bValid = true;
    }

    MG_LOGMANAGER_CATCH(L"MgLogManager.ValidateAuthenticationLogHeader")

    return bValid;
}

bool MgLogManager::ValidateErrorLogHeader()
{
    bool bValid = false;

    MG_LOGMANAGER_TRY()

    // Compare the log file parameters list with the current logging parameters
    STRING logFileParameters = ReadParametersFromLogFile(mltError);
    STRING currentLogParams = GetErrorLogParameters();
    if (0 == logFileParameters.compare(currentLogParams))
    {
        bValid = true;
    }

    MG_LOGMANAGER_CATCH(L"MgLogManager.ValidateErrorLogHeader")

    return bValid;
}

bool MgLogManager::ValidateSessionLogHeader()
{
    bool bValid = false;

    MG_LOGMANAGER_TRY()

    // Compare the log file parameters list with the current logging parameters
    STRING logFileParameters = ReadParametersFromLogFile(mltSession);
    STRING currentLogParams = GetSessionLogParameters();
    if (0 == logFileParameters.compare(currentLogParams))
    {
        bValid = true;
    }

    MG_LOGMANAGER_CATCH(L"MgLogManager.ValidateSessionLogHeader")

    return bValid;
}

bool MgLogManager::ValidateTraceLogHeader()
{
    bool bValid = false;

    MG_LOGMANAGER_TRY()

    // Compare the log file parameters list with the current logging parameters
    STRING logFileParameters = ReadParametersFromLogFile(mltTrace);
    STRING currentLogParams = GetTraceLogParameters();
    if (0 == logFileParameters.compare(currentLogParams))
    {
        bValid = true;
    }

    MG_LOGMANAGER_CATCH(L"MgLogManager.ValidateTraceLogHeader")

    return bValid;
}

STRING MgLogManager::ReadParametersFromLogFile(enum MgLogType logType)
{
    STRING logParameters = L"";

    MG_LOGMANAGER_TRY()

    Ptr<MgByteReader> bytes;

    switch(logType)
    {
    case mltAccess:
        bytes = GetAccessLog();
        break;
    case mltAdmin:
        bytes = GetAdminLog();
        break;
    case mltAuthentication:
        bytes = GetAuthenticationLog();
        break;
    case mltError:
        bytes = GetErrorLog();
        break;
    case mltSession:
        bytes = GetSessionLog();
        break;
    case mltTrace:
        bytes = GetTraceLog();
        break;
    default:
        {
            STRING buffer;
            MgUtil::Int32ToString(logType, buffer);

            MgStringCollection arguments;
            arguments.Add(L"1");
            arguments.Add(buffer);

            throw new MgInvalidArgumentException(L"MgLogManager.ReadParametersFromLogFile",
                __LINE__, __WFILE__, &arguments, L"MgInvalidLogType", NULL);
        }
    }

    if (bytes->GetLength() > 0)
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
            string strHeaderPrefix = MgUtil::WideCharToMultiByte(MgLogManager::HeaderLine2);
            if (0 == strLine2.compare(0, strHeaderPrefix.length(), strHeaderPrefix))
            {
                // The prefix for the header line is OK.  Now lets get the parameters
                string strParams = strLine2.substr(strHeaderPrefix.length() + 1);
                logParameters = MgUtil::MultiByteToWideChar(strParams);
            }
        }
    }

    MG_LOGMANAGER_CATCH(L"MgLogManager.ReadParametersFromLogFile")

    return logParameters;
}

STRING MgLogManager::ReadLogTypeFromLogFile(CREFSTRING logFilename)
{
    STRING logType = MgLogManager::UnspecifiedLog;

    MG_LOGMANAGER_TRY()

    Ptr<MgByteReader> bytes;
    bytes = GetLogContents(logFilename);

    if (bytes->GetLength() > 0)
    {
        char buffer[4096] = {0};
        bytes->Read((BYTE_ARRAY_OUT)&buffer, 4095);
        string strBuffer;
        strBuffer.assign(buffer);

        // Find the first line in the log
        size_t startLine1 = 0;
        size_t endLine1 = strBuffer.find("\n");
        if (0 != endLine1)
        {
            string strLineBuf = strBuffer.substr(startLine1, endLine1);

            // Check if there is a carriage return (\r) at the end of the line
            size_t endLineBuf = strLineBuf.rfind("\r");
            string strLine1 = strLineBuf.substr(0, endLineBuf);

            // Attempt to extract the log type
            string strHeaderPrefix = MgUtil::WideCharToMultiByte(MgLogManager::HeaderLine1);
            if (0 == strLine1.compare(0, strHeaderPrefix.length(), strHeaderPrefix))
            {
                // The prefix for the header line is OK.  Now lets get the type
                string strType = strLine1.substr(strHeaderPrefix.length() + 1);
                logType = MgUtil::MultiByteToWideChar(strType);
            }
        }

    }

    MG_LOGMANAGER_CATCH(L"MgLogManager.ReadLogTypeFromLogFile")

    return logType;
}

STRING MgLogManager::DetermineLogFileStatus(CREFSTRING logFilename, CREFSTRING logFileType)
{
    STRING logStatus = L"";

    MG_LOGMANAGER_TRY()

    STRING currentLogName = L"";

    // Get the name being used for the current log
    if (0 == logFileType.compare(MgLogManager::AccessLog))
    {
        currentLogName = GetAccessLogFileName();
    }
    else if (0 == logFileType.compare(MgLogManager::AdminLog))
    {
        currentLogName = GetAdminLogFileName();
    }
    else if (0 == logFileType.compare(MgLogManager::AuthenticationLog))
    {
        currentLogName = GetAuthenticationLogFileName();
    }
    else if (0 == logFileType.compare(MgLogManager::ErrorLog))
    {
        currentLogName = GetErrorLogFileName();
    }
    else if (0 == logFileType.compare(MgLogManager::SessionLog))
    {
        currentLogName= GetSessionLogFileName();
    }
    else if (0 == logFileType.compare(MgLogManager::TraceLog))
    {
        currentLogName = GetTraceLogFileName();
    }
    else if (0 == logFileType.compare(MgLogManager::UnspecifiedLog))
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

        throw new MgInvalidArgumentException(L"MgLogManager.DetermineLogFileStatus",
            __LINE__, __WFILE__, &arguments, L"MgInvalidLogType", NULL);
    }

    currentLogName = RemoveArchiveFrequencySpecifier(currentLogName);

    // Compare the current log name with the filename to determine its status.
    if (0 == logFilename.compare(currentLogName))
    {
        logStatus = MgLogManager::LogStatusActive;
    }
    else
    {
        logStatus = MgLogManager::LogStatusArchive;
    }

    MG_LOGMANAGER_CATCH(L"MgLogManager.DetermineLogFileStatus")

    return logStatus;
}

void MgLogManager::AddDelimiter(REFSTRING entry)
{
    entry += m_delimiter;
}

// Check if the delimiter is any one of the reserved characters ( \t \r \n \f \v ).
// If found, it will need to be translated to actually write the character.
void MgLogManager::TranslateDelimiter()
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

void MgLogManager::SetMaximumLogSize(INT32 size)
{
    m_maxLogSize = size;
}

void MgLogManager::SetLogDelimiter(CREFSTRING delimiter)
{
    m_delimiter = delimiter;
}

void MgLogManager::EnableMaximumLogSize(bool useMaxSize)
{
    m_useMaxLogSize = useMaxSize;
}

bool MgLogManager::IsMaximumLogSizeEnabled()
{
    return m_useMaxLogSize;
}

bool MgLogManager::CheckArchiveFrequency(enum MgLogType logType, CREFSTRING logFilename)
{
    bool bCurrentLog = true;

    // Get the frequency specifier from the filename
    STRING rawFilename;

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
        rawFilename = m_ErrorLogFileName;
        break;
    case mltSession:
        rawFilename = m_SessionLogFileName;
        break;
    case mltTrace:
        rawFilename = m_TraceLogFileName;
        break;
    }

    STRING specifier;
    STRING::size_type specifierIndex = rawFilename.find_first_of(L'%');
    if (specifierIndex != string::npos)
    {
        specifier = rawFilename[specifierIndex + 1];
    }


    // Get the timestamp of the current log file
    if (MgFileUtil::PathnameExists(logFilename))
    {
        Ptr<MgDateTime> logTimestamp = new MgDateTime(MgFileUtil::GetFileModificationTime(logFilename));
        STRING str = logTimestamp->ToString();

        // Compare the timestamp to the current time
        MgDateTime currentTime;

        // If the frequency boundary has been crossed, the file needs to be archived
        if (specifier == L"d")
        {
            if ( logTimestamp->GetYear() <= currentTime.GetYear() 
                && logTimestamp->GetMonth() <= currentTime.GetMonth() )
            {
                if ( logTimestamp->GetDay() != currentTime.GetDay() )
                {
                    bCurrentLog = false;
                }
            }
        }
        else if (specifier == L"m")
        {
            if ( logTimestamp->GetYear() <= currentTime.GetYear() )
            {
                if (logTimestamp->GetMonth() != currentTime.GetMonth())
                {
                    bCurrentLog = false;
                }
            }
        }
        else if (specifier == L"y")
        {
            if ( logTimestamp->GetYear() < currentTime.GetYear() )
            {
                bCurrentLog = false;
            }
        }
    }

    return bCurrentLog;
}

STRING MgLogManager::RemoveArchiveFrequencySpecifier(CREFSTRING logFilename)
{
    STRING newFileName;

    // Remove the archive frequency specifier from the filename, if it is present
    newFileName = MgUtil::ReplaceString(logFilename, L"%y", L"");     // NOXLATE
    newFileName = MgUtil::ReplaceString(newFileName, L"%m", L"");     // NOXLATE
    newFileName = MgUtil::ReplaceString(newFileName, L"%d", L"");     // NOXLATE

    return newFileName;
}
