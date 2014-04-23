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

#ifndef MG_LOG_MANAGER_H_
#define MG_LOG_MANAGER_H_

// Undefine conflicting macros on Linux only.
#ifndef _WIN32
#undef min
#undef max
#endif

#include <fstream>
//#include "ServerManager.h"
//#include "SessionManager.h"
#include "LogType.h"
//#include "SessionInfo.h"
//#include "Connection.h"

class MgdLogThread;

#define MG_DESKTOP_LOG_EXCEPTION() \
    STRING locale = MgResources::DefaultMessageLocale; \
    STRING message = mgException->GetExceptionMessage(locale); \
    STRING details = mgException->GetDetails(locale); \
    STRING stackTrace = mgException->GetStackTrace(locale); \
    MG_LOG_EXCEPTION_ENTRY(message.c_str(), stackTrace.c_str()) \

///////////////////////////////////////////////////////////////////////////////
/// Mg Log Manager try/catch/throw macros.
///
#define MG_LOGMANAGER_TRY()                                                   \
    MG_TRY()                                                                  \

#define MG_LOGMANAGER_CATCH(methodName)                                       \
    MG_CATCH(methodName)                                                      \

#define MG_LOGMANAGER_THROW()                                                 \
    MG_THROW()                                                                \

#define MG_LOGMANAGER_CATCH_AND_THROW(methodName)                             \
    MG_CATCH_AND_THROW(methodName)                                            \

#define MG_LOG_SYSTEM_ENTRY(Priority, Entry) \
  do { \
    MgdLogManager* pMan = MgdLogManager::GetInstance(); \
    pMan->LogSystemEntry(Priority, Entry); \
  } while (0)

//TODO: Log macros need session information such as client ID, version etc.
#define MG_LOG_ACCESS_ENTRY(Entry, Client, ClientIp, UserName) \
  do { \
    MgdLogManager* pMan = MgdLogManager::GetInstance(); \
    if(pMan->IsAccessLogEnabled()) \
    { \
        pMan->LogAccessEntry(Entry, Client, ClientIp, UserName); \
    } \
  } while (0)

#define MG_LOG_ADMIN_ENTRY(Entry, Client, ClientIp, UserName) \
  do { \
    MgdLogManager* pMan = MgdLogManager::GetInstance(); \
    if(pMan->IsAdminLogEnabled()) \
    { \
        pMan->LogAdminEntry(Entry, Client, ClientIp, UserName); \
    } \
  } while (0)

#define MG_LOG_AUTHENTICATION_ENTRY(Entry) \
  do { \
    MgdLogManager* pMan = MgdLogManager::GetInstance(); \
    if(pMan->IsAuthenticationLogEnabled()) \
    { \
        MG_CONNECTION_INFO \
        pMan->LogAuthenticationEntry(Entry, connInfoClient.c_str(), connInfoClientIp.c_str(), connInfoUserName.c_str()); \
    } \
  } while (0)

#define MG_LOG_ERROR_ENTRY(Entry) \
  do { \
    MgdLogManager* pMan = MgdLogManager::GetInstance(); \
    if(pMan->IsErrorLogEnabled()) \
    { \
        MG_CONNECTION_INFO \
        pMan->LogError(Entry, connInfoClient.c_str(), connInfoClientIp.c_str(), connInfoUserName.c_str()); \
    } \
  } while (0)

#define MG_LOG_EXCEPTION_ENTRY(Entry, StackTrace) \
  do { \
    MgdLogManager* pMan = MgdLogManager::GetInstance(); \
    if(pMan->IsErrorLogEnabled()) \
    { \
        MG_CONNECTION_INFO \
        pMan->LogError(Entry, connInfoClient.c_str(), connInfoClientIp.c_str(), connInfoUserName.c_str(), StackTrace); \
    } \
  } while (0)

#define MG_LOG_WARNING_ENTRY(Service, Entry, StackTrace) \
  do { \
    MgdLogManager* pMan = MgdLogManager::GetInstance(); \
    MG_CONNECTION_INFO \
    pMan->LogWarning(Service, Entry, connInfoClient.c_str(), connInfoClientIp.c_str(), connInfoUserName.c_str(), StackTrace); \
  } while (0)

#define MG_LOG_TRACE_ENTRY(Entry) \
  do { \
    MgdLogManager* pMan = MgdLogManager::GetInstance(); \
    if(pMan->IsTraceLogEnabled()) \
    { \
        MG_CONNECTION_INFO \
        pMan->LogTraceEntry(Entry, connInfoClient.c_str(), connInfoClientIp.c_str(), connInfoUserName.c_str()); \
    } \
  } while (0)

#define MG_LOG_OPERATION_MESSAGE(Operation) \
    wchar_t bufferConversion[255]; \
    bufferConversion[0] = 0; \
    STRING operationMessage = Operation; \
    MG_CONNECTION_INFO

#define MG_LOG_OPERATION_MESSAGE_INIT(Version, Arguments) \
    operationMessage += L"."; \
    INT32 tempVersion = (Version & 0x00ff0000) >> 16; \
    ACE_OS::itoa(tempVersion, bufferConversion, 10); \
    operationMessage += bufferConversion; \
    operationMessage += L"."; \
    tempVersion = (Version & 0x0000ff00) >> 8; \
    ACE_OS::itoa(tempVersion, bufferConversion, 10); \
    operationMessage += bufferConversion; \
    operationMessage += L"."; \
    tempVersion = (Version & 0x000000ff); \
    ACE_OS::itoa(tempVersion, bufferConversion, 10); \
    operationMessage += bufferConversion; \
    operationMessage += L":"; \
    ACE_OS::itoa(Arguments, bufferConversion, 10); \
    operationMessage += bufferConversion;

#define MG_LOG_OPERATION_MESSAGE_PARAMETERS_START() \
    operationMessage += L"(";

#define MG_LOG_OPERATION_MESSAGE_PARAMETERS_END() \
    operationMessage += L") ";

#define MG_LOG_OPERATION_MESSAGE_ADD_STRING(Text) \
    operationMessage += Text;

#define MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR() \
    operationMessage += L",";

#define MG_LOG_OPERATION_MESSAGE_ADD_INT32(Integer) \
    ACE_OS::itoa(Integer, bufferConversion, 10); \
    operationMessage += bufferConversion;

#define MG_LOG_OPERATION_MESSAGE_ADD_DOUBLE(Double) \
    ACE_OS::sprintf(bufferConversion, L"%g", Double); \
    operationMessage += bufferConversion;

#define MG_LOG_OPERATION_MESSAGE_ADD_BOOL(Boolean) \
    if(Boolean == true) \
    { \
        operationMessage += L"true"; \
    } \
    else \
    { \
        operationMessage += L"false"; \
    }

#ifdef _DEBUG
#define MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY() \
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("  (%t) %W\n"), operationMessage.c_str())); \
    MG_LOG_ACCESS_ENTRY(operationMessage.c_str(), connInfoClient.c_str(), connInfoClientIp.c_str(), connInfoUserName.c_str());
#else
#define MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY() \
    MG_LOG_ACCESS_ENTRY(operationMessage.c_str(), connInfoClient.c_str(), connInfoClientIp.c_str(), connInfoUserName.c_str());
#endif

#ifdef _DEBUG
#define MG_LOG_OPERATION_MESSAGE_ADMIN_ENTRY() \
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("  (%t) %W\n"), operationMessage.c_str())); \
    MG_LOG_ADMIN_ENTRY(operationMessage.c_str(), connInfoClient.c_str(), connInfoClientIp.c_str(), connInfoUserName.c_str());
#else
#define MG_LOG_OPERATION_MESSAGE_ADMIN_ENTRY() \
    MG_LOG_ADMIN_ENTRY(operationMessage.c_str(), connInfoClient.c_str(), connInfoClientIp.c_str(), connInfoUserName.c_str());
#endif


#define MG_CONNECTION_INFO \
    STRING connInfoClient = L""; \
    STRING connInfoClientIp = L""; \
    STRING connInfoUserName = L""; 

class MG_DESKTOP_API MgdLogManager : public MgGuardDisposable
{
    DECLARE_CLASSNAME(MgdLogManager)

public:
    virtual ~MgdLogManager();

    virtual void Dispose();

    static MgdLogManager* GetInstance();
    void Initialize();
    void LoadConfigurationProperties();
    STRING GetLogsPath();
    void SetMaximumLogSize(INT32 size);
    void SetLogDelimiter(CREFSTRING delimiter);
    STRING GetLogDelimiter();
    void EnableMaximumLogSize(bool useMaxSize);
    bool IsMaximumLogSizeEnabled();


    // Logging mechanism for errors and warnings
    void LogError(CREFSTRING entry, CREFSTRING client, CREFSTRING clientIp, CREFSTRING userName, CREFSTRING stackTrace = L"");
    void LogWarning(INT16 service, CREFSTRING entry, CREFSTRING client, CREFSTRING clientIp, CREFSTRING userName, CREFSTRING stackTrace = L"");

    // Log file entry methods
    void LogSystemEntry(ACE_Log_Priority priority, CREFSTRING entry);
    void LogAccessEntry(CREFSTRING opId, CREFSTRING client, CREFSTRING clientIp, CREFSTRING userName);
    void LogAdminEntry(CREFSTRING opId, CREFSTRING client, CREFSTRING clientIp, CREFSTRING userName);
    void LogAuthenticationEntry(CREFSTRING entry, CREFSTRING client, CREFSTRING clientIp, CREFSTRING userName);
    void LogErrorEntry(CREFSTRING entry, CREFSTRING client, CREFSTRING clientIp, CREFSTRING userName, CREFSTRING stackTrace, CREFSTRING type);
    void LogTraceEntry(CREFSTRING entry, CREFSTRING client, CREFSTRING clientIp, CREFSTRING userName, CREFSTRING stackTrace = L"", CREFSTRING type = L"");
    void LogSystemErrorEntry(MgException* except);

    // Access log methods
    void SetAccessLogInfo(bool bEnabled, CREFSTRING filename, CREFSTRING parameters);
    bool IsAccessLogEnabled();
    void SetAccessLogEnabled(bool bEnabled);
    STRING GetAccessLogFileName();
    void SetAccessLogFileName(CREFSTRING filename);
    STRING GetAccessLogParameters();
    void SetAccessLogParameters(CREFSTRING parameters);
    bool ClearAccessLog();
    MgByteReader* GetAccessLog();
    MgByteReader* GetAccessLog(INT32 numEntries);
    MgByteReader* GetAccessLog(MgDateTime* fromDate, MgDateTime* toDate);

    // Admin log methods
    void SetAdminLogInfo(bool bEnabled, CREFSTRING filename, CREFSTRING parameters);
    bool IsAdminLogEnabled();
    void SetAdminLogEnabled(bool bEnabled);
    STRING GetAdminLogFileName();
    void SetAdminLogFileName(CREFSTRING filename);
    STRING GetAdminLogParameters();
    void SetAdminLogParameters(CREFSTRING parameters);
    bool ClearAdminLog();
    MgByteReader* GetAdminLog();
    MgByteReader* GetAdminLog(INT32 numEntries);
    MgByteReader* GetAdminLog(MgDateTime* fromDate, MgDateTime* toDate);

    // Authentication log methods
    void SetAuthenticationLogInfo(bool bEnabled, CREFSTRING filename, CREFSTRING parameters);
    bool IsAuthenticationLogEnabled();
    void SetAuthenticationLogEnabled(bool bEnabled);
    STRING GetAuthenticationLogFileName();
    void SetAuthenticationLogFileName(CREFSTRING filename);
    STRING GetAuthenticationLogParameters();
    void SetAuthenticationLogParameters(CREFSTRING parameters);
    bool ClearAuthenticationLog();
    MgByteReader* GetAuthenticationLog();
    MgByteReader* GetAuthenticationLog(INT32 numEntries);
    MgByteReader* GetAuthenticationLog(MgDateTime* fromDate, MgDateTime* toDate);

    // Error log methods
    void SetErrorLogInfo(bool bEnabled, CREFSTRING filename, CREFSTRING parameters);
    bool IsErrorLogEnabled();
    void SetErrorLogEnabled(bool bEnabled);
    STRING GetErrorLogFileName();
    void SetErrorLogFileName(CREFSTRING filename);
    STRING GetErrorLogParameters();
    void SetErrorLogParameters(CREFSTRING parameters);
    bool ClearErrorLog();
    MgByteReader* GetErrorLog();
    MgByteReader* GetErrorLog(INT32 numEntries);
    MgByteReader* GetErrorLog(MgDateTime* fromDate, MgDateTime* toDate);

    // Performance log methods
    void SetPerformanceLogInfo(bool bEnabled, CREFSTRING filename, CREFSTRING parameters);
    bool IsPerformanceLogEnabled();
    void SetPerformanceLogEnabled(bool bEnabled);
    STRING GetPerformanceLogFileName();
    void SetPerformanceLogFileName(CREFSTRING filename);
    STRING GetPerformanceLogParameters();
    void SetPerformanceLogParameters(CREFSTRING parameters);
    bool ClearPerformanceLog();
    MgByteReader* GetPerformanceLog();
    MgByteReader* GetPerformanceLog(INT32 numEntries);
    MgByteReader* GetPerformanceLog(MgDateTime* fromDate, MgDateTime* toDate);

    // Session log methods
    void SetSessionLogInfo(bool bEnabled, CREFSTRING filename, CREFSTRING parameters);
    bool IsSessionLogEnabled();
    void SetSessionLogEnabled(bool bEnabled);
    STRING GetSessionLogFileName();
    void SetSessionLogFileName(CREFSTRING filename);
    STRING GetSessionLogParameters();
    void SetSessionLogParameters(CREFSTRING parameters);
    bool ClearSessionLog();
    MgByteReader* GetSessionLog();
    MgByteReader* GetSessionLog(INT32 numEntries);
    MgByteReader* GetSessionLog(MgDateTime* fromDate, MgDateTime* toDate);

    // Trace log methods
    void SetTraceLogInfo(bool bEnabled, CREFSTRING filename, CREFSTRING parameters);
    bool IsTraceLogEnabled();
    void SetTraceLogEnabled(bool bEnabled);
    STRING GetTraceLogFileName();
    void SetTraceLogFileName(CREFSTRING filename);
    STRING GetTraceLogParameters();
    void SetTraceLogParameters(CREFSTRING parameters);
    INT8 GetDetailLevelForService(INT16 serviceNum);
    bool ClearTraceLog();
    MgByteReader* GetTraceLog();
    MgByteReader* GetTraceLog(INT32 numEntries);
    MgByteReader* GetTraceLog(MgDateTime* fromDate, MgDateTime* toDate);

    MgPropertyCollection* EnumerateLogs();
    void RenameLog(CREFSTRING oldFileName, CREFSTRING newFileName);
    void DeleteLog(CREFSTRING fileName);
    MgByteReader* GetLogFile(CREFSTRING filename);

    static const STRING DefaultAccessLogFileName;
    static const STRING DefaultAdminLogFileName;
    static const STRING DefaultAuthenticationLogFileName;
    static const STRING DefaultErrorLogFileName;
    static const STRING DefaultPerformanceLogFileName;
    static const STRING DefaultSessionLogFileName;
    static const STRING DefaultTraceLogFileName;

    // Write the log message to the file
    void WriteLogMessage(enum MgLogType logType, CREFSTRING message, ACE_Log_Priority logPriority);

    void StopLogThread();

private:
    // Constructor
    MgdLogManager();

    static Ptr<MgdLogManager> m_logManager;
    static STRING m_path;
    static INT32 m_maxLogSize;
    static STRING m_delimiter;
    std::vector<INT8> m_logsDetail;
    static bool m_useMaxLogSize;

    // Strings that represent the parameters that are used in the configuration file
    static const STRING AverageOpTimeParam;
    static const STRING ClientParam;
    static const STRING ClientIpParam;
    static const STRING DurationParam;
    static const STRING EndTimeParam;
    static const STRING ErrorParam;
    static const STRING InfoParam;
    static const STRING OpIdParam;
    static const STRING OpsFailedParam;
    static const STRING OpsProcessedParam;
    static const STRING OpsReceivedParam;
    static const STRING StackTraceParam;
    static const STRING StartTimeParam;
    static const STRING UserParam;

    // Strings that represent the parameters that are used for the performance log
    static const STRING PerformanceAdminOperationsQueueCount;
    static const STRING PerformanceClientOperationsQueueCount;
    static const STRING PerformanceSiteOperationsQueueCount;
    static const STRING PerformanceAverageOperationTime;
    static const STRING PerformanceCpuUtilization;
    static const STRING PerformanceWorkingSet;
    static const STRING PerformanceVirtualMemory;
    static const STRING PerformanceTotalOperationTime;
    static const STRING PerformanceTotalActiveConnections;
    static const STRING PerformanceTotalConnections;
    static const STRING PerformanceTotalProcessedOperations;
    static const STRING PerformanceTotalReceivedOperations;
    static const STRING PerformanceUptime;
    static const STRING PerformanceCacheSize;
    static const STRING PerformanceCacheDroppedEntries;

    // Strings that represent the prefix of the header lines in the log files
    static const STRING HeaderLine1;
    static const STRING HeaderLine2;

    // Log type strings
    static const STRING AccessLog;
    static const STRING AdminLog;
    static const STRING AuthenticationLog;
    static const STRING ErrorLog;
    static const STRING PerformanceLog;
    static const STRING SessionLog;
    static const STRING TraceLog;
    static const STRING UnspecifiedLog;

    // Log file properties
    static const STRING LogNameProperty;
    static const STRING LogTypeProperty;
    static const STRING LogStatusProperty;

    // Log status strings
    static const STRING LogStatusActive;
    static const STRING LogStatusArchive;

    STRING m_applicationName;

    std::ofstream m_accessLogStream;
    std::ofstream m_adminLogStream;
    std::ofstream m_authenticationLogStream;
    std::ofstream m_errorLogStream;
    std::ofstream m_performanceLogStream;
    std::ofstream m_sessionLogStream;
    std::ofstream m_traceLogStream;
    ACE_OSTREAM_TYPE* m_outputStream;

    // ACE wrapper methods to log to the appropriate stream
    void LogToSysLog(ACE_Log_Msg* pAce, char* application);
    void LogToOStream(ACE_Log_Msg* pAce, ACE_OSTREAM_TYPE* output);
    void LogToStderr(ACE_Log_Msg* pAce);

    // Queue the log message
    void QueueLogEntry(enum MgLogType logType, CREFSTRING message, ACE_Log_Priority logPriority);

    // remove the specified log file
    bool RemoveLogFile(CREFSTRING filename);

    // Get the contents of the specified log
    MgByteReader* GetLogHeader(enum MgLogType logType);
    MgByteReader* GetLogHeader(CREFSTRING filename);
    MgByteReader* GetLogContents(CREFSTRING filename);
    MgByteReader* GetLogContents(CREFSTRING filename, INT32 numEntries);
    MgByteReader* GetLogContents(enum MgLogType logType, MgDateTime* fromDate, MgDateTime* toDate);

    // Replace %y %m and %d in the filename with the year month and day
    STRING BuildFileName(CREFSTRING filename);
    STRING BuildFileNameFromDateTime(CREFSTRING filename, MgDateTime* date);
    MgStringCollection* DeterminePotentialFileNames(enum MgLogType logType, MgDateTime* fromDate, MgDateTime* toDate);

    // Searches for the date that's closest to the date specified in the log entries
    INT32 SearchClosestDateAfter(MgStringCollection* lines, MgDateTime* date);
    INT32 SearchClosestDateBefore(MgStringCollection* lines, MgDateTime* date);

    // Extracts the date from a log entry and returns a MgDateTime object
    MgDateTime* GetDateTimeFromEntry(CREFSTRING entry);

    // Compares two MgDateTimes to see if they are more than 24 hours apart
    bool IsMoreThan24HourDiff(MgDateTime* date1, MgDateTime* date2);

    // Helper methods.
    void AddDateTime(REFSTRING entry, const MgDateTime& value);
    void AddDouble(REFSTRING entry, double value);
    void AddInt32(REFSTRING entry, INT32 value);
    void AddString(REFSTRING entry, CREFSTRING value);

    // Add the parameter specified in the configuration file to the log entry
    void AddClient(REFSTRING entry, CREFSTRING client);
    void AddClientIp(REFSTRING entry, CREFSTRING clientIp);
    void AddError(REFSTRING entry, CREFSTRING error, CREFSTRING type);
    void AddInfo(REFSTRING entry, CREFSTRING info);
    void AddOpId(REFSTRING entry, CREFSTRING opId);
    void AddStackTrace(REFSTRING entry, CREFSTRING stackTrace);
    void AddThreadId(REFSTRING entry);
    void AddUserName(REFSTRING entry, CREFSTRING userName);

    // Check if the log file has reached the maximum size
    bool IsMaxSizeExceeded(CREFSTRING logFilename);

    // Create an archive of the specified log
    void ArchiveLog(enum MgLogType logType);

    // Check if existing log files contain valid header information
    void ValidateLogHeaders(enum MgLogType logType);
    bool ValidateAccessLogHeader();
    bool ValidateAdminLogHeader();
    bool ValidateAuthenticationLogHeader();
    bool ValidateErrorLogHeader();
    bool ValidatePerformanceLogHeader();
    bool ValidateSessionLogHeader();
    bool ValidateTraceLogHeader();

    // Helper function to retrive the first header line the logs
    STRING ReadLogTypeFromLogFile(CREFSTRING logFilename);

    // Helper function to retrieve the second header line in the logs
    STRING ReadParametersFromLogFile(enum MgLogType logType);

    // Helper function to identify if a log is an archived or currently active
    STRING DetermineLogFileStatus(CREFSTRING logFilename, CREFSTRING logFileType);

    void AddDelimiter(REFSTRING entry);
    void TranslateDelimiter();

    void ParseLogService(INT16 serviceType, CREFSTRING configString);

    // Helper function to determine and archive should be created based on the specified archive frequency
    bool CheckArchiveFrequency(enum MgLogType logType, CREFSTRING logFilename);

    // Helper function to remove the archive frequency specifier from the filename
    STRING RemoveArchiveFrequencySpecifier(CREFSTRING logFilename);

    STRING ValidateLogFileName(CREFSTRING filename);

    bool IsLogFileInUse(CREFSTRING filename, enum MgLogType& logType);
    bool IsLogInUse(enum MgLogType& logType);
    void EnableLog(enum MgLogType logType);
    void DisableLog(enum MgLogType logType);
    void SetLogHasHeader(enum MgLogType logType, bool bHeader);
    bool LogHasHeader(enum MgLogType logType);

    void UpdateLogFilesTimestampCache();

    // Access Log
    bool m_bAccessLogEnabled;
    bool m_bAccessLogHeader;
    STRING m_AccessLogFileName;
    STRING m_AccessLogParameters;

    // Admin Log
    bool m_bAdminLogEnabled;
    bool m_bAdminLogHeader;
    STRING m_AdminLogFileName;
    STRING m_AdminLogParameters;

    // Authentication Log
    bool m_bAuthenticationLogEnabled;
    bool m_bAuthenticationLogHeader;
    STRING m_AuthenticationLogFileName;
    STRING m_AuthenticationLogParameters;

    // Error Log
    bool m_bErrorLogEnabled;
    bool m_bErrorLogHeader;
    STRING m_ErrorLogFileName;
    STRING m_ErrorLogParameters;

    // Performance Log
    bool m_bPerformanceLogEnabled;
    bool m_bPerformanceLogHeader;
    STRING m_PerformanceLogFileName;
    STRING m_PerformanceLogParameters;

    // Session Log
    bool m_bSessionLogEnabled;
    bool m_bSessionLogHeader;
    STRING m_SessionLogFileName;
    STRING m_SessionLogParameters;

    // Trace Log
    bool m_bTraceLogEnabled;
    bool m_bTraceLogHeader;
    STRING m_TraceLogFileName;
    STRING m_TraceLogParameters;

    ACE_Recursive_Thread_Mutex m_MgdMutex;
    ACE_Thread_Manager m_threadManager;
    MgdLogThread* m_pLogThread;
    INT64 m_writeCount;

    MgDateTime m_cacheAccessLogTimestamp;
    MgDateTime m_cacheAdminLogTimestamp;
    MgDateTime m_cacheAuthenticationLogTimestamp;
    MgDateTime m_cacheErrorLogTimestamp;
    MgDateTime m_cachePerformanceLogTimestamp;
    MgDateTime m_cacheSessionLogTimestamp;
    MgDateTime m_cacheTraceLogTimestamp;
};

#endif
