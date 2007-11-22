//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: DbXmlFwd.hpp,v 1.116.2.4 2007/01/30 15:34:40 gmf Exp $
//

#ifndef __DBXMLFWD_HPP
#define	__DBXMLFWD_HPP

//BEGIN-DBXML-VERSION
/* DO NOT EDIT: automatically built by dist/s_include. */
#define DBXML_VERSION_MAJOR 2
#define DBXML_VERSION_MINOR 3
#define DBXML_VERSION_PATCH 10
#define DBXML_VERSION_STRING "Oracle: Berkeley DB XML 2.3.10: (January 30, 2007)"
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
class XmlEventReader;
class XmlEventWriter;	

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
	DBXML_ADOPT_DBENV           = 0x00000001, // take ownership of DbEnv
	DBXML_ALLOW_EXTERNAL_ACCESS = 0x00000002, // allow FS and net access
	DBXML_ALLOW_AUTO_OPEN       = 0x00000004, // auto-open in queries

	// Flags used for container create/open
	DBXML_ALLOW_VALIDATION      = 0x00100000, // validate if specified
	DBXML_TRANSACTIONAL         = 0x00200000, // transactional container
	DBXML_CHKSUM                = 0x00400000, // use DB_CKSUM
	DBXML_ENCRYPT               = 0x00800000, // db->set_flags(DB_ENCRYPT);
	DBXML_INDEX_NODES           = 0x01000000, // use node indexes
	DBXML_NO_INDEX_NODES        = 0x00010000, // also used by lookupIndex
	
	// these next three are only used by XmlContainer::lookupIndex, and can
	// safely re-use the preceding enumeration values.
	DBXML_REVERSE_ORDER         = 0x00100000, // return in reverse sort
	DBXML_INDEX_VALUES          = 0x00200000, // return values also
	DBXML_CACHE_DOCUMENTS       = 0x00400000, // ensure that two index entries that refer to the same document return the exact same XmlDocument object

	DBXML_LAZY_DOCS             = 0x00800000, // lazily materialize docs
	// below used for putDocument, and query operations (reuses enum)
	DBXML_WELL_FORMED_ONLY      = 0x01000000, // well-formed parser only
	// only used in putDocument, safe to reuse enum above
	DBXML_GEN_NAME              = 0x02000000  // generate name in putDoc
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

DBXML_EXPORT const char *dbxml_db_version(int *majorp, int *minorp, int *patchp);

DBXML_EXPORT void setLogLevel(LogLevel level, bool enabled);

DBXML_EXPORT void setLogCategory(LogCategory category, bool enabled);
}

#endif
