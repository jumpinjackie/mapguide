//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: NsDocument.cpp,v 1.75.2.1 2006/12/08 16:27:50 jsnelson Exp $
//

#include "NsUtil.hpp"
#include "NsDocument.hpp"
#include "NsDom.hpp"
#include "NsConstants.hpp"
#include "NsDocumentDatabase.hpp"
#include "NsFormat.hpp"
#include "../Document.hpp"
#include "../NameID.hpp"
#include <vector>
#include <xercesc/util/XMLUTF8Transcoder.hpp>
#include <xercesc/util/XMLUniDefs.hpp>
#include <sstream>

using namespace DbXml;
using namespace std;
XERCES_CPP_NAMESPACE_USE

//
// internal classes
//
namespace DbXml {
//
// Constants -- see NsConstants.hpp
//
	
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

// constants for XML declaration (1.0 vs 1.1)
const xmlbyte_t _decl8_1_0[] = {'1', '.', '0', 0};
const xmlbyte_t _decl8_1_1[] = {'1', '.', '1', 0};
const xmlch_t _decl_1_0[] = {'1', '.', '0', 0};
const xmlch_t _decl_1_1[] = {'1', '.', '1', 0};
const xmlch_t *_NsDecl[2] = {_decl_1_0, _decl_1_1};
const xmlbyte_t *_NsDecl8[2] = {_decl8_1_0, _decl8_1_1};

class NsDocInfo {
public:
	NsDocInfo(MemoryManager *mgr)
		: mgr_(mgr), xmlDecl_(-1), sniffedEnc_(0), enc_(0),
		standStr_(0), enc16_(0), modified_(false) {}
	~NsDocInfo();
	void putDocInfo(NsDocumentDatabase *ndb,
			OperationContext &oc, const DocID &docId);
	bool getDocInfo(NsDocumentDatabase *ndb,
			OperationContext &oc, const DocID &docId,
			u_int32_t flags);
	const xmlbyte_t *getEncodingStr();
	const xmlbyte_t *getSniffedEncodingStr();
	const xmlbyte_t *getStandaloneStr();
	const xmlch_t *getEncodingStr16();
	int32_t getXmlDecl();

	void setXmlDecl(int32_t decl);
	void setEncodingStr(const xmlbyte_t *str);
	void setSniffedEncodingStr(const xmlbyte_t *str);
	void setStandalone(bool standalone);
	bool getIsStandalone() const;
	bool isModified() const { return modified_; }
	void setModified() { modified_ = true; }
private:
	// NSDOC_FLAGS -- flags stored with a marshaled document
	enum NSDOC_FLAGS {
		NSDOC_STANDYES =  0x0001,
		NSDOC_STANDNO =   0x0002,
		NSDOC_HASENCODE = 0x0004,
		NSDOC_NAMESPACE = 0x0008, /**< do namespaces */
		NSDOC_HASNSNODE = 0x0010, /**< unused as of 2.3 */
		NSDOC_HASDECL   = 0x0020, /**< has xml decl */
		NSDOC_HASSNIFF  = 0x0040  /**< has sniffed encoding */
	};
	MemoryManager *mgr_;
	int32_t xmlDecl_;
	const xmlbyte_t *sniffedEnc_;
	const xmlbyte_t *enc_;
	const xmlbyte_t *standStr_;
	mutable const xmlch_t *enc16_;
	bool modified_;
};

} // namespace DbXml


#if defined(NS_DEBUG) && defined(NODE_COMPAT)

void
NsDocument::_domTraverse(NsDomNode *node)
{
	NsDomNode *current = node;
	while (current) {
		NsNode::dumpNsDomNode(current);
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
// NsDocument implementation
//

// Basic ctor
NsDocument::NsDocument(const Document *ownerDoc, MemoryManager *memManager) :
	_ownerDoc(ownerDoc), _memManager(memManager), _docId(0),
	_hasContent(true), _docInfo(0),
	_documentUri(0), _domFactory(0), _domDocNode(0),
	_modifications(0), _addNsInfo(true), _fromParse(false),
	_domCreateEntityText(false), _ndb(0), _ddb(0)
{
}

// init to create a new node storage doc
void
NsDocument::initDoc(Transaction *txn, NsDocumentDatabase *ndb,
		    DictionaryDatabase *ddb, const DocID &docId,
		    u_int32_t flags)
{
	_oc.set(txn);
	_ndb = ndb;
	_ddb = ddb;
	_docId = docId;
	_flags = flags;
}

//
// init for a pure DOM parse
// Still requires dictionary access, optionally use
// node map
//
void
NsDocument::initDOM(NsDomFactory *factory, const DocID &docID,
		    DictionaryDatabase *ddb)
{
	_domFactory = factory;
	_docId = docID;
	_ddb = ddb;
}

void
NsDocument::initDOMParse(Transaction *txn, uint32_t flags,
			 bool forQuery)
{
	_oc.set(txn);
	DBXML_ASSERT(_ddb);
	_flags = flags;
	_fromParse = true;
	if(forQuery)
		_addNsInfo = false;
}

// init for an index parse -- whole document
void
NsDocument::initIndex()
{
	// nothing for now
}

//
// TBD: for some parses (e.g. DOM), may not
//  want, or need to deallocate -- depends on memory manager
//
NsDocument::~NsDocument()
{
	if (_domFactory) {
		_domFactory->destroyDomList();
	}
	if (_docInfo)
		delete _docInfo;
	clearModifications();
}

//
// Next few methods deal with store/retrieve of namespace
// URI and prefixes.  If there is an actual container for
// the document, use its dictionary for storing mappings
// between strings and IDs.  Otherwise (e.g. parsing a
// transient doc from the file system), use a local data
// structure, destroyed with the object.
//
int
NsDocument::addIDForString(const char *strng, uint32_t len)
{
	DBXML_ASSERT(_ddb);
	DBXML_ASSERT(::strlen(strng) == len);
	NameID id;
	int err = _ddb->lookupIDFromStringName(_oc, strng, len, id, true);
	if (err != 0) {
		std::string error =  "Unable to add a URI or prefix string to dictionary: ";
		error += (std::string)strng;
		NsUtil::nsThrowException(XmlException::DATABASE_ERROR,
					 error.c_str(),
					 __FILE__, __LINE__);
	}
	return (int)id.raw();
}

const char *
NsDocument::getStringForID(int32_t dictId)
{
	if (dictId != NS_NOPREFIX) {
		DBXML_ASSERT(_ddb);
		NameID id(dictId);
		const char *retval = 0;
		(void) _ddb->lookupStringNameFromID(_oc, id, 
						    &retval);
		return retval;
	}
	return NULL;
}

// get string into dbt, pass back data pointer
const char *
NsDocument::getStringForID(int32_t dictId, DbtOut &dbt)
{
	const char *retval = NULL;
	if (dictId != NS_NOPREFIX) {
		DBXML_ASSERT(_ddb);
		NameID id(dictId);
		int err = _ddb->lookupStringNameFromID(_oc, id, dbt);
		if (err == 0)
			retval = (const char *)dbt.get_data();
	}
	return retval;
}

const xmlch_t *
NsDocument::getStringForID16(int32_t dictId)
{
	if (dictId != NS_NOPREFIX) {
		DBXML_ASSERT(_ddb);
		NameID id(dictId);
		const char *retval = 0;
		(void) _ddb->lookupStringNameFromID(_oc, id, 
						    &retval);
		if (retval) {
			// transcode.  Memory is in the _oc.data
			// DBT, which needs to be reallocated for
			// transcoding.  Use a std::string to hold
			// the original (utf-8) string
			std::string temp(retval);
			DbtOut &data(_oc.data());
			uint32_t len = temp.size() + 1;
			data.set(0, len * 3);
			xmlch_t *dest = (xmlch_t *) data.get_data();
			(void)NsUtil::nsFromUTF8(NULL,
						 &dest,
						 (const xmlbyte_t*)temp.c_str(),
						 len, len);
			DBXML_ASSERT(dest == (xmlch_t *)data.get_data());
			return dest;
		}
	}
	return NULL;
}

const xmlch_t *
NsDocument::getStringForID16(int32_t dictId, DbtOut &dbt)
{
	if (dictId != NS_NOPREFIX) {
		DBXML_ASSERT(_ddb);
		NameID id(dictId);
		const char *retval = 0;
		(void) _ddb->lookupStringNameFromID(_oc, id, 
						    &retval);
		if (retval) {
			// transcode.  Memory is in the _oc.data
			// DBT, which needs to be reallocated for
			// transcoding.  Use a std::string to hold
			// the original (utf-8) string
			std::string temp(retval);
			uint32_t len = temp.size() + 1;
			dbt.set(0, len << 3);
			xmlch_t *dest = (xmlch_t *) dbt.get_data();
			(void)NsUtil::nsFromUTF8(NULL,
						 &dest,
						 (const xmlbyte_t*)temp.c_str(),
						 len, len);
			DBXML_ASSERT(dest == (xmlch_t *)dbt.get_data());
			return dest;
		}
	}
	return NULL;
}

void
NsDocument::completeNode(NsNode *node)
{
	// if persistent, write node out, and free it.
	int err = 0;
	if (_ndb) {
		// use local function to put record.  All records
		// are new, so no need to look for existing nodes.
		err = _ndb->putNodeRecord(_oc, _docId, node);
	}
	if(!_domFactory) {
		// If there is no DOM factory, then noone
		// else needs the NsNode
		NsNode::freeNode(_memManager, node);
	}
	if(err != 0) throw XmlException(err);
}

//
// done parsing.
// if there is persistent information to write, write it
//
void
NsDocument::completeDocument()
{
	if (_ndb) {
		updateSpecialNodes(_oc);
	}
}

void
NsDocument::updateSpecialNodes(OperationContext &oc)
{
	if(_docInfo)
		_docInfo->putDocInfo(_ndb, oc, _docId);
}

void
NsDocument::copyId(NsNid *to, const NsNid *from)
{
	to->copyNid(_memManager, from);
}

void NsDocument::freeId(NsNid *id)
{
	id->freeNid(_memManager);
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
	const xmlch_t *prefix = 0;
	if (name->n_prefix != NS_NOPREFIX)
		prefix = getStringForID16(name->n_prefix);
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

/**
 * methods accessing NsDocInfo
 */
const xmlbyte_t *
NsDocument::getEncodingStr() const
{
	ensureDocInfo(true);
	return _docInfo->getEncodingStr();
}

const xmlbyte_t *
NsDocument::getSniffedEncodingStr() const
{
	ensureDocInfo(true);
	return _docInfo->getSniffedEncodingStr();
}

const xmlbyte_t *
NsDocument::getStandaloneStr() const
{
	ensureDocInfo(true);
	return _docInfo->getStandaloneStr();
}

const xmlch_t *NsDocument::getEncodingStr16() const
{
	ensureDocInfo(true);
	return _docInfo->getEncodingStr16();
}

const xmlbyte_t *
NsDocument::getXmlDecl() const
{
	ensureDocInfo(true);
	int32_t decl = _docInfo->getXmlDecl();
	if (decl >= 0)
		return _NsDecl8[decl];
	return 0;
}

const xmlch_t *
NsDocument::getXmlDecl16() const
{
	ensureDocInfo(true);
	int32_t decl = _docInfo->getXmlDecl();
	if (decl >= 0)
		return _NsDecl[decl];
	return 0;
}

void NsDocument::setSniffedEncodingStr(const xmlbyte_t *str) {
	ensureDocInfo(false);
	_docInfo->setSniffedEncodingStr(str);
}

void NsDocument::setEncodingStr(const xmlbyte_t *str) {
	ensureDocInfo(false);
	_docInfo->setEncodingStr(str);
}

void
NsDocument::setStandalone(bool standalone)
{
	ensureDocInfo(false);
	_docInfo->setStandalone(standalone);
}

const bool
NsDocument::getIsStandalone() const
{
	ensureDocInfo(true);
	return _docInfo->getIsStandalone();
}

void
NsDocument::setXmlDecl(int32_t decl)
{
	ensureDocInfo(false);
	_docInfo->setXmlDecl(decl);
}

void
NsDocument::setDocumentUri(const xmlch_t *uri)
{
	// We don't own _documentUri, so we don't have to delete it
	_documentUri = uri;
}

const xmlch_t *
NsDocument::getDocumentUri() const
{
	return _documentUri;
}

bool
NsDocument::hasContent()
{
	getDocumentNode();
	return _hasContent;
}

void
NsDocument::createDocInfo(bool fetch) const
{
	DBXML_ASSERT(!_docInfo);
	_docInfo = new NsDocInfo(_memManager);
	if (fetch) {
		_docInfo->getDocInfo(_ndb, _oc, _docId, _flags);
	}
}

/**
 * NsDocInfo implementation
 */

NsDocInfo::~NsDocInfo()
{
	if (sniffedEnc_)
		mgr_->deallocate((void*)sniffedEnc_);
	if (enc_)
		mgr_->deallocate((void*)enc_);
	if (enc16_)
		mgr_->deallocate((void*)enc16_);
}

const xmlbyte_t *
NsDocInfo::getEncodingStr()
{
	return enc_;
}

const xmlbyte_t *
NsDocInfo::getSniffedEncodingStr()
{
	return sniffedEnc_;
}

const xmlbyte_t *
NsDocInfo::getStandaloneStr()
{
	return standStr_;
}

int32_t
NsDocInfo::getXmlDecl()
{
	return xmlDecl_;
}

void
NsDocInfo::setXmlDecl(int32_t decl)
{
	xmlDecl_ = decl;
	modified_ = true;
}

void
NsDocInfo::setSniffedEncodingStr(const xmlbyte_t *str)
{
	if (sniffedEnc_)
		mgr_->deallocate((void*)sniffedEnc_);
	sniffedEnc_ = NsUtil::nsStringDup(mgr_, str, 0);
	modified_ = true;
}

void
NsDocInfo::setEncodingStr(const xmlbyte_t *str)
{
	if (enc_)
		mgr_->deallocate((void*)enc_);
	enc_ = NsUtil::nsStringDup(mgr_, str, 0);
	modified_ = true;
}

void
NsDocInfo::setStandalone(bool standalone)
{
	if (standalone)
		standStr_ = _standYes;
	else
		standStr_ = _standNo;
	modified_ = true;
}

bool
NsDocInfo::getIsStandalone() const
{
	if (standStr_ && (::strcmp((const char *)standStr_,
				   (const char *)_standYes) == 0))
                return true;
        return false;
}

const xmlch_t *NsDocInfo::getEncodingStr16()
{
	if (!enc16_ && enc_) {
		int len = NsUtil::nsStringLen(enc_) + 1;
		xmlch_t *enc = 0;
		NsUtil::nsFromUTF8(mgr_, &enc, enc_, len, len);
		enc16_ = enc;
	}
	return enc16_;
}

// marshal and put document metadata
void
NsDocInfo::putDocInfo(NsDocumentDatabase *ndb,
		      OperationContext &oc, const DocID &docId)
{
	DBXML_ASSERT(ndb);
	if(isModified()) {
		// buffer size count
		int size = NsNid::getNidOverhead() + 1; // +1 == flags byte
		uint32_t flags = 0;
		int encLen =0;
		int sniffLen =0;
		if (xmlDecl_ >= 0) {
			flags |= NSDOC_HASDECL;
			size += 1; // decl is 0 or 1, which is one byte
		}

		if (enc_) {
			flags |= NSDOC_HASENCODE;
			encLen = NsUtil::nsStringLen(enc_) + 1;
			size += encLen;
		}
		if (standStr_) {
			if (standStr_[0] == 'y')
				flags |= NSDOC_STANDYES;
			else {
				DBXML_ASSERT(standStr_[0] == 'n');
				flags |= NSDOC_STANDNO;
			}
		}
		if (sniffedEnc_) {
			flags |= NSDOC_HASSNIFF;
			sniffLen = NsUtil::nsStringLen(sniffedEnc_) + 1;
			size += sniffLen;
		}
		char *buf = (char *) mgr_->allocate(size);
		if (!buf)
			NsUtil::nsThrowException(XmlException::NO_MEMORY_ERROR,
						 "putDocInfo failed to allocate memory",
						 __FILE__, __LINE__);

		// marshal
		NsNid nid;
		nid.setSpecialNid(0, NS_METADATA_ID);

		xmlbyte_t *ptr = (xmlbyte_t*)buf;
		*ptr++ = NS_PROTOCOL_VERSION;
		ptr += NsFormat::marshalId(ptr, &nid);

		ptr += NsFormat::marshalInt(ptr, flags);
		if (xmlDecl_ >= 0)
			ptr += NsFormat::marshalInt(ptr, xmlDecl_);
		if (enc_) {
			memcpy(ptr, enc_, encLen);
			ptr += encLen;
		}
		if (sniffedEnc_) {
			memcpy(ptr, sniffedEnc_, sniffLen);
			ptr += sniffLen;
		}

		Dbt data(buf, size);
		int err = ndb->putNodeRecord(oc, docId, &nid, &data);
		mgr_->deallocate(buf);
		if(err != 0)
			throw XmlException(err);
		modified_ = false;
	}
}

bool
NsDocInfo::getDocInfo(NsDocumentDatabase *ndb,
		      OperationContext &oc, const DocID &docId,
		      u_int32_t flags)
{
	if(!ndb) return true;

	NsNid nid;
	nid.setSpecialNid(0, NS_METADATA_ID);

	DbtOut data;
	int ret = ndb->getNodeRecord(oc, docId, &nid, &data, flags);
	if (ret == 0) {
		const xmlbyte_t *ptr = (const xmlbyte_t *) data.get_data();
		ptr += NsNid::getNidOverhead(); // past version and id
		uint32_t flags;
		int len;
		ptr += NsFormat::unmarshalInt(ptr, &flags);
		if (flags & NSDOC_HASDECL) {
			ptr += NsFormat::unmarshalInt(ptr, &xmlDecl_);
			DBXML_ASSERT(xmlDecl_ == 0 || xmlDecl_ == 1);
		}
		if (flags & NSDOC_HASENCODE) {
			DBXML_ASSERT(!enc_);
			enc_ = NsUtil::nsStringDup(mgr_, ptr, &len);
			ptr += len;
		}
		if (flags & NSDOC_HASSNIFF) {
			if (sniffedEnc_)
				mgr_->deallocate((void*) sniffedEnc_);
			sniffedEnc_ = NsUtil::nsStringDup(mgr_, ptr, &len);
			ptr += len;
		}
		if (flags & NSDOC_STANDYES)
			standStr_ = _standYes;
		if (flags & NSDOC_STANDNO)
			standStr_ = _standNo;
		modified_ = false;
		return true;
	} else {
		// ID doesn't exist.  handle no-content documents
		return false;
	}
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

NsNode *
NsDocument::_getNode(const NsNid *nid, bool getNext)
{
	if (!_ndb) {
		NsUtil::nsThrowException(XmlException::INTERNAL_ERROR,
					 "fetchDomElement: db is not initialized",
					 __FILE__, __LINE__);
	}
	if (!nid) {
		// no nid, start at document element
		nid = NsNid::getRootNid();
	}
	DBXML_ASSERT((nid->getBytes())[0] != 0);

	// create data key
	DBXML_ASSERT(_ndb);
	if(Log::isLogEnabled(Log::C_NODESTORE, Log::L_DEBUG)) {
		ostringstream oss;
		oss << "fetching";
		if(getNext) oss << " next";
		oss << " node did: " << _docId.asString()
		    << ", nid: 0x";
		nid->displayNid(oss);
		Log::log(_ndb->getDbEnv(), Log::C_NODESTORE, Log::L_DEBUG,
			 _ndb->getContainerName().c_str(), oss.str().c_str());
	}
	

	NsNode *nsNode = 0;
	try {
		Dbt data;
		data.set_flags(DB_DBT_MALLOC);

		if(getNext) {
			int err = _ndb->getNextNodeRecord(_oc, _docId, nid, &data, _flags);
			if(err == 0) {
				unsigned char *datap = (unsigned char *)data.get_data();
				const NsFormat &fmt = NsFormat::getFormat((int)(*datap));

				nsNode = fmt.unmarshalNodeData(_memManager, datap, /*adoptBuffer*/true);
				DBXML_ASSERT(nsNode);

				// Set the node ID in nsNode from the key of the next
				// node, which is contained in context.key()
				DocID did;
				fmt.unmarshalNodeKey(did, nsNode->getNid(), _memManager,
					(unsigned char *)_oc.key().get_data(), /*copyStrings*/true);
			}
		} else {
			int err = _ndb->getNodeRecord(_oc, _docId, nid, &data, _flags);
			if(err == 0) {
				unsigned char *datap = (unsigned char *)data.get_data();
				const NsFormat &fmt = NsFormat::getFormat((int)(*datap));

				nsNode = fmt.unmarshalNodeData(_memManager, datap, /*adoptBuffer*/true);
				DBXML_ASSERT(nsNode);

				// Set the node ID in nsNode
				nsNode->getNid()->copyNid(_memManager, nid);
			}
		}
	}
	catch(DbException &de) {
		// probably a deadlock, re-throw
		throw;
	}
	catch(XmlException &de) {
		// probably a deadlock, re-throw
		throw;
	}
	catch(...) {
		// fall through, return null; caller throws
	}

	return nsNode;
}

NsDomElement *
NsDocument::fetchDomElement(const NsNid *pnid)
{
	if(_fromParse) {
		NsDomElement *docElem = getDocumentNode()->getElemFirstChild();
		if(docElem != 0) {
			return docElem->lookupElement(pnid);
		}
		return 0;
	}

	NsDomElement *result = _fetchDomElement(pnid);
	if (!result) {
		ostringstream oss;
		oss << "Could not fetch DOM element for doc id: ";
		oss << _docId.asString();
		if(pnid != 0) {
			oss << ", nid: ";
			pnid->displayNid(oss);
		}
		else {
			NsNid::getRootNid()->displayNid(oss);
		}
		throw XmlException(XmlException::INTERNAL_ERROR, oss.str());
	}

	return result;
}

NsDomElement *
NsDocument::fetchNextDomElement(const NsNid *pnid)
{
	if(_fromParse) {
		NsDomElement *elem = getDocumentNode()->getElemFirstChild();
		if(elem != 0) {
			elem = elem->lookupElement(pnid);
			if(elem->getElemFirstChild() != 0)
				return elem->getElemFirstChild();
			return elem->getElemNext();
		}
		return 0;
	}

	NsNode *nsNode = _getNode(pnid, /*getNext*/true);
	if(!nsNode) {
		ostringstream oss;
		oss << "Could not fetch next DOM element for doc id: ";
		oss << _docId.asString();
		if(pnid != 0) {
			oss << ", nid: ";
			pnid->displayNid(oss);
		}
		else {
			NsNid::getRootNid()->displayNid(oss);
		}
		throw XmlException(XmlException::INTERNAL_ERROR, oss.str());
	}

	return _domFactory->createNsDomElement(this, nsNode, NULL, false);
}

NsDomElement *
NsDocument::_fetchDomElement(const NsNid *pnid)
{
	DBXML_ASSERT(_domFactory);

	NsNode *nsNode = _getNode(pnid, /*getNext*/false);
	if(!nsNode) return 0;

	return _domFactory->createNsDomElement(this, nsNode, NULL, false);
}

NsDomElement *
NsDocument::getDomElement(const NsNid *pnid)
{
	if (pnid->isDocRootNid())
		return getDocumentNode();
	return fetchDomElement(pnid);
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
		_domDocNode = _fetchDomElement(0);
		_hasContent = _domDocNode != 0;
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
		memset(&value_.nid_, 0, sizeof(NsNid));
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

const NsNid *NodeModification::getNID() const
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

void NodeModification::setToRemove(const NsNid *n) const
{
	if(action_ != REMOVE) {
		memset(&value_.nid_, 0, sizeof(NsNid));
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
	return getNID()->compareNids(o.getNID()) < 0;
}

void NsDocument::addToModifications(NodeModification::Action action,
				    NsDomElement *node)
{
	if (!_modifications)
		_modifications = new NodeModifications();
	if(_ownerDoc) _ownerDoc->domModified();
		
	// Don't store modifications to non-database nodes
	if(_ndb && !node->getNsNode()->isStandalone()) {
		NodeModification *newNM = new NodeModification(*this, action, node);
		std::pair<NodeModifications::iterator, bool> inserted =
			_modifications->insert(newNM);
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
					_modifications->erase(inserted.first);
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
	if (_modifications) {
		NodeModifications::iterator end = _modifications->end();
		NodeModifications::iterator it = _modifications->begin();
		for(; it != end; ++it) {
			delete *it;
		}
		_modifications->clear();
		delete _modifications;
		_modifications = 0;
	}
}

#ifdef NS_DEBUG
void
NsDocument::dumpDocument(const NsNid *nid)
{
	printf("NsDocument::dumpDocument needs to be reimplemented using a cursor\n");
}

#endif
