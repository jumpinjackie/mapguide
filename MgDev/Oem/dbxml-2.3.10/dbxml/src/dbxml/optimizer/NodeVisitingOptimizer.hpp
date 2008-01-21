//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: NodeVisitingOptimizer.hpp,v 1.23 2006/10/30 17:45:59 bostic Exp $
//

#ifndef __NODEVISITINGOPTIMIZER_HPP
#define	__NODEVISITINGOPTIMIZER_HPP

#include "Optimizer.hpp"

#include <xqilla/simple-api/XQQuery.hpp>
#include <xqilla/ast/XQFunction.hpp>
#include <xqilla/ast/ASTNodeImpl.hpp>
#include <xqilla/ast/XQLiteral.hpp>
#include <xqilla/ast/XQNav.hpp>
#include <xqilla/ast/XQParenthesizedExpr.hpp>
#include <xqilla/ast/XQSequence.hpp>
#include <xqilla/ast/XQStep.hpp>
#include <xqilla/ast/XQVariable.hpp>
#include <xqilla/ast/XQIf.hpp>
#include <xqilla/ast/XQInstanceOf.hpp>
#include <xqilla/ast/XQCastableAs.hpp>
#include <xqilla/ast/XQCastAs.hpp>
#include <xqilla/ast/XQTreatAs.hpp>
#include <xqilla/ast/XQOperator.hpp>
#include <xqilla/ast/XQContextItem.hpp>
#include <xqilla/ast/XQDebugHook.hpp>
#include <xqilla/ast/XQDOMConstructor.hpp>
#include <xqilla/ast/XQFLWOR.hpp>
#include <xqilla/ast/XQVariableBinding.hpp>
#include <xqilla/ast/XQQuantified.hpp>
#include <xqilla/ast/XQFunctionCall.hpp>
#include <xqilla/ast/XQGlobalVariable.hpp>
#include <xqilla/ast/XQTypeswitch.hpp>
#include <xqilla/ast/XQValidate.hpp>
#include <xqilla/ast/XQOrderingChange.hpp>
#include <xqilla/ast/XQAtomize.hpp>
#include <xqilla/ast/XPath1Compat.hpp>
#include <xqilla/ast/ConvertFunctionArg.hpp>
#include <xqilla/ast/XQDocumentOrder.hpp>
#include <xqilla/ast/XQPredicate.hpp>
#include <xqilla/functions/XQUserFunction.hpp>

namespace DbXml
{

class DbXmlCompare;
class DbXmlFilter;
class DbXmlContains;
class DbXmlStep;
class DbXmlNav;
class LookupIndex;
class QueryPlanFunction;
class Join;

class NodeVisitingOptimizer : public Optimizer
{
public:
	NodeVisitingOptimizer(Optimizer *parent = 0)
		: Optimizer(parent) {}
	virtual ~NodeVisitingOptimizer() {}

protected:
	virtual void resetInternal() = 0;

	virtual void optimize(XQQuery *query);
	virtual ASTNode *optimize(ASTNode *item);
	virtual ASTNode *optimizeFunction(XQFunction *item);
	virtual ASTNode *optimizeLiteral(XQLiteral *item);
	virtual ASTNode *optimizeNav(XQNav *item);
	virtual ASTNode *optimizeParenthesizedExpr(XQParenthesizedExpr *item);
	virtual ASTNode *optimizeSequence(XQSequence *item);
	virtual ASTNode *optimizeStep(XQStep *item);
	virtual ASTNode *optimizeVariable(XQVariable *item);
	virtual ASTNode *optimizeIf(XQIf *item);
	virtual ASTNode *optimizeInstanceOf(XQInstanceOf *item);
	virtual ASTNode *optimizeCastableAs(XQCastableAs *item);
	virtual ASTNode *optimizeCastAs(XQCastAs *item);
	virtual ASTNode *optimizeTreatAs(XQTreatAs *item);
	virtual ASTNode *optimizeOperator(XQOperator *item);
	virtual ASTNode *optimizeContextItem(XQContextItem *item);
	virtual ASTNode *optimizeDebugHook(XQDebugHook *item);
	virtual ASTNode *optimizeFLWOR(XQFLWOR *item);
	virtual ASTNode *optimizeFLWORQuantified(XQQuantified *item);
	virtual ASTNode *optimizeTypeswitch(XQTypeswitch *item);
	virtual ASTNode *optimizeValidate(XQValidate *item);
	virtual XQGlobalVariable *optimizeGlobalVar(XQGlobalVariable *item);
	virtual ASTNode *optimizeFunctionCall(XQFunctionCall *item);
	virtual ASTNode *optimizeDOMConstructor(XQDOMConstructor *item);
	virtual ASTNode *optimizeOrderingChange(XQOrderingChange *item);
	virtual ASTNode *optimizeAtomize(XQAtomize *item);
	virtual ASTNode *optimizeXPath1CompatConvertFunctionArg(XPath1CompatConvertFunctionArg *item);
	virtual ASTNode *optimizePromoteUntyped(XQPromoteUntyped *item);
	virtual ASTNode *optimizePromoteNumeric(XQPromoteNumeric *item);
	virtual ASTNode *optimizePromoteAnyURI(XQPromoteAnyURI *item);
	virtual ASTNode *optimizeDocumentOrder(XQDocumentOrder *item);
	virtual ASTNode *optimizePredicate(XQPredicate *item);
	virtual ASTNode *optimizeUserFunction(XQUserFunction::XQFunctionEvaluator *item);
	virtual ASTNode *optimizeDbXmlCompare(DbXmlCompare *item);
	virtual ASTNode *optimizeDbXmlFilter(DbXmlFilter *item);
	virtual ASTNode *optimizeDbXmlContains(DbXmlContains *item);
	virtual ASTNode *optimizeLookupIndex(LookupIndex *item);
	virtual ASTNode *optimizeQueryPlanFunction(QueryPlanFunction *item);
	virtual ASTNode *optimizeDbXmlStep(DbXmlStep *item);
	virtual ASTNode *optimizeJoin(Join *item);
	virtual ASTNode *optimizeDbXmlNav(DbXmlNav *item);
};

}

#endif
