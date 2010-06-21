//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: XmlPortability.hpp,v 1.28 2006/10/30 17:45:48 bostic Exp $
//

#ifndef __XMLPORTABILITY_HPP
#define	__XMLPORTABILITY_HPP

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
