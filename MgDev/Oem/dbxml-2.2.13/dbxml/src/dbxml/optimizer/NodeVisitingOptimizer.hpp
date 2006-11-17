//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: NodeVisitingOptimizer.hpp,v 1.9 2005/08/11 12:26:36 jsnelson Exp $
//

#ifndef __NODEVISITINGOPTIMIZER_HPP
#define	__NODEVISITINGOPTIMIZER_HPP

#include "Optimizer.hpp"

#include <pathan/dataItem/DataItem.hpp>
#include <pathan/dataItem/DataItemFunction.hpp>
#include <pathan/dataItem/DataItemImpl.hpp>
#include <pathan/dataItem/DataItemLiteral.hpp>
#include <pathan/dataItem/DataItemNav.hpp>
#include <pathan/dataItem/DataItemParenthesizedExpr.hpp>
#include <pathan/dataItem/DataItemSequence.hpp>
#include <pathan/dataItem/DataItemStep.hpp>
#include <pathan/dataItem/DataItemVariable.hpp>
#include <pathan/dataItem/DataItemIf.hpp>
#include <pathan/dataItem/DataItemFor.hpp>
#include <pathan/dataItem/DataItemQuantifiedExpr.hpp>
#include <pathan/dataItem/DataItemInstanceOf.hpp>
#include <pathan/dataItem/DataItemCastableAs.hpp>
#include <pathan/dataItem/DataItemCastAs.hpp>
#include <pathan/dataItem/DataItemTreatAs.hpp>
#include <pathan/dataItem/DataItemOperator.hpp>
#include <pathan/dataItem/DataItemContextItem.hpp>

#include <xquery/XQQuery.hpp>
#include <xquery/dataItem/XQDebugHook.hpp>
#include <xquery/dataItem/XQDOMConstructor.hpp>
#include <xquery/dataItem/XQFLWOR.hpp>
#include <xquery/dataItem/XQVariableBinding.hpp>
#include <xquery/dataItem/XQQuantified.hpp>
#include <xquery/dataItem/XQFunctionCall.hpp>
#include <xquery/dataItem/XQGlobalVariable.hpp>
#include <xquery/dataItem/XQTypeswitch.hpp>
#include <xquery/dataItem/XQValidate.hpp>
#include <xquery/dataItem/XQOrderingChange.hpp>
#include <xquery/functions/XQFunction.hpp>

namespace DbXml
{

class DbXmlCompare;
class DbXmlFilter;
class DbXmlContains;

class NodeVisitingOptimizer : public Optimizer
{
public:
	NodeVisitingOptimizer(Optimizer *parent = 0)
		: Optimizer(parent) {}
	virtual ~NodeVisitingOptimizer() {}

protected:
	virtual void resetInternal() = 0;

	virtual void optimize(XQQuery *query);
	virtual DataItem *optimize(DataItem *item);
	virtual DataItem *optimizeFunction(DataItemFunction *item);
	virtual DataItem *optimizeLiteral(DataItemLiteral *item);
	virtual DataItem *optimizeNav(DataItemNav *item);
	virtual DataItem *optimizeParenthesizedExpr(DataItemParenthesizedExpr *item);
	virtual DataItem *optimizeSequence(DataItemSequence *item);
	virtual DataItem *optimizeStep(DataItemStep *item);
	virtual DataItem *optimizeVariable(DataItemVariable *item);
	virtual DataItem *optimizeIf(DataItemIf *item);
	virtual DataItem *optimizeFor(DataItemFor *item);
	virtual DataItem *optimizeQuantifiedExpr(DataItemQuantifiedExpr *item);
	virtual DataItem *optimizeInstanceOf(DataItemInstanceOf *item);
	virtual DataItem *optimizeCastableAs(DataItemCastableAs *item);
	virtual DataItem *optimizeCastAs(DataItemCastAs *item);
	virtual DataItem *optimizeTreatAs(DataItemTreatAs *item);
	virtual DataItem *optimizeOperator(DataItemOperator *item);
	virtual DataItem *optimizeContextItem(DataItemContextItem *item);
	virtual DataItem *optimizeDebugHook(XQDebugHook *item);
	virtual DataItem *optimizeFLWOR(XQFLWOR *item);
	virtual DataItem *optimizeFLWORQuantified(XQQuantified *item);
	virtual DataItem *optimizeTypeswitch(XQTypeswitch *item);
	virtual DataItem *optimizeValidate(XQValidate *item);
	virtual XQGlobalVariable *optimizeGlobalVar(XQGlobalVariable *item);
	virtual DataItem *optimizeFunctionCall(XQFunctionCall *item);
	virtual DataItem *optimizeDOMConstructor(XQDOMConstructor *item);
	virtual DataItem *optimizeOrderingChange(XQOrderingChange *item);
	virtual DataItem *optimizeUserFunction(XQFunction::XQFunctionEvaluator *item);
	virtual DataItem *optimizeDbXmlCompare(DbXmlCompare *item);
	virtual DataItem *optimizeDbXmlFilter(DbXmlFilter *item);
	virtual DataItem *optimizeDbXmlContains(DbXmlContains *item);

	virtual void optimizePredicates(DataItemImpl *item);
	virtual DataItem *optimizePredicate(DataItem *item);
};

}

#endif
