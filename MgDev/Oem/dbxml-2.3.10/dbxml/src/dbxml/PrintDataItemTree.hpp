//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: PrintDataItemTree.hpp,v 1.28 2006/10/30 17:45:53 bostic Exp $
//

#ifndef __DBXMLPRINTDATAITEMTREE_HPP
#define	__DBXMLPRINTDATAITEMTREE_HPP

#include <iostream>

#include <xqilla/utils/PrintAST.hpp>

#include "optimizer/Optimizer.hpp"

namespace DbXml
{

class OutputTree;
class QueryPlanFunction;
class DbXmlDocAvailable;
class QueryPlan;
class DbXmlCompare;
class DbXmlFilter;
class DbXmlContains;
class LookupIndex;
class DbXmlStep;
class DbXmlNav;
class Join;

class DbXmlPrintXQTree : public PrintAST
{
public:
	static std::string print(const XQillaExpression *expr, const DynamicContext *context, int indent = 0);
	static std::string print(const XQQuery *query, const DynamicContext *context, int indent = 0);
	static std::string print(const ASTNode *item, const DynamicContext *context, int indent = 0);


	virtual std::string printASTNode(const ASTNode *item, const DynamicContext *context, int indent);
	virtual std::string printFunction(const XQFunction *item, const DynamicContext *context, int indent);
	virtual std::string printQueryPlanFunction(const QueryPlanFunction *item, const DynamicContext *context,
		int indent);
	virtual std::string printDbXmlDocAvailable(const DbXmlDocAvailable *item, const DynamicContext *context,
		int indent);
	virtual std::string printDbXmlStep(const DbXmlStep *item, const DynamicContext *context, int indent);
	virtual std::string printDbXmlCompare(const DbXmlCompare *item, const DynamicContext *context, int indent);
	virtual std::string printDbXmlFilter(const DbXmlFilter *item, const DynamicContext *context, int indent);
	virtual std::string printDbXmlContains(const DbXmlContains *item, const DynamicContext *context, int indent);
        virtual std::string printLookupIndex(const LookupIndex *item, const DynamicContext *context, int indent);
        virtual std::string printJoin(const Join *item, const DynamicContext *context, int indent);
	virtual std::string printDbXmlNav(const DbXmlNav *item, const DynamicContext *context, int indent);

	virtual std::string printQueryPlan(const QueryPlan *qp, bool isExecutable, int indent);

	static std::string printNodeTestAttrs(const NodeTest *step);
};

class OutputTree : public Optimizer
{
public:
	OutputTree(std::string label, const DynamicContext *context, Optimizer *parent = 0)
		: Optimizer(parent), label_(label), context_(context) {}
	virtual ~OutputTree() {}
protected:
	virtual void resetInternal() {}
	virtual void optimize(XQQuery *query)
	{
		std::cerr << label_ << ":" << std::endl;
		std::cerr << DbXmlPrintXQTree::print(query, context_) << std::endl;
	}
	virtual ASTNode *optimize(ASTNode *item)
	{
		std::cerr << label_ << ":" << std::endl;
		std::cerr << DbXmlPrintXQTree::print(item, context_) << std::endl;
		return item;
	}
private:
	std::string label_;
	const DynamicContext *context_;
};

}

#endif
