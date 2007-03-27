//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//      Sleepycat Software.  All rights reserved.
//
// $Id: NsDom.cpp,v 1.51 2005/12/07 15:58:17 jsnelson Exp $
//

#include "NsUtil.hpp"
#include "NsDocument.hpp"
#include "NsConstants.hpp"
#include "NsDom.hpp"
#include <xercesc/framework/MemoryManager.hpp>

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

static void NsDomErrNotFound(const nid_t *nid, int line)
{
	NsUtil::nsThrowException(XmlException::INTERNAL_ERROR,
				 "Cannot find persistent node",
				 __FILE__, line);
}

void
NsDomString::clear(XER_NS MemoryManager *mmgr)
{
	if(_text && _owned)
		mmgr->deallocate((void*)_text);
	_text = 0;
	_owned = false;
}

//
// NsDomFactory implementation
//
NsDomElement *
NsDomFactory::createNsDomElement(NsDocument *doc, const xmlch_t *uri,
				 const xmlch_t *qname)
{
	nsNode_t *node = NsUtil::nsAllocNode(doc->getMemoryManager(),
					     0, NS_STANDALONE);
	const xmlch_t *lname = qname;
	if(uri != 0) {
		// Set the uri
		int uriId = doc->addUri(uri, -1, /*isUTF8*/false,
					/*isDonated*/false, 0);
		nsSetUriIndex(node, uriId);
		node->nd_header.nh_flags |= NS_HASURI;

		// Find the localname
		while(*lname != 0 && *(lname++) != ':');
		if(*lname == 0) {
			lname = qname;
			nsSetNamePrefix(node, NS_NOPREFIX);
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
			int prefixId = doc->addPrefix(prefix, uriId,
						      /*isUTF8*/false,
						      /*isDonated*/false, 0);
			nsSetNamePrefix(node, prefixId);
			node->nd_header.nh_flags |= NS_NAMEPREFIX;

			doc->getMemoryManager()->deallocate(prefix);
		}
	}
	else {
		nsSetNamePrefix(node, NS_NOPREFIX);
	}

	// Set the localname
	xmlbyte_t *name8 = 0;
	int nlen8 = NsUtil::nsToUTF8(doc->getMemoryManager(), &name8,
				     lname, NsUtil::nsStringLen(lname) + 1, 0);
	NsUtil::nsCreateText(doc->getMemoryManager(),
			     &node->nd_header.nh_name.n_text, name8,
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

//
// NsDomElement implementation
//
NsDomElement::NsDomElement(NsDocument *document,
			   nsNode_t *node,
			   NsDomElement *parent,
			   bool standalone) :
	NsDomNav(parent), _document(document), _node(node), _clIndex(-1),
	_firstChild(0), _lastChild(0),
	_elemChildren(document->getMemoryManager()),
	_textChildren(document->getMemoryManager()),
	_childNodeList(0), _attributes(0), _lname(0),
	_nsElemFirstChild(0), _nsElemLastChild(0),
	_nsElemPrev(0), _nsElemNext(0)
{
	_elemChildren.resize(nsNumChildElem(_node));
	_textChildren.resize(nsNumText(_node));
	if (nsIsDoc(node)) {
		_qname.set(_nsDomDocName, false);
		_lname = _qname.get();
	}
}

void
NsDomElement::destroy()
{
	_qname.clear(_document->getMemoryManager());
	_textContent.clear(_document->getMemoryManager());
	if (_node) {
		NsUtil::nsFreeNode(_document->getMemoryManager(),
				   _node);
	}
	_elemChildren.clear();
	_textChildren.clear();
}

void
NsDomElement::_getName() const
{
	bool owned = false;
	const xmlch_t *qname = _document->getQname(nsName(_node),
						   nsIsUTF16(_node),
						   owned);
	DBXML_ASSERT(qname);
	_qname.set(qname, owned);
	_lname = qname;
	if (nsNamePrefix(_node) != NS_NOPREFIX)
		while (*_lname++ != xmlchColon);
}

bool
NsDomElement::isNativeUTF8() const
{
	return !nsIsUTF16(_node);
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
	if(nsIsUTF16(_node)) return 0;
	return (const xmlbyte_t*)nsName(_node)->n_text.t_chars;
}

const xmlbyte_t *
NsDomElement::getNsPrefix8() const
{
	if (nsIsDoc(_node))
		return 0;
	return _document->getPrefix8(nsNamePrefix(_node));
}

const xmlbyte_t *
NsDomElement::getNsUri8() const
{
	if (nsIsDoc(_node) || !nsHasUri(_node))
		return 0;
	return _document->getUri8(nsUriIndex(_node));
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
	if (!_qname.get()) {
		_getName();
		DBXML_ASSERT(_qname.get() && _lname);
	}
	return _qname.get();
}

void
NsDomElement::nsRename(const xmlch_t *uri, const xmlch_t *qname)
{
	nsMakeTransient();

	const xmlch_t *lname = qname;
	if(uri != 0) {
		// Set the uri
		int uriId = getNsDocument()->addUri(uri, -1, /*isUTF8*/false,
						    /*isDonated*/false, 0);
		nsSetUriIndex(_node, uriId);
		_node->nd_header.nh_flags |= NS_HASURI;

		// Find the localname
		while(*lname != 0 && *(lname++) != ':');
		if(*lname == 0) {
			lname = qname;
			_node->nd_header.nh_flags &= ~NS_NAMEPREFIX;
			nsSetNamePrefix(_node, NS_NOPREFIX);
		}
		else {
			// Copy the prefix
			unsigned int plen = lname - qname;
			xmlch_t *prefix = (xmlch_t*)getNsDocument()->getMemoryManager()->
				allocate(plen * sizeof(xmlch_t));
			plen -= 1;
			memcpy(prefix, qname, plen * sizeof(xmlch_t));
			prefix[plen] = 0;

			// Set the prefix
			int prefixId = getNsDocument()->addPrefix(prefix, uriId,
								  /*isUTF8*/false,
								  /*isDonated*/false, 0);
			nsSetNamePrefix(_node, prefixId);
			_node->nd_header.nh_flags |= NS_NAMEPREFIX;

			getNsDocument()->getMemoryManager()->deallocate(prefix);
		}
	}
	else {
		_node->nd_header.nh_flags &= ~NS_HASURI;
		_node->nd_header.nh_flags &= ~NS_NAMEPREFIX;
		nsSetNamePrefix(_node, NS_NOPREFIX);
	}

	// Set the localname
	getNsDocument()->getMemoryManager()->
		deallocate(_node->nd_header.nh_name.n_text.t_chars);

	if(nsIsUTF16(_node)) {
		NsUtil::nsCreateText(getNsDocument()->getMemoryManager(),
				     &_node->nd_header.nh_name.n_text, lname,
				     NsUtil::nsStringLen(lname),
				     /*isUTF8*/false, /*isDonated*/false);
	}
	else {
		xmlbyte_t *name8 = 0;
		int nlen8 = NsUtil::nsToUTF8(
			getNsDocument()->getMemoryManager(), &name8,
			lname, NsUtil::nsStringLen(lname) + 1, 0);
		NsUtil::nsCreateText(getNsDocument()->getMemoryManager(),
				     &_node->nd_header.nh_name.n_text, name8,
				     nlen8 - 1,
				     /*isUTF8*/true, /*isDonated*/true);
	}

	// clear the cached name
	_qname.clear(getNsDocument()->getMemoryManager());

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
		return _removeNsText((NsDomText*)child);
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
	nsMakeTransient();

	// Relink the child list,and parent first/last
	// child pointers
	if(child->getNsNextSibling()) {
		child->getNsNextSibling()->
			setNsPrevSib(child->getNsPrevSibling());
	}
	else {
		// No next sibling, so it must be the last child
		_lastChild = child->getNsPrevSibling();
	}

	if(child->getNsPrevSibling()) {
		child->getNsPrevSibling()->
			setNsNextSib(child->getNsNextSibling());
	}
	else {
		// No previous sibling, so it must be the first child
		_firstChild = child->getNsNextSibling();
	}

	// Remove the text node from the _textChildren cache,
	// and rewrite the indexes of following text that is
	// already materialized
	int index = child->getIndex();
	int numText = nsNumText(_node) - 1;
	NsDomText *text;
	for(int i = index; i < numText; ++i) {
		text = _textChildren[i + 1];
		if(text != 0)
			text->setIndex(i);
		_textChildren[i] = text;
	}
	_textChildren.resize(numText);

	// Remove text from the nsNode_t
	child->makeStandalone();
	NsUtil::nsRemoveText(getNsDocument()->getMemoryManager(),
			     _node, index);

	getNsDocument()->addToModifications(NodeModification::UPDATE, this);
	return child;
}

NsDomElement *
NsDomElement::_removeNsElement(NsDomElement *child)
{
	nsMakeTransient();

	// Relink the child list,and parent first/last
	// child pointers
	if(child->getNsNextSibling()) {
		child->getNsNextSibling()->
			setNsPrevSib(child->getNsPrevSibling());
	}
	else {
		// No next sibling, so it must be the last child
		_lastChild = child->getNsPrevSibling();
	}

	if(child->getNsPrevSibling()) {
		child->getNsPrevSibling()->
			setNsNextSib(child->getNsNextSibling());
	}
	else {
		// No previous sibling, so it must be the first child
		_firstChild = child->getNsNextSibling();
	}

	// Modify the element's sibling elements to point to the correct
	// next and previous nid_t
	NsDomElement *next = child->getElemNext();
	NsDomElement *prev = child->getElemPrev();
	if(next) {
		next->setElemPrev(prev);
	}
	else {
		// No next element sibling, so it must be the last element child
		_nsElemLastChild = prev;
	}
	if(prev) {
		prev->setElemNext(next);
	}
	else {
		// No previous element sibling, so it must be the first element child
		_nsElemFirstChild = next;
	}

	// Remove the element node from the _elemChildren cache,
	// and rewrite the indexes of following elements that are
	// already materialized
	int index = child->_clIndex;
	int numElem = numNsChildElements() - 1;
	NsDomElement *element;
	for(int i = index; i < numElem; ++i) {
		element = _elemChildren[i + 1];
		if(element != 0)
			element->_clIndex = i;
		_elemChildren[i] = element;
	}
	_elemChildren.resize(numElem);

	// Remove the child from the tree
	child->_makeStandalone();
	child->_clIndex = -1;
	child->_parent = 0;
	child->_prevSib = 0;
	child->_nextSib = 0;
	child->_nsElemPrev = 0;
	child->_nsElemNext = 0;

	// Remove child from the nsNode_t
	NsUtil::nsRemoveChild(getNsDocument()->getMemoryManager(),
			      _node, index);

	getNsDocument()->addToModifications(NodeModification::UPDATE, this);
	return child;
}

NsDomNav *
NsDomElement::insertNsChild(NsDomNode *child, NsDomNode *refChild)
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
		return _insertNsElement((NsDomElement*)child, (NsDomNav*)refChild);
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

	// Find the index for the new text, and the previous child
	uint32_t index = 0;
	int elemIndex = 0;
	NsDomNav *prevChild = 0;
	if(nextChild != 0) {
		prevChild = nextChild->getNsPrevSibling();

		if(nextChild->getNsNodeType() == nsNodeElement) {
			NsDomElement *refElement = (NsDomElement*)nextChild;
			index = refElement->getTextIndex();
			elemIndex = refElement->_clIndex;
		}
		else {
			NsDomText *refText = (NsDomText*)nextChild;
			index = refText->getIndex();

			nsChildList_t *childList = _node->nd_child;
			if(childList) {
				int numElem = childList->cl_numChild;
				while(elemIndex < numElem && childList->
				      cl_child[elemIndex].ce_textIndex <= index)
					++elemIndex;
			}
		}
	}
	else {
		prevChild = getNsLastChild();
		elemIndex = numNsChildElements();
		index = nsNumText(_node);
	}

	// Insert text into the nsNode_t
	if(nsTextType(child->getNsTextType()) == NS_PINST) {
		NsUtil::nsInsertPI(getNsDocument()->getMemoryManager(),
				   _node, index, child->getNsNodeName(),
				   child->getNsNodeValue(), elemIndex);
	}
	else {
		NsUtil::nsInsertText(getNsDocument()->getMemoryManager(),
				     _node, index, child->getNsNodeValue(),
				     child->getNsTextType(), elemIndex);
	}

	// Insert the text node into the _textChildren cache
	// and rewrite the indexes of following text that is
	// already materialized
	_textChildren.insert(index, child);
	int numText = _textChildren.size();
	NsDomText *text;
	for(int i = index + 1; i < numText; ++i) {
		text = _textChildren[i];
		if(text != 0)
			text->setIndex(i);
	}

	// Relink the child list,and parent first/last
	// child pointers
	child->makeNonStandalone(this, index, prevChild, nextChild);
	if(prevChild == 0) _firstChild = child;
	else prevChild->setNsNextSib(child);
	if(nextChild == 0) _lastChild = child;
	else nextChild->setNsPrevSib(child);

	getNsDocument()->addToModifications(NodeModification::UPDATE, this);
	return child;
}

NsDomElement *
NsDomElement::_insertNsElement(NsDomElement *child, NsDomNav *nextNode)
{
	nsMakeTransient();

	// Find the previous and next elements, and the previous node
	NsDomNav *prevNode;
	NsDomElement *previous, *next;
	uint32_t textIndex;
	if(nextNode != 0) {
		if(nextNode->getNsNodeType() == nsNodeElement) {
			next = (NsDomElement*)nextNode;
			previous = next->getElemPrev();
			textIndex = next->getTextIndex();
		}
		else {
			NsDomNav *tmp = nextNode->getNsPrevSibling();
			while(tmp != 0 && tmp->getNsNodeType() != nsNodeElement)
				tmp = tmp->getNsPrevSibling();
			previous = (NsDomElement*)tmp;

			if(previous == 0) {
				NsDomNav *tmp = nextNode->getNsNextSibling();
				while(tmp != 0 && tmp->getNsNodeType() != nsNodeElement)
					tmp = tmp->getNsNextSibling();
				next = (NsDomElement*)tmp;
			}
			else {
				next = previous->getElemNext();
			}

			textIndex = ((NsDomText*)nextNode)->getIndex();
		}
		prevNode = nextNode->getNsPrevSibling();
	}
	else {
		prevNode = this->getNsLastChild();
		previous = this->getElemLastChild();
		next = 0;
		textIndex = nsNumText(_node);
	}
	int elemIndex = next == 0 ? numNsChildElements() : next->_clIndex;

	// Find the preceding element, in document order
	NsDomElement *preceding;
	if(previous == 0) {
		preceding = this;
	}
	else {
		preceding = previous;
		while(preceding->getElemLastChild() != 0)
			preceding = preceding->getElemLastChild();
	}

	// Find the following element, in document order
	// (There may not be one)
	NsDomElement *following = next;
	if(following == 0) {
		following = this;
		while(following != 0 && following->getElemNext() == 0)
			following = following->getElemParent();
		if(following != 0)
			following = following->getElemNext();
	}

	// Link in the child
	if(prevNode == 0) _firstChild = child;
	else {
		prevNode->setNsNextSib(child);
		child->setNsPrevSib(prevNode);
	}
	if(nextNode == 0) _lastChild = child;
	else {
		nextNode->setNsPrevSib(child);
		child->setNsNextSib(nextNode);
	}

	// Add the new node to the _elemChildren cache,
	// and rewrite the indexes of following elements that are
	// already materialized
	_elemChildren.insert(elemIndex, child);
	int numElem = _elemChildren.size();
	NsDomElement *element;
	for(int i = elemIndex + 1; i < numElem; ++i) {
		element = _elemChildren[i];
		if(element != 0)
			element->_clIndex = i;
	}

	// Add an nsChildEntry_t for the new child
	NsUtil::nsInsertChild(getNsDocument()->getMemoryManager(),
			      _node, elemIndex, textIndex);

	// Allocate IDs to the new child, and it's descendents
	child->_makeNonStandalone(this, elemIndex, previous, next, preceding, following);

	getNsDocument()->addToModifications(NodeModification::UPDATE, this);
	return child;
}

NsDomElement *
NsDomElement::_makeNonStandalone(NsDomElement *parent, int elemIndex,
				 NsDomElement *previous, NsDomElement *next,
				 NsDomElement *preceding, NsDomElement *following)
{
	DBXML_ASSERT(nsIsStandalone(_node));

	if(!nsIsStandalone(parent->_node)) {
		_node->nd_header.nh_flags &= ~NS_STANDALONE;
	}
	_clIndex = elemIndex;

	if(parent->_node->nd_header.nh_flags & NS_ISDOCUMENT) {
		_node->nd_header.nh_flags |= NS_ISROOT;
	}
	setElemParent(parent);
	if(previous == 0) {
		parent->_nsElemFirstChild = this;
	}
	else {
		previous->setElemNext(this);
		setElemPrev(previous);
	}
	if(next == 0) {
		parent->_nsElemLastChild = this;
	}
	else {
		next->setElemPrev(this);
		setElemNext(next);
	}

	if(nsIsStandalone(_node)) return 0;

	// Generate an id for ourselves
	NsUtil::nsBetweenId(getNsDocument()->getMemoryManager(),
			    &(_node->nd_header.nh_id), nsGetNid(preceding->_node),
			    following ? nsGetNid(following->_node) : 0);

	_node->nd_header.nh_flags &= ~NS_STANDALONE;
	_node->nd_level = parent->_node->nd_level + 1;

	// Set our id in our parent's nsChildList_t
	nsChildEntry_t *parentEntry = &parent->_node->nd_child->cl_child[elemIndex];
	NsUtil::nsCopyId(getNsDocument()->getMemoryManager(),
			 &parentEntry->ce_id, &(_node->nd_header.nh_id));

	// Set our parent id
	NsUtil::nsCopyId(getNsDocument()->getMemoryManager(),
			 &(_node->nd_header.nh_parent),
			 &(parent->_node->nd_header.nh_id));

	// Loop over our children, doing the same
	int childIndex = 0;
	NsDomElement *child_preceding = this;
	NsDomElement *child_previous = 0;
	NsDomElement *child = getElemFirstChild();
	while(child != 0) {
		child_preceding = child->
			_makeNonStandalone(this, childIndex, child_previous, 0,
					   child_preceding, following);

		child_previous = child;
		child = child->getElemNext();
		++childIndex;
	}

	getNsDocument()->addToModifications(NodeModification::ADD, this);
	return child_preceding;
}

void
NsDomElement::_makeStandalone()
{
	if(!nsIsStandalone(_node)) {
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

		// Remove our id and our parent's id from the nsNode_t
		NsUtil::nsFreeId(getNsDocument()->getMemoryManager(),
				 &(_node->nd_header.nh_id));
		memset(&(_node->nd_header.nh_id), 0, sizeof(nid_t));
		NsUtil::nsFreeId(getNsDocument()->getMemoryManager(),
				 &(_node->nd_header.nh_parent));
		memset(&(_node->nd_header.nh_parent), 0, sizeof(nid_t));
		_node->nd_header.nh_flags |= NS_STANDALONE;
		_node->nd_level = 0;
	}
}

const xmlch_t *
NsDomElement::getNsLocalName() const
{
	if (!_lname) {
		_getName();
		DBXML_ASSERT(_qname.get() && _lname);
	}
	return _lname;
}

const xmlch_t *
NsDomElement::getNsPrefix() const
{
	if (nsIsDoc(_node))
		return 0;
	return _document->getPrefix(nsNamePrefix(_node));
}

void NsDomElement::setNsPrefix(const xmlch_t *prefix)
{
	if (nsIsDoc(_node) || !nsHasUri(_node))
		return;

	int prefixId = _document->
		addPrefix(prefix, nsUriIndex(_node), /*isUTF8*/false,
			  /*isDonated*/false, 0);
	if(nsNamePrefix(_node) != prefixId) {
		nsSetNamePrefix(_node, prefixId);

		_qname.clear(_document->getMemoryManager());
		_lname = 0;

		_document->addToModifications(NodeModification::UPDATE, this);
	}
}

const xmlch_t *
NsDomElement::getNsUri() const
{
	if (nsIsDoc(_node) || !nsHasUri(_node))
	    return 0;
	return _document->getUri(nsUriIndex(_node));
}

const xmlch_t *
NsDomElement::getNsNodeValue() const
{
	return 0;
}

NsDomElement *
NsDomElement::getElemParent(bool fetch)
{
	// There should always be a parent
	DBXML_ASSERT(_parent || nsIsDoc(_node) || nsIsStandalone(_node));
	return _parent;
}

NsDomElement *
NsDomElement::getElemChild(int index, bool fetch)
{
	if((uint32_t)index >= numNsChildElements() ||
	   index < 0) return 0;

	NsDomElement *child = _elemChildren[index];
	if(fetch && !child && !nsIsStandalone(_node)) {
		child = _document->fetchDomElement(nsChildNid(_node, index));
		if (!child)
			NsDomErrNotFound(nsChildNid(_node, index), __LINE__);

		child->_clIndex = index;
		child->setElemParent(this);
		_elemChildren[index] = child;
		
		if(index == 0) _nsElemFirstChild = child;
		if(index == (int)((numNsChildElements() - 1)))
			_nsElemLastChild = child;
	}
	DBXML_ASSERT(!child || (child->getElemParent() == this));
	return child;
}

int
NsDomElement::childBinarySearch(const nid_t *nid, bool &exact)
{
	// Binary searches the nsChildList_t for the child with the
	// bigest NID smaller than or equal to the given NID.
	int min = 0;
	int max = numNsChildElements() - 1;
	int index = -1;
	int middle, compare;
	const nid_t *middle_nid;
	while(min <= max) {
		middle = (min + max) >> 1;

		middle_nid = &nsChild(_node, middle)->ce_id;
		if(middle_nid->idLen == 0) {
			// This is a transient DOM, so they child must exist in the _elemChildren list
			DBXML_ASSERT(_elemChildren[middle] != 0);
			middle_nid = _elemChildren[middle]->getNodeId();
		}

		compare = NsUtil::nsCompareIds(middle_nid, nid);

		if(compare == 0) {
			exact = true;
			return middle;
		}
		else if(compare < 0) {
			// guess is too small, so try bigger
			index = middle;
			min = middle + 1;
		}
		else { // compare > 0
			// guess is too big, so try smaller
			max = middle - 1;
		}
	}

	exact = false;
	return index; // The index the child is a descendant of
}

NsDomElement *
NsDomElement::findDescendantElement(const nid_t *nid)
{
	if(!nsHasChildElem(_node)) return 0;

	NsDomElement *result = this;
	int index;
	bool exact;
	do {
		index = result->childBinarySearch(nid, exact);

		if(exact) {
			// We fetch it, because we've found an exact match
			return result->getElemChild(index, /*fetch*/true);
		}

		if(index == -1) return 0;

		result = result->getElemChild(index, /*fetch*/true);
	} while(result != 0);

	return 0;
}

NsDomElement *
NsDomElement::getElemPrev(bool fetch)
{
	NsDomElement *prev = _nsElemPrev;
	if (!prev && !nsIsDoc(_node) && !nsIsStandalone(_node) && fetch) {
		DBXML_ASSERT(_parent);

		prev = _parent->getElemChild(_clIndex - 1);
		_nsElemPrev = prev;
		if(prev) prev->setElemNext((NsDomElement*)this);
	}
	DBXML_ASSERT(!prev || (prev->_nsElemNext == this));
	return prev;
}

NsDomElement *
NsDomElement::getElemNext(bool fetch)
{
	NsDomElement *next = _nsElemNext;
	if (!next && !nsIsDoc(_node) && !nsIsStandalone(_node)  && fetch) {
		DBXML_ASSERT(_parent);

		next = _parent->getElemChild(_clIndex + 1);
		_nsElemNext = next;
		if(next) next->setElemPrev(this);
	}
	DBXML_ASSERT(!next || (next->_nsElemPrev == this));
	return next;
}

NsDomElement *
NsDomElement::getElemLastChild(bool fetch)
{
	return getElemChild(numNsChildElements() - 1, fetch);
}

NsDomElement *
NsDomElement::getElemFirstChild(bool fetch)
{
	return getElemChild(0, fetch);
}

//
// Methods introduce in NsDomElement -- child navigation
//
NsDomNav *
NsDomElement::getNsFirstChild()
{
	// may be 1) NULL 2) first text node 3) child elem
	// It depends on presence of text nodes and leading
	// text on any child elements.  If leading text, materialize
	// all text nodes before 1st child element.

	NsDomNav *child = _firstChild;
	if (!child && nsHasChildNode(_node)) {
		bool hasText = nsHasText(_node);
		uint32_t endIndex = nsNumText(_node);

		NsDomElement *firstChildElem = 0;
		if (nsHasChildElem(_node)) {
			firstChildElem = getElemFirstChild();
			DBXML_ASSERT(firstChildElem);
			firstChildElem->setNsParent(this);

			// if the current node has no text, OR the
			// child element has no leading text, the child
			// element is first
			endIndex = firstChildElem->getTextIndex();
			if (!hasText || (endIndex == 0)) {
				_firstChild = firstChildElem;
				return firstChildElem;
			}
		}

		DBXML_ASSERT(hasText);
		// make all leading text nodes (may only be one).  Append
		// firstChildElem as "next" sibling, if it's non-null
		child = makeTextNodes(0, endIndex, 0, NULL, firstChildElem);
		_firstChild = child;

		if (!firstChildElem) {
			// all text nodes; set lastChild, too
			NsDomNav *cur = child;
			while (cur->getNextSib())
				cur = cur->getNextSib();
			_lastChild = cur;
		}
	}
	// optionally skip NS_ENTSTART, NS_ENTEND text nodes
	if(child && child->getNsIsEntityType() &&
	   !(getNsDocument()->getCreateEntityText())) {
		child = child->getNsNextSibling();
	}
	return child;
}

NsDomNav *
NsDomElement::getNsLastChild()
{
	// may be 1) NULL 2) last text node 3) a child elem
	// It depends on presence of text nodes and trailing
	// text on the last child element, if present

	NsDomNav *child = _lastChild;
	if (!child && nsHasChildNode(_node)) {
		NsDomElement *lastChildElem = 0;
		bool hasText = nsHasText(_node);
		uint32_t textIndex = 0;
		uint32_t lastIndex = nsNumText(_node);

		if (nsHasChildElem(_node)) {
			lastChildElem = getElemLastChild();
			lastChildElem->setNsParent(this);
			DBXML_ASSERT(lastChildElem);
			textIndex = lastChildElem->getTextIndex();

			// if the current node has no text, OR the
			// child element has no trailing text, the child
			// element is last
			if (!hasText || (textIndex == lastIndex)) {
				_lastChild = lastChildElem;
				return lastChildElem;
			}
		}
		DBXML_ASSERT(hasText);
		// lastChildElem, from above, if set, come before this
		// range of text nodes, so use it.
		child = makeTextNodes(textIndex, lastIndex,
				      lastIndex - 1, lastChildElem, NULL);
		DBXML_ASSERT(child);
		_lastChild = child;

		if (!lastChildElem) {
			// all text nodes; set firstChild, too
			NsDomNav *cur = child;
			while (cur->getPrevSib())
				cur = cur->getPrevSib();
			_firstChild = cur;
		}
	}
	// optionally skip NS_ENTSTART, NS_ENTEND text nodes
	if(child && child->getNsIsEntityType() &&
	   !(getNsDocument()->getCreateEntityText())) {
		child = child->getNsPrevSibling();
	}
	return child;
}

NsDomElement *
NsDomElement::getNsParentNode()
{
	return _parent;
}

// get interior siblings -- next, previous
NsDomNav *
NsDomElement::getNsNextSibling()
{
	NsDomNav *sib = getNextSib();
	if (!sib) {
		// sibling is either text or next element
		NsDomElement *nextElem = getElemNext();
		uint32_t startIndex = getTextIndex();
		uint32_t endIndex = getElemParent() ? nsNumText(getElemParent()->_node) : 0;
		if (nextElem) {
			// if element has leading text, the text is the
			// endIndex for one of more text nodes;
			// otherwise the target is the element itself
			endIndex = nextElem->getTextIndex();
			if (endIndex == startIndex) {
				_nextSib = nextElem;
				nextElem->setNsPrevSib(this);
				// element plumbing already set
				return nextElem;
			}
		}

		if(startIndex != endIndex) {
			NsDomElement *parent = getNsParentNode();
			DBXML_ASSERT(parent);
			sib = parent->makeTextNodes(startIndex, endIndex, startIndex,
						    this, nextElem);

			// did we just materialize the _lastChild?
			if (!nextElem) {
				NsDomNav *cur = sib;
				while (cur->getNextSib())
					cur = cur->getNextSib();
				parent->_lastChild = cur;
			}
			DBXML_ASSERT(getNextSib() == sib);
		}
	}
	if(sib && sib->getNsIsEntityType() &&
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
		// sibling is either text or previous element
		NsDomElement *prevElem = getElemPrev();
		int startIndex = 0;
		int endIndex = getTextIndex();
		if(prevElem) {
			startIndex = prevElem->getTextIndex();
			if(startIndex == endIndex) {
				_prevSib = prevElem;
				prevElem->setNsNextSib(this);
				return prevElem;
			}
		}

		if(startIndex != endIndex) {
			NsDomElement *parent = getNsParentNode();
			DBXML_ASSERT(parent);
			sib = parent->makeTextNodes(startIndex, endIndex, endIndex -1,
						    prevElem, this);
			// did we just materialize the _firstChild?
			if (!prevElem) {
				NsDomNav *cur = sib;
				while (cur->getPrevSib())
					cur = cur->getPrevSib();
				parent->_firstChild = cur;
			}
			DBXML_ASSERT(getPrevSib() == sib);
		}
	}
	if(sib && sib->getNsIsEntityType() &&
	   !(getNsDocument()->getCreateEntityText())) {
		sib = sib->getNsPrevSibling();
	}
	return sib;
}

NsNodeType_t
NsDomElement::getNsNodeType() const
{
	if (nsIsDoc(_node))
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

static int
_processText(const nsNode_t *node, int startIndex, int endIndex,
	     struct textContent &buf)
{
	while (startIndex < endIndex) {
		nsTextEntry_t *tp = &(node->nd_text->tl_text[startIndex]);
		if (nsTextType(tp->te_type) == NS_TEXT ||
		    nsTextType(tp->te_type) == NS_CDATA) {
			uint32_t needed = tp->te_text.t_len;
			_reserve(buf, needed);
			if (nsIsUTF16(node)) {
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
	return endIndex;
}

//
// Acquiring text content recursively may materialize
// element nodes, but will not materialize NsDomText
// or other text nodes. Text is read directly from nsNode_t structures.
//
static void
_getNsTextContent(NsDomElement *current,
		  struct textContent &buf)
{
	const nsNode_t *node = current->getNsNode();
	uint32_t lastIndex = 0;
	NsDomElement *child = current->getElemFirstChild();
	while(child) {
		lastIndex = _processText(node, lastIndex, child->getTextIndex(), buf);
		_getNsTextContent(child, buf);
		child = child->getElemNext();
	}
	_processText(node, lastIndex, nsNumText(node), buf);
}

// not const, because of caching side effects
const xmlch_t *
NsDomElement::getNsTextContent() const
{
	if (nsIsDoc(_node))
		return 0;

	// We recalculate the text content every time, just in case there
	// have been tree modifications. - jpcs
	_textContent.clear(getNsDocument()->getMemoryManager());

	if (!_textContent.get()) {
		// concat text content of all child text and element nodes,
		// exclude comments, PIs, attrs
		struct textContent buf(getNsDocument()->getMemoryManager());
		_getNsTextContent(const_cast<NsDomElement*>(this), buf);
		_reserve(buf, 1);
		buf.buf[buf.offset] = 0;
		_textContent.set(buf.buf, true);
	}
	return _textContent.get();
}

void
NsDomElement::setNsTextContent(const xmlch_t *value)
{
	nsMakeTransient();

	// Remove my element descendants
	removeElemDescendants();

	// Remove my text content
	nsTextList_t *text = _node->nd_text;
	if (_node->nd_header.nh_flags & NS_HASTEXT) {
		int32_t ntext = text->tl_ntext;
		for (int i = 0; i < ntext; ++i) {
			getNsDocument()->getMemoryManager()->
				deallocate(text->tl_text[i].te_text.t_chars);
			memset(&text->tl_text[i], 0, sizeof(nsTextEntry_t));
		}

		text->tl_len = 0;
		text->tl_ntext = 0;
		_textChildren.resize(0);
	}

	// Add the value as a text node child
	if(value && *value) {
		NsUtil::nsAddText(getNsDocument()->getMemoryManager(),
				  _node, value, NS_TEXT);
		_textChildren.resize(1);
	}
	else if(_node->nd_header.nh_flags & NS_HASTEXT) {
		nsChildList_t *childList = _node->nd_child;

		getNsDocument()->getMemoryManager()->deallocate(text);

		_node->nd_text = 0;
		_node->nd_header.nh_flags &= ~NS_HASTEXT;

		if(childList) {
			for(uint32_t i = 0; i < childList->cl_numChild; ++i) {
				childList->cl_child[i].ce_textIndex = 0;
			}
		}
	}

	_firstChild = 0;
	_lastChild = 0;
	_nsElemFirstChild = 0;
	_nsElemLastChild = 0;

	getNsDocument()->addToModifications(NodeModification::UPDATE, this);
}

void
NsDomElement::removeElemDescendants()
{
	if (_node->nd_header.nh_flags & NS_HASCHILD) {
		// Remove the children, and their descendants
		NsDomElement *child, *new_child;
		for(child = getElemFirstChild(); child != 0; child = new_child) {
			child->removeElemDescendants();
			getNsDocument()->addToModifications(NodeModification::REMOVE, child);

			new_child = child->getElemNext();
			getNsDomFactory()->destroy(child);
		}

		// Deallocate the child list, if this is a transient node
		if(nsIsAllocated(_node))
			NsUtil::nsFreeChildList(getNsDocument()->getMemoryManager(), _node->nd_child);
		_node->nd_header.nh_flags &= ~NS_HASCHILD;
		_node->nd_child = 0;

		_firstChild = 0;
		_lastChild = 0;
		_nsElemFirstChild = 0;
		_nsElemLastChild = 0;
		_elemChildren.resize(0);

		getNsDocument()->addToModifications(NodeModification::UPDATE, this);
	}
}

// DOM spec appears to require that a namednodemap be returned,
// even if there are no attributes.  So create an empty one.
NsDomNamedNodeMap *
NsDomElement::getNsAttributes()
{
	if (!_attributes) {  // && nsHasAttrs(_node)) {
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
NsDomElement::numNsChildNodes() const
{
	return nsNumChild(_node);
}

uint32_t
NsDomElement::numNsChildElements() const
{
	return nsNumChildElem(_node);
}

uint32_t
NsDomElement::numNsAttributes() const
{
	return nsNumAttrs(_node);
}

const nid_t *
NsDomElement::getNodeId() const
{
	return nsGetNid(_node);
}

int32_t
NsDomElement::getNsLevel() const
{
	return nsGetLevel(_node);
}

void
NsDomElement::nsMakeTransient()
{
	_node = NsUtil::nsMakeTransient(_document->getMemoryManager(),
					_node);
}

NsDomFactory *
NsDomElement::getNsDomFactory() const
{
	return _document->getDomFactory();
}

uint32_t
NsDomElement::getTextIndex()
{
	// materialize the parent, if necessary
	NsDomElement *parent = getElemParent();
	if(parent == 0) return 0;
	return nsChildTextIndex(parent->_node, _clIndex);
}

// utility function to create a range of text nodes.  Must
// be contiguous, and it is assumed that there are no
// intervening elements.  index arguments are inclusive.
// Return the node corresponding to retIndex
//
// NOTE: this function will make NsDomText nodes for the
// "special" text types of NS_ENTSTART and NS_ENTEND
// It is up to higher level code to ignore these specific
// nodes during actual navigation.  The type can be determined
// by getNsNodeType().  Skipping these nodes in this code
// is not sufficient.  It's simplest to let them be created,
// and skip them during navigation.
//
NsDomNav*
NsDomElement::makeTextNodes(int startIndex, int endIndex,
			    int retIndex, NsDomNav *prev, NsDomNav *next)
{
	NsDomText *ret = 0;
	nsTextEntry_t *tlist = _node->nd_text->tl_text;
	NsDomText *newtext = 0;
	DBXML_ASSERT(startIndex <= endIndex);
	for (int i = startIndex; i < endIndex; i++) {
		newtext = _textChildren[i];
		if(newtext == 0) {
			newtext = getNsDomFactory()->
				createNsDomText(this, i, tlist[i].te_type);
			DBXML_ASSERT(newtext);
			// init plumbing
			newtext->setNsParent(this);
			_textChildren[i] = newtext;
		}
		if (prev) {
			prev->setNsNextSib(newtext);
			newtext->setNsPrevSib(prev);
		}
		if (retIndex == i)
			ret = newtext; // return value
		prev = newtext;
	}
	// if next is set, hook it up.  prev is pointing to the last node
	// created
	if (next) {
		prev->setNsNextSib(next);
		next->setNsPrevSib(prev);
	}
	return ret;
}

//
// NsDomText implementation
//
NsDomText::NsDomText(NsDomElement *parent, int index, uint32_t type)
	: NsDomNav(parent),
	  _doc(parent->getNsDocument()),
	  _index(index),
	  _type(type)
{
}

void
NsDomText::makeNonStandalone(NsDomElement *parent, int index,
			     NsDomNav *prevSib, NsDomNav *nextSib)
{
	if(_parent == 0) {
		_text.clear(getNsDocument()->getMemoryManager());
		_value.clear(getNsDocument()->getMemoryManager());
		_parent = parent;
		_doc = _parent->getNsDocument();
		_index = index;
		_prevSib = prevSib;
		_nextSib = nextSib;
	}
}

/// Creates a standalone text type node
NsDomText::NsDomText(NsDocument *doc, const xmlch_t *value, uint32_t type)
	: NsDomNav(0),
	  _doc(doc),
	  _index(-1),
	  _type(type)
{
	DBXML_ASSERT(nsTextType(type) != NS_PINST);
	if(value != 0)
		_text.set(NsUtil::nsStringDup(getNsDocument()->getMemoryManager(),
					      value, 0), true);
}

/// Creates a standalone processing instruction type node
NsDomText::NsDomText(NsDocument *doc, const xmlch_t *target, const xmlch_t *data)
	: NsDomNav(0),
	  _doc(doc),
	  _index(-1),
	  _type(NS_PINST)
{
	if(target != 0)
		_text.set(NsUtil::nsStringDup(getNsDocument()->getMemoryManager(),
					      target, 0), true);
	if(data != 0)
		_value.set(NsUtil::nsStringDup(getNsDocument()->getMemoryManager(),
					       data, 0), true);
}

void
NsDomText::makeStandalone()
{
	if(_parent != 0) {
		xmlch_t *text = 0, *value = 0;
		if(nsTextType(_type) == NS_PINST) {
			text = NsUtil::nsStringDup(getNsDocument()->getMemoryManager(),
						   getNsNodeName(), 0);
			value = NsUtil::nsStringDup(getNsDocument()->getMemoryManager(),
						    getNsNodeValue(), 0);
		}
		else {
			text = NsUtil::nsStringDup(getNsDocument()->getMemoryManager(),
						   getNsNodeValue(), 0);
		}

		_text.clear(getNsDocument()->getMemoryManager());
		_value.clear(getNsDocument()->getMemoryManager());
		_parent = 0;
		_index = -1;
		_prevSib = 0;
		_nextSib = 0;

		if(text != 0) _text.set(text, true);
		if(value != 0) _value.set(value, true);
	}
}

void
NsDomText::destroy()
{
	_text.clear(getNsDocument()->getMemoryManager());
	_value.clear(getNsDocument()->getMemoryManager());
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
	return nsIsEntityText(_type);
}

NsDomElement *
NsDomText::getNsParentNode()
{
	return _parent;
}

NsDomNav *
NsDomText::getNsNextSibling()
{
	NsDomNav *sib = getNextSib();
	if (!sib) {
		// Because of the way text nodes are created, this
		// means that there is no next sibling.
		// Text nodes are never created without also using
		// adjacent elements, when they are present.  They
		// are also always created as a sequence.
	}
	if(sib && sib->getNsIsEntityType() &&
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
		// Because of the way text nodes are created, this
		// means that there is no previous sibling.
		// Text nodes are never created without also using
		// adjacent elements, when they are present.  They
		// are also always created as a sequence.
	}
	if(sib && sib->getNsIsEntityType() &&
	   !(getNsDocument()->getCreateEntityText())) {
		sib = sib->getNsPrevSibling();
	}
	return sib;
}

// text nodes are one level down from parents
int32_t
NsDomText::getNsLevel() const
{
	if(_parent)
		return _parent->getNsLevel() + 1;
	else return -1;

}

bool
NsDomText::isNativeUTF8() const
{
	return _parent != 0 && !nsIsUTF16(_parent->getNsNode());
}

const xmlbyte_t *
NsDomText::getNsNodeName8() const
{
	if(_parent != 0 && nsTextType(_type) == NS_PINST) {
		return (const xmlbyte_t *)nsText(_parent->getNsNode(), _index)->t_chars;
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
	if(_parent != 0) {
		switch(nsTextType(_type)) {
		case NS_TEXT:
		case NS_COMMENT:
		case NS_CDATA:
		case NS_SUBSET:
			return (const xmlbyte_t*)nsText(_parent->getNsNode(), _index)->t_chars;
		case NS_PINST: {
			const xmlbyte_t *val = (const xmlbyte_t *)
				nsText(_parent->getNsNode(), _index)->t_chars;
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
	const xmlch_t *text = _text.get();
	if(text == 0 && _parent != 0) {
		bool owned = false;
		nsNode_t *node = _parent->getNsNode();
		text = getNsDocument()->getText(nsText(node, _index),
						nsIsUTF16(node), owned);
		DBXML_ASSERT(text);
		_text.set(text, owned);
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
		if(_value.get() == 0 && _parent != 0) {
			const xmlch_t *val = _getText();
			while (*val++); // get past NULL
			_value.set(val, false);
		}
		return _value.get();
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
	_parent->nsMakeTransient();

	if(nsTextType(_type) == NS_PINST) {
		_piSetNodeValue(value);
	}
	else {
		_textSetNodeValue(value);
	}

	// Reset the lazily cached name and value.
	_text.clear(getNsDocument()->getMemoryManager());
	_value.clear(getNsDocument()->getMemoryManager());

	getNsDocument()->addToModifications(NodeModification::UPDATE, _parent);
}

void
NsDomText::_textSetNodeValue(const xmlch_t *value)
{
	if(_parent == 0) {
		_text.clear(getNsDocument()->getMemoryManager());
		_text.set(NsUtil::nsStringDup(getNsDocument()->getMemoryManager(),
					      value, 0), true);
		return;
	}

	nsNode_t *node = _parent->getNsNode();
	nsText_t *text = nsText(node, _index);

	if(nsIsUTF16(node)) {
		// Work out size of new buffer
		int vlen = NsUtil::nsStringLen(value);
		int len = vlen + 1;

		// Allocate buffer
		xmlch_t *buffer = (xmlch_t*)getNsDocument()->getMemoryManager()->
			allocate(len * sizeof(xmlch_t));
		if (!buffer)
			NsUtil::nsThrowException(XmlException::NO_MEMORY_ERROR,
				 "Unable to allocate memory for _piSetNodeValue",
				  __FILE__, __LINE__);

		// Copy value into buffer
		memcpy(buffer, value, len * sizeof(xmlch_t));

		// Recalculate the total attributes size
		node->nd_text->tl_len += len;
		node->nd_text->tl_len -= text->t_len + 1;

		// Set the attribute to the new buffer
		getNsDocument()->getMemoryManager()->
			deallocate(text->t_chars);
		text->t_chars = buffer;
		text->t_len = vlen;
	}
	else {
		// Work out size of new buffer
		int vlen = NsUtil::nsStringLen(value);
		int len = vlen * 3 + 1;

		// Allocate buffer
		xmlbyte_t *buffer = (xmlbyte_t*)getNsDocument()->getMemoryManager()->
			allocate(len * sizeof(xmlbyte_t));
		if (!buffer)
			NsUtil::nsThrowException(XmlException::NO_MEMORY_ERROR,
				 "Unable to allocate memory for _piSetNodeValue",
				  __FILE__, __LINE__);

		// Transcode value into buffer
		int vlen8 = NsUtil::nsToUTF8(
			getNsDocument()->getMemoryManager(),
			&buffer, value, vlen + 1, len);

		// Recalculate the total attributes size
		node->nd_text->tl_len += vlen8 + 1;
		node->nd_text->tl_len -= text->t_len + 1;

		// Set the attribute to the new buffer
		getNsDocument()->getMemoryManager()->
			deallocate(text->t_chars);
		text->t_chars = buffer;
		text->t_len = vlen8 - 1;
	}
}

void
NsDomText::_piSetNodeValue(const xmlch_t *value)
{
	if(_parent == 0) {
		_value.clear(getNsDocument()->getMemoryManager());
		_value.set(NsUtil::nsStringDup(getNsDocument()->getMemoryManager(),
					       value, 0), true);
		return;
	}

	nsNode_t *node = _parent->getNsNode();
	nsText_t *text = nsText(node, _index);

	if(nsIsUTF16(node)) {
		// Work out size of new buffer
		int nlen = NsUtil::nsStringLen((xmlch_t*)text->t_chars);
		int vlen = NsUtil::nsStringLen(value);
		int len = nlen + vlen + 2; // 2 nulls

		// Allocate buffer
		xmlch_t *buffer = (xmlch_t*)getNsDocument()->getMemoryManager()->
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
		node->nd_text->tl_len += len;
		node->nd_text->tl_len -= text->t_len + 1;

		// Set the attribute to the new buffer
		getNsDocument()->getMemoryManager()->
			deallocate(text->t_chars);
		text->t_chars = buffer;
		text->t_len = nlen + 1 + vlen;
	}
	else {
		// Work out size of new buffer
		int nlen = NsUtil::nsStringLen((xmlbyte_t*)text->t_chars);
		int vlen = NsUtil::nsStringLen(value);
		int len = nlen + vlen * 3 + 2; // 2 nulls

		// Allocate buffer
		xmlbyte_t *buffer = (xmlbyte_t*)getNsDocument()->getMemoryManager()->
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
			getNsDocument()->getMemoryManager(),
			&value8, value, vlen + 1, vlen * 3 + 1);

		// Recalculate the total attributes size
		node->nd_text->tl_len += nlen + vlen8 + 1;
		node->nd_text->tl_len -= text->t_len + 1;

		// Set the attribute to the new buffer
		getNsDocument()->getMemoryManager()->
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

nsNode_t *
NsDomText::getNsNode() const
{
	return _parent ? _parent->getNsNode() : 0;
}

const nid_t *
NsDomText::getNodeId() const
{
	return _parent ? _parent->getNodeId() : 0;
}

bool
NsDomText::nsIgnorableWhitespace() const
{
	if(_parent != 0) {
		nsNode_t *node = _parent->getNsNode();
		if (node->nd_text->tl_text[_index].te_type
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
		_name.clear(getNsDocument()->getMemoryManager());
		_lname = 0;
		_value.clear(getNsDocument()->getMemoryManager());
		_prefix.clear(getNsDocument()->getMemoryManager());
		_uri.clear(getNsDocument()->getMemoryManager());
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
		_name.set(NsUtil::nsStringDup(doc->getMemoryManager(), qname, 0), true);

		// point _lname to the localname
		_lname = _name.get();
		while(*_lname != 0 && *_lname++ != ':');
		if(*_lname == 0) _lname = _name.get();
	}

	// copy the uri
	if(uri != 0)
		_uri.set(NsUtil::nsStringDup(doc->getMemoryManager(), uri, 0), true);

	// set the value to a zero length string
	static const xmlch_t zeroLength[] = { 0 };
	_value.set(zeroLength, false);
}

void
NsDomAttr::makeStandalone()
{
	if(_owner != 0) {
		// Get the present attributes
		xmlch_t *uri = NsUtil::nsStringDup(getNsDocument()->getMemoryManager(),
						   getNsUri(), 0);
		xmlch_t *qname = NsUtil::nsStringDup(getNsDocument()->getMemoryManager(),
						     getNsNodeName(), 0);
		xmlch_t *value = NsUtil::nsStringDup(getNsDocument()->getMemoryManager(),
						     getNsNodeValue(), 0);

		// Clear the internal state
		_name.clear(getNsDocument()->getMemoryManager());
		_lname = 0;
		_value.clear(getNsDocument()->getMemoryManager());
		_prefix.clear(getNsDocument()->getMemoryManager());
		_uri.clear(getNsDocument()->getMemoryManager());
		_owner = 0;
		_index = -1;

		// Set the internal state
		if(qname != 0) {
			_name.set(qname, true);

			// point _lname to the localname
			_lname = qname;
			while(*_lname != 0 && *_lname++ != ':');
			if(*_lname == 0) _lname = qname;
		}
		if(uri != 0)
			_uri.set(uri, true);
		if(value != 0)
			_value.set(value, true);
	}
}

void
NsDomAttr::destroy()
{
	_name.clear(getNsDocument()->getMemoryManager());
	_value.clear(getNsDocument()->getMemoryManager());
	_prefix.clear(getNsDocument()->getMemoryManager());
	_uri.clear(getNsDocument()->getMemoryManager());
}

NsDomFactory *
NsDomAttr::getNsDomFactory() const
{
	return _doc->getDomFactory();
}

bool
NsDomAttr::isNativeUTF8() const
{
	return _owner != 0 && !nsIsUTF16(_owner->getNsNode());
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
	if(_owner != 0 && !nsIsUTF16(_owner->getNsNode())) {
		nsName_t *aname = nsAttrName(_owner->getNsNode(), _index);
		return (const xmlbyte_t*)aname->n_text.t_chars;
	}
	return 0;
}

const xmlbyte_t *
NsDomAttr::getNsPrefix8() const
{
	if(_owner != 0 && !nsIsUTF16(_owner->getNsNode())) {
		nsName_t *aname = nsAttrName(_owner->getNsNode(), _index);
		return getNsDocument()->getPrefix8(aname->n_prefix);
	}
	return 0;
}

const xmlbyte_t *
NsDomAttr::getNsUri8() const
{
	if(_owner != 0 && !nsIsUTF16(_owner->getNsNode())) {
		int32_t uriIndex = nsAttrUri(_owner->getNsNode(), _index);
		if(uriIndex != NS_NOURI)
			return getNsDocument()->getUri8(uriIndex);
	}
	return 0;
}

const xmlbyte_t *
NsDomAttr::getNsNodeValue8() const
{
	if(_owner != 0 && !nsIsUTF16(_owner->getNsNode())) {
		nsName_t *aname = nsAttrName(_owner->getNsNode(), _index);
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
	nsNode_t *node = _owner->getNsNode();
	nsName_t *aname = nsAttrName(node, _index);
	const xmlch_t *qname = getNsDocument()->
		getQname(aname, nsIsUTF16(node), owned);
	DBXML_ASSERT(qname);
	_name.set(qname, owned);
	_lname = qname;
	if (aname->n_prefix != NS_NOPREFIX)
		while (*_lname++ != xmlchColon);
	const xmlch_t *val = _lname;
	while(*(val++)); // get past null to value
	_value.set(val, false);
}

const xmlch_t *
NsDomAttr::getNsNodeValue() const
{
	if (_value.get() == 0 && _owner != 0) {
		_getName();
		DBXML_ASSERT(_value.get());
	}
	return _value.get();
}

void
NsDomAttr::setNsNodeValue(const xmlch_t *value)
{
	if(_owner == 0) {
		_value.clear(getNsDocument()->getMemoryManager());
		_value.set(NsUtil::nsStringDup(getNsDocument()->getMemoryManager(),
					       value, 0), true);
		return;
	}

	_owner->nsMakeTransient();
	nsNode_t *node = _owner->getNsNode();
	nsAttr_t *attr = nsAttr(node, _index);

	if(nsIsUTF16(node)) {
		// Work out size of new buffer
		int nlen = NsUtil::nsStringLen((xmlch_t*)attr->a_name.n_text.t_chars);
		int vlen = NsUtil::nsStringLen(value);
		int len = nlen + vlen + 2; // 2 nulls

		// Allocate buffer
		xmlch_t *buffer = (xmlch_t*)getNsDocument()->getMemoryManager()->
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
		node->nd_attrs->al_len += len;
		node->nd_attrs->al_len -= attr->a_name.n_text.t_len + 1;

		// Set the attribute to the new buffer
		getNsDocument()->getMemoryManager()->
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
		xmlbyte_t *buffer = (xmlbyte_t*)getNsDocument()->getMemoryManager()->
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
			getNsDocument()->getMemoryManager(), &value8, value,
			vlen + 1, vlen * 3 + 1, &needsEscaping, isAttrVal);
		if(needsEscaping) attr->a_flags |= NS_ATTR_ENT;

		// Recalculate the total attributes size
		node->nd_attrs->al_len += nlen + vlen8 + 1;
		node->nd_attrs->al_len -= attr->a_name.n_text.t_len + 1;

		// Set the attribute to the new buffer
		getNsDocument()->getMemoryManager()->
			deallocate(attr->a_name.n_text.t_chars);
		attr->a_name.n_text.t_chars = buffer;
		attr->a_name.n_text.t_len = nlen + vlen8;
		attr->a_value = value8;
	}

	// Reset the lazily cached name and value.
	_name.clear(getNsDocument()->getMemoryManager());
	_lname = 0;
	_value.clear(getNsDocument()->getMemoryManager());

	getNsDocument()->addToModifications(NodeModification::UPDATE, _owner);
}

const xmlch_t *
NsDomAttr::getNsNodeName() const
{
	if (_name.get() == 0 && _owner != 0) {
		_getName();
		DBXML_ASSERT(_name.get() && _lname);
	}
	return _name.get();
}

void
NsDomAttr::nsRename(const xmlch_t *uri, const xmlch_t *qname)
{
	NsDomElement *owner = _owner;
	if(owner != 0) {
		owner->getNsAttributes()->removeNsItem(_index);
	}

	// copy qname
	_name.clear(getNsDocument()->getMemoryManager());
	_name.set(NsUtil::nsStringDup(getNsDocument()->
				      getMemoryManager(),
				      qname, 0), true);

	// point _lname to the localname
	_lname = _name.get();
	while(*_lname != 0 && *_lname++ != ':');
	if(*_lname == 0) _lname = _name.get();

	// clear the lazily evaluated prefix
	_prefix.clear(getNsDocument()->getMemoryManager());

	// copy the uri
	_uri.clear(getNsDocument()->getMemoryManager());
	_uri.set(NsUtil::nsStringDup(getNsDocument()->
				     getMemoryManager(),
				     uri, 0), true);

	// If this attribute has an owner, then set the name in the nsNode_t
	if(owner != 0) {
		owner->getNsAttributes()->setNsNamedItemNS(this);
	}
}

const xmlch_t *
NsDomAttr::getNsLocalName() const
{
	if (_lname == 0 && _owner != 0) {
		_getName();
		DBXML_ASSERT(_name.get() && _lname);
	}
	return _lname;
}

const xmlch_t *
NsDomAttr::getNsPrefix() const
{
	if(_prefix.get() == 0) {
		if(_owner == 0) {
			unsigned int plen = _lname - _name.get();
			if(plen != 0) {
				_prefix.set((xmlch_t*)getNsDocument()->getMemoryManager()->
					    allocate(plen * sizeof(xmlch_t)), true);
				if(_prefix.get() == 0)
					NsUtil::nsThrowException(XmlException::NO_MEMORY_ERROR,
							 "getNsPrefix failed to allocate memory",
							 __FILE__, __LINE__);
				// don't copy colon
				plen -= 1;
				memcpy((void*)_prefix.get(), _name.get(), plen * sizeof(xmlch_t));
				// null terminate
				(xmlch_t&)_prefix.get()[plen] = 0;
			}
		}
		else {
			nsName_t *aname = nsAttrName(_owner->getNsNode(), _index);
			_prefix.set(getNsDocument()->getPrefix(aname->n_prefix), false);
		}
	}
	return _prefix.get();
}

void NsDomAttr::setNsPrefix(const xmlch_t *prefix)
{
	if(_owner == 0) {
		if(_uri.get() == 0 || *_uri.get() == 0)
			return;

		int nlen = NsUtil::nsStringLen(_lname);
		int len = nlen + 1;
		int plen;
		if(prefix != 0) {
			plen = NsUtil::nsStringLen(prefix);
			len += plen + 1;
		}

		xmlch_t *buf = (xmlch_t*)getNsDocument()->
			getMemoryManager()->allocate(len * sizeof(xmlch_t));

		xmlch_t *ptr = buf;
		if(prefix != 0) {
			memcpy(ptr, prefix, plen * sizeof(xmlch_t));
			ptr += plen;
			*ptr++ = ':';
		}
		memcpy(ptr, _lname, (nlen + 1) * sizeof(xmlch_t));

		_name.clear(getNsDocument()->getMemoryManager());
		_name.set(buf, true);
		_lname = ptr;
		_prefix.clear(getNsDocument()->getMemoryManager());
		return;
	}

	if (!nsAttrHasUri(_owner->getNsNode(), _index))
		return;

	int32_t attr_prefix = nsAttrNamePrefix(_owner->getNsNode(), _index);
	int prefixId = getNsDocument()->
		addPrefix(prefix, getNsDocument()->
			  getUriIndexForPrefix(attr_prefix),
			  /*isUTF8*/false,
			  /*isDonated*/false, 0);
	if(attr_prefix != prefixId) {
		nsSetAttrNamePrefix(_owner->getNsNode(), _index, prefixId);

		_name.clear(getNsDocument()->getMemoryManager());
		_lname = 0;
		_prefix.clear(getNsDocument()->getMemoryManager());
		_value.clear(getNsDocument()->getMemoryManager());

		getNsDocument()->addToModifications(NodeModification::UPDATE, _owner);
	}
}

const xmlch_t *
NsDomAttr::getNsUri() const
{
	if(_uri.get() == 0 && _owner != 0) {
		int32_t uriIndex = nsAttrUri(_owner->getNsNode(), _index);
		if (uriIndex != NS_NOURI)
			_uri.set(getNsDocument()->getUri(uriIndex), false);
	}
	return _uri.get();
}

nsNode_t *
NsDomAttr::getNsNode() const
{
	return _owner ? _owner->getNsNode() : 0;
}

const nid_t *
NsDomAttr::getNodeId() const
{
	return _owner ? _owner->getNodeId() : 0;
}

bool
NsDomAttr::getNsSpecified() const
{
	if (!_owner)
		return true;

	nsNode_t *node = _owner->getNsNode();
	uint32_t flags = nsAttrFlags(node, _index);
	if (flags & NS_ATTR_NOT_SPECIFIED)
		return false;
	return true;
}

//
// NsDomNamedNodeMap implementation
//

NsDomNamedNodeMap::NsDomNamedNodeMap(NsDomElement *owner) :
	_attrs(nsNumAttrs(owner->getNsNode())), _owner(owner)
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
	NsUtil::nsSetAttr(getNsDocument(), _owner->getNsNode(), index,
		attr->getNsPrefix(), attr->getNsUri(),
		attr->getNsLocalName(), attr->getNsNodeValue(), -1, true);
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
	NsUtil::nsAddAttr(getNsDocument(), _owner->getNsNode(),
		attr->getNsPrefix(), attr->getNsUri(),
		attr->getNsLocalName(), attr->getNsNodeValue(), -1, true);
	_attrs.push_back(attr);
	attr->makeNonStandalone(_owner, _attrs.size() - 1);

	getNsDocument()->addToModifications(NodeModification::UPDATE, _owner);
}

NsDomAttr *
NsDomNamedNodeMap::removeNsItem(int index)
{
	_owner->nsMakeTransient();

	NsDomAttr *retAttr = getNsItem(index);
	nsNode_t *node = _owner->getNsNode();

	// Remove old attribute
	retAttr->makeStandalone();
	NsUtil::nsRemoveAttr(getNsDocument()->getMemoryManager(),
			     node, index);

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
	: _owner(owner)
{
}

void
NsDomNodeList::destroy()
{
}

int
NsDomNodeList::getNsLength() const
{
	if(_owner)
		return _owner->numNsChildNodes();

	return 0;
}

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

	if (!NsUtil::nsStringEqual(lookupBaseUri(node1),
				   lookupBaseUri(node2)))
		return false;

	return true;
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

	if (thisNode == otherNode) {
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
	if (thisNode == otherNode) {
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
		if (lower == stable) {
			// if upper/stable node is not an attribute,
			// relationship is ANCESTOR or DESCENDANT; otherwise,
			// it's PRECEDING or FOLLOWING.
			if (stable == thisNode) {
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
		if (stable == thisNode) {
			otherNode = lower;
			otherType = lower->getNsNodeType();
		} else {
			thisNode = lower;
			thisType = lower->getNsNodeType();
		}
	}
	DBXML_ASSERT(thisNode != otherNode);
	// if nodes are text siblings, order is based on
	// text index.  If only one is nsNodeText, it's necessary
	// to walk the sibling tree to find a non-text sibling for
	// comparison.  Cannot use leading/trailing text because it
	// may not be present.
	if (const_cast<NsDomNode*>(thisNode)->getNsParentNode() ==
	    const_cast<NsDomNode*>(otherNode)->getNsParentNode()) {
		if (thisType == otherType && thisType == nsNodeText) {
			if (((const NsDomText *)thisNode)->getIndex() <
			    ((const NsDomText *)otherNode)->getIndex()) {
				return TREE_FOLLOWING;
			}
			return TREE_PRECEDING;
		}
		if (thisType == nsNodeText || otherType == nsNodeText) {
			if (thisType == nsNodeText) {
				// walk backwards to non-text sibling or otherNode
				while (thisNode && (thisNode != otherNode) &&
				       (thisNode->getNsNodeType() == nsNodeText))
					thisNode = ((NsDomNode *)thisNode)->
						getNsPrevSibling();
				if (!thisNode) {
					return TREE_FOLLOWING;
				}
				if (thisNode == otherNode) {
					return TREE_PRECEDING;
				}
			} else {
				// walk backwards to non-text sibling or thisNode
				while (otherNode && (thisNode != otherNode) &&
				       (otherNode->getNsNodeType() == nsNodeText))
					otherNode = ((NsDomNode *)otherNode)->
						getNsPrevSibling();
				if (!otherNode) {
					return TREE_PRECEDING;
				}
				if (thisNode == otherNode) {
					return TREE_FOLLOWING;
				}
			}
		}
	}

	// just compare node ids
	const nid_t *thisNid = thisNode->getNodeId();
	const nid_t *otherNid = otherNode->getNodeId();
	DBXML_ASSERT(thisNid && otherNid);
	Dbt thisDbt((void*)NsUtil::nsNode(thisNid), thisNid->idLen);
	Dbt otherDbt((void*)NsUtil::nsNode(otherNid), otherNid->idLen);
	int ret =  nsCompareNodes(0, thisDbt.get_const_DBT(),
				  otherDbt.get_const_DBT());
	// TBD: probably should not be the same -- document cache
	// isn't yet implemented.  Need this for W3C test cases
	// DBXML_ASSERT(ret != 0)  // better not be the same.
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
	if (elem->hasNsAttributes() && nsHasNsinfo(elem->getNsNode())) {
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
	if (elem && !nsIsDoc(elem->getNsNode()))
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
	if (elem->hasNsAttributes() && nsHasNsinfo(elem->getNsNode())) {
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
	if (elem && !nsIsDoc(elem->getNsNode()))
		return lookupUri(elem, prefix);
	return 0;
}

// see base uri in DOM spec:
//http://www.w3.org/TR/2004/REC-DOM-Level-3-Core-20040407/core.html#Node3-lookupNamespaceURI
//
/* static */ const xmlch_t *
NsDomNode::lookupBaseUri(const NsDomNode *node)
{
	// TBD
	return 0;
}
