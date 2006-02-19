//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: PrintDataItemTree.hpp,v 1.15 2005/08/16 11:56:28 jsnelson Exp $
//

#ifndef __DBXMLPRINTDATAITEMTREE_HPP
#define	__DBXMLPRINTDATAITEMTREE_HPP

#include <iostream>

#include <xquery/utils/PrintDataItemTree.hpp>

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

class DbXmlPrintDataItemTree : public XQueryPrintDataItemTree
{
public:
	static std::string print(const PathanExpression *expr, const DynamicContext *context, int indent = 0);
	static std::string print(const XQQuery *query, const DynamicContext *context, int indent = 0);
	static std::string print(const DataItem *item, const DynamicContext *context, int indent = 0);

	virtual std::string printDataItem(const DataItem *item, const DynamicContext *context, int indent);
	virtual std::string printFunction(const DataItemFunction *item, const DynamicContext *context, int indent);
	virtual std::string printQueryPlanFunction(const QueryPlanFunction *item, const DynamicContext *context, int indent);
	virtual std::string printDbXmlDocAvailable(const DbXmlDocAvailable *item, const DynamicContext *context, int indent);
	virtual std::string printStep(const DataItemStep *item, const DynamicContext *context, int indent);
	virtual std::string printDbXmlCompare(const DbXmlCompare *item, const DynamicContext *context, int indent);
	virtual std::string printDbXmlFilter(const DbXmlFilter *item, const DynamicContext *context, int indent);
	virtual std::string printDbXmlContains(const DbXmlContains *item, const DynamicContext *context, int indent);

	virtual std::string printQueryPlan(const QueryPlan *qp, bool isExecutable, int indent);
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
		std::cerr << DbXmlPrintDataItemTree::print(query, context_) << std::endl;
	}
	virtual DataItem *optimize(DataItem *item)
	{
		std::cerr << label_ << ":" << std::endl;
		std::cerr << DbXmlPrintDataItemTree::print(item, context_) << std::endl;
		return item;
	}
private:
	std::string label_;
	const DynamicContext *context_;
};

}

#endif
