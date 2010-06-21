//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: CostBasedOptimizer.hpp,v 1.12 2006/10/30 17:45:59 bostic Exp $
//

#ifndef __COSTBASEDOPTIMIZER_HPP
#define	__COSTBASEDOPTIMIZER_HPP

#include "NodeVisitingOptimizer.hpp"
#include "../dataItem/Join.hpp"
#include "../dataItem/DbXmlNav.hpp"

namespace DbXml
{

class DbXmlContext;
class DbXmlNav;

class CostBasedOptimizer : public NodeVisitingOptimizer
{
public:
	CostBasedOptimizer(XmlManager &mgr, Transaction *txn, DbXmlContext *xpc, Optimizer *parent = 0)
		: NodeVisitingOptimizer(parent), mgr_(mgr), txn_(txn), xpc_(xpc) {}

protected:
	virtual void resetInternal() {}

	virtual ASTNode *optimizeDbXmlNav(DbXmlNav *item);
	virtual ASTNode *optimizeDbXmlFilter(DbXmlFilter *item);

private:
	void compressSteps(DbXmlNav *nav);
	DbXmlNav::Steps::reverse_iterator findBestIndex(DbXmlNav::Steps::reverse_iterator start,
		DbXmlNav::Steps::reverse_iterator end, bool findJoin);

	bool isASTNodeReversible(ASTNode *ast);
	bool reverseASTNode(ASTNode *ast, Join::Type &axis, LookupIndex *index, DbXmlNav *&reverse);
	bool reverseDbXmlStep(DbXmlStep *step, Join::Type &axis, DbXmlNav *&reverse);
	bool reverseVariable(XQVariable *var, Join::Type &axis, DbXmlNav *&reverse);
	bool reverseDbXmlCompare(DbXmlCompare *compare, Join::Type &axis, DbXmlNav *&reverse);
	bool reverseDbXmlContains(DbXmlContains *contains, Join::Type &axis, DbXmlNav *&reverse);
	bool reverseDbXmlFilter(DbXmlFilter *filter, Join::Type &axis, DbXmlNav *&reverse);
	bool reverseJoin(Join *join, Join::Type &axis, DbXmlNav *&reverse);
	bool reverseLookupIndex(LookupIndex *index, Join::Type &axis, DbXmlNav *&reverse);
	bool reverseQueryPlanFunction(QueryPlanFunction *qpf, Join::Type &axis, LookupIndex *index,
		DbXmlNav *&reverse);

	bool addQPFToLookup(QueryPlanFunction *qpf, LookupIndex *index);
	
	XmlManager &mgr_;
	Transaction *txn_;
	DbXmlContext *xpc_;
};

class DocOrderOptimizer : public NodeVisitingOptimizer
{
public:
	DocOrderOptimizer(DbXmlContext *xpc, Optimizer *parent = 0)
		: NodeVisitingOptimizer(parent), xpc_(xpc) {}

protected:
	virtual void resetInternal() {}

	virtual ASTNode *optimizeDbXmlNav(DbXmlNav *item);
	virtual ASTNode *optimizeDbXmlStep(DbXmlStep *item);

private:
	DbXmlContext *xpc_;
};

}

#endif
