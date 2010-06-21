//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: DbXmlContains.hpp,v 1.12 2006/11/21 18:49:43 jsnelson Exp $
//

#ifndef __DBXMLCONTAINS_HPP
#define	__DBXMLCONTAINS_HPP

#include "QueryPlanHolder.hpp"
#include "QueryPlanResultImpl.hpp"
#include "DbXmlASTNode.hpp"
#include "Join.hpp"

#include <xqilla/ast/ASTNodeImpl.hpp>
#include <xqilla/functions/FunctionContains.hpp>

namespace DbXml
{

class DbXmlContains : public DbXmlASTNode, public QueryPlanHolder
{
public:
	class ContainsResult : public ResultImpl
	{
	public:
		ContainsResult(const Result &parent, const DbXmlContains *contains);
		Item::Ptr next(DynamicContext *context);
		void skip();
		std::string asString(DynamicContext *context, int indent) const;
	private:
		Result parent_;
		const DbXmlContains *contains_;
	};

	DbXmlContains(ASTNode *arg, XPath2MemoryManager* memMgr);
	DbXmlContains(Join::Type join, const DbXmlContains *cnt, XPath2MemoryManager* memMgr);
	DbXmlContains(Join::Type join, DbXmlNodeTest *nodeTest, const DbXmlContains *cnt,
		XPath2MemoryManager* memMgr);

	virtual ASTNode *staticTyping(StaticContext *context);
	virtual DbXmlResult createResult(const DbXmlResult &contextItems, unsigned int props,
		DynamicContext* context) const;

	Join::Type getJoinType() const { return joinType_; }
	const DbXmlNodeTest *getNodeTest() const { return nodeTest_; }

	const ASTNode *getArgument() const { return arg_; }
	void setArgument(ASTNode *a) { arg_ = a; }
	
private:
	class QueryPlanContainsResult : public QueryPlanResultImpl
	{
	public:
		QueryPlanContainsResult(const DbXmlResult &parent, const DbXmlContains *contains, bool useQP);
		void setResult(const DbXmlNodeImpl *node, DynamicContext *context);
		std::string asString(DynamicContext *context, int indent) const;
	private:
		const DbXmlContains *contains_;
	};

	Join::Type joinType_;
	DbXmlNodeTest *nodeTest_;
	ASTNode *arg_;
};

class DbXmlFunContains : public FunctionContains
{
public:
	DbXmlFunContains(FunctionContains *comp, XPath2MemoryManager *mm);

	QueryPlanHolder &getArg0QPH() { return arg0qph_; }

private:
	QueryPlanHolder arg0qph_;
};

}

#endif
