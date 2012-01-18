#ifndef DESKTOP_MAPPING_DEFS_H
#define DESKTOP_MAPPING_DEFS_H

#include "MgDesktop.h"
#include <assert.h>

///////////////////////////////////////////////////////////////////////////////
/// Mg Server Mapping Service try/catch/throw macros.
///
#define MG_SERVER_MAPPING_SERVICE_TRY()                                       \
    MG_TRY()                                                                  \

// For the Mapping service, gracefully handle and remap any Fdo exceptions to MapGuide exceptions.
// Some Fdo providiers like WMS return useful error strings when a request fails.
// Without the remap, this information is lost and MapGuide returns an unclassified exception.
#define MG_SERVER_MAPPING_SERVICE_CATCH(methodName)                         \
    }                                                                       \
    catch (MgException* e)                                                  \
                                                                            \
    {                                                                       \
        mgException = e;                                                    \
        mgException->AddStackTraceInfo(methodName, __LINE__, __WFILE__);    \
    }                                                                       \
    catch (exception& e)                                                    \
    {                                                                       \
        mgException = MgSystemException::Create(e, methodName, __LINE__, __WFILE__); \
    }                                                                       \
    catch (FdoException* e)                                                 \
    {                                                                       \
        STRING messageId;                                                   \
        MgStringCollection arguments;                                       \
        wchar_t* buf = (wchar_t*)e->GetExceptionMessage();                  \
        INT64 nativeErrorCode = e->GetNativeErrorCode();                    \
        if (NULL != buf)                                                    \
        {                                                                   \
            messageId = L"MgFormatInnerExceptionMessage";                   \
            arguments.Add(buf);                                             \
        }                                                                   \
        FDO_SAFE_RELEASE(e);                                                \
        mgException = new MgFdoException(methodName, __LINE__, __WFILE__, NULL, messageId, &arguments, nativeErrorCode); \
    }                                                                       \
    catch (...)                                                             \
    {                                                                       \
        mgException = new MgUnclassifiedException(methodName, __LINE__, __WFILE__, NULL, L"", NULL); \
    }                                                                       \

#define MG_SERVER_MAPPING_SERVICE_THROW()                                     \
    MG_THROW()                                                                \

#define MG_SERVER_MAPPING_SERVICE_CATCH_AND_THROW(methodName)                 \
    MG_SERVER_MAPPING_SERVICE_CATCH(methodName)                               \
                                                                              \
    MG_SERVER_MAPPING_SERVICE_THROW()  

#endif