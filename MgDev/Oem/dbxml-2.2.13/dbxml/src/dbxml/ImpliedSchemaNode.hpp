//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: ImpliedSchemaNode.hpp,v 1.7 2005/11/28 13:13:20 jsnelson Exp $
//

#ifndef __IMPLIEDSCHEMANODE_HPP
#define	__IMPLIEDSCHEMANODE_HPP

#include <vector>
#include <map>
#include <string>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/framework/MemoryManager.hpp>

class DataItem;

namespace DbXml
{

class QueryPlanFunction;

class ImpliedSchemaNode
{
public:
	typedef enum {
		ATTRIBUTE,
		CHILD,
		DESCENDANT,
		ROOT,
		METADATA,
		EQUALS,
		LTX,
		LTE,
		GTX,
		GTE,
		PREFIX,
		SUBSTRING
	} Type;

	typedef std::vector<ImpliedSchemaNode*> Vector;

	ImpliedSchemaNode(Type type, XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *mm);
	ImpliedSchemaNode(Type type, const DataItem *value, XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *mm);
	ImpliedSchemaNode(const XMLCh *uri, bool wildcardURI, const XMLCh *name, bool wildcardName,
			  bool wildcardNodeType, Type type, XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *mm);
	~ImpliedSchemaNode();

	const QueryPlanFunction *getQueryPlanFunction() const { return qpf_; }
	void setQueryPlanFunction(QueryPlanFunction *qpf) { qpf_ = qpf; }

	bool isWildcardURI() const;
	bool isWildcardName() const;
	bool isWildcardNodeType() const;

	bool isWildcard() const;

	const XMLCh *getURI() const;
	const XMLCh *getName() const;
	/// Lazily calculated
	const char *getUTF8URI() const;
	/// Lazily calculated
	const char *getUTF8Name() const;

	Type getType() const;
	void setType(Type t);

	const DataItem *getValue() const;
	void setValue(const DataItem *value);

	ImpliedSchemaNode *getParent();
	const ImpliedSchemaNode *getParent() const;
	ImpliedSchemaNode *getNextSibling();
	const ImpliedSchemaNode *getNextSibling() const;
	ImpliedSchemaNode *getPreviousSibling();
	const ImpliedSchemaNode *getPreviousSibling() const;
	ImpliedSchemaNode *getFirstChild();
	const ImpliedSchemaNode *getFirstChild() const;
	ImpliedSchemaNode *getLastChild();
	const ImpliedSchemaNode *getLastChild() const;

	ImpliedSchemaNode *getRoot();
	const ImpliedSchemaNode *getRoot() const;

	ImpliedSchemaNode *appendChild(ImpliedSchemaNode *childToAdopt);
	void removeChild(ImpliedSchemaNode *child, bool deleteChild = true);
	void stealChildren(ImpliedSchemaNode *victim);
	void markSubtree();

	std::string toString(int level = 0) const;
	std::string getPath() const;
	std::string getStepName() const;
	std::string getUriName() const;

	bool equals(const ImpliedSchemaNode *o) const;

	/// Returns true if the uri and name information would match the node
	bool matches(const ImpliedSchemaNode *node) const;

	/// Performs a deep copy of the node and it's children
	ImpliedSchemaNode *copy(XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *mm = 0) const;

private:
	ImpliedSchemaNode(const ImpliedSchemaNode &);
	ImpliedSchemaNode &operator=(const ImpliedSchemaNode &);

	const XMLCh *uri_, *name_;
	bool wildcardURI_, wildcardName_, wildcardNodeType_;
	Type type_;

	const DataItem *value_;

	char *utf8URI_, *utf8Name_;

	ImpliedSchemaNode *parent_;
	ImpliedSchemaNode *nextSibling_;
	ImpliedSchemaNode *prevSibling_;
	ImpliedSchemaNode *firstChild_;
	ImpliedSchemaNode *lastChild_;

	QueryPlanFunction *qpf_;

	XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *memMgr_;
};

}

#endif
