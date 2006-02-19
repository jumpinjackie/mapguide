//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: DbXmlNodeImpl.cpp,v 1.25 2005/12/07 15:58:17 jsnelson Exp $
//

#include <algorithm>

#include "DbXmlNodeImpl.hpp"
#include "DbXmlNodeTest.hpp"
#include "DbXmlFactoryImpl.hpp"
#include "../nodeStore/NsXercesDom.hpp"
#include "../nodeStore/NsDocument.hpp"
#include "../UTF8.hpp"
#include "../Value.hpp"
#include "../Document.hpp"
#include "../Globals.hpp"

#include <pathan/DynamicContext.hpp>
#include <pathan/PathanFactory.hpp>
#include <pathan/ATAnyURIOrDerived.hpp>
#include <pathan/ATStringOrDerived.hpp>
#include <pathan/ATQNameOrDerived.hpp>
#include <pathan/ATUntypedAtomic.hpp>
#include <pathan/exceptions/ItemException.hpp>
#include <pathan/XPath2Utils.hpp>
#include <pathan/internal/context/DocumentCacheImpl.hpp>
#include <pathan/functions/FunctionConstructor.hpp>
#include <pathan/exceptions/XPath2TypeCastException.hpp>
#include <pathan/PathanException.hpp>
#include <pathan/functions/FunctionRoot.hpp>
#include <pathan/XPath2NSUtils.hpp>

#include <pathan/internal/navigation/AncestorAxis.hpp>
#include <pathan/internal/navigation/AncestorOrSelfAxis.hpp>
#include <pathan/internal/navigation/FollowingAxis.hpp>
#include <pathan/internal/navigation/FollowingSiblingAxis.hpp>
#include <pathan/internal/navigation/PrecedingAxis.hpp>
#include <pathan/internal/navigation/PrecedingSiblingAxis.hpp>
#include <pathan/internal/navigation/SelfAxis.hpp>

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

#define MATERIALISE_NODE if(node_ == 0) { const_cast<DbXmlNodeImpl*>(this)->node_ = document_.getContentAsDOM(); }

const XMLCh DbXmlNodeImpl::gDbXml[] =
{
	chLatin_D, chLatin_b,
	chLatin_X, chLatin_m,
	chLatin_l,
	chNull
};

static inline const NsDomNode *fakeDynamicCastNsDomNode(const DOMNode *node)
{
	if (!node) return 0;
	return (NsDomNode*)
		const_cast<DOMNode*>(node)->getInterface(_nsDomString);
}

static DOMNode *fakeDynamicCastDOMNode(NsDomNode *node)
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
	case nsNodeDocument:
	{
		NsXDOMDocument *doc =
			(NsXDOMDocument*)node->getNsInterface("document");
		return doc;
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
	default:
		NsXDOMNotImplemented("bad NsNodeType");
	}
	return 0;
}

inline const DOMNode *fakeDynamicCastDOMNode(const NsDomNode *node)
{
	return fakeDynamicCastDOMNode(const_cast<NsDomNode*>(node));
}



DbXmlNodeImpl::DbXmlNodeImpl(Document *doc, const DynamicContext *context, const DbXmlFactoryImpl *factory)
	: node_(0),
	  ie_(0),
	  document_(doc),
	  factory_(factory),
	  poolNext(0),
	  allocNext(0)
{
	DBXML_ASSERT(doc != 0);
}

DbXmlNodeImpl::DbXmlNodeImpl(const DOMNode *node, Document *doc, const DynamicContext *context, const DbXmlFactoryImpl *factory)
	: node_(node),
	  ie_(0),
	  document_(doc),
	  factory_(factory),
	  poolNext(0),
	  allocNext(0)
{
	DBXML_ASSERT(doc != 0);
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
			const_cast<DbXmlNodeImpl*>(this)->node_ = 0;
			const_cast<DbXmlNodeImpl*>(this)->ie_.reset(0);
			const_cast<DbXmlNodeImpl*>(this)->document_ = 0;
			factory_->addToNodePool(const_cast<DbXmlNodeImpl*>(this));
		} else {
			delete this;
		}
	}
}

void DbXmlNodeImpl::init(Document *doc)
{
	DBXML_ASSERT(doc != 0);

	document_ = doc;
}

void DbXmlNodeImpl::init(const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node, Document *doc)
{
	DBXML_ASSERT(doc != 0);

	node_ = node;
	document_ = doc;
}

const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *DbXmlNodeImpl::getDOMNode() const
{
	MATERIALISE_NODE;

	return node_;
}

const Node::Ptr DbXmlNodeImpl::createNode(const DOMNode *node, const DynamicContext *context) const
{
  return ((DbXmlFactoryImpl*)context->getPathanFactory())->createNode(node, document_, context);
}

DbXmlNodeImpl::Ptr DbXmlNodeImpl::getDescendantElement(const IndexEntry::SharedPtr &ie, const DynamicContext *context) const
{
	MATERIALISE_NODE;

	const NsDomNode *nsnode = fakeDynamicCastNsDomNode(node_);

	switch(nsnode->getNsNodeType()) {
	case nsNodeDocument:
	case nsNodeElement: {
		NsDomElement *resultElement = ((NsDomElement*)nsnode)->findDescendantElement(&ie->getNodeID());
		if(resultElement == 0) return 0;
		DbXmlNodeImpl::Ptr result = ((DbXmlFactoryImpl*)context->getPathanFactory())->
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
	XmlValue value;
	if(document_.getMetaData(XMLChToUTF8(uri).str(), XMLChToUTF8(name).str(), value)) {
		return Value::convertToItem(value, context);
	}
	return 0;
}

bool DbXmlNodeImpl::hasElemChildren() const
{
	if(node_ == 0) return true;

	const NsDomNode *nsnode = fakeDynamicCastNsDomNode(node_);

	switch(nsnode->getNsNodeType()) {
	case nsNodeDocument:
	case nsNodeElement: {
		return ((const NsDomElement*)nsnode)->numNsChildElements() != 0;
	}
	default: {
		return false;
	}
	}
}

u_int32_t DbXmlNodeImpl::getLevel() const
{
	if(node_ == 0) return 0;

	return fakeDynamicCastNsDomNode(node_)->getNsLevel();
}

const nid_t *DbXmlNodeImpl::getNID() const
{
	if(node_ == 0) return &Globals::docRootNID;

	return fakeDynamicCastNsDomNode(node_)->getNodeId();
}

const nid_t *DbXmlNodeImpl::getLastElemDescendantNID() const
{
	if(ie_ && ie_->isSpecified(IndexEntry::LAST_DESCENDANT_ID))
		return &ie_->getLastDescendant();

	return 0;
}

const nid_t *DbXmlNodeImpl::getLastElemChildNID(bool fetch) const
{
	if(!fetch && node_ == 0) return 0;

	MATERIALISE_NODE;

	const NsDomNode *nsnode = fakeDynamicCastNsDomNode(node_);

	switch(nsnode->getNsNodeType()) {
	case nsNodeDocument:
	case nsNodeElement: {
		return nsLastChildNid(nsnode->getNsNode());
	}
	default: {
		return 0;
	}
	}
}

DbXmlNodeImpl::Ptr DbXmlNodeImpl::getLastElemChild(const DynamicContext *context) const
{
	MATERIALISE_NODE;

	const NsDomNode *nsnode = fakeDynamicCastNsDomNode(node_);

	switch(nsnode->getNsNodeType()) {
	case nsNodeDocument:
	case nsNodeElement: {
		NsDomElement *lastChild = ((NsDomElement*)nsnode)->getElemLastChild();
		if(lastChild == 0) return 0;
		return ((DbXmlFactoryImpl*)context->getPathanFactory())->createNode(fakeDynamicCastDOMNode(lastChild), document_, context);
	}
	default: {
		return 0;
	}
	}	
}

DbXmlNodeImpl::Ptr DbXmlNodeImpl::getAttribute(u_int32_t index, const DynamicContext *context) const
{
	if(node_ == 0) return 0;

	DOMNode *attr = node_->getAttributes()->item(index);
	DBXML_ASSERT(attr);

	return ((DbXmlFactoryImpl*)context->getPathanFactory())->createNode(attr, document_, context);
}

const XMLCh* DbXmlNodeImpl::asString(const DynamicContext* context) const
{
	MATERIALISE_NODE;

	DOMWriter *writer =
		DOMImplementation::getImplementation()->
		createDOMWriter(context->getMemoryManager());
	const XMLCh *result = writer->writeToString(*node_);
	writer->release();
	return result;
}

Sequence DbXmlNodeImpl::dmBaseURI(const DynamicContext* context) const
{
	// We don't currently store the base URI
	return Sequence(context->getMemoryManager());
}

Sequence DbXmlNodeImpl::dmDocumentURI(const DynamicContext* context) const
{
	// We don't currently store the document URI
	return Sequence(context->getMemoryManager());
}

const XMLCh* DbXmlNodeImpl::dmNodeKind() const
{  
	if(node_ == 0) return document_string;

	switch(node_->getNodeType()) {
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
	case DOMXPathNamespace::XPATH_NAMESPACE_NODE:
		return namespace_string;
	}
    
	DSLthrow(ItemException, X("DbXmlNodeImpl::dmNodeKind"), X("Unknown node type."));
}

ATQNameOrDerived::Ptr DbXmlNodeImpl::dmTypeName(const DynamicContext* context) const
{
	if(node_ == 0) return 0;

	// We don't currently store type information
	switch(node_->getNodeType()) {
		case DOMNode::ELEMENT_NODE: {
			return context->getPathanFactory()->createQName(FunctionConstructor::XMLChXPath2DatatypesURI,
				XMLUni::fgZeroLenString, DocumentCacheParser::g_szUntyped, context);
		}
		case DOMNode::TEXT_NODE:
		case DOMNode::CDATA_SECTION_NODE:
		case DOMNode::ATTRIBUTE_NODE: {
			return context->getPathanFactory()->createQName(FunctionConstructor::XMLChXPath2DatatypesURI,
				XMLUni::fgZeroLenString, ATUntypedAtomic::fgDT_UNTYPEDATOMIC, context);
		}
		default: {
			return 0;
		}
	}
}

ATBooleanOrDerived::Ptr DbXmlNodeImpl::dmNilled(const DynamicContext* context) const
{
	if(node_ == 0) return 0;
	
	// We don't currently store type information
	else if(node_->getNodeType() != DOMNode::ELEMENT_NODE) {
		return 0;
	}

	return context->getPathanFactory()->createBoolean(false, context);
}

ATBooleanOrDerived::Ptr DbXmlNodeImpl::dmIsId(const DynamicContext* context) const
{
	// We don't currently store type information
	return context->getPathanFactory()->createBoolean(false, context);
}

ATBooleanOrDerived::Ptr DbXmlNodeImpl::dmIsIdRefs(const DynamicContext* context) const
{
	// We don't currently store type information
	return context->getPathanFactory()->createBoolean(false, context);
}

bool DbXmlNodeImpl::hasInstanceOfType(const XMLCh* typeURI, const XMLCh* typeName, const DynamicContext* context) const
{
	return context->isTypeOrDerivedFromType(getTypeURI(), getTypeName(), typeURI, typeName);
}

const XMLCh* DbXmlNodeImpl::getTypeName() const
{
	if(node_ == 0) return 0;

	// We don't currently store type information
	switch(node_->getNodeType()) {
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
	if(node_ == 0) return 0;

	// We don't currently store type information
	switch(node_->getNodeType()) {
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

bool DbXmlNodeImpl::lessThan(const Node::Ptr &other, const DynamicContext *context) const
{
	MATERIALISE_NODE;

	const DbXmlNodeImpl *otherImpl = (const DbXmlNodeImpl*)other->getInterface(DbXmlNodeImpl::gDbXml);
	if(otherImpl == 0) {
		// It's not a DB XML implementation Node, so it can't
		// be from the same tree as us - jpcs

		// Order them according to the address of their roots
		return FunctionRoot::root(this, context).get() < FunctionRoot::root(other, context).get();
	}

	const DOMNode* otherNode = otherImpl->getDOMNode();

	// Are they from the same container?
	long container_compare = ((Document&)document_).getContainer() - ((Document&)otherImpl->getXmlDocument()).getContainer();
	if(container_compare != 0) return container_compare < 0;

	// Are they in the same document?
	long id_compare = ((Document&)document_).getID().raw() - ((Document&)otherImpl->getXmlDocument()).getID().raw();
	if(id_compare != 0) return id_compare < 0;

	short pos = const_cast<DOMNode *>(node_)->compareTreePosition(const_cast<DOMNode *>(otherNode));
	// compareTreePosition returns the position of the other node, compared to my position; so, if it sets the bit
	// TREE_POSITION_FOLLOWING, it means that we come before it
	if(pos & DOMNode::TREE_POSITION_FOLLOWING)
		return true;
	if(pos & DOMNode::TREE_POSITION_PRECEDING)
		return false;
	if(pos & DOMNode::TREE_POSITION_SAME_NODE)
		return false;
	// if the two nodes are attributes or namespaces, we get that they are equivalent; we need a stable ordering, so
	// we resort to comparing their pointers (we could even decide to sort them on their names...)
	if(pos & DOMNode::TREE_POSITION_EQUIVALENT)
		return node_ < otherNode;
	// if we get they are disconnected, it could be they belong to different documents; in this case, order them according
	// to the pointer of their documents (all the nodes in document A must come before or after all the nodes in document B,
	// regardless of the chosen ordering criteria)
	// If they belong to the same document, they are floating, or maybe just one of them is floating; let's state we consider
	// the one connected coming before the disconnected one, and, if both are disconnected, we compare the two roots
	if(pos == DOMNode::TREE_POSITION_DISCONNECTED) {
		Node::Ptr root1=FunctionRoot::root(this, context);
		Node::Ptr root2=FunctionRoot::root(other, context);
		return root1.get() < root2.get();
	}
	DBXML_ASSERT(false);
	return true;
}

bool DbXmlNodeImpl::equals(const Node::Ptr &other) const
{
	MATERIALISE_NODE;

	const DbXmlNodeImpl *otherImpl = (const DbXmlNodeImpl*)other->getInterface(DbXmlNodeImpl::gDbXml);
	if(otherImpl == 0) return false;
	return node_->isSameNode(otherImpl->getDOMNode());
}

bool DbXmlNodeImpl::uniqueLessThan(const Node::Ptr &other, const DynamicContext *context) const
{
	MATERIALISE_NODE;

	const DbXmlNodeImpl *otherImpl = (const DbXmlNodeImpl*)other->getInterface(DbXmlNodeImpl::gDbXml);
	if(otherImpl == 0) {
		// It's not a DB XML implementation Node, so it can't
		// be from the same tree as us - jpcs

		// Order them according to the address of their roots
		return FunctionRoot::root(this, context).get() < FunctionRoot::root(other, context).get();
	}
	return node_ < otherImpl->getDOMNode();
}

ATQNameOrDerived::Ptr DbXmlNodeImpl::dmNodeName(const DynamicContext* context) const
{  
	if(node_ == 0) return 0;

	switch(node_->getNodeType())
	{
	case DOMNode::ATTRIBUTE_NODE:
	case DOMNode::ELEMENT_NODE:
		return context->getPathanFactory()->createQName(node_->getNamespaceURI(),
			node_->getPrefix(), node_->getLocalName(), context);
	case DOMNode::PROCESSING_INSTRUCTION_NODE:
		return context->getPathanFactory()->createQName(XMLUni::fgZeroLenString,
			XMLUni::fgZeroLenString, node_->getNodeName(), context);
	case DOMXPathNamespace::XPATH_NAMESPACE_NODE: {
		const XMLCh* prefix = node_->getPrefix();
		if(prefix)
			return context->getPathanFactory()->createQName(XMLUni::fgZeroLenString,
				XMLUni::fgZeroLenString, prefix, context);
        }
	}
	return 0;
}

ATQNameOrDerived::Ptr DbXmlNamespaceNodeImpl::dmNodeName(const DynamicContext* context) const
{  
	if(prefix_)
		return context->getPathanFactory()->createQName(XMLUni::fgZeroLenString,
			XMLUni::fgZeroLenString, prefix_, context);
	return 0;
}

const XMLCh* DbXmlNodeImpl::dmStringValue(const DynamicContext* context) const
{
	MATERIALISE_NODE;

	// We don't currently store type information
	
	switch(node_->getNodeType()) {
	case DOMNode::DOCUMENT_NODE:
	case DOMNode::ELEMENT_NODE:
	case DOMNode::ATTRIBUTE_NODE:
	case DOMNode::PROCESSING_INSTRUCTION_NODE:
	case DOMNode::COMMENT_NODE:
	case DOMNode::TEXT_NODE:
	case DOMNode::CDATA_SECTION_NODE:
	case DOMXPathNamespace::XPATH_NAMESPACE_NODE: {
		return context->getMemoryManager()->getPooledString(node_->getTextContent());
	}

	}
	return XMLUni::fgZeroLenString;
}

const XMLCh* DbXmlNamespaceNodeImpl::dmStringValue(const DynamicContext* context) const
{
	return uri_;
}

Sequence DbXmlNodeImpl::dmTypedValue(const DynamicContext* context) const
{
	MATERIALISE_NODE;

	switch(node_->getNodeType()) {
	case DOMNode::ATTRIBUTE_NODE:
	case DOMNode::ELEMENT_NODE:
	case DOMNode::DOCUMENT_NODE:
	case DOMNode::TEXT_NODE:
	case DOMNode::CDATA_SECTION_NODE: {
		return Sequence(context->getPathanFactory()->createUntypedAtomic(dmStringValue(context), context), context->getMemoryManager()); 
        }
	case DOMXPathNamespace::XPATH_NAMESPACE_NODE:
	case DOMNode::COMMENT_NODE:
	case DOMNode::PROCESSING_INSTRUCTION_NODE: {
		return Sequence(context->getPathanFactory()->createString(dmStringValue(context), context), context->getMemoryManager());
        }
    }
    return Sequence(context->getMemoryManager());
}

Sequence DbXmlNamespaceNodeImpl::dmTypedValue(const DynamicContext* context) const
{
	return Sequence(context->getPathanFactory()->createString(uri_, context), context->getMemoryManager());
}

Node::Ptr DbXmlNodeImpl::dmParent(const DynamicContext* context) const
{
	if(node_ == 0) return 0;

	DOMNode *parent = XPath2NSUtils::getParent(node_);

	// Skip out of entity reference nodes
	while(parent != 0 && parent->getNodeType() == DOMNode::ENTITY_REFERENCE_NODE) {
		parent = parent->getParentNode();
	}

	if(parent == 0) return 0;

	return ((DbXmlFactoryImpl*)context->getPathanFactory())->createNode(parent, document_, context);
}

Node::Ptr DbXmlNamespaceNodeImpl::dmParent(const DynamicContext* context) const
{
	if(parent_ == 0) return 0;

	return ((DbXmlFactoryImpl*)context->getPathanFactory())->createNode(fakeDynamicCastDOMNode(parent_), document_, context);
}

Result DbXmlNodeImpl::dmAttributes(const DynamicContext* context) const
{
	if(node_ == 0) return 0;

	if(node_->getNodeType() == DOMNode::ELEMENT_NODE) {
		return new DbXmlAttributeAxis(this, NULL, const_cast<DynamicContext*>(context));
	}
	return 0;
}

Result DbXmlNodeImpl::dmNamespaceNodes(const DynamicContext* context) const
{
	if(node_ == 0) return 0;

	if(node_->getNodeType() == DOMNode::ELEMENT_NODE) {
		return new DbXmlNamespaceAxis(this, NULL, const_cast<DynamicContext*>(context));
	}
	return 0;
}

Result DbXmlNodeImpl::dmChildren(const DynamicContext *context) const
{
	if(node_ == 0 || node_->getNodeType() == DOMNode::ELEMENT_NODE || node_->getNodeType() == DOMNode::DOCUMENT_NODE) {
		return new DbXmlChildAxis(this, NULL, const_cast<DynamicContext*>(context));
	}
	return 0;
}

Result DbXmlNodeImpl::getAxisResult(DataItemStep::Axis axis, const NodeTest *nt, const DynamicContext *context) const
{
	DBXML_ASSERT(nt->getInterface(DbXmlNodeTest::gDbXml) != 0);
	const DbXmlNodeTest *nodeTest = (const DbXmlNodeTest*)nt;

	switch(axis) {
	case DataItemStep::ANCESTOR: {
		MATERIALISE_NODE;
		return new AncestorAxis(node_, this, nodeTest, const_cast<DynamicContext*>(context), *this);
	}
	case DataItemStep::ANCESTOR_OR_SELF: {
		MATERIALISE_NODE;
		return new AncestorOrSelfAxis(node_, this, nodeTest, const_cast<DynamicContext*>(context), *this);
	}
	case DataItemStep::ATTRIBUTE: {
		if(node_ != 0 && node_->getNodeType() == DOMNode::ELEMENT_NODE) {
			return new DbXmlAttributeAxis(this, nodeTest, const_cast<DynamicContext*>(context));
		}
		break;
	}
	case DataItemStep::CHILD: {
		if(node_ == 0 || node_->getNodeType() == DOMNode::ELEMENT_NODE || node_->getNodeType() == DOMNode::DOCUMENT_NODE) {
			if(nodeTest->getItemType() == 0 && (nodeTest->getHasChildren() ||
				   (!nodeTest->getTypeWildcard() && nodeTest->getNodeType() == Node::element_string))) {
				return new ElementChildAxis(this, nodeTest, const_cast<DynamicContext*>(context));
			}
			return new DbXmlChildAxis(this, nodeTest, const_cast<DynamicContext*>(context));
		}
		break;
	}
	case DataItemStep::DESCENDANT: {
		if(node_ == 0 || node_->getNodeType() == DOMNode::ELEMENT_NODE || node_->getNodeType() == DOMNode::DOCUMENT_NODE) {
			if(nodeTest->getItemType() == 0 && (nodeTest->getHasChildren() ||
				   (!nodeTest->getTypeWildcard() && nodeTest->getNodeType() == Node::element_string))) {
				return new ElementDescendantAxis(this, nodeTest, const_cast<DynamicContext*>(context));
			}
			return new DbXmlDescendantAxis(this, nodeTest, const_cast<DynamicContext*>(context));
		}
		break;
	}
	case DataItemStep::DESCENDANT_OR_SELF: {
		if(nodeTest->getItemType() == 0 && (nodeTest->getHasChildren() ||
			   (!nodeTest->getTypeWildcard() && nodeTest->getNodeType() == Node::element_string))) {
			return new ElementDescendantOrSelfAxis(this, nodeTest, const_cast<DynamicContext*>(context));
		}
		return new DbXmlDescendantOrSelfAxis(this, nodeTest, const_cast<DynamicContext*>(context));
	}
	case DataItemStep::FOLLOWING: {
		MATERIALISE_NODE;
		return new FollowingAxis(node_, this, nodeTest, const_cast<DynamicContext*>(context), *this);
	}
	case DataItemStep::FOLLOWING_SIBLING: {
		MATERIALISE_NODE;
		return new FollowingSiblingAxis(node_, this, nodeTest, const_cast<DynamicContext*>(context), *this);
	}
	case DataItemStep::NAMESPACE: {
		if(node_ != 0 && node_->getNodeType() == DOMNode::ELEMENT_NODE) {
			return new DbXmlNamespaceAxis(this, nodeTest, const_cast<DynamicContext*>(context));
		}
		break;
	}
	case DataItemStep::PARENT: {
		return new DbXmlParentAxis(this, nodeTest, const_cast<DynamicContext*>(context));
	}
	case DataItemStep::PRECEDING: {
		MATERIALISE_NODE;
		return new PrecedingAxis(node_, this, nodeTest, const_cast<DynamicContext*>(context), *this);
	}
	case DataItemStep::PRECEDING_SIBLING: {
		MATERIALISE_NODE;
		return new PrecedingSiblingAxis(node_, this, nodeTest, const_cast<DynamicContext*>(context), *this);
	}
	case DataItemStep::SELF: {
		return nodeTest->filterResult(new SelfAxis(this, const_cast<DynamicContext*>(context)),
			const_cast<DynamicContext*>(context));
	}
	}

	return 0;
}




DbXmlAxis::DbXmlAxis(const DbXmlNodeImpl *contextNode, const DbXmlNodeTest *nodeTest,
	DynamicContext *context)
	: ResultImpl(context),
	  nodeObj_(contextNode),
	  contextNode_(const_cast<NsDomNode*>(fakeDynamicCastNsDomNode(contextNode->getDOMNode()))),
	  nodeTest_(nodeTest),
	  toDo_(true)
{
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
	DbXmlFactoryImpl *factory = (DbXmlFactoryImpl*)context->getPathanFactory();

	const NsDomNode *node = 0;
	while((node = nextNode()) != 0)
	{
		if(nodeTest_ == 0) return factory->createNode(fakeDynamicCastDOMNode(node), nodeObj_->getXmlDocument(), context);

		SequenceType::ItemType *itemType = nodeTest_->getItemType();
		if(itemType != 0)
		{
			Node::Ptr result = factory->createNode(fakeDynamicCastDOMNode(node), nodeObj_->getXmlDocument(), context);
			if(itemType->matches(result, context)) {
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
					const XMLCh *uri = nodeTest_->getNodeUri();
					if(uri == 0 && context->getDefaultElementAndTypeNS() != 0) {
						if(!XPath2Utils::equals(node->getNsUri(),
							   context->getDefaultElementAndTypeNS())) continue;
					} else {
						if(node->isNativeUTF8()) {
							if(!equals(node->getNsUri8(), nodeTest_->getNodeUri8())) continue;
						} else {
							if(!XPath2Utils::equals(node->getNsUri(), uri)) continue;
						}
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
			}
			default: {
				continue;
			}
			}

			return factory->createNode(fakeDynamicCastDOMNode(node), nodeObj_->getXmlDocument(), context);
		}
	}

	return 0;
}




DbXmlAttributeAxis::DbXmlAttributeAxis(const DbXmlNodeImpl *contextNode, const DbXmlNodeTest *nodeTest,
	DynamicContext *context)
	: DbXmlAxis(contextNode, nodeTest, context),
	  nodeMap_(0),
	  i_(0)
{
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

DbXmlNamespaceAxis::DbXmlNamespaceAxis(const DbXmlNodeImpl *contextNode, const DbXmlNodeTest *nodeTest,
	DynamicContext *context)
	: ResultImpl(context),
	  nodeObj_(contextNode),
	  contextNode_(const_cast<NsDomNode*>(fakeDynamicCastNsDomNode(contextNode->getDOMNode()))),
	  nodeTest_(nodeTest),
	  toDo_(true),
	  node_((contextNode_->getNsNodeType() == nsNodeElement) ? ((NsDomElement*)contextNode_) : 0),
	  nodeMap_((node_ == 0) ? 0 : node_->getNsAttributes()),
	  i_(0)
{
}

Item::Ptr DbXmlNamespaceAxis::next(DynamicContext *context)
{
	DbXmlNamespaceNodeImpl::Ptr node;
	while((node = nextNode(context)) != 0) {
		if(nodeTest_ == 0) return node;

		SequenceType::ItemType *itemType = nodeTest_->getItemType();
		if(itemType != 0) {
			if(itemType->matches((Node::Ptr)node, context)) {
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
				continue;
			}
			else {
				nodeMap_ = 0;
				break;
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

DbXmlChildAxis::DbXmlChildAxis(const DbXmlNodeImpl *contextNode, const DbXmlNodeTest *nodeTest,
	DynamicContext *context)
	: DbXmlAxis(contextNode, nodeTest, context),
	  child_(0)
{
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


ElementChildAxis::ElementChildAxis(const DbXmlNodeImpl *contextNode, const DbXmlNodeTest *nodeTest,
	DynamicContext *context)
	: DbXmlAxis(contextNode, nodeTest, context),
	  child_(0)
{
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



DbXmlDescendantAxis::DbXmlDescendantAxis(const DbXmlNodeImpl *contextNode, const DbXmlNodeTest *nodeTest, DynamicContext *context)
	: DbXmlAxis(contextNode, nodeTest, context),
	  descendant_(0)
{
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
				if(descendant_ == contextNode_ || descendant_ == 0) break;
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


ElementDescendantAxis::ElementDescendantAxis(const DbXmlNodeImpl *contextNode, const DbXmlNodeTest *nodeTest, DynamicContext *context)
	: DbXmlAxis(contextNode, nodeTest, context),
	  nscontext_(0),
	  descendant_(0)
{
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
				if(descendant_ == nscontext_ || descendant_ == 0) break;
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



DbXmlDescendantOrSelfAxis::DbXmlDescendantOrSelfAxis(const DbXmlNodeImpl *contextNode, const DbXmlNodeTest *nodeTest,
	DynamicContext *context)
	: DbXmlAxis(contextNode, nodeTest, context),
	  descendant_(0)
{
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

		while(result == 0) {
			result = descendant_->getNsNextSibling();
			if(result == 0) {
				descendant_ = descendant_->getNsParentNode();
				if(descendant_ == contextNode_ || descendant_ == 0) break;
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


ElementDescendantOrSelfAxis::ElementDescendantOrSelfAxis(const DbXmlNodeImpl *contextNode, const DbXmlNodeTest *nodeTest,
	DynamicContext *context)
	: DbXmlAxis(contextNode, nodeTest, context),
	  nscontext_(0),
	  descendant_(0)
{
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

		while ((result == 0) && (descendant_ != nscontext_)){
			result = descendant_->getElemNext();
			if(result == 0) {
				descendant_ = descendant_->getElemParent();
				if(descendant_ == nscontext_ || descendant_ == 0) break;
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


DbXmlParentAxis::DbXmlParentAxis(const DbXmlNodeImpl *contextNode, const DbXmlNodeTest *nodeTest,
	DynamicContext *context)
	: DbXmlAxis(contextNode, nodeTest, context)
{
}

const NsDomNode *DbXmlParentAxis::nextNode()
{
	if(toDo_) {
		// initialise
		toDo_ = false;
		return contextNode_->getNsParentNode();
	}
	return 0;
}

std::string DbXmlParentAxis::asString(DynamicContext *context, int indent) const
{
	return "DbXmlParentAxis";
}


