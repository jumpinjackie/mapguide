//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//      Sleepycat Software.  All rights reserved.
//
// $Id: NsDocument.hpp,v 1.23 2005/09/20 14:35:53 jsnelson Exp $
//

#ifndef __DBXMLNSDOCUMENT_HPP
#define __DBXMLNSDOCUMENT_HPP

#include "NsTypes.hpp"
#include "../OperationContext.hpp"
#include "../ID.hpp"
#include <db_cxx.h>
#include <set>
#include <map>

extern "C" {
int nsCompareNodes(DB *db, const DBT *dbt1, const DBT *dbt2);
};

namespace DbXml
{

// forwards
class NsNamespaceInfo;
class NsDomElement;
class NsDomObj;
class NsDomNode;
class NsDomFactory;
class NsDocumentDatabase;
class NsDocument;
class Document;

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
	const nid_t *getNID() const;
	const NsDomElement *getNode() const;

	void setToAdd(NsDomElement *n) const;
	void setToUpdate(NsDomElement *n) const;
	void setToRemove(const nid_t *n) const;
	void reset() const;

	bool operator<(const NodeModification &o) const;
private:
	NodeModification(const NodeModification &o);
	NodeModification &operator=(const NodeModification &o);
private:
	NsDocument &doc_;
	mutable Action action_;
	mutable union {
		nid_t nid_;
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
	void initDoc(Transaction *txn, NsDocumentDatabase *db, const ID &docId, u_int32_t flags, bool newDoc);
	// init for a pure DOM parse
	void initDOM(NsDomFactory *factory);
	// init for an index parse -- whole document
	void initIndex();

	void setDoNamespaces(bool doNamespace);
	bool getDoNamespaces() const { return _doNamespace; }
	void completeNode(nsNode_t *node);
	void completeDocument(void);
	void copyId(nid_t *to, const nid_t *from);
	void freeId(nid_t *id);

	//
	// accessors to namespace info (both utf-8 and utf-16)
	//
	int addUri(const void *uri, int32_t uriIndex,
		   bool isUTF8, bool isDonated, uint32_t len);
	int addPrefix(const void *prefix, int32_t uriIndex,
		      bool isUTF8, bool isDonated, uint32_t len);
	int32_t getUriIndexForPrefix(int32_t prefixIndex) const;
	const xmlbyte_t *getPrefix8(int32_t prefixIndex);
	const xmlbyte_t *getUri8(int32_t uriIndex);
	const xmlbyte_t *getUriForPrefix8(int32_t prefixIndex);

	const xmlch_t *getPrefix(int32_t prefixIndex);
	const xmlch_t *getUri(int32_t uriIndex);
	const xmlch_t *getUriForPrefix(int32_t prefixIndex);
	const xmlch_t *getQname(const nsName_t *name, bool isUTF16,
				bool &owned);
	const xmlch_t *getText(const nsText_t *text, bool isUTF16,
				bool &owned);
	//
	// access to document metadata (xml decl, encoding string, etc)
	//
	const xmlbyte_t *getXmlDecl8() const;
	const xmlch_t *getXmlDecl() const;
	void setXmlDecl(int32_t decl) {
		_xmlDecl = decl;
		_xmlDeclModified = true;
	}

	const xmlbyte_t *getSniffedEncodingStr() const { return _sniffedEncodingStr8; }
	void setSniffedEncodingStr(const xmlbyte_t *str);

	const xmlch_t *getEncodingStr() const;
	const xmlbyte_t *getEncodingStr8() const { return _encodingStr8; }
	void setEncodingStr(const xmlbyte_t *str);

	const xmlbyte_t *getStandaloneStr() const { return _standaloneStr; }
	void setStandalone(bool standalone);
	const bool getIsStandalone() const;
	
	XER_NS MemoryManager *getMemoryManager() { return _memManager; }

	//
	// DOM access
	//

	// Does not search the tree for it first
	NsDomElement *fetchDomElement(const nid_t *nid);

	NsDomElement *getDomElement(const nid_t *nid);
	NsDomElement *getDocumentNode();
	void setDocumentElement(NsDomElement *node) { _domDocNode = node; }
	void releaseDomElement(NsDomElement *domElement);
	NsDomFactory *getDomFactory(void) const { return _domFactory; }
	void setNsDomFactory(NsDomFactory *factory) { _domFactory = factory; }
	bool getCreateEntityText(void) const { return _domCreateEntityText; }
	void setCreateEntityText(bool val) { _domCreateEntityText = val; }

	//
	// DOM modification methods
	//
	void addToModifications(NodeModification::Action action, NsDomElement *node);
	void clearModifications();
	NodeModifications &getModifications()
	{
		return _modifications;
	}

	//
	// DB access
	//
	void updateSpecialNodes(OperationContext &oc);
	void putDocInfo(OperationContext &oc, bool hasNsNode);
	bool getDocInfo(void); // return true if there's a namepace node
	void putNamespaceInfo(OperationContext &oc, char *buf, int size);
	void getNamespaceInfo(void);
	NsDocumentDatabase *getDb(void) { return _db; }
	const ID &getDocId(void) const { return _docId; }
	Transaction *getTxn(void) const { return _oc.txn(); }
	bool hasContent() const { return _hasContent; }

#ifdef NS_DEBUG
	// Debug
	void printNsInfo(void);
	void dumpDocument(const nid_t *nid = 0);
	void _nsDumpNode(NsDomElement *dnode, const char *indent = "");
	void _domTraverse(NsDomNode *node);
#endif	
private:
	nsNode_t *_getNode(const nid_t *nid);
	int _putNodeRecord(nsNode_t *node);
private:
	// state for both parsing and access
	const Document *_ownerDoc;
	XER_NS MemoryManager *_memManager;
	ID _docId;
	bool _doNamespace;
	bool _hasNsNode;
	bool _hasContent;
	NsNamespaceInfo *_nsInfo;

	// xml decl info
	int32_t _xmlDecl;
	const xmlbyte_t *_sniffedEncodingStr8;
	const xmlbyte_t *_encodingStr8;
	mutable const xmlch_t *_encodingStr;
	const xmlbyte_t *_standaloneStr;
	bool _xmlDeclModified;

	// DOM state
	NsDomFactory *_domFactory;
	NsDomElement *_domDocNode;
	NodeModifications _modifications;

	// DOM access only (parsed, or materialized)
	bool _domCreateEntityText;
	
	// DB state
	NsDocumentDatabase *_db;
	mutable OperationContext _oc;
	u_int32_t _flags;
};

}

#endif
