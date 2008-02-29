//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: NsTypes.hpp,v 1.33 2006/10/30 17:45:57 bostic Exp $
//

/*
 * NsTypes.hpp
 *
 * Core types for node storage
 */

#ifndef __DBXMLNSTYPES_HPP
#define __DBXMLNSTYPES_HPP

#include <dbxml/DbXmlFwd.hpp>
#include <xercesc/framework/MemoryManager.hpp>

// Put all Xerces forward decls in one place (TBD: find better place)
namespace XERCESC {
	class MemoryManager;
};
#define XER_NS XERCESC::

/*
 * Atomic types for node storage.  Size matters.
 */
#include <db_cxx.h>
#define uint8_t u_int8_t
#define uint16_t u_int16_t
#define uint32_t u_int32_t
#define uint64_t u_int64_t
#define int64_t int64_t
typedef unsigned char xmlbyte_t; /* UTF-8 character */
/*
 * xmlch_t is typedef'd to XMLCh to handle
 * the fact that sometimes it'sunsigned short,
 * and sometimes a wchar_t (windows)
 */
typedef XMLCh xmlch_t;  /* UTF-16 character */

#endif
