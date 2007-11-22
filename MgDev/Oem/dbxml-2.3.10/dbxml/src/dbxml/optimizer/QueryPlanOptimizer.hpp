//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: QueryPlanOptimizer.hpp,v 1.2 2006/10/30 17:45:59 bostic Exp $
//

#ifndef __QUERYPLANOPTIMIZER_HPP
#define	__QUERYPLANOPTIMIZER_HPP

#include "NodeVisitingOptimizer.hpp"

namespace DbXml
{

class DbXmlContext;
class XmlManager;
class Transaction;

class QueryPlanOptimizer : public NodeVisitingOptimizer
{
public:
	QueryPlanOptimizer(XmlManager &mgr, Transaction *txn, DbXmlContext *xpc,
		Optimizer *parent = 0)
		: NodeVisitingOptimizer(parent), mgr_(mgr), txn_(txn), xpc_(xpc) {}

protected:
	virtual void resetInternal() {}

	virtual ASTNode *optimizeQueryPlanFunction(QueryPlanFunction *item);
	virtual ASTNode *optimizeFunction(XQFunction *item);
	virtual ASTNode *optimizeOperator(XQOperator *item);
	virtual ASTNode *optimizeDbXmlStep(DbXmlStep *item);
	virtual ASTNode *optimizeDbXmlCompare(DbXmlCompare *item);
	virtual ASTNode *optimizeDbXmlContains(DbXmlContains *item);

private:
	XmlManager &mgr_;
	Transaction *txn_;
	DbXmlContext *xpc_;
};

}

#endif
