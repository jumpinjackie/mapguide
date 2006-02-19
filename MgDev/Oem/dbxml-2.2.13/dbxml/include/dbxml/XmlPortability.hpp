//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: XmlPortability.hpp,v 1.25 2005/11/22 00:57:32 gmf Exp $
//

#ifndef __XMLPORTABILITY_HPP
#define	__XMLPORTABILITY_HPP

// Choose third party libraries.
// For now, only Xerces and Pathan are fully supported.

// #undef DBXML_SAX_EXPAT 1
// #undef DBXML_SAX_LIBXML2 1
#define	DBXML_SAX_XERCES 1
// #undef DBXML_SAX_MSXML 1

#define	DBXML_DOM_XERCES2 1
// #undef DBXML_DOM_MSXML 1
// #undef DBXML_SAX_LIBXML2 1

// #undef DBXML_XPATH_XALAN 1
#define	DBXML_XPATH_PATHAN 1
// #undef DBXML_XPATH_MSXML 1
// #undef DBXML_SAX_LIBXML2 1

#if !defined(DBXML_SAX_XERCES)
#error "Invalid configuration: no SAX implementation selected"
#endif
#if !defined(DBXML_DOM_XERCES2)
#error "Invalid configuration: no DOM implementation selected"
#endif
#if !defined(DBXML_XPATH_PATHAN)
#error "Invalid configuration: no XPath implementation selected"
#endif

// Keep compilers quiet about unused parameters
#define	UNUSED(x) static_cast<void>(&(x))

#if defined(_MSC_VER)
// versions check:
// we don't support Visual C++ prior to version 6:
#if (_MSC_VER<1200)
#error "Compiler not supported"
#endif // _MSC_VER<1200

// MSVC requires explicit exportation of symbols accessed in a DLL
// If we are building the library, export symbols
// If we are linking against the library, import symbols

#if defined(DBXML_CREATE_DLL)
// Building a DLL of Berkeley DB XML
#define	DBXML_EXPORT __declspec(dllexport)
#define	DBXML_EXPORT_TEMPLATE template class DBXML_EXPORT
#else
// Making use of a DLL of Berkeley DB XML
#define	DBXML_EXPORT __declspec(dllimport)
#define	DBXML_EXPORT_TEMPLATE extern template class DBXML_EXPORT
#endif // DBXML_CREATE_DLL

#else // !_MSC_VER

// Building on a non-Windows platform.
#define	DBXML_EXPORT
#define	DBXML_EXPORT_TEMPLATE template class

#endif // !_MSC_VER

#endif // __XMLPORTABILITY_HPP
