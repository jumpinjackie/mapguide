//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: DbXmlNodeImpl.cpp,v 1.57.2.1 2007/01/19 14:56:01 jsnelson Exp $
//

#include <algorithm>

#include "DbXmlNodeImpl.hpp"
#include "DbXmlNodeTest.hpp"
#include "DbXmlFactoryImpl.hpp"
#include "DbXmlStep.hpp"
#include "DbXmlContext.hpp"
#include "../nodeStore/NsXercesDom.hpp"
#include "../nodeStore/NsDocument.hpp"
#include "../nodeStore/NsNid.hpp"
#include "../UTF8.hpp"
#include "../Value.hpp"
#include "../Document.hpp"
#include "../TransactedContainer.hpp"

#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/context/ItemFactory.hpp>
#include <xqilla/items/ATAnyURIOrDerived.hpp>
#include <xqilla/items/ATStringOrDerived.hpp>
#include <xqilla/items/ATQNameOrDerived.hpp>
#include <xqilla/items/ATUntypedAtomic.hpp>
#include <xqilla/exceptions/ItemException.hpp>
#include <xqilla/utils/XPath2Utils.hpp>
#include <xqilla/schema/DocumentCacheImpl.hpp>
#include <xqilla/functions/FunctionConstructor.hpp>
#include <xqilla/exceptions/XPath2TypeCastException.hpp>
#include <xqilla/exceptions/XQillaException.hpp>
#include <xqilla/functions/FunctionRoot.hpp>
#include <xqilla/utils/XPath2NSUtils.hpp>

#include <xqilla/axis/AncestorAxis.hpp>
#include <xqilla/axis/AncestorOrSelfAxis.hpp>
#include <xqilla/axis/FollowingAxis.hpp>
#include <xqilla/axis/FollowingSiblingAxis.hpp>
#include <xqilla/axis/PrecedingAxis.hpp>
#include <xqilla/axis/PrecedingSiblingAxis.hpp>
#include <xqilla/axis/SelfAxis.hpp>

#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMWriter.hpp>
#include <xercesc/dom/DOMXPathNamespace.hpp>
#include <xercesc/util/XMLUniDefs.hpp>
#include <cassert>

#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

using namespace DbXml;
using namespace std;

#define MATERIALISE_NODE \
  if(node_ == 0) { \
    if(ie_) { \
      const_cast<DbXmlNodeImpl*>(this)->node_ = ie_->fetchNode(getXmlDocument()); \
    } \
    else { \
      const_cast<DbXmlNodeImpl*>(this)->node_ = getXmlDocument().getContentAsDOM(); \
    } \
  }

#define MATERIALISE_DOCUMENT \
  if(document_.isNull()) { \
    DBXML_ASSERT(ie_ && container_ != 0 && qc_ != 0); \
    ie_->getDocID().fetchDocument(container_, qc_, \
      const_cast<DbXmlNodeImpl*>(this)->document_, qc_->getMinder()); \
    ((Document&)document_).setDocumentURI(documentUri_); \
  }

const XMLCh DbXmlNodeImpl::gDbXml[] =
{
	chLatin_D, chLatin_b,
	chLatin_X, chLatin_m,
	chLatin_l,
	chNull
};

DbXmlNodeImpl::DbXmlNodeImpl(Document *doc, const DynamicContext *context, const DbXmlFactoryImpl *factory)
	: ie_(0),
	  container_(0),
	  qc_(0),
	  documentUri_(0),
	  document_(doc),
	  node_(0),
	  factory_(factory),
	  poolNext(0),
	  allocNext(0)
{
	DBXML_ASSERT(doc != 0);
}

DbXmlNodeImpl::DbXmlNodeImpl(const DOMNode *node, Document *doc, const DynamicContext *context,
	const DbXmlFactoryImpl *factory)
	: ie_(0),
	  container_(0),
	  qc_(0),
	  documentUri_(0),
	  document_(doc),
	  node_(node),
	  factory_(factory),
	  poolNext(0),
	  allocNext(0)
{
	DBXML_ASSERT(doc != 0);
}

DbXmlNodeImpl::DbXmlNodeImpl(const IndexEntry::Ptr &ie, Document *doc, const DynamicContext *context,
	const DbXmlFactoryImpl *factory)
	: ie_(ie),
	  container_(0),
	  qc_(0),
	  documentUri_(0),
	  document_(doc),
	  node_(0),
	  factory_(factory),
	  poolNext(0),
	  allocNext(0)
{
	DBXML_ASSERT(doc != 0);
}

DbXmlNodeImpl::DbXmlNodeImpl(const IndexEntry::Ptr &ie, const Container *container, const XMLCh *documentUri,
	const DynamicContext *context, const DbXmlFactoryImpl *factory)
	: ie_(ie),
	  container_(container),
	  qc_(&CAST_TO_CONST_DBXMLCONTEXT(context)->getQueryContext()),
	  documentUri_(documentUri),
	  document_(0),
	  node_(0),
	  factory_(factory),
	  poolNext(0),
	  allocNext(0)
{
	DBXML_ASSERT(container != 0);
}

DbXmlNamespaceNodeImpl::DbXmlNamespaceNodeImpl(const XMLCh *prefix, const XMLCh *uri, NsDomElement *parent, Document *doc, const DynamicContext *context)
	: DbXmlNodeImpl(doc, context, 0),
	  prefix_(context->getMemoryManager()->getPooledString(prefix)),
	  uri_(context->getMemoryManager()->getPooledString(uri)),
	  parent_(parent)
{
}

DbXmlNodeImpl::~DbXmlNodeImpl()
{
}

void DbXmlNodeImpl::decrementRefCount() const
{
	if(--const_cast<unsigned int&>(_ref_count) == 0) {
		if(factory_) {
			const_cast<DbXmlNodeImpl*>(this)->ie_.reset(0);
			const_cast<DbXmlNodeImpl*>(this)->container_ = 0;
			const_cast<DbXmlNodeImpl*>(this)->qc_ = 0;
			const_cast<DbXmlNodeImpl*>(this)->documentUri_ = 0;
			const_cast<DbXmlNodeImpl*>(this)->document_ = 0;
			const_cast<DbXmlNodeImpl*>(this)->node_ = 0;
			factory_->addToNodePool(const_cast<DbXmlNodeImpl*>(this));
		} else {
			delete this;
		}
	}
}

void DbXmlNodeImpl::init(Document *doc)
{
	DBXML_ASSERT(doc != 0);

	ie_ = 0;
	container_ = 0;
	qc_ = 0;
	documentUri_ = 0;
	document_ = doc;
	node_ = 0;
}

void DbXmlNodeImpl::init(const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node, Document *doc)
{
	DBXML_ASSERT(doc != 0);

	ie_ = 0;
	container_ = 0;
	qc_ = 0;
	documentUri_ = 0;
	document_ = doc;
	node_ = node;
}

void DbXmlNodeImpl::init(const IndexEntry::Ptr &ie, Document *doc)
{
	DBXML_ASSERT(doc != 0);

	ie_ = ie;
	container_ = 0;
	qc_ = 0;
	documentUri_ = 0;
	document_ = doc;
	node_ = 0;
}

void DbXmlNodeImpl::init(const IndexEntry::Ptr &ie, const Container *container, const XMLCh *documentUri,
	const DynamicContext *context)
{
	DBXML_ASSERT(container != 0);

	ie_ = ie;
	container_ = container;
	qc_ = &CAST_TO_CONST_DBXMLCONTEXT(context)->getQueryContext();
	documentUri_ = documentUri;
	document_ = 0;
	node_ = 0;
}

const XmlDocument &DbXmlNodeImpl::getXmlDocument() const
{
	MATERIALISE_DOCUMENT;

	return document_;
}

const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *DbXmlNodeImpl::getDOMNode() const
{
	MATERIALISE_NODE;

	return node_;
}

const Node::Ptr DbXmlNodeImpl::createNode(const DOMNode *node, const DynamicContext *context) const
{
	MATERIALISE_DOCUMENT;

	return ((DbXmlFactoryImpl*)context->getItemFactory())->createNode(node, document_, context);
}

DbXmlNodeImpl::Ptr DbXmlNodeImpl::getDescendantElement(const IndexEntry::Ptr &ie, const DynamicContext *context) const
{
	MATERIALISE_NODE;
	if(node_ == 0) return 0; // No content documents

	const NsDomNode *nsnode = fakeDynamicCastNsDomNodeC(node_);

	switch(nsnode->getNsNodeType()) {
	case nsNodeDocument:
	case nsNodeElement: {
		NsDomElement *resultElement = ((NsDomElement*)nsnode)->lookupDescendantElement(&ie->getNodeID());
		if(resultElement == 0) return 0;
		DbXmlNodeImpl::Ptr result = ((DbXmlFactoryImpl*)context->getItemFactory())->
			createNode(fakeDynamicCastDOMNode(resultElement), document_, context);
		((DbXmlNodeImpl*)result.get())->setIndexEntry(ie);
		return result;
	}
	default: break;
	}

	return 0;
}

void *DbXmlNodeImpl::getInterface(const XMLCh *name) const
{
	if(name == gDbXml) {
		return (void*)this;
	}
	return 0;
}

Item::Ptr DbXmlNodeImpl::getMetaData(const XMLCh *uri, const XMLCh *name, DynamicContext *context) const
{
	MATERIALISE_DOCUMENT;

	XmlValue value;
	if(document_.getMetaData(XMLChToUTF8(uri).str(), XMLChToUTF8(name).str(), value)) {
		return Value::convertToItem(value, context);
	}
	return 0;
}

#define CONTAINER_AND_DOCID_CHECK(me, other) { \
  TransactedContainer *myContainer; \
  DocID myDocID; \
  if((me)->document_.isNull()) { \
	  myContainer = (TransactedContainer*)(me)->container_; \
	  myDocID = (me)->ie_->getDocID(); \
  } else { \
	  myContainer = ((Document&)(me)->document_).getContainer(); \
	  myDocID = ((Document&)(me)->document_).getID(); \
  } \
\
  TransactedContainer *otherContainer; \
  DocID otherDocID; \
  if((other)->document_.isNull()) { \
	  otherContainer = (TransactedContainer*)(other)->container_; \
	  otherDocID = (other)->ie_->getDocID(); \
  } else { \
	  otherContainer = ((Document&)(other)->document_).getContainer(); \
	  otherDocID = ((Document&)(other)->document_).getID(); \
  } \
\
  if(myContainer < otherContainer) return -1; \
  if(myContainer > otherContainer) return +1; \
  if(myDocID < otherDocID) return -1; \
  if(myDocID > otherDocID) return +1; \
}

int DbXmlNodeImpl::isSameNID(const DbXmlNodeImpl *other) const
{
	CONTAINER_AND_DOCID_CHECK(this, other);

	// Compare starting NID
	return getNID()->compareNids(other->getNID());
}

int DbXmlNodeImpl::isDescendantOf(const DbXmlNodeImpl *parent, bool orSelf) const
{
	CONTAINER_AND_DOCID_CHECK(this, parent);

	// Every node is a decendant of the document node
	if(getNodeType() == DOMNode::DOCUMENT_NODE) {
		return (orSelf && parent->getNodeType() == DOMNode::DOCUMENT_NODE) ? 0 : -1;
	}
	if(parent->getNodeType() == DOMNode::DOCUMENT_NODE) {
		return 0;
	}

	const NsNid *thisNid = getNID();

	// Compare starting NID
	int cmp = thisNid->compareNids(parent->getNID());
	if(cmp < 0) return -1;
	if(cmp == 0) return (orSelf || parent->isChildText()) ? 0 : -1;

	const NsNid *ld = parent->getLastElemDescendantNID();
	if(ld == 0) return +1;

	// Compare ending NID
	return (thisNid->compareNids(ld) <= 0) ? 0 : +1;
}

u_int32_t DbXmlNodeImpl::getLevel() const
{
	if(node_ == 0) {
		if(!ie_ || !ie_->isSpecified(IndexEntry::NODE_ID))
			return 0;
		if(ie_->isSpecified(IndexEntry::NODE_LEVEL))
			return ie_->getNodeLevel();
	}

	MATERIALISE_NODE;

	return fakeDynamicCastNsDomNodeC(node_)->getNsLevel();
}

TransactedContainer *DbXmlNodeImpl::getContainer() const
{
	if(document_.isNull()) return (TransactedContainer*)container_;
	return ((Document&)document_).getContainer();
}

DocID DbXmlNodeImpl::getDocID() const
{
	if(document_.isNull()) return ie_->getDocID();
	return ((Document&)document_).getID();
}

const NsNid *DbXmlNodeImpl::getNID() const
{
	if(node_ == 0) {
		if(ie_ && ie_->isSpecified(IndexEntry::NODE_ID))
			return &ie_->getNodeID();
		else return NsNid::getRootNid();
	}

	return fakeDynamicCastNsDomNodeC(node_)->getNodeId();
}

int DbXmlNodeImpl::getIndex() const
{
	if(node_ == 0) {
		if(ie_ && (ie_->isSpecified(IndexEntry::ATTRIBUTE_INDEX) ||
			   ie_->isSpecified(IndexEntry::TEXT_INDEX) ||
			   ie_->isSpecified(IndexEntry::COMMENT_INDEX) ||
			   ie_->isSpecified(IndexEntry::PI_INDEX)))
			return ie_->getIndex();
		return -1;
	}

	return fakeDynamicCastNsDomNodeC(node_)->getIndex();
}

bool DbXmlNodeImpl::isLeadingText() const
{
	if(node_ == 0) return false;

	const NsDomNode *nsdom = fakeDynamicCastNsDomNodeC(node_);

	return nsdom->getIndex() != -1 &&
		nsdom->getIndex() < nsdom->getNsNode()->getNumLeadingText();
}

bool DbXmlNodeImpl::isChildText() const
{
	if(node_ == 0) return false;

	const NsDomNode *nsdom = fakeDynamicCastNsDomNodeC(node_);

	return nsdom->getIndex() != -1 &&
		nsdom->getIndex() >= nsdom->getNsNode()->getNumLeadingText();
}

const NsNid *DbXmlNodeImpl::getLastElemDescendantNID() const
{
	if(ie_ && ie_->isSpecified(IndexEntry::LAST_DESCENDANT_ID))
		return &ie_->getLastDescendant();

	MATERIALISE_NODE;
	if(node_ == 0) return NsNid::getRootNid(); // No content documents

	return fakeDynamicCastNsDomNodeC(node_)->getLastDescendantNid();
}

DbXmlNodeImpl::Ptr DbXmlNodeImpl::getAttribute(u_int32_t index, const DynamicContext *context) const
{
	if(node_ == 0) {
		if(ie_) const_cast<DbXmlNodeImpl*>(this)->node_ = ie_->fetchNode(getXmlDocument());
		else return 0;
	}

	DOMNode *attr = node_->getAttributes()->item(index);
	DBXML_ASSERT(attr);

	return ((DbXmlFactoryImpl*)context->getItemFactory())->createNode(attr, document_, context);
}

string DbXmlNodeImpl::getNodeHandle() const
{
	IndexEntry ie;
	ie.setDocID(getDocID());

	short type = getNodeType();
	if(type != DOMNode::DOCUMENT_NODE) {
		ie.setNodeID(getNID());

		if(type == DOMNode::ELEMENT_NODE) {
			ie.setFormat(IndexEntry::NH_ELEMENT_FORMAT);
		} else if(type == DOMNode::ATTRIBUTE_NODE) {
			ie.setFormat(IndexEntry::NH_ATTRIBUTE_FORMAT);
			ie.setIndex(getIndex());
		} else if(type == DOMNode::TEXT_NODE ||
			type == DOMNode::CDATA_SECTION_NODE) {
			ie.setFormat(IndexEntry::NH_TEXT_FORMAT);
			ie.setIndex(getIndex());
		} else if(type == DOMNode::COMMENT_NODE) {
			ie.setFormat(IndexEntry::NH_COMMENT_FORMAT);
			ie.setIndex(getIndex());
		} else if(type == DOMNode::PROCESSING_INSTRUCTION_NODE) {
			ie.setFormat(IndexEntry::NH_PI_FORMAT);
			ie.setIndex(getIndex());
		} else {
			throw XmlException(XmlException::INVALID_VALUE,
				"Node handle unavailable for node type");
		}
	} else {
		ie.setFormat(IndexEntry::NH_DOCUMENT_FORMAT);
	}

	return ie.getNodeHandle();
}

const XMLCh* DbXmlNodeImpl::asString(const DynamicContext* context) const
{
	MATERIALISE_NODE;
	if(node_ == 0) return XMLUni::fgZeroLenString; // No content documents

	DOMWriter *writer =
		DOMImplementation::getImplementation()->
		createDOMWriter(context->getMemoryManager());
	const XMLCh *result = writer->writeToString(*node_);
	writer->release();
	return result;
}

Sequence DbXmlNodeImpl::dmBaseURI(const DynamicContext* context) const
{
	MATERIALISE_NODE;
	if(node_ == 0) return Sequence(context->getMemoryManager()); // No content documents

	const XMLCh *baseURI = node_->getBaseURI();
	if((baseURI == 0 || *baseURI == 0) &&
		(node_->getNodeType() == DOMNode::ELEMENT_NODE ||
			node_->getNodeType() == DOMNode::DOCUMENT_NODE) &&
		XPath2NSUtils::getParent(node_) == 0) {
		baseURI = context->getBaseURI();
	}

	if(baseURI == 0 || *baseURI == 0) {
		return Sequence(context->getMemoryManager());
	}

	return Sequence(context->getItemFactory()->createAnyURI(baseURI, context), context->getMemoryManager());
}

Sequence DbXmlNamespaceNodeImpl::dmBaseURI(const DynamicContext* context) const
{
	return Sequence(context->getMemoryManager());
}

Sequence DbXmlNodeImpl::dmDocumentURI(const DynamicContext* context) const
{	
	if(getNodeType() != DOMNode::DOCUMENT_NODE)
		return Sequence(context->getMemoryManager());

	MATERIALISE_DOCUMENT;

	const XMLCh *documentURI = ((Document*)document_)->getDocumentURI();
	if(documentURI == 0) return Sequence(context->getMemoryManager());

	return Sequence(context->getItemFactory()->createAnyURI(documentURI, context), context->getMemoryManager());
}

Sequence DbXmlNamespaceNodeImpl::dmDocumentURI(const DynamicContext* context) const
{	
	return Sequence(context->getMemoryManager());
}

short DbXmlNodeImpl::getNodeType() const
{
	if(node_ == 0) {
		if(ie_) {
			if(ie_->isSpecified(IndexEntry::ATTRIBUTE_INDEX)) {
				return DOMNode::ATTRIBUTE_NODE;
			} else if(ie_->isSpecified(IndexEntry::TEXT_INDEX)) {
				return DOMNode::TEXT_NODE;
			} else if(ie_->isSpecified(IndexEntry::COMMENT_INDEX)) {
				return DOMNode::COMMENT_NODE;
			} else if(ie_->isSpecified(IndexEntry::PI_INDEX)) {
				return DOMNode::PROCESSING_INSTRUCTION_NODE;
			} else if(ie_->isSpecified(IndexEntry::NODE_ID)) {
				return DOMNode::ELEMENT_NODE;
			}
		}

		return DOMNode::DOCUMENT_NODE;
	}
	return node_->getNodeType();
}

const XMLCh* DbXmlNodeImpl::dmNodeKind() const
{  
	switch(getNodeType()) {
	case DOMNode::DOCUMENT_NODE:
		return document_string;
	case DOMNode::ELEMENT_NODE:
		return element_string;
	case DOMNode::ATTRIBUTE_NODE:
		return attribute_string;
	case DOMNode::CDATA_SECTION_NODE:
	case DOMNode::TEXT_NODE:
		return text_string;
	case DOMNode::PROCESSING_INSTRUCTION_NODE:
		return processing_instruction_string;
	case DOMNode::COMMENT_NODE:
		return comment_string;
	}
    
	XQThrow2(ItemException, X("DbXmlNodeImpl::dmNodeKind"), X("Unknown node type."));
}

ATQNameOrDerived::Ptr DbXmlNodeImpl::dmTypeName(const DynamicContext* context) const
{
	// We don't currently store type information
	switch(getNodeType()) {
		case DOMNode::ELEMENT_NODE: {
			return context->getItemFactory()->createQName(FunctionConstructor::XMLChXPath2DatatypesURI,
				XMLUni::fgZeroLenString, DocumentCacheParser::g_szUntyped, context);
		}
		case DOMNode::TEXT_NODE:
		case DOMNode::CDATA_SECTION_NODE:
		case DOMNode::ATTRIBUTE_NODE: {
			return context->getItemFactory()->createQName(FunctionConstructor::XMLChXPath2DatatypesURI,
				XMLUni::fgZeroLenString, ATUntypedAtomic::fgDT_UNTYPEDATOMIC, context);
		}
		default: {
			return 0;
		}
	}
}

ATBooleanOrDerived::Ptr DbXmlNodeImpl::dmNilled(const DynamicContext* context) const
{
	// We don't currently store type information
	if(getNodeType() != DOMNode::ELEMENT_NODE) {
		return 0;
	}
	return context->getItemFactory()->createBoolean(false, context);
}

ATBooleanOrDerived::Ptr DbXmlNodeImpl::dmIsId(const DynamicContext* context) const
{
	// We don't currently store type information
	return context->getItemFactory()->createBoolean(false, context);
}

ATBooleanOrDerived::Ptr DbXmlNodeImpl::dmIsIdRefs(const DynamicContext* context) const
{
	// We don't currently store type information
	return context->getItemFactory()->createBoolean(false, context);
}

bool DbXmlNodeImpl::hasInstanceOfType(const XMLCh* typeURI, const XMLCh* typeName, const DynamicContext* context) const
{
	return context->isTypeOrDerivedFromType(getTypeURI(), getTypeName(), typeURI, typeName);
}

const XMLCh* DbXmlNodeImpl::getTypeName() const
{
	// We don't currently store type information
	switch(getNodeType()) {
		case DOMNode::ELEMENT_NODE: {
			return DocumentCacheParser::g_szUntyped;
		}
		case DOMNode::TEXT_NODE:
		case DOMNode::CDATA_SECTION_NODE:
		case DOMNode::ATTRIBUTE_NODE: {
			return ATUntypedAtomic::fgDT_UNTYPEDATOMIC;
		}
		default: {
			return 0;
		}
	}
}

const XMLCh* DbXmlNodeImpl::getTypeURI() const
{
	// We don't currently store type information
	switch(getNodeType()) {
		case DOMNode::TEXT_NODE:
		case DOMNode::CDATA_SECTION_NODE:
		case DOMNode::ATTRIBUTE_NODE:
		case DOMNode::ELEMENT_NODE: {
			return FunctionConstructor::XMLChXPath2DatatypesURI;
		}
		default: {
			return 0;
		}
	}
}

static int compareElementAndText(int ret, const DbXmlNodeImpl *element, const DbXmlNodeImpl *text)
{
	if(ret == 0) {
		return element->getLevel() == text->getLevel() ? +1 : -1;
	}

	// Element is before text's owner
	if(ret < 0) return ret;

	// Text is leading text, and element is after text's owner
	if(text->isLeadingText()) {
		return +1;
	}

	const NsNid *ld = text->getLastElemDescendantNID();
	if(ld != 0) ret = element->getNID()->compareNids(ld);

	// Element is after text's owner's subtree
	if(ret > 0) return ret;

	// Element is in text's owner's subtree, so element must be smaller
	// than text
	return -1;
}

int DbXmlNodeImpl::compare(const DbXmlNodeImpl *other) const
{
	CONTAINER_AND_DOCID_CHECK(this, other);

	// Now we know it's in the same document
	short thisType = getNodeType();
	short otherType = other->getNodeType();

	// The document root is always less than any other nodes in the tree
	if(thisType == DOMNode::DOCUMENT_NODE) {
		return otherType == DOMNode::DOCUMENT_NODE ? 0 : -1;
	}
	if(otherType == DOMNode::DOCUMENT_NODE) {
		return +1;
	}

	// compare node ids
	const NsNid *thisNid = getNID();
	const NsNid *otherNid = other->getNID();

	// Are the nodes stand alone constructed ones?
	if(thisNid == 0 || thisNid->isNull()) {
		return (otherNid == 0 || otherNid->isNull()) ? getDOMNode() - other->getDOMNode() : +1;
	}
	if(otherNid == 0 || otherNid->isNull()) {
		return -1;
	}

	int ret = thisNid->compareNids(otherNid);

	switch(thisType) {
	case DOMNode::ELEMENT_NODE: {
		switch(otherType) {
		case DOMNode::ELEMENT_NODE: {
			return ret;
		}
		case DOMNode::ATTRIBUTE_NODE: {
			return ret == 0 ? -1 : ret;
		}
		default: {
			return compareElementAndText(ret, this, other);
		}
		}
		break;
	}
	case DOMNode::ATTRIBUTE_NODE: {
		switch(otherType) {
		case DOMNode::ELEMENT_NODE: {
			return ret == 0 ? +1 : ret;
		}
		case DOMNode::ATTRIBUTE_NODE: {
			return ret == 0 ? getIndex() - other->getIndex() : ret;
		}
		default: {
			return compareElementAndText(ret, this, other);
		}
		}
		break;
	}
	default: {
		switch(otherType) {
		case DOMNode::ATTRIBUTE_NODE:
		case DOMNode::ELEMENT_NODE: {
			return -compareElementAndText(-ret, other, this);
		}
		default: {
			if(ret == 0) {
				return getIndex() - other->getIndex();
			}

			if(ret > 0) {
				// Other's owner is before my owner
				if(other->isLeadingText()) return ret;

				// Am I in other's owner's subtree?
				const NsNid *ld = other->getLastElemDescendantNID();
				if(ld == 0) return ret;

				ret = thisNid->compareNids(ld);
				return ret == 0 ? -1 : ret;
			} else {
				// My owner is before other's owner
				if(isLeadingText()) return ret;

				// Is other in my owner's subtree?
				const NsNid *ld = getLastElemDescendantNID();
				if(ld == 0) return ret;

				ret = ld->compareNids(otherNid);
				return ret == 0 ? +1 : ret;
			}
		}
		}
		break;
	}
	}

	return 0;
}

bool DbXmlNodeImpl::lessThan(const Node::Ptr &other, const DynamicContext *context) const
{
	return compare((const DbXmlNodeImpl*)other->getInterface(DbXmlNodeImpl::gDbXml)) < 0;
}

bool DbXmlNodeImpl::equals(const Node::Ptr &other) const
{
	return compare((const DbXmlNodeImpl*)other->getInterface(DbXmlNodeImpl::gDbXml)) == 0;
}

bool DbXmlNodeImpl::uniqueLessThan(const Node::Ptr &other, const DynamicContext *context) const
{
	return compare((const DbXmlNodeImpl*)other->getInterface(DbXmlNodeImpl::gDbXml)) < 0;
}

ATQNameOrDerived::Ptr DbXmlNodeImpl::dmNodeName(const DynamicContext* context) const
{  
	switch(getNodeType())
	{
	case DOMNode::ATTRIBUTE_NODE:
	case DOMNode::ELEMENT_NODE:
		MATERIALISE_NODE;
		return context->getItemFactory()->createQName(node_->getNamespaceURI(),
			node_->getPrefix(), node_->getLocalName(), context);
	case DOMNode::PROCESSING_INSTRUCTION_NODE:
		MATERIALISE_NODE;
		return context->getItemFactory()->createQName(XMLUni::fgZeroLenString,
			XMLUni::fgZeroLenString, node_->getNodeName(), context);
	}
	return 0;
}

ATQNameOrDerived::Ptr DbXmlNamespaceNodeImpl::dmNodeName(const DynamicContext* context) const
{  
	if(prefix_)
		return context->getItemFactory()->createQName(XMLUni::fgZeroLenString,
			XMLUni::fgZeroLenString, prefix_, context);
	return 0;
}

const XMLCh* DbXmlNodeImpl::dmStringValue(const DynamicContext* context) const
{
	MATERIALISE_NODE;
	if(node_ == 0) return XMLUni::fgZeroLenString; // No content documents

	// TBD store the node value in the IndexEntry object - jpcs

	// We don't currently store type information
	
	switch(node_->getNodeType()) {
	case DOMNode::DOCUMENT_NODE: {
		// Use getNsTextContent(), as that returns the value
		// for a document node properly
		return context->getMemoryManager()->getPooledString(
			((NsDomElement*)fakeDynamicCastNsDomNodeC(node_))->getNsTextContent());
	}
	case DOMNode::ELEMENT_NODE:
	case DOMNode::ATTRIBUTE_NODE:
	case DOMNode::PROCESSING_INSTRUCTION_NODE:
	case DOMNode::COMMENT_NODE:
	case DOMNode::TEXT_NODE:
	case DOMNode::CDATA_SECTION_NODE: {
		return context->getMemoryManager()->getPooledString(node_->getTextContent());
	}

	}
	return XMLUni::fgZeroLenString;
}

const XMLCh* DbXmlNamespaceNodeImpl::dmStringValue(const DynamicContext* context) const
{
	return uri_;
}

Sequence DbXmlNodeImpl::dmTypedValue(DynamicContext* context) const
{
	switch(getNodeType()) {
	case DOMNode::ATTRIBUTE_NODE:
	case DOMNode::ELEMENT_NODE:
	case DOMNode::DOCUMENT_NODE:
	case DOMNode::TEXT_NODE:
	case DOMNode::CDATA_SECTION_NODE: {
		return Sequence(context->getItemFactory()->createUntypedAtomic(dmStringValue(context), context),
			context->getMemoryManager()); 
        }
	case DOMNode::COMMENT_NODE:
	case DOMNode::PROCESSING_INSTRUCTION_NODE: {
		return Sequence(context->getItemFactory()->createString(dmStringValue(context), context),
			context->getMemoryManager());
        }
    }
    return Sequence(context->getMemoryManager());
}

Sequence DbXmlNamespaceNodeImpl::dmTypedValue(const DynamicContext* context) const
{
	return Sequence(context->getItemFactory()->createString(uri_, context), context->getMemoryManager());
}

Node::Ptr DbXmlNodeImpl::dmParent(const DynamicContext* context) const
{
	if(node_ == 0) {
		if(ie_ && ie_->isSpecified(IndexEntry::NODE_ID))
			const_cast<DbXmlNodeImpl*>(this)->node_ = ie_->fetchNode(getXmlDocument());
		else return 0;
	}

	DOMNode *parent = XPath2NSUtils::getParent(node_);
	if(parent == 0) return 0;

	return ((DbXmlFactoryImpl*)context->getItemFactory())->createNode(parent, document_, context);
}

Node::Ptr DbXmlNamespaceNodeImpl::dmParent(const DynamicContext* context) const
{
	if(parent_ == 0) return 0;

	return ((DbXmlFactoryImpl*)context->getItemFactory())->createNode(fakeDynamicCastDOMNodeC(parent_), document_, context);
}

Result DbXmlNodeImpl::dmAttributes(const DynamicContext* context, const LocationInfo *location) const
{
	if(getNodeType() == DOMNode::ELEMENT_NODE) {
		return new DbXmlAttributeAxis(location, this, NULL);
	}
	return 0;
}

Result DbXmlNodeImpl::dmNamespaceNodes(const DynamicContext* context, const LocationInfo *location) const
{
	if(getNodeType() == DOMNode::ELEMENT_NODE) {
		return new DbXmlNamespaceAxis(location, this, NULL);
	}
	return 0;
}

Result DbXmlNodeImpl::dmChildren(const DynamicContext *context, const LocationInfo *location) const
{
	if(getNodeType() == DOMNode::ELEMENT_NODE || getNodeType() == DOMNode::DOCUMENT_NODE) {
		return new DbXmlChildAxis(location, this, NULL);
	}
	return 0;
}

Result DbXmlNodeImpl::getAxisResult(XQStep::Axis axis, const NodeTest *nt, const DynamicContext *context,
	const LocationInfo *location) const
{
	return getAxisDbXmlResult(axis, nt, context, location);
}

DbXmlResult DbXmlNodeImpl::getAxisDbXmlResult(XQStep::Axis axis, const NodeTest *nt, const DynamicContext *context,
	const LocationInfo *location) const
{
	DBXML_ASSERT(nt == 0 || nt->getInterface(DbXmlNodeTest::gDbXml) != 0);
	const DbXmlNodeTest *nodeTest = nt == 0 ? 0 : (const DbXmlNodeTest*)nt;

	switch((Join::Type)axis) {
	case Join::ANCESTOR: {
		MATERIALISE_NODE;
		if(node_ == 0) return 0; // No content documents

		return new AncestorAxis(location, node_, this, nodeTest, *this);
	}
	case Join::ANCESTOR_OR_SELF: {
		if(getNodeType() == DOMNode::DOCUMENT_NODE) {
			if(nodeTest == 0) return new SelfAxis(location, this);
			return nodeTest->filterResult(new SelfAxis(location, this), location);
		}

		MATERIALISE_NODE;
		return new AncestorOrSelfAxis(location, node_, this, nodeTest, *this);
	}
	case Join::ATTRIBUTE: {
		if(getNodeType() == DOMNode::ELEMENT_NODE) {
			return new DbXmlAttributeAxis(location, this, nodeTest);
		}
		break;
	}
	case Join::ATTRIBUTE_OR_CHILD: {
		if(getNodeType() == DOMNode::DOCUMENT_NODE) {
			MATERIALISE_NODE;
			if(node_ == 0) return 0; // No content documents

			if(nodeTest != 0 && nodeTest->getItemType() == 0 && (nodeTest->getHasChildren() ||
				   (!nodeTest->getTypeWildcard() && nodeTest->getNodeType() == Node::element_string))) {
				return new ElementChildAxis(location, this, nodeTest);
			}
			return new DbXmlChildAxis(location, this, nodeTest);
		}
		else if(getNodeType() == DOMNode::ELEMENT_NODE) {
			if(nodeTest != 0 && nodeTest->getItemType() == 0 && (nodeTest->getHasChildren() ||
				   (!nodeTest->getTypeWildcard() && nodeTest->getNodeType() == Node::element_string))) {
				return new ElementChildAxis(location, this, nodeTest);
			}
			return new DbXmlAttributeOrChildAxis(location, this, nodeTest);
		}
		break;
	}
	case Join::CHILD: {
		if(getNodeType() == DOMNode::ELEMENT_NODE || getNodeType() == DOMNode::DOCUMENT_NODE) {
			MATERIALISE_NODE;
			if(node_ == 0) return 0; // No content documents

			if(nodeTest != 0 && nodeTest->getItemType() == 0 && (nodeTest->getHasChildren() ||
				   (!nodeTest->getTypeWildcard() && nodeTest->getNodeType() == Node::element_string))) {
				return new ElementChildAxis(location, this, nodeTest);
			}
			return new DbXmlChildAxis(location, this, nodeTest);
		}
		break;
	}
	case Join::DESCENDANT: {
		if(getNodeType() == DOMNode::ELEMENT_NODE || getNodeType() == DOMNode::DOCUMENT_NODE) {
			MATERIALISE_NODE;
			if(node_ == 0) return 0; // No content documents

			if(nodeTest != 0 && nodeTest->getItemType() == 0 && (nodeTest->getHasChildren() ||
				   (!nodeTest->getTypeWildcard() && nodeTest->getNodeType() == Node::element_string))) {
				return new ElementDescendantAxis(location, this, nodeTest);
			}
			return new DbXmlDescendantAxis(location, this, nodeTest);
		}
		break;
	}
	case Join::DESCENDANT_OR_SELF: {
		MATERIALISE_NODE;
		if(node_ == 0) {
			// No content documents
			if(nodeTest == 0) return new SelfAxis(location, this);
			return nodeTest->filterResult(new SelfAxis(location, this), location);
		}

		if(nodeTest != 0 && nodeTest->getItemType() == 0 && (nodeTest->getHasChildren() ||
			   (!nodeTest->getTypeWildcard() && nodeTest->getNodeType() == Node::element_string))) {
			return new ElementDescendantOrSelfAxis(location, this, nodeTest);
		}
		return new DbXmlDescendantOrSelfAxis(location, this, nodeTest);
	}
	case Join::FOLLOWING: {
		MATERIALISE_NODE;
		if(node_ == 0) return 0; // No content documents
		return new FollowingAxis(location, node_, this, nodeTest, *this);
	}
	case Join::FOLLOWING_SIBLING: {
		MATERIALISE_NODE;
		if(node_ == 0) return 0; // No content documents
		return new FollowingSiblingAxis(location, node_, this, nodeTest, *this);
	}
	case Join::NAMESPACE: {
		if(getNodeType() == DOMNode::ELEMENT_NODE) {
			return new DbXmlNamespaceAxis(location, this, nodeTest);
		}
		break;
	}
	case Join::PARENT_A:
	case Join::PARENT_C:
	case Join::PARENT: {
		MATERIALISE_NODE;
		if(node_ == 0) return 0; // No content documents

		return new DbXmlParentAxis(location, this, nodeTest);
	}
	case Join::PRECEDING: {
		MATERIALISE_NODE;
		if(node_ == 0) return 0; // No content documents

		return new PrecedingAxis(location, node_, this, nodeTest, *this);
	}
	case Join::PRECEDING_SIBLING: {
		MATERIALISE_NODE;
		if(node_ == 0) return 0; // No content documents

		return new PrecedingSiblingAxis(location, node_, this, nodeTest, *this);
	}
	case Join::SELF: {
		if(nodeTest == 0) return new SelfAxis(location, this);
		return nodeTest->filterResult(new SelfAxis(location, this), location);
	}
	case Join::NONE: break;
	}

	return 0;
}




DbXmlAxis::DbXmlAxis(const LocationInfo *location, const DbXmlNodeImpl *contextNode, const DbXmlNodeTest *nodeTest)
	: DbXmlResultImpl(location),
	  nodeObj_(contextNode),
	  contextNode_(const_cast<NsDomNode*>(fakeDynamicCastNsDomNodeC(contextNode->getDOMNode()))),
	  nodeTest_(nodeTest),
	  toDo_(true)
{
	DBXML_ASSERT(contextNode_);
}

inline bool equals(const xmlbyte_t *const str1, const xmlbyte_t *const str2) {
	if(str1 == str2) return true;

	if(str1 == 0) return *str2 == 0; // str2 == 0 is handled by the first line
	if(str2 == 0) return *str1 == 0; // str1 == 0 is handled by the first line

	register const xmlbyte_t* psz1 = str1;
	register const xmlbyte_t* psz2 = str2;

	while(*psz1 == *psz2) {
		// If either has ended, then they both ended, so equal
		if(*psz1 == 0)
			return true;
                                                                                                                                                              
		// Move upwards for the next round
		++psz1;
		++psz2;
	}
	return false;
}

Item::Ptr DbXmlAxis::next(DynamicContext *context)
{
	DbXmlFactoryImpl *factory = (DbXmlFactoryImpl*)context->getItemFactory();

	const NsDomNode *node = 0;
	while((node = nextNode()) != 0) {
		context->testInterrupt();

		// Check for ignorable whitespace
		if(node->getNsNodeType() == nsNodeText) {
			NsDomText *text = (NsDomText*)node;
			if(nsTextType(text->getNsTextType()) == NS_TEXT ||
				nsTextType(text->getNsTextType()) == NS_CDATA) {
				if(((NsDomText*)node)->nsIgnorableWhitespace()) {
					continue;
				}
			}
		}

		if(nodeTest_ == 0) return factory->createNode(fakeDynamicCastDOMNodeC(node), nodeObj_->getXmlDocument(), context);

		SequenceType::ItemType *itemType = nodeTest_->getItemType();
		if(itemType != 0)
		{
			Node::Ptr result = factory->createNode(fakeDynamicCastDOMNodeC(node), nodeObj_->getXmlDocument(), context);
			if(itemType->matches(result, context, this)) {
				return result;
			}
		}
		else
		{
			switch(node->getNsNodeType())
			{      
			case nsNodeDocument: {
				if(!nodeTest_->getTypeWildcard() && nodeTest_->getNodeType() != Node::document_string) continue;
				if(!nodeTest_->getNameWildcard() || !nodeTest_->getNamespaceWildcard()) continue;
				break;
			}
			case nsNodeElement: {
				if(!nodeTest_->getTypeWildcard() && nodeTest_->getNodeType() != Node::element_string) continue;
				if(!nodeTest_->getNameWildcard()) {
					if(node->isNativeUTF8()) {
						if(!equals(node->getNsLocalName8(), nodeTest_->getNodeName8())) continue;
					} else {
						if(!XPath2Utils::equals(node->getNsLocalName(), nodeTest_->getNodeName())) continue;
					}
				}
				if(!nodeTest_->getNamespaceWildcard())
				{
					if(node->isNativeUTF8()) {
						if(!equals(node->getNsUri8(), nodeTest_->getNodeUri8())) continue;
					} else {
						if(!XPath2Utils::equals(node->getNsUri(), nodeTest_->getNodeUri())) continue;
					}
				}
				break;
			}
			case nsNodeAttr: {
				if(nodeTest_->getTypeWildcard()) { if(nodeTest_->getHasChildren()) continue; }
				else if(nodeTest_->getNodeType() != Node::attribute_string) continue;
				if(!nodeTest_->getNameWildcard()) {
					if(node->isNativeUTF8()) {
						if(!equals(node->getNsLocalName8(), nodeTest_->getNodeName8())) continue;
					} else {
						if(!XPath2Utils::equals(node->getNsLocalName(), nodeTest_->getNodeName())) continue;
					}
				}
				if(!nodeTest_->getNamespaceWildcard()) {
					if(node->isNativeUTF8()) {
						if(!equals(node->getNsUri8(), nodeTest_->getNodeUri8())) continue;
					} else {
						if(!XPath2Utils::equals(node->getNsUri(), nodeTest_->getNodeUri())) continue;
					}
				}
				break;
			}
			case nsNodeText: {
				switch(nsTextType(((NsDomText*)node)->getNsTextType()))
				{
				case NS_CDATA:
				case NS_TEXT: {
					if(nodeTest_->getTypeWildcard()) { if(nodeTest_->getHasChildren()) continue; }
					else if(nodeTest_->getNodeType() != Node::text_string) continue;
					if(!nodeTest_->getNameWildcard() || !nodeTest_->getNamespaceWildcard()) continue;
					break;
				}
				case NS_PINST: {
					if(nodeTest_->getTypeWildcard()) { if(nodeTest_->getHasChildren()) continue; }
					else if(nodeTest_->getNodeType() != Node::processing_instruction_string) continue;
					if(!nodeTest_->getNameWildcard()) {
						if(node->isNativeUTF8()) {
							if(!equals(node->getNsNodeName8(), nodeTest_->getNodeName8())) continue;
						} else {
							if(!XPath2Utils::equals(node->getNsNodeName(), nodeTest_->getNodeName())) continue;
						}
					}
					if(!nodeTest_->getNamespaceWildcard()) continue;
					break;
				}
				case NS_COMMENT: {
					if(nodeTest_->getTypeWildcard()) { if(nodeTest_->getHasChildren()) continue; }
					else if(nodeTest_->getNodeType() != Node::comment_string) continue;
					if(!nodeTest_->getNameWildcard() || !nodeTest_->getNamespaceWildcard()) continue;
					break;
				}
				default: {
					continue;
				}
				}
				break;
			}
			default: {
				continue;
			}
			}

			return factory->createNode(fakeDynamicCastDOMNodeC(node), nodeObj_->getXmlDocument(), context);
		}
	}

	return 0;
}




DbXmlAttributeAxis::DbXmlAttributeAxis(const LocationInfo *location, const DbXmlNodeImpl *contextNode, const DbXmlNodeTest *nodeTest)
	: DbXmlAxis(location, contextNode, nodeTest),
	  nodeMap_(0),
	  i_(0)
{
}

Item::Ptr DbXmlAttributeAxis::seek(const DocID &did, const NsNid &nid, DynamicContext *context)
{
	if(contextNode_->getNsDocument()->getDocId() < did ||
		nid.compareNids(contextNode_->getNodeId()) > 0) {
		toDo_ = false;
		nodeMap_ = 0;
		return 0;
	}

	return next(context);
}

const NsDomNode *DbXmlAttributeAxis::nextNode()
{
	if(toDo_) {
		// initialise
		toDo_ = false;

		if(contextNode_->getNsNodeType() == nsNodeElement) {
			nodeMap_ = ((NsDomElement*)contextNode_)->getNsAttributes();
		}
	}

	const NsDomAttr *result = 0;
	if(nodeMap_ != 0) {
		unsigned int nLen = nodeMap_->getNsLength();
		while(result == 0 && i_ < (int)nLen) {
			result = nodeMap_->getNsItem(i_);
			++i_;
			// Check to see if this is a namespace attribute
			if(XPath2Utils::equals(result->getNsUri(), XMLUni::fgXMLNSURIName))
				result = 0;
		}
	}

	return result;
}

std::string DbXmlAttributeAxis::asString(DynamicContext *context, int indent) const
{
	return "DbXmlAttributeAxis";
}

DbXmlNamespaceAxis::DbXmlNamespaceAxis(const LocationInfo *location, const DbXmlNodeImpl *contextNode, const DbXmlNodeTest *nodeTest)
	: ResultImpl(location),
	  nodeObj_(contextNode),
	  contextNode_(const_cast<NsDomNode*>(fakeDynamicCastNsDomNodeC(contextNode->getDOMNode()))),
	  nodeTest_(nodeTest),
	  toDo_(true),
	  node_((contextNode_->getNsNodeType() == nsNodeElement) ? ((NsDomElement*)contextNode_) : 0),
	  nodeMap_((node_ == 0) ? 0 : node_->getNsAttributes()),
	  i_(0),
	  defNsTested_(false)
{
}

Item::Ptr DbXmlNamespaceAxis::next(DynamicContext *context)
{
	DbXmlNamespaceNodeImpl::Ptr node;
	while((node = nextNode(context)).notNull()) {
		if(nodeTest_ == 0) return node;

		SequenceType::ItemType *itemType = nodeTest_->getItemType();
		if(itemType != 0) {
			if(itemType->matches((Node::Ptr)node, context, this)) {
				return node;
			}
		}
		else {
			if(nodeTest_->getTypeWildcard()) { if(nodeTest_->getHasChildren()) continue; }
			else if(nodeTest_->getNodeType() != Node::namespace_string) continue;
			if(!nodeTest_->getNameWildcard() && !XPath2Utils::equals(node->getPrefix(), nodeTest_->getNodeName())) continue;
			if(!nodeTest_->getNamespaceWildcard() && !XPath2Utils::equals(XMLUni::fgZeroLenString, nodeTest_->getNodeUri())) continue;

			return node;
		} 
	}

	return 0;
}

DbXmlNamespaceNodeImpl::Ptr DbXmlNamespaceAxis::nextNode(DynamicContext *context)
{
	if(toDo_) {
		// initialise
		toDo_ = false;

		// Output the obligatory namespace node for the "xml" prefix
		return new DbXmlNamespaceNodeImpl(XMLUni::fgXMLString, XMLUni::fgXMLURIName, (NsDomElement*)contextNode_,
			nodeObj_->getXmlDocument(), context);
	}

	if(node_ == 0) return 0;

	const NsDomAttr *result = 0;
	const XMLCh *prefix = 0;
	while(result == 0) {
		if(nodeMap_ == 0 || i_ >= nodeMap_->getNsLength()) {
			node_ = node_->getNsParentNode();
			if(node_ != 0) {
				nodeMap_ = node_->getNsAttributes();
				i_ = 0;
				// check if this node has a namespace, but the prefix is not declared in the attribute map
				const XMLCh* uri = node_->getNsUri();
				prefix = node_->getNsPrefix();
				if(uri && *uri && 
					nodeMap_->getNsNamedItemNS(XMLUni::fgXMLNSString, prefix) == 0 &&
					done_.insert(prefix).second) {
					return new DbXmlNamespaceNodeImpl(prefix, uri, (NsDomElement*)contextNode_,
						nodeObj_->getXmlDocument(), context);
				}
				continue;
			}
			else {
				nodeMap_ = 0;
				if(!defNsTested_) {
					defNsTested_ = true;
					if(context->getDefaultElementAndTypeNS() != 0 &&
						done_.insert(0).second) {
						return new DbXmlNamespaceNodeImpl(0, context->getDefaultElementAndTypeNS(),
							(NsDomElement*)contextNode_, nodeObj_->getXmlDocument(), context);
					}
				}
				return 0;
			}
		}

		result = nodeMap_->getNsItem(i_);
		++i_;

		// Check to see if this is a namespace attribute
		if(!XPath2Utils::equals(result->getNsUri(), XMLUni::fgXMLNSURIName)) {
			result = 0;
			continue;
		}

		prefix = (result->getNsPrefix() == 0) ? 0 : result->getNsLocalName();

		// If the uri is empty, the prefix has been unset;
		// add it to the map of "already reported" prefixes and go on
		const XMLCh* uri = result->getNsNodeValue();
		if(uri == 0 || *uri==0) {
			done_.insert(prefix);
			result = 0;
			continue;
		}
 
		// Add namespace, if not already there
		if(!done_.insert(prefix).second) {
			result = 0;
			continue;
		}
	}

	return new DbXmlNamespaceNodeImpl(prefix, result->getNsNodeValue(),
		(NsDomElement*)contextNode_, nodeObj_->getXmlDocument(), context);
}

std::string DbXmlNamespaceAxis::asString(DynamicContext *context, int indent) const
{
	return "DbXmlNamespaceAxis";
}

DbXmlAttributeOrChildAxis::DbXmlAttributeOrChildAxis(const LocationInfo *location, const DbXmlNodeImpl *contextNode, const DbXmlNodeTest *nodeTest)
	: DbXmlAxis(location, contextNode, nodeTest),
	  nodeMap_(0),
	  i_(0),
	  child_(0)
{
}

Item::Ptr DbXmlAttributeOrChildAxis::seek(const DocID &did, const NsNid &nid, DynamicContext *context)
{
	if(contextNode_->getNsDocument()->getDocId() < did ||
		nid.compareNids(contextNode_->getLastDescendantNid()) > 0) {
		toDo_ = false;
		nodeMap_ = 0;
		child_ = 0;
		return 0;
	}

	return next(context);
}

const NsDomNode *DbXmlAttributeOrChildAxis::nextNode()
{
	if(toDo_) {
		// initialise
		toDo_ = false;

		if(contextNode_->getNsNodeType() == nsNodeElement) {
			nodeMap_ = ((NsDomElement*)contextNode_)->getNsAttributes();
		}
	}

	if(nodeMap_ != 0) {
		const NsDomAttr *result;
		unsigned int nLen = nodeMap_->getNsLength();
		while(i_ < (int)nLen) {
			result = nodeMap_->getNsItem(i_);
			++i_;
			// Check to see if this is a namespace attribute
			if(!XPath2Utils::equals(result->getNsUri(), XMLUni::fgXMLNSURIName))
				return result;
		}

		nodeMap_ = 0;
		child_ = contextNode_->getNsFirstChild();
	} else if(child_ != 0) {
		child_ = child_->getNsNextSibling();
	}

	return child_;
}

std::string DbXmlAttributeOrChildAxis::asString(DynamicContext *context, int indent) const
{
	return "DbXmlAttributeOrChildAxis";
}

DbXmlChildAxis::DbXmlChildAxis(const LocationInfo *location, const DbXmlNodeImpl *contextNode, const DbXmlNodeTest *nodeTest)
	: DbXmlAxis(location, contextNode, nodeTest),
	  child_(0)
{
}

Item::Ptr DbXmlChildAxis::seek(const DocID &did, const NsNid &nid, DynamicContext *context)
{
	if(contextNode_->getNsDocument()->getDocId() < did ||
		nid.compareNids(contextNode_->getLastDescendantNid()) > 0) {
		toDo_ = false;
		child_ = 0;
		return 0;
	}

	return next(context);
}

const NsDomNode *DbXmlChildAxis::nextNode()
{
	if(toDo_) {
		// initialise
		toDo_ = false;

		child_ = contextNode_->getNsFirstChild();
	}
	else if(child_ != 0) {
		child_ = child_->getNsNextSibling();
	}

	return child_;
}

std::string DbXmlChildAxis::asString(DynamicContext *context, int indent) const
{
	return "DbXmlChildAxis";
}


ElementChildAxis::ElementChildAxis(const LocationInfo *location, const DbXmlNodeImpl *contextNode, const DbXmlNodeTest *nodeTest)
	: DbXmlAxis(location, contextNode, nodeTest),
	  child_(0)
{
}

Item::Ptr ElementChildAxis::seek(const DocID &did, const NsNid &nid, DynamicContext *context)
{
	if(contextNode_->getNsDocument()->getDocId() < did ||
		nid.compareNids(contextNode_->getLastDescendantNid()) > 0) {
		toDo_ = false;
		child_ = 0;
		return 0;
	}

	return next(context);
}

const NsDomNode *ElementChildAxis::nextNode()
{
	if(toDo_) {
		// initialise
		toDo_ = false;

		if(contextNode_->getNsNodeType() == nsNodeElement ||
			contextNode_->getNsNodeType() == nsNodeDocument) {

			child_ = ((NsDomElement*)contextNode_)->getElemFirstChild();
		}
	}
	else if(child_ != 0) {
		child_ = child_->getElemNext();
	}

	return child_;
}

std::string ElementChildAxis::asString(DynamicContext *context, int indent) const
{
	return "ElementChildAxis";
}



DbXmlDescendantAxis::DbXmlDescendantAxis(const LocationInfo *location, const DbXmlNodeImpl *contextNode, const DbXmlNodeTest *nodeTest)
	: DbXmlAxis(location, contextNode, nodeTest),
	  descendant_(0)
{
}

Item::Ptr DbXmlDescendantAxis::seek(const DocID &did, const NsNid &nid, DynamicContext *context)
{
	if(contextNode_->getNsDocument()->getDocId() < did ||
		nid.compareNids(contextNode_->getLastDescendantNid()) > 0) {
		toDo_ = false;
		descendant_ = 0;
		return 0;
	}

	return next(context);
}

const NsDomNode *DbXmlDescendantAxis::nextNode()
{
	if(toDo_) {
		// initialise
		toDo_ = false;
		descendant_ = contextNode_->getNsFirstChild();
	}
	else if(descendant_ != 0) {
		NsDomNode *result = descendant_->getNsFirstChild();

		while(result == 0) {
			result = descendant_->getNsNextSibling();
			if(result == 0) {
				descendant_ = descendant_->getNsParentNode();
				if(descendant_ == 0 || *descendant_ == *contextNode_) break;
			}
		}

		descendant_ = result;
	}

	return descendant_;
}

std::string DbXmlDescendantAxis::asString(DynamicContext *context, int indent) const
{
	return "DbXmlDescendantAxis";
}


ElementDescendantAxis::ElementDescendantAxis(const LocationInfo *location, const DbXmlNodeImpl *contextNode, const DbXmlNodeTest *nodeTest)
	: DbXmlAxis(location, contextNode, nodeTest),
	  nscontext_(0),
	  descendant_(0)
{
}

Item::Ptr ElementDescendantAxis::seek(const DocID &did, const NsNid &nid, DynamicContext *context)
{
	if(contextNode_->getNsDocument()->getDocId() < did ||
		nid.compareNids(contextNode_->getLastDescendantNid()) > 0) {
		toDo_ = false;
		descendant_ = 0;
		return 0;
	}

	return next(context);
}

const NsDomNode *ElementDescendantAxis::nextNode()
{
	if(toDo_) {
		// initialise
		toDo_ = false;

		if(contextNode_->getNsNodeType() == nsNodeElement ||
			contextNode_->getNsNodeType() == nsNodeDocument) {

			nscontext_ = (NsDomElement*)contextNode_;
			descendant_ = nscontext_->getElemFirstChild();
		}
	}
	else if(descendant_ != 0) {
		NsDomElement *result = descendant_->getElemFirstChild();

		while(result == 0) {
			result = descendant_->getElemNext();
			if(result == 0) {
				descendant_ = descendant_->getElemParent();
				if(descendant_ == 0 || *descendant_ == *nscontext_) break;
			}
		}

		descendant_ = result;
	}

	return descendant_;
}

std::string ElementDescendantAxis::asString(DynamicContext *context, int indent) const
{
	return "ElementDescendantAxis";
}



DbXmlDescendantOrSelfAxis::DbXmlDescendantOrSelfAxis(const LocationInfo *location, const DbXmlNodeImpl *contextNode, const DbXmlNodeTest *nodeTest)
	: DbXmlAxis(location, contextNode, nodeTest),
	  descendant_(0)
{
}

Item::Ptr DbXmlDescendantOrSelfAxis::seek(const DocID &did, const NsNid &nid, DynamicContext *context)
{
	if(contextNode_->getNsDocument()->getDocId() < did ||
		nid.compareNids(contextNode_->getLastDescendantNid()) > 0) {
		toDo_ = false;
		descendant_ = 0;
		return 0;
	}

	return next(context);
}

const NsDomNode *DbXmlDescendantOrSelfAxis::nextNode()
{
	if(toDo_) {
		// initialise
		toDo_ = false;
		descendant_ = contextNode_;
	}
	else if(descendant_ != 0) {
		NsDomNode *result = descendant_->getNsFirstChild();

		while(result == 0 && *descendant_ != *contextNode_) {
			result = descendant_->getNsNextSibling();
			if(result == 0) {
				descendant_ = descendant_->getNsParentNode();
				if(descendant_ == 0 || *descendant_ == *contextNode_) break;
			}
		}

		descendant_ = result;
	}

	return descendant_;
}

std::string DbXmlDescendantOrSelfAxis::asString(DynamicContext *context, int indent) const
{
	return "DbXmlDescendantOrSelfAxis";
}


ElementDescendantOrSelfAxis::ElementDescendantOrSelfAxis(const LocationInfo *location, const DbXmlNodeImpl *contextNode, const DbXmlNodeTest *nodeTest)
	: DbXmlAxis(location, contextNode, nodeTest),
	  nscontext_(0),
	  descendant_(0)
{
}

Item::Ptr ElementDescendantOrSelfAxis::seek(const DocID &did, const NsNid &nid, DynamicContext *context)
{
	if(contextNode_->getNsDocument()->getDocId() < did ||
		nid.compareNids(contextNode_->getLastDescendantNid()) > 0) {
		toDo_ = false;
		descendant_ = 0;
		return 0;
	}

	return next(context);
}

const NsDomNode *ElementDescendantOrSelfAxis::nextNode()
{
	if(toDo_) {
		// initialise
		toDo_ = false;

		if(contextNode_->getNsNodeType() == nsNodeElement ||
			contextNode_->getNsNodeType() == nsNodeDocument) {

			nscontext_ = (NsDomElement*)contextNode_;
			descendant_ = nscontext_;
		}
	} else if(descendant_ != 0) {
		NsDomElement *result = descendant_->getElemFirstChild();

		while ((result == 0) && (*descendant_ != *nscontext_)){
			result = descendant_->getElemNext();
			if(result == 0) {
				descendant_ = descendant_->getElemParent();
				if(descendant_ == 0 || *descendant_ == *nscontext_) break;
			}
		}

		descendant_ = result;
	}

	return descendant_;
}

std::string ElementDescendantOrSelfAxis::asString(DynamicContext *context, int indent) const
{
	return "ElementDescendantOrSelfAxis";
}


DbXmlParentAxis::DbXmlParentAxis(const LocationInfo *location, const DbXmlNodeImpl *contextNode, const DbXmlNodeTest *nodeTest)
	: DbXmlAxis(location, contextNode, nodeTest)
{
}

Item::Ptr DbXmlParentAxis::seek(const DocID &did, const NsNid &nid, DynamicContext *context)
{
	if(contextNode_->getNsDocument()->getDocId() < did ||
		nid.compareNids(contextNode_->getNodeId()) > 0) {
		toDo_ = false;
		return 0;
	}

	return next(context);
}

const NsDomNode *DbXmlParentAxis::nextNode()
{
	if(toDo_) {
		// initialise
		toDo_ = false;
		if(contextNode_->getNsNodeType() == nsNodeAttr)
			return ((NsDomAttr*)contextNode_)->getNsOwner();
		return contextNode_->getNsParentNode();
	}
	return 0;
}

std::string DbXmlParentAxis::asString(DynamicContext *context, int indent) const
{
	return "DbXmlParentAxis";
}


