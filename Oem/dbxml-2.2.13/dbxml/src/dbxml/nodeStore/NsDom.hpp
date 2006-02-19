//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//      Sleepycat Software.  All rights reserved.
//
// $Id: NsDom.hpp,v 1.33 2005/12/07 15:58:17 jsnelson Exp $
//

#ifndef __DBXMLNSDOM_HPP
#define __DBXMLNSDOM_HPP
/*
 * Node storage "DOM" classes.  These are primitive
 * classes that encapsulate nsNode_t structures.
 * They provide the underpinnings for higher-level DOM
 * implementations (e.g. Xerces or other).
 *
 * The intent is that higher-level implementations
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
 *       NsDomElement -- element.  Concrete.  Adds nsNode_t * and
 *           child pointers, as well as plumbing pointers to other
 *           NsDomElement objects which comprise the tree of nsNode_t
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
 *  o A number of DOM-like methods are not implemented because they
 *    are not used by Pathan or XQuery.   The implementation can be
 *    extended if necessary.
 *  o The Document object is just another NsDomElement
 *  o During a transient parse, an element-only tree is created using
 *    NsDomElement objects (containing nsNode_t pointers).  DOM access
 *    to this tree will create the DOM tree on demand, materializing
 *    text and attribute nodes, for example.
 *    This is done for lazy evaluation -- there is no reason to
 *    actively create objects that will never be used.
 *  o It would have been possible to implement higher-level DOM
 *    code by layering on top of a single, NsDomNode, abstraction.
 *    The extra layer serves 2 purposes:
 *    1.  it provides a layer to "cache" transcoded strings, and index offsets
 *    2.  it factors out NsDom-specific code, which simplifies the upper layer.
 *    In addition, the higher layer needs to create objects conforming
 *    to a specific DOM anyway; requiring those objects to derive from
 *    NsDom* classes as well doesn't add significant overhead.
 *  o There are no "creation" interfaces, for now.  They are not required by
 *    Pathan/XQuery, and can be worked around for XmlModify.  These
 *    are a goal for the future, so that applications can perform
 *    persistent DOM modifications directly.
 *
 *  Encoding note:  the underlying nsNode_t structure may have text
 *    in either UTF-8 or UTF-16.  Because the current (only) DOM
 *    implementation is Xerces-based, the text method in these classes
 *    return UTF-16 strings (xmlch_t *).  If a UTF-8 based
 *    implementation is desired, the simplest thing is to add function
 *    overloads that return xmlbyte_t *.  The UTF-16-based implementations
 *    will transcode/cache text results on demand.  For this reason,
 *    the methods are not const.
 *
 * Naming convention:
 *    get/set functions for NsDom* objects are named "getNs*"
 *    to avoid collisions with Xerces DOM names.  In general,
 *    they are the same as the DOM names, with the "Ns" inserted.
 */
#include "NsTypes.hpp"
#include "NsUtil.hpp"
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
class NsDocument;

// simplified types for NsDom* classes
typedef enum {
	nsNodeElement,
	nsNodeText,
	nsNodeAttr,
	nsNodeDocument,
	nsNodeEntStart,
	nsNodeEntEnd
} NsNodeType_t;

/** utility class to cache transcoded (or not) UTF-16 strings
    for names, attributes, text */
class NsDomString {
public:
	NsDomString() : _text(0), _owned(false) {}
	void set(const xmlch_t *text, bool owned);
	void clear(XER_NS MemoryManager *mmgr);
	const xmlch_t *get() const;
	const bool getOwned() const;
private:
	const xmlch_t *_text;
	bool _owned;
};

class NsDomFactory {
public:
	virtual ~NsDomFactory() {}
	virtual NsDomElement *createNsDomElement(NsDocument *doc,
						 nsNode_t *node,
						 NsDomElement *parent,
						 bool standalone) = 0;
	virtual NsDomText *createNsDomText(NsDomElement *parent,
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

	/// set document object (instance of document object)
	void setNsDocumentObj(void *doc) { _domDocumentObj = doc; }
	/// get document object (instance of document object)
	void *getNsDocumentObj() { return _domDocumentObj; }
protected:
	NsDomFactory() : _domDocumentObj(0), _domFreeList(0) {}
	void addToDomFreeList(NsDomObj *dnode);
	void removeFromDomFreeList(NsDomObj *dnode);
protected:
	void *_domDocumentObj;
	NsDomObj *_domFreeList;
};

class NsDomObj {
public:
	virtual ~NsDomObj() {}
	NsDomObj() : _next(0), _prev(0) {}
	virtual void destroy() = 0;
	virtual NsDocument *getNsDocument() const = 0;
	virtual NsDomFactory *getNsDomFactory() const = 0;
	/** similar to DOM getInterface -- allows subclasses to
	    fake dynamic_cast (callers need to check for 0 return) */
	virtual void *getNsInterface(const char *name) { return 0; }
	/// handle document object
	void setNsDocumentObj(void *doc) {
		getNsDomFactory()->setNsDocumentObj(doc);
	}
	void *getNsDocumentObj() {
		return getNsDomFactory()->getNsDocumentObj();
	}
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
	virtual NsDomNodeList *getNsChildNodes();
	virtual NsNodeType_t getNsNodeType() const = 0;
	virtual bool getNsIsEntityType() const { return false; }

	// namespaces
	virtual const xmlch_t *getNsPrefix() const { return 0; }
	virtual void setNsPrefix(const xmlch_t *prefix) {} // default implementation does nothing
	virtual const xmlch_t *getNsUri() const { return 0; }
	virtual const xmlch_t *getNsLocalName() const = 0;

	// These functions are only guarenteed to return a
	// result if isNativeUTF8 returns true
	virtual bool isNativeUTF8() const = 0;
	virtual const xmlbyte_t *getNsNodeName8() const = 0;
	virtual const xmlbyte_t *getNsLocalName8() const = 0;
	virtual const xmlbyte_t *getNsPrefix8() const { return 0; }
	virtual const xmlbyte_t *getNsUri8() const { return 0; }
	virtual const xmlbyte_t *getNsNodeValue8() const = 0;

	// "extensions" beyond DOM spec
	virtual uint32_t numNsChildNodes() const {
		return 0;  // include text children
	}
	virtual uint32_t numNsChildElements() const {
		return 0; // only element children
	}
	virtual uint32_t numNsAttributes() const {
		return 0;
	}
	virtual const nid_t *getNodeId() const = 0;
	virtual int32_t getNsLevel() const = 0; // attrs return level of owner
	virtual nsNode_t *getNsNode() const = 0;

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
	static const xmlch_t *lookupBaseUri(const NsDomNode *node);
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
protected:
	NsDomElement *_parent;
	NsDomNav *_prevSib;
	NsDomNav *_nextSib;
};

/**
 * Only behaves correctly with pointer or struct types,
 * that don't require construction.
 */
template<class TYPE>
class Store {
public:
	Store(XER_NS MemoryManager *mmgr)
		: _mmgr(mmgr), _size(0),
		  _maxsize(0), _store(0) {}
	~Store() { if(_store) _mmgr->deallocate(_store); }
	TYPE &operator[](unsigned int i) {
		if(i >= _size) resize(i + 1);
		return _store[i];
	}
	const TYPE &operator[](unsigned int i) const {
		DBXML_ASSERT(i < _size);
		return _store[i];
	}
	void push_back(const TYPE &val) {
		if(_size == _maxsize)
			reserve((_maxsize + 3) << 1);
		_store[_size++] = val;
	}
	TYPE &back() {
		return _store[_size - 1];
	}
	const TYPE &back() const {
		return _store[_size - 1];
	}
	void pop_back() {
		--_size;
	}
	void insert(unsigned int i, const TYPE &val) {
		if(i > _size) resize(i);
		if(_size == _maxsize)
			reserve((_maxsize + 3) << 1);
		TYPE *pos = _store + i;
		memmove(pos + 1, pos, (_size - i) * sizeof(TYPE));
		*pos = val;
		++_size;
	}
	void clear() {
		if(_store) _mmgr->deallocate(_store);
		_size = 0;
		_maxsize = 0;
		_store = 0;
	}
	unsigned int size() const { return _size; }
	void resize(unsigned int i) {
		reserve(i);
		if(i > _size)
			memset(_store + _size, 0, (i - _size) * sizeof(TYPE));
		_size = i;
	}
	void resize(unsigned int i, const TYPE &val) {
		reserve(i);
		for(unsigned int x = _size; x < i; ++x)
			_store[x] = val;
		_size = i;
	}
	void reserve(unsigned int i) {
		if(_maxsize >= i) return;

		TYPE *newstore = (TYPE*)_mmgr->
			allocate(i * sizeof(TYPE));
		if (!newstore)
			NsUtil::nsThrowException(XmlException::NO_MEMORY_ERROR,
			  "Unable to allocate memory for Store::reserve",
			  __FILE__, __LINE__);
		memcpy(newstore, _store, _size * sizeof(TYPE));

		TYPE *tmp = _store;
		_store = newstore;
		_maxsize = i;

		if(tmp)
			_mmgr->deallocate(tmp);
	}
private:
	XER_NS MemoryManager *_mmgr;
	unsigned int _size;
	unsigned int _maxsize;
	TYPE *_store;
};


/**
 * NsDomElement -- add nsNode_t and element tree links, as
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

	virtual bool isNativeUTF8() const;
	virtual const xmlbyte_t *getNsNodeName8() const;
	virtual const xmlbyte_t *getNsLocalName8() const;
	virtual const xmlbyte_t *getNsPrefix8() const;
	virtual const xmlbyte_t *getNsUri8() const;
	virtual const xmlbyte_t *getNsNodeValue8() const;

	virtual NsDomNav *getNsFirstChild();
	virtual NsDomNav *getNsLastChild();
	virtual bool hasNsAttributes() const { return nsHasAttrs(_node); }
	virtual bool hasNsChildNodes() const { return nsHasChildNode(_node); }
	virtual NsDomNodeList *getNsChildNodes();
	virtual NsNodeType_t getNsNodeType() const;
	virtual NsDomElement *getNsParentNode();
	virtual NsDomNav *getNsNextSibling();
	virtual NsDomNav *getNsPrevSibling();
	virtual const xmlch_t *getNsTextContent() const;
	virtual void setNsTextContent(const xmlch_t *value);
	// NsDomNode extensions
	virtual uint32_t numNsChildNodes() const; // includes entity text nodes
	virtual uint32_t numNsChildElements() const;
	virtual uint32_t numNsAttributes() const;
	virtual const nid_t *getNodeId() const;
	virtual int32_t getNsLevel() const;
	nsNode_t *getNsNode() const { return _node; }
	void nsMakeTransient();
	//
	// methods introduced in NsDomElement
	//
	void nsRename(const xmlch_t *uri, const xmlch_t *qname);
	NsDomNamedNodeMap *getNsAttributes();
	NsDomNav *removeNsChild(NsDomNode *child);
	// If refChild is null, the insert works like an append
	NsDomNav *insertNsChild(NsDomNode *child, NsDomNode *refChild);

public: // these methods are for NsDom* internal use
	NsDomElement(NsDocument *doc, nsNode_t *node, NsDomElement *parent,
		     bool standalone);
	virtual ~NsDomElement() {}
	virtual void destroy();

	/** Removes the descendants of this element, and schedules them for removal
	    from the database */
	void removeElemDescendants();

	// these will fetch persistent node, if necessary
	NsDomElement *getElemParent(bool fetch=true);
	NsDomElement *getElemChild(int index, bool fetch=true);
	NsDomElement *getElemFirstChild(bool fetch=true);
	NsDomElement *getElemLastChild(bool fetch=true);
	NsDomElement *getElemPrev(bool fetch=true);
	NsDomElement *getElemNext(bool fetch=true);
	int getIndex() const { return _clIndex; }
	Store<NsDomElement*> &getElemChildren() { return _elemChildren; }
	Store<NsDomText*> &getTextChildren() { return _textChildren; }

	/**
	 * Returns the descendant with the given NID.
	 * This method returns null if the node does not exist.
	 */
	NsDomElement *findDescendantElement(const nid_t *nid);

	// set
	void setElemParent(NsDomElement* node) { setNsParent(node); }
	void setElemFirstChild(NsDomElement* node) {
		_nsElemFirstChild = node;
	}
	void setElemLastChild(NsDomElement* node) {
		_nsElemLastChild = node;
	}
	void setElemPrev(NsDomElement* node) { _nsElemPrev = node; }
	void setElemNext(NsDomElement* node) { _nsElemNext = node; }
	void setIndex(int i) { _clIndex = i; }

	// utility -- available to other classes
	uint32_t getTextIndex();
	NsDomNav* makeTextNodes(int startIndex, int endIndex,
				int retIndex, NsDomNav *prev,
				NsDomNav *next);
protected:
	int childBinarySearch(const nid_t *nid, bool &exact);

	NsDomElement *_insertNsElement(NsDomElement *child, NsDomNav *refChild);
	NsDomElement *_removeNsElement(NsDomElement *child);
	NsDomText *_insertNsText(NsDomText *child, NsDomNav *refChild);
	NsDomText *_removeNsText(NsDomText *child);
	void _makeStandalone();
	NsDomElement *_makeNonStandalone(NsDomElement *parent,
					 int elemIndex,
					 NsDomElement *previous,
					 NsDomElement *next,
					 NsDomElement *preceding,
					 NsDomElement *following);

	/// utility -- internal
	void _getName() const;
protected:
	// NsDocument and nsNode_t
	NsDocument *_document;
	nsNode_t  *_node;
	int _clIndex;

	// name, first/lastchild.  Children are  nav nodes
	NsDomNav *_firstChild;
	NsDomNav *_lastChild;

	Store<NsDomElement*> _elemChildren;
	Store<NsDomText*> _textChildren;

	NsDomNodeList *_childNodeList;
	NsDomNamedNodeMap *_attributes;

	// Names, created/cached on demand
	mutable NsDomString _qname;
	mutable NsDomString _textContent; // getTextContent result
	mutable const xmlch_t *_lname;  // points into, or at _qname string
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
	NsDomText(NsDomElement *parent, int index, uint32_t type);
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
	virtual const nid_t *getNodeId() const;
	virtual nsNode_t *getNsNode() const;

	int getIndex() const { return _index; }
	void setIndex(int index) { _index = index; }
	uint32_t getNsTextType() const { return _type; }
	bool nsIgnorableWhitespace() const;

	//
	// Introduced in NsDomText
	//
	virtual void makeStandalone();
	virtual void makeNonStandalone(NsDomElement *parent, int index,
				       NsDomNav *prevSib, NsDomNav *nextSib);
private:
	const xmlch_t *_getText() const;
	void _textSetNodeValue(const xmlch_t *value);
	void _piSetNodeValue(const xmlch_t *value);
protected:
	NsDocument *_doc;
	int _index;
	uint32_t _type;
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

	virtual bool isNativeUTF8() const;
	virtual const xmlbyte_t *getNsNodeName8() const;
	virtual const xmlbyte_t *getNsLocalName8() const;
	virtual const xmlbyte_t *getNsPrefix8() const;
	virtual const xmlbyte_t *getNsUri8() const;
	virtual const xmlbyte_t *getNsNodeValue8() const;

	virtual NsDomElement *getNsParentNode() { return 0; }
	virtual NsNodeType_t getNsNodeType() const { return nsNodeAttr; }
	virtual int32_t getNsLevel() const { return _owner ? _owner->getNsLevel() : -1; }
	virtual const nid_t *getNodeId() const;
	virtual nsNode_t *getNsNode() const;

	//
	// Introduced in NsDomAttr
	//
	virtual bool getNsSpecified() const;
	virtual NsDomElement *getNsOwner() const { return _owner; }
	virtual bool isStandalone() const { return _owner == 0; }
	virtual void makeStandalone();
	virtual void makeNonStandalone(NsDomElement *owner, int index);
	virtual int getIndex() const { return _index; }
	virtual void setIndex(int index) { _index = index; }
	void nsRename(const xmlch_t *uri, const xmlch_t *qname);
private:
	const void _getName() const;
protected:
	/// If _owner is null, this is a standalone attribute
	NsDomElement *_owner;
	NsDocument *_doc;
	int _index;
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

inline void NsDomString::set(const xmlch_t *text, bool owned)
{
	_text = text;
	_owned = owned;
}

inline const xmlch_t *NsDomString::get() const
{
	return _text;
}

inline const bool NsDomString::getOwned() const
{
	return _owned;
}

}

#endif
