#ifndef DESKTOP_FEATURE_DEFS_H
#define DESKTOP_FEATURE_DEFS_H

#include "MgDesktop.h"

#define CHECK_FEATURE_SOURCE_ARGUMENT(resource, method)                                        \
    CHECKARGUMENTNULL(resource, method);                                                            \
    resource->Validate();                                                                           \
    if (resource->GetResourceType() != MgResourceType::FeatureSource)                               \
        throw new MgInvalidResourceTypeException(method, __LINE__, __WFILE__, NULL, L"", NULL)

#define MG_FEATURE_SERVICE_TRY()                                              \
    MG_TRY()                                                                  \

#define MG_FEATURE_SERVICE_CATCH(methodName)                                  \
    }                                                                         \
    catch (FdoException* e)                                                   \
    {                                                                         \
        STRING messageId;                                                     \
        MgStringCollection arguments;                                         \
        wchar_t* buf = (wchar_t*)e->GetExceptionMessage();                    \
        INT64 nativeErrorCode = e->GetNativeErrorCode();                      \
        FdoPtr<FdoException> cause = e->GetCause();                           \
        FdoPtr<FdoException> root = e->GetRootCause();                        \
                                                                              \
        wchar_t* causeStr = NULL;                                             \
        wchar_t* rootCauseStr = NULL;                                         \
        if (NULL != cause.p)                                                  \
            causeStr = (wchar_t*)cause->GetExceptionMessage();                \
        if (NULL != root.p)                                                   \
            rootCauseStr = (wchar_t*)root->GetExceptionMessage();             \
                                                                              \
        if (NULL != buf)                                                      \
        {                                                                     \
            messageId = L"MgFormatFdoExceptionMessage";                       \
            arguments.Add(buf);                                               \
            arguments.Add((causeStr == NULL) ? L"" : causeStr);               \
            arguments.Add((rootCauseStr == NULL) ? L"" : rootCauseStr);       \
        }                                                                     \
                                                                              \
        FDO_SAFE_RELEASE(e);                                                  \
        mgException = new MgFdoException(methodName, __LINE__, __WFILE__, NULL, messageId, &arguments, nativeErrorCode); \
                                                                              \
    MG_CATCH(methodName)                                                      \

#define MG_FEATURE_SERVICE_CATCH_WITH_FEATURE_SOURCE(methodName, fsId)        \
    }                                                                         \
    catch (FdoException* e)                                                   \
    {                                                                         \
        STRING messageId;                                                     \
        MgStringCollection arguments;                                         \
        wchar_t* buf = (wchar_t*)e->GetExceptionMessage();                    \
        INT64 nativeErrorCode = e->GetNativeErrorCode();                      \
        FdoPtr<FdoException> cause = e->GetCause();                           \
        FdoPtr<FdoException> root = e->GetRootCause();                        \
                                                                              \
        wchar_t* causeStr = NULL;                                             \
        wchar_t* rootCauseStr = NULL;                                         \
        if (NULL != cause.p)                                                  \
            causeStr = (wchar_t*)cause->GetExceptionMessage();                \
        if (NULL != root.p)                                                   \
            rootCauseStr = (wchar_t*)root->GetExceptionMessage();             \
                                                                              \
        if (NULL != buf)                                                      \
        {                                                                     \
            messageId = L"MgFeatureSourceFormatInnerExceptionMessage";        \
            arguments.Add(fsId->ToString());                                  \
            arguments.Add(buf);                                               \
            arguments.Add((causeStr == NULL) ? L"" : causeStr);               \
            arguments.Add((rootCauseStr == NULL) ? L"" : rootCauseStr);       \
        }                                                                     \
                                                                              \
        FDO_SAFE_RELEASE(e);                                                  \
        mgException = new MgFdoException(methodName, __LINE__, __WFILE__, NULL, messageId, &arguments, nativeErrorCode); \
                                                                              \
    MG_CATCH(methodName)                                                      \

#define MG_FEATURE_SERVICE_THROW()                                            \
    MG_THROW()                                                                \

#define MG_FEATURE_SERVICE_CATCH_AND_THROW(methodName)                        \
    MG_FEATURE_SERVICE_CATCH(methodName)                                      \
                                                                              \
    MG_FEATURE_SERVICE_THROW()                                                \

#define MG_FEATURE_SERVICE_CATCH_AND_THROW_WITH_FEATURE_SOURCE(methodName, fsId) \
    MG_FEATURE_SERVICE_CATCH_WITH_FEATURE_SOURCE(methodName, fsId)            \
                                                                              \
    MG_FEATURE_SERVICE_THROW()                                                \

// The catch below is used when we want to mark a connection as no longer valid because
// the underlying RDBMS has been stopped or the connection has been lost.

// FDO doesn't support an exception hierarchy to let us know which are connection
// related exceptions so the error message string must be parsed.
// This is a localization issue and is something FDO should address in a future release.

// The following is a list of the RDBMS errors looked for:
// MySQL
//   1) Lost connection to MySQL server during query
//   2) MySQL server has gone away
//
// Oracle
//   1) ORA-03113
//   2) ORA-03114
//
// SQL Server
//   1) ConnectionRead
//   2) Communication link failure

#define MG_FEATURE_SERVICE_CHECK_CONNECTION_CATCH(resource, methodName)       \
    }                                                                         \
    catch (FdoException* e)                                                   \
    {                                                                         \
        STRING messageId;                                                     \
        MgStringCollection arguments;                                         \
        wchar_t* buf = (wchar_t*)e->GetExceptionMessage();                    \
        INT64 nativeErrorCode = e->GetNativeErrorCode();                      \
        FdoPtr<FdoException> cause = e->GetCause();                           \
        FdoPtr<FdoException> root = e->GetRootCause();                        \
                                                                              \
        wchar_t* causeStr = NULL;                                             \
        wchar_t* rootCauseStr = NULL;                                         \
        if (NULL != cause.p)                                                  \
            causeStr = (wchar_t*)cause->GetExceptionMessage();                \
        if (NULL != root.p)                                                   \
            rootCauseStr = (wchar_t*)root->GetExceptionMessage();             \
                                                                              \
        if (NULL != buf)                                                      \
        {                                                                     \
            if((NULL != wcsstr(buf, L"Lost connection to MySQL server during query")) || \
               (NULL != wcsstr(buf, L"MySQL server has gone away")) ||        \
               (NULL != wcsstr(buf, L"ORA-03113")) ||                         \
               (NULL != wcsstr(buf, L"ORA-03114")) ||                         \
               (NULL != wcsstr(buf, L"ConnectionRead")) ||                    \
               (NULL != wcsstr(buf, L"Communication link failure")))          \
            {                                                                 \
                MgdFdoConnectionPool::PurgeCachedConnections(resource);        \
            }                                                                 \
            messageId = L"MgFeatureSourceFormatInnerExceptionMessage";        \
            arguments.Add(resource->ToString());                              \
            arguments.Add(buf);                                               \
            arguments.Add((causeStr == NULL) ? L"" : causeStr);               \
            arguments.Add((rootCauseStr == NULL) ? L"" : rootCauseStr);       \
        }                                                                     \
                                                                              \
        FDO_SAFE_RELEASE(e);                                                  \
        mgException = new MgFdoException(methodName, __LINE__, __WFILE__, NULL, messageId, &arguments, nativeErrorCode); \
                                                                              \
    MG_CATCH(methodName)                                                      \

#define MG_FEATURE_SERVICE_CHECK_CONNECTION_CATCH_AND_THROW(resource, methodName) \
    MG_FEATURE_SERVICE_CHECK_CONNECTION_CATCH(resource, methodName)           \
                                                                              \
    MG_FEATURE_SERVICE_THROW()                                                \


#endif

#define PRIMARY_ALIAS L"a"
#define SECONDARY_ALIAS L"b"