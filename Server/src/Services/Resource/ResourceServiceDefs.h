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

#ifndef MG_RESOURCE_SERVICE_DEFS_H_
#define MG_RESOURCE_SERVICE_DEFS_H_

#include "MapGuideCommon.h"
#include "System/XmlDefs.h"
#include "System/XmlUtil.h"

#include <assert.h>
#include <map>
#include <vector>
#include <dbxml/DbXml.hpp>
#include <dwfcore/ZipFileDescriptor.h>

using namespace DbXml;

const int MG_DBXML_CURRENT_VERSION = 6;

///////////////////////////////////////////////////////////////////////////////
/// Mg Resource Service try/catch/throw macros.
///
#define MG_RESOURCE_SERVICE_TRY()                                             \
    MG_TRY()                                                                  \

#define MG_RESOURCE_SERVICE_CATCH(methodName)                                 \
    }                                                                         \
    catch (XmlException& e)                                                   \
    {                                                                         \
        MgStringCollection arguments;                                         \
        STRING message;                                                       \
                                                                              \
        if (DB_LOCK_DEADLOCK == e.getDbErrno())                               \
        {                                                                     \
            message = MgUtil::GetResourceMessage(                             \
                MgResources::ResourceService, L"MgRepositoryBusy");           \
        }                                                                     \
        else                                                                  \
        {                                                                     \
            MgUtil::MultiByteToWideChar(string(e.what()), message);           \
        }                                                                     \
                                                                              \
        arguments.Add(message);                                               \
        mgException = new MgDbXmlException(methodName, __LINE__, __WFILE__, NULL, L"MgFormatInnerExceptionMessage", &arguments); \
        (static_cast<MgThirdPartyException*>(mgException.p))->SetErrorCode(e.getDbErrno()); \
    }                                                                         \
    catch (DbException& e)                                                    \
    {                                                                         \
        MgStringCollection arguments;                                         \
        STRING message;                                                       \
                                                                              \
        if (DB_LOCK_DEADLOCK == e.get_errno())                                \
        {                                                                     \
            message = MgUtil::GetResourceMessage(                             \
                MgResources::ResourceService, L"MgRepositoryBusy");           \
        }                                                                     \
        else                                                                  \
        {                                                                     \
            MgUtil::MultiByteToWideChar(string(e.what()), message);           \
        }                                                                     \
                                                                              \
        arguments.Add(message);                                               \
        mgException = new MgDbException(methodName, __LINE__, __WFILE__, NULL, L"MgFormatInnerExceptionMessage", &arguments); \
        (static_cast<MgThirdPartyException*>(mgException.p))->SetErrorCode(e.get_errno()); \
    }                                                                         \
    catch (DWFException& e)                                                   \
    {                                                                         \
        MgStringCollection arguments;                                         \
        arguments.Add(STRING(e.message()));                                   \
        mgException = new MgDwfException(methodName, __LINE__, __WFILE__, NULL, L"MgFormatInnerExceptionMessage", &arguments); \
    }                                                                         \
    catch (const XMLException& e)                                             \
    {                                                                         \
        MgStringCollection arguments;                                         \
        arguments.Add(X2W(e.getMessage()));                                   \
        mgException = new MgXmlParserException(methodName, __LINE__, __WFILE__, NULL, L"MgFormatInnerExceptionMessage", &arguments); \
    }                                                                         \
    catch (const DOMException& e)                                             \
    {                                                                         \
        MgStringCollection arguments;                                         \
        arguments.Add(X2W(e.msg));                                            \
        mgException = new MgXmlParserException(methodName, __LINE__, __WFILE__, NULL, L"MgFormatInnerExceptionMessage", &arguments); \
                                                                              \
    MG_CATCH(methodName)                                                      \

#define MG_RESOURCE_SERVICE_THROW()                                           \
    MG_THROW()                                                                \

#define MG_RESOURCE_SERVICE_CATCH_AND_THROW(methodName)                       \
    MG_RESOURCE_SERVICE_CATCH(methodName)                                     \
                                                                              \
    MG_RESOURCE_SERVICE_THROW()                                               \

#define MG_RESOURCE_CONTAINER_CATCH_AND_THROW(methodName)                     \
    MG_RESOURCE_SERVICE_CATCH_AND_THROW(FormatMethodName(methodName))


#endif
