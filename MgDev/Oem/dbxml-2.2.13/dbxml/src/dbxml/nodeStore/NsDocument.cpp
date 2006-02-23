//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//      Sleepycat Software.  All rights reserved.
//
// $Id: NsDocument.cpp,v 1.53 2005/12/14 17:57:07 jsnelson Exp $
//

#include "NsUtil.hpp"
#include "NsDocument.hpp"
#include "NsDom.hpp"
#include "NsConstants.hpp"
#include "NsDocumentDatabase.hpp"
#include "../Document.hpp"
#include <vector>
#include <xercesc/util/XMLUTF8Transcoder.hpp>
#include <xercesc/util/XMLUniDefs.hpp>
#include <sstream>

using namespace DbXml;
using namespace std;
XERCES_CPP_NAMESPACE_USE

//
// NsNamespaceInfo:
// number of well-known uris and prefixes in map
//   0 -- xml, 1 -- xmlns, 2 -- dbxml
#define NS_NSMAP_KNOWN 3
#define NS_DECL_1_0 0
#define NS_DECL_1_1 1

//
// internal classes
//
namespace DbXml {
/*
 * NsNamespaceInfo
 *
 * Encapsulates per-document namespace information, populated
 * when a document is in use (parse time, serialized, and DOM):
 *  o list of namespace URIs, including URIs for schema types
 *  o list of prefix mappings, and schema name mappings
 *
 * TBD: put the URIs in the name
 * dictionary, for sharing across documents.
 */
#define NS_DEFAULT_NURI 20
#define NS_DEFAULT_NPREFIX 20

class NsPrefixMapEntry {
public:
	NsPrefixMapEntry() : prefix8(0), prefix16(0),
			     uriIndex(-1), plen(0) {}
	// no destructor, NsNamespaceInfo manages memory
	const xmlbyte_t *prefix8;
	const xmlch_t *prefix16;
	int32_t uriIndex;
	int32_t plen; // number of utf-8 bytes, incl. null.
};

class NsUriMapEntry {
public:
	NsUriMapEntry() : uri8(0), uri16(0), ulen(0) {}
	// no destructor, NsNamespaceInfo manages memory
	const xmlbyte_t *uri8;
	const xmlch_t *uri16;
	int32_t ulen; // number of utf-8 bytes, incl. null.
};

typedef std::vector<NsPrefixMapEntry> NsPrefixMap;
typedef std::vector<NsUriMapEntry> NsUriMap;
typedef std::vector<int32_t> NsUriParserMap;

class NsNamespaceInfo {
public:
	NsNamespaceInfo(MemoryManager *mmgr);
	~NsNamespaceInfo();

	void initialize(void);
	void reset(void);
	//
	// dump/load to/from a document.  Dump will
	// overwrite any existing namespace info.
	// If there is no data beyond the static namespace
	// info, dump will NOT create a DB node, and will
	// return false.  This is not an error.
	//
	bool dump(OperationContext &oc, NsDocument &doc);
	void load(const char *buffer);

	//
	// the add methods return the index to (new) entry.
	// if the entry exists, the index is returned.
	// len, if non-zero does *not* include trailing null.
	//
	int32_t addUri(const void *uri, int32_t parserUri,
		       bool isUTF8, bool isDonated, uint32_t len);
	int32_t addPrefix(const void *prefix, int32_t uriIndex,
			  bool isUTF8, bool isDonated, uint32_t len);

	// utf-8 and utf-16 accessors
	const xmlbyte_t * getUri8(int32_t uriIndex);
	const xmlbyte_t * getPrefix8(int32_t prefIndex);
	const xmlch_t * getUri(int32_t uriIndex);
	const xmlch_t * getPrefix(int32_t prefIndex);

	//  return -1 on failure
	const int getUriForPrefix(int32_t index) const;

	int32_t getNUri() const { return _nURI; }
	int32_t getNPrefix() const { return _nPrefix; }
private:
	// lists
	MemoryManager *_memManager;
	NsPrefixMap _prefixMap;
	NsUriMap _uriMap;
	NsUriParserMap _uriParserMap;
	int32_t _nURI;
	int32_t _nPrefix;
	int32_t _nPUri;

	bool _modified;
};

// inline lookup.  return -1 on failure
inline const int NsNamespaceInfo::getUriForPrefix(int32_t index) const {
	if (index < _nPrefix)
		return _prefixMap[index].uriIndex;
	return -1;
}

// URIs
const char *_xmlnsUri8 = "http://www.w3.org/2000/xmlns/";
const xmlch_t *_xmlnsUri16 = XMLUni::fgXMLNSURIName;
const char *_xmlUri8 = "http://www.w3.org/XML/1998/namespace";
const xmlch_t *_xmlUri16 = XMLUni::fgXMLURIName;
const char *_dbxmlUri8 = "http://www.sleepycat.com/2002/dbxml";
const xmlch_t _dbxmlUri16[] =
{
	chLatin_h, chLatin_t, chLatin_t, chLatin_p, chColon, chForwardSlash,
	chForwardSlash, chLatin_w, chLatin_w, chLatin_w, chPeriod,
	chLatin_s, chLatin_l, chLatin_e, chLatin_e, chLatin_p, chLatin_y,
	chLatin_c, chLatin_a, chLatin_t, chPeriod, chLatin_c, chLatin_o,
	chLatin_m, chForwardSlash, chDigit_2, chDigit_0, chDigit_0, chDigit_2,
	chForwardSlash,	chLatin_d, chLatin_b, chLatin_x, chLatin_m,
	chLatin_l, chNull
};

// prefixes
const char *_xmlnsPrefix8 = "xmlns";
const xmlch_t *_xmlnsPrefix16 = XMLUni::fgXMLNSString;
const char *_xmlPrefix8 = "xml";
const xmlch_t *_xmlPrefix16 = XMLUni::fgXMLString;
const char *_dbxmlPrefix8 = "dbxml";
const xmlch_t _dbxmlPrefix16[] =
{
	chLatin_d, chLatin_b, chLatin_x, chLatin_m, chLatin_l, chNull
};

const xmlbyte_t *_standYes=(const xmlbyte_t*) "yes";
const xmlbyte_t *_standNo=(const xmlbyte_t*) "no";

// local constants for XML declaration (1.0 vs 1.1)
const xmlbyte_t _decl8_1_0[] = {'1', '.', '0', 0};
const xmlbyte_t _decl8_1_1[] = {'1', '.', '1', 0};
const xmlch_t _decl_1_0[] = {'1', '.', '0', 0};
const xmlch_t _decl_1_1[] = {'1', '.', '1', 0};
const xmlch_t *_NsDecl[2] = {_decl_1_0, _decl_1_1};
const xmlbyte_t *_NsDecl8[2] = {_decl8_1_0, _decl8_1_1};
const char _docMetadataId[] =
    { NS_PROTOCOL_VERSION, NS_METADATA_ID, 0 };
const char _docNamespaceId[] =
    { NS_PROTOCOL_VERSION, NS_NAMESPACE_ID, 0 };
const char _docRootId[] =
    { NS_ROOT_ID, 0 };

} // namespace DbXml

/*
 * Node Ids
 *
 * Node Ids are stored as null-terminated strings, with valid
 * characters: A-z (including "[\]^_`" between Z and a, for
 * simplicity).  '0' (character zero) is also valid, but is
 * treated specially for comparison.
 *
 * Comparison/collation order is lexicographical. The document
 * node's id is always "A", where in ids are allocated "B", "C"
 * until "z", when we continue with "zA":
 *
 *   "C" > "A"
 *   "zA" > "z"
 *   "zH" > "z"
 *
 * If an id is required between "A" and "B", then "AA" through
 * "Az" can be used. It is probably preferable to use "AZ", as
 * that occurs roughly halfway through the id space:
 *
 *   "AA" > "A"
 *   "Az" < "B"
 *
 * If an id is required between "A" and "AA", then the special
 * "0" character can be used, to create "A0A" through "A0z", all
 * of which lie between them:
 *
 *   "A0A" > "A"
 *   "A0z" < "AA"
 *
 * However, a "0" character should never occur at the end of an
 * id string:
 *
 *   "ABC"    - valid
 *   "AOA"    - valid
 *   "z000Z"  - valid
 *   "zcA0"   - invalid
 *
 * In this way, the id scheme resembles digits after a
 * decimal point. If instead of the range "A"-"z", we used
 * "1"-"9", keeping "0" the same, then the comparison
 * becomes more obvious:
 *
 *   0.3 > 0.1    ("C" > "A")
 *   0.91 > 0.9   ("zA" > "z")
 *   0.95 > 0.9   ("zH" > "z")
 *
 *   0.11 > 0.1   ("AA" > "A")
 *   0.19 < 0.2   ("Az" < "B")
 *
 *   0.101 > 0.1  ("A0A" > "A")
 *   0.109 < 0.11 ("A0z" > "AA")
 *
 * Note: IDs with one digit in (ie: "A"-"z") are reserved for
 * "special" nodes like the document info node, and the namespace
 * info node.
 *
 */

/*
 * Node Id Comparison function
 *
 * A simple lexicographical comparison, stopping at the first null.
 *
 */
extern "C" {
int
nsCompareNodes(DB *db, const DBT *dbt1, const DBT *dbt2)
{
	// Assume null termination of node ids
	int res;
	const xmlbyte_t *p1 = (const xmlbyte_t *)dbt1->data;
	const xmlbyte_t *p2 = (const xmlbyte_t *)dbt2->data;
	while((res = (int)*p1 - (int)*p2) == 0 &&
	      *p1 != 0) {
		++p1; ++p2;
	}
	return res;
}

};

#ifdef NS_DEBUG

void
NsDocument::_domTraverse(NsDomNode *node)
{
	NsDomNode *current = node;
	while (current) {
		NsUtil::dumpNsDomNode(current);
		if (current->hasNsChildNodes())
			_domTraverse(current->getNsFirstChild());
		current = current->getNsNextSibling();
	}
}

void
NsDocument::_nsDumpNode(NsDomElement *dnode, const char *indent)
{
	NsDomElement *current = dnode;
	_domTraverse(current);
}

#endif // NS_DEBUG

//
// NsNamespaceInfo implementation
//
NsNamespaceInfo::NsNamespaceInfo(MemoryManager *mmgr):
	_memManager(mmgr), _prefixMap(0), _uriMap(0), _uriParserMap(0),
	_nURI(0), _nPrefix(0), _nPUri(0), _modified(true)
{
}

void
NsNamespaceInfo::initialize(void)
{
	_nURI = _nPrefix = _nPUri = NS_NSMAP_KNOWN;
	_prefixMap.resize(NS_DEFAULT_NPREFIX);
	_uriMap.resize(NS_DEFAULT_NURI);
	_uriParserMap.resize(NS_DEFAULT_NURI, -1);

	// well-known uri(s) and prefix(es)

	// xml:
	_uriMap[0].uri16 = (const xmlch_t *) _xmlUri16;
	_uriMap[0].uri8 = (const xmlbyte_t *)_xmlUri8;
	_prefixMap[0].prefix8 = (const xmlbyte_t *)_xmlPrefix8;
	_prefixMap[0].prefix16 = (const xmlch_t *) _xmlPrefix16;
	_prefixMap[0].uriIndex = 0;

	// xmlns:
	_uriMap[1].uri16 = (const xmlch_t *) _xmlnsUri16;
	_uriMap[1].uri8 = (const xmlbyte_t *)_xmlnsUri8;
	_prefixMap[1].prefix8 = (const xmlbyte_t *)_xmlnsPrefix8;
	_prefixMap[1].prefix16 = (const xmlch_t *) _xmlnsPrefix16;
	_prefixMap[1].uriIndex = 1;

	// dbxml:
	_uriMap[2].uri16 = (const xmlch_t *) _dbxmlUri16;
	_uriMap[2].uri8 = (const xmlbyte_t *)_dbxmlUri8;
	_prefixMap[2].prefix8 = (const xmlbyte_t *)_dbxmlPrefix8;
	_prefixMap[2].prefix16 = (const xmlch_t *) _dbxmlPrefix16;
	_prefixMap[2].uriIndex = 2;
}

NsNamespaceInfo::~NsNamespaceInfo()
{
	reset();
}

void
NsNamespaceInfo::reset(void)
{
	int i;
	// start past static string(s)
	for (i = NS_NSMAP_KNOWN; i < _nURI; i++) {
		if (_uriMap[i].uri8)
			_memManager->deallocate((void*)_uriMap[i].uri8);
		if (_uriMap[i].uri16)
			_memManager->deallocate((void*)_uriMap[i].uri16);
		_uriMap[i].uri8 = 0;
		_uriMap[i].uri16 = 0;
	}
	for (i = NS_NSMAP_KNOWN; i < _nPrefix; i++) {
		if (_prefixMap[i].prefix8)
			_memManager->deallocate((void*)_prefixMap[i].prefix8);
		if (_prefixMap[i].prefix16)
			_memManager->deallocate((void*)_prefixMap[i].prefix16);
		_prefixMap[i].prefix8 = 0;
		_prefixMap[i].prefix16 = 0;
	}
	_nURI = NS_NSMAP_KNOWN;
	_nPrefix = NS_NSMAP_KNOWN;
	_nPUri = 0;
	_modified = true;
}

//
// dump/load marshal/unmarshal namespace info to/from
// a DB record.  The format is:
//   nUri -- integer number of uris
//   uri10uri20...
//   nPrefixes -- integer number of prefixes/mappings
//   [uriIndex1,prefix10]... (uriIndex is an integer)
//
//  The order is important -- the order implies index
//  numbers once loaded.  Actual uri and prefix strings
//  start at NS_NSMAP_KNOWN, since indexes less than that
//  are statically constructed (see class ctor).
//
//  The namespace info node ID comes from the document object.
//
bool
NsNamespaceInfo::dump(OperationContext &oc, NsDocument &doc)
{
	//
	// byte counting happens in transcode to utf-8.
	// lengths include trailing null.
	// Start with 3 -- size of version + node id.
	int i, size = 3;
	for (i = NS_NSMAP_KNOWN; i < _nURI; i++) {
		(void) getUri8(i); // force transcode
		DBXML_ASSERT(_uriMap[i].ulen);
		size += _uriMap[i].ulen;
	}
	for (i = NS_NSMAP_KNOWN; i < _nPrefix; i++) {
		(void)getPrefix8(i);
		DBXML_ASSERT(_prefixMap[i].plen);
		size += _prefixMap[i].plen;
		size += NsUtil::nsCountInt(_prefixMap[i].uriIndex);
	}
	// if no non-static entries, nothing to store
	if (size == 3)
		return false;

	if(_modified) {
		size += NsUtil::nsCountInt(_nURI - NS_NSMAP_KNOWN);
		size += NsUtil::nsCountInt(_nPrefix - NS_NSMAP_KNOWN);

		xmlbyte_t *buf = (xmlbyte_t *)malloc(size);
		if (!buf)
			NsUtil::nsThrowException(XmlException::NO_MEMORY_ERROR,
						 "Failed to allocate memory",
						 __FILE__, __LINE__);
		// marshal
		xmlbyte_t *ptr = buf + NS_ID_OVERHEAD; // get past version and id
		ptr += NsUtil::nsMarshInt(ptr, _nURI - NS_NSMAP_KNOWN);
		for (i = NS_NSMAP_KNOWN; i < _nURI; i++) {
			NsUriMapEntry *ent = &(_uriMap[i]);
			memcpy(ptr, ent->uri8, ent->ulen);
			ptr += ent->ulen;
		}
		ptr += NsUtil::nsMarshInt(ptr, _nPrefix - NS_NSMAP_KNOWN);
		for (i = NS_NSMAP_KNOWN; i < _nPrefix; i++) {
			NsPrefixMapEntry *ent = &(_prefixMap[i]);
			ptr += NsUtil::nsMarshInt(ptr, ent->uriIndex);
			memcpy(ptr, ent->prefix8, ent->plen);
			ptr += ent->plen;
		}

		DBXML_ASSERT(ptr <= (buf + size));

		// put buffer in DB
		doc.putNamespaceInfo(oc, (char*)buf, size);

		free(buf);
		_modified = false;
	}
	return true;
}

void
NsNamespaceInfo::load(const char *buf)
{
	reset(); // just in case
	int i;
	// caller got past version and id
	const xmlbyte_t *ptr = (const xmlbyte_t *) buf;
	ptr += NsUtil::nsUnmarshInt(ptr, &_nURI);
	_nURI += NS_NSMAP_KNOWN;
	if ((uint32_t)_nURI > _uriMap.size())
		_uriMap.resize(_nURI);
	for (i = NS_NSMAP_KNOWN; i < _nURI; i++) {
		int len = 0;
		_uriMap[i].uri8 = NsUtil::nsStringDup(_memManager, ptr, &len);
		_uriMap[i].ulen = len;
		ptr += len;
	}
	ptr += NsUtil::nsUnmarshInt(ptr, &_nPrefix);
	_nPrefix += NS_NSMAP_KNOWN;
	if ((uint32_t)_nPrefix > _prefixMap.size())
		_prefixMap.resize(_nPrefix);

	for (i = NS_NSMAP_KNOWN; i < _nPrefix; i++) {
		int len = 0;
		ptr += NsUtil::nsUnmarshInt(ptr, &(_prefixMap[i].uriIndex));
		_prefixMap[i].prefix8 = NsUtil::nsStringDup(_memManager,
							    ptr, &len);
		_prefixMap[i].plen = len;
		ptr += len;
	}
	_modified = false;
}

// this function is complicated by the need to handle both
// utf-8 and utf-16...
// len is assumed to be valid only if isDonated
int32_t
NsNamespaceInfo::addUri(const void *uri, int32_t parserUri,
			bool isUTF8, bool isDonated, uint32_t len)
{
	int32_t i;
	// check parserUri map first
	if (_uriParserMap.size() > (uint32_t)parserUri)
		if (_uriParserMap[parserUri] >= 0) {
			// for now, must free donated string, assuming
			// same memory manager was used.
			// TBD: think about how to avoid allocation of
			// duplicates in donation case
			if (isDonated)
				_memManager->deallocate((void*)uri);
			return _uriParserMap[parserUri];
		}
	if (parserUri == -1) {
		for (i = 0; i < _nURI; i++) {
			if (isUTF8) {
				const xmlbyte_t *turi = getUri8(i);
				if (NsUtil::nsStringEqual(
					    (const xmlbyte_t *)uri,
					    turi)) {
					if (isDonated)
						_memManager->deallocate(
							(void*)uri);
					return i;
				}
			} else {
				const xmlch_t *turi = getUri(i);
				if (NsUtil::nsStringEqual(
					    (const xmlch_t *)uri,
					    turi))
					return i;
			}
		}
	}
	// does it fit in existing
	// add at _nURI, then add to parser uri map
	i = _nURI;
	if ((uint32_t) i >= _uriMap.size())
		_uriMap.resize(i << 1);
	if (isUTF8) {
		xmlbyte_t *uriCopy;
		if (isDonated) {
			uriCopy = (xmlbyte_t *) uri;
			DBXML_ASSERT(len);
			++len; // add null
		} else {
			uriCopy = NsUtil::nsStringDup(_memManager,
						      (xmlbyte_t*)uri, (int*)&len);
		}
		DBXML_ASSERT(uriCopy);
		DBXML_ASSERT(len);
		NsUriMapEntry *ent = &(_uriMap[i]);
		ent->uri8 = uriCopy;
		ent->ulen = len;
	} else {
		xmlch_t *uriCopy = NsUtil::nsStringDup(_memManager,
						       (xmlch_t*)uri, 0);
		DBXML_ASSERT(uriCopy);
		_uriMap[i].uri16 = uriCopy;
	}
	++_nURI;

	// add to parser map
	if (parserUri != -1) {
		// double what's needed
		if ((uint32_t)parserUri >= _uriParserMap.size())
			_uriParserMap.resize(parserUri << 1, -1);
		_uriParserMap[parserUri] = i;
		++_nPUri;
	}

	_modified = true;
	return i;
}

const xmlch_t *
NsNamespaceInfo::getUri(int32_t uriIndex)
{
	DBXML_ASSERT(uriIndex != NS_NOPREFIX);
	const xmlch_t *uri = _uriMap[uriIndex].uri16;
	if (!uri) {
		xmlch_t *newuri = 0;
		const xmlbyte_t *uri8 = _uriMap[uriIndex].uri8;
		if (uri8)
			NsUtil::nsFromUTF8(_memManager, &newuri, uri8,
					   NsUtil::nsStringLen(uri8) + 1, 0);
		_uriMap[uriIndex].uri16 = uri = newuri;
	}
	return  uri;
}

const xmlbyte_t *
NsNamespaceInfo::getUri8(int32_t uriIndex)
{
	DBXML_ASSERT(uriIndex != NS_NOPREFIX);
	const xmlbyte_t *uri8 = _uriMap[uriIndex].uri8;
	if (!uri8) {
		xmlbyte_t *newuri = 0;
		const xmlch_t *uri16 = _uriMap[uriIndex].uri16;
		if (uri16)
			_uriMap[uriIndex].ulen =
				NsUtil::nsToUTF8(_memManager, &newuri, uri16,
						 NsUtil::nsStringLen(uri16) +1,
						 0);
		_uriMap[uriIndex].uri8 = uri8 = newuri;
	}
	return  uri8;
}

const xmlch_t *
NsNamespaceInfo::getPrefix(int32_t prefIndex)
{
	DBXML_ASSERT(prefIndex != NS_NOPREFIX);
	const xmlch_t *pref = _prefixMap[prefIndex].prefix16;
	if (!pref) {
		xmlch_t *newpref = 0;
		const xmlbyte_t *pref8 = _prefixMap[prefIndex].prefix8;
		if (pref8)
			NsUtil::nsFromUTF8(_memManager, &newpref, pref8,
					   NsUtil::nsStringLen(pref8) + 1, 0);
		_prefixMap[prefIndex].prefix16 = pref = newpref;
	}
	return  pref;
}

const xmlbyte_t *
NsNamespaceInfo::getPrefix8(int32_t prefIndex)
{
	DBXML_ASSERT(prefIndex != NS_NOPREFIX);
	const xmlbyte_t *pref8 = _prefixMap[prefIndex].prefix8;
	if (!pref8) {
		xmlbyte_t *newpref = 0;
		const xmlch_t *pref16 = _prefixMap[prefIndex].prefix16;
		if (pref16)
			_prefixMap[prefIndex].plen =
				NsUtil::nsToUTF8(_memManager, &newpref, pref16,
						 NsUtil::nsStringLen(pref16)+1,
						 0);
		_prefixMap[prefIndex].prefix8 = pref8 = newpref;
	}
	return  pref8;
}

// len is assumed to be valid only if isDonated
int32_t
NsNamespaceInfo::addPrefix(const void *prefix, int32_t uriIndex,
			   bool isUTF8, bool isDonated, uint32_t len)
{
	int32_t i;
	if (!prefix)
		return -1;
	for (i = 0; i < _nPrefix; i++) {
		// must match *both* prefix string and uri index
		// to support changing mappings
		if (uriIndex == _prefixMap[i].uriIndex) {
			if (isUTF8) {
				const xmlbyte_t *pref = getPrefix8(i);
				if (NsUtil::nsStringEqual(
					    (const xmlbyte_t *)prefix, pref)) {
					if (isDonated)
						_memManager->deallocate(
							(void*)prefix);
					return i;
				}
			} else {
				const xmlch_t *pref = getPrefix(i);
				if (NsUtil::nsStringEqual(
					    (const xmlch_t *)prefix, pref))
					return i;
			}
		}
	}
	// does it fit in existing
	if ((uint32_t) i >= _prefixMap.size())
		_prefixMap.resize(i << 1); // double what's required
	if (isUTF8) {
		const xmlbyte_t *prefixCopy;
		if (isDonated) {
			prefixCopy = (const xmlbyte_t *)prefix;
			DBXML_ASSERT(len);
			++len; // add null
		} else {
			prefixCopy = NsUtil::nsStringDup(_memManager,
							 (xmlbyte_t *)prefix,
							 (int*)&len);
			DBXML_ASSERT(prefixCopy);
		}
		DBXML_ASSERT(len);
		NsPrefixMapEntry *ent = &(_prefixMap[i]);
		ent->prefix8 = prefixCopy;
		ent->plen = len;
	} else {
		const xmlch_t *prefixCopy;
		prefixCopy = NsUtil::nsStringDup(_memManager,
						 (xmlch_t *)prefix,
						 0);
		DBXML_ASSERT(prefixCopy);
		_prefixMap[i].prefix16 = prefixCopy;
	}
	_prefixMap[i].uriIndex = uriIndex;
	++_nPrefix;

	_modified = true;
	return i;
}

//
// NsDocument implementation
//

// Basic ctor
NsDocument::NsDocument(const Document *ownerDoc, MemoryManager *memManager) :
	_ownerDoc(ownerDoc), _memManager(memManager), _docId(0),
	_doNamespace(true), _hasNsNode(false), _hasContent(true), _nsInfo(0),
	_xmlDecl(-1), _sniffedEncodingStr8(0), _encodingStr8(0), _encodingStr(0), _standaloneStr(0),
	_xmlDeclModified(true),
	_domFactory(0), _domDocNode(0),
	_domCreateEntityText(false),
	_db(0)
{
}

// init to create a new node storage doc
void
NsDocument::initDoc(Transaction *txn, NsDocumentDatabase *db, const ID &docId, u_int32_t flags, bool newDoc)
{
	if (_doNamespace)
		setDoNamespaces(true);

	_oc.set(txn);
	_db = db;
	_docId = docId;
	_flags = flags;

	if(!newDoc) {
		// load existing metadata
		bool hasNsNode = getDocInfo();
		if (!_hasContent)
			return; // no-content doc
		DBXML_ASSERT(_nsInfo);
		if (hasNsNode) {
			DBXML_ASSERT(_doNamespace);
			getNamespaceInfo();
		}
	}
}

// init for a pure DOM parse
void
NsDocument::initDOM(NsDomFactory *factory)
{
	if (_doNamespace)
		setDoNamespaces(true);
	_domFactory = factory;
}

// init for an index parse -- whole document
void
NsDocument::initIndex()
{
	if (_doNamespace)
		setDoNamespaces(true);
}

//
// TBD: for some parses (e.g. DOM), may not
//  want, or need to deallocate -- depends on memory manager
//
NsDocument::~NsDocument()
{
	if (_nsInfo)
		delete _nsInfo;
	if (_sniffedEncodingStr8)
		_memManager->deallocate((void*)_sniffedEncodingStr8);
	if (_encodingStr8)
		_memManager->deallocate((void*)_encodingStr8);
	if (_encodingStr)
		_memManager->deallocate((void*)_encodingStr);
	if (_domFactory) {
		_domFactory->destroyDomList();
	}
	clearModifications();
}

// turning off namespaces just changes the boolean,
// leaving any initialized state in place.  It should
// be ignored, and harmless.
void
NsDocument::setDoNamespaces(bool doNamespace)
{
	// only init for a positive state change
	if (!_nsInfo && doNamespace) {
		_nsInfo = new NsNamespaceInfo(_memManager);
		if (!_nsInfo)
			NsUtil::nsThrowException(XmlException::NO_MEMORY_ERROR,
				 "new failed to allocate memory",
				 __FILE__, __LINE__);
		_nsInfo->initialize();
	}
	_doNamespace = doNamespace;
}

int
NsDocument::addUri(const void *uri, int32_t uriIndex,
		   bool isUTF8, bool isDonated, uint32_t len)
{
	DBXML_ASSERT(_nsInfo);
	return _nsInfo->addUri(uri, uriIndex, isUTF8, isDonated, len);
}

int
NsDocument::addPrefix(const void *prefix, int32_t uriIndex,
		      bool isUTF8, bool isDonated, uint32_t len)
{
	DBXML_ASSERT(_nsInfo);
	return _nsInfo->addPrefix(prefix, uriIndex, isUTF8, isDonated, len);
}

int32_t NsDocument::getUriIndexForPrefix(int32_t prefixIndex) const
{
	DBXML_ASSERT(_nsInfo);
	return _nsInfo->getUriForPrefix(prefixIndex);
}

// marshal and put document metadata
void
NsDocument::putDocInfo(OperationContext &oc, bool hasNsNode)
{
	if(_xmlDeclModified) {
		// buffer size count
		int size = NS_ID_OVERHEAD + 1; // +1 == flags byte
		uint32_t flags = 0;
		int encLen =0;
		int sniffLen =0;
		if (_xmlDecl >= 0) {
			flags |= NSDOC_HASDECL;
			size += 1; // decl is 0 or 1, which is one byte
		}

		if (_encodingStr8) {
			flags |= NSDOC_HASENCODE;
			encLen = NsUtil::nsStringLen(_encodingStr8) + 1;
			size += encLen;
		}
		if (_standaloneStr) {
			if (_standaloneStr[0] == 'y')
				flags |= NSDOC_STANDYES;
			else {
				DBXML_ASSERT(_standaloneStr[0] == 'n');
				flags |= NSDOC_STANDNO;
			}
		}
		if (_sniffedEncodingStr8) {
			flags |= NSDOC_HASSNIFF;
			sniffLen = NsUtil::nsStringLen(_sniffedEncodingStr8) + 1;
			size += sniffLen;
		}
		if (_doNamespace)
			flags |= NSDOC_NAMESPACE;
		if (hasNsNode)
			flags |= NSDOC_HASNSNODE;
		char *buf = (char *) _memManager->allocate(size);
		if (!buf)
			NsUtil::nsThrowException(XmlException::NO_MEMORY_ERROR,
						 "putDocInfo failed to allocate memory",
						 __FILE__, __LINE__);

		// marshal
		xmlbyte_t *ptr = (xmlbyte_t*)buf + NS_ID_OVERHEAD;
		ptr += NsUtil::nsMarshInt(ptr, flags);
		if (_xmlDecl >= 0)
			ptr += NsUtil::nsMarshInt(ptr, _xmlDecl);
		if (_encodingStr8) {
			memcpy(ptr, _encodingStr8, encLen);
			ptr += encLen;
		}
		if (_sniffedEncodingStr8) {
			memcpy(ptr, _sniffedEncodingStr8, sniffLen);
			ptr += sniffLen;
		}

		Dbt data(buf, size);
		buf[0] = NS_PROTOCOL_VERSION;
		buf[1] = NS_METADATA_ID;
		buf [2] = '\0';

		int err = _db->putNodeRecord(oc, _docId, &data);
		_memManager->deallocate(buf);
		if(err != 0)
			throw XmlException(err);
		_xmlDeclModified = false;
	}
}

bool
NsDocument::getDocInfo(void)
{
	DbtOut data((void *)_docMetadataId, strlen(_docMetadataId) + 1);
	int ret = _db->getNodeRecord(_oc, _docId, &data, _flags);
	if (ret == 0) {
		const xmlbyte_t *ptr = (const xmlbyte_t *) data.get_data();
		ptr += 3;  // past version and id
		uint32_t flags;
		int len;
		ptr += NsUtil::nsUnmarshInt(ptr, &flags);
		if (flags & NSDOC_HASDECL) {
			ptr += NsUtil::nsUnmarshInt(ptr, &_xmlDecl);
			DBXML_ASSERT(_xmlDecl == 0 || _xmlDecl == 1);
		}
		if (flags & NSDOC_HASENCODE) {
			DBXML_ASSERT(!_encodingStr8);
			_encodingStr8 = NsUtil::nsStringDup(_memManager,
							   ptr, &len);
			ptr += len;
		}
		if (flags & NSDOC_HASSNIFF) {
			DBXML_ASSERT(!_sniffedEncodingStr8);
			_sniffedEncodingStr8 = NsUtil::nsStringDup(_memManager,
							   ptr, &len);
			ptr += len;
		}
		if (flags & NSDOC_NAMESPACE)
			_doNamespace = true;
		if (flags & NSDOC_HASNSNODE)
			_hasNsNode = true;
		if (flags & NSDOC_STANDYES)
			_standaloneStr = _standYes;
		if (flags & NSDOC_STANDNO)
			_standaloneStr = _standNo;
		_xmlDeclModified = false;
	} else {
		// ID doesn't exist.  handle no-content documents
		_hasContent = false;
	}

	return _hasNsNode;
}

void
NsDocument::getNamespaceInfo(void)
{
	if (_doNamespace) {
		DBXML_ASSERT(_nsInfo);
		DbtOut data((void *)_docNamespaceId,
			    strlen(_docNamespaceId) + 1);
		int ret = _db->getNodeRecord(_oc, _docId, &data, _flags);
		if (ret == 0) {
			_nsInfo->load((const char*)
				      ((const char *)data.get_data())+3);
		}
	}
}

void
NsDocument::putNamespaceInfo(OperationContext &oc, char *buf, int size)
{
	Dbt data(buf, size);
	buf[0] = NS_PROTOCOL_VERSION;
	buf[1] = NS_NAMESPACE_ID;
	buf [2] = '\0';

	int err = _db->putNodeRecord(oc, _docId, &data);
	if(err != 0) throw XmlException(err);
}

void
NsDocument::completeNode(nsNode_t *node)
{
	// if persistent, write node out, and free it.
	int err = 0;
	if (_db) {
		// use local function to put record.  All records
		// are new, so no need to look for existing nodes.
		err = _putNodeRecord(node);
	}
	if(!_domFactory) {
		// If there is no DOM factory, then noone
		// else needs the nsNode_t
		NsUtil::nsFreeNode(_memManager, node);
	}
	if(err != 0) throw XmlException(err);
}

// This version is used when initially creating a document.  It
// streamlines the process, not dealing with the possibility of
// existing nodes
#define STACK_BUF_SIZE 512
int
NsDocument::_putNodeRecord(nsNode_t *node)
{
	// note: the stack-allocation and marshaling code is the same as that
	// in NsDocumentDatabase::putNodeRecord.
	int err = 0;
	unsigned char stackBuf[STACK_BUF_SIZE]; // avoid malloc/free

	unsigned char *buf = stackBuf;
	int bufsize = NsUtil::nsMarshalNode(node, NULL, true);
	if (bufsize > STACK_BUF_SIZE) {
		buf = (unsigned char *) malloc(bufsize);
		if (!buf)
			NsUtil::nsThrowException(XmlException::NO_MEMORY_ERROR,
						 "NsDocument::_putNodeRecord",
						 __FILE__, __LINE__);
	}
	NsUtil::nsMarshalNode(node, buf, false);

	if(Log::isLogEnabled(Log::C_NODESTORE, Log::L_DEBUG)) {
		ostringstream oss;
		int nid_length = NsUtil::nsStringLen((const xmlbyte_t*)buf);
		oss << "updating node did: " << _docId << ", nid: 0x";
		NsUtil::displayNID(oss, (char *)buf + 1, nid_length - 1);
		oss << ", node: ";
		Buffer b(buf + nid_length + 1,
			 bufsize - nid_length - 1, /*wrapper*/true);
		oss << b.asString();

		Log::log(_db->getDbEnv(), Log::C_NODESTORE, Log::L_DEBUG,
			_db->getContainerName().c_str(), oss.str().c_str());
	}

	Dbt data(buf, bufsize);

	DbWrapper &nsdb = _db->getNodeStorageDatabase();
	_docId.setDbtFromThis(_oc.key());
	err = nsdb.put(_oc.txn(), &_oc.key(), &data, 0);

	if (bufsize > STACK_BUF_SIZE)
		free(buf);

	return err;
}


//
// done parsing.
// if there is persistent information to write, write it
//
void
NsDocument::completeDocument()
{
	if (_db) {
		updateSpecialNodes(_oc);
	}
}

void
NsDocument::updateSpecialNodes(OperationContext &oc)
{
	bool hasNsNode = false;
	if (_doNamespace && _nsInfo)
		hasNsNode = _nsInfo->dump(oc, *this);

	// Force an update if the status of NSDOC_HASNSNODE has changed
	if(hasNsNode != _hasNsNode) _xmlDeclModified = true;

	putDocInfo(oc, hasNsNode);
}

void
NsDocument::copyId(nid_t *to, const nid_t *from)
{
	NsUtil::nsCopyId(_memManager, to, from);
}

void NsDocument::freeId(nid_t *id)
{
	NsUtil::nsFreeId(_memManager, id);
}

const xmlbyte_t *
NsDocument::getPrefix8(int32_t prefixIndex)
{
	if (prefixIndex != NS_NOPREFIX) {
		DBXML_ASSERT(_doNamespace && _nsInfo);
		return _nsInfo->getPrefix8(prefixIndex);
	}
	return NULL;
}

const xmlbyte_t *
NsDocument::getUri8(int32_t uriIndex)
{
	if (uriIndex != NS_NOPREFIX && _doNamespace)
		return _nsInfo->getUri8(uriIndex);
	return NULL;
}

const xmlbyte_t *
NsDocument::getUriForPrefix8(int32_t prefixIndex)
{
	if (prefixIndex == NS_NOPREFIX)
		return NULL;
	DBXML_ASSERT(_doNamespace && _nsInfo);
	return _nsInfo->getUri8(_nsInfo->getUriForPrefix(prefixIndex));
}

const xmlch_t *
NsDocument::getPrefix(int32_t prefixIndex)
{
	if (prefixIndex != NS_NOPREFIX) {
		DBXML_ASSERT(_doNamespace && _nsInfo);
		return _nsInfo->getPrefix(prefixIndex);
	}
	return NULL;
}

const xmlch_t *
NsDocument::getUri(int32_t uriIndex)
{
	if (uriIndex != NS_NOPREFIX && _doNamespace)
		return _nsInfo->getUri(uriIndex);
	return NULL;
}

const xmlch_t *
NsDocument::getUriForPrefix(int32_t prefixIndex)
{
	if (prefixIndex == NS_NOPREFIX)
		return NULL;
	DBXML_ASSERT(_doNamespace && _nsInfo);
	return _nsInfo->getUri(_nsInfo->getUriForPrefix(prefixIndex));
}

// utility method to return a qualified name.  If the node is
// utf16, there is no transcoding to do, but there may be
// allocation/copying, if there is a namespace prefix present.
// if allocation is performed, the "owned" boolean is set to true,
// to indicate that the memory is owned by the caller, and must be
// freed.
const xmlch_t *
NsDocument::getQname(const nsName_t *name, bool isUTF16, bool &owned)
{
	const xmlch_t *prefix = getPrefix(name->n_prefix);
	if (isUTF16 && !prefix)
		return (const xmlch_t *)(name->n_text.t_chars);

	// must transcode and/or concat prefix:name
	owned = true;

	// collect len in terms of utf-16 characters to hold
	int plen = 0;
	int len = 1;  // add trailing null
	if (prefix) {
		plen = NsUtil::nsStringLen(prefix) + 1; // add ':'
		len += plen;
	}
	len += name->n_text.t_len;

	xmlch_t *qname = (xmlch_t *)
		_memManager->allocate(len << 1);
	xmlch_t *ptr = qname;
	if (!qname)
		NsUtil::nsThrowException(XmlException::NO_MEMORY_ERROR,
					 "getQname failed to allocate memory",
					 __FILE__, __LINE__);
	if (prefix) {
		memcpy(qname, prefix, ((plen-1) << 1));
		qname[plen-1] = chColon;
		ptr = qname + plen;
	}
	// truncate len back to just the text
	len = len - plen;
	if (!isUTF16) {
		// transcode to ptr
		NsUtil::nsFromUTF8(NULL, &ptr,
				   (const xmlbyte_t*)name->n_text.t_chars,
				   len, len);
	} else {
		memcpy(ptr, name->n_text.t_chars, (len << 1));
	}
	return qname;
}

const xmlch_t *
NsDocument::getText(const nsText_t *text, bool isUTF16,
		    bool &owned)
{
	if (isUTF16)
		return (const xmlch_t *)(text->t_chars);

	// must transcode
	owned = true;
	int len = text->t_len + 1; // add null
	xmlch_t *newText = NULL;
	NsUtil::nsFromUTF8(_memManager, &newText,
			   (const xmlbyte_t*)text->t_chars,
		    len, len);
	DBXML_ASSERT(newText);
	return newText;
}

const xmlbyte_t *
NsDocument::getXmlDecl8() const
{
	if (_xmlDecl >= 0)
		return _NsDecl8[_xmlDecl];
	return 0;
}

const xmlch_t *
NsDocument::getXmlDecl() const
{
	if (_xmlDecl >= 0)
		return _NsDecl[_xmlDecl];
	return 0;
}

void NsDocument::setSniffedEncodingStr(const xmlbyte_t *str) {
	if (_sniffedEncodingStr8)
		_memManager->deallocate((void*)_sniffedEncodingStr8);
	_sniffedEncodingStr8 = str;
	_xmlDeclModified = true;
}

void NsDocument::setEncodingStr(const xmlbyte_t *str) {
	if (_encodingStr8)
		_memManager->deallocate((void*)_encodingStr8);
	if (_encodingStr) {
		// utf-8 version is the reference value
		_memManager->deallocate((void*)_encodingStr);
		_encodingStr = 0;
	}
	_encodingStr8 = str;
	_xmlDeclModified = true;
}

const xmlch_t *NsDocument::getEncodingStr() const
{
	if (!_encodingStr && _encodingStr8) {
		int len = NsUtil::nsStringLen(_encodingStr8) + 1;
		xmlch_t *enc = 0;
		NsUtil::nsFromUTF8(_memManager, &enc,
				   _encodingStr8, len, len);
		_encodingStr = enc;
	}
	return _encodingStr;
}

void
NsDocument::setStandalone(bool standalone)
{
	if (standalone)
		_standaloneStr = _standYes;
	else
		_standaloneStr = _standNo;
	_xmlDeclModified = true;
}

const bool
NsDocument::getIsStandalone() const
{
	if (_standaloneStr && (::strcmp((const char *)_standaloneStr,
					(const char *)_standYes) == 0))
		return true;
	return false;
}

//
// Access to persistent nodes
//
// NOTE: if/when a multi-granularity storage model is implemented,
// this function is the interface where it would happen.
// If the target node were of the (tbd) specified type that is
// unparsed text, it would be parsed on demand into NsDom* objects.
// Because the plumbing links would be present, the new tree would
// fit seamlessly into the persistent tree.  Updates are an issue,
// however.
//

#define GETNODE_BUFFER_SIZE 30

nsNode_t *
NsDocument::_getNode(const nid_t *nid)
{
	if (!_db) {
		NsUtil::nsThrowException(XmlException::INTERNAL_ERROR,
					 "_getNode: db is not initialized",
					 __FILE__, __LINE__);
	}
	if (!nid) {
		// no nid, start at document element
		nid = &Globals::docRootNID;
	}

	// create data key
	int marshalSize = NsUtil::nsCountId(nid) + 1;

	unsigned char buf[GETNODE_BUFFER_SIZE];
	unsigned char *idptr = buf;

	if(marshalSize > GETNODE_BUFFER_SIZE) {
		idptr = (unsigned char*)_memManager->allocate(marshalSize);
	}

	nsNode_t *nsNode = 0;
	try {
		idptr[0] = NS_PROTOCOL_VERSION;
		NsUtil::nsMarshId((unsigned char *)idptr + 1, nid);

		Dbt data;
		data.set_data(idptr);
		data.set_size(marshalSize);
		data.set_flags(DB_DBT_MALLOC);

		int ret = _db->getNodeRecord(_oc, _docId, &data, _flags);
		if(ret == 0) {
			nsNode = NsUtil::nsUnmarshalNode(_memManager,
				(unsigned char *)data.get_data(),
				data.get_size(), /*adoptBuffer*/true);
			DBXML_ASSERT(nsNode);
			// ensure that the new dbt buffer was allocated by DB
			DBXML_ASSERT((unsigned char *)data.get_data() !=
				     idptr);
		}
	}
	catch (DbException &de) {
		if(marshalSize > GETNODE_BUFFER_SIZE) {
			_memManager->deallocate(idptr);
		}

		// probably a deadlock, re-throw
		throw;
	}
	catch(...) {
		// fall through, return null; caller throws
	}

	if(marshalSize > GETNODE_BUFFER_SIZE) {
		_memManager->deallocate(idptr);
	}

	return nsNode;
}

NsDomElement *
NsDocument::fetchDomElement(const nid_t *pnid)
{
	DBXML_ASSERT(_domFactory);

	nsNode_t *nsNode = _getNode(pnid);
	if (!nsNode) {
		ostringstream oss;
		oss << "Could not fetch DOM element for doc id: ";
		oss << _docId.raw();
		throw XmlException(XmlException::INTERNAL_ERROR, oss.str());
	}

	return _domFactory->createNsDomElement(this, nsNode, NULL, false);
}

NsDomElement *
NsDocument::getDomElement(const nid_t *pnid)
{
	if(strcmp((const char*)NsUtil::nsNode(pnid), _docRootId) == 0) {
		return getDocumentNode();
	}

	return getDocumentNode()->findDescendantElement(pnid);
}

// Return the document node, which is of type NsDomElement.
// 2 cases:
//   transient parse -- there is already a pointer stored
//   persistent -- if pointer is not stored, must get it from database
//
// In general, this function must be called before using the DOM
//
NsDomElement *
NsDocument::getDocumentNode()
{
	if (!_domDocNode) {
		_domDocNode = fetchDomElement(0);
		DBXML_ASSERT(_domDocNode);
	}
	return _domDocNode;
}

void
NsDocument::releaseDomElement(NsDomElement *domNode)
{
	/* no-op for now */
}

NodeModification::NodeModification(NsDocument &d, Action a, NsDomElement *n)
	: doc_(d),
	  action_(a)
{
	if(action_ == REMOVE) {
		memset(&value_.nid_, 0, sizeof(nid_t));
		doc_.copyId(&value_.nid_, n->getNodeId());
	}
	else {
		value_.node_ = n;
	}
}

NodeModification::~NodeModification()
{
	reset();
}

const nid_t *NodeModification::getNID() const
{
	return action_ == REMOVE ? &value_.nid_ : value_.node_->getNodeId();
}

const NsDomElement *NodeModification::getNode() const
{
	return action_ == REMOVE ? 0 : value_.node_;
}

void NodeModification::setToAdd(NsDomElement *n) const
{
	reset();
	action_ = ADD;
	value_.node_ = n;
}

void NodeModification::setToUpdate(NsDomElement *n) const
{
	reset();
	action_ = UPDATE;
	value_.node_ = n;
}

void NodeModification::setToRemove(const nid_t *n) const
{
	if(action_ != REMOVE) {
		memset(&value_.nid_, 0, sizeof(nid_t));
		action_ = REMOVE;
	}
	doc_.copyId(&value_.nid_, n); // copyId also frees nid_
}

void NodeModification::reset() const
{
	if(action_ == REMOVE) {
		doc_.freeId(&value_.nid_);
	}
	action_ = (Action)-1;
}

bool NodeModification::operator<(const NodeModification &o) const
{
	return NsUtil::nsCompareIds(getNID(), o.getNID()) < 0;
}

void NsDocument::addToModifications(NodeModification::Action action, NsDomElement *node)
{
	if(_ownerDoc) _ownerDoc->domModified();

	// Don't store modifications to non-database nodes
	if(_db && !nsIsStandalone(node->getNsNode())) {
		NodeModification *newNM = new NodeModification(*this, action, node);
		std::pair<NodeModifications::iterator, bool> inserted =
			_modifications.insert(newNM);
		if(!inserted.second) {
			delete newNM;
			// the Entry is a duplicate, see if it cancels
			// out the existing entry
			switch((*inserted.first)->getAction()) {
			case NodeModification::ADD: {
				switch(action) {
				case NodeModification::ADD:
					DBXML_ASSERT(0);
				case NodeModification::UPDATE:
					(*inserted.first)->setToAdd(node);
					break;
				case NodeModification::REMOVE:
					delete *inserted.first;
					_modifications.erase(inserted.first);
					break;
				}
				break;
			}
			case NodeModification::UPDATE: {
				switch(action) {
				case NodeModification::ADD:
					DBXML_ASSERT(0);
				case NodeModification::UPDATE:
					(*inserted.first)->setToUpdate(node);
					break;
				case NodeModification::REMOVE:
					(*inserted.first)->setToRemove(node->getNodeId());
					break;
				}
				break;
			}
			case NodeModification::REMOVE: {
				switch(action) {
				case NodeModification::ADD:
					(*inserted.first)->setToUpdate(node);
					break;
				case NodeModification::UPDATE:
					DBXML_ASSERT(0);
				case NodeModification::REMOVE:
					DBXML_ASSERT(0);
				}
				break;
			}
			}
		}
	}
}

void NsDocument::clearModifications()
{
	NodeModifications::iterator end = _modifications.end();
	NodeModifications::iterator it = _modifications.begin();
	for(; it != end; ++it) {
		delete *it;
	}
	_modifications.clear();
}

#ifdef NS_DEBUG
void
NsDocument::dumpDocument(const nid_t *nid)
{
	printf("NsDocument::dumpDocument needs to be reimplemented using a cursor\n");
}

void
NsDocument::printNsInfo(void)
{
	if (_nsInfo) {
		printf("\nNAMESPACE INFO\n");
		printf("\nURIs:\n");
        int i;
		for (i = 0; i < _nsInfo->getNUri(); i++)
			printf("\t%d: %s\n", i, _nsInfo->getUri8(i));
		printf("\nPrefixes:\n");
		for (i = 0; i < _nsInfo->getNPrefix(); i++)
			printf("\t%d: %s => %d\n", i, _nsInfo->getPrefix8(i),
			       _nsInfo->getUriForPrefix(i));
	} else {
		printf("\nNO NAMESPACE INFO\n");
	}
}
#endif
