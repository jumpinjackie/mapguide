//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: NsXercesDom.cpp,v 1.53.2.1 2007/01/18 19:36:13 gmf Exp $
//
#include "NsUtil.hpp"
#include "NsXercesDom.hpp"
#include "NsDocument.hpp"
#include "NsConstants.hpp"
#include <xercesc/dom/DOMTypeInfo.hpp>
#include <xercesc/validators/schema/SchemaSymbols.hpp>
#include <xercesc/dom/DOMException.hpp>

XERCES_CPP_NAMESPACE_USE
using namespace DbXml;


void DbXml::NsXDOMNotImplemented(const char *name)
{
	char buf[500];
	buf[0] = '\0';
	strcat(buf, "XDOM not implemented: ");
	strcat(buf, name);
	NsUtil::nsThrowException(XmlException::DOM_PARSER_ERROR,
				 buf, __FILE__, __LINE__);
}

void DbXml::NsXDOMNotSupported(const char *name)
{
	char buf[500];
	buf[0] = '\0';
	strcat(buf, "XDOM not supported: ");
	strcat(buf, name);
	NsUtil::nsThrowException(XmlException::DOM_PARSER_ERROR,
				 buf, __FILE__, __LINE__);
}

static void NsXDOMNoMemory(const char *name)
{
	char buf[500];
	buf[0] = '\0';
	strcat(buf, "XDOM out of memory: ");
	strcat(buf, name);
	NsUtil::nsThrowException(XmlException::NO_MEMORY_ERROR, buf,
				 __FILE__, __LINE__);
}

//
// methods to cast to DOM objects.  The tricky part is that
// the initial cast must be to the correct NsXDOM type, which is
// then cast to DOMNode
// type mappings:
//   NsDomElement -> either NsXDOMDocument or NsXDOMElement
//   NsDomAttr -> NsXDOMAttr
//   NsDomText -> NsXDOMText
//
static inline DOMElement *fakeDynamicCastDOMElement(NsDomNode *node)
{
	if (!node)
		return 0;

	DBXML_ASSERT(node->getNsNodeType() == nsNodeElement);
	NsXDOMElement *el =
		(NsXDOMElement*)node->getNsInterface("elem");
	return el;
}

static inline DOMAttr *fakeDynamicCastDOMAttr(NsDomNode *node)
{
	if (!node)
		return 0;

	DBXML_ASSERT(node->getNsNodeType() == nsNodeAttr);
	NsXDOMAttr *at =
		(NsXDOMAttr*)node->getNsInterface("attr");
	return at;
}

static inline DOMCDATASection *fakeDynamicCastDOMText(NsDomNode *node)
{
	if (!node)
		return 0;

	DBXML_ASSERT(node->getNsNodeType() == nsNodeText);
	DBXML_ASSERT(nsTextType(((NsDomText*)node)->getNsTextType()) == NS_ENTSTART ||
		  nsTextType(((NsDomText*)node)->getNsTextType()) == NS_ENTEND ||
		  nsTextType(((NsDomText*)node)->getNsTextType()) == NS_SUBSET ||
		  nsTextType(((NsDomText*)node)->getNsTextType()) == NS_CDATA ||
		  nsTextType(((NsDomText*)node)->getNsTextType()) == NS_TEXT);
	NsXDOMText *at =
		(NsXDOMText*)node->getNsInterface("text");
	return at;
}

static inline DOMComment *fakeDynamicCastDOMComment(NsDomNode *node)
{
	if (!node)
		return 0;

	DBXML_ASSERT(node->getNsNodeType() == nsNodeText);
	DBXML_ASSERT(nsTextType(((NsDomText*)node)->getNsTextType()) == NS_COMMENT);
	NsXDOMComment *at =
		(NsXDOMComment*)node->getNsInterface("comment");
	return at;
}

static inline DOMProcessingInstruction *fakeDynamicCastDOMProcessingInstruction(NsDomNode *node)
{
	if (!node)
		return 0;

	DBXML_ASSERT(node->getNsNodeType() == nsNodeText);
	DBXML_ASSERT(nsTextType(((NsDomText*)node)->getNsTextType()) == NS_PINST);
	NsXDOMProcessingInstruction *at =
		(NsXDOMProcessingInstruction*)node->getNsInterface("pi");
	return at;
}

DOMNode *DbXml::fakeDynamicCastDOMNode(NsDomNode *node)
{
	if (!node)
		return 0;

	NsNodeType_t type = node->getNsNodeType();
	switch (type) {
	case nsNodeElement:
	{
		NsXDOMElement *el =
			(NsXDOMElement*)node->getNsInterface("elem");
		return el;
	}
	case nsNodeAttr:
	{
		NsXDOMAttr *at =
			(NsXDOMAttr*)node->getNsInterface("attr");
		return at;
	}
	case nsNodeText:
	case nsNodeEntStart:
	case nsNodeEntEnd:
	{
		switch(nsTextType(((NsDomText*)node)->getNsTextType())) {
		case NS_ENTSTART:
		case NS_ENTEND:
		case NS_SUBSET:
		case NS_CDATA:
		case NS_TEXT:
			return (NsXDOMText*)node->getNsInterface("text");
		case NS_COMMENT:
			return (NsXDOMComment*)node->getNsInterface("comment");
		case NS_PINST:
			return (NsXDOMProcessingInstruction*)node->getNsInterface("pi");
		default:
			DBXML_ASSERT(false);
		}
	}
	case nsNodeDocument:
	{
		NsXDOMDocument *doc =
			(NsXDOMDocument*)node->getNsInterface("document");
		return doc;
	}
	default:
		NsXDOMNotImplemented("bad NsNodeType");
	}
	return 0;
}

static inline DOMNamedNodeMap *fakeDynamicCastDOMNodeMap(NsDomNamedNodeMap *map)
{
	if (!map)
		return 0;

	NsXDOMNamedNodeMap *dmap = (NsXDOMNamedNodeMap*)map->getNsInterface("");
	return dmap;
}

static inline DOMNodeList *fakeDynamicCastDOMNodeList(NsDomNodeList *list)
{
	if (!list)
		return 0;

	NsXDOMNodeList *dlist = (NsXDOMNodeList*)list->getNsInterface("");
	return dlist;
}

//
// DOMTypeInfo class to make XQilla happy
//
class NsXDOMTypeInfo : public DOMTypeInfo
{
public:
	NsXDOMTypeInfo(const XMLCh* uri, const XMLCh* name) :
		_uri(uri), _name(name) {}
	virtual const XMLCh* getName() const { return _name; }
	virtual const XMLCh* getNamespace() const { return _uri; }

protected:
	const XMLCh* _uri, *_name;
};

//
// NsXDOMFactory
//

DOMElement *
NsXDOMFactory::CastToDOMElement(NsDomElement *node)
{
	return fakeDynamicCastDOMElement(node);
}

DOMDocument *
NsXDOMFactory::getDocumentNode(NsDocument *nsdoc)
{
	if(documentNode_ == 0) {
		nsdoc->getDocumentNode();
	}
	return documentNode_;
}

NsDomElement *
NsXDOMFactory::createNsDomElement(NsDocument *doc,
				  NsNode *node,
				  NsDomElement *parent,
				  bool standalone)
{
	// handle both document elements and "normal" elements
	if (node->isDoc()) {
		NsXDOMDocument *de = (NsXDOMDocument *) _memManager->allocate(
			sizeof(NsXDOMDocument));
		if (!de)
			NsXDOMNoMemory("createNsDomDocument");

		(void) new (de) NsXDOMDocument(doc, node);
		documentNode_ = de;
		addToDomFreeList(de);
		return de;
	} else {
		NsXDOMElement *ne = (NsXDOMElement *) _memManager->allocate(
			sizeof(NsXDOMElement));
		if (!ne)
			NsXDOMNoMemory("createNsDomElement");

		(void) new (ne) NsXDOMElement(doc, node, parent, standalone);
		addToDomFreeList(ne);
		return ne;
	}
}

NsDomText *
NsXDOMFactory::createNsDomText(NsDomElement *parent,
			       NsDomElement *owner,
			       int index, uint32_t type)
{
	switch (nsTextType(type)) {
	case NS_ENTSTART:
	case NS_ENTEND:
	case NS_SUBSET:
	case NS_CDATA:
	case NS_TEXT: {
		NsXDOMText *dt = (NsXDOMText *)_memManager->
			allocate(sizeof(NsXDOMText));
		if (!dt) NsXDOMNoMemory("createNsDomText");
		(void) new (dt) NsXDOMText(parent, owner, index, type);
		addToDomFreeList(dt);
		return dt;
	}
	case NS_COMMENT: {
		NsXDOMComment *dt = (NsXDOMComment *)_memManager->
			allocate(sizeof(NsXDOMComment));
		if (!dt) NsXDOMNoMemory("createNsDomText");
		(void) new (dt) NsXDOMComment(parent, owner, index);
		addToDomFreeList(dt);
		return dt;
	}
	case NS_PINST: {
		NsXDOMProcessingInstruction *dt =
			(NsXDOMProcessingInstruction *)_memManager->
			allocate(sizeof(NsXDOMProcessingInstruction));
		if (!dt) NsXDOMNoMemory("createNsDomText");
		(void) new (dt) NsXDOMProcessingInstruction(parent, owner, index);
		addToDomFreeList(dt);
		return dt;
	}
	default:
		DBXML_ASSERT(0);
	}
	return 0;
}

NsDomText *
NsXDOMFactory::createNsDomText(NsDocument *doc, const xmlch_t *value,
			       uint32_t type)
{
	switch (nsTextType(type)) {
	case NS_ENTSTART:
	case NS_ENTEND:
	case NS_SUBSET:
	case NS_CDATA:
	case NS_TEXT: {
		NsXDOMText *dt = (NsXDOMText *)_memManager->
			allocate(sizeof(NsXDOMText));
		if (!dt) NsXDOMNoMemory("createNsDomText");
		(void) new (dt) NsXDOMText(doc, value, type);
		addToDomFreeList(dt);
		return dt;
	}
	case NS_COMMENT: {
		NsXDOMComment *dt = (NsXDOMComment *)_memManager->
			allocate(sizeof(NsXDOMComment));
		if (!dt) NsXDOMNoMemory("createNsDomText");
		(void) new (dt) NsXDOMComment(doc, value);
		addToDomFreeList(dt);
		return dt;
	}
	case NS_PINST:
	default:
		DBXML_ASSERT(0);
	}
	return 0;
}

NsDomText *
NsXDOMFactory::createNsDomText(NsDocument *doc, const xmlch_t *target,
			       const xmlch_t *data)
{
	NsXDOMProcessingInstruction *dt =
		(NsXDOMProcessingInstruction *)_memManager->
		allocate(sizeof(NsXDOMProcessingInstruction));
	if (!dt) NsXDOMNoMemory("createNsDomText");
	(void) new (dt) NsXDOMProcessingInstruction(doc, target, data);
	addToDomFreeList(dt);
	return dt;
}

NsDomNamedNodeMap *
NsXDOMFactory::createNsDomNodeMap(NsDomElement *owner)
{
	NsXDOMNamedNodeMap *nm = (NsXDOMNamedNodeMap *)
		_memManager->allocate(sizeof(NsXDOMNamedNodeMap));
	if (!nm)
		NsXDOMNoMemory("createNsDomNodeMap");
	(void) new (nm) NsXDOMNamedNodeMap(owner);
	addToDomFreeList(nm);
	return nm;
}

NsDomNodeList *
NsXDOMFactory::createNsDomNodeList(NsDomElement *owner)
{
	if(owner == 0 && _emptyNodeList != 0)
		return _emptyNodeList;

	NsXDOMNodeList *nl = (NsXDOMNodeList *)
		_memManager->allocate(sizeof(NsXDOMNodeList));
	if (!nl)
		NsXDOMNoMemory("createNsDomNodeList");
	(void) new (nl) NsXDOMNodeList(owner);
	addToDomFreeList(nl);

	if(owner == 0) _emptyNodeList = nl;
	return nl;
}

NsDomAttr *
NsXDOMFactory::createNsDomAttr(NsDomElement *owner, int index)
{
	NsXDOMAttr *attr = (NsXDOMAttr *)
		_memManager->allocate(sizeof(NsXDOMAttr));
	if (!attr)
		NsXDOMNoMemory("createNsDomAttr");
	(void) new (attr) NsXDOMAttr(owner, index);
	addToDomFreeList(attr);
	return attr;
}

NsDomAttr *
NsXDOMFactory::createNsDomAttr(NsDocument *doc, const xmlch_t *uri,
			       const xmlch_t *qname)
{
	NsXDOMAttr *attr = (NsXDOMAttr *)
		_memManager->allocate(sizeof(NsXDOMAttr));
	if (!attr)
		NsXDOMNoMemory("createNsDomAttr");
	(void) new (attr) NsXDOMAttr(doc, uri, qname);
	addToDomFreeList(attr);
	return attr;
}

NsXDOMFakeText *
NsXDOMFactory::createNsDomFakeText(NsDocument *doc, const xmlch_t *value,
	XNSQ DOMNode *parent)
{
	NsXDOMFakeText *dt = (NsXDOMFakeText *) _memManager->allocate(
		sizeof(NsXDOMFakeText));
	if (!dt)
		NsXDOMNoMemory("createNsDomFakeText");

	(void) new (dt) NsXDOMFakeText(doc, value, parent);
	addToDomFreeList(dt);
	return dt;
}

void
NsXDOMFactory::destroyDomList()
{
	NsDomObj *list = _domFreeList;
	while (list) {
		NsDomObj *node = list->getNextObj();

		list->destroy(); // acts as destructor
		_memManager->deallocate(
			list->getNsInterface("destroy"));
		list = node;
	}
	_domFreeList = 0;
}

void
NsXDOMFactory::destroy(NsDomObj *dnode)
{
	// Unlink the NsDomObj from the free list
	removeFromDomFreeList(dnode);

	// Deallocate the NsDomObj
	dnode->destroy(); // acts as destructor
	_memManager->deallocate(dnode->getNsInterface("destroy"));
}

//
// NsXDOMDocument
//

// Use Xerces implementation (TBD: avoid dipping into Xerces -- may
// do a native implementation -- only need a couple of methods)
XNSQ DOMNodeIterator *
NsXDOMDocument::createNodeIterator(XNSQ DOMNode *root,
				   unsigned long whatToShow,
				   XNSQ DOMNodeFilter* filter,
				   bool entityReferenceExpansion)
{
	// TBD
	// return new XERCES_CPP_NAMESPACE::DOMNodeIteratorImpl
	// (this, root, whatToShow, filter, entityReferenceExpansion);
	NsXDOMNotImplemented("createNodeIterator"); return 0;
}

DOMElement *
NsXDOMDocument::getDocumentElement() const
{
	// This is the root element -- first, and only child of
	// this (Document) node -- it must exist.
	NsDomElement *elem = const_cast<NsXDOMDocument*>(this)->getElemFirstChild();
	DBXML_ASSERT(elem);
	return fakeDynamicCastDOMElement(elem);
}

const XMLCh *
NsXDOMDocument::getVersion() const
{
	if (getNsDocument()->getXmlDecl16())
		return getNsDocument()->getXmlDecl16();
	return _decl_1_0;
}

const XMLCh *
NsXDOMDocument::getDocumentURI() const
{
	return _document->getDocumentUri();
}

void
NsXDOMDocument::setDocumentURI(const XMLCh* documentURI)
{
	_document->setDocumentUri(documentURI);
}

const XMLCh *
NsXDOMDocument::getEncoding() const
{
	return getNsDocument()->getEncodingStr16();
}

const XMLCh *
NsXDOMDocument::getActualEncoding() const
{
	// actual is same as encoding in NsDocument (TBD: fix?)
	return getNsDocument()->getEncodingStr16();
}

bool
NsXDOMDocument::getStandalone() const
{
	return getNsDocument()->getIsStandalone();
}

DOMAttr *
NsXDOMDocument::createAttribute(const XMLCh *name)
{
	NsDomAttr *at = getNsDomFactory()->
		createNsDomAttr(getNsDocument(),
				0, name);
	return fakeDynamicCastDOMAttr(at);
}

DOMAttr *
NsXDOMDocument::createAttributeNS(const XMLCh *namespaceURI,
				  const XMLCh *qualifiedName)
{
	NsDomAttr *at = getNsDomFactory()->
		createNsDomAttr(getNsDocument(),
				namespaceURI,
				qualifiedName);
	return fakeDynamicCastDOMAttr(at);
}

DOMText *
NsXDOMDocument::createTextNode(const XMLCh *data)
{
	NsDomText *txt = getNsDomFactory()->
		createNsDomText(getNsDocument(),
				data,
				NS_TEXT);
	return fakeDynamicCastDOMText(txt);
}

DOMComment *
NsXDOMDocument::createComment(const XMLCh *data)
{
	NsDomText *txt = getNsDomFactory()->
		createNsDomText(getNsDocument(),
				data,
				NS_COMMENT);
	return fakeDynamicCastDOMComment(txt);
}

DOMCDATASection *
NsXDOMDocument::createCDATASection(const XMLCh *data)
{
	NsDomText *txt = getNsDomFactory()->
		createNsDomText(getNsDocument(),
				data,
				NS_CDATA);
	return fakeDynamicCastDOMText(txt);
}

DOMProcessingInstruction *
NsXDOMDocument::createProcessingInstruction(const XMLCh *target,
					    const XMLCh *data)
{
	NsDomText *txt = getNsDomFactory()->
		createNsDomText(getNsDocument(),
				target,
				data);
	return fakeDynamicCastDOMProcessingInstruction(txt);
}

DOMElement *
NsXDOMDocument::createElement(const XMLCh *tagName)
{
	NsDomElement *el = getNsDomFactory()->
		createNsDomElement(getNsDocument(),
				   0, tagName);
	return fakeDynamicCastDOMElement(el);
}

DOMElement *
NsXDOMDocument::createElementNS(const XMLCh *namespaceURI,
				const XMLCh *qualifiedName)
{
	NsDomElement *el = getNsDomFactory()->
		createNsDomElement(getNsDocument(),
				   namespaceURI,
				   qualifiedName);
	return fakeDynamicCastDOMElement(el);
}

DOMNode *
NsXDOMDocument::importNode(DOMNode *importedNode, bool deep)
{
	switch(importedNode->getNodeType()) {
	case DOMNode::ATTRIBUTE_NODE: {
		DOMAttr *newAttr =
			createAttributeNS(importedNode->getNamespaceURI(),
					  importedNode->getNodeName());
		newAttr->setNodeValue(importedNode->getNodeValue());
		return newAttr;
	}
	case DOMNode::ELEMENT_NODE: {
		DOMElement *newElement =
			createElementNS(importedNode->getNamespaceURI(),
					importedNode->getNodeName());
		DOMNamedNodeMap *attrs = importedNode->getAttributes();
		DOMAttr *attr;
		XMLSize_t length = attrs->getLength();
		for(XMLSize_t index = 0; index < length; ++index) {
			attr = (DOMAttr*)attrs->item(index);
			if(attr->getSpecified()) {
				newElement->setAttributeNodeNS(
					(DOMAttr*)importNode(attr, deep));
			}
		}
		if(deep) {
			DOMNode *child = importedNode->getFirstChild();
			while(child != 0) {
				newElement->appendChild(importNode(child, deep));
				child = child->getNextSibling();
			}
		}
		return newElement;
	}
	case DOMNode::PROCESSING_INSTRUCTION_NODE: {
		DOMProcessingInstruction *newPI =
			createProcessingInstruction(importedNode->getNodeName(),
						    importedNode->getNodeValue());
		return newPI;
	}
	case DOMNode::TEXT_NODE: {
		DOMText *newText =
			createTextNode(importedNode->getNodeValue());
		return newText;
	}
	case DOMNode::CDATA_SECTION_NODE: {
		DOMCDATASection *newCDATASection =
			createCDATASection(importedNode->getNodeValue());
		return newCDATASection;
	}
	case DOMNode::COMMENT_NODE: {
		DOMComment *newComment =
			createComment(importedNode->getNodeValue());
		return newComment;
	}
	case DOMNode::DOCUMENT_TYPE_NODE: {
		// For now, this type is implemented as a special
		// text type
		DOMText *newText =
			createDocumentTypeTextNode(importedNode);
		return newText;
	}
	case DOMNode::DOCUMENT_NODE:
		throw XNSQ DOMException(XNSQ DOMException::NOT_SUPPORTED_ERR, 0, getNsDocument()->getMemoryManager());
	case DOMNode::ENTITY_NODE:
		NsXDOMNotImplemented("DOMEntity");
	case DOMNode::ENTITY_REFERENCE_NODE:
		NsXDOMNotImplemented("DOMEntityReference");
	case DOMNode::NOTATION_NODE:
		NsXDOMNotImplemented("DOMNotation");
	case DOMNode::DOCUMENT_FRAGMENT_NODE:
		NsXDOMNotImplemented("DOMDocumentFragment");
	default:
		NsXDOMNotImplemented("Unknown Node Type");
	}
	return 0;
}

DOMNode *
NsXDOMDocument::adoptNode(DOMNode* source)
{
	switch(source->getNodeType()) {
	case DOMNode::ATTRIBUTE_NODE: {
		DOMAttr *attr = (DOMAttr*)source;
		DOMElement *owner = attr->getOwnerElement();
		if(owner != 0) owner->removeAttributeNode(attr);
	}
	case DOMNode::TEXT_NODE:
	case DOMNode::CDATA_SECTION_NODE:
	case DOMNode::COMMENT_NODE:
	case DOMNode::PROCESSING_INSTRUCTION_NODE:
	case DOMNode::ELEMENT_NODE: {
		DOMNode *parent = source->getParentNode();
		if(parent != 0) parent->removeChild(source);
	}
	case DOMNode::DOCUMENT_TYPE_NODE:
	case DOMNode::DOCUMENT_NODE:
		throw XNSQ DOMException(XNSQ DOMException::NOT_SUPPORTED_ERR, 0, getNsDocument()->getMemoryManager());
	case DOMNode::ENTITY_NODE:
		NsXDOMNotImplemented("DOMEntity");
	case DOMNode::ENTITY_REFERENCE_NODE:
		NsXDOMNotImplemented("DOMEntityReference");
	case DOMNode::NOTATION_NODE:
		NsXDOMNotImplemented("DOMNotation");
	case DOMNode::DOCUMENT_FRAGMENT_NODE:
		NsXDOMNotImplemented("DOMDocumentFragment");
	}
	return importNode(source, true);
}

DOMNode *
NsXDOMDocument::renameNode(DOMNode* arg, const XMLCh* namespaceURI,
			   const XMLCh* qualifiedName)
{
	if(arg->getOwnerDocument() != this) {
		throw XNSQ DOMException(XNSQ DOMException::WRONG_DOCUMENT_ERR, 0, getNsDocument()->getMemoryManager());
	}

	// Check for prefix if there isn't a uri
	if(namespaceURI == 0) {
		const XMLCh *ptr = qualifiedName;
		while(*ptr != 0) {
			if(*ptr == ':') {
				throw XNSQ DOMException(XNSQ DOMException::NAMESPACE_ERR, 0, getNsDocument()->getMemoryManager());
			}
			++ptr;
		}
	}

	// check for prefix of "xml" or "xmlns"
	if(qualifiedName[0] == 'x' &&
	   qualifiedName[1] == 'm' &&
	   qualifiedName[2] == 'l') {
		if(qualifiedName[3] == ':') {
			if(!NsUtil::nsStringEqual(namespaceURI, _xmlUri16)) {
				throw XNSQ DOMException(XNSQ DOMException::NAMESPACE_ERR, 0, getNsDocument()->getMemoryManager());
			}
		}
		else if(qualifiedName[3] == 'n' &&
			qualifiedName[4] == 's' &&
			qualifiedName[5] == ':') {
			if(!NsUtil::nsStringEqual(namespaceURI, _xmlnsUri16)) {
				throw XNSQ DOMException(XNSQ DOMException::NAMESPACE_ERR, 0, getNsDocument()->getMemoryManager());
			}
		}
	}

	switch(arg->getNodeType()) {
	case DOMNode::ATTRIBUTE_NODE: {
		NsDomAttr *attr = (NsDomAttr*)fakeDynamicCastNsDomNode(arg);
		attr->nsRename(namespaceURI, qualifiedName);
		break;
	}
	case DOMNode::ELEMENT_NODE: {
		NsDomElement *el = (NsDomElement*)fakeDynamicCastNsDomNode(arg);
		el->nsRename(namespaceURI, qualifiedName);
		break;
	}
	default: {
		throw XNSQ DOMException(XNSQ DOMException::NOT_SUPPORTED_ERR, 0, getNsDocument()->getMemoryManager());
	}
	}

	return arg;
}

//
// NsXDOMDocument DOMNode implementation
//
// Methods are slightly complicated by the
// const-ness of DOM, whereas the NsDom objects
// have side effects (materialization/caching).
//
const XMLCh *
NsXDOMDocument::getNodeName() const
{
	return getNsNodeName();
}

const XMLCh *
NsXDOMDocument::getNodeValue() const
{
	return 0;
}

short
NsXDOMDocument::getNodeType() const
{
	DBXML_ASSERT(getNsNodeType() == nsNodeDocument);
	return DOMNode::DOCUMENT_NODE;
}

DOMNode *
NsXDOMDocument::getParentNode() const
{
	return 0; // no parent for document
}

DOMNodeList *
NsXDOMDocument::getChildNodes() const
{
	return fakeDynamicCastDOMNodeList(getNsDomFactory()->createNsDomNodeList(0));
}

DOMNode *
NsXDOMDocument::getFirstChild() const
{
	NsDomNode *node = const_cast<NsXDOMDocument *>(this)->getNsFirstChild();
	return fakeDynamicCastDOMNode(node);
}

DOMNode *
NsXDOMDocument::getLastChild() const
{
	NsDomNode *node = const_cast<NsXDOMDocument *>(this)->getNsLastChild();
	return fakeDynamicCastDOMNode(node);
}

DOMNode *
NsXDOMDocument::getPreviousSibling() const
{
	return 0;
}

DOMNode *
NsXDOMDocument::getNextSibling() const
{
	return 0;
}

DOMNamedNodeMap *
NsXDOMDocument::getAttributes() const
{
	return 0;
}

DOMDocument *
NsXDOMDocument::getOwnerDocument() const
{
	// DOMDocument::getOwnerDocument() is defined to return null
	return 0;
}

DOMNode *
NsXDOMDocument::cloneNode(bool deep) const
{
	return const_cast<NsXDOMDocument*>(this)->
		importNode(const_cast<NsXDOMDocument*>(this), deep);
}

DOMNode *
NsXDOMDocument::removeChild(DOMNode *oldChild)
{
	if (!isSameNode(oldChild->getParentNode())) {
		throw XNSQ DOMException(XNSQ DOMException::NOT_FOUND_ERR, 0, getNsDocument()->getMemoryManager());
	}
	NsDomNode *child = fakeDynamicCastNsDomNode(oldChild);
	return fakeDynamicCastDOMNode(removeNsChild(child));
}

DOMNode *
NsXDOMDocument::appendChild(DOMNode *newChild)
{
	if(newChild->getOwnerDocument() != this) {
		throw XNSQ DOMException(XNSQ DOMException::WRONG_DOCUMENT_ERR, 0, getNsDocument()->getMemoryManager());
	}
	NsDomNode *child = fakeDynamicCastNsDomNode(newChild);
	if(child->getNsNodeType() != nsNodeElement &&
	   child->getNsNodeType() != nsNodeText) {
		throw XNSQ DOMException(XNSQ DOMException::HIERARCHY_REQUEST_ERR, 0, getNsDocument()->getMemoryManager());
	}

	if(newChild->getParentNode() != 0) {
		newChild = newChild->getParentNode()->removeChild(newChild);
		child = fakeDynamicCastNsDomNode(newChild);
	}
	return fakeDynamicCastDOMNode(insertNsChild(child, 0, nsDomInsertAppend));
}

DOMNode *
NsXDOMDocument::insertBefore(DOMNode *newChild, DOMNode *refChild)
{
	if(refChild == 0) return appendChild(newChild);

	if(newChild->getOwnerDocument() != this) {
		throw XNSQ DOMException(XNSQ DOMException::WRONG_DOCUMENT_ERR, 0, getNsDocument()->getMemoryManager());
	}

	if(!isSameNode(refChild->getParentNode())) {
		throw XNSQ DOMException(XNSQ DOMException::NOT_FOUND_ERR, 0, getNsDocument()->getMemoryManager());
	}

	NsDomNode *child = fakeDynamicCastNsDomNode(newChild);
	if(child->getNsNodeType() != nsNodeElement &&
	   child->getNsNodeType() != nsNodeText) {
		throw XNSQ DOMException(XNSQ DOMException::HIERARCHY_REQUEST_ERR, 0, getNsDocument()->getMemoryManager());
	}

	if(newChild == refChild) return newChild;

	if(newChild->getParentNode() != 0) {
		newChild = newChild->getParentNode()->removeChild(newChild);
		child = fakeDynamicCastNsDomNode(newChild);
	}
	NsDomNode *rchild = fakeDynamicCastNsDomNode(refChild);
	return fakeDynamicCastDOMNode(insertNsChild(child, rchild, nsDomInsertBefore));
}

DOMNode *
NsXDOMDocument::replaceChild(DOMNode *newChild, DOMNode *oldChild)
{
	newChild = insertBefore(newChild, oldChild);
	return removeChild(oldChild);
}

bool
NsXDOMDocument::hasChildNodes() const
{
	return hasNsChildNodes();
}

const XMLCh *
NsXDOMDocument::getNamespaceURI() const
{
	return 0;
}

const XMLCh *
NsXDOMDocument::getPrefix() const
{
	return 0;
}

const XMLCh *
NsXDOMDocument::getLocalName() const
{
	return 0;
}

bool
NsXDOMDocument::hasAttributes() const
{
	return false;
}

bool
NsXDOMDocument::isSameNode(const DOMNode* other) const
{
	if ((const DOMNode *)this == other)
		return true;
	else
		return operator==(*fakeDynamicCastNsDomNodeC(other));
}

bool
NsXDOMDocument::isEqualNode(const DOMNode* arg) const
{
	const NsDomNode *thisNode = fakeDynamicCastNsDomNodeC(this);
	return compareNsNodes(thisNode, (const NsDomNode *)
			      (((DOMNode*)arg)->getInterface(_nsDomString)));
}

const XMLCh *
NsXDOMDocument::getBaseURI() const
{
	return _document->getDocumentUri();
}

static short
_compareNsPositions(const DOMNode *node1, const DOMNode *node2)
{
	if(node1 == 0 || node2 == 0 ||
	   !(node1->getNodeType() == DOMNode::DOCUMENT_NODE ? node1 : node1->getOwnerDocument())->
		isSameNode(node2->getNodeType() == DOMNode::DOCUMENT_NODE ? node2 : node2->getOwnerDocument())) {
		return DOMNode::TREE_POSITION_DISCONNECTED;
	}

	short nsret = NsDomNode::compareNsPositions(
		fakeDynamicCastNsDomNodeC(node1),
		fakeDynamicCastNsDomNodeC(node2));
	return nsret;
}

short
NsXDOMDocument::compareTreePosition(const DOMNode* other) const
{
	return _compareNsPositions(this, other);
}

const XMLCh *
NsXDOMDocument::getTextContent() const
{
	return 0;
}

const XMLCh *
NsXDOMDocument::lookupNamespacePrefix(const XMLCh* namespaceURI,
				      bool useDefault) const
{

	return getDocumentElement()->lookupNamespacePrefix(namespaceURI,
							   useDefault);
}

const XMLCh *
NsXDOMDocument::lookupNamespaceURI(const XMLCh* prefix) const
{
	return getDocumentElement()->lookupNamespaceURI(prefix);
}

// non-standard Xerces extensions
void
NsXDOMDocument::release()
{
	//TBD -- no-op for now
}

// private implementation

// create a text node that contains the information from
// a DOMDocumentType node.  This is mostly the internal subset,
// so the type of the underlying nsDomText node is NS_SUBSET.
DOMText *
NsXDOMDocument::createDocumentTypeTextNode(DOMNode *docTypeNode)
{
#if 1
	// TBD (not used/needed at this time)
	return 0;
#else
	// serialize the information
	XMLCh *data = 0;
	NsDomText *txt = getNsDomFactory()->
		createNsDomText(getNsDocument(),
				data,
				NS_SUBSET);
	return fakeDynamicCastDOMText(txt);
#endif
}

//
// NSXDOMElement implementation
//

DOMAttr *
NsXDOMElement::getAttributeNode(const XMLCh *name) const
{
	return (DOMAttr*)getAttributes()->getNamedItem(name);
}

DOMAttr *
NsXDOMElement::setAttributeNode(XNSQ DOMAttr *newAttr)
{
	return (DOMAttr*)getAttributes()->setNamedItem(newAttr);
}

DOMAttr *
NsXDOMElement::removeAttributeNode(XNSQ DOMAttr *oldAttr)
{
	if(oldAttr->getOwnerElement() != this) {
		throw XNSQ DOMException(XNSQ DOMException::NOT_FOUND_ERR, 0, getNsDocument()->getMemoryManager());
	}
	NsDomAttr *attr = (NsDomAttr*)fakeDynamicCastNsDomNode(oldAttr);

	return fakeDynamicCastDOMAttr(_attributes->removeNsItem(attr->getIndex()));
}

const XMLCh *
NsXDOMElement::getAttribute(const XMLCh *name) const
{
	DOMAttr *attr = (DOMAttr*)getAttributes()->getNamedItem(name);
	if(attr) {
		return attr->getNodeValue();
	}
	return 0;
}

bool
NsXDOMElement::hasAttribute(const XMLCh *name) const
{
	return getAttribute(name) != 0;
}

void
NsXDOMElement::setAttribute(const XMLCh *name, const XMLCh *value)
{
	DOMAttr *attr = getOwnerDocument()->createAttribute(name);
	attr->setNodeValue(value);
	attr = setAttributeNode(attr);
	if(attr != 0) attr->release();
}

void
NsXDOMElement::removeAttribute(const XMLCh *name)
{
	_attributes->removeNsNamedItem(name);
}

DOMAttr *
NsXDOMElement::getAttributeNodeNS(const XMLCh *namespaceURI,
				  const XMLCh *localName) const
{
	return (DOMAttr*)getAttributes()->getNamedItemNS(namespaceURI, localName);
}

DOMAttr *
NsXDOMElement::setAttributeNodeNS(XNSQ DOMAttr *newAttr)
{
	return (DOMAttr*)getAttributes()->setNamedItemNS(newAttr);
}

const XMLCh *
NsXDOMElement::getAttributeNS(const XMLCh *namespaceURI,
			      const XMLCh *localName) const
{
	DOMAttr *attr = (DOMAttr*)getAttributes()->getNamedItemNS(namespaceURI, localName);
	if(attr) {
		return attr->getNodeValue();
	}
	return 0;
}

bool
NsXDOMElement::hasAttributeNS(const XMLCh *namespaceURI,
			      const XMLCh *localName) const
{
	return getAttributeNS(namespaceURI, localName) != 0;
}

void
NsXDOMElement::setAttributeNS(const XMLCh *namespaceURI,
			      const XMLCh *qualifiedName,
			      const XMLCh *value)
{
	DOMAttr *attr = getOwnerDocument()->
		createAttributeNS(namespaceURI, qualifiedName);
	attr->setNodeValue(value);
	attr = setAttributeNodeNS(attr);
	if(attr != 0) attr->release();
}

void
NsXDOMElement::removeAttributeNS(const XMLCh *namespaceURI,
				 const XMLCh *localName)
{
	_attributes->removeNsNamedItemNS(namespaceURI, localName);
}

const DOMTypeInfo*
NsXDOMElement::getTypeInfo() const
{
	static NsXDOMTypeInfo
		g_untypedElement(SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
				 SchemaSymbols::fgATTVAL_ANYTYPE);
	return &g_untypedElement;
}

//
// NsXDOMElement DOMNode implementation
//
const XMLCh *
NsXDOMElement::getNodeName() const
{
	return getNsNodeName();
}

const XMLCh *
NsXDOMElement::getNodeValue() const
{
	return 0;
}

short
NsXDOMElement::getNodeType() const
{
	return DOMNode::ELEMENT_NODE;
}

DOMNode *
NsXDOMElement::getParentNode() const
{
	NsDomNode *node = const_cast<NsXDOMElement *>(this)->getNsParentNode();
	return fakeDynamicCastDOMNode(node);
}

DOMNodeList *
NsXDOMElement::getChildNodes() const
{
	return fakeDynamicCastDOMNodeList(
		const_cast<NsXDOMElement *>(this)->getNsChildNodes());
}

DOMNode *
NsXDOMElement::getFirstChild() const
{
	NsDomNode *node = const_cast<NsXDOMElement *>(this)->getNsFirstChild();
	return fakeDynamicCastDOMNode(node);
}

DOMNode *
NsXDOMElement::getLastChild() const
{
	NsDomNode *node = const_cast<NsXDOMElement *>(this)->getNsLastChild();
	return fakeDynamicCastDOMNode(node);
}

DOMNode *
NsXDOMElement::getPreviousSibling() const
{
	NsDomNode *node = const_cast<NsXDOMElement *>(this)->getNsPrevSibling();
	return fakeDynamicCastDOMNode(node);
}

DOMNode *
NsXDOMElement::getNextSibling() const
{
	NsDomNode *node = const_cast<NsXDOMElement *>(this)->getNsNextSibling();
	return fakeDynamicCastDOMNode(node);
}

DOMNamedNodeMap *
NsXDOMElement::getAttributes() const
{
	NsDomNamedNodeMap *map =
		const_cast<NsXDOMElement *>(this)->getNsAttributes();
	return fakeDynamicCastDOMNodeMap(map);
}

DOMDocument *
NsXDOMElement::getOwnerDocument() const
{
	return (NsXDOMDocument *) ((NsXDOMElement*)this)->getNsDocumentObj();
}

DOMNode *
NsXDOMElement::cloneNode(bool deep) const
{
	return getOwnerDocument()->importNode(const_cast<NsXDOMElement*>(this), deep);
}

DOMNode *
NsXDOMElement::removeChild(DOMNode *oldChild)
{
	if(oldChild->getParentNode() != this) {
		throw XNSQ DOMException(XNSQ DOMException::NOT_FOUND_ERR, 0, getNsDocument()->getMemoryManager());
	}
	NsDomNode *child = fakeDynamicCastNsDomNode(oldChild);
	return fakeDynamicCastDOMNode(removeNsChild(child));
}

DOMNode *
NsXDOMElement::appendChild(DOMNode *newChild)
{
	if(newChild->getOwnerDocument() != getOwnerDocument()) {
		throw XNSQ DOMException(XNSQ DOMException::WRONG_DOCUMENT_ERR, 0, getNsDocument()->getMemoryManager());
	}
	NsDomNode *child = fakeDynamicCastNsDomNode(newChild);
	if(child->getNsNodeType() != nsNodeElement &&
	   child->getNsNodeType() != nsNodeText) {
		throw XNSQ DOMException(XNSQ DOMException::HIERARCHY_REQUEST_ERR, 0, getNsDocument()->getMemoryManager());
	}
	// TBD: look for a better/faster way to check that new
	// child is not an ancestor of "this" node
	DOMNode *parent = this;
	while(parent != 0) {
		if(parent == newChild)
			throw XNSQ DOMException(XNSQ DOMException::HIERARCHY_REQUEST_ERR, 0, getNsDocument()->getMemoryManager());
		parent = parent->getParentNode();
	}

	if(newChild->getParentNode() != 0) {
		newChild = newChild->getParentNode()->removeChild(newChild);
		child = fakeDynamicCastNsDomNode(newChild);
	}
	return fakeDynamicCastDOMNode(insertNsChild(child, 0, nsDomInsertAppend));
}

DOMNode *
NsXDOMElement::insertBefore(DOMNode *newChild, DOMNode *refChild)
{
	if(refChild == 0) return appendChild(newChild);

	if(newChild->getOwnerDocument() != getOwnerDocument()) {
		throw XNSQ DOMException(XNSQ DOMException::WRONG_DOCUMENT_ERR, 0, getNsDocument()->getMemoryManager());
	}

	if(refChild->getParentNode() != this) {
		throw XNSQ DOMException(XNSQ DOMException::NOT_FOUND_ERR, 0, getNsDocument()->getMemoryManager());
	}

	NsDomNode *child = fakeDynamicCastNsDomNode(newChild);
	if(child->getNsNodeType() != nsNodeElement &&
	   child->getNsNodeType() != nsNodeText) {
		throw XNSQ DOMException(XNSQ DOMException::HIERARCHY_REQUEST_ERR, 0, getNsDocument()->getMemoryManager());
	}
	DOMNode *parent = this;
	while(parent != 0) {
		if(parent == newChild)
			throw XNSQ DOMException(XNSQ DOMException::HIERARCHY_REQUEST_ERR, 0, getNsDocument()->getMemoryManager());
		parent = parent->getParentNode();
	}

	if(newChild == refChild) return newChild;

	if(newChild->getParentNode() != 0) {
		newChild = newChild->getParentNode()->removeChild(newChild);
		child = fakeDynamicCastNsDomNode(newChild);
	}
	NsDomNode *rchild = fakeDynamicCastNsDomNode(refChild);
	return fakeDynamicCastDOMNode(insertNsChild(child, rchild, nsDomInsertBefore));
}

DOMNode *
NsXDOMElement::insertBefore(DOMNode *newChild, DOMNode *refChild,
			    NsDomInsertType itype)
{
	if(refChild == 0) return appendChild(newChild);

	if(newChild->getOwnerDocument() != getOwnerDocument()) {
		throw XNSQ DOMException(XNSQ DOMException::WRONG_DOCUMENT_ERR, 0, getNsDocument()->getMemoryManager());
	}

	if (!isSameNode(refChild->getParentNode())) {
		throw XNSQ DOMException(XNSQ DOMException::NOT_FOUND_ERR, 0, getNsDocument()->getMemoryManager());
	}

	NsDomNode *child = fakeDynamicCastNsDomNode(newChild);
	if(child->getNsNodeType() != nsNodeElement &&
	   child->getNsNodeType() != nsNodeText) {
		throw XNSQ DOMException(XNSQ DOMException::HIERARCHY_REQUEST_ERR, 0, getNsDocument()->getMemoryManager());
	}
	DOMNode *parent = this;
	while(parent != 0) {
		if(parent == newChild)
			throw XNSQ DOMException(XNSQ DOMException::HIERARCHY_REQUEST_ERR, 0, getNsDocument()->getMemoryManager());
		parent = parent->getParentNode();
	}

	if(newChild == refChild) return newChild;

	if(newChild->getParentNode() != 0) {
		newChild = newChild->getParentNode()->removeChild(newChild);
		child = fakeDynamicCastNsDomNode(newChild);
	}
	NsDomNode *rchild = fakeDynamicCastNsDomNode(refChild);
	return fakeDynamicCastDOMNode(insertNsChild(child, rchild, itype));
}

DOMNode *
NsXDOMElement::replaceChild(DOMNode *newChild, DOMNode *oldChild)
{
	newChild = insertBefore(newChild, oldChild);
	return removeChild(oldChild);
}

bool
NsXDOMElement::hasChildNodes() const
{
	return hasNsChildNodes();
}

const XMLCh *
NsXDOMElement::getNamespaceURI() const
{
	return getNsUri();
}

const XMLCh *
NsXDOMElement::getPrefix() const
{
	return getNsPrefix();
}

void NsXDOMElement::setPrefix(const XMLCh *prefix)
{
	if(getNsUri() == 0) {
		throw XNSQ DOMException(XNSQ DOMException::NAMESPACE_ERR, 0, getNsDocument()->getMemoryManager());
	}
	setNsPrefix(prefix);
}

const XMLCh *
NsXDOMElement::getLocalName() const
{
	return getNsLocalName();
}

bool
NsXDOMElement::hasAttributes() const
{
	return hasNsAttributes();
}

bool
NsXDOMElement::isSameNode(const DOMNode* other) const
{
	if ((const DOMNode *)this == other)
		return true;
	else
		return operator==(*fakeDynamicCastNsDomNodeC(other));
}

bool
NsXDOMElement::isEqualNode(const DOMNode* arg) const
{
	const NsDomNode *thisNode = fakeDynamicCastNsDomNodeC(this);
	return compareNsNodes(thisNode, (const NsDomNode *)
			      (((DOMNode*)arg)->getInterface(_nsDomString)));
}

const XMLCh *
NsXDOMElement::getBaseURI() const
{
	return getNsBaseUri();
}

short
NsXDOMElement::compareTreePosition(const DOMNode* other) const
{
	return _compareNsPositions(this, other);
}

const XMLCh *
NsXDOMElement::getTextContent() const
{
	return getNsTextContent();
}

void
NsXDOMElement::setTextContent(const XMLCh* textContent)
{
	setNsTextContent(textContent);
}

const XMLCh *
NsXDOMElement::lookupNamespacePrefix(const XMLCh* namespaceURI,
				     bool useDefault) const
{

	NsDomElement *thisElem = (NsDomElement*)fakeDynamicCastNsDomNodeC(this);
	return lookupPrefix(thisElem, namespaceURI, useDefault, thisElem);
}

const XMLCh *
NsXDOMElement::lookupNamespaceURI(const XMLCh* prefix) const
{
	const NsDomNode *thisNode = fakeDynamicCastNsDomNodeC(this);
	return lookupUri(thisNode, prefix);
}

// non-standard Xerces extensions
void
NsXDOMElement::release()
{
	//TBD -- no-op for now
}

//
// NsXDOMNamedNodeMap implementation
//
DOMNode *
NsXDOMNamedNodeMap::item(XMLSize_t index) const
{
	NsDomNamedNodeMap *thisMap =
		const_cast<NsDomNamedNodeMap*>(getNsDomNamedNodeMap());
	NsDomAttr *attr = thisMap->getNsItem(index);
	return fakeDynamicCastDOMNode(attr);
}

DOMNode *
NsXDOMNamedNodeMap::getNamedItem(const XMLCh *name) const
{
	NsDomNamedNodeMap *thisMap =
		const_cast<NsDomNamedNodeMap*>(getNsDomNamedNodeMap());
	NsDomAttr *attr = thisMap->getNsNamedItem(name);
	return fakeDynamicCastDOMNode(attr);
}

DOMNode *
NsXDOMNamedNodeMap::setNamedItem(XNSQ DOMNode *arg)
{
	if(arg->getNodeType() != DOMNode::ATTRIBUTE_NODE) {
		throw XNSQ DOMException(XNSQ DOMException::HIERARCHY_REQUEST_ERR, 0, getNsDocument()->getMemoryManager());
	}
	if(arg->getOwnerDocument() !=
	   fakeDynamicCastDOMElement(_owner)->getOwnerDocument()) {
		throw XNSQ DOMException(XNSQ DOMException::WRONG_DOCUMENT_ERR, 0, getNsDocument()->getMemoryManager());
	}
	NsDomAttr *attr = (NsDomAttr*)fakeDynamicCastNsDomNode(arg);
	if(!attr->isStandalone()) {
		throw XNSQ DOMException(XNSQ DOMException::INUSE_ATTRIBUTE_ERR, 0, getNsDocument()->getMemoryManager());
	}

	return fakeDynamicCastDOMNode(setNsNamedItem(attr));
}

DOMNode *
NsXDOMNamedNodeMap::removeNamedItem(const XMLCh *name)
{
	NsDomAttr *attr = removeNsNamedItem(name);
	if(attr == 0) {
		throw XNSQ DOMException(XNSQ DOMException::NOT_FOUND_ERR, 0, getNsDocument()->getMemoryManager());
	}

	return fakeDynamicCastDOMNode(attr);
}

DOMNode *
NsXDOMNamedNodeMap::getNamedItemNS(const XMLCh *namespaceURI,
				   const XMLCh *localName) const
{
	NsDomNamedNodeMap *thisMap =
		const_cast<NsDomNamedNodeMap*>(getNsDomNamedNodeMap());
	NsDomAttr *attr = thisMap->getNsNamedItemNS(namespaceURI, localName);
	return fakeDynamicCastDOMNode(attr);
}

DOMNode *
NsXDOMNamedNodeMap::setNamedItemNS(XNSQ DOMNode *arg)
{
	if(arg->getNodeType() != DOMNode::ATTRIBUTE_NODE) {
		throw XNSQ DOMException(XNSQ DOMException::HIERARCHY_REQUEST_ERR, 0, getNsDocument()->getMemoryManager());
	}
	if(arg->getOwnerDocument() !=
	   fakeDynamicCastDOMElement(_owner)->getOwnerDocument()) {
		throw XNSQ DOMException(XNSQ DOMException::WRONG_DOCUMENT_ERR, 0, getNsDocument()->getMemoryManager());
	}
	NsDomAttr *attr = (NsDomAttr*)fakeDynamicCastNsDomNode(arg);
	if(!attr->isStandalone()) {
		throw XNSQ DOMException(XNSQ DOMException::INUSE_ATTRIBUTE_ERR, 0, getNsDocument()->getMemoryManager());
	}

	return fakeDynamicCastDOMNode(setNsNamedItemNS(attr));
}

DOMNode *
NsXDOMNamedNodeMap::removeNamedItemNS(const XMLCh *namespaceURI,
				      const XMLCh *localName)
{
	NsDomAttr *attr = removeNsNamedItemNS(namespaceURI, localName);
	if(attr == 0) {
		throw XNSQ DOMException(XNSQ DOMException::NOT_FOUND_ERR, 0, getNsDocument()->getMemoryManager());
	}

	return fakeDynamicCastDOMNode(attr);
}

XMLSize_t
NsXDOMNamedNodeMap::getLength() const
{
	return getNsLength();
}

//
// NsXDOMNodeList implementation
//

DOMNode *
NsXDOMNodeList::item(XMLSize_t index) const
{
	return fakeDynamicCastDOMNode(getNsItem(index));
}

XMLSize_t
NsXDOMNodeList::getLength() const
{
	return getNsLength();
}

//
// NsXDOMAttr implementation
//

const XMLCh *
NsXDOMAttr::getName() const
{
	return const_cast<NsXDOMAttr *>(this)->getNsNodeName();
}

bool
NsXDOMAttr::getSpecified() const
{
	return const_cast<NsXDOMAttr *>(this)->getNsSpecified();
}

const XMLCh *
NsXDOMAttr::getValue() const
{
	return getNsNodeValue();
}

void
NsXDOMAttr::setValue(const XMLCh *value)
{
	setNsNodeValue(value);
}

DOMElement *
NsXDOMAttr::getOwnerElement() const
{
	DOMElement *element = 0;
	if(_owner != 0)
		element = (DOMElement *) _owner->getNsInterface("DOMElement");
	return element;
}

const DOMTypeInfo *
NsXDOMAttr::getTypeInfo() const
{
	static const XMLCh g_cdata[] = {'C', 'D', 'A', 'T', 'A', 0 };
	static NsXDOMTypeInfo g_untypedAttribute((const XMLCh*)0, g_cdata);
	return &g_untypedAttribute;
}

bool
NsXDOMAttr::isId() const
{
	// TBD -- do something better
	return false;
}

//
// NsXDOMAttr DOMNode implementation
//
const XMLCh *
NsXDOMAttr::getNodeName() const
{
	return getNsNodeName();
}

const XMLCh *
NsXDOMAttr::getNodeValue() const
{
	return getNsNodeValue();
}

void
NsXDOMAttr::setNodeValue(const XMLCh *nodeValue)
{
	setNsNodeValue(nodeValue);
}

short
NsXDOMAttr::getNodeType() const
{
	return DOMNode::ATTRIBUTE_NODE;
}

DOMNode *
NsXDOMAttr::getParentNode() const
{
	return 0;
}

DOMNodeList *
NsXDOMAttr::getChildNodes() const
{
	return fakeDynamicCastDOMNodeList(getNsDomFactory()->createNsDomNodeList(0));
}

DOMNode *
NsXDOMAttr::getFirstChild() const
{
	// Hack: Xerces' importNode needs a child text node for value
	NsXDOMFactory * fact = (NsXDOMFactory*) getNsDomFactory();
	const xmlch_t *value = getNodeValue();
	NsXDOMFakeText * ret = fact->createNsDomFakeText(getNsDocument(), value,
							 const_cast<NsXDOMAttr*>(this));
	return ret;
}

DOMNode *
NsXDOMAttr::getLastChild() const
{
	return 0;
}

DOMNode *
NsXDOMAttr::getPreviousSibling() const
{
	return 0;
}

DOMNode *
NsXDOMAttr::getNextSibling() const
{
	return 0;
}

DOMNamedNodeMap *
NsXDOMAttr::getAttributes() const
{
	return 0;
}

DOMDocument *
NsXDOMAttr::getOwnerDocument() const
{
	return (NsXDOMDocument *) (const_cast<NsXDOMAttr*>(this))->
		getNsDocumentObj();
}

DOMNode *
NsXDOMAttr::cloneNode(bool deep) const
{
	return getOwnerDocument()->importNode(const_cast<NsXDOMAttr*>(this), deep);
}

DOMNode *
NsXDOMAttr::removeChild(DOMNode *oldChild)
{
	throw XNSQ DOMException(XNSQ DOMException::NOT_FOUND_ERR, 0, getNsDocument()->getMemoryManager());
}

DOMNode *
NsXDOMAttr::appendChild(DOMNode *newChild)
{
	if(newChild->getOwnerDocument() != getOwnerDocument()) {
		throw XNSQ DOMException(XNSQ DOMException::WRONG_DOCUMENT_ERR, 0, getNsDocument()->getMemoryManager());
	}
	throw XNSQ DOMException(XNSQ DOMException::HIERARCHY_REQUEST_ERR, 0, getNsDocument()->getMemoryManager());
}

DOMNode *
NsXDOMAttr::insertBefore(DOMNode *newChild, DOMNode *refChild)
{
	if(newChild->getOwnerDocument() != getOwnerDocument()) {
		throw XNSQ DOMException(XNSQ DOMException::WRONG_DOCUMENT_ERR, 0, getNsDocument()->getMemoryManager());
	}
	throw XNSQ DOMException(XNSQ DOMException::HIERARCHY_REQUEST_ERR, 0, getNsDocument()->getMemoryManager());
}

DOMNode *
NsXDOMAttr::replaceChild(DOMNode *newChild, DOMNode *oldChild)
{
	newChild = insertBefore(newChild, oldChild);
	return removeChild(oldChild);
}

bool
NsXDOMAttr::hasChildNodes() const
{
	return false;
}

const XMLCh *
NsXDOMAttr::getNamespaceURI() const
{
	return getNsUri();
}

const XMLCh *
NsXDOMAttr::getPrefix() const
{
	return getNsPrefix();
}

void NsXDOMAttr::setPrefix(const XMLCh *prefix)
{
	if(getNsUri() == 0) {
		throw XNSQ DOMException(XNSQ DOMException::NAMESPACE_ERR, 0, getNsDocument()->getMemoryManager());
	}
	setNsPrefix(prefix);
}

const XMLCh *
NsXDOMAttr::getLocalName() const
{
	return const_cast<NsXDOMAttr *>(this)->getNsLocalName();
}

bool
NsXDOMAttr::hasAttributes() const
{
	return false;
}

bool
NsXDOMAttr::isSameNode(const DOMNode* other) const
{
	if ((const DOMNode *)this == other)
		return true;
	else
		return operator==(*fakeDynamicCastNsDomNodeC(other));
}

bool
NsXDOMAttr::isEqualNode(const DOMNode* arg) const
{
	NsDomAttr *thisAttr = (NsDomAttr*)
		fakeDynamicCastNsDomNodeC(this);
	return compareNsNodes(thisAttr, (const NsDomNode *)
			      (((DOMNode*)arg)->getInterface(_nsDomString)));
}

const XMLCh *
NsXDOMAttr::getBaseURI() const
{
	return getNsBaseUri();
}

short
NsXDOMAttr::compareTreePosition(const DOMNode* other) const
{
	return _compareNsPositions(this, other);
}

const XMLCh *
NsXDOMAttr::getTextContent() const
{
	return getNsNodeValue();
}

void
NsXDOMAttr::setTextContent(const XMLCh* textContent)
{
	setNsNodeValue(textContent);
}

const XMLCh *
NsXDOMAttr::lookupNamespacePrefix(const XMLCh* namespaceURI,
				  bool useDefault) const
{
	NsDomAttr *thisAttr = (NsDomAttr*)fakeDynamicCastNsDomNodeC(this);
	NsDomElement *owner = thisAttr->getNsOwner();
	return lookupPrefix(owner, namespaceURI, useDefault, owner);
}

const XMLCh *
NsXDOMAttr::lookupNamespaceURI(const XMLCh* prefix) const
{
	return lookupUri(getNsOwner(), prefix);
}

// non-standard Xerces extensions
void
NsXDOMAttr::release()
{
	//TBD -- no-op for now
}

//
// NsXDOMText DOMNode implementation
//
const XMLCh *
NsXDOMText::getNodeName() const
{
	return getNsNodeName();
}

const XMLCh *
NsXDOMText::getNodeValue() const
{
	return getNsNodeValue();
}

void
NsXDOMText::setNodeValue(const XMLCh *nodeValue)
{
	setNsNodeValue(nodeValue);
}

short
NsXDOMText::getNodeType() const
{
	switch (nsTextType(_type)) {
	case NS_TEXT:
		return DOMNode::TEXT_NODE;
	case NS_CDATA:
		return DOMNode::CDATA_SECTION_NODE;
	case NS_ENTSTART:
	case NS_ENTEND:
	case NS_SUBSET:
		// TBD: think about this return -- who
		// needs to handle it -- should these nodes
		// not appear in NsXDOM?  Probably not.
		return DOMNode::TEXT_NODE;
	default:
		DBXML_ASSERT(0);
	}
	return 0;
}

DOMNode *
NsXDOMText::getParentNode() const
{
	NsDomNode *node = const_cast<NsXDOMText *>(this)->getNsParentNode();
	return fakeDynamicCastDOMNode(node);
}

DOMNodeList *
NsXDOMText::getChildNodes() const
{
	return fakeDynamicCastDOMNodeList(getNsDomFactory()->createNsDomNodeList(0));
}

DOMNode *
NsXDOMText::getFirstChild() const
{
	return 0;
}

DOMNode *
NsXDOMText::getLastChild() const
{
	return 0;
}

DOMNode *
NsXDOMText::getPreviousSibling() const
{
	NsDomNode *node = const_cast<NsXDOMText *>(this)->getNsPrevSibling();
	return fakeDynamicCastDOMNode(node);
}

DOMNode *
NsXDOMText::getNextSibling() const
{
	NsDomNode *node = const_cast<NsXDOMText *>(this)->getNsNextSibling();
	return fakeDynamicCastDOMNode(node);
}

DOMNamedNodeMap *
NsXDOMText::getAttributes() const
{
	return 0;
}

DOMDocument *
NsXDOMText::getOwnerDocument() const
{
	return (NsXDOMDocument *) const_cast<NsXDOMText*>(this)->
		getNsDocumentObj();
}

DOMNode *
NsXDOMText::cloneNode(bool deep) const
{
	return getOwnerDocument()->importNode(const_cast<NsXDOMText*>(this), deep);
}

DOMNode *
NsXDOMText::removeChild(DOMNode *oldChild)
{
	throw XNSQ DOMException(XNSQ DOMException::NOT_FOUND_ERR, 0, getNsDocument()->getMemoryManager());
}

DOMNode *
NsXDOMText::appendChild(DOMNode *newChild)
{
	if(newChild->getOwnerDocument() != getOwnerDocument()) {
		throw XNSQ DOMException(XNSQ DOMException::WRONG_DOCUMENT_ERR, 0, getNsDocument()->getMemoryManager());
	}
	throw XNSQ DOMException(XNSQ DOMException::HIERARCHY_REQUEST_ERR, 0, getNsDocument()->getMemoryManager());
}

DOMNode *
NsXDOMText::insertBefore(DOMNode *newChild, DOMNode *refChild)
{
	if(newChild->getOwnerDocument() != getOwnerDocument()) {
		throw XNSQ DOMException(XNSQ DOMException::WRONG_DOCUMENT_ERR, 0, getNsDocument()->getMemoryManager());
	}
	throw XNSQ DOMException(XNSQ DOMException::HIERARCHY_REQUEST_ERR, 0, getNsDocument()->getMemoryManager());
}

DOMNode *
NsXDOMText::replaceChild(DOMNode *newChild, DOMNode *oldChild)
{
	newChild = insertBefore(newChild, oldChild);
	return removeChild(oldChild);
}

bool
NsXDOMText::hasChildNodes() const
{
	return false;
}

const XMLCh *
NsXDOMText::getNamespaceURI() const
{
	return 0;
}

const XMLCh *
NsXDOMText::getPrefix() const
{
	return 0;
}

const XMLCh *
NsXDOMText::getLocalName() const
{
	return const_cast<NsXDOMText *>(this)->getNsLocalName();
}

bool
NsXDOMText::hasAttributes() const
{
	return false;
}

bool
NsXDOMText::isSameNode(const DOMNode* other) const
{
	if ((const DOMNode *)this == other)
		return true;
	else
		return operator==(*fakeDynamicCastNsDomNodeC(other));
}

bool
NsXDOMText::isEqualNode(const DOMNode* arg) const
{
	NsDomText *thisText = (NsDomText*)
		fakeDynamicCastNsDomNodeC(this);
	return compareNsNodes(thisText, (const NsDomNode *)
			      (((DOMNode*)arg)->getInterface(_nsDomString)));
}

const XMLCh *
NsXDOMText::getBaseURI() const
{
	return getNsBaseUri();
}

short
NsXDOMText::compareTreePosition(const DOMNode* other) const
{
	return _compareNsPositions(this, other);
}

const XMLCh *
NsXDOMText::getTextContent() const
{
	return getNsNodeValue();
}

void
NsXDOMText::setTextContent(const XMLCh* textContent)
{
	setNsNodeValue(textContent);
}

const XMLCh *
NsXDOMText::lookupNamespacePrefix(const XMLCh* namespaceURI,
				  bool useDefault) const
{
	return lookupPrefix(_parent, namespaceURI, useDefault, _parent);
}

const XMLCh *
NsXDOMText::lookupNamespaceURI(const XMLCh* prefix) const
{
	return lookupUri(_parent, prefix);
}

// non-standard Xerces extensions
void
NsXDOMText::release()
{
	//TBD -- no-op for now
}

const XMLCh *
NsXDOMText::getData() const
{
	return getNsNodeValue();
}

void
NsXDOMText::setData(const XMLCh *data)
{
	setNsNodeValue(data);
}

bool
NsXDOMText::getIsWhitespaceInElementContent() const
{
	return nsIgnorableWhitespace();
}

bool
NsXDOMText::isIgnorableWhitespace() const
{
	return nsIgnorableWhitespace();
}

//
// NsXDOMComment DOMNode implementation
//
const XMLCh *
NsXDOMComment::getNodeName() const
{
	return getNsNodeName();
}

const XMLCh *
NsXDOMComment::getNodeValue() const
{
	return getNsNodeValue();
}

void
NsXDOMComment::setNodeValue(const XMLCh *nodeValue)
{
	setNsNodeValue(nodeValue);
}

short
NsXDOMComment::getNodeType() const
{
	return DOMNode::COMMENT_NODE;
}

DOMNode *
NsXDOMComment::getParentNode() const
{
	NsDomNode *node = const_cast<NsXDOMComment *>(this)->getNsParentNode();
	return fakeDynamicCastDOMNode(node);
}

DOMNodeList *
NsXDOMComment::getChildNodes() const
{
	return fakeDynamicCastDOMNodeList(getNsDomFactory()->createNsDomNodeList(0));
}

DOMNode *
NsXDOMComment::getFirstChild() const
{
	return 0;
}

DOMNode *
NsXDOMComment::getLastChild() const
{
	return 0;
}

DOMNode *
NsXDOMComment::getPreviousSibling() const
{
	NsDomNode *node = const_cast<NsXDOMComment *>(this)->getNsPrevSibling();
	return fakeDynamicCastDOMNode(node);
}

DOMNode *
NsXDOMComment::getNextSibling() const
{
	NsDomNode *node = const_cast<NsXDOMComment *>(this)->getNsNextSibling();
	return fakeDynamicCastDOMNode(node);
}

DOMNamedNodeMap *
NsXDOMComment::getAttributes() const
{
	return 0;
}

DOMDocument *
NsXDOMComment::getOwnerDocument() const
{
	return (NsXDOMDocument *) const_cast<NsXDOMComment*>(this)->
		getNsDocumentObj();
}

DOMNode *
NsXDOMComment::cloneNode(bool deep) const
{
	return getOwnerDocument()->importNode(const_cast<NsXDOMComment*>(this), deep);
}

DOMNode *
NsXDOMComment::removeChild(DOMNode *oldChild)
{
	throw XNSQ DOMException(XNSQ DOMException::NOT_FOUND_ERR, 0, getNsDocument()->getMemoryManager());
}

DOMNode *
NsXDOMComment::appendChild(DOMNode *newChild)
{
	if(newChild->getOwnerDocument() != getOwnerDocument()) {
		throw XNSQ DOMException(XNSQ DOMException::WRONG_DOCUMENT_ERR, 0, getNsDocument()->getMemoryManager());
	}
	throw XNSQ DOMException(XNSQ DOMException::HIERARCHY_REQUEST_ERR, 0, getNsDocument()->getMemoryManager());
}

DOMNode *
NsXDOMComment::insertBefore(DOMNode *newChild, DOMNode *refChild)
{
	if(newChild->getOwnerDocument() != getOwnerDocument()) {
		throw XNSQ DOMException(XNSQ DOMException::WRONG_DOCUMENT_ERR, 0, getNsDocument()->getMemoryManager());
	}
	throw XNSQ DOMException(XNSQ DOMException::HIERARCHY_REQUEST_ERR, 0, getNsDocument()->getMemoryManager());
}

DOMNode *
NsXDOMComment::replaceChild(DOMNode *newChild, DOMNode *oldChild)
{
	newChild = insertBefore(newChild, oldChild);
	return removeChild(oldChild);
}

bool
NsXDOMComment::hasChildNodes() const
{
	return false;
}

const XMLCh *
NsXDOMComment::getNamespaceURI() const
{
	return 0;
}

const XMLCh *
NsXDOMComment::getPrefix() const
{
	return 0;
}

const XMLCh *
NsXDOMComment::getLocalName() const
{
	return const_cast<NsXDOMComment *>(this)->getNsLocalName();
}

bool
NsXDOMComment::hasAttributes() const
{
	return false;
}

bool
NsXDOMComment::isSameNode(const DOMNode* other) const
{
	if ((const DOMNode *)this == other)
		return true;
	else
		return operator==(*fakeDynamicCastNsDomNodeC(other));
}

bool
NsXDOMComment::isEqualNode(const DOMNode* arg) const
{
	NsDomText *thisText = (NsDomText*)
		fakeDynamicCastNsDomNodeC(this);
	return compareNsNodes(thisText, (const NsDomNode *)
			      (((DOMNode*)arg)->getInterface(_nsDomString)));
}

const XMLCh *
NsXDOMComment::getBaseURI() const
{
	return getNsBaseUri();
}

short
NsXDOMComment::compareTreePosition(const DOMNode* other) const
{
	return _compareNsPositions(this, other);
}

const XMLCh *
NsXDOMComment::getTextContent() const
{
	return getNsNodeValue();
}

void
NsXDOMComment::setTextContent(const XMLCh* textContent)
{
	setNsNodeValue(textContent);
}

const XMLCh *
NsXDOMComment::lookupNamespacePrefix(const XMLCh* namespaceURI,
				     bool useDefault) const
{
	return lookupPrefix(_parent, namespaceURI, useDefault, _parent);
}

const XMLCh *
NsXDOMComment::lookupNamespaceURI(const XMLCh* prefix) const
{
	return lookupUri(_parent, prefix);
}

// non-standard Xerces extensions
void
NsXDOMComment::release()
{
	//TBD -- no-op for now
}

const XMLCh *
NsXDOMComment::getData() const
{
	return getNsNodeValue();
}

void
NsXDOMComment::setData(const XMLCh *data)
{
	setNsNodeValue(data);
}

//
// NsXDOMProcessingInstruction DOMNode implementation
//
const XMLCh *
NsXDOMProcessingInstruction::getNodeName() const
{
	return getNsNodeName();
}

const XMLCh *
NsXDOMProcessingInstruction::getNodeValue() const
{
	return getNsNodeValue();
}

void
NsXDOMProcessingInstruction::setNodeValue(const XMLCh *nodeValue)
{
	setNsNodeValue(nodeValue);
}

short
NsXDOMProcessingInstruction::getNodeType() const
{
	return DOMNode::PROCESSING_INSTRUCTION_NODE;
}

DOMNode *
NsXDOMProcessingInstruction::getParentNode() const
{
	NsDomNode *node = const_cast<NsXDOMProcessingInstruction *>(this)->
		getNsParentNode();
	return fakeDynamicCastDOMNode(node);
}

DOMNodeList *
NsXDOMProcessingInstruction::getChildNodes() const
{
	return fakeDynamicCastDOMNodeList(getNsDomFactory()->createNsDomNodeList(0));
}

DOMNode *
NsXDOMProcessingInstruction::getFirstChild() const
{
	return 0;
}

DOMNode *
NsXDOMProcessingInstruction::getLastChild() const
{
	return 0;
}

DOMNode *
NsXDOMProcessingInstruction::getPreviousSibling() const
{
	NsDomNode *node = const_cast<NsXDOMProcessingInstruction *>(this)->
		getNsPrevSibling();
	return fakeDynamicCastDOMNode(node);
}

DOMNode *
NsXDOMProcessingInstruction::getNextSibling() const
{
	NsDomNode *node = const_cast<NsXDOMProcessingInstruction *>(this)->
		getNsNextSibling();
	return fakeDynamicCastDOMNode(node);
}

DOMNamedNodeMap *
NsXDOMProcessingInstruction::getAttributes() const
{
	return 0;
}

DOMDocument *
NsXDOMProcessingInstruction::getOwnerDocument() const
{
	return (NsXDOMDocument *) const_cast<NsXDOMProcessingInstruction*>(this)->
		getNsDocumentObj();
}

DOMNode *
NsXDOMProcessingInstruction::cloneNode(bool deep) const
{
	return getOwnerDocument()->importNode(const_cast<NsXDOMProcessingInstruction*>(this), deep);
}

DOMNode *
NsXDOMProcessingInstruction::removeChild(DOMNode *oldChild)
{
	throw XNSQ DOMException(XNSQ DOMException::NOT_FOUND_ERR, 0, getNsDocument()->getMemoryManager());
}

DOMNode *
NsXDOMProcessingInstruction::appendChild(DOMNode *newChild)
{
	if(newChild->getOwnerDocument() != getOwnerDocument()) {
		throw XNSQ DOMException(XNSQ DOMException::WRONG_DOCUMENT_ERR, 0, getNsDocument()->getMemoryManager());
	}
	throw XNSQ DOMException(XNSQ DOMException::HIERARCHY_REQUEST_ERR, 0, getNsDocument()->getMemoryManager());
}

DOMNode *
NsXDOMProcessingInstruction::insertBefore(DOMNode *newChild, DOMNode *refChild)
{
	if(newChild->getOwnerDocument() != getOwnerDocument()) {
		throw XNSQ DOMException(XNSQ DOMException::WRONG_DOCUMENT_ERR, 0, getNsDocument()->getMemoryManager());
	}
	throw XNSQ DOMException(XNSQ DOMException::HIERARCHY_REQUEST_ERR, 0, getNsDocument()->getMemoryManager());
}

DOMNode *
NsXDOMProcessingInstruction::replaceChild(DOMNode *newChild, DOMNode *oldChild)
{
	newChild = insertBefore(newChild, oldChild);
	return removeChild(oldChild);
}

bool
NsXDOMProcessingInstruction::hasChildNodes() const
{
	return false;
}

const XMLCh *
NsXDOMProcessingInstruction::getNamespaceURI() const
{
	return 0;
}

const XMLCh *
NsXDOMProcessingInstruction::getPrefix() const
{
	return 0;
}

const XMLCh *
NsXDOMProcessingInstruction::getLocalName() const
{
	return const_cast<NsXDOMProcessingInstruction *>(this)->
		getNsLocalName();
}

bool
NsXDOMProcessingInstruction::hasAttributes() const
{
	return false;
}

bool
NsXDOMProcessingInstruction::isSameNode(const DOMNode* other) const
{
	if ((const DOMNode *)this == other)
		return true;
	else
		return operator==(*fakeDynamicCastNsDomNodeC(other));
}

bool
NsXDOMProcessingInstruction::isEqualNode(const DOMNode* arg) const
{
	NsDomText *thisText = (NsDomText *)fakeDynamicCastNsDomNodeC(this);
	return compareNsNodes(thisText, (const NsDomNode *)
			      (((DOMNode*)arg)->getInterface(_nsDomString)));
}

const XMLCh *
NsXDOMProcessingInstruction::getBaseURI() const
{
	return getNsBaseUri();
}

short
NsXDOMProcessingInstruction::compareTreePosition(const DOMNode* other) const
{
	return _compareNsPositions(this, other);
}

const XMLCh *
NsXDOMProcessingInstruction::getTextContent() const
{
	return getNsNodeValue();
}

void
NsXDOMProcessingInstruction::setTextContent(const XMLCh* textContent)
{
	setNsNodeValue(textContent);
}

const XMLCh *
NsXDOMProcessingInstruction::lookupNamespacePrefix(const XMLCh* namespaceURI,
				  bool useDefault) const
{
	return lookupPrefix(_parent, namespaceURI, useDefault, _parent);
}

const XMLCh *
NsXDOMProcessingInstruction::lookupNamespaceURI(const XMLCh* prefix) const
{
	return lookupUri(_parent, prefix);
}

// non-standard Xerces extensions
void
NsXDOMProcessingInstruction::release()
{
	//TBD -- no-op for now
}

const XMLCh *
NsXDOMProcessingInstruction::getData() const
{
	return getNsNodeValue();
}

void
NsXDOMProcessingInstruction::setData(const XMLCh *data)
{
	setNsNodeValue(data);
}

const XMLCh *NsXDOMProcessingInstruction::getTarget() const
{
	return getNodeName();
}
