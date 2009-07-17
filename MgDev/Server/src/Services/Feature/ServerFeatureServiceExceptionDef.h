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

#ifndef MG_FEATURE_SERVICE_EXCEPTION_DEF_H
#define MG_FEATURE_SERVICE_EXCEPTION_DEF_H

#include "MapGuideCommon.h"
#include "Fdo.h"
#include "FdoConnectionManager.h"


#define MG_FEATURE_SERVICE_TRY()                                              \
    MG_TRY()                                                                  \

#define MG_FEATURE_SERVICE_CATCH(methodName)                                  \
    }                                                                         \
    catch (FdoException* e)                                                   \
    {                                                                         \
        STRING messageId;                                                     \
        MgStringCollection arguments;                                         \
        wchar_t* buf = (wchar_t*)e->GetExceptionMessage();                    \
                                                                              \
        if (NULL != buf)                                                      \
        {                                                                     \
            messageId = L"MgFormatInnerExceptionMessage";                     \
            arguments.Add(buf);                                               \
        }                                                                     \
                                                                              \
        FDO_SAFE_RELEASE(e);                                                  \
        mgException = new MgFdoException(methodName, __LINE__, __WFILE__, NULL, messageId, &arguments); \
                                                                              \
    MG_CATCH(methodName)                                                      \

#define MG_FEATURE_SERVICE_THROW()                                            \
    MG_THROW()                                                                \

#define MG_FEATURE_SERVICE_CATCH_AND_THROW(methodName)                        \
    MG_FEATURE_SERVICE_CATCH(methodName)                                      \
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
                MgFdoConnectionManager* pFdoConnectionManager = MgFdoConnectionManager::GetInstance(); \
                pFdoConnectionManager->SetCachedFdoConnectionAsInvalid(resource); \
            }                                                                 \
                                                                              \
            messageId = L"MgFormatInnerExceptionMessage";                     \
            arguments.Add(buf);                                               \
        }                                                                     \
                                                                              \
        FDO_SAFE_RELEASE(e);                                                  \
        mgException = new MgFdoException(methodName, __LINE__, __WFILE__, NULL, messageId, &arguments); \
                                                                              \
    MG_CATCH(methodName)                                                      \

#define MG_FEATURE_SERVICE_CHECK_CONNECTION_CATCH_AND_THROW(resource, methodName) \
    MG_FEATURE_SERVICE_CHECK_CONNECTION_CATCH(resource, methodName)           \
                                                                              \
    MG_FEATURE_SERVICE_THROW()                                                \

#endif
