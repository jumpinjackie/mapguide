//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: NsTransientDomBuilder.cpp,v 1.13 2006/11/17 18:18:29 jsnelson Exp $
//

#include "NsUtil.hpp"
#include "NsTransientDomBuilder.hpp"
#include "NsDom.hpp"
#include "NsDocument.hpp"
#include "NsConstants.hpp"
#include "../UTF8.hpp"
#include <xercesc/framework/MemoryManager.hpp>

using namespace DbXml;
XERCES_CPP_NAMESPACE_USE

NsTransientDomBuilder::NsTransientDomBuilder(MemoryManager *mmgr, NsDocument *doc,
					     NsDomFactory *factory,
						     bool includeEntityInfo) :
	NsHandlerBase(mmgr, doc, 0),
	_domFactory(factory), _domCurrent(0), _domPrevious(0),
	_includeEntityInfo(includeEntityInfo)
{
	// set factory on document as well.
	doc->setNsDomFactory(factory);
}

NsTransientDomBuilder::~NsTransientDomBuilder()
{
}

NsDomElement *
NsTransientDomBuilder::createDomElem(NsNode *node)
{
	DBXML_ASSERT(_domFactory);
	NsDomElement *dnode = _domFactory->createNsDomElement(_doc,
							      node,
							      _domCurrent,
							      false);
	dnode->setElemParent(_domCurrent);

	if(_domCurrent) {
		if (!_domCurrent->getElemFirstChild(false))
			_domCurrent->setElemFirstChild(dnode);
	}
	if (_domPrevious) {
		// setElemPrev sets next for _domPrevious
		dnode->setElemPrev(_domPrevious);
		DBXML_ASSERT(_domPrevious->getElemNext(false) == dnode);
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
	NsNode *node = NsNode::allocNode(_memManager, 0, NS_UTF16|NS_STANDALONE);
	node->setIsDocument();
	node->setNoNav();

	// Set the id
	nextId(node->getNid());
	DBXML_ASSERT(node->getNid()->isDocRootNid());

	// initialize the remainder of the parse state
	_current = node;
	// hook up DOM plumbing
	NsDomElement *dnode = createDomElem(node);
	_doc->setDocumentElement(dnode);

	if (sniffedEncoding && *sniffedEncoding) {
		_doc->setSniffedEncodingStr((const xmlbyte_t*)XMLChToUTF8Null(sniffedEncoding).str());
	}
}

void
NsTransientDomBuilder::xmlDecl(const xmlch_t *xmlDecl,
			       const xmlch_t *encodingStr,
			       const xmlch_t *standaloneStr)
{
	DBXML_ASSERT(_current);
	DBXML_ASSERT(_current->isDoc());

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
		_doc->setEncodingStr((const xmlbyte_t*)XMLChToUTF8Null(encodingStr).str());
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
				    NsEventAttrList16 *attrs,
				    const uint32_t attrCount,
				    bool isEmpty)
{

	// create the node
	NsNode *node = NsNode::allocNode(_memManager,
					 attrCount, NS_UTF16|NS_STANDALONE);
	node->setNoNav(); // do not copy navigation nids
	
	// Allocate an id, even for transient parse; it's used
	// for position comparisons
	nextId(node->getNid());

	// hook up DOM plumbing
	createDomElem(node);

	// initialize the node plumbing
	XMLChToUTF8Null xuri(uri);
	XMLChToUTF8Null xprefix(prefix);
	startElem(node, localName, xuri.str(), xprefix.str(),
		  false, //isUTF8
		  false /* isDonated */);
	
	// because attribute list is type-specific, attribute
	// initialization is per-parser type

	if (attrCount != 0) {	
		for (unsigned int i = 0; i < attrCount; i++) {
			node->addAttr(_doc, attrs->prefix(i),
				      attrs->uri(i), attrs->localName(i),
				      attrs->value(i),
				      attrs->isSpecified(i));
		}
	}

	if (isEmpty)
		endElement(localName, prefix, uri);
}

void
NsTransientDomBuilder::endElement(const xmlch_t *localName,
				  const xmlch_t *prefix,
				  const xmlch_t *uri)
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

void
NsTransientDomBuilder::docTypeDecl(const xmlch_t *name, uint32_t len)
{
	if (_includeEntityInfo) {
		addText((void*)name, len, NS_SUBSET, false, false);
	}
}

void
NsTransientDomBuilder::startEntity(const xmlch_t *name, uint32_t len)
{
	if (_includeEntityInfo) {
		addText((void*)name, len, NS_ENTSTART, false, false);
	}
}

void
NsTransientDomBuilder::endEntity(const xmlch_t *name, uint32_t len)
{
	if (_includeEntityInfo) {
		addText((void*)name, len, NS_ENTEND, false, false);
	}
}
