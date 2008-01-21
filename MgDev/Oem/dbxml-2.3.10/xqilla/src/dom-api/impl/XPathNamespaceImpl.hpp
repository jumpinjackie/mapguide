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
 * $Id: XPathNamespaceImpl.hpp,v 1.7 2006/11/01 16:37:18 jpcs Exp $
 */

#ifndef __XPATHNAMESPACEIMPL_HPP
#define __XPATHNAMESPACEIMPL_HPP

#include <xqilla/framework/XQillaExport.hpp>

#include <xercesc/util/XercesDefs.hpp>
#include <xercesc/dom/impl/DOMNodeImpl.hpp>
#include <xercesc/dom/DOMXPathNamespace.hpp>

XERCES_CPP_NAMESPACE_BEGIN
class DOMElement;
class DOMDocument;
XERCES_CPP_NAMESPACE_END


class XQILLA_API XPathNamespaceImpl : public XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathNamespace
{
public:
	XERCES_CPP_NAMESPACE_QUALIFIER DOMNodeImpl fNode;
    const XMLCh *uri;
    const XMLCh *prefix;

    enum XPathObjectType {
      XPATH_NAMESPACE_OBJECT = 13
    };

public:
	//Main constructor
	XPathNamespaceImpl(const XMLCh* const prefix, const XMLCh* const uri, XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *owner, XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *docOwner);
	XPathNamespaceImpl(const XPathNamespaceImpl &other);
	virtual ~XPathNamespaceImpl();

	static XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathNamespace *createXPathNamespace(const XMLCh* const prefix, const XMLCh* const uri, XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *owner);

	// Declare functions from DOMNode.  They all must be implemented by this class
    virtual       XERCES_CPP_NAMESPACE_QUALIFIER DOMNode*         appendChild(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *newChild) ;
    virtual       XERCES_CPP_NAMESPACE_QUALIFIER DOMNode*         cloneNode(bool deep) const ;
    virtual       XERCES_CPP_NAMESPACE_QUALIFIER DOMNamedNodeMap* getAttributes() const ;
    virtual       XERCES_CPP_NAMESPACE_QUALIFIER DOMNodeList*     getChildNodes() const ;
    virtual       XERCES_CPP_NAMESPACE_QUALIFIER DOMNode*         getFirstChild() const ;
    virtual       XERCES_CPP_NAMESPACE_QUALIFIER DOMNode*         getLastChild() const ;
    virtual const XMLCh*           getLocalName() const ;
    virtual const XMLCh*           getNamespaceURI() const ;
    virtual       XERCES_CPP_NAMESPACE_QUALIFIER DOMNode*         getNextSibling() const ;
    virtual const XMLCh*           getNodeName() const ;
    virtual       short            getNodeType() const ;
    virtual const XMLCh*           getNodeValue() const ;
    virtual       XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument*     getOwnerDocument() const ;
    virtual const XMLCh*           getPrefix() const ;
    virtual       XERCES_CPP_NAMESPACE_QUALIFIER DOMNode*         getParentNode() const ;
    virtual       XERCES_CPP_NAMESPACE_QUALIFIER DOMNode*         getPreviousSibling() const ;
    virtual       bool             hasChildNodes() const ;
    virtual       XERCES_CPP_NAMESPACE_QUALIFIER DOMNode*         insertBefore(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *newChild, XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *refChild) ;
    virtual       void             normalize() ;
    virtual       XERCES_CPP_NAMESPACE_QUALIFIER DOMNode*         removeChild(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *oldChild) ;
    virtual       XERCES_CPP_NAMESPACE_QUALIFIER DOMNode*         replaceChild(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *newChild, XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *oldChild) ;
    virtual       void             setNodeValue(const XMLCh  *nodeValue) ;
    virtual       bool             isSupported(const XMLCh *feature, const XMLCh *version) const ;
    virtual       bool             hasAttributes() const ;
    virtual       void             setPrefix(const XMLCh * prefix) ;
    virtual       void*            setUserData(const XMLCh* key, void* data, XERCES_CPP_NAMESPACE_QUALIFIER DOMUserDataHandler* handler) ;
    virtual       void*            getUserData(const XMLCh* key) const ;
    virtual       bool             isSameNode(const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode* other) const;
    virtual       bool             isEqualNode(const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode* arg) const;
    virtual const XMLCh*           getBaseURI() const ;
    virtual short                  compareTreePosition(const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode* other) const ;
    virtual const XMLCh*           getTextContent() const ;
    virtual void                   setTextContent(const XMLCh* textContent) ;
    virtual const XMLCh*           lookupNamespacePrefix(const XMLCh* namespaceURI, bool useDefault) const  ;
    virtual bool                   isDefaultNamespace(const XMLCh* namespaceURI) const;
    virtual const XMLCh*           lookupNamespaceURI(const XMLCh* prefix) const  ;
    virtual       XERCES_CPP_NAMESPACE_QUALIFIER DOMNode*         getInterface(const XMLCh* feature) ;
    virtual       void             release();


	virtual XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *getOwnerElement() const;
};



#endif
