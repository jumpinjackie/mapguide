//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//      Sleepycat Software.  All rights reserved.
//
// $Id: NsTransientDomBuilder.cpp,v 1.4 2005/12/07 15:58:17 jsnelson Exp $
//

#include "NsUtil.hpp"
#include "NsTransientDomBuilder.hpp"
#include "NsDom.hpp"
#include "NsDocument.hpp"
#include "NsConstants.hpp"
#include <xercesc/framework/MemoryManager.hpp>

#define nsSetFlag(node, flag) (node)->nd_header.nh_flags |= (flag)

using namespace DbXml;
XERCES_CPP_NAMESPACE_USE

NsTransientDomBuilder::NsTransientDomBuilder(MemoryManager *mmgr, NsDocument *doc,
				 NsDomFactory *factory) :
	NsHandlerBase(mmgr, doc, 0),
	_domFactory(factory), _domCurrent(0), _domPrevious(0)
{
	// set factory on document as well.
	doc->setNsDomFactory(factory);
	
}

NsTransientDomBuilder::~NsTransientDomBuilder()
{
}

NsDomElement *
NsTransientDomBuilder::createDomElem(nsNode_t *node)
{
	DBXML_ASSERT(_domFactory);
	NsDomElement *dnode = _domFactory->createNsDomElement(_doc,
							      node,
							      _domCurrent,
							      false);
	dnode->setElemParent(_domCurrent);

	if(_domCurrent) {
		dnode->setIndex(_domCurrent->getElemChildren().size());
		_domCurrent->getElemChildren().push_back(dnode);
		if (!_domCurrent->getElemFirstChild(false))
			_domCurrent->setElemFirstChild(dnode);
	}
	if (_domPrevious) {
		dnode->setElemPrev(_domPrevious);
		_domPrevious->setElemNext(dnode);
		_domPrevious = 0;
	}
	_domCurrent = dnode;
	_domPrevious = 0;
	return dnode;
}

void
NsTransientDomBuilder::startDocument(const xmlch_t *sniffedEncoding)
{
	// create the document node
	nsNode_t *node = NsUtil::nsAllocNode(_memManager, 0, NS_UTF16|NS_STANDALONE);
	node->nd_header.nh_flags |= NS_ISDOCUMENT;

	// Set the id
	NsUtil::nsCopyId(_memManager, &node->nd_header.nh_id, &Globals::docRootNID);
	
	// initialize the remainder of the parse state
	_current = node;
	// hook up DOM plumbing
	NsDomElement *dnode = createDomElem(node);
	_doc->setDocumentElement(dnode);

	if (sniffedEncoding && *sniffedEncoding) {
		xmlbyte_t *encStr = 0;
		NsUtil::nsToUTF8(_memManager, &encStr, sniffedEncoding,
				 NsUtil::nsStringLen(sniffedEncoding) + 1, 0);
		_doc->setSniffedEncodingStr(encStr);
	}
}

void
NsTransientDomBuilder::xmlDecl(const xmlch_t *xmlDecl,
			 const xmlch_t *encodingStr,
			 const xmlch_t *standaloneStr)
{
	DBXML_ASSERT(_current);
	DBXML_ASSERT(nsIsDoc(_current));

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
				 NsUtil::nsStringLen(encodingStr) + 1, 0);
		_doc->setEncodingStr(encStr);
	}
	if (standaloneStr && *standaloneStr) {
		if (*standaloneStr == (unsigned short)('y'))
			_doc->setStandalone(true);
		else
			_doc->setStandalone(false);
	}
}

void
NsTransientDomBuilder::endDocument()
{
	_domCurrent->setElemLastChild(_domPrevious);

	_domPrevious = 0;
	_domCurrent = 0;
	endDoc();
}

void
NsTransientDomBuilder::startElement(const xmlch_t *localName,
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
					     attrCount, NS_UTF16|NS_STANDALONE);
	if (isRoot)
		nsSetFlag(node, NS_ISROOT);

	// Allocate an id, even for transient parse; it's used
	// for position comparisons
	nextId(&node->nd_header.nh_id);

	// hook up DOM plumbing
	createDomElem(node);

	// initialize the node plumbing
	startElem(node, localName, uriId, uri, prefix, false, false, /*copyIDs*/false);
	
	// because attribute list is type-specific, attribute
	// initialization is per-parser type

	if (attrCount != 0) {	
		for (unsigned int i = 0; i < attrCount; i++) {
			NsUtil::nsAddAttr(_doc, node, attrs->prefix(i),
				attrs->uri(i), attrs->localName(i),
				attrs->value(i), attrs->uriId(i),
				attrs->isSpecified(i));
		}
	}

	if (isEmpty)
		endElement(localName, prefix, uri, isRoot);
}

void
NsTransientDomBuilder::endElement(const xmlch_t *localName,
			    const xmlch_t *prefix,
			    const xmlch_t *uri,
			    bool isRoot)
{
	// DOM plumbing
	_domCurrent->setElemLastChild(_domPrevious);

	_domPrevious = _domCurrent;
	_domCurrent = _domCurrent->getElemParent();
	// common node work
	endElem();
}

void
NsTransientDomBuilder::characters(const xmlch_t *characters,
			    uint32_t len, bool isCDATA,
			    bool needsEscape)
{
	if (!len)
		len = NsUtil::nsStringLen(characters);

	uint32_t type = isCDATA ? NS_CDATA : NS_TEXT;
	if(needsEscape) type |= NS_ENTITY_CHK;

	addText((void*)characters, len, type, false, false);
}

void
NsTransientDomBuilder::ignorableWhitespace(const xmlch_t *characters,
				     uint32_t len, bool isCDATA)
{
	if (!len)
		len = NsUtil::nsStringLen(characters);

	uint32_t type = isCDATA ? NS_CDATA : NS_TEXT;
	type |= NS_IGNORABLE;

	addText((void*)characters, len, type, false, false);
}

void
NsTransientDomBuilder::comment(const xmlch_t *comment, uint32_t len)
{
	if (!len)
		len = NsUtil::nsStringLen(comment);
	addText((void*)comment, len, NS_COMMENT, false, false);
}

void
NsTransientDomBuilder::processingInstruction(const xmlch_t *target,
				       const xmlch_t *data)
{
	addPI((void*)target, (void*)data, false, false, 0);
}

