//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: DbXmlFwd.hpp,v 1.97 2005/12/19 19:52:37 gmf Exp $
//

#ifndef __DBXMLFWD_HPP
#define	__DBXMLFWD_HPP

//BEGIN-DBXML-VERSION
/* DO NOT EDIT: automatically built by dist/s_include. */
#define DBXML_VERSION_MAJOR 2
#define DBXML_VERSION_MINOR 2
#define DBXML_VERSION_PATCH 13
#define DBXML_VERSION_STRING "Sleepycat Software: Berkeley DB XML 2.2.13: (December 19, 2005)"
#define XERCESC xercesc_2_7
//END-DBXML-VERSION

namespace XERCESC {
class DOMDocument;
class DOMNode;
}

#include "XmlPortability.hpp"

namespace DbXml
{

class XmlContainer;
class XmlManager;
class XmlDocument;
class XmlException;
class XmlQueryContext;
class XmlUpdateContext;
class XmlResults;
class XmlValue;
class XmlQueryExpression;
class XmlIndexSpecification;
class XmlResolver;
class XmlModify;
class XmlData;
class XmlTransaction;
class XmlInputStream;
class XmlMetaDataIterator;
class XmlStatistics;

//
// Flags:
//
// NOTE: these flags have specific values to (1) avoid
// conflicts with Berekeley DB flags and (2) be used
// along side them by users.
// When DB flags change, these flags may need to, as well.
// Some of these are reproduced in swig/dbxml.i
//
enum Flags {
	// XmlManager construction flags
	DBXML_ADOPT_DBENV           = 0x00000001, ///< adopt the given DbEnv
	DBXML_ALLOW_EXTERNAL_ACCESS = 0x00000002, ///< turn on access to the file system
	DBXML_ALLOW_AUTO_OPEN       = 0x00000004, ///< turn on auto-open of containers referenced in a query

	// Common DbXml flags.
	DBXML_ALLOW_VALIDATION      = 0x00100000, ///< perform validation on documents if they contain dtd or xsd references
	DBXML_TRANSACTIONAL         = 0x00200000, ///< make the container Transactional
	DBXML_CHKSUM                = 0x00400000, ///< mapped onto db->set_flags(DB_CHKSUM);

	// these next three are used by XmlContainer::lookupIndex, and can
	// safely re-use the preceding enumeration values.
	DBXML_REVERSE_ORDER         = 0x00100000, ///< perform index lookup in reverse order
	DBXML_INDEX_VALUES          = 0x00200000, ///< return index values along with nodes/documents
	DBXML_CACHE_DOCUMENTS       = 0x00400000, ///< two index entries that refer to the same document return the exact same XmlDocument object

	DBXML_ENCRYPT               = 0x00800000, ///< mapped onto db->set_flags(DB_ENCRYPT);
	DBXML_GEN_NAME              = 0x20000000, ///< Used by putDocument
	DBXML_LAZY_DOCS             = 0x40000000, ///< Used by XmlContainer::getDocument() and XmlQueryExpression::execute()
	DBXML_INDEX_NODES           = 0x80000000, ///< Specified at the creation of a container
	DBXML_NO_INDEX_NODES        = 0x10000000  ///< Specified at the creation of a container
};

enum LogLevel
{
	LEVEL_NONE =    0x00000000,  ///< No debug levels are enabled.
	LEVEL_DEBUG =   0x00000001,  ///< Program execution tracing messages.
	LEVEL_INFO =    0x00000002,  ///< Informational, just for interest.
	LEVEL_WARNING = 0x00000004,  ///< For warnings, bad things but recoverable.
	LEVEL_ERROR =   0x00000008,  ///< For errors that can't be recovered from.
	LEVEL_ALL =     0xFFFFFFFF   ///< All debug levels are enabled.
};

enum LogCategory
{
	CATEGORY_NONE =       0x00000000,
	CATEGORY_INDEXER =    0x00000001,
	CATEGORY_QUERY =      0x00000002,
	CATEGORY_OPTIMIZER =  0x00000004,
	CATEGORY_DICTIONARY = 0x00000008,
	CATEGORY_CONTAINER =  0x00000010,
	CATEGORY_NODESTORE =  0x00000020,
	CATEGORY_MANAGER =    0x00000040,
	CATEGORY_ALL =        0xFFFFFFFF
};

DBXML_EXPORT const char *dbxml_version(int *majorp, int *minorp, int *patchp);

DBXML_EXPORT void setLogLevel(LogLevel level, bool enabled);

DBXML_EXPORT void setLogCategory(LogCategory category, bool enabled);
}

#endif
