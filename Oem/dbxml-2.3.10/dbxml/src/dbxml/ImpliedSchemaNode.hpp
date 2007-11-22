//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: ImpliedSchemaNode.hpp,v 1.15 2006/10/30 17:45:52 bostic Exp $
//

#ifndef __IMPLIEDSCHEMANODE_HPP
#define	__IMPLIEDSCHEMANODE_HPP

#include <vector>
#include <map>
#include <string>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/framework/MemoryManager.hpp>

#include "Syntax.hpp"

class ASTNode;

namespace DbXml
{

class QueryPlanHolder;

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
	ImpliedSchemaNode(Type type, bool generalComp, Syntax::Type syntaxType, const ASTNode *value,
		XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *mm);
	ImpliedSchemaNode(const XMLCh *uri, bool wildcardURI, const XMLCh *name, bool wildcardName,
			  bool wildcardNodeType, Type type, XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *mm);
	~ImpliedSchemaNode();

	const QueryPlanHolder *getQueryPlanHolder() const { return qph_; }
	void setQueryPlanHolder(QueryPlanHolder *qph) { qph_ = qph; }

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

	bool getGeneralComp() const { return generalComp_; }
	void setGeneralComp(bool v) { generalComp_ = v; }
	Syntax::Type getSyntaxType() const { return syntaxType_; }
	void setSyntaxType(Syntax::Type st) { syntaxType_ = st; }
	const ASTNode *getASTNode() const { return astnode_; }
	void setASTNode(const ASTNode *value) { astnode_ = value; }

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

	ImpliedSchemaNode *getBaseNode();

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

	bool generalComp_;
	Syntax::Type syntaxType_;
	const ASTNode *astnode_;

	char *utf8URI_, *utf8Name_;

	ImpliedSchemaNode *parent_;
	ImpliedSchemaNode *nextSibling_;
	ImpliedSchemaNode *prevSibling_;
	ImpliedSchemaNode *firstChild_;
	ImpliedSchemaNode *lastChild_;

	QueryPlanHolder *qph_;

	XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *memMgr_;
};

}

#endif
