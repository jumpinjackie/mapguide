//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: NsDom.cpp,v 1.80.2.2 2007/01/09 18:52:17 gmf Exp $
//

#include "NsUtil.hpp"
#include "NsDocument.hpp"
#include "NsConstants.hpp"
#include "NsDom.hpp"
#include "../UTF8.hpp" // for XMLChToUTF8
#include <xercesc/framework/MemoryManager.hpp>
#include <xercesc/util/XMLUri.hpp>

using namespace DbXml;
XERCES_CPP_NAMESPACE_USE;

static xmlch_t xmlchColon = ':';
static xmlch_t _nsDomDocName[] = { '#', 'd', 'o', 'c', 'u',
				'm', 'e', 'n', 't', 0 };
static xmlch_t _nsDomTextName[] = { '#', 't', 'e', 'x', 't', 0 };
static xmlch_t _nsDomCommentName[] = { '#', 'c', 'o', 'm',
				       'm', 'e', 'n', 't', 0 };
static xmlch_t _nsDomCdataName[] = { '#', 'c', 'd', 'a', 't', 'a', '-',
				       's', 'e', 'c', 't', 'i', 'o', 'n', 0 };

static void NsDomErrNotFound(const NsNid *nid, int line)
{
	NsUtil::nsThrowException(XmlException::INTERNAL_ERROR,
				 "Cannot find persistent node",
				 __FILE__, line);
}

static void setString16(NsDomString &ds,
			XER_NS MemoryManager *mmgr,
			NsDocument *doc,
			uint32_t id)
{
	ds.set16(mmgr,
		 NsUtil::nsStringDup(mmgr,
				     doc->getStringForID16(id), 0),
		 true);
}

static void setString(NsDomString &ds,
		      XER_NS MemoryManager *mmgr,
		      NsDocument *doc,
		      uint32_t id)
{
	ds.set(mmgr,
	       NsUtil::nsStringDup(mmgr,
				   (const xmlbyte_t *)doc->getStringForID(id),
				   0),
	       true);
}

void
NsDomString::clear(XER_NS MemoryManager *mmgr)
{
	if(text_ && isOwned())
		mmgr->deallocate((void*)text_);
	text_ = 0;
	flags_ = 0;
}

//
// NsDomFactory implementation
//
NsDomElement *
NsDomFactory::createNsDomElement(NsDocument *doc, const xmlch_t *uri,
				 const xmlch_t *qname)
{
	NsNode *node = NsNode::allocNode(doc->getMemoryManager(),
					 0, NS_STANDALONE);
	const xmlch_t *lname = qname;
	if(uri != 0) {
		// Set the uri
		XMLChToUTF8Null xcode(uri);
		int uriId = doc->addIDForString(xcode.str(), xcode.len());
		node->setUriIndex(uriId);
		node->setFlag(NS_HASURI);

		// Find the localname
		while(*lname != 0 && *(lname++) != ':');
		if(*lname == 0) {
			lname = qname;
			node->setNamePrefix(NS_NOPREFIX);
		}
		else {
			// Copy the prefix
			unsigned int plen = lname - qname;
			xmlch_t *prefix = (xmlch_t*)doc->getMemoryManager()->
				allocate(plen * sizeof(xmlch_t));
			plen -= 1;
			memcpy(prefix, qname, plen * sizeof(xmlch_t));
			prefix[plen] = 0;

			// Set the prefix
			XMLChToUTF8Null xcode(prefix);
			int prefixId =
				doc->addIDForString(xcode.str(), xcode.len());
			node->setNamePrefix(prefixId);
			node->setFlag(NS_NAMEPREFIX);

			doc->getMemoryManager()->deallocate(prefix);
		}
	}
	else {
		node->setNamePrefix(NS_NOPREFIX);
	}

	// Set the localname
	xmlbyte_t *name8 = 0;
	int nlen8 = NsUtil::nsToUTF8(doc->getMemoryManager(), &name8,
				     lname, NsUtil::nsStringLen(lname) + 1, 0);
	NsNode::createText(doc->getMemoryManager(),
			   node->getNameText(), name8,
			   nlen8 - 1, /*isUTF8*/true, /*isDonated*/true);

	return createNsDomElement(doc, node, 0, true);
}

void
NsDomFactory::addToDomFreeList(NsDomObj *dnode)
{
	dnode->setNextObj(_domFreeList);
	if(_domFreeList)
		_domFreeList->setPrevObj(dnode);
	_domFreeList = dnode;
}

void
NsDomFactory::removeFromDomFreeList(NsDomObj *dnode)
{
	if(dnode->getNextObj()) {
		dnode->getNextObj()->setPrevObj(dnode->getPrevObj());
	}

	if(dnode->getPrevObj()) {
		dnode->getPrevObj()->setNextObj(dnode->getNextObj());
	}
	else {
		DBXML_ASSERT(_domFreeList = dnode);
		_domFreeList = dnode->getNextObj();
	}
}

//
// NsDomNav implementation
//
NsDomNav::NsDomNav(NsDomElement *parent) :
	NsDomNode(), _parent(parent),
	_prevSib(0), _nextSib(0)
{

}

const xmlch_t *
NsDomNav::getNsBaseUri() const
{
	if(_parent) return _parent->getNsBaseUri();
	return 0;
}


//
// NsDomElement implementation
//
NsDomElement::NsDomElement(NsDocument *document,
			   NsNode *node,
			   NsDomElement *parent,
			   bool standalone) :
	NsDomNav(parent), _document(document), _node(node),
	_firstChild(0), _lastChild(0),
	_childNodeList(0), _attributes(0), _lname(0),
	_nsElemFirstChild(0), _nsElemLastChild(0),
	_nsElemPrev(0), _nsElemNext(0)
{
	if (isDocumentNode()) {
		_qname.set16(getMemoryManager(), _nsDomDocName, false);
		_lname = _qname.get16();
	}
}

void
NsDomElement::destroy()
{
	MemoryManager *mgr = getMemoryManager();
	_qname.clear(mgr);
	_textContent.clear(mgr);
	_baseUri.clear(mgr);
	if (_node) {
		NsNode::freeNode(mgr,
				 _node);
	}
	_uri.clear(mgr);
	_prefix.clear(mgr);
}

const xmlch_t *
NsDomElement::getNsBaseUri() const
{
	static const xmlch_t xmlBaseString[] = { 'x', 'm', 'l', ':', 'b', 'a', 's', 'e', 0 };

	if(getNsNodeType() == nsNodeDocument) {
		return _document->getDocumentUri();
	}

	_baseUri.clear(_document->getMemoryManager());

	const xmlch_t *baseURI = 0;
	if(const_cast<NsDomElement*>(this)->getElemParent() != 0) {
		baseURI = const_cast<NsDomElement*>(this)->getElemParent()->getNsBaseUri();
	}
	else if(const_cast<NsDomElement*>(this)->getNsDocumentObj() != 0) {
		baseURI = const_cast<NsDomElement*>(this)->getNsDocumentObj()->getNsBaseUri();
	}

	if(hasNsAttributes()) {
		NsDomAttr *attrNode = const_cast<NsDomElement*>(this)->getNsAttributes()->getNsNamedItem(xmlBaseString);
		if(attrNode) {
			const xmlch_t *uri = attrNode->getNsNodeValue();
			if(uri && *uri && baseURI && *baseURI) {
				XMLUri temp(baseURI, _document->getMemoryManager());
				XMLUri temp2(&temp, uri, _document->getMemoryManager());
				_baseUri.set16(getMemoryManager(),
					       NsUtil::nsStringDup(_document->getMemoryManager(),
								   temp2.getUriText(), 0),
					       true);
				return _baseUri.get16();
			}
			baseURI = uri;
		}
	}
	_baseUri.set16(getMemoryManager(),
		       NsUtil::nsStringDup(_document->getMemoryManager(),
					   baseURI, 0),
		       true);
	return _baseUri.get16();
}

void
NsDomElement::_getName() const
{
	bool owned = false;
	const xmlch_t *qname = _document->getQname(_node->getName(),
						   _node->isUTF16(),
						   owned);
	DBXML_ASSERT(qname);
	_qname.set16(getMemoryManager(), qname, owned);
	_lname = qname;
	if (_node->namePrefix() != NS_NOPREFIX)
		while (*_lname++ != xmlchColon);
}

bool
NsDomElement::isNativeUTF8() const
{
	return !_node->isUTF16();
}

const xmlbyte_t *
NsDomElement::getNsNodeName8() const
{
	NsUtil::nsThrowException(XmlException::DOM_PARSER_ERROR,
				"NsDomElement::getNsNodeName8 not implemented",
				 __FILE__, __LINE__);
	return 0;
}

const xmlbyte_t *
NsDomElement::getNsLocalName8() const
{
	if(_node->isUTF16()) return 0;
	return (const xmlbyte_t*)_node->getName()->n_text.t_chars;
}

const xmlbyte_t *
NsDomElement::getNsPrefix8() const
{
	if (isDocumentNode() || _node->namePrefix() == NS_NOPREFIX)
		return 0;

	if (!_prefix.get()) {
		setString(_prefix, getMemoryManager(), _document,
			  _node->namePrefix());
	}
	return _prefix.get();
}

const xmlbyte_t *
NsDomElement::getNsUri8() const
{
	if (isDocumentNode() || !_node->hasUri())
		return 0;
	if (!_uri.get()) {
		setString(_uri, getMemoryManager(), _document,
			  _node->uriIndex());
	}
	return _uri.get();

}

const xmlbyte_t *
NsDomElement::getNsNodeValue8() const
{
	return 0;
}

// nodeName is the qname (vs local part), so need to address
// namespaces.
const xmlch_t *
NsDomElement::getNsNodeName() const
{
	if (!_qname.get16()) {
		_getName();
		DBXML_ASSERT(_qname.get16() && _lname);
	}
	return _qname.get16();
}

void
NsDomElement::nsRename(const xmlch_t *uri, const xmlch_t *qname)
{
	nsMakeTransient();

	const xmlch_t *lname = qname;
	if(uri != 0) {
		// Set the uri
		XMLChToUTF8Null xcode(uri);
		int uriId = getNsDocument()->
			addIDForString(xcode.str(), xcode.len());
		_node->setUriIndex(uriId);
		_node->setFlag(NS_HASURI);

		// Find the localname
		while(*lname != 0 && *(lname++) != ':');
		if(*lname == 0) {
			lname = qname;
			_node->clearFlag(NS_NAMEPREFIX);
			_node->setNamePrefix(NS_NOPREFIX);
		}
		else {
			// Copy the prefix
			unsigned int plen = lname - qname;
			xmlch_t *prefix = (xmlch_t*)getMemoryManager()->
				allocate(plen * sizeof(xmlch_t));
			plen -= 1;
			memcpy(prefix, qname, plen * sizeof(xmlch_t));
			prefix[plen] = 0;

			// Set the prefix
			XMLChToUTF8Null xcode(prefix);
			int prefixId =
				getNsDocument()->addIDForString(
					xcode.str(), xcode.len());
			_node->setNamePrefix(prefixId);
			_node->setFlag(NS_NAMEPREFIX);

			getMemoryManager()->deallocate(prefix);
		}
	}
	else {
		_node->clearFlag(NS_HASURI);
		_node->clearFlag(NS_NAMEPREFIX);
		_node->setNamePrefix(NS_NOPREFIX);
	}

	// Set the localname
	getMemoryManager()->
		deallocate(_node->getNameText()->t_chars);

	if(_node->isUTF16()) {
		NsNode::createText(getMemoryManager(),
				   _node->getNameText(), lname,
				   NsUtil::nsStringLen(lname),
				   /*isUTF8*/false, /*isDonated*/false);
	}
	else {
		xmlbyte_t *name8 = 0;
		int nlen8 = NsUtil::nsToUTF8(
			getMemoryManager(), &name8,
			lname, NsUtil::nsStringLen(lname) + 1, 0);
		NsNode::createText(getMemoryManager(),
				   _node->getNameText(), name8,
				   nlen8 - 1,
				   /*isUTF8*/true, /*isDonated*/true);
	}

	// clear the cached name
	_qname.clear(getMemoryManager());

	_document->addToModifications(NodeModification::UPDATE, this);
}

NsDomNav *
NsDomElement::removeNsChild(NsDomNode *child)
{
	if(child == 0 || child->getNsParentNode() != this)
		return 0;

	switch(child->getNsNodeType()) {
	case nsNodeElement: {
		return _removeNsElement((NsDomElement*)child);
	}
	case nsNodeText: {
		// text nodes belong to their owner, which
		// may not be "this"
		NsDomText *tchild = (NsDomText*)child;
		return tchild->getOwner()->_removeNsText(tchild);
	}
	case nsNodeEntStart:
	case nsNodeEntEnd:
		// Don't want to remove entity start or end
		return 0;
	case nsNodeDocument:
	case nsNodeAttr:
		DBXML_ASSERT(false);
	}

	return 0;
}

NsDomText *
NsDomElement::_removeNsText(NsDomText *child)
{
	// this node is either the parent or simply
	// the owning element of the text, where the
	// text is a leading text node.

	// detach this node from the document
	nsMakeTransient();

	// prev/next links for text are purely transient,
	// so these changes don't affect the persistent state
	// of the nodes involved, other than this one.

	// Some of this plumbing is common to both leading text and
	// direct child text
	if (child->getNsNextSibling()) {
		child->getNsNextSibling()->
			setNsPrevSib(child->getNsPrevSibling());
	} else {
		// No next sibling, so it must be the last child, and
		// a direct child node
		_lastChild = child->getNsPrevSibling();
	}

	if (child->getNsPrevSibling()) {
		child->getNsPrevSibling()->
			setNsNextSib(child->getNsNextSibling());
	} else {
		// No previous sibling, so it must be the first child,
		// but it may be either a leading text or direct child, so
		// get its parent (which may be "this").
		NsDomElement *parent = child->getNsParentNode();
		parent->_firstChild = child->getNsNextSibling();
	}

	// grab index before it's cleared
	int32_t index = child->getIndex();

	// Renumber/index all text siblings to the "right"
	// This works for leading and child text
	NsDomNav *next = child->getNsNextSibling();
	while (next && (next->getNsNodeType() == nsNodeText)) {
		int32_t newindex = ((NsDomText*)next)->getIndex();
		((NsDomText*)next)->setIndex(newindex - 1);
		next = next->getNsNextSibling();
	}
	
	// Detach the node from its surroundings
	child->makeStandalone();

	// Detach node from its owner (this)
	_node->removeText(getMemoryManager(), index);

	// Add to modification list for persistence
	getNsDocument()->addToModifications(NodeModification::UPDATE, this);
	return child;
}

NsDomElement *
NsDomElement::_removeNsElement(NsDomElement *child)
{
	nsMakeTransient();

	// Update:
	//  o next/prev links (elements and nav)
	//  o leading text gets move to either next element sibling,
	//    if present, or parent (child text)
	//  o if this was last child, update last descendant NID up
	//    the ancestor chain
	//  Node updates apply to:
	//    child -- removed
	//    next/prev elements
	//    this, but only if child is first or last
	//    ancestors -- only if last child
	
	// Relink the child list,and parent first/last
	// child pointers
	if (child->getNsNextSibling()) {
		child->getNsNextSibling()->
			setNsPrevSib(child->getNsPrevSibling());
	} else {
		// No next sibling, so it must be the last child
		_lastChild = child->getNsPrevSibling();
	}

	if (child->getNsPrevSibling()) {
		child->getNsPrevSibling()->
			setNsNextSib(child->getNsNextSibling());
	} else {
		// No previous sibling, so it must be the first child
		_firstChild = child->getNsNextSibling();
	}

	// Modify the element's sibling elements to point to the correct
	// next and previous NsNid
	NsDomElement *next = child->getElemNext();
	NsDomElement *prev = child->getElemPrev();
	MemoryManager *mmgr = getMemoryManager();

	// make next/prev mutable before they are modified
	if (next)
		next->nsMakeTransient();
	if (prev)
		prev->nsMakeTransient();

	// Move text, if present.  Just copy; explicit
	// remove isn't required, as the node's going away
	NsNode *node = child->getNsNode();
	if (node->hasLeadingText()) {
		if (next) {
			_coalesceTextNodes(child, next, false);
		} else {
			_coalesceTextNodes(child, this, true);
		}
	}

	if (next) {
		next->setElemPrev(prev);
		if (prev)
			next->getNsNode()->setNextPrev(
				mmgr, prev->getNsNode());
		else
			next->getNsNode()->clearPrev(mmgr);
		getNsDocument()->addToModifications(
			NodeModification::UPDATE, next);
	} else {
		// It was the last element child
		_nsElemLastChild = prev;
		NsNode *pnode = (prev ? prev->getNsNode() : 0);
		getNsNode()->setLastChild(mmgr, pnode);
		const NsNid *lastNid;
		if (prev)
			lastNid = prev->getLastDescendantNid();
		else
			lastNid = getNodeId();
		// if "this" is now a leaf, it'll be updated
		// anyway, and it *should* be ignored by marshaling code
		updateLastDescendants(lastNid);
	}

	if (prev) {
		prev->setElemNext(next);
		if (!next)
			prev->getNsNode()->clearNext(mmgr);
		// if both prev and next, they are linked, above
		getNsDocument()->addToModifications(
			NodeModification::UPDATE, prev);
	} else {
		// It was the first element child
		_nsElemFirstChild = next;
	}

	if (!prev || !next)
		getNsDocument()->addToModifications(
			NodeModification::UPDATE, this);

	// Remove the child from the tree
	child->_makeStandalone();
	child->_parent = 0;
	child->_prevSib = 0;
	child->_nextSib = 0;
	child->_nsElemPrev = 0;
	child->_nsElemNext = 0;


	return child;
}

NsDomNav *
NsDomElement::insertNsChild(NsDomNode *child, NsDomNode *refChild,
			    NsDomInsertType itype)
{
	if(child == 0 || child->getNsParentNode() != 0)
		return 0;

	DBXML_ASSERT(refChild == 0 ||
		  refChild->getNsNodeType() == nsNodeElement ||
		  refChild->getNsNodeType() == nsNodeText ||
		  refChild->getNsNodeType() == nsNodeEntStart ||
		  refChild->getNsNodeType() == nsNodeEntEnd);

	switch(child->getNsNodeType()) {
	case nsNodeElement: {
		return _insertNsElement((NsDomElement*)child,
					(NsDomNav*)refChild, itype);
	}
	case nsNodeText: {
		return _insertNsText((NsDomText*)child, (NsDomNav*)refChild);
	}
	case nsNodeEntStart:
	case nsNodeEntEnd:
		// Don't want to append entity start or end
		return 0;
	case nsNodeDocument:
	case nsNodeAttr:
		DBXML_ASSERT(false);
	}

	return 0;
}

NsDomText *
NsDomElement::_insertNsText(NsDomText *child, NsDomNav *nextChild)
{
	nsMakeTransient();

	// Modification may apply to one of 3 nodes:
	// o this node (if adding a text child)
	// o nextChild (if it's an element)
	// o nextChild's owner (if nextChild is text)
	NsDomElement *modified = 0;
	int32_t index = -1; // -1 means append as last on list
	bool isChild = false;
	NsDomNav *prev = 0;
	
	// if no next, operation is appending child text
	if (!nextChild) {
		modified = this;
		isChild = true;
		prev = getNsLastChild(true);
		index = _node->getNumText();
	} else {
		prev = nextChild->getNsPrevSibling();
		// if next is text, and a child, then inserting child text,
		// otherwise, it's leading text
		if (nextChild->getNsNodeType() == nsNodeText) {
			NsDomText *tchild = (NsDomText*)nextChild;
			index = tchild->getIndex();
			if (tchild->getNsNode()->isTextChild(index)) {
				modified = this;
				isChild = true;
			} else
				modified = tchild->getOwner();
		} else {
			DBXML_ASSERT(nextChild->getNsNodeType() ==
				     nsNodeElement);
			modified = (NsDomElement*)nextChild;
			index = modified->getNsNode()->getNumLeadingText();
		}
	}

	DBXML_ASSERT(modified);
	if (modified != this) // already done if "this"
		modified->nsMakeTransient();
	NsNode *mnode = modified->getNsNode();
	// insert the new text
	if (nsTextType(child->getNsTextType()) == NS_PINST) {
		mnode->insertPI(getMemoryManager(),
				index, child->getNsNodeName(),
				child->getNsNodeValue(), isChild);
	} else {
		mnode->insertText(getMemoryManager(),
				  index, child->getNsNodeValue(),
				  child->getNsTextType(), isChild);
	}

	// Relink the child list,and parent first/last
	// child pointers
	child->makeNonStandalone(this, modified, index, prev, nextChild);
	if (prev == 0)
		_firstChild = child;
	else
		prev->setNsNextSib(child);
	if (nextChild == 0)
		_lastChild = child;
	else
		nextChild->setNsPrevSib(child);

	// renumber text siblings to the right, but only if
	// they have the same owner ('modified')
	NsDomNav *next = getNsNextSibling();
	while (next && (next->getNsNodeType() == nsNodeText)) {
		if (((NsDomText*)next)->getOwner() != modified)
			break;
		int32_t newindex = ((NsDomText*)next)->getIndex();
		((NsDomText*)next)->setIndex(newindex + 1);
		next = next->getNsNextSibling();
	}
	
	getNsDocument()->addToModifications(NodeModification::UPDATE, modified);
	return child;
}

NsDomElement *
NsDomElement::_insertNsElement(NsDomElement *child, NsDomNav *nextNode,
			       NsDomInsertType itype)
{
	// Modification affects these nodes:
	//  o parent/this -- if inserting a first or last child
	//  o all ancestors -- if inserting a last child/descendant
	//  o next/prev -- always, if they exist

	nsMakeTransient();
	NsDomElement *previous = 0;
	NsDomElement *next = 0;
	NsDomNav *prevNav = 0;
	NsDomText *tnext = 0;
	// find next/prev elements
	if(nextNode) {
		prevNav = nextNode->getNsPrevSibling();
		if (nextNode->getNsNodeType() == nsNodeElement) {
			next = (NsDomElement*)nextNode;
			previous = next->getElemPrev();
		} else {
			DBXML_ASSERT(nextNode->getNsNodeType() == nsNodeText);
			// next is either the node's owner (leading text)
			// or null (child text)
			tnext = (NsDomText*)nextNode;
			if (tnext->getNsNode()->isTextChild(tnext->getIndex())) {
				if (prevNav->getNsNodeType() == nsNodeElement)
					previous = (NsDomElement*)prevNav;
				next = NULL;
			} else {
				next = tnext->getOwner();
				previous = next->getElemPrev();
			}
		}
	} else {
		// appending as last child -- no next
		previous = getElemLastChild();
		prevNav = getNsLastChild(true);
		_lastChild = child;
	}
	if (!prevNav)
		_firstChild = child;

	// Make next/prev elements transient/mutable, before
	// moving text nodes around
	if (next)
		next->nsMakeTransient();
	if (previous)
		previous->nsMakeTransient();
	
	// text nodes need to be moved if prevNav is a text node.
	// Move it, and all text nodes "left" of it, to the child
	// If text is child text, only move child text nodes
	// This affects the owner element (either this or the next elem)
	if (prevNav && (prevNav->getNsNodeType() == nsNodeText))
		_moveTextNodes((NsDomText *)prevNav, child);

	// more basic plumbing
	child->setElemParent(this);
	_node->setHasChildElem();
	child->setElemPrev(previous);
	child->setElemNext(next);
	if (!previous)
		_nsElemFirstChild = child;
	if (!next)
		_nsElemLastChild = child;

	// find preceding and following NIDs for insertion of
	// new NIDs between them
	const NsNid *preceding = 0;
	const NsNid *following = 0;
	if (previous)
		preceding = previous->getLastDescendantNid();
	else
		preceding = getNodeId();

	if (next) {
		following = next->getNodeId();
	} else {
		NsDomElement *felem = this;
		while (felem && felem->getElemNext() == 0)
			felem = felem->getElemParent();
		if (felem)
			felem = felem->getElemNext();
		if (felem)
			following = felem->getNodeId();
	}

	child->setNsNextSib(nextNode);
	child->setNsPrevSib(prevNav);
	if (prevNav) {
		prevNav->setNsNextSib(child);
	}
	if (nextNode) {
		nextNode->setNsPrevSib(child);
	}

	child->_attachToTree(this, previous, next, preceding, following, itype);

	
	// decide which nodes have been updated, and add them to
	// the modifications list.  This is modified only if a first/last/only
	// child is added.  Prev/next are always modified.
	if (!previous || !next) {
		getNsDocument()->addToModifications(NodeModification::UPDATE,
						    this);
	}
	if (previous)
		getNsDocument()->addToModifications(NodeModification::UPDATE,
						    previous);
	if (next) {
		getNsDocument()->addToModifications(NodeModification::UPDATE,
						    next);
	} else if (!_node->isStandalone())
		updateLastDescendants(child->getLastDescendantNid());
	
	// walk the new tree, adding nodes to the modification list
	child->_addTreeToModifications(true);
		
	return child;
}

// stop updating last descendant if either at root,
// or this node has a next sibling element, which means
// that the parent's last descendant is in another tree
void
NsDomElement::updateLastDescendants(const NsNid *nid)
{
	DBXML_ASSERT(!nid->isNull());
	NsDomElement *current, *parent = this;
	do {
		current = parent;
		current->nsMakeTransient();
		current->setLastDescendantNid(nid);
		getNsDocument()->addToModifications(
			NodeModification::UPDATE,
			current);
		parent = current->getElemParent();
	} while (parent && !parent->isDocumentNode() &&
		 !(current->getNsNode()->hasNext()));
}


// Add the element, along with its descendants to
// the modification list
void
NsDomElement::_addTreeToModifications(bool first) {
	NsDomElement *current = this;
	while (current) {
		getNsDocument()->addToModifications(NodeModification::ADD,
						    current);
		if (current->hasNsChildElements())
			current->getElemFirstChild()->
				_addTreeToModifications(false);
		if (first)
			break; // don't add "root's" siblings
		current = current->getElemNext();
	}
}

// when inserting a new element, text node ownership changes.
// Move nodes starting with "start", from their
// owning element to the target.
// Target nodes will always be leading text, and the target
// must not already have it.  Source nodes may be leading or
// child.  In the case of child, just move the child nodes.
void
NsDomElement::_moveTextNodes(NsDomText *start, NsDomElement *target)
{
	NsNode *tnode = target->getNsNode();
	DBXML_ASSERT(tnode->getNumLeadingText() == 0);
	NsDomNav *ttext = 0;
	if (tnode->hasTextChild()) {
		// existing text on target will move "right."
		// get first text child for renumbering, later
		NsDomNav *last = target->getNsLastChild(true);
		while (last && last->getNsNodeType() == nsNodeText) {
			ttext = (NsDomText*)last;
			last = last->getNsPrevSibling();
		}
	}
	
	NsDomElement *owner = start->getOwner();
	NsNode *snode = owner->getNsNode();
	uint32_t endIndex = start->getIndex();
	uint32_t curIndex;
	if (snode->isTextChild(endIndex)) {
		curIndex = snode->getFirstTextChildIndex();
	} else
		curIndex = 0;
	uint32_t num = endIndex - curIndex + 1;

	// find real starting node (start parameter is really the end)
	NsDomText *current = start;
	NsDomNav *tmp = start->getNsPrevSibling();
	while (tmp && (tmp->getNsNodeType() == nsNodeText)) {
		current = (NsDomText *) tmp;
		tmp = tmp->getNsPrevSibling();
	}
	NsDomNav *end = start->getNsNextSibling();

	// insert, starting at new index 0 
	uint32_t newIndex = 0;
	MemoryManager *mmgr = getMemoryManager();

	// insert new nodes
	while (current != end) {
		DBXML_ASSERT(current->getNsNodeType() == nsNodeText);
		// add new node
		if (current->getNsTextType() == NS_PINST) {
			tnode->insertPI(mmgr, newIndex,
					current->getNsNodeName(),
					current->getNsNodeValue(),
					false /* is never child text */);
			
		} else {
			tnode->insertText(mmgr, newIndex,
					  current->getNsNodeValue(),
					  current->getNsTextType(),
					  false /* is never child text */);
		}
		// new owner
		current->setOwner(target);
		current->setIndex(newIndex++);
		current = (NsDomText *) current->getNsNextSibling();
		DBXML_ASSERT((current == end) ||
			     (current->getNsNodeType() == nsNodeText));
	}
	// rearrange siblings:
	// the next sibling for the "start" node is the target
	// set next/prev for target
	start->setNsNextSib(target);
	target->setNsNextSib(end);
	target->setNsPrevSib(start);
	if (end)
		end->setNsPrevSib(target);

	// remove old nodes.  Can't do this in loop above -- it
	// affects index values.  Remove from the same index location,
	// as each removal results in moving the remaining text nodes
	for (uint32_t i = 0; i < num; i++)
		snode->removeText(mmgr, curIndex);
	
	// renumber text siblings for source
	tmp = current;
	while (tmp && (tmp->getNsNodeType() == nsNodeText)) {
		int32_t newindex = ((NsDomText*)tmp)->getIndex();
		((NsDomText*)tmp)->setIndex(newindex - num);
		tmp = tmp->getNsNextSibling();
	}
	// renumber text siblings for target
	while (ttext && (ttext->getNsNodeType() == nsNodeText)) {
		int32_t newindex = ((NsDomText*)ttext)->getIndex();
		((NsDomText*)ttext)->setIndex(newindex + num);
		ttext = ttext->getNsNextSibling();
	}
}

// copy all leading text from source to target,
// pre-pending it.  If toChild is true, then it goes
// on the front end of the child text list; otherwise,
// it starts at index 0.  The source node is not modified.
void
NsDomElement::_coalesceTextNodes(NsDomElement *source,
				 NsDomElement *target,
				 bool toChild)
{
	NsNode *tnode = target->getNsNode();
	MemoryManager *mmgr = getMemoryManager();

	// Cannot count on sibling links to the source.
	// They have been eliminated in the caller.
	// Instead use a counter.  Previous sibling link
	// from source remains intact...
	int32_t num = source->getNsNode()->getNumLeadingText();
	DBXML_ASSERT(num != 0);
	NsDomNav *tmp = source->getNsPrevSibling();
	DBXML_ASSERT(tmp->getNsNodeType() == nsNodeText);
	NsDomText *first = 0;
	while (tmp &&  (tmp->getNsNodeType() == nsNodeText)) {
		first = (NsDomText*) tmp;
		tmp = tmp->getNsPrevSibling();
	}
	DBXML_ASSERT(first);
	int32_t index = 0;
	if (toChild) {
		index = tnode->getFirstTextChildIndex();
		DBXML_ASSERT(index >= 0);
	}
	for (int i = 0; i < num; i++) {
		DBXML_ASSERT(first->getNsNodeType() == nsNodeText);
		if (nsTextType(first->getNsTextType()) == NS_PINST) {
			tnode->insertPI(mmgr, index,
					first->getNsNodeName(),
					first->getNsNodeValue(),
					toChild);
		} else {
			tnode->insertText(mmgr, index,
					  first->getNsNodeValue(),
					  first->getNsTextType(),
					  toChild);
		}
		first->setOwner(target);
		first->setIndex(index++);
		// parent does not change
		first = (NsDomText *) first->getNsNextSibling();
	}
	// caller has already rearranged sibling links correctly
	// need to renumber indexes on any remaining text siblings
	// "first" is pointing to it, or it's pointing to the source node
	tmp = first;
	while (tmp && (tmp->getNsNodeType() == nsNodeText)) {
		int32_t newindex = ((NsDomText*)tmp)->getIndex();
		((NsDomText*)tmp)->setIndex(newindex + num);
		tmp = tmp->getNsNextSibling();
	}
}

//
// This is the routine that inserts new elements (nodes)
// into an existing tree, allocating new node IDs along the
// way.  It is recursive, adding all child elements, as necessary
// Assumptions:
//  o parent is already part of tree, as are next and previous, if set
//  o "this" is not part of any other tree (i.e. it's standalone)
//  o preceding/following are the node ids that surround the to-be-created
//    id for this node
//  o transient element links are valid.  They are not modified here
//
NsDomElement *
NsDomElement::_attachToTree(NsDomElement *parent,
			    NsDomElement *previous, NsDomElement *next,
			    const NsNid *preceding, const NsNid *following,
			    NsDomInsertType itype)
{
	DBXML_ASSERT(_node->isStandalone());

	if(!parent->_node->isStandalone())
		_node->clearFlag(NS_STANDALONE);

	// if the parent is/was standalone, then
	// this node is too, and there's nothing left to do until
	// the parent's tree is attached to a non-standalone tree
	if (_node->isStandalone())
		return 0;

	//
	// attaching to a tree with real NIDs...
	// do not modify transient links -- just change the nid-based
	// links from here down
	//
	DBXML_ASSERT(!_node->isStandalone());

	MemoryManager *mmgr = getMemoryManager();

	// Generate an id for ourselves
	_node->getNid()->getBetweenNid(mmgr, preceding, following, itype);
	
	_node->setLevel(parent->_node->getLevel() + 1);

	// Set our parent id
	_node->getParentNid()->copyNid(mmgr, parent->_node->getNid());
	
	// Set parent's first/last child nid, if appropriate, as well
	// as next/prev links
	if (previous != 0)
		_node->setNextPrev(mmgr, previous->getNsNode());

	if (next == 0)
		parent->getNsNode()->setLastChild(mmgr, _node);
	else
		next->getNsNode()->setNextPrev(mmgr, _node);
	
	// Walk the child tree, doing the same, also adding
	// last descendant NID
	const NsNid *child_preceding = getNodeId();
	NsDomElement *new_child = this;
	NsDomElement *child_previous = 0;
	NsDomElement *child = getElemFirstChild();
	while(child != 0) {
		// use nsDomInsertAppend to get the best allocation
		// of ids for the subtree
		new_child = child->
			_attachToTree(this, child_previous, 0,
				      child_preceding, following,
				      nsDomInsertAppend);
		child_preceding = new_child->getNodeId();
		child_previous = child;
		child = child->getElemNext();
	}
	// set last descendant nid
	if (new_child != this)
		setLastDescendantNid(new_child->getLastDescendantNid());

	if (next == 0)
		parent->setLastDescendantNid(getLastDescendantNid());
	
	// do not add to modifications list here.  There's a loop
	// in the caller doing that to avoid redundancy and conflict

	return new_child;
}

void
NsDomElement::_makeStandalone()
{
	if(!_node->isStandalone()) {
		nsMakeTransient();

		NsDomElement *next;
		NsDomElement *child = getElemFirstChild();
		while(child != 0) {
			next = child->getElemNext();
			child->_makeStandalone();
			child = next;
		}

		// Mark us as being removed
		getNsDocument()->addToModifications(NodeModification::REMOVE, this);

		// Remove Node IDs from the NsNode -- they're no longer
		// needed or useful for navigation.  This method also
		// sets NS_STANDALONE, and sets level to 0
		_node->removeNids(getMemoryManager());
	}
}

const xmlch_t *
NsDomElement::getNsLocalName() const
{
	if (!_lname) {
		_getName();
		DBXML_ASSERT(_qname.get16() && _lname);
	}
	return _lname;
}

const xmlch_t *
NsDomElement::getNsPrefix() const
{
	if (isDocumentNode() || _node->namePrefix() == NS_NOPREFIX)
		return 0;

	if (!_prefix.get16()) {
		setString16(_prefix, getMemoryManager(), _document,
			    _node->namePrefix());
	}
	return _prefix.get16();
}

void NsDomElement::setNsPrefix(const xmlch_t *prefix)
{
	if (isDocumentNode() || !_node->hasUri())
		return;

	XMLChToUTF8Null xcode(prefix);
	int prefixId =
		getNsDocument()->addIDForString(xcode.str(), xcode.len());
	if(_node->namePrefix() != prefixId) {
		_node->setNamePrefix(prefixId);

		_qname.clear(_document->getMemoryManager());
		_lname = 0;

		_document->addToModifications(NodeModification::UPDATE, this);
	}
}

const xmlch_t *
NsDomElement::getNsUri() const
{
	if (isDocumentNode() || !_node->hasUri())
		return 0;

	if (!_uri.get16()) {
		setString16(_uri, getMemoryManager(), _document,
			    _node->uriIndex());
	}
	return _uri.get16();
}

const xmlch_t *
NsDomElement::getNsNodeValue() const
{
	return 0;
}

NsDomElement *
NsDomElement::getElemParent(bool fetch)
{
	// When node identity was pointer identity, there was
	// always a parent.  No longer true.  Materialize if necessary
	if (fetch && !_parent && !_node->isStandalone() && !_node->isDoc())
		_parent = _document->fetchDomElement(_node->getParentNid());
	return _parent;
}

#if 0
// Brute-force method to find a specific NsNid in a document,
// starting at the specified element.  No longer required,
// given the NodeMap in NsDocument.  Leaving code for now, in case
// it's needed, later on.
// NOTE: recursive algorithm
static NsDomElement *
_findNid(NsDomElement *elem, const NsNid *nid)
{
	// This should only be called for materialized dom trees and DLS+
	NsDomElement *result = elem;

	while (result) {
		if (*result->getNodeId() == *nid)
			return result;
		if (result->hasNsChildElements()) {
			NsDomElement *childRes =
				_findNid(result->getElemFirstChild(), nid);
			if (childRes)
				return childRes;
		}
		result = result->getElemNext();
	}
	return result;
}
#endif

NsDomElement *
NsDomElement::lookupDescendantElement(const NsNid *nid, bool fetch)
{
	if(_document->isFromParse()) {
		NsDomElement *elem = getElemFirstChild(fetch);
		if(elem != 0) {
			return elem->lookupElement(nid, fetch);
		}
		return 0;
	}

	if (!hasNsChildElements() || !fetch)
		return 0;

	return _document->fetchDomElement(nid);
}

NsDomElement *
NsDomElement::lookupElement(const NsNid *nid, bool fetch)
{
	int cmp = getNodeId()->compareNids(nid);
	if(cmp == 0) return this;
	if(cmp > 0) return 0;

	// The node we are looking for is after us
	NsDomElement *elem = getElemNext(fetch);
	if(elem != 0) {
		elem = elem->lookupElement(nid);
		if(elem != 0) return elem;
	}

	// The node we are looking for is in our descendants
	elem = getElemFirstChild(fetch);
	if(elem != 0) {
		return elem->lookupElement(nid);
	}

	return 0;
}

NsDomElement *
NsDomElement::getElemPrev(bool fetch)
{
	NsDomElement *prev = _nsElemPrev;
	if (!prev && _node->hasPrev() && !_node->isStandalone() && fetch) {
		DBXML_ASSERT(_node->getPrevNid() &&
			     !_node->getPrevNid()->isNull());
		prev = _document->fetchDomElement(_node->getPrevNid());
		if (!prev)
			NsDomErrNotFound(_node->getPrevNid(), __LINE__);
		_nsElemPrev = prev;
		prev->setElemNext((NsDomElement*)this);
		if(_parent) {
			prev->setElemParent(_parent);
			if (!prev->getNsNode()->hasPrev()) {
				// 'prev' is parent's first elem child...
				_parent->setElemFirstChild(this);
			}
		}
	}
	DBXML_ASSERT(!prev || (prev->_nsElemNext == this));
	return prev;
}

NsDomElement *
NsDomElement::getElemNext(bool fetch)
{
	NsDomElement *next = _nsElemNext;
	if (!next && _node->hasNext() && !_node->isStandalone()  && fetch) {
		next = _document->fetchNextDomElement(_node->getLastDescendantNidOrSelf());
		if (!next)
			NsDomErrNotFound(_node->getLastDescendantNidOrSelf(), __LINE__);
		_nsElemNext = next;
		next->setElemPrev(this);
		if(_parent) {
			next->setElemParent(_parent);
			if (!next->getNsNode()->hasNext()) {
				// 'next' is parent's last elem child...
				_parent->setElemLastChild(next);
			}
		}
	}
	
	DBXML_ASSERT(!next || (next->_nsElemPrev == this));
	return next;
}

NsDomElement *
NsDomElement::getElemLastChild(bool fetch)
{
	NsDomElement *last = _nsElemLastChild;
	if (!last && _node->hasChildElem() &&
	    !_node->isStandalone() && fetch) {
		last = _document->fetchDomElement(
			_node->getLastChildNid());
		if (!last)
			NsDomErrNotFound(_node->getLastChildNid(), __LINE__);
		_nsElemLastChild = last;
		last->setElemParent(this);
	}
	DBXML_ASSERT(!last || last->_parent);
	return last;
}

NsDomElement *
NsDomElement::getElemFirstChild(bool fetch)
{
	NsDomElement *first = _nsElemFirstChild;
	if (!first && _node->hasChildElem() &&
	    !_node->isStandalone()  && fetch) {
		first = _document->fetchNextDomElement(
			_node->getNid());
		if (!first)
			NsDomErrNotFound(_node->getNid(),
				__LINE__);
		_nsElemFirstChild = first;
		first->setElemParent(this);
	}
	DBXML_ASSERT(!first || first->_parent);
	return first;
}

//
// Methods introduce in NsDomElement -- child navigation
//
NsDomNav *
NsDomElement::getNsFirstChild()
{
	NsDomNav *child = _firstChild;
	if (!child && _node->hasChildNode()) {
		// fetch: either (1) an element or its prev sib
		// or (2) the first child text node
		if (_node->hasChildElem()) {
			NsDomNav *first =
				getElemFirstChild();
			DBXML_ASSERT(first);
			child = first;
			// move "left" until done
			while ((first = child->getNsPrevSibling()))
				child = first;
		} else {
			// first child is a direct child text node
			child = makeChildTextNodes(NULL, false/*returnLast*/);
		}
	}
	_firstChild = child;
	// optionally skip NS_ENTSTART, NS_ENTEND text nodes
	if (child && child->getNsIsEntityType() &&
	       !(getNsDocument()->getCreateEntityText())) {
		child = child->getNsNextSibling();
	}
	return child;
}

NsDomNav *
NsDomElement::getNsLastChild(bool forUpdate)
{
	NsDomNav *child = _lastChild;
	if (!child && _node->hasChildNode()) {
		// fetch: either (1) last child element or
		// (2) last child text node
		if (_node->hasTextChild()) {
			// text children are always last.
			NsDomNav *prev = (forUpdate ?
					  getElemLastChild() : 0);
			child = makeChildTextNodes(prev, true/*returnLast*/);
		} else {
			child =	getElemLastChild();
			DBXML_ASSERT(child);
		}
	}
	_lastChild = child;
	// optionally skip NS_ENTSTART, NS_ENTEND text nodes
	if (child && child->getNsIsEntityType() &&
	       !(getNsDocument()->getCreateEntityText())) {
		child = child->getNsPrevSibling();
	}
	return child;
}

NsDomElement *
NsDomElement::getNsParentNode()
{
	// Document node is not always materialized first,
	// so it may have to be done on demand.
	if (_parent || isDocumentNode())
		return _parent;
	// need to materialize it
	return getElemParent();
}

// get interior siblings -- next, previous
NsDomNav *
NsDomElement::getNsNextSibling()
{
	NsDomNav *sib = getNextSib();
	if (!sib) {
		// "next" node is one of:
		// 1. not present
		// 2. next element/node
		// 3. leading text for next element/node
		// 4. child text of parent
		NsDomElement *parent = getElemParent();
		if (!_node->hasNext() &&
		    (!parent || !parent->getNsNode()->hasTextChild()))
			return 0; // no next

		if (_node->hasNext()) {
			// either next node, or its leading text
			NsDomElement *next = getElemNext();
			if (next->getNsNode()->hasLeadingText()) {
				sib = next->makeLeadingTextNodes(this, false /*returnLast*/);
				DBXML_ASSERT(sib == getNextSib());
			} else {
				sib = next;
			}
		} else {
			// next sib is parent's child text
			DBXML_ASSERT(parent);
			sib = parent->makeChildTextNodes(this, //previous
							 false /*returnLast*/);
		}
	}
	if (sib && sib->getNsIsEntityType() &&
	       !(getNsDocument()->getCreateEntityText())) {
		sib = sib->getNsNextSibling();
	}
	return sib;
}

NsDomNav *
NsDomElement::getNsPrevSibling()
{
	NsDomNav *sib = getPrevSib();
	if (!sib) {
		// "previous" node is one of:
		// 1. not present
		// 2. prev sibling element/node
		// 3. leading text
		if (_node->hasLeadingText()) {
			// it's a leading text node
			sib = makeLeadingTextNodes(NULL, true);
			DBXML_ASSERT(sib && (sib == getPrevSib()));
		} else if (_node->hasPrev()) {
			sib = getElemPrev();
		}
	}
	if (sib && sib->getNsIsEntityType() &&
	       !(getNsDocument()->getCreateEntityText())) {
		sib = sib->getNsPrevSibling();
	}
	return sib;
}

NsDomText *
NsDomElement::getNsTextNode(int index)
{
	int endIndex = _node->getNumText();
	if(index >= endIndex) return 0;

	int splitIndex = endIndex - _node->getNumChildText();
	NsDomNav *result = 0;
	if(index < splitIndex) {
		// It's a leading text node
		result = getNsPrevSibling();
	} else {
		// It's a child text node
		result = getNsLastChild();
	}

	while(result->getIndex() > index) {
		result = result->getNsPrevSibling();
	}
	DBXML_ASSERT(result->getIndex() == index);
	return (NsDomText*)result;
}

NsDomNav *
NsDomElement::makeChildTextNodes(NsDomNav *previous, bool returnLast)
{
	int endIndex = _node->getNumText();
	int startIndex = endIndex - _node->getNumChildText();
	NsDomNav *retval = 0;
	for (int i = startIndex; i < endIndex; i++) {
		// owner and parent are the same -- this
		NsDomText *newtext = getNsDomFactory()->createNsDomText(
			this, this, i, _node->textType(i));
		// initialize plumbing
		DBXML_ASSERT(newtext->getNsParentNode() == this);
		if (previous) {
			previous->setNsNextSib(newtext);
			newtext->setNsPrevSib(previous);
		}
		previous = newtext;
		if (returnLast)
			retval = newtext; // return last
		else if (i == startIndex)
			retval = newtext;
	}
	return retval;
}

NsDomNav *
NsDomElement::makeLeadingTextNodes(NsDomNav *previous, bool returnLast)
{
	int endIndex = _node->getNumText() - _node->getNumChildText();
	int startIndex = 0;
	NsDomNav *retval = 0;
	NsDomText *newtext = 0;
	NsDomElement *parent = getElemParent(false); // don't fetch
	for (int i = startIndex; i < endIndex; i++) {
		// "this" is the owner, but not parent
		newtext = getNsDomFactory()->createNsDomText(
			parent, this, i, _node->textType(i));
		// initialize plumbing
		DBXML_ASSERT(newtext->getOwner() == this);
		DBXML_ASSERT(!parent || (newtext->getNsParentNode() == parent));
		if (previous) {
			previous->setNsNextSib(newtext);
			newtext->setNsPrevSib(previous);
		}
		previous = newtext;
		if (returnLast)
			retval = newtext;
		else if (i == startIndex)
			retval = newtext;
	}
	if (newtext) {
		// last one is previous sibling to "this"
		newtext->setNsNextSib(this);
		setNsPrevSib(newtext);
	}
	return retval;
}

NsNodeType_t
NsDomElement::getNsNodeType() const
{
	if (isDocumentNode())
		return nsNodeDocument;
	return nsNodeElement;
}

//
// Structure and functions to implement
// getNsTextContent().
//

struct textContent {
public:
	textContent(MemoryManager *mgr)
		: buf(0), bufsize(0), offset(0), mmgr(mgr){}
	xmlch_t *buf;
	uint32_t bufsize;
	uint32_t offset;
	MemoryManager *mmgr;
};

static void
_reserve(struct textContent &buf, uint32_t needed)
{
	uint32_t avail = buf.bufsize - buf.offset;
	if (avail < needed) {
		uint32_t allocSize = (buf.bufsize + needed) << 1;
		xmlch_t *newbuf = (xmlch_t*)buf.mmgr->allocate(allocSize<<1);
		DBXML_ASSERT(newbuf);
		memcpy(newbuf, buf.buf, (buf.bufsize << 1));
		::free(buf.buf);
		buf.buf = newbuf;
		buf.bufsize = allocSize;
	}
}

static void
_processText(const NsNode *node, int startIndex, int endIndex,
	     struct textContent &buf)
{
	while (startIndex < endIndex) {
		nsTextEntry_t *tp = node->getTextEntry(startIndex);
		if ((nsTextType(tp->te_type) == NS_TEXT ||
		    nsTextType(tp->te_type) == NS_CDATA) &&
			(tp->te_type & NS_IGNORABLE) == 0) {
			uint32_t needed = tp->te_text.t_len;
			_reserve(buf, needed);
			if (node->isUTF16()) {
				memcpy(buf.buf + buf.offset,
				       tp->te_text.t_chars,
				       (needed << 1));
				buf.offset += needed;
			} else {
				// transcode
				xmlch_t *dest = buf.buf + buf.offset;
				buf.offset +=
					NsUtil::nsFromUTF8(
					  buf.mmgr, &dest,
					  (const xmlbyte_t *)tp->te_text.t_chars,
					  needed, needed);
			}

		}
		++startIndex;
	}
}

//
// Acquiring text content recursively may materialize
// element nodes, but will not materialize NsDomText
// or other text nodes. Text is read directly from NsNode structures.
//
static void
_getNsTextContent(NsDomElement *current,
		  struct textContent &buf)
{
	const NsNode *node = current->getNsNode();
	NsDomElement *child = current->getElemFirstChild();
	while(child) {
		const NsNode *cnode = child->getNsNode();
		uint32_t endIndex = cnode->getNumLeadingText();
		if (endIndex != 0)
			_processText(cnode, 0, endIndex, buf);
		_getNsTextContent(child, buf);
		child = child->getElemNext();
	}
	
	_processText(node, node->getNumLeadingText(), node->getNumText(), buf);
}

// not const, because of caching side effects
const xmlch_t *
NsDomElement::getNsTextContent() const
{
	// We recalculate the text content every time, just in case there
	// have been tree modifications. - jpcs
	_textContent.clear(getMemoryManager());

	if (!_textContent.get16()) {
		// concat text content of all child text and element nodes,
		// exclude comments, PIs, attrs
		struct textContent buf(getMemoryManager());
		_getNsTextContent(const_cast<NsDomElement*>(this), buf);
		_reserve(buf, 1);
		buf.buf[buf.offset] = 0;
		_textContent.set16(getMemoryManager(), buf.buf, true);
	}
	return _textContent.get16();
}

void
NsDomElement::setNsTextContent(const xmlch_t *value)
{
	nsMakeTransient();

	// Remove my element descendants
	removeElemDescendants();
	NsNode *node = getNsNode();

	// remove any child text nodes
	int32_t firstChildIndex = node->getFirstTextChildIndex();
	if (firstChildIndex != -1) {
		for (int i = firstChildIndex; i < node->getNumText(); i++)
			node->removeText(getMemoryManager(), i);
	}

	// insert new text child
	if(value && *value) {
		node->insertText(getMemoryManager(), node->getNumText(),
				 value, NS_TEXT, true);
	}

	// child navigational state has been removed
	_firstChild = 0;
	_lastChild = 0;
	_nsElemFirstChild = 0;
	_nsElemLastChild = 0;

	getNsDocument()->addToModifications(NodeModification::UPDATE, this);
}

void
NsDomElement::removeElemDescendants()
{
	if (_node->getFlags() & NS_HASCHILD) {
		// Remove the children, and their descendants
		NsDomElement *child, *new_child;
		for(child = getElemFirstChild(); child != 0; child = new_child) {
			child->removeElemDescendants();
			getNsDocument()->addToModifications(
				NodeModification::REMOVE, child);

			new_child = child->getElemNext();
			getNsDomFactory()->destroy(child);
		}

		_node->clearFlag(NS_HASCHILD);

		_firstChild = 0;
		_lastChild = 0;
		_nsElemFirstChild = 0;
		_nsElemLastChild = 0;

		getNsDocument()->addToModifications(NodeModification::UPDATE, this);
	}
}

// DOM spec appears to require that a namednodemap be returned,
// even if there are no attributes.  So create an empty one.
NsDomNamedNodeMap *
NsDomElement::getNsAttributes()
{
	if (!_attributes) {  // && _node->hasAttrs()) {
		_attributes = getNsDomFactory()->createNsDomNodeMap(this);
	}
	return _attributes;
}

NsDomNodeList *
NsDomElement::getNsChildNodes()
{
	if(!_childNodeList) {
		_childNodeList = getNsDomFactory()->createNsDomNodeList(this);
	}
	return _childNodeList;
}

uint32_t
NsDomElement::numNsAttributes() const
{
	return _node->numAttrs();
}

const NsNid *
NsDomElement::getNodeId() const
{
	return _node->getNid();
}

int32_t
NsDomElement::getNsLevel() const
{
	return _node->getLevel();
}

void
NsDomElement::nsMakeTransient()
{
	_node = _node->makeTransient(_document->getMemoryManager());
}

NsDomFactory *
NsDomElement::getNsDomFactory() const
{
	return _document->getDomFactory();
}

//
// NsDomText implementation
//
NsDomText::NsDomText(NsDomElement *parent, NsDomElement *owner,
		     int32_t index, uint32_t type)
	: NsDomNav(parent),
	  _doc(owner->getNsDocument()), //use owner, parent may be null
	  _owner(owner),
	  _type(type),
	  _index(index)
{
}

void
NsDomText::makeNonStandalone(NsDomElement *parent, NsDomElement *owner,
			     int index, NsDomNav *prevSib, NsDomNav *nextSib)
{
	if(_parent == 0) {
		_text.clear(getMemoryManager());
		_value.clear(getMemoryManager());
		_parent = parent;
		_owner = owner;
		_doc = owner->getNsDocument();
		_index = index;
		_prevSib = prevSib;
		_nextSib = nextSib;
	}
}

/// Creates a standalone text type node
NsDomText::NsDomText(NsDocument *doc, const xmlch_t *value, uint32_t type)
	: NsDomNav(0),
	  _doc(doc),
	  _owner(0),
	  _type(type),
	  _index(-1)
{
	DBXML_ASSERT(nsTextType(type) != NS_PINST);
	if(value != 0)
		_text.set16(getMemoryManager(),
			    NsUtil::nsStringDup(getMemoryManager(),
						value, 0),
			    true);
}

/// Creates a standalone processing instruction type node
NsDomText::NsDomText(NsDocument *doc, const xmlch_t *target, const xmlch_t *data)
	: NsDomNav(0),
	  _doc(doc),
	  _owner(0),
	  _type(NS_PINST),
	  _index(-1)
{
	if(target != 0)
		_text.set16(getMemoryManager(),
			    NsUtil::nsStringDup(getMemoryManager(),
						target, 0),
			    true);
	if(data != 0)
		_value.set16(getMemoryManager(), NsUtil::nsStringDup(getMemoryManager(),
					       data, 0), true);
}

void
NsDomText::makeStandalone()
{
	if(_parent != 0) {
		xmlch_t *text = 0, *value = 0;
		if(nsTextType(_type) == NS_PINST) {
			text = NsUtil::nsStringDup(getMemoryManager(),
						   getNsNodeName(), 0);
			value = NsUtil::nsStringDup(getMemoryManager(),
						    getNsNodeValue(), 0);
		}
		else {
			text = NsUtil::nsStringDup(getMemoryManager(),
						   getNsNodeValue(), 0);
		}

		_text.clear(getMemoryManager());
		_value.clear(getMemoryManager());
		_parent = 0;
		_owner = 0;
		_prevSib = 0;
		_nextSib = 0;
		_index = -1;
		if(text != 0) _text.set16(getMemoryManager(), text, true);
		if(value != 0) _value.set16(getMemoryManager(), value, true);
	}
}

void
NsDomText::destroy()
{
	_text.clear(getMemoryManager());
	_value.clear(getMemoryManager());
}

NsDomFactory *
NsDomText::getNsDomFactory() const
{
	return _doc->getDomFactory();
}

NsNodeType_t
NsDomText::getNsNodeType() const
{
	if (!nsIsEntityText(_type))
		return nsNodeText;
	else {
		if (nsIsEntityStart(_type))
			return nsNodeEntStart;
		return nsNodeEntEnd;
	}
}

bool
NsDomText::getNsIsEntityType() const
{
	const uint32_t type = nsTextType(_type);
	return ((type == NS_ENTSTART)
		|| (type == NS_ENTSTART)
		|| (type == NS_SUBSET));
}

NsDomElement *
NsDomText::getNsParentNode()
{
	if (!_parent && _owner) {
		// need to materialize it -- use owner's parent, since
		// owner is always a sibling if parent is not yet set,
		// unless this is a free-standing node...
		_parent = _owner->getElemParent();
	}
	return _parent;
}

NsDomNav *
NsDomText::getNsNextSibling()
{
	NsDomNav *sib = getNextSib();
	if (!sib) {
		// text nodes are always created with their next
		// siblings, so this means there is none
	}
	if (sib && sib->getNsIsEntityType() &&
	       !(getNsDocument()->getCreateEntityText())) {
		sib = sib->getNsNextSibling();
	}
	return sib;
}

NsDomNav *
NsDomText::getNsPrevSibling()
{
	NsDomNav *sib = getPrevSib();
	if (!sib) {
		// Because text nodes are created in a list
		// with their owner nodes, a previous sibling
		// is not text, but it may be an adjacent element,
		// NOTE: pointer equality check is safe here because
		// if the parent is the owner, that was set during
		// construction
		if (_parent && (_parent == _owner)) {
			// if present, sib is the last child of parent
			sib = _parent->getElemLastChild();
		} else if (_owner->getNsNode()->hasPrev()) {
			sib = _owner->getElemPrev();
		}
		// hook up plumbing 
		if (sib) {
			sib->setNsNextSib(this);
			setNsPrevSib(sib);
		}
	}
	if (sib && sib->getNsIsEntityType() &&
	       !(getNsDocument()->getCreateEntityText())) {
		sib = sib->getNsPrevSibling();
	}
	return sib;
}

// text nodes are one level down from parents
int32_t
NsDomText::getNsLevel() const
{
	if(_owner) {
		if(_index < _owner->getNsNode()->getNumLeadingText()) {
			return _owner->getNsLevel();
		} else {
			return _owner->getNsLevel() + 1;
		}
	} else return -1;
}

bool
NsDomText::isNativeUTF8() const
{
	return _owner != 0 && !_owner->getNsNode()->isUTF16();
}

const xmlbyte_t *
NsDomText::getNsNodeName8() const
{
	if(_owner != 0 && nsTextType(_type) == NS_PINST) {
		return (const xmlbyte_t *)_owner->getNsNode()->getText(_index)->t_chars;
	}
	return 0;
}

const xmlbyte_t *
NsDomText::getNsLocalName8() const
{
	return 0;
}

const xmlbyte_t *
NsDomText::getNsNodeValue8() const
{
	if(_owner != 0) {
		switch(nsTextType(_type)) {
		case NS_TEXT:
		case NS_COMMENT:
		case NS_CDATA:
		case NS_SUBSET:
			return (const xmlbyte_t*)_owner->getNsNode()->getText(_index)->t_chars;
		case NS_PINST: {
			const xmlbyte_t *val = (const xmlbyte_t *)
				_owner->getNsNode()->getText(_index)->t_chars;
			while(*val++); // get past NULL
			return val;
		}
		}
	}
	return 0;
}

const xmlch_t *
NsDomText::_getText() const
{
	const xmlch_t *text = _text.get16();
	if(text == 0 && _owner != 0) {
		bool owned = false;
		NsNode *node = _owner->getNsNode();
		text = getNsDocument()->getText(node->getText(_index),
						node->isUTF16(), owned);
		DBXML_ASSERT(text);
		_text.set16(getMemoryManager(), text, owned);
	}
	return text;
}

const xmlch_t *
NsDomText::getNsNodeValue() const
{
	uint32_t type = nsTextType(_type);
	switch (type) {
	case NS_TEXT:
	case NS_COMMENT:
	case NS_CDATA:
	case NS_SUBSET:
		return _getText();
	case NS_PINST: {
		if(_value.get16() == 0 && _owner != 0) {
			const xmlch_t *val = _getText();
			while (*val++); // get past NULL
			_value.set16(getMemoryManager(), val, false);
		}
		return _value.get16();
	}
	case NS_ENTSTART:
	case NS_ENTEND:
		return _getText();
	default:
		DBXML_ASSERT(0);
		break;
	}
	return 0;
}

void
NsDomText::setNsNodeValue(const xmlch_t *value)
{
	if (_owner)
		_owner->nsMakeTransient();

	if(nsTextType(_type) == NS_PINST)
		_piSetNodeValue(value);
	else
		_textSetNodeValue(value);

	// Reset the lazily cached name and value.
	_text.clear(getMemoryManager());
	_value.clear(getMemoryManager());

	getNsDocument()->addToModifications(NodeModification::UPDATE, _owner);
}

void
NsDomText::_textSetNodeValue(const xmlch_t *value)
{
	if(_owner == 0) {
		_text.clear(getMemoryManager());
		_text.set16(getMemoryManager(),
			    NsUtil::nsStringDup(getMemoryManager(),
						value, 0), true);
		return;
	}

	NsNode *node = _owner->getNsNode();
	nsText_t *text = node->getText(_index);

	if(node->isUTF16()) {
		// Work out size of new buffer
		int vlen = NsUtil::nsStringLen(value);
		int len = vlen + 1;

		// Allocate buffer
		xmlch_t *buffer = (xmlch_t*)getMemoryManager()->
			allocate(len * sizeof(xmlch_t));
		if (!buffer)
			NsUtil::nsThrowException(XmlException::NO_MEMORY_ERROR,
				 "Unable to allocate memory for _piSetNodeValue",
				  __FILE__, __LINE__);

		// Copy value into buffer
		memcpy(buffer, value, len * sizeof(xmlch_t));

		// Recalculate the total attributes size
		node->getTextList()->tl_len += len;
		node->getTextList()->tl_len -= text->t_len + 1;

		// Set the attribute to the new buffer
		getMemoryManager()->
			deallocate(text->t_chars);
		text->t_chars = buffer;
		text->t_len = vlen;
	} else {
		// Work out size of new buffer
		int vlen = NsUtil::nsStringLen(value);
		int len = vlen * 3 + 1;

		// Allocate buffer
		xmlbyte_t *buffer = (xmlbyte_t*)getMemoryManager()->
			allocate(len * sizeof(xmlbyte_t));
		if (!buffer)
			NsUtil::nsThrowException(XmlException::NO_MEMORY_ERROR,
				 "Unable to allocate memory for _piSetNodeValue",
				  __FILE__, __LINE__);

		// Transcode value into buffer
		int vlen8 = NsUtil::nsToUTF8(
			getMemoryManager(),
			&buffer, value, vlen + 1, len);

		// Recalculate the total attributes size
		node->getTextList()->tl_len += vlen8 + 1;
		node->getTextList()->tl_len -= text->t_len + 1;

		// Set the attribute to the new buffer
		getMemoryManager()->
			deallocate(text->t_chars);
		text->t_chars = buffer;
		text->t_len = vlen8 - 1;
	}
}

void
NsDomText::_piSetNodeValue(const xmlch_t *value)
{
	if(_owner == 0) {
		_value.clear(getMemoryManager());
		_value.set16(getMemoryManager(),
			     NsUtil::nsStringDup(getMemoryManager(),
						 value, 0), true);
		return;
	}
	NsNode *node = _owner->getNsNode();
	nsText_t *text = node->getText(_index);

	if(node->isUTF16()) {
		// Work out size of new buffer
		int nlen = NsUtil::nsStringLen((xmlch_t*)text->t_chars);
		int vlen = NsUtil::nsStringLen(value);
		int len = nlen + vlen + 2; // 2 nulls

		// Allocate buffer
		xmlch_t *buffer = (xmlch_t*)getMemoryManager()->
			allocate(len * sizeof(xmlch_t));
		if (!buffer)
			NsUtil::nsThrowException(XmlException::NO_MEMORY_ERROR,
				 "Unable to allocate memory for _piSetNodeValue",
				  __FILE__, __LINE__);

		// Copy name into buffer
		memcpy(buffer, text->t_chars, (nlen + 1) * sizeof(xmlch_t));

		// Copy value into buffer
		xmlch_t *vbuf = buffer + nlen + 1;
		memcpy(vbuf, value, (vlen + 1) * sizeof(xmlch_t));

		// Recalculate the total attributes size
		node->getTextList()->tl_len += len;
		node->getTextList()->tl_len -= text->t_len + 1;

		// Set the attribute to the new buffer
		getMemoryManager()->
			deallocate(text->t_chars);
		text->t_chars = buffer;
		text->t_len = nlen + 1 + vlen;
	} else {
		// Work out size of new buffer
		int nlen = NsUtil::nsStringLen((xmlbyte_t*)text->t_chars);
		int vlen = NsUtil::nsStringLen(value);
		int len = nlen + vlen * 3 + 2; // 2 nulls

		// Allocate buffer
		xmlbyte_t *buffer = (xmlbyte_t*)getMemoryManager()->
			allocate(len * sizeof(xmlbyte_t));
		if (!buffer)
			NsUtil::nsThrowException(XmlException::NO_MEMORY_ERROR,
				 "Unable to allocate memory for _piSetNodeValue",
				  __FILE__, __LINE__);

		// Copy name into buffer
		memcpy(buffer, text->t_chars, (nlen + 1) * sizeof(xmlbyte_t));

		// Transcode value into buffer
		xmlbyte_t *value8 = buffer + nlen + 1;
		int vlen8 = NsUtil::nsToUTF8(
			getMemoryManager(),
			&value8, value, vlen + 1, vlen * 3 + 1);

		// Recalculate the total attributes size
		node->getTextList()->tl_len += nlen + vlen8 + 1;
		node->getTextList()->tl_len -= text->t_len + 1;

		// Set the attribute to the new buffer
		getMemoryManager()->
			deallocate(text->t_chars);
		text->t_chars = buffer;
		text->t_len = nlen + vlen8;
	}
}

const xmlch_t *
NsDomText::getNsNodeName() const
{
	uint32_t type = nsTextType(_type);
	switch (type) {
	case NS_TEXT:
	case NS_SUBSET:
		return _nsDomTextName;
	case NS_COMMENT:
		return _nsDomCommentName;
	case NS_CDATA:
		return _nsDomCdataName;
	case NS_PINST: {
		return _getText(); // PI target is name
	}
	case NS_ENTSTART:
	case NS_ENTEND:
		return 0;
	default:
		DBXML_ASSERT(0);
		break;
	}
	return 0;
}

const xmlch_t *
NsDomText::getNsLocalName() const
{
	// Local name is specified as null
	return 0;
}

NsNode *
NsDomText::getNsNode() const
{
	return _owner ? _owner->getNsNode() : 0;
}

const NsNid *
NsDomText::getNodeId() const
{
	return _owner ? _owner->getNodeId() : 0;
}

bool
NsDomText::nsIgnorableWhitespace() const
{
	if(_owner != 0) {
		NsNode *node = _owner->getNsNode();
		if (node->getTextEntry(_index)->te_type
		    & NS_IGNORABLE)
			return true;
	}
	return false;
}

//
// NsDomAttr implementation
//
NsDomAttr::NsDomAttr(NsDomElement *owner, int index)
	: _owner(owner),
	  _doc(_owner->getNsDocument()),
	  _index(index),
	  _lname(0)
{
}

void
NsDomAttr::makeNonStandalone(NsDomElement *owner, int index)
{
	if(_owner == 0) {
		_name.clear(getMemoryManager());
		_lname = 0;
		_value.clear(getMemoryManager());
		_prefix.clear(getMemoryManager());
		_uri.clear(getMemoryManager());
		_owner = owner;
		_doc = _owner->getNsDocument();
		_index = index;
	}
}

NsDomAttr::NsDomAttr(NsDocument *doc, const xmlch_t *uri, const xmlch_t *qname)
	: _owner(0),
	  _doc(doc),
	  _index(-1),
	  _lname(0)
{
	// copy the qname
	if(qname != 0) {
		_name.set16(getMemoryManager(),
			    NsUtil::nsStringDup(doc->getMemoryManager(),
						qname, 0), true);

		// point _lname to the localname
		_lname = _name.get16();
		while(*_lname != 0 && *_lname++ != ':');
		if(*_lname == 0) _lname = _name.get16();
	}

	// copy the uri
	if(uri != 0)
		_uri.set16(getMemoryManager(),
			   NsUtil::nsStringDup(doc->getMemoryManager(),
					       uri, 0), true);

	// set the value to a zero length string
	static const xmlch_t zeroLength[] = { 0 };
	_value.set16(getMemoryManager(), zeroLength, false);
}

void
NsDomAttr::makeStandalone()
{
	if(_owner != 0) {
		// Get the present attributes
		xmlch_t *uri = NsUtil::nsStringDup(getMemoryManager(),
						   getNsUri(), 0);
		xmlch_t *qname = NsUtil::nsStringDup(getMemoryManager(),
						     getNsNodeName(), 0);
		xmlch_t *value = NsUtil::nsStringDup(getMemoryManager(),
						     getNsNodeValue(), 0);

		// Clear the internal state
		_name.clear(getMemoryManager());
		_lname = 0;
		_value.clear(getMemoryManager());
		_prefix.clear(getMemoryManager());
		_uri.clear(getMemoryManager());
		_owner = 0;
		_index = -1;

		// Set the internal state
		if(qname != 0) {
			_name.set16(getMemoryManager(), qname, true);

			// point _lname to the localname
			_lname = qname;
			while(*_lname != 0 && *_lname++ != ':');
			if(*_lname == 0) _lname = qname;
		}
		if(uri != 0)
			_uri.set16(getMemoryManager(), uri, true);
		if(value != 0)
			_value.set16(getMemoryManager(), value, true);
	}
}

void
NsDomAttr::destroy()
{
	_name.clear(getMemoryManager());
	_value.clear(getMemoryManager());
	_prefix.clear(getMemoryManager());
	_uri.clear(getMemoryManager());
}

NsDomFactory *
NsDomAttr::getNsDomFactory() const
{
	return _doc->getDomFactory();
}

const xmlch_t *
NsDomAttr::getNsBaseUri() const
{
	if(_owner) return _owner->getNsBaseUri();
	return 0;
}

bool
NsDomAttr::isNativeUTF8() const
{
	return _owner != 0 && !_owner->getNsNode()->isUTF16();
}

const xmlbyte_t *
NsDomAttr::getNsNodeName8() const
{
	NsUtil::nsThrowException(XmlException::DOM_PARSER_ERROR,
				 "NsDomAttr::getNsNodeName8 not implemented",
				 __FILE__, __LINE__);
	return 0;
}

const xmlbyte_t *
NsDomAttr::getNsLocalName8() const
{
	if(_owner != 0 && !_owner->getNsNode()->isUTF16()) {
		nsName_t *aname = _owner->getNsNode()->getAttrName(_index);
		return (const xmlbyte_t*)aname->n_text.t_chars;
	}
	return 0;
}

const xmlbyte_t *
NsDomAttr::getNsPrefix8() const
{
	if(_owner != 0 && !_owner->getNsNode()->isUTF16()) {
		nsName_t *aname = _owner->getNsNode()->getAttrName(_index);
		if (aname->n_prefix != NS_NOPREFIX) {
			if (!_prefix.get())
				setString(_prefix, getMemoryManager(),
					  getNsDocument(), aname->n_prefix);
		}
	}
	return _prefix.get();
}

const xmlbyte_t *
NsDomAttr::getNsUri8() const
{
	if(_owner != 0 && !_owner->getNsNode()->isUTF16()) {
		int32_t uriIndex = _owner->getNsNode()->attrUri(_index);
		if(uriIndex != NS_NOURI) {
			if (!_uri.get())
				setString(_uri, getMemoryManager(),
					  getNsDocument(), uriIndex);
		}
	}
	return _uri.get();
}

const xmlbyte_t *
NsDomAttr::getNsNodeValue8() const
{
	if(_owner != 0 && !_owner->getNsNode()->isUTF16()) {
		nsName_t *aname = _owner->getNsNode()->getAttrName(_index);
		const xmlbyte_t *val = (const xmlbyte_t*)aname->n_text.t_chars;
		while(*val++); // get past the NULL
		return val;
	}
	return 0;
}

const void
NsDomAttr::_getName() const
{
	DBXML_ASSERT(_owner);
	bool owned = false;
	NsNode *node = _owner->getNsNode();
	nsName_t *aname = node->getAttrName(_index);
	const xmlch_t *qname = getNsDocument()->
		getQname(aname, node->isUTF16(), owned);
	DBXML_ASSERT(qname);
	_name.set16(getMemoryManager(), qname, owned);
	_lname = qname;
	if (aname->n_prefix != NS_NOPREFIX)
		while (*_lname++ != xmlchColon);
	const xmlch_t *val = _lname;
	while(*(val++)); // get past null to value
	_value.set16(getMemoryManager(), val, false);
}

const xmlch_t *
NsDomAttr::getNsNodeValue() const
{
	if (_value.get16() == 0 && _owner != 0) {
		_getName();
		DBXML_ASSERT(_value.get16());
	}
	return _value.get16();
}

void
NsDomAttr::setNsNodeValue(const xmlch_t *value)
{
	if(_owner == 0) {
		_value.clear(getMemoryManager());
		_value.set16(getMemoryManager(),
			     NsUtil::nsStringDup(getMemoryManager(),
						 value, 0), true);
		return;
	}

	_owner->nsMakeTransient();
	NsNode *node = _owner->getNsNode();
	nsAttr_t *attr = node->getAttr(_index);

	if(node->isUTF16()) {
		// Work out size of new buffer
		int nlen = NsUtil::nsStringLen((xmlch_t*)attr->a_name.n_text.t_chars);
		int vlen = NsUtil::nsStringLen(value);
		int len = nlen + vlen + 2; // 2 nulls

		// Allocate buffer
		xmlch_t *buffer = (xmlch_t*)getMemoryManager()->
			allocate(len * sizeof(xmlch_t));
		if (!buffer)
			NsUtil::nsThrowException(XmlException::NO_MEMORY_ERROR,
				 "Unable to allocate memory for setNsNodeValue",
				  __FILE__, __LINE__);

		// Copy name into buffer
		memcpy(buffer, attr->a_name.n_text.t_chars, (nlen + 1) * sizeof(xmlch_t));

		// Copy value into buffer
		xmlch_t *vbuf = buffer + nlen + 1;
		memcpy(vbuf, value, (vlen + 1) * sizeof(xmlch_t));

		// Recalculate the total attributes size
		node->getAttrList()->al_len += len;
		node->getAttrList()->al_len -= attr->a_name.n_text.t_len + 1;

		// Set the attribute to the new buffer
		getMemoryManager()->
			deallocate(attr->a_name.n_text.t_chars);
		attr->a_name.n_text.t_chars = buffer;
		attr->a_name.n_text.t_len = nlen + 1 + vlen;
		attr->a_value = vbuf;
	}
	else {
		// Work out size of new buffer
		int nlen = NsUtil::nsStringLen((xmlbyte_t*)attr->a_name.n_text.t_chars);
		int vlen = NsUtil::nsStringLen(value);
		int len = nlen + vlen * 3 + 2; // 2 nulls

		// Allocate buffer
		xmlbyte_t *buffer = (xmlbyte_t*)getMemoryManager()->
			allocate(len * sizeof(xmlbyte_t));
		if (!buffer)
			NsUtil::nsThrowException(XmlException::NO_MEMORY_ERROR,
				 "Unable to allocate memory for setNsNodeValue",
				  __FILE__, __LINE__);

		// Copy name into buffer
		memcpy(buffer, attr->a_name.n_text.t_chars, (nlen + 1) * sizeof(xmlbyte_t));

		// Transcode value into buffer
		bool needsEscaping;
		xmlbyte_t *value8 = buffer + nlen + 1;
		int vlen8 = NsUtil::nsToUTF8(
			getMemoryManager(), &value8, value,
			vlen + 1, vlen * 3 + 1, &needsEscaping, isAttrVal);
		if(needsEscaping) attr->a_flags |= NS_ATTR_ENT;

		// Recalculate the total attributes size
		node->getAttrList()->al_len += nlen + vlen8 + 1;
		node->getAttrList()->al_len -= attr->a_name.n_text.t_len + 1;

		// Set the attribute to the new buffer
		getMemoryManager()->
			deallocate(attr->a_name.n_text.t_chars);
		attr->a_name.n_text.t_chars = buffer;
		attr->a_name.n_text.t_len = nlen + vlen8;
		attr->a_value = value8;
	}

	// Reset the lazily cached name and value.
	_name.clear(getMemoryManager());
	_lname = 0;
	_value.clear(getMemoryManager());

	getNsDocument()->addToModifications(NodeModification::UPDATE, _owner);
}

const xmlch_t *
NsDomAttr::getNsNodeName() const
{
	if (_name.get16() == 0 && _owner != 0) {
		_getName();
		DBXML_ASSERT(_name.get16() && _lname);
	}
	return _name.get16();
}

void
NsDomAttr::nsRename(const xmlch_t *uri, const xmlch_t *qname)
{
	NsDomElement *owner = _owner;
	if(owner != 0) {
		owner->getNsAttributes()->removeNsItem(_index);
	}

	// copy qname
	_name.clear(getMemoryManager());
	_name.set16(getMemoryManager(), NsUtil::nsStringDup(getMemoryManager(),
					qname, 0), true);

	// point _lname to the localname
	_lname = _name.get16();
	while(*_lname != 0 && *_lname++ != ':');
	if(*_lname == 0) _lname = _name.get16();

	// clear the lazily evaluated prefix
	_prefix.clear(getMemoryManager());

	// copy the uri
	_uri.clear(getMemoryManager());
	_uri.set16(getMemoryManager(), NsUtil::nsStringDup(getMemoryManager(),
				       uri, 0), true);

	// If this attribute has an owner, then set the name in the NsNode
	if(owner != 0) {
		owner->getNsAttributes()->setNsNamedItemNS(this);
	}
}

const xmlch_t *
NsDomAttr::getNsLocalName() const
{
	if (_lname == 0 && _owner != 0) {
		_getName();
		DBXML_ASSERT(_name.get16() && _lname);
	}
	return _lname;
}

const xmlch_t *
NsDomAttr::getNsPrefix() const
{
	if(_prefix.get16() == 0) {
		if(_owner == 0) {
			unsigned int plen = _lname - _name.get16();
			if(plen != 0) {
				_prefix.set16(getMemoryManager(),
					      (xmlch_t*)getMemoryManager()->
					      allocate(plen * sizeof(xmlch_t)),
					      true);
				if(_prefix.get16() == 0)
					NsUtil::nsThrowException(XmlException::NO_MEMORY_ERROR,
							 "getNsPrefix failed to allocate memory",
							 __FILE__, __LINE__);
				// don't copy colon
				plen -= 1;
				memcpy((void*)_prefix.get16(), _name.get16(), plen * sizeof(xmlch_t));
				// null terminate
				(xmlch_t&)_prefix.get16()[plen] = 0;
			}
		} else {
			nsName_t *aname =
				_owner->getNsNode()->getAttrName(_index);
			if (aname->n_prefix != NS_NOPREFIX)
				setString16(_prefix, getMemoryManager(),
					    getNsDocument(), aname->n_prefix);
		}
	}
	return _prefix.get16();
}

void NsDomAttr::setNsPrefix(const xmlch_t *prefix)
{
	if(_owner == 0) {
		if(_uri.get16() == 0 || *_uri.get16() == 0)
			return;

		int nlen = NsUtil::nsStringLen(_lname);
		int len = nlen + 1;
		int plen;
		if(prefix != 0) {
			plen = NsUtil::nsStringLen(prefix);
			len += plen + 1;
		}

		xmlch_t *buf = (xmlch_t*)
			getMemoryManager()->allocate(len * sizeof(xmlch_t));

		xmlch_t *ptr = buf;
		if(prefix != 0) {
			memcpy(ptr, prefix, plen * sizeof(xmlch_t));
			ptr += plen;
			*ptr++ = ':';
		}
		memcpy(ptr, _lname, (nlen + 1) * sizeof(xmlch_t));

		_name.clear(getMemoryManager());
		_name.set16(getMemoryManager(), buf, true);
		_lname = ptr;
		_prefix.clear(getMemoryManager());
		return;
	}

	if (!_owner->getNsNode()->attrHasUri(_index))
		return;

	int32_t attr_prefix = _owner->getNsNode()->attrNamePrefix(_index);
	XMLChToUTF8Null xcode(prefix);
	int prefixId =
		getNsDocument()->addIDForString(xcode.str(), xcode.len());
	if(attr_prefix != prefixId) {
		_owner->getNsNode()->setAttrNamePrefix(_index, prefixId);

		_name.clear(getMemoryManager());
		_lname = 0;
		_prefix.clear(getMemoryManager());
		_value.clear(getMemoryManager());

		getNsDocument()->addToModifications(NodeModification::UPDATE, _owner);
	}
}

const xmlch_t *
NsDomAttr::getNsUri() const
{
	if(_uri.get16() == 0 && _owner != 0) {
		int32_t uriIndex = _owner->getNsNode()->attrUri(_index);
		if (uriIndex != NS_NOURI) {
			setString16(_uri, getMemoryManager(), getNsDocument(),
				    uriIndex);
		}
	}
	return _uri.get16();
}

NsNode *
NsDomAttr::getNsNode() const
{
	return _owner ? _owner->getNsNode() : 0;
}

const NsNid *
NsDomAttr::getNodeId() const
{
	return _owner ? _owner->getNodeId() : 0;
}

bool
NsDomAttr::getNsSpecified() const
{
	if (!_owner)
		return true;

	NsNode *node = _owner->getNsNode();
	uint32_t flags = node->attrFlags(_index);
	if (flags & NS_ATTR_NOT_SPECIFIED)
		return false;
	return true;
}

//
// NsDomNamedNodeMap implementation
//

NsDomNamedNodeMap::NsDomNamedNodeMap(NsDomElement *owner) :
	_attrs(owner->getNsNode()->numAttrs()), _owner(owner)
{

}

void
NsDomNamedNodeMap::destroy()
{
	_attrs.~NsDomAttrVector();
}

int
NsDomNamedNodeMap::getNsLength() const
{
	return _attrs.size();
}

NsDomAttr *
NsDomNamedNodeMap::getNsItem(int index)
{
	if (!_attrs.size())
		return 0;
	DBXML_ASSERT((uint32_t)index <= _attrs.size());
	NsDomAttr *attr = _attrs[index];
	if (!attr) {
		attr = getNsDomFactory()->createNsDomAttr(_owner, index);
		DBXML_ASSERT(attr);
		_attrs[index] = attr;
	}
	return attr;
}

NsDomAttr *
NsDomNamedNodeMap::setNsItem(int index, NsDomAttr *attr)
{
	_owner->nsMakeTransient();

	NsDomAttr *retAttr = getNsItem(index);

	// Replace old attribute
	retAttr->makeStandalone();
	_owner->getNsNode()->setAttr(
		getNsDocument(), index,
		attr->getNsPrefix(), attr->getNsUri(),
		attr->getNsLocalName(), attr->getNsNodeValue(), true);
	_attrs[index] = attr;
	attr->makeNonStandalone(_owner, index);

	getNsDocument()->addToModifications(NodeModification::UPDATE, _owner);
	return retAttr;
}

void
NsDomNamedNodeMap::addNsItem(NsDomAttr *attr)
{
	_owner->nsMakeTransient();

	// Append new attribute
	_owner->getNsNode()->addAttr(
		getNsDocument(), attr->getNsPrefix(), attr->getNsUri(),
		attr->getNsLocalName(), attr->getNsNodeValue(), true);
	_attrs.push_back(attr);
	attr->makeNonStandalone(_owner, _attrs.size() - 1);

	getNsDocument()->addToModifications(NodeModification::UPDATE, _owner);
}

NsDomAttr *
NsDomNamedNodeMap::removeNsItem(int index)
{
	_owner->nsMakeTransient();

	NsDomAttr *retAttr = getNsItem(index);
	NsNode *node = _owner->getNsNode();

	// Remove old attribute
	retAttr->makeStandalone();
	node->removeAttr(getNsDocument()->getMemoryManager(), index);

	// Resize and rewrite the attributes vector
	uint32_t size = _attrs.size() - 1;
	NsDomAttr *tmp;
	for(; (uint32_t)index < size; ++index) {
		tmp = _attrs[index + 1];
		if(tmp != 0) tmp->setIndex(index);
		_attrs[index] = tmp;
	}
	_attrs.resize(size);

	getNsDocument()->addToModifications(NodeModification::UPDATE, _owner);
	return retAttr;
}

NsDomAttr *
NsDomNamedNodeMap::getNsNamedItem(const xmlch_t *name)
{
	// this materializes all attributes up to the
	// desired attr, but that's what
	// the caller's going to do, anyway.  Hopefully not
	// used often.
	for (uint32_t i = 0; i < _attrs.size(); ++i) {
		NsDomAttr *attr = getNsItem(i);
		DBXML_ASSERT(attr);
		if (NsUtil::nsStringEqual(name, attr->getNsNodeName())) {
			return attr;
		}
	}

	return 0;
}

NsDomAttr *
NsDomNamedNodeMap::setNsNamedItem(NsDomAttr *attr)
{
	const xmlch_t *name = attr->getNsNodeName();
	uint32_t index = 0;
	for(; index < _attrs.size(); ++index) {
		NsDomAttr *tmp = getNsItem(index);
		DBXML_ASSERT(tmp);
		if (NsUtil::nsStringEqual(name, tmp->getNsNodeName())) {
			return setNsItem(index, attr);
		}
	}

	addNsItem(attr);
	return 0;
}

NsDomAttr *
NsDomNamedNodeMap::removeNsNamedItem(const xmlch_t *name)
{
	uint32_t index = 0;
	uint32_t size = _attrs.size();
	for(; index < size; ++index) {
		NsDomAttr *tmp = getNsItem(index);
		DBXML_ASSERT(tmp);
		if (NsUtil::nsStringEqual(name, tmp->getNsNodeName())) {
			return removeNsItem(index);
		}
	}

	return 0;
}

NsDomAttr *
NsDomNamedNodeMap::getNsNamedItemNS(const xmlch_t *uri, const xmlch_t *localname)
{
	// this materializes all attributes up to the
	// desired attr, but that's what
	// the caller's going to do, anyway.  Hopefully not
	// used often.
	for (uint32_t i = 0; i < _attrs.size(); ++i) {
		NsDomAttr *attr = getNsItem(i);
		DBXML_ASSERT(attr);
		if (NsUtil::nsStringEqual(localname, attr->getNsLocalName()) &&
		    NsUtil::nsStringEqual(uri, attr->getNsUri())) {
			return attr;
		}
	}

	return 0;
}

NsDomAttr *
NsDomNamedNodeMap::setNsNamedItemNS(NsDomAttr *attr)
{
	const xmlch_t *localname = attr->getNsLocalName();
	const xmlch_t *uri = attr->getNsUri();
	uint32_t index = 0;
	for(; index < _attrs.size(); ++index) {
		NsDomAttr *tmp = getNsItem(index);
		DBXML_ASSERT(tmp);
		if (NsUtil::nsStringEqual(localname, tmp->getNsLocalName()) &&
		    NsUtil::nsStringEqual(uri, tmp->getNsUri())) {
			return setNsItem(index, attr);
		}
	}

	addNsItem(attr);
	return 0;
}

NsDomAttr *
NsDomNamedNodeMap::removeNsNamedItemNS(const xmlch_t *uri, const xmlch_t *localname)
{
	uint32_t index = 0;
	uint32_t size = _attrs.size();
	for(; index < size; ++index) {
		NsDomAttr *tmp = getNsItem(index);
		DBXML_ASSERT(tmp);
		if (NsUtil::nsStringEqual(localname, tmp->getNsLocalName()) &&
		    NsUtil::nsStringEqual(uri, tmp->getNsUri())) {
			return removeNsItem(index);
		}
	}

	return 0;
}

NsDomNodeList::NsDomNodeList(NsDomElement *owner)
	: _owner(owner), _num(-1)
{
}

void
NsDomNodeList::destroy()
{
}

// NOTE: the new storage format does not store the number
// of child nodes in an element, so it has to be counted,
// brute-force, materializing them all.
// Fortunately, this is only done if inserting a new node
// to a specific, indexed location, which should be discouraged.
// Append does not require it.
// TBD: think about a better way -- go back to tracking number of
// children (incl. text)in NsNode?
int
NsDomNodeList::getNsLength() const
{
	if(_owner) {
		// check for already-calculated value.
		// This value is not valid after an insert or
		// delete -- callers must not trust it.
		if (_num < 0) {
			_num = 0;
			NsDomNode *cur = _owner->getNsFirstChild();
			while (cur) {
				++_num;
				cur = cur->getNsNextSibling();
			}
		}
		return _num;
	}

	return 0;
}

// NOTE: this is inefficient in the face of a large child
// list, because of the need to call
// getNsLength().  TBD: is there a better way?  The only one
// that comes to mind is tracking the number of children in
// the persistent NsNode...  May be worthwhile...
NsDomNode *
NsDomNodeList::getNsItem(int index) const
{
	NsDomNode *result = 0;
	int num = getNsLength();
	if(_owner && num > index) {
		if(index < num - index) {
			// Traverse from the front
			num = 0;
			result = _owner->getNsFirstChild();
			while(num < index && result != 0) {
				result = result->getNsNextSibling();
				++num;
			}
		}
		else {
			// Traverse from the back
			--num;
			result = _owner->getNsLastChild();
			while(num > index && result != 0) {
				result = result->getNsPrevSibling();
				--num;
			}
		}
	}

	return result;
}

//
// utility functions
//

NsDomNodeList *
NsDomNode::getNsChildNodes()
{
	// Create an empty NsDomNodeList
	return getNsDomFactory()->createNsDomNodeList(0);
}

/* static */ const bool
NsDomNode::compareNsNodes(const NsDomNode *node1,
			  const NsDomNode *node2)
{
	if (!node1 || !node2)
		return false;

	if (node1 == node2)
		return true;

	if (node1->getNsNodeType() != node2->getNsNodeType())
		return false;

	// name is qname, so prefix is included.  Still need
	// to check uri, but prefix should be all set
	if (!NsUtil::nsStringEqual(node1->getNsNodeName(),
				   node2->getNsNodeName()))
		return false;

	if (!NsUtil::nsStringEqual(node1->getNsUri(),
				   node2->getNsUri()))
		return false;

	if (!NsUtil::nsStringEqual(node1->getNsNodeValue(),
				   node2->getNsNodeValue()))
		return false;

	if (!NsUtil::nsStringEqual(node1->getNsBaseUri(),
				   node2->getNsBaseUri()))
		return false;

	return true;
}


// node identity comparison is not just pointer equality
// Also check:
//  o container and doc id (isSameDocument())
//  o node id
//  o index if element if attr or text
//
bool
NsDomNode::operator==(const NsDomNode &other) const
{
	if (this == &other)
		return true;
	bool ret = false;
	// do the more thorough comparison that doesn't assume
	// pointer identity.  Use comparisons more likely to fail first
	NsNodeType_t type = getNsNodeType();
	NsNodeType_t otype = other.getNsNodeType();
	if (type == otype) {
		const NsNid *nid = getNodeId();
		const NsNid *onid = other.getNodeId();
		if (nid && onid && !nid->isNull() && !onid->isNull() &&
		    (*nid == *onid)) {
			if ((type == nsNodeElement) || (type == nsNodeDocument))
				ret = true;
			else if (getIndex() == other.getIndex())
				ret = true;
		}
	}
	if (ret && getNsDocument()->isSameDocument(other.getNsDocument()))
		return true;
	return false;
}


//
// "reference" node is "this" node
// preceding/following are document order
// From Xerces:
// enum DOMTreePosition
//   TREE_POSITION_PRECEDING = 0x01,    // node precedes ref node
//   TREE_POSITION_FOLLOWING = 0x02,    // node follows ref node
//   TREE_POSITION_ANCESTOR  = 0x04,    // node is ancestor of ref node
//   TREE_POSITION_DESCENDANT = 0x08,   // node is descendant of ref node
//   TREE_POSITION_EQUIVALENT = 0x10,   // attrs w/same owner, or "same" node
//   TREE_POSITION_SAME_NODE = 0x20,    // same node
//   TREE_POSITION_DISCONNECTED = 0x00  // no common ancestor
//                                         (e.g. not in same doc)
// };
enum treePositions {
	TREE_PRECEDING    = 0x01,
	TREE_FOLLOWING    = 0x02,
	TREE_ANCESTOR     = 0x04,
	TREE_DESCENDANT   = 0x08,
	TREE_EQUIVALENT   = 0x10,
	TREE_SAME         = 0x20,
	TREE_DISCONNECTED = 0x00
};

//
// "this" node is node1
//

/* static */ const short
NsDomNode::compareNsPositions(const NsDomNode *node1,
			      const NsDomNode *node2)
{
	// simplifying assumptions:
	//  o attributes never have child nodes
	//  o node ids are in document order
	//  o level/tree depth information is accurate
	const NsDomNode *thisNode = node1;
	const NsDomNode *otherNode = node2;

	if (*thisNode == *otherNode) {
		return TREE_SAME|TREE_EQUIVALENT;
	}
	NsNodeType_t thisType = thisNode->getNsNodeType();
	NsNodeType_t otherType = otherNode->getNsNodeType();
	// Xerces marks ENTITY and NOTATION as disconnected.
	// NsDom doesn't currently materialize those
	if (thisType > nsNodeDocument || otherType > nsNodeDocument) {
		return TREE_DISCONNECTED; // disconnected
	}
	if (thisType == nsNodeAttr)
		thisNode = ((const NsDomAttr *)thisNode)->getNsOwner();
	if (otherType == nsNodeAttr)
		otherNode = ((const NsDomAttr *)otherNode)->getNsOwner();
	if (*thisNode == *otherNode) {
		// either 2 attrs with same owner, or attr vs its owner
		if (thisType == otherType)
			return TREE_EQUIVALENT; // attrs w/same owner
		if (thisType == nsNodeAttr) {
			DBXML_ASSERT(otherType == nsNodeElement);
			// other is owner of "this"
			return TREE_ANCESTOR|TREE_PRECEDING;
		} else {
			DBXML_ASSERT(otherType == nsNodeAttr);
			// "this" is owner of other
			return TREE_DESCENDANT|TREE_FOLLOWING;
		}
	}
	if (thisNode->getNsLevel() != otherNode->getNsLevel()) {
		const NsDomNode *lower;
		const NsDomNode *stable;
		if (thisNode->getNsLevel() > otherNode->getNsLevel()) {
			lower = thisNode;
			stable = otherNode;
		} else {
			lower = otherNode;
			stable = thisNode;
		}
		lower = ((NsDomNode *)lower)->getNsParentNode();
		while (lower->getNsLevel() != stable->getNsLevel()) {
			lower = ((NsDomNode *)lower)->getNsParentNode();
		}
		// check for direct relationship.  NOTE: this won't happen
		// if the stable node is TEXT.
		if (*lower == *stable) {
			// if upper/stable node is not an attribute,
			// relationship is ANCESTOR or DESCENDANT; otherwise,
			// it's PRECEDING or FOLLOWING.
			if (*stable == *thisNode) {
				if (thisType == nsNodeAttr) {
					return TREE_FOLLOWING;
				}
				return TREE_DESCENDANT|TREE_FOLLOWING;
			} else {
				if (otherType == nsNodeAttr) {
					return TREE_PRECEDING;
				}
				return TREE_ANCESTOR|TREE_PRECEDING;
			}
		}
		// same level, reset the one that moved to its ancestor
		// need to reset type as well.
		if (*stable == *thisNode) {
			otherNode = lower;
			otherType = lower->getNsNodeType();
		} else {
			thisNode = lower;
			thisType = lower->getNsNodeType();
		}
	}
	DBXML_ASSERT(*thisNode != *otherNode);
	// if nodes are text siblings, there are two cases:
	// 1.  they have the same owner, in which case, the text
	//     index is the indicator
	// 2.  different owner, which means that the one that is
	//     direct child text follows the one that is leading text
	//
	// If only one is nsNodeText, it's necessary
	// to walk the sibling tree to find a non-text sibling for
	// comparison.  Cannot use leading/trailing text because it
	// may not be present.
	if (*const_cast<NsDomNode*>(thisNode)->getNsParentNode() ==
	    *const_cast<NsDomNode*>(otherNode)->getNsParentNode()) {
		if (thisType == otherType && thisType == nsNodeText) {
			const NsDomText *thisText = (const NsDomText*)thisNode;
			const NsDomText *otherText = (const NsDomText*)otherNode;
			int32_t thisIndex = thisText->getIndex();
			int32_t otherIndex = otherText->getIndex();
			if (*(thisText->getOwner()) ==
			    *(otherText->getOwner())) {
				if (thisIndex < otherIndex)
					return TREE_FOLLOWING;
				return TREE_PRECEDING;
			} else {
				// different owners, look for direct child
				// this check using pointers is safe because
				// owner/parent are materialized together
				// in this case
				if (thisText->getOwner() ==
				    const_cast<NsDomText*>(thisText)->getNsParentNode())
					return TREE_PRECEDING;
				else if (otherText->getOwner() ==
				    const_cast<NsDomText*>(otherText)->getNsParentNode())
					return TREE_FOLLOWING;
				else {
					// both leading text for 2
					// sibling elements; compare owner ids
					// assign nodes and fall-through to
					// end of function
					thisNode = thisText->getOwner();
					otherNode = otherText->getOwner();
				}
			}
		} else if (thisType == nsNodeText || otherType == nsNodeText) {
			if (thisType == nsNodeText) {
				// walk backwards to non-text sibling or otherNode
				while (thisNode && (*thisNode != *otherNode) &&
				       (thisNode->getNsNodeType() == nsNodeText))
					thisNode = ((NsDomNode *)thisNode)->
						getNsPrevSibling();
				if (!thisNode) {
					return TREE_FOLLOWING;
				}
				if (*thisNode == *otherNode) {
					return TREE_PRECEDING;
				}
			} else {
				// walk backwards to non-text sibling or thisNode
				while (otherNode && (*thisNode != *otherNode) &&
				       (otherNode->getNsNodeType() == nsNodeText))
					otherNode = ((NsDomNode *)otherNode)->
						getNsPrevSibling();
				if (!otherNode) {
					return TREE_PRECEDING;
				}
				if (*thisNode == *otherNode) {
					return TREE_FOLLOWING;
				}
			}
		}
	}

	// just compare node ids
	int ret = thisNode->getNodeId()->compareNids(otherNode->getNodeId());
	if (ret == 0) {
		return TREE_EQUIVALENT;
	}
	if (ret < 0) {
		return TREE_FOLLOWING;
	}
	return TREE_PRECEDING;
}

// see namespace prefix lookup in DOM spec:
// http://www.w3.org/TR/2004/REC-DOM-Level-3-Core-20040407/namespaces-algorithms.html#lookupNamespacePrefixAlgo
//
/* static */ const xmlch_t *
NsDomNode::lookupPrefix(NsDomElement *node, const xmlch_t *uri,
			bool useDefault, NsDomElement *origEl)
{
	if (node->getNsNodeType() != nsNodeElement)
		return 0;

	NsDomElement *elem = (NsDomElement *) node;
	const xmlch_t *nodeNs = elem->getNsUri();
	const xmlch_t *nodePrefix = elem->getNsPrefix();
	if (nodeNs && NsUtil::nsStringEqual(nodeNs, uri)) {
		if (useDefault || nodePrefix) {
			const xmlch_t *ns = lookupUri(origEl, nodePrefix);
			if (ns && NsUtil::nsStringEqual(ns, uri))
				return nodePrefix;
		}
	}
	if (elem->hasNsAttributes() && elem->getNsNode()->hasNsinfo()) {
		NsDomNamedNodeMap *map = elem->getNsAttributes();
		for (int i = 0; i < map->getNsLength(); i ++) {
			NsDomAttr *attr = map->getNsItem(i);
			const xmlch_t *attrPrefix = attr->getNsPrefix();
			const xmlch_t *value = attr->getNsNodeValue();
			nodeNs = attr->getNsUri();
			if (nodeNs && NsUtil::nsStringEqual(nodeNs,
							    _xmlnsUri16)) {
				// xmlns=value or xmlns:prefix=value
				if ((useDefault &&
				     NsUtil::nsStringEqual(attr->getNsNodeName(),
							   _xmlnsPrefix16)) ||
				    (attrPrefix &&
				     NsUtil::nsStringEqual(attrPrefix,
							   _xmlnsPrefix16)) &&
				    NsUtil::nsStringEqual(value, uri)) {
					const xmlch_t *lname =
						attr->getNsLocalName();
					const xmlch_t *ns =
						lookupUri(origEl, lname);

					if (ns && NsUtil::nsStringEqual(ns, uri))
						return lname;
				}
			}
		}
	}
	elem = elem->getNsParentNode();
	if (elem && !elem->isDocumentNode())
		return lookupPrefix(elem, uri, useDefault, origEl);
	return 0;
}

// see namespace uri lookup in DOM spec:
// http://www.w3.org/TR/2004/REC-DOM-Level-3-Core-20040407/namespaces-algorithms.html#lookupNamespaceURIAlgo
//
/* static */ const xmlch_t *
NsDomNode::lookupUri(const NsDomNode *node, const xmlch_t *prefix)
{
	if (node->getNsNodeType() != nsNodeElement)
		return 0;

	const xmlch_t *nodeNs = node->getNsUri();
	const xmlch_t *nodePrefix = node->getNsPrefix();
	if (nodeNs) {
		if (!prefix && !nodePrefix)
			return nodeNs; // default namespace
		else if (nodePrefix && NsUtil::nsStringEqual(nodePrefix, prefix))
			return nodeNs;
	}

	NsDomElement *elem = (NsDomElement *)node;
	if (elem->hasNsAttributes() && elem->getNsNode()->hasNsinfo()) {
		NsDomNamedNodeMap *map = elem->getNsAttributes();
		for (int i = 0; i < map->getNsLength(); i ++) {
			NsDomAttr *attr = map->getNsItem(i);
			const xmlch_t *attrPrefix = attr->getNsPrefix();
			const xmlch_t *value = attr->getNsNodeValue();
			nodeNs = attr->getNsUri();
			if (nodeNs && NsUtil::nsStringEqual(nodeNs,
							    _xmlnsUri16)) {
				if (!prefix &&
				    NsUtil::nsStringEqual(attr->getNsNodeName(),
							  _xmlnsPrefix16)) {
					// default namespace
					return value;
				} else if (attrPrefix &&
					   NsUtil::nsStringEqual(
						   attrPrefix,
						   _xmlnsPrefix16) &&
					   NsUtil::nsStringEqual(
						   attr->getNsLocalName(),
						   prefix)) {
					// non-default namespace
					return value;
				}
			}
		}
	}
	// keep looking
	elem = elem->getNsParentNode();
	if (elem && !elem->isDocumentNode())
		return lookupUri(elem, prefix);
	return 0;
}
