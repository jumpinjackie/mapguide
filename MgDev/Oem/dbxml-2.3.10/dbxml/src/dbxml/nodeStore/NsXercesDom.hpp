//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: NsXercesDom.hpp,v 1.42 2006/10/30 17:45:57 bostic Exp $
//
#ifndef __NSXERCESDOM_HPP
#define __NSXERCESDOM_HPP
/*
 * Xerces C++ DOM implementation, based on NsDom abstractions.
 * Design:
 *   o implement a subset of DOM.  Concrete classes include:
 *     NsXDOMDocument -- DOMDocument, NsDomElement
 *     NsXDOMElement -- DOMElement, NsDomElement
 *     NsXDOMText -- DOMText, NsDomText
 *     NsXDOMAttr -- DOMAttr, NsDomAttr
 *     NsXDOMNamedNodeMap -- DOMNamedNodeMap, NsDomNamedNodeMap
 *   o methods for DOM construction are NOT implemented
 *   o unimplemented/unsupported interfaces will throw an exception
 *   o there is no NsXDOMNode implementation; each subclass of DOMNode
 *   is on its own.  Because the implementations are either trivial or
 *   not shareable, this makes sense.
 *   o there are no implementations for comment, cdata, or processing
 *   instruction nodes.  These are all implemented inside NsXDOMText,
 *   which is typed.  XQilla and XQuery never ask for instances of
 *   these types, and instead rely on Node methods.
 *
 * Implementation note:  methods are not implemented/supported are
 * at the end of each class definition, with inlined implementations.
 */
//
// TBD: make constructors protected/private and declare factory a friend
//
#include <xercesc/dom/DOMNode.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/dom/DOMAttr.hpp>
#include <xercesc/dom/DOMCDATASection.hpp>
#include <xercesc/dom/DOMComment.hpp>
#include <xercesc/dom/DOMProcessingInstruction.hpp>
#include <xercesc/dom/DOMNamedNodeMap.hpp>
#include <xercesc/dom/DOMNodeList.hpp>
#include <xercesc/dom/DOMNode.hpp>
#include <xercesc/framework/MemoryManager.hpp>
#include "NsDom.hpp"
#include "NsUtil.hpp"

// The XERCES_CPP... string is just too long
#define XNSQ XERCES_CPP_NAMESPACE_QUALIFIER

namespace DbXml
{

class NsXDOMDocument;

static const xmlch_t _nsDomString[] = { 'N', 's', 'D', 'o', 'm', 0 };
static const xmlch_t _nsXDomString[] = { 'N', 's', 'X', 'D', 'o', 'm', 0 };

//
// Error functions -- will throw
//
void NsXDOMNotImplemented(const char *name);
void NsXDOMNotSupported(const char *name);

//
// NsXDOMDocument
//
class NsXDOMDocument: public XNSQ DOMDocument,
		      public NsDomElement {
	friend class NsXDOMFactory;
public:
	NsXDOMDocument(NsDocument *doc, NsNode *node) :
		NsDomElement(doc, node, 0 /* parent */, false) {};

public:
	virtual ~NsXDOMDocument() {};

	// DOMDocumentTraversal
	virtual XNSQ DOMNodeIterator *
	    createNodeIterator(XNSQ DOMNode *root,
			       unsigned long whatToShow,
			       XNSQ DOMNodeFilter *filter,
			       bool entityReferenceExpansion);
	virtual XNSQ DOMElement *getDocumentElement() const;
	virtual const XMLCh *getVersion() const;
	virtual const XMLCh *getDocumentURI() const;
	virtual void setDocumentURI(const XMLCh* documentURI);
	virtual bool getStandalone() const;

	//
	// DOMNode virtual interface
	//
	// getters
	virtual const XMLCh *getNodeName() const;
	virtual const XMLCh *getNodeValue() const;
	virtual void setNodeValue(const XMLCh  *nodeValue) {}
	virtual short getNodeType() const;
	virtual XNSQ DOMNode *getParentNode() const;
	virtual XNSQ DOMNodeList *getChildNodes() const;
	virtual XNSQ DOMNode *getFirstChild() const;
	virtual XNSQ DOMNode *getLastChild() const;
	virtual XNSQ DOMNode *getPreviousSibling() const;
	virtual XNSQ DOMNode *getNextSibling() const;
	virtual XNSQ DOMNamedNodeMap *getAttributes() const;
	virtual XNSQ DOMDocument *getOwnerDocument() const;
	virtual XNSQ DOMNode *cloneNode(bool deep) const;
	virtual XNSQ DOMNode *removeChild(XNSQ DOMNode *oldChild);
 	virtual XNSQ DOMNode *appendChild(XNSQ DOMNode *newChild);
	virtual XNSQ DOMNode *insertBefore(XNSQ DOMNode *newChild,
					   XNSQ DOMNode *refChild);
	virtual XNSQ DOMNode *replaceChild(XNSQ DOMNode *newChild,
					   XNSQ DOMNode *oldChild);

	// query methods
	virtual bool hasChildNodes() const;

	// newer DOM methods (level 2, 3, experimental)
	virtual const XMLCh *getNamespaceURI() const;
	virtual const XMLCh *getPrefix() const;
	virtual void setPrefix(const XMLCh *prefix) {}
	virtual const XMLCh *getLocalName() const;
	virtual bool hasAttributes() const;
	virtual bool isSameNode(const XNSQ DOMNode* other) const;
	virtual bool isEqualNode(const XNSQ DOMNode* arg) const;
	virtual const XMLCh *getBaseURI() const;
	virtual short compareTreePosition(const XNSQ DOMNode* other) const;
	virtual const XMLCh *getTextContent() const;
	virtual void setTextContent(const XMLCh* textContent) {}
	virtual const XMLCh *lookupNamespacePrefix(const XMLCh* namespaceURI,
						   bool useDefault) const;
	virtual const XMLCh *lookupNamespaceURI(const XMLCh* prefix) const ;

	// Virtual DOMDocument interface
	virtual XNSQ DOMAttr *createAttribute(const XMLCh *name);
	virtual XNSQ DOMAttr *createAttributeNS(const XMLCh *namespaceURI,
						const XMLCh *qualifiedName);
	virtual XNSQ DOMText *createTextNode(const XMLCh *data);
	virtual XNSQ DOMComment *createComment(const XMLCh *data);
	virtual XNSQ DOMCDATASection *createCDATASection(const XMLCh *data);
	virtual XNSQ DOMProcessingInstruction
	         *createProcessingInstruction(const XMLCh *target,
					      const XMLCh *data);
	virtual XNSQ DOMElement *createElement(const XMLCh *tagName);
	virtual XNSQ DOMElement *createElementNS(const XMLCh *namespaceURI,
						 const XMLCh *qualifiedName);
	virtual XNSQ DOMNode *importNode(XNSQ DOMNode *importedNode,
					 bool deep);
	virtual XNSQ DOMNode *adoptNode(XNSQ DOMNode* source);
	virtual XNSQ DOMNode *
	renameNode(XNSQ DOMNode* n, const XMLCh* namespaceURI,
		   const XMLCh* qualifiedName);

	virtual const XMLCh *getEncoding() const;
	virtual const XMLCh *getActualEncoding() const;

	// non-standard Xerces extensions
	virtual void release();
	virtual XNSQ DOMNode *getInterface(const XMLCh* feature) {
		// This interface is hijacked to cast from DOMNode *
		// to NsDomNode *.  The feature string must be
		// "NsDom"
		if (feature) {
			if (NsUtil::nsStringEqual((const xmlch_t *)feature,
						  _nsDomString))
				return (XNSQ DOMNode *)((NsDomNode*)this);
		}
		return 0;
	}
	// For NsDom extensions, use the
	// NsDomNode methods, such as numNsAttributes(), nodeId(), etc.
	// directly
	virtual void *getNsInterface(const char *name) { return this; }

	//
	// unimplemented DOMNode methods
	//
	// node methods (modify)
	virtual void normalize() {
		NsXDOMNotImplemented("normalize");
	}
	virtual bool isSupported(const XMLCh *feature,
				 const XMLCh *version) const {
		NsXDOMNotImplemented("isSupported"); return false;
	}
	virtual void *setUserData(const XMLCh* key,
				  void* data,
				  XNSQ DOMUserDataHandler* handler) {
		NsXDOMNotImplemented("setUserData"); return 0;
	}
	virtual void *getUserData(const XMLCh* key) const {
		NsXDOMNotImplemented("getUserData"); return 0;
	}
	virtual bool isDefaultNamespace(const XMLCh* namespaceURI) const {
		NsXDOMNotImplemented("isDefaultNamespace"); return false;
	}

	//
	// end of implemented methods -- the rest will throw
	//

	// Virtual DOMDocument interface
	virtual XNSQ DOMDocumentFragment *createDocumentFragment() {
		NsXDOMNotImplemented("createDocumentFragment"); return 0;
	}
	virtual XNSQ DOMEntityReference *
	createEntityReference(const XMLCh *name) {
		NsXDOMNotImplemented("createEntityReference"); return 0;
	}
	virtual XNSQ DOMDocumentType *getDoctype() const {
		NsXDOMNotImplemented("getDocType"); return 0;
	}
	virtual XNSQ DOMImplementation *getImplementation() const {
		NsXDOMNotImplemented("getImplementation"); return 0;
	}
	virtual XNSQ DOMNodeList *
	getElementsByTagName(const XMLCh *tagname) const {
		NsXDOMNotImplemented("getElementsByTagName"); return 0;
	}
	virtual XNSQ DOMNodeList *
	getElementsByTagNameNS(const XMLCh *namespaceURI,
			       const XMLCh *localName) const {
		NsXDOMNotImplemented("getElementsByTagNameNS"); return 0;
	}
	virtual  XNSQ DOMElement *
	getElementById(const XMLCh *elementId) const {
		NsXDOMNotImplemented("getElementById"); return 0;
	}

	// DOM 3
	virtual void setActualEncoding(const XMLCh* actualEncoding) {
		NsXDOMNotImplemented("setActualEncoding");
	}
	// getEncoding is supported...
	virtual void setEncoding(const XMLCh* encoding) {
		NsXDOMNotImplemented("setEncoding");
	}
	virtual void setStandalone(bool standalone) {
		NsXDOMNotImplemented("setStandalone");
	}
	virtual void setVersion(const XMLCh* version) {
		NsXDOMNotImplemented("setVersion");
	}
	virtual bool getStrictErrorChecking() const {
		NsXDOMNotImplemented("getStrictErrorChecking"); return false;
	}
	virtual void setStrictErrorChecking(bool strictErrorChecking) {
		NsXDOMNotImplemented("setStrictErrorChecking");
	}
	virtual void normalizeDocument() {
		NsXDOMNotImplemented("normalizeDocument");
	}
	virtual XNSQ DOMConfiguration *getDOMConfiguration() const {
		NsXDOMNotImplemented("getDOMConfiguration"); return 0;
	}

	// non-standard Xerces extensions
	virtual XNSQ DOMEntity *createEntity(const XMLCh *name) {
		NsXDOMNotImplemented("createEntity"); return 0;
	}
	virtual XNSQ DOMDocumentType *createDocumentType(const XMLCh *name) {
		NsXDOMNotImplemented("createDocumentType"); return 0;
	}
	virtual XNSQ DOMDocumentType *createDocumentType(const XMLCh *qName,
							 const XMLCh *pubId,
							 const XMLCh *sysId) {
		NsXDOMNotImplemented("createDocumentType1"); return 0;
	}
	virtual XNSQ DOMNotation *createNotation(const XMLCh *name) {
		NsXDOMNotImplemented("createNotation"); return 0;
	}
	virtual XNSQ DOMElement *createElementNS(const XMLCh *namespaceURI,
					    const XMLCh *qualifiedName,
					    const XMLSSize_t lineNum,
					    const XMLSSize_t columnNum) {
		NsXDOMNotImplemented("createElementNS"); return 0;
	}

	// DOMDocumentRange
	virtual XNSQ DOMRange *createRange() {
		NsXDOMNotImplemented("createRange"); return 0;
	}
	// DOMXPathEvaluator
	virtual const XNSQ DOMXPathExpression *
	    createExpression(const XMLCh *expression,
			     const XNSQ DOMXPathNSResolver *resolver) {
		NsXDOMNotImplemented("createExpression"); return 0;
	}

	virtual const XNSQ DOMXPathNSResolver *
	    createNSResolver(XNSQ DOMNode *nodeResolver) {
		NsXDOMNotImplemented("createNSResolver"); return 0;
	}

	virtual void* evaluate(const XMLCh *expression,
			       XNSQ DOMNode *contextNode,
			       const XNSQ DOMXPathNSResolver *resolver,
			       unsigned short type, void* result) {
		NsXDOMNotImplemented("evaluate"); return 0;
	}
	virtual XNSQ DOMTreeWalker *
	    createTreeWalker(XNSQ DOMNode *root,
			     unsigned long     whatToShow,
			     XNSQ DOMNodeFilter  *filter,
			     bool  entityReferenceExpansion) {
		NsXDOMNotImplemented("createTreeWalker"); return 0;
	}
private:
	XNSQ DOMText *createDocumentTypeTextNode(XNSQ DOMNode *docTypeNode);
};

//
// NsXDOMElement
//
class NsXDOMElement: public XNSQ DOMElement,
		     public NsDomElement {
	friend class NsXDOMFactory;
public:
	NsXDOMElement(NsDocument *doc, NsNode *node,
		      NsDomElement *parent, bool standalone) :
		NsDomElement(doc, node, parent, standalone) {};

public:
	virtual ~NsXDOMElement() {};
	//
	// DOMElement interfaces
	//
	virtual const XMLCh *getTagName() const { return getNodeName(); }
	virtual const XNSQ DOMTypeInfo* getTypeInfo() const;

	virtual XNSQ DOMAttr *getAttributeNode(const XMLCh *name) const;
	virtual XNSQ DOMAttr *setAttributeNode(XNSQ DOMAttr *newAttr);
	virtual XNSQ DOMAttr *removeAttributeNode(XNSQ DOMAttr *oldAttr);

	virtual const XMLCh *getAttribute(const XMLCh *name) const;
	virtual bool hasAttribute(const XMLCh *name) const;
	virtual void setAttribute(const XMLCh *name, const XMLCh *value);
	virtual void removeAttribute(const XMLCh *name);

	virtual XNSQ DOMAttr *getAttributeNodeNS(const XMLCh *namespaceURI,
						 const XMLCh *localName) const;
	virtual XNSQ DOMAttr *setAttributeNodeNS(XNSQ DOMAttr *newAttr);

	virtual const XMLCh *getAttributeNS(const XMLCh *namespaceURI,
					    const XMLCh *localName) const;
	virtual bool hasAttributeNS(const XMLCh *namespaceURI,
				    const XMLCh *localName) const;
	virtual void setAttributeNS(const XMLCh *namespaceURI,
				    const XMLCh *qualifiedName,
				    const XMLCh *value);
	virtual void removeAttributeNS(const XMLCh *namespaceURI,
				       const XMLCh *localName);

	//
	// DOMNode virtual interface
	//
	// getters
	virtual const XMLCh *getNodeName() const;
	virtual const XMLCh *getNodeValue() const;
	virtual void setNodeValue(const XMLCh  *nodeValue) {}
	virtual short getNodeType() const;
	virtual XNSQ DOMNode *getParentNode() const;
	virtual XNSQ DOMNodeList *getChildNodes() const;
	virtual XNSQ DOMNode *getFirstChild() const;
	virtual XNSQ DOMNode *getLastChild() const;
	virtual XNSQ DOMNode *getPreviousSibling() const;
	virtual XNSQ DOMNode *getNextSibling() const;
	virtual XNSQ DOMNamedNodeMap *getAttributes() const;
	virtual XNSQ DOMDocument *getOwnerDocument() const;
	virtual XNSQ DOMNode *cloneNode(bool deep) const;
	virtual XNSQ DOMNode *removeChild(XNSQ DOMNode *oldChild);
	virtual XNSQ DOMNode *appendChild(XNSQ DOMNode *newChild);
	virtual XNSQ DOMNode *insertBefore(XNSQ DOMNode *newChild,
					   XNSQ DOMNode *refChild);
	virtual XNSQ DOMNode *replaceChild(XNSQ DOMNode *newChild,
					   XNSQ DOMNode *oldChild);

	// query methods
	virtual bool hasChildNodes() const;

	// newer DOM methods (level 2, 3, experimental)
	virtual const XMLCh *getNamespaceURI() const;
	virtual const XMLCh *getPrefix() const;
	virtual void setPrefix(const XMLCh *prefix);
	virtual const XMLCh *getLocalName() const;
	virtual bool hasAttributes() const;
	virtual bool isSameNode(const XNSQ DOMNode* other) const;
	virtual bool isEqualNode(const XNSQ DOMNode* arg) const;
	virtual const XMLCh *getBaseURI() const;
	virtual short compareTreePosition(const XNSQ DOMNode* other) const;
	virtual const XMLCh *getTextContent() const;
	virtual void setTextContent(const XMLCh* textContent);
	virtual const XMLCh *lookupNamespacePrefix(const XMLCh* namespaceURI,
						   bool useDefault) const;
	virtual const XMLCh *lookupNamespaceURI(const XMLCh* prefix) const ;

	// non-standard Xerces extensions
	virtual void release();
	virtual XNSQ DOMNode *getInterface(const XMLCh* feature) {
		// This interface is hijacked to cast from DOMNode *
		// to NsDomNode *.  The feature string must be
		// "NsDom"
		if (feature) {
			if (NsUtil::nsStringEqual((const xmlch_t *)feature,
						  _nsDomString))
				return (XNSQ DOMNode *)((NsDomNode*)this);
			if (NsUtil::nsStringEqual((const xmlch_t *)feature,
						  _nsXDomString))
				return this;
		}
		return 0;
	}

	// For NsDom extensions, use the
	// NsDomNode methods, such as numNsAttributes(), nodeId(), etc.
	// directly
	virtual void *getNsInterface(const char *name) { return this; }

	// NsXDOM extension (not virtual)
	XNSQ DOMNode *insertBefore(XNSQ DOMNode *newChild,
				   XNSQ DOMNode *refChild,
				   NsDomInsertType itype);

	//
	// unimplemented DOMNode methods
	//
	// node methods (modify)
	virtual void normalize() {
		NsXDOMNotImplemented("normalize");
	}
	virtual bool isSupported(const XMLCh *feature,
				 const XMLCh *version) const {
		NsXDOMNotImplemented("isSupported"); return false;
	}
	virtual void *setUserData(const XMLCh* key,
				  void* data,
				  XNSQ DOMUserDataHandler* handler) {
		NsXDOMNotImplemented("setUserData"); return 0;
	}
	virtual void *getUserData(const XMLCh* key) const {
		NsXDOMNotImplemented("getUserData"); return 0;
	}
	virtual bool isDefaultNamespace(const XMLCh* namespaceURI) const {
		NsXDOMNotImplemented("isDefaultNamespace"); return false;
	}
	//
	// end of implemented interfaces
	//

	//
	// unimplemented DOMElement methods
	//
	virtual XNSQ DOMNodeList *
	getElementsByTagName(const XMLCh *name) const {
		NsXDOMNotImplemented("getElementsByTagName"); return 0;
	}

	// DOM level 2
	virtual XNSQ DOMNodeList *
	getElementsByTagNameNS(const XMLCh *namespaceURI,
			       const XMLCh *localName) const {
		NsXDOMNotImplemented("getElementsByTagNameNS"); return 0;
	}

	// DOM level 3
	virtual void setIdAttribute(const XMLCh* name) {
		NsXDOMNotImplemented("setIdAttribute");
	}
	virtual void setIdAttributeNS(const XMLCh* namespaceURI,
				      const XMLCh* localName) {
		NsXDOMNotImplemented("setIdAttributeNS");
	}
	virtual void setIdAttributeNode(const XNSQ DOMAttr *idAttr) {
		NsXDOMNotImplemented("setIdAttributeNode");
	}
};

class NsXDOMAttr: public XNSQ DOMAttr, public NsDomAttr {
	friend class NsXDOMFactory;
public:
	NsXDOMAttr(NsDomElement *owner, int index)
		: NsDomAttr(owner, index) {}
	NsXDOMAttr(NsDocument *doc, const xmlch_t *uri, const xmlch_t *qname)
		: NsDomAttr(doc, uri, qname) {}
public:
	virtual ~NsXDOMAttr() {};

	// Virtual DOMAttr interface
	virtual const XMLCh *getName() const;
	virtual bool getSpecified() const;
	virtual const XMLCh *getValue() const;
	virtual void setValue(const XMLCh *value);

	// DOM level 2
	virtual XNSQ DOMElement *getOwnerElement() const;

	// DOM level 3
	virtual const XNSQ DOMTypeInfo *getTypeInfo() const;
	virtual bool isId() const;

	//
	// DOMNode virtual interface
	//
	// getters
	virtual const XMLCh *getNodeName() const;
	virtual const XMLCh *getNodeValue() const;
	virtual void setNodeValue(const XMLCh  *nodeValue);
	virtual short getNodeType() const;
	virtual XNSQ DOMNode *getParentNode() const;
	virtual XNSQ DOMNodeList *getChildNodes() const;
	virtual XNSQ DOMNode *getFirstChild() const;
	virtual XNSQ DOMNode *getLastChild() const;
	virtual XNSQ DOMNode *getPreviousSibling() const;
	virtual XNSQ DOMNode *getNextSibling() const;
	virtual XNSQ DOMNamedNodeMap *getAttributes() const;
	virtual XNSQ DOMDocument *getOwnerDocument() const;
	virtual XNSQ DOMNode *cloneNode(bool deep) const;
	virtual XNSQ DOMNode *removeChild(XNSQ DOMNode *oldChild);
	virtual XNSQ DOMNode *appendChild(XNSQ DOMNode *newChild);
	virtual XNSQ DOMNode *insertBefore(XNSQ DOMNode *newChild,
					   XNSQ DOMNode *refChild);
	virtual XNSQ DOMNode *replaceChild(XNSQ DOMNode *newChild,
					   XNSQ DOMNode *oldChild);

	// query methods
	virtual bool hasChildNodes() const;

	// newer DOM methods (level 2, 3, experimental)
	virtual const XMLCh *getNamespaceURI() const;
	virtual const XMLCh *getPrefix() const;
	virtual void setPrefix(const XMLCh *prefix);
	virtual const XMLCh *getLocalName() const;
	virtual bool hasAttributes() const;
	virtual bool isSameNode(const XNSQ DOMNode* other) const;
	virtual bool isEqualNode(const XNSQ DOMNode* arg) const;
	virtual const XMLCh *getBaseURI() const;
	virtual short compareTreePosition(const XNSQ DOMNode* other) const;
	virtual const XMLCh *getTextContent() const;
	virtual void setTextContent(const XMLCh* textContent);
	virtual const XMLCh *lookupNamespacePrefix(const XMLCh* namespaceURI,
						   bool useDefault) const;
	virtual const XMLCh *lookupNamespaceURI(const XMLCh* prefix) const ;

	// non-standard Xerces extensions
	virtual void release();
	virtual XNSQ DOMNode *getInterface(const XMLCh* feature) {
		// This interface is hijacked to cast from DOMNode *
		// to NsDomNode *.  The feature string must be
		// "NsDom"
		if (NsUtil::nsStringEqual((const xmlch_t *)feature,
					  _nsDomString))
			return (XNSQ DOMNode *)((NsDomNode*)this);
		return 0;
	}

	// For NsDom extensions, use the
	// NsDomNode methods, such as numNsAttributes(), nodeId(), etc.
	// directly
	virtual void *getNsInterface(const char *name) { return this; }

	//
	// unimplemented DOMNode methods
	//
	// node methods (modify)
	virtual void normalize() {
		NsXDOMNotImplemented("normalize");
	}
	virtual bool isSupported(const XMLCh *feature,
				 const XMLCh *version) const {
		NsXDOMNotImplemented("isSupported"); return false;
	}
	virtual void *setUserData(const XMLCh* key,
				  void* data,
				  XNSQ DOMUserDataHandler* handler) {
		NsXDOMNotImplemented("setUserData"); return 0;
	}
	virtual void *getUserData(const XMLCh* key) const {
		NsXDOMNotImplemented("getUserData"); return 0;
	}
	virtual bool isDefaultNamespace(const XMLCh* namespaceURI) const {
		NsXDOMNotImplemented("isDefaultNamespace"); return false;
	}
};

//
// NsXDOMText is a shared class for text and cdata
//
class NsXDOMText: public XNSQ DOMCDATASection, public NsDomText {
	friend class NsXDOMFactory;
public:
	NsXDOMText(NsDomElement *parent, NsDomElement *owner,
		   int index, uint32_t type)
		: NsDomText(parent, owner, index, type) {}
	NsXDOMText(NsDocument *doc, const xmlch_t *value, uint32_t type)
		: NsDomText(doc, value, type) {}
public:
	virtual ~NsXDOMText() {};
	//
	// DOMNode virtual interface
	//
	// getters
	virtual const XMLCh *getNodeName() const;
	virtual const XMLCh *getNodeValue() const;
	virtual void setNodeValue(const XMLCh *nodeValue);
	virtual short getNodeType() const;
	virtual XNSQ DOMNode *getParentNode() const;
	virtual XNSQ DOMNodeList *getChildNodes() const;
	virtual XNSQ DOMNode *getFirstChild() const;
	virtual XNSQ DOMNode *getLastChild() const;
	virtual XNSQ DOMNode *getPreviousSibling() const;
	virtual XNSQ DOMNode *getNextSibling() const;
	virtual XNSQ DOMNamedNodeMap *getAttributes() const;
	virtual XNSQ DOMDocument *getOwnerDocument() const;
	virtual XNSQ DOMNode *cloneNode(bool deep) const;
	virtual XNSQ DOMNode *removeChild(XNSQ DOMNode *oldChild);
	virtual XNSQ DOMNode *appendChild(XNSQ DOMNode *newChild);
	virtual XNSQ DOMNode *insertBefore(XNSQ DOMNode *newChild,
					   XNSQ DOMNode *refChild);
	virtual XNSQ DOMNode *replaceChild(XNSQ DOMNode *newChild,
					   XNSQ DOMNode *oldChild);

	// query methods
	virtual bool hasChildNodes() const;

	// newer DOM methods (level 2, 3, experimental)
	virtual const XMLCh *getNamespaceURI() const;
	virtual const XMLCh *getPrefix() const;
	virtual void setPrefix(const XMLCh *prefix) {}
	virtual const XMLCh *getLocalName() const;
	virtual bool hasAttributes() const;
	virtual bool isSameNode(const XNSQ DOMNode* other) const;
	virtual bool isEqualNode(const XNSQ DOMNode* arg) const;
	virtual const XMLCh *getBaseURI() const;
	virtual short compareTreePosition(const XNSQ DOMNode* other) const;
	virtual const XMLCh *getTextContent() const;
	virtual void setTextContent(const XMLCh* textContent);
	virtual const XMLCh *lookupNamespacePrefix(const XMLCh* namespaceURI,
						   bool useDefault) const;
	virtual const XMLCh *lookupNamespaceURI(const XMLCh* prefix) const ;
	virtual bool getIsWhitespaceInElementContent() const;
	virtual bool isIgnorableWhitespace() const;

	// non-standard Xerces extensions
	virtual void release();
	virtual XNSQ DOMNode *getInterface(const XMLCh* feature) {
		// This interface is hijacked to cast from DOMNode *
		// to NsDomNode *.  The feature string must be
		// "NsDom"
		if (NsUtil::nsStringEqual((const xmlch_t *)feature,
					  _nsDomString))
			return (XNSQ DOMNode *)((NsDomNode*)this);
		return 0;
	}

	//
	// DOMCharacterData virtual interface
	//
	virtual const XMLCh *getData() const;
	virtual void setData(const XMLCh *data);

	// For NsDom extensions, use the
	// NsDomNode methods, such as numNsAttributes(), nodeId(), etc.
	// directly
	virtual void *getNsInterface(const char *name) { return this; }

	//
	// unimplemented DOMNode methods
	//
	// node methods (modify)
	virtual void normalize() {
		NsXDOMNotImplemented("normalize");
	}
	virtual bool isSupported(const XMLCh *feature,
				 const XMLCh *version) const {
		NsXDOMNotImplemented("isSupported"); return false;
	}
	virtual void *setUserData(const XMLCh* key,
				  void* data,
				  XNSQ DOMUserDataHandler* handler) {
		NsXDOMNotImplemented("setUserData"); return 0;
	}
	virtual void *getUserData(const XMLCh* key) const {
		NsXDOMNotImplemented("getUserData"); return 0;
	}
	virtual bool isDefaultNamespace(const XMLCh* namespaceURI) const {
		NsXDOMNotImplemented("isDefaultNamespace"); return false;
	}
	//
	// unimplemented DOMCharacterData methods
	//
	virtual XMLSize_t getLength() const {
		NsXDOMNotImplemented("getLength"); return 0;
	}
	virtual const XMLCh *substringData(XMLSize_t offset,
					   XMLSize_t count) const {
		NsXDOMNotImplemented("substringData"); return 0;
	}
	virtual void appendData(const XMLCh *arg) {
		NsXDOMNotImplemented("appendData");
	}
	virtual void insertData(XMLSize_t offset, const  XMLCh *arg) {
		NsXDOMNotImplemented("insertData");
	}
	virtual void deleteData(XMLSize_t offset,
				XMLSize_t count) {
		NsXDOMNotImplemented("deleteData");
	}
	virtual void replaceData(XMLSize_t offset,
				 XMLSize_t count,
				 const XMLCh *arg) {
		NsXDOMNotImplemented("replaceData");
	}
	//
	// unimplemented DOMText methods
	//
	virtual XNSQ DOMText *splitText(XMLSize_t offset) {
		NsXDOMNotImplemented("splitText"); return 0;
	}
	virtual const XMLCh* getWholeText() {
		NsXDOMNotImplemented("getWholeText"); return 0;
	}
	virtual XNSQ DOMText* replaceWholeText(const XMLCh* content) {
		NsXDOMNotImplemented("replaceWholeText"); return 0;
	}
};

class NsXDOMComment: public XNSQ DOMComment, public NsDomText {
	friend class NsXDOMFactory;
public:
	NsXDOMComment(NsDomElement *parent, NsDomElement *owner, int index)
		: NsDomText(parent, owner, index, NS_COMMENT) {}
	NsXDOMComment(NsDocument *doc, const xmlch_t *value)
		: NsDomText(doc, value, NS_COMMENT) {}
public:
	virtual ~NsXDOMComment() {};
	//
	// DOMNode virtual interface
	//
	// getters
	virtual const XMLCh *getNodeName() const;
	virtual const XMLCh *getNodeValue() const;
	virtual void setNodeValue(const XMLCh  *nodeValue);
	virtual short getNodeType() const;
	virtual XNSQ DOMNode *getParentNode() const;
	virtual XNSQ DOMNodeList *getChildNodes() const;
	virtual XNSQ DOMNode *getFirstChild() const;
	virtual XNSQ DOMNode *getLastChild() const;
	virtual XNSQ DOMNode *getPreviousSibling() const;
	virtual XNSQ DOMNode *getNextSibling() const;
	virtual XNSQ DOMNamedNodeMap *getAttributes() const;
	virtual XNSQ DOMDocument *getOwnerDocument() const;
	virtual XNSQ DOMNode *cloneNode(bool deep) const;
	virtual XNSQ DOMNode *removeChild(XNSQ DOMNode *oldChild);
	virtual XNSQ DOMNode *appendChild(XNSQ DOMNode *newChild);
	virtual XNSQ DOMNode *insertBefore(XNSQ DOMNode *newChild,
					   XNSQ DOMNode *refChild);
	virtual XNSQ DOMNode *replaceChild(XNSQ DOMNode *newChild,
					   XNSQ DOMNode *oldChild);

	// query methods
	virtual bool hasChildNodes() const;

	// newer DOM methods (level 2, 3, experimental)
	virtual const XMLCh *getNamespaceURI() const;
	virtual const XMLCh *getPrefix() const;
	virtual void setPrefix(const XMLCh *prefix) {}
	virtual const XMLCh *getLocalName() const;
	virtual bool hasAttributes() const;
	virtual bool isSameNode(const XNSQ DOMNode* other) const;
	virtual bool isEqualNode(const XNSQ DOMNode* arg) const;
	virtual const XMLCh *getBaseURI() const;
	virtual short compareTreePosition(const XNSQ DOMNode* other) const;
	virtual const XMLCh *getTextContent() const;
	virtual void setTextContent(const XMLCh* textContent);
	virtual const XMLCh *lookupNamespacePrefix(const XMLCh* namespaceURI,
						   bool useDefault) const;
	virtual const XMLCh *lookupNamespaceURI(const XMLCh* prefix) const ;

	// non-standard Xerces extensions
	virtual void release();
	virtual XNSQ DOMNode *getInterface(const XMLCh* feature) {
		// This interface is hijacked to cast from DOMNode *
		// to NsDomNode *.  The feature string must be
		// "NsDom"
		if (NsUtil::nsStringEqual((const xmlch_t *)feature,
					  _nsDomString))
			return (XNSQ DOMNode *)((NsDomNode*)this);
		return 0;
	}

	//
	// DOMCharacterData virtual interface
	//
	virtual const XMLCh *getData() const;
	virtual void setData(const XMLCh *data);

	// For NsDom extensions, use the
	// NsDomNode methods, such as numNsAttributes(), nodeId(), etc.
	// directly
	virtual void *getNsInterface(const char *name) { return this; }

	//
	// unimplemented DOMNode methods
	//
	// node methods (modify)
	virtual void normalize() {
		NsXDOMNotImplemented("normalize");
	}
	virtual bool isSupported(const XMLCh *feature,
				 const XMLCh *version) const {
		NsXDOMNotImplemented("isSupported"); return false;
	}
	virtual void *setUserData(const XMLCh* key,
				  void* data,
				  XNSQ DOMUserDataHandler* handler) {
		NsXDOMNotImplemented("setUserData"); return 0;
	}
	virtual void *getUserData(const XMLCh* key) const {
		NsXDOMNotImplemented("getUserData"); return 0;
	}
	virtual bool isDefaultNamespace(const XMLCh* namespaceURI) const {
		NsXDOMNotImplemented("isDefaultNamespace"); return false;
	}
	//
	// unimplemented DOMCharacterData methods
	//
	virtual XMLSize_t getLength() const {
		NsXDOMNotImplemented("getLength"); return 0;
	}
	virtual const XMLCh *substringData(XMLSize_t offset,
					   XMLSize_t count) const {
		NsXDOMNotImplemented("substringData"); return 0;
	}
	virtual void appendData(const XMLCh *arg) {
		NsXDOMNotImplemented("appendData");
	}
	virtual void insertData(XMLSize_t offset, const  XMLCh *arg) {
		NsXDOMNotImplemented("insertData");
	}
	virtual void deleteData(XMLSize_t offset,
				XMLSize_t count) {
		NsXDOMNotImplemented("deleteData");
	}
	virtual void replaceData(XMLSize_t offset,
				 XMLSize_t count,
				 const XMLCh *arg) {
		NsXDOMNotImplemented("replaceData");
	}
};

class NsXDOMProcessingInstruction: public XNSQ DOMProcessingInstruction, public NsDomText {
	friend class NsXDOMFactory;
public:
	NsXDOMProcessingInstruction(NsDomElement *parent, NsDomElement *owner, int index)
		: NsDomText(parent, owner, index, NS_PINST) {}
	NsXDOMProcessingInstruction(NsDocument *doc, const xmlch_t *target, const xmlch_t *data)
		: NsDomText(doc, target, data) {}
public:
	virtual ~NsXDOMProcessingInstruction() {};
	//
	// DOMNode virtual interface
	//
	// getters
	virtual const XMLCh *getNodeName() const;
	virtual const XMLCh *getNodeValue() const;
	virtual void setNodeValue(const XMLCh  *nodeValue);
	virtual short getNodeType() const;
	virtual XNSQ DOMNode *getParentNode() const;
	virtual XNSQ DOMNodeList *getChildNodes() const;
	virtual XNSQ DOMNode *getFirstChild() const;
	virtual XNSQ DOMNode *getLastChild() const;
	virtual XNSQ DOMNode *getPreviousSibling() const;
	virtual XNSQ DOMNode *getNextSibling() const;
	virtual XNSQ DOMNamedNodeMap *getAttributes() const;
	virtual XNSQ DOMDocument *getOwnerDocument() const;
	virtual XNSQ DOMNode *cloneNode(bool deep) const;
	virtual XNSQ DOMNode *removeChild(XNSQ DOMNode *oldChild);
	virtual XNSQ DOMNode *appendChild(XNSQ DOMNode *newChild);
	virtual XNSQ DOMNode *insertBefore(XNSQ DOMNode *newChild,
					   XNSQ DOMNode *refChild);
	virtual XNSQ DOMNode *replaceChild(XNSQ DOMNode *newChild,
					   XNSQ DOMNode *oldChild);

	// query methods
	virtual bool hasChildNodes() const;

	// newer DOM methods (level 2, 3, experimental)
	virtual const XMLCh *getNamespaceURI() const;
	virtual const XMLCh *getPrefix() const;
	virtual void setPrefix(const XMLCh *prefix) {}
	virtual const XMLCh *getLocalName() const;
	virtual bool hasAttributes() const;
	virtual bool isSameNode(const XNSQ DOMNode* other) const;
	virtual bool isEqualNode(const XNSQ DOMNode* arg) const;
	virtual const XMLCh *getBaseURI() const;
	virtual short compareTreePosition(const XNSQ DOMNode* other) const;
	virtual const XMLCh *getTextContent() const;
	virtual void setTextContent(const XMLCh* textContent);
	virtual const XMLCh *lookupNamespacePrefix(const XMLCh* namespaceURI,
						   bool useDefault) const;
	virtual const XMLCh *lookupNamespaceURI(const XMLCh* prefix) const ;

	// non-standard Xerces extensions
	virtual void release();
	virtual XNSQ DOMNode *getInterface(const XMLCh* feature) {
		// This interface is hijacked to cast from DOMNode *
		// to NsDomNode *.  The feature string must be
		// "NsDom"
		if (NsUtil::nsStringEqual((const xmlch_t *)feature,
					  _nsDomString))
			return (XNSQ DOMNode *)((NsDomNode*)this);
		return 0;
	}

	//
	// DOMProcessingInstruction virtual interface
	//
	virtual const XMLCh *getTarget() const;
	virtual const XMLCh *getData() const;
	virtual void setData(const XMLCh * data);

	// For NsDom extensions, use the
	// NsDomNode methods, such as numNsAttributes(), nodeId(), etc.
	// directly
	virtual void *getNsInterface(const char *name) { return this; }

	//
	// unimplemented DOMNode methods
	//
	// node methods (modify)
	virtual void normalize() {
		NsXDOMNotImplemented("normalize");
	}
	virtual bool isSupported(const XMLCh *feature,
				 const XMLCh *version) const {
		NsXDOMNotImplemented("isSupported"); return false;
	}
	virtual void *setUserData(const XMLCh* key,
				  void* data,
				  XNSQ DOMUserDataHandler* handler) {
		NsXDOMNotImplemented("setUserData"); return 0;
	}
	virtual void *getUserData(const XMLCh* key) const {
		NsXDOMNotImplemented("getUserData"); return 0;
	}
	virtual bool isDefaultNamespace(const XMLCh* namespaceURI) const {
		NsXDOMNotImplemented("isDefaultNamespace"); return false;
	}
};

class NsXDOMNamedNodeMap:
		public XNSQ DOMNamedNodeMap, public NsDomNamedNodeMap {
	friend class NsXDOMFactory;
public:
	NsXDOMNamedNodeMap(NsDomElement *owner) :
		NsDomNamedNodeMap(owner) {};
public:
	virtual ~NsXDOMNamedNodeMap() {};

	//  Virtual DOMNamedNodeMap interface
	// getters
	virtual XNSQ DOMNode *item(XMLSize_t index) const;
	virtual XNSQ DOMNode *getNamedItem(const XMLCh *name) const;
	virtual XNSQ DOMNode *setNamedItem(XNSQ DOMNode *arg);
	virtual XNSQ DOMNode *removeNamedItem(const XMLCh *name);
	virtual XNSQ DOMNode *
	getNamedItemNS(const XMLCh *namespaceURI,
		       const XMLCh *localName) const;
	virtual XNSQ DOMNode *setNamedItemNS(XNSQ DOMNode *arg);
	virtual XNSQ DOMNode *removeNamedItemNS(const XMLCh *namespaceURI,
						const XMLCh *localName);
	virtual XMLSize_t getLength() const;

	// For NsDom casting
	virtual void *getNsInterface(const char *name) { return this; }
	virtual NsDomNamedNodeMap *getNsDomNamedNodeMap() { return this; }
	virtual const NsDomNamedNodeMap *getNsDomNamedNodeMap() const
		{ return this; }
};

class NsXDOMNodeList:
		public XNSQ DOMNodeList, public NsDomNodeList {
public:
	NsXDOMNodeList(NsDomElement *owner) :
		NsDomNodeList(owner) {};
public:
	virtual ~NsXDOMNodeList() {};

	//  Virtual DOMNodeList interface
	virtual XNSQ DOMNode *item(XMLSize_t index) const;
	virtual XMLSize_t getLength() const;

	// For NsDom casting
	virtual void *getNsInterface(const char *name) { return this; }
};

// Hack: Xerces' importNode assumes that attribute
// value is a child text node... So create a fake one.  It
// will never be attached to any other DOM node.
class NsXDOMFakeText : public NsXDOMText
{
public:
	NsXDOMFakeText(NsDocument *document, const xmlch_t *value,
		XNSQ DOMNode *parent)
		: NsXDOMText(document, (const xmlch_t*)0, NS_TEXT),
		  _parent(parent)
	{
		_text.set16(document->getMemoryManager(), value, false);
	}
	virtual XNSQ DOMNode *getParentNode() const { return _parent; }
private:
	XNSQ DOMNode *_parent;
};

//
// NsXDOMFactory
//
class NsXDOMFactory: public NsDomFactory {
public:
	NsXDOMFactory(XNSQ MemoryManager *memManager)
		: _memManager(memManager), _emptyNodeList(0), documentNode_(0) {}
public:
	virtual NsDomElement *createNsDomElement(NsDocument *doc,
						 NsNode *node,
						 NsDomElement *parent,
						 bool standalone);
	virtual NsDomText *createNsDomText(NsDomElement *parent,
					   NsDomElement *owner,
					   int index, uint32_t type);
	virtual NsDomNamedNodeMap *createNsDomNodeMap(NsDomElement *owner);
	virtual NsDomNodeList *createNsDomNodeList(NsDomElement *owner);
	virtual NsDomAttr *createNsDomAttr(NsDomElement *owner, int index);

	/// Creates a standalone attribute
	virtual NsDomAttr *createNsDomAttr(NsDocument *doc, const xmlch_t *uri,
					   const xmlch_t *qname);
	/// Creates a standalone text type node
	virtual NsDomText *createNsDomText(NsDocument *doc, const xmlch_t *value,
					   uint32_t type);
	/// Creates a standalone processing instruction type node
	virtual NsDomText *createNsDomText(NsDocument *doc, const xmlch_t *target,
					   const xmlch_t *data);

	virtual void destroyDomList();
	virtual void destroy(NsDomObj *dnode);

	XNSQ DOMDocument *getDocumentNode(NsDocument *nsdoc);

	// Hack: Xerces' importNode assumes that attribute
	// value is a child text node... So create a fake one.
	NsXDOMFakeText *createNsDomFakeText(NsDocument *doc, const xmlch_t *value,
		XNSQ DOMNode *parent);

	static XNSQ DOMElement *CastToDOMElement(NsDomElement *node);
private:
	XNSQ MemoryManager *_memManager;
	NsDomNodeList *_emptyNodeList;
	NsXDOMDocument *documentNode_;
};

XNSQ DOMNode *fakeDynamicCastDOMNode(NsDomNode *node);
inline NsDomNode *fakeDynamicCastNsDomNode(XNSQ DOMNode *node)
{
	if (!node) return 0;
	return (NsDomNode*)node->getInterface(_nsDomString);
}
inline const XNSQ DOMNode *fakeDynamicCastDOMNodeC(const NsDomNode *node)
{ return fakeDynamicCastDOMNode(const_cast<NsDomNode*>(node)); }
inline const NsDomNode *fakeDynamicCastNsDomNodeC(const XNSQ DOMNode *node)
{ return fakeDynamicCastNsDomNode(const_cast<XNSQ DOMNode*>(node)); }

} // namespace DbXml

#endif

