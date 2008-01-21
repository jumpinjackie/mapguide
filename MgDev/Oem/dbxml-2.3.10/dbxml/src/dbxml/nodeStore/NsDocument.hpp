//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: NsDocument.hpp,v 1.37 2006/11/01 11:32:00 jsnelson Exp $
//

#ifndef __DBXMLNSDOCUMENT_HPP
#define __DBXMLNSDOCUMENT_HPP

#include "NsNode.hpp"
#include "NsNid.hpp"
#include "NsFormat.hpp"
#include "../OperationContext.hpp"
#include "../DocID.hpp"
#include "../Document.hpp"
#include <db_cxx.h>
#include <set>
#include <map>

namespace DbXml
{

// forwards
class NsDomElement;
class NsDomObj;
class NsDomNode;
class NsDomFactory;
class NsDocumentDatabase;
class NsDocument;
class Document;
class NsDocInfo;
	
class NodeModification {
public:
	enum Action {
		ADD,
		UPDATE,
		REMOVE
	};

	NodeModification(NsDocument &d, Action a, NsDomElement *n);
	~NodeModification();

	Action getAction() const { return action_; }
	const NsNid *getNID() const;
	const NsDomElement *getNode() const;

	void setToAdd(NsDomElement *n) const;
	void setToUpdate(NsDomElement *n) const;
	void setToRemove(const NsNid *n) const;
	void reset() const;

	bool operator<(const NodeModification &o) const;
private:
	NodeModification(const NodeModification &o);
	NodeModification &operator=(const NodeModification &o);
private:
	NsDocument &doc_;
	mutable Action action_;
	mutable union {
		NsNid nid_;
		NsDomElement *node_;
	} value_;
};

struct NodeModificationSort
{
	bool operator()(const NodeModification *a, const NodeModification *b)
	{
		return *a < *b;
	}
};

typedef std::set<NodeModification*, NodeModificationSort> NodeModifications;

/*
 * NsDocument
 *
 * The class that encapsulates Node Storage state.
 * It is the interface between the low-level storage
 * and input/ouput classes (parsers, event generators, and DOM).
 *
 * There is one instance of this class for each XML document.
 */
class NsDocument
{
public:	
	// Basic ctor, needs initialize call to specialize
	NsDocument(const Document *ownerDoc, XER_NS MemoryManager *memManager);
	~NsDocument();

	// init to create a new node storage doc
	void initDoc(Transaction *txn, NsDocumentDatabase *ndb,
		     DictionaryDatabase *ddb,
		     const DocID &docId, u_int32_t flags);
	// init for handling DOM (used for both DOM parse and
	// node storage materialization).  Requires dictionary.
	void initDOM(NsDomFactory *factory, const DocID &docId,
		     DictionaryDatabase *ddb);
	// init for pure DOM parse (initDOM has been called)
	void initDOMParse(Transaction *txn, u_int32_t flags,
			  bool forQuery);
	// init for an index parse -- whole document
	void initIndex();

	void completeNode(NsNode *node);
	void completeDocument(void);
	void copyId(NsNid *to, const NsNid *from);
	void freeId(NsNid *id);

	//
	// accessors to namespace info (both utf-8 and utf-16)
	//
	int addIDForString(const char *strng, uint32_t len);
	const char *getStringForID(int32_t id);
	const xmlch_t *getStringForID16(int32_t id);
	const char *getStringForID(int32_t id, DbtOut &dbt);
	const xmlch_t *getStringForID16(int32_t id, DbtOut &dbt);

	const xmlch_t *getQname(const nsName_t *name, bool isUTF16,
				bool &owned);
	const xmlch_t *getText(const nsText_t *text, bool isUTF16,
				bool &owned);
	//
	// access to document metadata (NsDocInfo object)
	//
	const xmlbyte_t *getXmlDecl() const;
	const xmlch_t *getXmlDecl16() const;
	void setXmlDecl(int32_t decl);
	const xmlbyte_t *getSniffedEncodingStr() const;
	void setSniffedEncodingStr(const xmlbyte_t *str);
	const xmlch_t *getEncodingStr16() const;
	const xmlbyte_t *getEncodingStr() const;
	void setEncodingStr(const xmlbyte_t *str);
	const xmlbyte_t *getStandaloneStr() const;
	void setStandalone(bool standalone);
	const bool getIsStandalone() const;

	void setDocumentUri(const xmlch_t *uri);
	const xmlch_t *getDocumentUri() const;
	
	XER_NS MemoryManager *getMemoryManager() { return _memManager; }

	//
	// DOM access
	//

	// Does not search the tree for it first
	NsDomElement *fetchDomElement(const NsNid *nid);
	NsDomElement *fetchNextDomElement(const NsNid *nid);

	NsDomElement *getDomElement(const NsNid *nid);
	NsDomElement *getDocumentNode();
	void setDocumentElement(NsDomElement *node) { _domDocNode = node; }
	void releaseDomElement(NsDomElement *domElement);
	NsDomFactory *getDomFactory(void) const { return _domFactory; }
	void setNsDomFactory(NsDomFactory *factory) { _domFactory = factory; }
	bool getCreateEntityText(void) const { return _domCreateEntityText; }
	void setCreateEntityText(bool val) { _domCreateEntityText = val; }
	bool isFromParse() const { return _fromParse; }

	//
	// DOM modification methods
	//
	void addToModifications(NodeModification::Action action,
				NsDomElement *node);
	void clearModifications();
	NodeModifications &getModifications() {
		if (!_modifications)
			_modifications = new NodeModifications();
		return *_modifications;
	}

	//
	// DB access
	//
	void updateSpecialNodes(OperationContext &oc);
	NsDocumentDatabase *getDb(void) { return _ndb; }
	const DocID &getDocId(void) const { return _docId; }
	Transaction *getTxn(void) const { return _oc.txn(); }
	bool hasContent();

	bool isSameDocument(const NsDocument *other) const {
		return this == other ||
			(_docId != 0 && _docId == other->getDocId() &&
			 _ownerDoc->getContainer() ==
			 other->_ownerDoc->getContainer());
	}
#ifdef NS_DEBUG
	// Debug
	void dumpDocument(const NsNid *nid = 0);
	void _nsDumpNode(NsDomElement *dnode, const char *indent = "");
	void _domTraverse(NsDomNode *node);
#endif	
private:
	// internal get/put methods for nodes
	NsNode *_getNode(const NsNid *nid, bool getNext);
	NsDomElement *_fetchDomElement(const NsNid *pnid);
	void ensureDocInfo(bool fetch) const {
		if (!_docInfo)
			return createDocInfo(fetch);
	}
	void createDocInfo(bool fetch) const;
private:
	// state for both parsing and access
	const Document *_ownerDoc;
	XER_NS MemoryManager *_memManager;
	DocID _docId;
	mutable bool _hasContent;

	// xml decl info
	mutable NsDocInfo *_docInfo;

	// We don't own _documentUri, so we don't have to delete it
	const xmlch_t *_documentUri;

	// DOM state
	NsDomFactory *_domFactory;
	NsDomElement *_domDocNode;
	NodeModifications *_modifications;
	bool _addNsInfo;
	bool _fromParse;
	
	// DOM access only (parsed, or materialized)
	bool _domCreateEntityText;
	
	// DB state -- optionally requires NsDocumentDatabase
	// and DictionaryDatabase
	NsDocumentDatabase *_ndb;
	DictionaryDatabase *_ddb;
	mutable OperationContext _oc;
	u_int32_t _flags;
};

}

#endif
