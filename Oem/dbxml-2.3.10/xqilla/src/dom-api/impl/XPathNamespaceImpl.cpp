/*
 * Copyright (c) 2001-2006
 *     DecisionSoft Limited. All rights reserved.
 * Copyright (c) 2004-2006
 *     Progress Software Corporation. All rights reserved.
 * Copyright (c) 2004-2006
 *     Oracle. All rights reserved.
 *
 * See the file LICENSE for redistribution information.
 *
 * $Id: XPathNamespaceImpl.cpp,v 1.8 2006/11/01 16:37:18 jpcs Exp $
 */

#include "../../config/xqilla_config.h"
#include "XPathNamespaceImpl.hpp"
#include <xqilla/utils/XPath2Utils.hpp>
#include <xqilla/utils/XStr.hpp>
#include <xercesc/dom/DOMException.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/impl/DOMDocumentImpl.hpp>
#include <xercesc/dom/impl/DOMAttrImpl.hpp>
#include <xercesc/util/XercesVersion.hpp>
#include <xercesc/dom/impl/DOMCasts.hpp>
#include <xercesc/dom/impl/DOMNodeImpl.hpp>

//Main constructor
XPathNamespaceImpl::XPathNamespaceImpl(const XMLCh* const nsPrefix, 
		const XMLCh* const nsUri, XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *owner, XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *docOwner) 
	: fNode(docOwner)
{
    XERCES_CPP_NAMESPACE_QUALIFIER DOMNodeImpl *argImpl = XERCES_CPP_NAMESPACE_QUALIFIER castToNodeImpl(this);
    argImpl->fOwnerNode = owner;
    argImpl->isOwned(true);
    argImpl->setIsLeafNode(true);

    XERCES_CPP_NAMESPACE_QUALIFIER DOMDocumentImpl *docImpl = (XERCES_CPP_NAMESPACE_QUALIFIER DOMDocumentImpl *)docOwner;
    uri = docImpl->getPooledString(nsUri);
    prefix = docImpl->getPooledString(nsPrefix);

    //	this->ownerElement = owner;
}//constructor


//Copy constructor
XPathNamespaceImpl::XPathNamespaceImpl(const XPathNamespaceImpl &other) 
	: fNode(other.fNode), uri(other.uri), prefix(other.prefix)
{
}//copy constructor


//Destructor
XPathNamespaceImpl::~XPathNamespaceImpl()
{
	//Nothing
}//destructor


XERCES_CPP_NAMESPACE_QUALIFIER DOMNode * XPathNamespaceImpl::cloneNode(bool deep) const 
{
	throw XERCES_CPP_NAMESPACE_QUALIFIER DOMException::NOT_SUPPORTED_ERR;
}//cloneNode

const XMLCh *XPathNamespaceImpl::getNodeName() const
{
	return prefix;
}//getNodeName

short XPathNamespaceImpl::getNodeType() const
{
  //	return XERCES_CPP_NAMESPACE_QUALIFIER DOMNode::XPATH_NAMESPACE_NODE;
  return XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathNamespace::XPATH_NAMESPACE_NODE;
}//getNodeType

const XMLCh *XPathNamespaceImpl::getPrefix() const
{
	return prefix;
}//getPrefix


const XMLCh *XPathNamespaceImpl::getNamespaceURI() const 
{
	return uri;
}//getNamespaceURI

XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *XPathNamespaceImpl::getOwnerElement() const 
{
    return (XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *) (fNode.isOwned() ? fNode.fOwnerNode : 0);
}//getOwnerElement

const XMLCh *XPathNamespaceImpl::getNodeValue() const 
{
	return 0;
}//getNodeVale

void XPathNamespaceImpl::setNodeValue(const XMLCh *value)
{
	throw XERCES_CPP_NAMESPACE_QUALIFIER DOMException(XERCES_CPP_NAMESPACE_QUALIFIER DOMException::NO_MODIFICATION_ALLOWED_ERR, 0);
}//setNodeValue


void* XPathNamespaceImpl::setUserData(const XMLCh* key, void* data, XERCES_CPP_NAMESPACE_QUALIFIER DOMUserDataHandler* handler)
{
	throw XERCES_CPP_NAMESPACE_QUALIFIER DOMException(XERCES_CPP_NAMESPACE_QUALIFIER DOMException::NO_MODIFICATION_ALLOWED_ERR, 0);
}

void XPathNamespaceImpl::setPrefix(const XMLCh *prefix)
{
	throw XERCES_CPP_NAMESPACE_QUALIFIER DOMException(XERCES_CPP_NAMESPACE_QUALIFIER DOMException::NO_MODIFICATION_ALLOWED_ERR, 0);
}//setPrefix



short            XPathNamespaceImpl::compareTreePosition(const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode* other) const {

    //note: order of namespace nodes currently has issues (number 51). For our purposes
    //namespace nodes belong to the element on which they were originally declared
    //this is proberbly incorecct but easiest to implement. Will change when the idea
    //is firmed up REVISIT

    // Questions of clarification for this method - to be answered by the
    // DOM WG.   Current assumptions listed - LM
    //
    // 1. How do ENTITY nodes compare?
    //    Current assumption: TREE_POSITION_DISCONNECTED, as ENTITY nodes
    //    aren't really 'in the tree'
    //
    // 2. How do NOTATION nodes compare?
    //    Current assumption: TREE_POSITION_DISCONNECTED, as NOTATION nodes
    //    aren't really 'in the tree'
    //
    // 3. Are TREE_POSITION_ANCESTOR and TREE_POSITION_DESCENDANT
    //    only relevant for nodes that are "part of the document tree"?
    //     <outer>
    //         <inner  myattr="true"/>
    //     </outer>
    //    Is the element node "outer" considered an ancestor of "myattr"?
    //    Current assumption: No.
    //
    // 4. How do children of ATTRIBUTE nodes compare (with eachother, or
    //    with children of other attribute nodes with the same element)
    //    Current assumption: Children of ATTRIBUTE nodes are treated as if
    //    they are the attribute node itself, unless the 2 nodes
    //    are both children of the same attribute.
    //
    // 5. How does an ENTITY_REFERENCE node compare with it's children?
    //    Given the DOM, it should precede its children as an ancestor.
    //    Given "document order",  does it represent the same position?
    //    Current assumption: An ENTITY_REFERENCE node is an ancestor of its
    //    children.
    //
    // 6. How do children of a DocumentFragment compare?
    //    Current assumption: If both nodes are part of the same document
    //    fragment, there are compared as if they were part of a document.



    const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode* thisNode = this;

    // If the nodes are the same...
    if (thisNode == other)
        return (XERCES_CPP_NAMESPACE_QUALIFIER DOMNode::TREE_POSITION_SAME_NODE | XERCES_CPP_NAMESPACE_QUALIFIER DOMNode::TREE_POSITION_EQUIVALENT);

    // If either node is of type ENTITY or NOTATION, compare as disconnected
    short thisType = thisNode->getNodeType();
    short otherType = other->getNodeType();

    // If either node is of type ENTITY or NOTATION, compare as disconnected
    if (thisType == XERCES_CPP_NAMESPACE_QUALIFIER DOMNode::ENTITY_NODE ||
            thisType == XERCES_CPP_NAMESPACE_QUALIFIER DOMNode::NOTATION_NODE ||
            otherType == XERCES_CPP_NAMESPACE_QUALIFIER DOMNode::ENTITY_NODE ||
            otherType == XERCES_CPP_NAMESPACE_QUALIFIER DOMNode::NOTATION_NODE ) {
        return XERCES_CPP_NAMESPACE_QUALIFIER DOMNode::TREE_POSITION_DISCONNECTED;
    }

    //if it is a custom node and bigger than us we must ask it for the order
    if(otherType > XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathNamespace::XPATH_NAMESPACE_NODE) {
        XERCES_CPP_NAMESPACE_QUALIFIER DOMNodeImpl tmp(0);
        return tmp.reverseTreeOrderBitPattern(other->compareTreePosition(this));
    }

    // Find the ancestor of each node, and the distance each node is from
    // its ancestor.
    // During this traversal, look for ancestor/descendent relationships
    // between the 2 nodes in question.
    // We do this now, so that we get this info correct for attribute nodes
    // and their children.

    const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node;
    const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *thisAncestor = this;
    const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *otherAncestor = other;
    int thisDepth=0;
    int otherDepth=0;
    for (node = this; node != 0; node = node->getParentNode()) {
        thisDepth +=1;
        if (node == other)
            // The other node is an ancestor of this one.
            return (XERCES_CPP_NAMESPACE_QUALIFIER DOMNode::TREE_POSITION_ANCESTOR | XERCES_CPP_NAMESPACE_QUALIFIER DOMNode::TREE_POSITION_PRECEDING);
        thisAncestor = node;
    }

    for (node=other; node != 0; node = node->getParentNode()) {
        otherDepth +=1;
        if (node == this)
            // The other node is a descendent of the reference node.
            return (XERCES_CPP_NAMESPACE_QUALIFIER DOMNode::TREE_POSITION_DESCENDANT | XERCES_CPP_NAMESPACE_QUALIFIER DOMNode::TREE_POSITION_FOLLOWING);
        otherAncestor = node;
    }


    const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *otherNode = other;

    short thisAncestorType = thisAncestor->getNodeType();
    short otherAncestorType = otherAncestor->getNodeType();

    // if the ancestor is an attribute, get owning element.
    // we are now interested in the owner to determine position.

    if (thisAncestorType == XERCES_CPP_NAMESPACE_QUALIFIER DOMNode::ATTRIBUTE_NODE) {
        thisNode = ((XERCES_CPP_NAMESPACE_QUALIFIER DOMAttrImpl *)thisAncestor)->getOwnerElement();
    }

    if (thisAncestorType == XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathNamespace::XPATH_NAMESPACE_NODE) {
        thisNode = ((XPathNamespaceImpl *)thisAncestor)->getOwnerElement();
    }

    if (otherAncestorType == XERCES_CPP_NAMESPACE_QUALIFIER DOMNode::ATTRIBUTE_NODE) {
        otherNode = ((XERCES_CPP_NAMESPACE_QUALIFIER DOMAttrImpl *)otherAncestor)->getOwnerElement();
    }

    if (otherAncestorType == XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathNamespace::XPATH_NAMESPACE_NODE) {
        otherNode = ((XPathNamespaceImpl *)otherAncestor)->getOwnerElement();
    }


    // Before proceeding, we should check if both ancestor nodes turned
    // out to be attributes for the same element
    if (thisAncestorType == XERCES_CPP_NAMESPACE_QUALIFIER DOMNode::ATTRIBUTE_NODE &&
        otherAncestorType == XERCES_CPP_NAMESPACE_QUALIFIER DOMNode::ATTRIBUTE_NODE &&
            thisNode==otherNode)
        return XERCES_CPP_NAMESPACE_QUALIFIER DOMNode::TREE_POSITION_EQUIVALENT;

    //now do the same for namespace nodes
    if (thisAncestorType == XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathNamespace::XPATH_NAMESPACE_NODE &&
        otherAncestorType == XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathNamespace::XPATH_NAMESPACE_NODE &&
            thisNode==otherNode)
        return XERCES_CPP_NAMESPACE_QUALIFIER DOMNode::TREE_POSITION_EQUIVALENT;

    //now do comparison between attrs
    if (thisAncestorType == XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathNamespace::XPATH_NAMESPACE_NODE &&
        otherAncestorType == XERCES_CPP_NAMESPACE_QUALIFIER DOMNode::ATTRIBUTE_NODE &&
            thisNode==otherNode)
        return XERCES_CPP_NAMESPACE_QUALIFIER DOMNode::TREE_POSITION_FOLLOWING;

    //now do comparison between attrs
    if (thisAncestorType == XERCES_CPP_NAMESPACE_QUALIFIER DOMNode::ATTRIBUTE_NODE &&
        otherAncestorType == XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathNamespace::XPATH_NAMESPACE_NODE &&
            thisNode==otherNode)
        return XERCES_CPP_NAMESPACE_QUALIFIER DOMNode::TREE_POSITION_PRECEDING;


    // Now, find the ancestor of the owning element, if the original
    // ancestor was an attribute

    if (thisAncestorType == XERCES_CPP_NAMESPACE_QUALIFIER DOMNode::ATTRIBUTE_NODE || thisAncestorType == XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathNamespace::XPATH_NAMESPACE_NODE) {
        thisDepth=0;
        for (node=thisNode; node != 0; node = node->getParentNode()) {
            thisDepth +=1;
            if (node == otherNode)
                // The other node is an ancestor of the owning element
                return XERCES_CPP_NAMESPACE_QUALIFIER DOMNode::TREE_POSITION_PRECEDING;
            thisAncestor = node;
        }
        for (node=otherNode; node != 0; node = node->getParentNode()) {
            if (node == thisNode)
                // The other node is an ancestor of the owning element
                return XERCES_CPP_NAMESPACE_QUALIFIER DOMNode::TREE_POSITION_FOLLOWING;
        }
    }

    // Now, find the ancestor of the owning element, if the original
    // ancestor was an attribute
    if (otherAncestorType == XERCES_CPP_NAMESPACE_QUALIFIER DOMNode::ATTRIBUTE_NODE || otherAncestorType == XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathNamespace::XPATH_NAMESPACE_NODE) {
        otherDepth=0;
        for (node=otherNode; node != 0; node = node->getParentNode()) {
            otherDepth +=1;
            if (node == thisNode)
                // The other node is a descendent of the reference
                // node's element
                return XERCES_CPP_NAMESPACE_QUALIFIER DOMNode::TREE_POSITION_FOLLOWING;
            otherAncestor = node;
        }
        for (node=thisNode; node != 0; node = node->getParentNode()) {
            if (node == otherNode)
                // The other node is an ancestor of the owning element
                return XERCES_CPP_NAMESPACE_QUALIFIER DOMNode::TREE_POSITION_PRECEDING;
        }
    }

    // thisAncestor and otherAncestor must be the same at this point,
    // otherwise, we are not in the same tree or document fragment
    if (thisAncestor != otherAncestor)
        return XERCES_CPP_NAMESPACE_QUALIFIER DOMNode::TREE_POSITION_DISCONNECTED;

    // Determine which node is of the greatest depth.
    if (thisDepth > otherDepth) {
        for (int i= 0 ; i < thisDepth - otherDepth; i++)
            thisNode = thisNode->getParentNode();
    }
    else {
        for (int i = 0; i < otherDepth - thisDepth; i++)
            otherNode = otherNode->getParentNode();
    }

    // We now have nodes at the same depth in the tree.  Find a common
    // ancestor.
    XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *thisNodeP, *otherNodeP;
    for (thisNodeP = thisNode->getParentNode(),
                 otherNodeP = otherNode->getParentNode();
             thisNodeP != otherNodeP;) {
        thisNode = thisNodeP;
        otherNode = otherNodeP;
        thisNodeP = thisNodeP->getParentNode();
        otherNodeP = otherNodeP->getParentNode();
    }

    // See whether thisNode or otherNode is the leftmost
    for (XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *current = thisNodeP->getFirstChild();
             current != 0;
             current = current->getNextSibling()) {
        if (current == otherNode) {
            return XERCES_CPP_NAMESPACE_QUALIFIER DOMNode::TREE_POSITION_PRECEDING;
        }
        else if (current == thisNode) {
            return XERCES_CPP_NAMESPACE_QUALIFIER DOMNode::TREE_POSITION_FOLLOWING;
        }
    }
    // REVISIT:  shouldn't get here.   Should probably throw an
    // exception
    return 0;
}

//Should be moved into XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument
//Produce a new XPathNamespace object or give one back from the internal map
XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathNamespace *XPathNamespaceImpl::createXPathNamespace(const XMLCh* const prefix, const XMLCh* const uri, XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *owner)
{
    XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *ownerDocument = owner->getOwnerDocument();
    return new ((XERCES_CPP_NAMESPACE_QUALIFIER DOMDocumentImpl *)ownerDocument, (XERCES_CPP_NAMESPACE_QUALIFIER DOMDocumentImpl::NodeObjectType)XPathNamespaceImpl::XPATH_NAMESPACE_OBJECT) XPathNamespaceImpl(prefix, uri, owner, ownerDocument);
}


void  XPathNamespaceImpl::release() {
    if (fNode.isOwned() && !fNode.isToBeReleased())
        throw XERCES_CPP_NAMESPACE_QUALIFIER DOMException(XERCES_CPP_NAMESPACE_QUALIFIER DOMException::INVALID_ACCESS_ERR,0);

    XERCES_CPP_NAMESPACE_QUALIFIER DOMDocumentImpl* doc = (XERCES_CPP_NAMESPACE_QUALIFIER DOMDocumentImpl*) getOwnerDocument();
    if (doc) {
        fNode.callUserDataHandlers(XERCES_CPP_NAMESPACE_QUALIFIER DOMUserDataHandler::NODE_DELETED, 0, 0);
        doc->release(this, (XERCES_CPP_NAMESPACE_QUALIFIER DOMDocumentImpl::NodeObjectType)XPathNamespaceImpl::XPATH_NAMESPACE_OBJECT);
    }
    else {
        // shouldn't reach here
        throw XERCES_CPP_NAMESPACE_QUALIFIER DOMException(XERCES_CPP_NAMESPACE_QUALIFIER DOMException::INVALID_ACCESS_ERR,0);
    }

}

const XMLCh* XPathNamespaceImpl::getTextContent() const {
  return getNamespaceURI(); 
}

const XMLCh* XPathNamespaceImpl::getLocalName() const {
  return this->getPrefix();
}

//
//   Functions inherited from Node
//

           XERCES_CPP_NAMESPACE_QUALIFIER DOMNamedNodeMap* XPathNamespaceImpl::getAttributes() const                   {return fNode.getAttributes (); };
           XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument*     XPathNamespaceImpl::getOwnerDocument() const                {return getOwnerElement()->getOwnerDocument(); };
           bool             XPathNamespaceImpl::hasAttributes() const                   {return fNode.hasAttributes(); };
           bool             XPathNamespaceImpl::isEqualNode(const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode* arg) const   {return fNode.isEqualNode(arg); };

           XERCES_CPP_NAMESPACE_QUALIFIER DOMNode*         XPathNamespaceImpl::appendChild(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *newChild)          {return fNode.appendChild (newChild); };
           XERCES_CPP_NAMESPACE_QUALIFIER DOMNodeList*     XPathNamespaceImpl::getChildNodes() const                   {return fNode.getChildNodes (); };
           XERCES_CPP_NAMESPACE_QUALIFIER DOMNode*         XPathNamespaceImpl::getFirstChild() const                   {return fNode.getFirstChild (); };
           XERCES_CPP_NAMESPACE_QUALIFIER DOMNode*         XPathNamespaceImpl::getLastChild() const                    {return fNode.getLastChild (); };
           XERCES_CPP_NAMESPACE_QUALIFIER DOMNode*         XPathNamespaceImpl::getNextSibling() const                  {return fNode.getNextSibling (); };

//The exception is wrong - we need to sort out all these methods when we can compile again.
           XERCES_CPP_NAMESPACE_QUALIFIER DOMNode*         XPathNamespaceImpl::getParentNode() const                   {return fNode.getParentNode ();};
           XERCES_CPP_NAMESPACE_QUALIFIER DOMNode*         XPathNamespaceImpl::getPreviousSibling() const              {return fNode.getParentNode ();};
           bool             XPathNamespaceImpl::hasChildNodes() const                   {return fNode.hasChildNodes ();};
           XERCES_CPP_NAMESPACE_QUALIFIER DOMNode*         XPathNamespaceImpl::insertBefore(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *newChild, XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *refChild)
                                                                                    {return fNode.insertBefore (newChild, refChild); };
           void             XPathNamespaceImpl::normalize()                             {fNode.normalize (); };
           XERCES_CPP_NAMESPACE_QUALIFIER DOMNode*         XPathNamespaceImpl::removeChild(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *oldChild)          {return fNode.removeChild (oldChild); };
           XERCES_CPP_NAMESPACE_QUALIFIER DOMNode*         XPathNamespaceImpl::replaceChild(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *newChild, XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *oldChild)
                                                                                    {return fNode.replaceChild (newChild, oldChild); };
           bool             XPathNamespaceImpl::isSupported(const XMLCh *feature, const XMLCh *version) const
                                                                                    {return fNode.isSupported (feature, version); };
           bool             XPathNamespaceImpl::isSameNode(const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode* other) const       {return fNode.isSameNode(other); };
           void*            XPathNamespaceImpl::getUserData(const XMLCh* key) const     {return fNode.getUserData(key); };
           void             XPathNamespaceImpl::setTextContent(const XMLCh* textContent){fNode.setTextContent(textContent); };
#if _XERCES_VERSION >= 20100
           const XMLCh*     XPathNamespaceImpl::lookupNamespacePrefix(const XMLCh* namespaceURI, bool useDefault) const {return fNode.lookupNamespacePrefix(namespaceURI, useDefault); };
           bool             XPathNamespaceImpl::isDefaultNamespace(const XMLCh* namespaceURI) const {return fNode.isDefaultNamespace(namespaceURI); };
           const XMLCh*     XPathNamespaceImpl::lookupNamespaceURI(const XMLCh* prefix) const {return fNode.lookupNamespaceURI(prefix); };
#else
           const XMLCh*     XPathNamespaceImpl::lookupNamespacePrefix(const XMLCh* namespaceURI, bool useDefault) {return fNode.lookupNamespacePrefix(namespaceURI, useDefault); };
           bool             XPathNamespaceImpl::isDefaultNamespace(const XMLCh* namespaceURI) {return fNode.isDefaultNamespace(namespaceURI); };
           const XMLCh*     XPathNamespaceImpl::lookupNamespaceURI(const XMLCh* prefix) {return fNode.lookupNamespaceURI(prefix); };
#endif
 
           const XMLCh*     XPathNamespaceImpl::getBaseURI() const                      {return fNode.getBaseURI(); };

           XERCES_CPP_NAMESPACE_QUALIFIER DOMNode*         XPathNamespaceImpl::getInterface(const XMLCh* feature)      {return fNode.getInterface(feature); };
