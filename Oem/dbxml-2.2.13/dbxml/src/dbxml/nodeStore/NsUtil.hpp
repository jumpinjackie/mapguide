//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//      Sleepycat Software.  All rights reserved.
//
// $Id: NsUtil.hpp,v 1.34 2005/12/07 15:58:18 jsnelson Exp $
//

#ifndef __DBXMLNSUTIL_HPP
#define __DBXMLNSUTIL_HPP

/*
 * NsUtil.hpp
 *
 * Utility functions, including those responsible for
 * marshal/unmarshal of node storage nodes.
 */

/*
 * Node format
 *
 * This documents the persistent node
 * format, as determined by the marshaling functions.
 * If there are any discrepancies, the code is the
 * true reference.
 * 
 * NOTES: Namespace prefixes, uri indexes, and
 * type information are all integer indexes into
 * the document's namespace information maps, mapping
 * prefixes to URIs.  All string values are encoded
 * in UTF-8, and length fields are in bytes.
 * Integers are encoded in a variable-length format (see
 * comments in NsUtil.cpp on the format)
 *
 * All node storage documents are kept in a single DB btree
 * database.  The database allows duplicates, and keeps them
 * sorted.  Each individual document is a set of duplicate
 * keys, where the key is the integer document id.  The
 * duplicates are sorted by their values.  The first several
 * bytes of each node are used for sorting.  They are:
 *   protocol version byte, node id string (null-terminated).
 * There is a custom comparison function registered that knows
 * how to compare node ids.  The protocol version is ignored
 * for sorting purposes, as it's the same for each node.
 *
 * Each document has 1-2 metadata nodes (ids '1' and '2') that
 * keep document metadata and (optional) namespace/type information.
 * The first "regular" node of a document (id 'A') is the
 * "document" node, but looks like any regular node.
 *
 * This is the format of nodes:
 *
 * <header information>
 *    Protocol version -- one byte (NS_PROTOCOL_VERSION)
 *    Node id -- variable length, null-terminated id
 *    Unmarshal Size -- integer size indicating how much space
 *         is needed to unmarshal this node
 *    Flags -- integer flags (NS_*, from NsTypes.hpp)
 *    Level -- integer tree level of node (doc node is 0)
 *    Parent id -- node id of parent (except for document node)
 *    Node name (except for document node)
 *      optional integer prefix
 *      optional integer type
 *      text name
 *      optional integer URI index
 * <end header information>
 * <optional information>
 *    If has child nodes (elements),
 *      Integer number of child nodes
 *      For each child:
 *        If there are text children, an integer for each of the element
 *          children, representing the child's text index.
 *        A node id for the child
 *    If has attributes:
 *      Integer number of attributes
 *      For each attribute:
 *        optional integer prefix
 *        optional integer type
 *        name/value byte array (name0value0)
 *
 *    If has text nodes:
 *      Integer number of text nodes
 *      For each text node:
 *        integer type of text node (enum NS_TEXTTYPES)
 *        text bytes (PIs may have embedded nulls)
 *
 * This is the format of node id '1' -- document metadata:
 *  Protocol version byte (NS_PROTOCOL_VERSION)
 *  ID string -- 2 bytes: '1', 0
 *  Flags -- integer
 *  XML decl -- integer indicating xml 1.0 or 1.1
 *  Optional encoding string (if specified)
 *
 * The optional namespace/type info, id '2', is of the format:
 *  Protocol version byte (NS_PROTOCOL_VERSION)
 *  ID string -- 2 bytes: '2', 0
 *  Num Uris -- integer number of URIs
 *  For each URI, null-terminated uri string
 *  Num prefixes -- integer number of prefixes in map
 *  For each prefix:
 *    integer index into uri array
 *    null-terminated prefix string
 */

#ifdef DEBUG
#define NS_DEBUG 1
#endif

#include "NsTypes.hpp"
#include "../DbXmlInternal.hpp"
#include "../Globals.hpp"
#include "../db_utils.h"
#include <string>
#include "dbxml/XmlException.hpp"
#include <xercesc/framework/MemoryManager.hpp>

/*
 * Utility routines used by Node Storage, including
 *   o marshal/unmarshal of nodes
 *   o utf-8 and utf-16 string manipulation and transcoding
 * These are grouped as static functions in a single class,
 * NsUtil, for namespace and convenience.
 */
// marshaling "protocol" version
#define NS_PROTOCOL_VERSION 1

class DbException;

typedef XMLCh xmlch_t;  /* UTF-16 character */

namespace DbXml
{

enum checkType {
	ignore = 0,
	isCharacters = 1,
	isAttrVal = 2
};

class NsDocument;
class NsDomNode;

class NsUtil {
public:
	// transcoding
	static int nsToUTF8(XER_NS MemoryManager *mmgr, xmlbyte_t **dest,
			    const xmlch_t *src, uint32_t nchars,
			    uint32_t maxbytes,
			    bool *hasEntity = 0, enum checkType type = ignore);
	static int nsFromUTF8(XER_NS MemoryManager *mmgr, xmlch_t **dest,
			      const xmlbyte_t *src,
			      uint32_t nbytes, uint32_t maxchars);
	static int nsCharSizeUTF8(xmlbyte_t firstByte);
	
	// Parse-time utilities.  TBD: group these into a single class?

	static int nsCompareIds(const nid_t *id1, const nid_t *id2);
	/** next is allowed to be null, in which case this method
	    calculates the next id after prev */
	static void nsBetweenId(XER_NS MemoryManager *mmgr, nid_t *id,
				const nid_t *prev, const nid_t *next);
	static void nsFreeId(XER_NS MemoryManager *mmgr, nid_t *id);
	static void nsCopyId(XER_NS MemoryManager *mmgr, nid_t *to, const nid_t *from);

	static void nsCopyText(XER_NS MemoryManager *mmgr, nsText_t *to, const nsText_t *from,
			       bool isUTF8);
	static void nsCopyName(XER_NS MemoryManager *mmgr, nsName_t *to, const nsName_t *from,
			       bool isUTF8);

	static nsNode_t *nsAllocNode(XER_NS MemoryManager *mmgr, uint32_t attrCount,
				     uint32_t flags);
	static nsNode_t *nsMakeTransient(XER_NS MemoryManager *mmgr, nsNode_t *node);
	static nsNode_t *nsCopyNode(XER_NS MemoryManager *mmgr, const nsNode_t *node);
	static void nsFreeNode(XER_NS MemoryManager *mmgr, nsNode_t *node);

	static nsAttrList_t *nsAllocAttrList(XER_NS MemoryManager *mmgr, uint32_t attrCount);
	static nsAttrList_t *nsCopyAttrList(XER_NS MemoryManager *mmgr, const nsAttrList_t *attrs, bool isUTF8);
	static void nsFreeAttrList(XER_NS MemoryManager *mmgr, nsAttrList_t *attrs);

	static nsTextList_t *nsCopyTextList(XER_NS MemoryManager *mmgr, const nsTextList_t *text, bool isUTF8);
	static void nsFreeTextList(XER_NS MemoryManager *mmgr, nsTextList_t *text);

	static nsChildList_t *nsAllocChildList(XER_NS MemoryManager *mmgr, uint32_t nchild);
	static int nsAddChild(XER_NS MemoryManager *mmgr, nsNode_t *node, uint32_t textIndex);
	static void nsInsertChild(XER_NS MemoryManager *mmgr, nsNode_t *node, uint32_t index,
				  uint32_t textIndex);
	static void nsRemoveChild(XER_NS MemoryManager *mmgr, nsNode_t *node, uint32_t index);
	static nsChildList_t *nsCopyChildList(XER_NS MemoryManager *mmgr, const nsChildList_t *clist);
	static void nsFreeChildList(XER_NS MemoryManager *mmgr, nsChildList_t *clist);

	static int nsCreateText(XER_NS MemoryManager *mmgr, nsText_t *dest,
				const void *text, uint32_t length, bool isUTF8,
				bool donate, bool *hasEntity = 0,
				enum checkType type = ignore);
	static int nsAddText(XER_NS MemoryManager *mmgr, nsNode_t *node,
			     const xmlch_t *text, uint32_t type);
	static int nsAddText(XER_NS MemoryManager *mmgr, nsNode_t *node,
			     const void *text, uint32_t len,
			     uint32_t type, bool isUTF8, bool donate);
	static void nsInsertText(XER_NS MemoryManager *mmgr, nsNode_t *node, uint32_t index,
				 const xmlch_t *text, uint32_t type, int elemIndex);
	static int nsCreatePI(XER_NS MemoryManager *mmgr, nsText_t *dest,
			      const void *target, const void *data, bool isUTF8,
			      bool donate, uint32_t len);
	static int nsAddPI(XER_NS MemoryManager *mmgr, nsNode_t *node,
			   const xmlch_t *target, const xmlch_t *data);
	static int nsAddPI(XER_NS MemoryManager *mmgr, nsNode_t *node,
			   const void *target, const void *data,
			   bool isUTF8, bool donate, uint32_t len);
	static void nsInsertPI(XER_NS MemoryManager *mmgr, nsNode_t *node, uint32_t index,
			       const xmlch_t *target, const xmlch_t *data, int elemIndex);
	static int nsCoalesceText(XER_NS MemoryManager *mmgr,
				  nsTextList_t *list,
				  uint32_t index, const void *text,
				  uint32_t len, bool isUTF8, bool checkEnt);
	static void nsRemoveText(XER_NS MemoryManager *mmgr, nsNode_t *node, uint32_t index);

	static nsAttr_t *nsSetAttr(NsDocument *doc, nsNode_t *node, uint32_t index,
				   const xmlch_t *prefix, const xmlch_t *uri,
				   const xmlch_t *localname, const xmlch_t *value,
				   int32_t parseruri, bool specified);
	static nsAttr_t *nsAddAttr(NsDocument *doc, nsNode_t *node, const xmlch_t *prefix,
				   const xmlch_t *uri, const xmlch_t *localname,
				   const xmlch_t *value, int32_t parseruri, bool specified);
	static void nsRemoveAttr(XER_NS MemoryManager *mmgr, nsNode_t *node, uint32_t index);

	// string methods
	static bool nsStringEqual(const xmlch_t *str1, const xmlch_t *str2);
	static bool nsStringEqual(const xmlbyte_t *str1,
				  const xmlbyte_t *str2);
	static int nsStringLen(const xmlch_t *str);
	static int nsStringLen(const unsigned char *str);
	static xmlbyte_t *nsStringDup(XER_NS MemoryManager *mmgr,
				      const xmlbyte_t *str, int *lenP);
	static xmlch_t *nsStringDup(XER_NS MemoryManager *mmgr,
				    const xmlch_t *str,
				    int *lenP);
	// Escape < & " (latter only if isAttr)
	static uint32_t nsEscape(char *dest, const xmlbyte_t *src, int len,
				 bool isAttr = false);
	
	// Marshal/unmarshal methods
	static int nsMarshalNode(const nsNode_t *node,
				 unsigned char *buf, bool count);
	static nsNode_t *nsUnmarshalNode(XER_NS MemoryManager *mmgr,
		unsigned char *buf, uint32_t bufsize, bool adoptBuffer);
	static int nsMarshId(unsigned char *ptr, const nid_t *id);
	static uint32_t nsUnmarshId(const xmlbyte_t *ptr, nid_t *id,
				    xmlbyte_t **endPP, bool copyStrings);
	static int nsMarshInt(xmlbyte_t *buf, uint32_t i);
	static int nsUnmarshInt(const xmlbyte_t *buf, uint32_t *i);
	static int nsUnmarshInt(const xmlbyte_t *buf, int32_t *i);
	static int nsCountInt(uint32_t i);
	static int nsCountMarshalledInt(const xmlbyte_t *buf);
	static const xmlbyte_t *nsSkipId(const xmlbyte_t *ptr);
	static int nsCountId(const nid_t *id);

	static const xmlbyte_t *nsNode(const nid_t *id);
	static void displayNID(std::ostringstream &out, const char *buf, int len);
	static void displayNID(std::ostringstream &out, const nid_t *nid);

	// error handling
	// TBD: change code to XmlException::ExceptionCode
	static void nsThrowException(XmlException::ExceptionCode,
				     const char *desc,
				     const char *file =0, int line = 0);
	static void nsThrowDbException(const DbException &e,
				       const char *file =0,
				       int line = 0);
	static void nsThrowParseException(const char *message);

	// Debug/maybe
#ifdef NS_DEBUG
	static void nsPrintId(const char *label, const nid_t *id);
	static void nsPrintNode16(nsNode_t *node, NsDocument *doc);
	static void nsPrintNode8(nsNode_t *node, NsDocument *doc);
	static void nsTestBetweenId(XER_NS MemoryManager *mmgr);
	static void nsTestMarshal(XER_NS MemoryManager *mmgr, nsNode_t *node);
	static void nsTestCopy(XER_NS MemoryManager *mmgr, nsNode_t *node);
	static void dumpNsDomNode(NsDomNode *node, const char *indent = "");
#endif
};

/**
 * NsDonator
 *
 * An internal class to transcode UTF-16 strings to UTF-8, for donation
 * to the node storage classes.
 *
 * For multi-strings (str10str20), getLen() returns the total length,
 * including both nulls.  For normal strings, it's the same as nsStringLen(),
 * which does NOT include the single, trailing NULL.
 *
 * len passed in does not include trailing NULL.  This must be added
 * for nsToUTF8.
 *
 * IMPORTANT: the same MemoryManager instance must be used for this
 * class as is used by the related classes (NsDocument, etc), as it
 * will be used for deallocation of the donated memory.
 */
class NsDonator {
public:
	NsDonator(XER_NS MemoryManager *mmgr, const xmlch_t *src, uint32_t len,
		  enum checkType type = ignore);
	// multi-string constructor (creates src10src20)
	NsDonator(XER_NS MemoryManager *mmgr, const xmlch_t *src1,
		  const xmlch_t *src2, enum checkType type = ignore);
	xmlbyte_t *getStr() const { return _str; }
	xmlbyte_t *getStr2() const { return _str2; }
	uint32_t getLen() const { return _len; }
	bool getHasEntity() const { return _hasEntity; }
private:
	xmlbyte_t *_str;
	xmlbyte_t *_str2;
	uint32_t _len;
	bool _hasEntity;
};

// inlines
inline int
NsUtil::nsStringLen(const xmlch_t *str)
{
	int ret = 0;
	while (*str++ != 0)
		ret++;
	return ret;
}

inline int
NsUtil::nsStringLen(const unsigned char *str)
{
	return strlen((const char *)str);
}

inline bool
NsUtil::nsStringEqual(const xmlbyte_t *str1, const xmlbyte_t *str2)
{
	if (str1 == 0 || str2 == 0)
		return (str1 == str2);

	while (*str1 == *str2) {
		if (*str1 == 0)
			return true;
		++str1;
		++str2;
	}
	return false;
// 	return (::strcmp((const char *)str1, (const char *)str2) == 0);
}

inline int NsUtil::nsUnmarshInt(const xmlbyte_t *buf, int32_t *i)
{
	return nsUnmarshInt(buf, (uint32_t*)i);
}

inline const xmlbyte_t *NsUtil::nsSkipId(const xmlbyte_t *ptr)
{
	while (*ptr++);
	return ptr;
}

// return correct id pointer in nid_t
inline const xmlbyte_t *NsUtil::nsNode(const nid_t *id)
{
#ifdef ENABLE_ASSERTS
	if (id->idLen == 0)
		return (const xmlbyte_t *) "NONE";
#endif
	if (id->idLen > NID_BYTES_SIZE)
		return id->idStore.idPtr;
	return id->idStore.idBytes;
}

inline int NsUtil::nsCountId(const nid_t *id)
{
	return id->idLen;
}

inline int NsUtil::nsMarshId(unsigned char *ptr, const nid_t *id)
{
	memcpy(ptr, NsUtil::nsNode(id), id->idLen);
	return id->idLen;
}

/*
 * Simple, variable-length encoding for integers.
 * The algorithm extends beyond 4-byte numbers, but
 * that's all that's necessary.  This is based on
 * an algorithm from:
 *   http://www.dlugosz.com/ZIP2/VLI.html
 * Used with permission.
 *
 * The high-order bits of the first byte indicate
 * number of bytes used for encoding:
 *   [0 xxxxxxx] 0-127
 *   [10 xxxxxx] [yyyyyyyy] 0-16383 [msb][lsb]
 *   [110 xxxxx] [yyyyyyyy] [zzzzzzzz] 0-2097151 [msb]...[lsb]
 *   [111 00 xxx] [3 bytes] 0-134217727 [msb]...[lsb]
 *   [111 01 xxx] [4 bytes] up to 35-bits (more than 32...)
 *
 * This results in using 1 byte for small numbers, which
 * is the common case in DB XML (nattrs, ntext, nchild, other
 * small sizes).
 */

#define NSINT_1BIT_MASK 0x80  /* one bit */
#define NSINT_2BIT_MASK 0xc0  /* two bits */
#define NSINT_3BIT_MASK 0xe0  /* three bits */
#define NSINT_5BIT_MASK 0xf8  /* five bits */

#define NSINT_2BYTE_VAL 0x80  /* 10 */
#define NSINT_3BYTE_VAL 0xc0  /* 110 */
#define NSINT_4BYTE_VAL 0xe0  /* 111 00 */
#define NSINT_5BYTE_VAL 0xe8  /* 111 01 */

#define NSINT_MAX1 0x7f
#define NSINT_MAX2 0x3fff
#define NSINT_MAX3 0x1fffff
#define NSINT_MAX4 0x7ffffff
/* don't need MAX5, just > MAX4 */

/*
 * NOTE: for everything else in DB XML, on-disk order is
 * little-endian, but this compression algorithm depends
 * on big-endian order, so swap if necessary.
 */
inline int
NsUtil::nsCountInt(uint32_t i)
{
	if (i <= NSINT_MAX1)
		return 1;
	else if (i <= NSINT_MAX2)
		return 2;
	else if (i <= NSINT_MAX3)
		return 3;
	else  if (i <= NSINT_MAX4)
		return 4;
	else
		return 5;
}

/* encode: return nbytes */
inline int
NsUtil::nsMarshInt(xmlbyte_t *buf, uint32_t i)
{
	if (i <= NSINT_MAX1) {
		// no swapping for one byte value
		buf[0] = (unsigned char) i;
		return 1;
	} else {
		uint32_t tmp = i;
		if (!Globals::isBigendian_)
			M_32_SWAP(tmp);
		unsigned char *p = (unsigned char *) & tmp;
		if (i <= NSINT_MAX2) {
			buf[0] = (p[2] | NSINT_2BYTE_VAL);
			buf[1] = p[3];
			return 2;
		} else if (i <= NSINT_MAX3) {
			buf[0] = (p[1] | NSINT_3BYTE_VAL);
			buf[1] = p[2];
			buf[2] = p[3];
			return 3;
		} else  if (i <= NSINT_MAX4) {
			buf[0] = (p[0] | NSINT_4BYTE_VAL);
			buf[1] = p[1];
			buf[2] = p[2];
			buf[3] = p[3];
			return 4;
		} else {
			buf[0] = NSINT_5BYTE_VAL;
			buf[1] = p[0];
			buf[2] = p[1];
			buf[3] = p[2];
			buf[4] = p[3];
			return 5;
		}
	}
}

inline int
NsUtil::nsCountMarshalledInt(const xmlbyte_t *buf)
{
	unsigned char c = buf[0];
	if ((c & NSINT_1BIT_MASK) == 0) {
		return 1;
	} else if ((c & NSINT_2BIT_MASK) == NSINT_1BIT_MASK) {
		return 2;
	} else if ((c & NSINT_3BIT_MASK) == NSINT_2BIT_MASK) {
		return 3;
	} else if ((c & NSINT_5BIT_MASK) == 0xe0) {
		return 4;
	} else {
		return 5;
	}
}

/* decode: return nbytes */
inline int
NsUtil::nsUnmarshInt(const xmlbyte_t *buf, uint32_t *i)
{
	int len;
	uint32_t tmp;
	unsigned char *p = (unsigned char *) &tmp;
	unsigned char c = buf[0];
	if ((c & NSINT_1BIT_MASK) == 0) {
		// no swapping for one byte value
		*i = buf[0];
		return 1;
	} else if ((c & NSINT_2BIT_MASK) == NSINT_1BIT_MASK) {
		p[0] = p[1] = 0;
		p[2] = (c & ~NSINT_2BIT_MASK);
		p[3] = buf[1];
		len = 2;
		goto out;
	} else if ((c & NSINT_3BIT_MASK) == NSINT_2BIT_MASK) {
		p[0] = 0;
		p[1] = (c & ~NSINT_3BIT_MASK);
		p[2] = buf[1];
		p[3] = buf[2];
		len = 3;
		goto out;
	} else if ((c & NSINT_5BIT_MASK) == 0xe0) {
		p[0] = (c & ~NSINT_5BIT_MASK);
		p[1] = buf[1];
		p[2] = buf[2];
		p[3] = buf[3];
		len = 4;
		goto out;
	} else {
		p[0] = buf[1];
		p[1] = buf[2];
		p[2] = buf[3];
		p[3] = buf[4];
		len = 5;
		goto out;
	}
 out:
	*i = tmp;
	if (!Globals::isBigendian_)
		M_32_SWAP(*i);
	return len;
}

}

#endif
