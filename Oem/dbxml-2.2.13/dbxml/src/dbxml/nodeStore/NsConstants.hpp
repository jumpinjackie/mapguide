//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//      Sleepycat Software.  All rights reserved.
//
// $Id: NsConstants.hpp,v 1.4 2005/04/05 16:44:05 bostic Exp $
//

#ifndef __DBXMLCONSTANTS_HPP
#define __DBXMLCONSTANTS_HPP

/*
 * External declaration of node storage globals
 */

#include "NsTypes.hpp"

// well-known IDs
#define NS_ID_OVERHEAD 3  // protocol, id byte, null byte
#define NS_ID_ZERO 0x01
#define NS_ID_FIRST 0x02
#define NS_ID_LAST 0xFF
#define NS_METADATA_ID (NS_ID_FIRST + 0) // document metadata
#define NS_NAMESPACE_ID (NS_ID_FIRST + 1) // namespace info
#define NS_ROOT_ID (NS_ID_FIRST + 2) // the document node

namespace DbXml {

extern const char *_xmlnsUri8;
extern const xmlch_t *_xmlnsUri16;
extern const char *_xmlUri8;
extern const xmlch_t *_xmlUri16;
extern const char *_dbxmlUri8;
extern const xmlch_t _dbxmlUri16[];
extern const char *_xmlnsPrefix8;
extern const xmlch_t *_xmlnsPrefix16;
extern const char *_xmlPrefix8;
extern const xmlch_t *_xmlPrefix16;
extern const char *_dbxmlPrefix8;
extern const xmlch_t _dbxmlPrefix16[];
extern const xmlbyte_t *_standYes;
extern const xmlbyte_t *_standNo;
extern const xmlbyte_t _decl8_1_0[];
extern const xmlbyte_t _decl8_1_1[];
extern const xmlch_t _decl_1_0[];
extern const xmlch_t _decl_1_1[];
#define NS_DECL_1_0 0
#define NS_DECL_1_1 1
extern const xmlch_t *_NsDecl[];
extern const xmlbyte_t *_NsDecl8[];
extern const char _docRootId[];
extern const char _docNamespaceId[];
extern const char _docMetadataId[];
}

#endif
