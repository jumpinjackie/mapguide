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

#ifndef MG_RESOURCE_SERVICE_DEFS_H_
#define MG_RESOURCE_SERVICE_DEFS_H_

#include "AceCommon.h"
#include "BaseService.h"
#include "ResourceService.h"
#include "XmlDefs.h"
#include "XmlUtil.h"

#include <assert.h>
#include <map>
#include <vector>
#include <dbxml/DbXml.hpp>
#include <dbxml/TypeConversions.hpp>
#include <dwfcore/ZipFileDescriptor.h>

using namespace DbXml;

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
        arguments.Add(MgUtil::MultiByteToWideChar(string(e.what())));         \
        mgException = new MgDbXmlException(methodName, __LINE__, __WFILE__, NULL, L"MgFormatInnerExceptionMessage", &arguments); \
    }                                                                         \
    catch (DbException& e)                                                    \
    {                                                                         \
        MgStringCollection arguments;                                         \
        arguments.Add(MgUtil::MultiByteToWideChar(string(e.what())));         \
        mgException = new MgDbException(methodName, __LINE__, __WFILE__, NULL, L"MgFormatInnerExceptionMessage", &arguments); \
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

#endif
