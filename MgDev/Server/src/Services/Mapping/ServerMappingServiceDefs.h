//
//  Copyright (C) 2004-2008 by Autodesk, Inc.
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

#ifndef MG_MAPPING_SERVICE_DEFS_H
#define MG_MAPPING_SERVICE_DEFS_H

#include "ServiceManager.h"
#include "ServiceOperation.h"
#include "ServerManager.h"
#include "LogManager.h"
#include "IServiceHandler.h"

#include "CSysTransformer.h"

#include "ServerMappingService.h"
#include "MappingOperation.h"

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
        if (NULL != buf)                                                    \
        {                                                                   \
            messageId = L"MgFormatInnerExceptionMessage";                   \
            arguments.Add(buf);                                             \
        }                                                                   \
        FDO_SAFE_RELEASE(e);                                                \
        mgException = new MgFdoException(methodName, __LINE__, __WFILE__, NULL, messageId, &arguments); \
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
    MG_SERVER_MAPPING_SERVICE_THROW()                                         \

#endif
