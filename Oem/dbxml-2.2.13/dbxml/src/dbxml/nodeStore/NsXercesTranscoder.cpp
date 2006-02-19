//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//      Sleepycat Software.  All rights reserved.
//
// $Id: NsXercesTranscoder.cpp,v 1.26 2005/12/07 15:58:18 jsnelson Exp $
//

#include "NsXercesTranscoder.hpp"
#include "NsDom.hpp"
#include "NsDocument.hpp"
#include "NsConstants.hpp"
#include <xercesc/framework/MemoryManager.hpp>

#define nsSetFlag(node, flag) (node)->nd_header.nh_flags |= (flag)

using namespace DbXml;
XERCES_CPP_NAMESPACE_USE

NsXercesTranscoder::NsXercesTranscoder(Transaction *txn, NsDocumentDatabase *db, const ID &docId,
				       u_int32_t flags, XER_NS MemoryManager *mmgr)
	: NsHandlerBase(mmgr, &_document, 0), _document(0, mmgr)
{
	_document.initDoc(txn, db, docId, flags, /*newDoc*/true);
}

NsXercesTranscoder::~NsXercesTranscoder()
{
}

void
NsXercesTranscoder::startDocument(const xmlch_t *sniffedEncoding)
{
	// create the document node
	nsNode_t *node = NsUtil::nsAllocNode(_memManager, 0, NS_STANDALONE);
	node->nd_header.nh_flags |= NS_ISDOCUMENT;
	_current = node;
	
	NsUtil::nsCopyId(_memManager, &node->nd_header.nh_id, &Globals::docRootNID);

	if (sniffedEncoding && *sniffedEncoding) {
		xmlbyte_t *encStr = 0;
		NsUtil::nsToUTF8(_memManager, &encStr, sniffedEncoding,
				 NsUtil::nsStringLen(sniffedEncoding) + 1, 0, 0);
		_doc->setSniffedEncodingStr(encStr);
	}

	// event handler
	if (_handler)
		_handler->startDocument(_doc->getSniffedEncodingStr());
}

void
NsXercesTranscoder::xmlDecl(const xmlch_t *xmlDecl,
			    const xmlch_t *encodingStr,
			    const xmlch_t *standaloneStr)
{
	DBXML_ASSERT(_current && nsIsDoc(_current));
	// initialize document element-specific state
	if (xmlDecl && *xmlDecl) {
		if (NsUtil::nsStringEqual(xmlDecl, _decl_1_0 ))
			_doc->setXmlDecl(NS_DECL_1_0);
		else if (NsUtil::nsStringEqual(xmlDecl, _decl_1_1 ))
			_doc->setXmlDecl(NS_DECL_1_1);
		else
			NsUtil::nsThrowException(XmlException::INTERNAL_ERROR,
						 "startDocument: bad XML decl",
						 __FILE__, __LINE__);
	}
	if (encodingStr && *encodingStr) {
		xmlbyte_t *encStr = 0;
		NsUtil::nsToUTF8(_memManager, &encStr, encodingStr,
				 NsUtil::nsStringLen(encodingStr) + 1, 0, 0);
		_doc->setEncodingStr(encStr);
	}
	if (standaloneStr && *standaloneStr) {
		if (*standaloneStr == (unsigned short)('y'))
			_doc->setStandalone(true);
		else
			_doc->setStandalone(false);
	}
	// event handler
	if (_handler) {
		_handler->xmlDecl(_doc->getXmlDecl8(),
				  _doc->getEncodingStr8(),
				  _doc->getStandaloneStr());
	}
}

void
NsXercesTranscoder::endDocument()
{
	endDoc();
	// event handler
	if (_handler)
		_handler->endDocument();
}

void
NsXercesTranscoder::startElement(const xmlch_t *localName,
				 const xmlch_t *prefix,
				 const xmlch_t *uri,
				 int32_t uriId,
				 NsEventAttrList *attrs,
				 const uint32_t attrCount,
				 bool isEmpty,
				 bool isRoot)
{
	// create the node
	nsNode_t *node = NsUtil::nsAllocNode(_memManager,
					     attrCount, NS_STANDALONE);
	if (isRoot)
		nsSetFlag(node, NS_ISROOT);

	nextId(&node->nd_header.nh_id);
	
	NsDonator lname8(_memManager, localName,
			 NsUtil::nsStringLen(localName));

	NsDonator uri8(_memManager, uri, (uri ? NsUtil::nsStringLen(uri) : 0));
	NsDonator prefix8(_memManager, prefix,
			  (prefix ? NsUtil::nsStringLen(prefix) : 0));

	// initialize the node plumbing
	startElem(node, lname8.getStr(), uriId,
		  uri8.getStr(), prefix8.getStr(), true, true);

	if (attrCount != 0) {	
		for (unsigned int i = 0; i < attrCount; i++) {
			NsUtil::nsAddAttr(_doc, node, attrs->prefix(i),
				attrs->uri(i), attrs->localName(i),
				attrs->value(i), attrs->uriId(i),
				attrs->isSpecified(i));
		}
	}

	// event handler
	if (_handler) {
		// can't trust that the transcoded prefix and uri strings
		// are still valid, so get them here
		const xmlbyte_t *tpref = 0;
		const xmlbyte_t *turi = 0;
		if (nsHasUri(node)) {
			turi = _doc->getUri8(node->nd_header.
					     nh_uriIndex);
			if (nsHasNamePrefix(node))
				tpref = _doc->getPrefix8(node->nd_header.
							 nh_name.n_prefix);
		}
		const xmlbyte_t *tname = (const xmlbyte_t *)
			(node->nd_header.nh_name.n_text.t_chars);
		// constructor is ok with empty attr list.
		NsEventNodeAttrList8 alist(node->nd_attrs, *_doc);
		_handler->startElement(tname, tpref, turi, -1,
				       (attrCount ? &alist : 0),
				       attrCount, isEmpty, isRoot,
				       node);
	}
	
	if (isEmpty)
		endElem();
}

void
NsXercesTranscoder::endElement(const xmlch_t *localName,
			       const xmlch_t *prefix,
			       const xmlch_t *uri,
			       bool isRoot)
{
	if (_handler) {
		// pull utf-8 strings from node, rather than transcoding
		// again.  After endElem(), _current has become _previous
		nsNode_t *node = _current;
		DBXML_ASSERT(node);
		const xmlbyte_t *tpref = 0;
		const xmlbyte_t *turi = 0;
		if (nsHasUri(node)) {
			turi = _doc->getUri8(node->nd_header.
					     nh_uriIndex);
			if (nsHasNamePrefix(node))
				tpref = _doc->getPrefix8(node->nd_header.
							 nh_name.n_prefix);
		}
		const xmlbyte_t *tname = (const xmlbyte_t *)
			(node->nd_header.nh_name.n_text.t_chars);
		_handler->endElement(tname, tpref, turi, isRoot, node);
	}

	// common node work
	endElem();
}

void
NsXercesTranscoder::doCharacters(const xmlch_t *characters,
				 uint32_t len, bool isCDATA,
				 bool ignorable)
{
	bool needsEscape = false;
	if (!len)
		len = NsUtil::nsStringLen(characters);
	enum checkType ttype = (isCDATA || ignorable) ? ignore : isCharacters;
	NsDonator chars(_memManager, characters, len, ttype);
	uint32_t textType;
	if (isCDATA)
		textType = NS_CDATA;
	else {
		textType = NS_TEXT;
		if (chars.getHasEntity()) {
			textType |= NS_ENTITY_CHK;
			needsEscape = true;
		}
	}
	if (ignorable)
		textType |= NS_IGNORABLE;

	// do event before adding text.  If coalesced, the donated
	// string will be deleted by addText().
	if (_handler) {
		if (!ignorable)
			_handler->characters(chars.getStr(), chars.getLen(),
					     isCDATA, needsEscape);
		else
			_handler->ignorableWhitespace(chars.getStr(),
						      chars.getLen(),
						      isCDATA);
	}
	
	addText(chars.getStr(), chars.getLen(), textType,
		true, true);
}

void
NsXercesTranscoder::characters(const xmlch_t *characters,
			       uint32_t len, bool isCDATA,
			       bool needsEscape)
{
	doCharacters(characters, len, isCDATA, false);
}

void
NsXercesTranscoder::ignorableWhitespace(const xmlch_t *characters,
					uint32_t len, bool isCDATA)
{
	doCharacters(characters, len, isCDATA, true);
}

void
NsXercesTranscoder::comment(const xmlch_t *comment, uint32_t len)
{
	if (!len)
		len = NsUtil::nsStringLen(comment);
	NsDonator chars(_memManager, comment, len);
	addText(chars.getStr(), chars.getLen(), NS_COMMENT, true, true);

	// event handler (comment text will never be deleted by addText)
	if (_handler)
		_handler->comment(chars.getStr(), chars.getLen());
}

void
NsXercesTranscoder::processingInstruction(const xmlch_t *target,
					  const xmlch_t *data)
{
	NsDonator chars(_memManager, target, data);
	addPI(chars.getStr(), chars.getStr2(), true, true, chars.getLen());

	// event handler (PI text will never be deleted by addText)
	if (_handler)
		_handler->processingInstruction(chars.getStr(), chars.getStr2());
}

void
NsXercesTranscoder::setTypeInfo(const xmlch_t *name,
				const xmlch_t *uri)
{
	// TBD put type info into _current element -- add method
	// to NsHandlerBase() for this.
	// event handler (PI text will never be deleted by addText)
#if 0	
	NsDonator lname(_memManager, name, NsUtil::nsStringLen(name));
	NsDonator luri(_memManager, uri, (uri ? NsUtil::nsStringLen(uri) : 0));

	// TBD...
	
	if (_handler)
		_handler->setTypeInfo(lname.getStr(), luri.getStr());
#endif	
}

void
NsXercesTranscoder::docTypeDecl(const xmlch_t *data, uint32_t len)
{
	NsDonator chars(_memManager, data, len);
	addText(chars.getStr(), chars.getLen(), NS_SUBSET, true, true);
	if (_handler)
		_handler->docTypeDecl(chars.getStr(), chars.getLen());
}

void
NsXercesTranscoder::startEntity(const xmlch_t *name, uint32_t len)
{
	NsDonator chars(_memManager, name, len);
	addText(chars.getStr(), chars.getLen(), NS_ENTSTART, true, true);
	if (_handler)
		_handler->startEntity(chars.getStr(), chars.getLen());
}

void
NsXercesTranscoder::endEntity(const xmlch_t *name, uint32_t len)
{
	NsDonator chars(_memManager, name, len);
	addText(chars.getStr(), chars.getLen(), NS_ENTEND, true, true);
	if (_handler)
		_handler->endEntity(chars.getStr(), chars.getLen());
}

