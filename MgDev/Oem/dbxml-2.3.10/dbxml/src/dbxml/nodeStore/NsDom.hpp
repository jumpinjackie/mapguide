//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: NsDom.hpp,v 1.50 2006/10/30 17:45:57 bostic Exp $
//

#ifndef __DBXMLNSDOM_HPP
#define __DBXMLNSDOM_HPP
/*
 * Node storage "DOM" classes.  These are primitive
 * classes that encapsulate NsNode.
 * They provide the underpinnings for higher-level DOM
 * and navigational implementations (e.g. Xerces or other).
 *
 * Higher-level implementations must
 * derive from these classes, and provide a
 * node factory implementation to be associated with
 * an NsDocument instance for node creation.
 *
 * Class hierarchy:
 *  NsDomObj -- base class for all objects.  Only state
 *     is NsDomObj * pointer, which is a linked list of objects for
 *     cleanup on destruction.  This list is normally kept in
 *     the associated NsDocument object.
 *   NsDomNode -- interface.  pure virtual base for nodes.
 *     NsDomNav -- a "navigational" interface, for element and text nodes.
 *         adds parent and sibling pointers
 *       NsDomElement -- element.  Concrete.  Adds NsNode * and
 *           child pointers, as well as plumbing pointers to other
 *           NsDomElement objects which comprise the tree of NsNode
 *           elements (vs DOM nodes).
 *       NsDomText -- concrete text nodes -- includes comments, cdata, PI
 *           points to parent NsDomElem, and caches strings.
 *     NsDomAttr -- concrete attribute node. points to owning NsDomElem,
 *        and caches strings.
 *  NsDomNamedNodeMap -- holds attributes.  A wrapper around the nsAttrList_t.
 *  NsDomFactory -- factory class for above classes -- implemented
 *   by implementor of the DOM layer on top of these classes.
 *
 * NOTES:
 *  o Implementation methods are based on need, and can be expanded
 *  o The Document object is just another NsDomElement
 *  o During a transient parse, an element-only tree is created using
 *    NsDomElement objects (containing NsNode pointers).  DOM access
 *    to this tree will create the DOM tree on demand, materializing
 *    text and attribute nodes, for example.
 *
 *  Encoding note:  the underlying NsNode object may have text
 *    in either UTF-8 or UTF-16.  Because the current (only) DOM
 *    implementation is Xerces-based, the text method in these classes
 *    return UTF-16 strings (xmlch_t *).  The UTF-16-based implementations
 *    will transcode/cache text results on demand.  For this reason,
 *    the methods are not const.
 *
 * Naming convention:
 *    get/set functions for NsDom* objects are named "getNs*"
 *    to avoid collisions with Xerces DOM names.  In general,
 *    they are the same as the DOM names, with the "Ns" inserted.
 */
#include "NsNode.hpp"
#include "NsUtil.hpp"
#include "NsDocument.hpp"
#include <vector>

namespace DbXml
{
// forward
class NsDomNamedNodeMap;
class NsDomNodeList;
class NsDomElement;
class NsDomNav;
class NsDomText;
class NsDomAttr;
class NsDomObj;

// simplified types for NsDom classes
typedef enum {
	nsNodeElement,
	nsNodeText,
	nsNodeAttr,
	nsNodeDocument,
	nsNodeEntStart,
	nsNodeEntEnd
} NsNodeType_t;

typedef enum {
	nsDomInsertBefore,
	nsDomInsertAfter,
	nsDomInsertAppend,
	nsDomInsertUnknown
} NsDomInsertType;
	
// utility class to cache transcoded (or not) UTF-16 strings
// for names, attributes, text
class NsDomString {
public:
	NsDomString() : text_(0), flags_(0) {}
	void set16(XER_NS MemoryManager *mmgr, const xmlch_t *text, bool owned);
	void set(XER_NS MemoryManager *mmgr, const xmlbyte_t *text, bool owned);
	void clear(XER_NS MemoryManager *mmgr);
	const xmlch_t *get16() const;
	const xmlbyte_t *get() const;
	const bool isOwned() const;
	const bool isUTF16() const;
private:
	enum {
		OWNED = 0x1,
		UTF16 = 0x2
	};
	const void *text_;
	uint32_t flags_;
};

class NsDomFactory {
public:
	virtual ~NsDomFactory() {}
	virtual NsDomElement *createNsDomElement(NsDocument *doc,
						 NsNode *node,
						 NsDomElement *parent,
						 bool standalone) = 0;
	virtual NsDomText *createNsDomText(NsDomElement *parent,
					   NsDomElement *owner,
					   int index, uint32_t type) = 0;
	virtual NsDomNamedNodeMap *createNsDomNodeMap(NsDomElement *owner) = 0;
	virtual NsDomNodeList *createNsDomNodeList(NsDomElement *owner) = 0;
	virtual NsDomAttr *createNsDomAttr(NsDomElement *owner, int index) = 0;

	/// Creates a standalone attribute
	virtual NsDomAttr *createNsDomAttr(NsDocument *doc, const xmlch_t *uri,
					   const xmlch_t *qname) = 0;
	/// Creates a standalone text type node
	virtual NsDomText *createNsDomText(NsDocument *doc, const xmlch_t *value,
					   uint32_t type) = 0;
	/// Creates a standalone processing instruction type node
	virtual NsDomText *createNsDomText(NsDocument *doc, const xmlch_t *target,
					   const xmlch_t *data) = 0;
	/// Creates a standalone element
	virtual NsDomElement *createNsDomElement(NsDocument *doc, const xmlch_t *uri,
						 const xmlch_t *qname);

	virtual void destroyDomList() = 0;
	virtual void destroy(NsDomObj *dnode) = 0;
protected:
	NsDomFactory() : _domFreeList(0) {}
	void addToDomFreeList(NsDomObj *dnode);
	void removeFromDomFreeList(NsDomObj *dnode);
protected:
	NsDomObj *_domFreeList;
};

class NsDomObj {
public:
	virtual ~NsDomObj() {}
	NsDomObj() : _next(0), _prev(0) {}
	virtual void destroy() = 0;
	virtual NsDocument *getNsDocument() const = 0;
	virtual NsDomElement *getNsDocumentObj() { return getNsDocument()->getDocumentNode(); }
	virtual NsDomFactory *getNsDomFactory() const = 0;
	/** similar to DOM getInterface -- allows subclasses to
	    fake dynamic_cast (callers need to check for 0 return) */
	virtual void *getNsInterface(const char *name) { return 0; }
public:
	NsDomObj *getNextObj() { return _next; }
	void setNextObj(NsDomObj *next) { _next = next; }
	NsDomObj *getPrevObj() { return _prev; }
	void setPrevObj(NsDomObj *prev) { _prev = prev; }
protected:
	NsDomObj *_next; ///< linked list for cleanup
	NsDomObj *_prev; ///< linked list for cleanup
};

class NsDomNode : public NsDomObj {
public:
	NsDomNode() : NsDomObj() {}
	// content/navigation
	virtual const xmlch_t *getNsNodeName() const = 0;
	virtual const xmlch_t *getNsNodeValue() const = 0;
	virtual void setNsNodeValue(const xmlch_t *value) {}
	virtual NsDomElement *getNsParentNode() = 0;
	// child/sibling navigation overridden when necessary
	virtual NsDomNav *getNsFirstChild() { return 0; }
	virtual NsDomNav *getNsLastChild() { return 0; }
	virtual NsDomNav *getNsNextSibling() { return 0; }
	virtual NsDomNav *getNsPrevSibling() { return 0; }
	virtual bool hasNsAttributes() const { return false; }
	virtual bool hasNsChildNodes() const { return false; }
	virtual bool hasNsChildElements() const { return false; }
	virtual NsDomNodeList *getNsChildNodes();
	virtual NsNodeType_t getNsNodeType() const = 0;
	virtual bool getNsIsEntityType() const { return false; }

	// namespaces
	virtual const xmlch_t *getNsPrefix() const { return 0; }
	virtual void setNsPrefix(const xmlch_t *prefix) {} // default implementation does nothing
	virtual const xmlch_t *getNsUri() const { return 0; }
	virtual const xmlch_t *getNsLocalName() const = 0;

	virtual const xmlch_t *getNsBaseUri() const = 0;

	// These functions are only guarenteed to return a
	// result if isNativeUTF8 returns true
	virtual bool isNativeUTF8() const = 0;
	virtual const xmlbyte_t *getNsNodeName8() const = 0;
	virtual const xmlbyte_t *getNsLocalName8() const = 0;
	virtual const xmlbyte_t *getNsPrefix8() const { return 0; }
	virtual const xmlbyte_t *getNsUri8() const { return 0; }
	virtual const xmlbyte_t *getNsNodeValue8() const = 0;

#if 0
	// "extensions" beyond DOM spec
	virtual uint32_t numNsChildNodes() const {
		return 0;  // include text children
	}
	virtual uint32_t numNsChildElements() const {
		return 0; // only element children
	}
#endif
	virtual uint32_t numNsAttributes() const {
		return 0;
	}
	virtual const NsNid *getNodeId() const = 0;
	virtual const NsNid *getLastDescendantNid() const {
		return getNsNode()->getLastDescendantNidOrSelf();
	}
	virtual int32_t getNsLevel() const = 0; // attrs return level of owner
	virtual NsNode *getNsNode() const = 0;

	virtual int32_t getIndex() const { return -1; }
	
	XER_NS MemoryManager *getMemoryManager() const {
		return getNsDocument()->getMemoryManager();
	}

	// Node comparison
	bool operator==(const NsDomNode &other) const;
	bool operator!=(const NsDomNode &other) const {
		return ((*this == other) == false);
	}

	// 
	// utilities for use by DOM
	// see DOM3 for semantics
	static const bool compareNsNodes(const NsDomNode *node1,
					 const NsDomNode *node2);
	static const short compareNsPositions(const NsDomNode *node1,
					      const NsDomNode *node2);
	static const xmlch_t *lookupPrefix(NsDomElement *node,
					   const xmlch_t* uri,
					   bool useDefault,
					   NsDomElement *origEl);
	static const xmlch_t *lookupUri(const NsDomNode *node,
					const xmlch_t *prefix);
};

//
// NsDomNav -- add sibling/parent navigational state
//
class NsDomNav : public NsDomNode {
public:
	NsDomNav(NsDomElement *parent);
public:
	void setNsParent(NsDomElement *parent) { _parent = parent; }
	void setNsNextSib(NsDomNav *sib) { _nextSib = sib; }
	void setNsPrevSib(NsDomNav *sib) { _prevSib = sib; }
	NsDomNav *getPrevSib() const { return _prevSib; }
	NsDomNav *getNextSib() const { return _nextSib; }

	virtual const xmlch_t *getNsBaseUri() const;
protected:
	NsDomElement *_parent;
	NsDomNav *_prevSib;
	NsDomNav *_nextSib;
};

/**
 * NsDomElement -- add NsNode and element tree links, as
 * well as NsDomNav child pointers, and an element name cache
 */
class NsDomElement : public NsDomNav {
public:
	//
	// methods from NsDomObj (inlined below)
	//
	virtual NsDocument *getNsDocument() const;
	virtual NsDomFactory *getNsDomFactory() const;

	//
	// methods from NsDomNode
	//
	virtual const xmlch_t *getNsNodeName() const;
	virtual const xmlch_t *getNsLocalName() const;
	virtual const xmlch_t *getNsPrefix() const;
	virtual void setNsPrefix(const xmlch_t *prefix);
	virtual const xmlch_t *getNsUri() const;
	virtual const xmlch_t *getNsNodeValue() const;

	virtual const xmlch_t *getNsBaseUri() const;

	virtual bool isNativeUTF8() const;
	virtual const xmlbyte_t *getNsNodeName8() const;
	virtual const xmlbyte_t *getNsLocalName8() const;
	virtual const xmlbyte_t *getNsPrefix8() const;
	virtual const xmlbyte_t *getNsUri8() const;
	virtual const xmlbyte_t *getNsNodeValue8() const;

	virtual NsDomNav *getNsFirstChild();
	virtual NsDomNav *getNsLastChild(bool forUpdate = false);
	virtual bool hasNsAttributes() const { return _node->hasAttributes(); }
	virtual bool hasNsChildNodes() const { return _node->hasChildNode(); }
	virtual bool hasNsChildElements() const { return _node->hasChildElem(); }
	virtual NsDomNodeList *getNsChildNodes();
	virtual NsNodeType_t getNsNodeType() const;
	virtual NsDomElement *getNsParentNode();
	virtual NsDomNav *getNsNextSibling();
	virtual NsDomNav *getNsPrevSibling();
	virtual const xmlch_t *getNsTextContent() const;
	virtual void setNsTextContent(const xmlch_t *value);
	// NsDomNode extensions
#if 0	
	virtual uint32_t numNsChildNodes() const; // includes entity text nodes
	virtual uint32_t numNsChildElements() const;
#endif
	virtual uint32_t numNsAttributes() const;
	virtual const NsNid *getNodeId() const;
	virtual int32_t getNsLevel() const;
	NsNode *getNsNode() const { return _node; }
	void nsMakeTransient();
	//
	// methods introduced in NsDomElement
	//
	void nsRename(const xmlch_t *uri, const xmlch_t *qname);
	NsDomNamedNodeMap *getNsAttributes();
	NsDomNav *removeNsChild(NsDomNode *child);
	// If refChild is null, the insert works like an append
	NsDomNav *insertNsChild(NsDomNode *child, NsDomNode *refChild,
				NsDomInsertType itype);
	bool isDocumentNode() const {
		return _node->isDoc();
	}
	void setLastDescendantNid(const NsNid *nid) {
		_node->setLastDescendantNid(getMemoryManager(),
					    nid);
	}
	void updateLastDescendants(const NsNid *nid);
	NsDomText *getNsTextNode(int index);
	
public: // these methods are for NsDom* internal use
	NsDomElement(NsDocument *doc, NsNode *node, NsDomElement *parent,
		     bool standalone);
	virtual ~NsDomElement() {}
	virtual void destroy();

	/** Removes the descendants of this element, and schedules them for removal
	    from the database */
	void removeElemDescendants();

	// these will fetch persistent node, if necessary
	NsDomElement *getElemParent(bool fetch=true);
	NsDomElement *getElemFirstChild(bool fetch=true);
	NsDomElement *getElemLastChild(bool fetch=true);
	NsDomElement *getElemPrev(bool fetch=true);
	NsDomElement *getElemNext(bool fetch=true);
	/**
	 * Returns the descendant with the given NID.
	 * This method returns null if the node does not exist.
	 */
	NsDomElement *lookupDescendantElement(const NsNid *nid, bool fetch=true);
	NsDomElement *lookupElement(const NsNid *nid, bool fetch=true);

	// set
	void setElemParent(NsDomElement* node) { setNsParent(node); }
	void setElemFirstChild(NsDomElement* node) {
		_nsElemFirstChild = node;
	}
	void setElemLastChild(NsDomElement* node) {
		_nsElemLastChild = node;
	}
	void setElemPrev(NsDomElement* node) {
		_nsElemPrev = node;
		if (node) {
			node->_nsElemNext = this;
			_node->setFlag(NS_HASPREV);
			node->getNsNode()->setFlag(NS_HASNEXT);
		}
	}
	void setElemNext(NsDomElement* node) {
		_nsElemNext = node;
		if (node) {
			node->_nsElemPrev = this;
			_node->setFlag(NS_HASNEXT);
			node->getNsNode()->setFlag(NS_HASPREV);
		}
	}

	NsDomNav *makeChildTextNodes(NsDomNav *previous, bool returnLast);
	NsDomNav *makeLeadingTextNodes(NsDomNav *previous, bool returnLast);
protected:
	void _moveTextNodes(NsDomText *start, NsDomElement *target);
	void _coalesceTextNodes(NsDomElement *source,
				NsDomElement *target, bool toChild);
	NsDomElement *_insertNsElement(NsDomElement *child, NsDomNav *refChild,
				       NsDomInsertType itype);
	NsDomElement *_removeNsElement(NsDomElement *child);
	NsDomText *_insertNsText(NsDomText *child, NsDomNav *refChild);
	NsDomText *_removeNsText(NsDomText *child);
	void _makeStandalone();
	NsDomElement *_attachToTree(NsDomElement *parent,
				    NsDomElement *previous,
				    NsDomElement *next,
				    const NsNid *preceding,
				    const NsNid *following,
				    NsDomInsertType itype);

	void _addTreeToModifications(bool first);
	
	/// utility -- internal
	void _getName() const;
protected:
	// NsDocument and NsNode
	NsDocument *_document;
	NsNode  *_node;

	// name, first/lastchild.  Children are  nav nodes
	NsDomNav *_firstChild;
	NsDomNav *_lastChild;

	NsDomNodeList *_childNodeList;
	NsDomNamedNodeMap *_attributes;

	// Names, created/cached on demand
	mutable NsDomString _qname;
	mutable NsDomString _textContent; // getTextContent result
	mutable const xmlch_t *_lname;  // points into, or at _qname string
	mutable NsDomString _baseUri;
	mutable NsDomString _uri;
	mutable NsDomString _prefix;
protected:
	// these links are populated during a transient DOM parse.
	// they are also initialized during node materialization.
	// NOTE: _parent, from NsDomNav, is used as parent, to
	// avoid redundancy.
	NsDomElement *_nsElemFirstChild;
	NsDomElement *_nsElemLastChild;
	NsDomElement *_nsElemPrev;
	NsDomElement *_nsElemNext;
};

//
// NsDomText -- adds cache of text, as well as type
//
class NsDomText : public NsDomNav {
public:
	NsDomText(NsDomElement *parent, NsDomElement *owner,
		  int32_t index, uint32_t type);
	/// Creates a standalone text type node
	NsDomText(NsDocument *doc, const xmlch_t *value, uint32_t type);
	/// Creates a standalone processing instuction type node
	NsDomText(NsDocument *doc, const xmlch_t *target, const xmlch_t *data);
	virtual void destroy();
	//
	// methods from NsDomObj (inlined below)
	//
	virtual NsDocument *getNsDocument() const;
	virtual NsDomFactory *getNsDomFactory() const;
	//
	// methods from NsDomNode
	//
	virtual const xmlch_t *getNsNodeName() const;
	virtual const xmlch_t *getNsLocalName() const;
	virtual const xmlch_t *getNsNodeValue() const;
	virtual void setNsNodeValue(const xmlch_t *value);

	virtual bool isNativeUTF8() const;
	virtual const xmlbyte_t *getNsNodeName8() const;
	virtual const xmlbyte_t *getNsLocalName8() const;
	virtual const xmlbyte_t *getNsNodeValue8() const;

	virtual NsNodeType_t getNsNodeType() const;
	virtual bool getNsIsEntityType() const;
	virtual NsDomElement *getNsParentNode();
	virtual NsDomNav *getNsNextSibling();
	virtual NsDomNav *getNsPrevSibling();
	virtual int32_t getNsLevel() const;
	virtual const NsNid *getNodeId() const;
	virtual NsNode *getNsNode() const;
	virtual int32_t getIndex() const { return _index; }
	void setIndex(int32_t index) { _index = index; }
	
	uint32_t getNsTextType() const { return _type; }
	bool nsIgnorableWhitespace() const;

	//
	// Introduced in NsDomText
	//
	virtual void makeStandalone();
	virtual void makeNonStandalone(NsDomElement *parent,
				       NsDomElement *owner, int index,
				       NsDomNav *prevSib, NsDomNav *nextSib);
	virtual NsDomElement *getOwner() const { return _owner; }
	virtual void setOwner(NsDomElement *owner) { _owner = owner; }
private:
	const xmlch_t *_getText() const;
	void _textSetNodeValue(const xmlch_t *value);
	void _piSetNodeValue(const xmlch_t *value);
protected:
	NsDocument *_doc;
	NsDomElement *_owner; // owning node: may be parent or (later) sibling
	uint32_t _type;
	int32_t _index;
	mutable NsDomString _text;
	mutable NsDomString _value; ///< For processing instructions
};

//
// NsDomAttr -- a single attribute
//
class NsDomAttr : public NsDomNode {
public:
	NsDomAttr(NsDomElement *owner, int index);
	/// Constructor for a standalone attribute
	NsDomAttr(NsDocument *doc, const xmlch_t *uri, const xmlch_t *qname);
	virtual void destroy();
	//
	// methods from NsDomObj (inlined below)
	//
	virtual NsDocument *getNsDocument() const;
	virtual NsDomFactory *getNsDomFactory() const;

	//
	// methods from NsDomNode
	//
	virtual const xmlch_t *getNsNodeName() const;
	virtual const xmlch_t *getNsLocalName() const;
	virtual const xmlch_t *getNsPrefix() const;
	virtual void setNsPrefix(const xmlch_t *prefix);
	virtual const xmlch_t *getNsUri() const;
	virtual const xmlch_t *getNsNodeValue() const;
	virtual void setNsNodeValue(const xmlch_t *value);

	virtual const xmlch_t *getNsBaseUri() const;

	virtual bool isNativeUTF8() const;
	virtual const xmlbyte_t *getNsNodeName8() const;
	virtual const xmlbyte_t *getNsLocalName8() const;
	virtual const xmlbyte_t *getNsPrefix8() const;
	virtual const xmlbyte_t *getNsUri8() const;
	virtual const xmlbyte_t *getNsNodeValue8() const;

	virtual NsDomElement *getNsParentNode() { return 0; }
	virtual NsNodeType_t getNsNodeType() const { return nsNodeAttr; }
	virtual int32_t getNsLevel() const { return _owner ? _owner->getNsLevel() : -1; }
	virtual const NsNid *getNodeId() const;
	virtual NsNode *getNsNode() const;
	virtual int32_t getIndex() const { return _index; }
	//
	// Introduced in NsDomAttr
	//
	virtual bool getNsSpecified() const;
	virtual NsDomElement *getNsOwner() const { return _owner; }
	virtual bool isStandalone() const { return _owner == 0; }
	virtual void makeStandalone();
	virtual void makeNonStandalone(NsDomElement *owner, int index);
	void nsRename(const xmlch_t *uri, const xmlch_t *qname);
	void setIndex(int32_t index) { _index = index; }
private:
	const void _getName() const;
protected:
	/// If _owner is null, this is a standalone attribute
	NsDomElement *_owner;
	NsDocument *_doc;
	int32_t _index;
	mutable NsDomString _name;
	mutable const xmlch_t *_lname;
	/// when not standalone, _value points into _name string at value part
	mutable NsDomString _value;
	mutable NsDomString _prefix;
	mutable NsDomString _uri;
};

typedef std::vector<NsDomAttr *> NsDomAttrVector;

//
// Wrapper for vector of NsDomAttr
//
class NsDomNamedNodeMap : public NsDomObj {
public:
	NsDomNamedNodeMap(NsDomElement *owner);
	virtual ~NsDomNamedNodeMap() {}
	virtual void destroy();
	//
	// methods from NsDomObj (inlined below)
	//
	virtual NsDocument *getNsDocument() const;
	virtual NsDomFactory *getNsDomFactory() const;

	int getNsLength() const;
	NsDomAttr *getNsItem(int index);
	NsDomAttr *setNsItem(int index, NsDomAttr *attr);
	void addNsItem(NsDomAttr *attr);
	NsDomAttr *removeNsItem(int index);

	NsDomAttr *getNsNamedItem(const xmlch_t *name);
	NsDomAttr *setNsNamedItem(NsDomAttr *attr);
	NsDomAttr *removeNsNamedItem(const xmlch_t *name);

	NsDomAttr *getNsNamedItemNS(const xmlch_t *uri, const xmlch_t *localname);
	NsDomAttr *setNsNamedItemNS(NsDomAttr *attr);
	NsDomAttr *removeNsNamedItemNS(const xmlch_t *uri, const xmlch_t *localname);
protected:
	NsDomAttrVector _attrs;  // attributes are added lazily
	NsDomElement *_owner;
};

//
// A nodelist
//
class NsDomNodeList : public NsDomObj {
public:
	NsDomNodeList(NsDomElement *owner);
	virtual ~NsDomNodeList() {}
	virtual void destroy();
	//
	// methods from NsDomObj (inlined below)
	//
	virtual NsDocument *getNsDocument() const;
	virtual NsDomFactory *getNsDomFactory() const;

	int getNsLength() const;
	NsDomNode *getNsItem(int index) const;
protected:
	NsDomElement *_owner;
	mutable int _num;
};

//
// inlines
//
inline NsDocument * NsDomElement::getNsDocument() const
{
	return _document;
}

inline NsDocument * NsDomText::getNsDocument() const
{
	return _doc;
}

inline NsDocument * NsDomAttr::getNsDocument() const
{
	return _doc;
}

inline NsDocument * NsDomNamedNodeMap::getNsDocument() const
{
	return _owner->getNsDocument();
}

inline NsDomFactory *NsDomNamedNodeMap::getNsDomFactory() const
{
	return _owner->getNsDomFactory();
}

inline NsDocument * NsDomNodeList::getNsDocument() const
{
	return _owner->getNsDocument();
}

inline NsDomFactory *NsDomNodeList::getNsDomFactory() const
{
	return _owner->getNsDomFactory();
}

inline void NsDomString::set16(XER_NS MemoryManager *mmgr,
			       const xmlch_t *text, bool owned)
{
	if (text_)
		mmgr->deallocate((void *)text_);
	text_ = (const void *)text;
	flags_ = UTF16;
	if (owned)
		flags_ |= OWNED;
}

inline void NsDomString::set(XER_NS MemoryManager *mmgr,
			     const xmlbyte_t *text, bool owned)
{
	if (text_)
		mmgr->deallocate((void *)text_);
	text_ = (const void *)text;
	flags_ = 0;
	if (owned)
		flags_ |= OWNED;
}

inline const xmlch_t *NsDomString::get16() const
{
	if (isUTF16())
		return (const xmlch_t *) text_;
	return NULL;
}

inline const xmlbyte_t *NsDomString::get() const
{
	if (!isUTF16())
		return (const xmlbyte_t *) text_;
	return NULL;
}

inline const bool NsDomString::isOwned() const
{
	return ((flags_ & OWNED) != 0);
}

inline const bool NsDomString::isUTF16() const
{
	return ((flags_ & UTF16) != 0);
}

}

#endif
