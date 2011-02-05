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

/// \cond INTERNAL

#ifndef MG_XML_DEFS_H_
#define MG_XML_DEFS_H_

// disable C4244 of XSerializeEngine about __w64 int assigned to unsigned long
#pragma warning(push)
#pragma warning(disable: 4244)

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/dom/DOM.hpp>

#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationLS.hpp>

#include <xercesc/framework/StdOutFormatTarget.hpp>
#include <xercesc/framework/MemBufFormatTarget.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/XMLUni.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/dom/DOMLSSerializer.hpp>
#include <xercesc/dom/DOMConfiguration.hpp>
#include <xercesc/dom/DOMLSOutput.hpp>
#include <xercesc/framework/XMLFormatter.hpp>

#pragma warning(pop)

#if defined(XERCES_NEW_IOSTREAMS)
#include <iostream>
#else
#include <iostream.h>
#endif

//static char*                    gXmlFile               = 0;
//static bool                     gDoNamespaces          = false;
//static bool                     gDoSchema              = false;
//static bool                     gSchemaFullChecking    = false;
//static bool                     gDoCreate              = false;
//
//static char*                    goutputfile            = 0;
//// options for DOMWriter's features
//static XMLCh*                   gOutputEncoding        = 0;
//
static bool                     gSplitCdataSections    = true;
static bool                     gDiscardDefaultContent = true;
// static bool                     gUseFilter             = false;
static bool                     gFormatPrettyPrint     = false;
static bool                     gWriteBOM              = false;
//
//// static XercesDOMParser::ValSchemes    gValScheme       = XercesDOMParser::Val_Auto;

XERCES_CPP_NAMESPACE_USE

///////////////////////////////////////////////////////////////////////////////
/// MapGuide XML try/catch/throw macros.
///
#define MG_XML_TRY()                                                          \
    MG_TRY()                                                                  \

#define MG_XML_CATCH(methodName)                                              \
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

#define MG_XML_THROW()                                                        \
    MG_THROW()                                                                \

#define MG_XML_CATCH_AND_THROW(methodName)                                    \
    MG_XML_CATCH(methodName)                                                  \
                                                                              \
    MG_XML_THROW()                                                            \

#endif

/// \endcond
