//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: ASTRewriteOptimizer.hpp,v 1.3 2006/10/30 17:45:59 bostic Exp $
//

#ifndef __ASTREWRITEOPTIMIZER_HPP
#define	__ASTREWRITEOPTIMIZER_HPP

#include "NodeVisitingOptimizer.hpp"

#include <xqilla/operators/GeneralComp.hpp>

class DynamicContext;
class Intersect;

namespace DbXml
{

class DbXmlNav;
class DbXmlFunContains;
class QueryPlanHolder;
class DbXmlContext;

class ASTRewriteOptimizer : public NodeVisitingOptimizer
{
public:
	ASTRewriteOptimizer(DbXmlContext *xpc, Optimizer *parent = 0)
		: NodeVisitingOptimizer(parent),
		  xpc_(xpc)
	{}
	virtual ~ASTRewriteOptimizer() {}

protected:
	virtual void resetInternal() {}

	virtual void optimize(XQQuery *query) {
		NodeVisitingOptimizer::optimize(query);
	}
	virtual ASTNode *optimize(ASTNode *item);
	virtual ASTNode *optimizePredicate(XQPredicate *item);
	virtual ASTNode *optimizeOperator(XQOperator *item);
	virtual ASTNode *optimizeIf(XQIf *item);
	virtual ASTNode *optimizeNav(XQNav *item);
	virtual ASTNode *optimizeFLWOR(XQFLWOR *item);
	virtual ASTNode *optimizeFLWORQuantified(XQQuantified *item);
	virtual ASTNode *optimizeFunction(XQFunction *item);

private:
	ASTNode *createDbXmlCompare(XQOperator *item, GeneralComp::ComparisonOperation op, bool generalComp,
		QueryPlanHolder *arg0qph, QueryPlanHolder *arg1qph);
	ASTNode *createDbXmlContains(DbXmlFunContains *item);
	ASTNode *createDbXmlIntersect(Intersect *item);
	ASTNode *optimizeFLOWRToNav(VectorOfVariableBinding *bindings, XQFLWOR *item, bool quantified);

	typedef std::pair<ASTNode*, bool> ParentInfo;
	typedef std::vector<ParentInfo> AncestorStack;

	AncestorStack ancestors_;

	DbXmlContext *xpc_;
};

}

#endif
