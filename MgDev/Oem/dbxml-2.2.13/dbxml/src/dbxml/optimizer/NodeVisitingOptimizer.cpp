//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: NodeVisitingOptimizer.cpp,v 1.13 2005/09/06 10:42:12 jsnelson Exp $
//

#include "DbXmlInternal.hpp"
#include <iostream>
#include "UTF8.hpp"

#include "NodeVisitingOptimizer.hpp"
#include "dataItem/DbXmlCompare.hpp"
#include "dataItem/DbXmlFilter.hpp"
#include "dataItem/DbXmlContext.hpp"
#include "dataItem/DbXmlNav.hpp"
#include "dataItem/DbXmlContains.hpp"

#include <xquery/XQContext.hpp>

using namespace DbXml;
using namespace std;

void NodeVisitingOptimizer::optimize(XQQuery *query)
{
	for(vector<XQGlobalVariable*, PathanAllocator<XQGlobalVariable*> >::iterator it = query->m_userDefVars.begin();
	    it != query->m_userDefVars.end(); ++it) {
		(*it) = optimizeGlobalVar(*it);
	}
	for(vector<XQFunction*, PathanAllocator<XQFunction*> >::iterator i = query->m_userDefFns.begin();
	    i != query->m_userDefFns.end(); ++i) {
		(*i)->setFunctionBody(optimize(const_cast<DataItem *>((*i)->getFunctionBody())));
	}
	query->setQueryBody(optimize(query->getQueryBody()));
}

DataItem *NodeVisitingOptimizer::optimize(DataItem *item)
{
	DataItem *result = item;
	switch(item->getType()) {
	case DataItem::LITERAL: {
		result = optimizeLiteral((DataItemLiteral *)item);
		break;
	}
	case DataItem::SEQUENCE: {
		result = optimizeSequence((DataItemSequence *)item);
		break;
	}
	case DataItem::FUNCTION: {
		result = optimizeFunction((DataItemFunction *)item);
		break;
	}
	case DataItem::NAVIGATION: {
		result = optimizeNav((DataItemNav *)item);
		break;
	}
	case DataItem::VARIABLE: {
		result = optimizeVariable((DataItemVariable *)item);
		break;
	}
	case DataItem::STEP: {
		result = optimizeStep((DataItemStep *)item);
		break;
	}
	case DataItem::IF: {
		result = optimizeIf((DataItemIf *)item);
		break;
	}
	case DataItem::INSTANCE_OF: {
		result = optimizeInstanceOf((DataItemInstanceOf *)item);
		break;
	}
	case DataItem::CASTABLE_AS: {
		result = optimizeCastableAs((DataItemCastableAs *)item);
		break;
	}
	case DataItem::CAST_AS: {
		result = optimizeCastAs((DataItemCastAs *)item);
		break;
	}
	case DataItem::TREAT_AS: {
		result = optimizeTreatAs((DataItemTreatAs *)item);
		break;
	}
	case DataItem::PARENTHESIZED: {
		result = optimizeParenthesizedExpr((DataItemParenthesizedExpr *)item);
		break;
	}
	case DataItem::FOR: {
		result = optimizeFor((DataItemFor *)item);
		break;
	}
	case DataItem::QUANTIFIED: {
		result = optimizeQuantifiedExpr((DataItemQuantifiedExpr *)item);
		break;
	}
	case DataItem::OPERATOR: {
		result = optimizeOperator((DataItemOperator *)item);
		break;
	}
	case DataItem::CONTEXT_ITEM: {
		result = optimizeContextItem((DataItemContextItem *)item);
		break;
	}
	case ((DataItem::whichType)XQContext::DEBUG_HOOK): {
		result = optimizeDebugHook((XQDebugHook *)item);
		break;
	}
	case ((DataItem::whichType)XQContext::FLWOR): {
		result = optimizeFLWOR((XQFLWOR *)item);
		break;
	}
	case ((DataItem::whichType)XQContext::FLWOR_QUANTIFIED): {
		result = optimizeFLWORQuantified((XQQuantified *)item);
		break;
	}
	case ((DataItem::whichType)XQContext::TYPESWITCH): {
		result = optimizeTypeswitch((XQTypeswitch *)item);
		break;
	}
	case ((DataItem::whichType)XQContext::VALIDATE): {
		result = optimizeValidate((XQValidate *)item);
		break;
	}
	case ((DataItem::whichType)XQContext::VARIABLE_DEFINITION): {
		result = optimizeGlobalVar((XQGlobalVariable *)item);
		break;
	}
	case ((DataItem::whichType)XQContext::FUNCTION_CALL): {
		result = optimizeFunctionCall((XQFunctionCall *)item);
		break;
	}
	case ((DataItem::whichType)XQContext::DOM_CONSTRUCTOR): {
		result = optimizeDOMConstructor((XQDOMConstructor *)item);
		break;
	}
	case ((DataItem::whichType)XQContext::ORDERING_CHANGE): {
		result = optimizeOrderingChange((XQOrderingChange *)item);
		break;
	}
	case ((DataItem::whichType)XQContext::USER_FUNCTION): {
		result = optimizeUserFunction((XQFunction::XQFunctionEvaluator *)item);
		break;
	}
	case ((DataItem::whichType)DbXmlContext::DBXML_COMPARE): {
		result = optimizeDbXmlCompare((DbXmlCompare*)item);
		break;
	}
	case ((DataItem::whichType)DbXmlContext::DBXML_FILTER): {
		result = optimizeDbXmlFilter((DbXmlFilter*)item);
		break;
	}
	case ((DataItem::whichType)DbXmlContext::DBXML_CONTAINS): {
		result = optimizeDbXmlContains((DbXmlContains*)item);
		break;
	}
	default: break;
	}
	return result;
}

DataItem *NodeVisitingOptimizer::optimizeFunction(DataItemFunction *item)
{
	VectorOfDataItems &args = const_cast<VectorOfDataItems &>(item->getArguments());
	for(VectorOfDataItems::iterator i = args.begin(); i != args.end(); ++i) {
		*i = optimize(*i);
	}
	optimizePredicates(item);
	return item;
}

DataItem *NodeVisitingOptimizer::optimizeContextItem(DataItemContextItem *item)
{
	optimizePredicates(item);
	return item;
}

DataItem *NodeVisitingOptimizer::optimizeLiteral(DataItemLiteral *item)
{
	optimizePredicates(item);
	return item;
}

DataItem *NodeVisitingOptimizer::optimizeNav(DataItemNav *item)
{
	DataItemNav::Steps &args = const_cast<DataItemNav::Steps &>(item->getSteps());
	for(DataItemNav::Steps::iterator i = args.begin(); i != args.end(); ++i) {
		i->step = optimize(i->step);
	}
	optimizePredicates(item);
	return item;
}

DataItem *NodeVisitingOptimizer::optimizeParenthesizedExpr(DataItemParenthesizedExpr *item)
{
	VectorOfDataItems &args = const_cast<VectorOfDataItems &>(item->getChildren());
	for(VectorOfDataItems::iterator i = args.begin(); i != args.end(); ++i) {
		*i = optimize(*i);
	}
	optimizePredicates(item);
	return item;
}

DataItem *NodeVisitingOptimizer::optimizeSequence(DataItemSequence *item)
{
	optimizePredicates(item);
	return item;
}

DataItem *NodeVisitingOptimizer::optimizeStep(DataItemStep *item)
{
	optimizePredicates(item);
	return item;
}

DataItem *NodeVisitingOptimizer::optimizeDbXmlCompare(DbXmlCompare *item)
{
	item->setArgument(optimize(const_cast<DataItem *>(item->getArgument())));
	optimizePredicates(item);
	return item;
}

DataItem *NodeVisitingOptimizer::optimizeDbXmlFilter(DbXmlFilter *item)
{
	item->setFilterExpr((DbXmlNav*)optimize(const_cast<DbXmlNav *>(item->getFilterExpr())));
	optimizePredicates(item);
	return item;
}

DataItem *NodeVisitingOptimizer::optimizeDbXmlContains(DbXmlContains *item)
{
	item->setArgument(optimize(const_cast<DataItem *>(item->getArgument())));
	optimizePredicates(item);
	return item;
}

DataItem *NodeVisitingOptimizer::optimizeVariable(DataItemVariable *item)
{
	optimizePredicates(item);
	return item;
}

DataItem *NodeVisitingOptimizer::optimizeIf(DataItemIf *item)
{
	item->setTest(optimize(const_cast<DataItem *>(item->getTest())));
	item->setWhenTrue(optimize(const_cast<DataItem *>(item->getWhenTrue())));
	item->setWhenFalse(optimize(const_cast<DataItem *>(item->getWhenFalse())));
	optimizePredicates(item);
	return item;
}

DataItem *NodeVisitingOptimizer::optimizeFor(DataItemFor *item)
{
	DataItemFor::Bindings &bindings = const_cast<DataItemFor::Bindings &>(item->getBindings());
	for(DataItemFor::Bindings::iterator i = bindings.begin(); i != bindings.end(); ++i) {
		(*i)->_allValues = optimize((*i)->_allValues);
	}
	item->setReturnExpr(optimize(const_cast<DataItem *>(item->getReturnExpr())));
	optimizePredicates(item);
	return item;
}

DataItem *NodeVisitingOptimizer::optimizeQuantifiedExpr(DataItemQuantifiedExpr *item)
{
	DataItemFor::Bindings &bindings = const_cast<DataItemFor::Bindings &>(item->getBindings());
	for(DataItemFor::Bindings::iterator i = bindings.begin(); i != bindings.end(); ++i) {
		(*i)->_allValues = optimize((*i)->_allValues);
	}
	item->setReturnExpr(optimize(const_cast<DataItem *>(item->getReturnExpr())));
	optimizePredicates(item);
	return item;
}

DataItem *NodeVisitingOptimizer::optimizeInstanceOf(DataItemInstanceOf *item)
{
	item->setExpression(optimize(const_cast<DataItem *>(item->getExpression())));
	optimizePredicates(item);
	return item;
}

DataItem *NodeVisitingOptimizer::optimizeCastableAs(DataItemCastableAs *item)
{
	item->setExpression(optimize(const_cast<DataItem *>(item->getExpression())));
	optimizePredicates(item);
	return item;
}

DataItem *NodeVisitingOptimizer::optimizeCastAs(DataItemCastAs *item)
{
	item->setExpression(optimize(const_cast<DataItem *>(item->getExpression())));
	optimizePredicates(item);
	return item;
}

DataItem *NodeVisitingOptimizer::optimizeTreatAs(DataItemTreatAs *item)
{
	item->setExpression(optimize(const_cast<DataItem *>(item->getExpression())));
	optimizePredicates(item);
	return item;
}

DataItem *NodeVisitingOptimizer::optimizeOperator(DataItemOperator *item)
{
	VectorOfDataItems &args = const_cast<VectorOfDataItems &>(item->getArguments());
	for(VectorOfDataItems::iterator i = args.begin(); i != args.end(); ++i) {
		*i = optimize(*i);
	}
	optimizePredicates(item);
	return item;
}

void NodeVisitingOptimizer::optimizePredicates(DataItemImpl *item)
{
	DataItem::Predicates &preds = const_cast<DataItem::Predicates &>(item->getPredicates());

	if(!preds.empty()) {
		for(DataItem::Predicates::iterator i = preds.begin(); i != preds.end(); ++i) {
			i->pred = optimizePredicate(i->pred);
		}
	}
}

DataItem *NodeVisitingOptimizer::optimizePredicate(DataItem *item)
{
	return optimize(item);
}

DataItem *NodeVisitingOptimizer::optimizeDebugHook(XQDebugHook *item)
{
	item->m_impl = optimize(item->m_impl);
	optimizePredicates(item);
	return item;
}

DataItem *NodeVisitingOptimizer::optimizeFLWOR(XQFLWOR *item)
{
	// bindings
	VectorOfVariableBinding *bindings = const_cast<VectorOfVariableBinding *>(item->getBindings());
	for(VectorOfVariableBinding::iterator i = bindings->begin(); i != bindings->end(); ++i) {
		(*i)->_allValues = optimize((*i)->_allValues);
		if((*i)->_where) {
			(*i)->_where = optimize((*i)->_where);
		}
	}

	// sort specs
	const XQSort *sort = item->getSort();
	if(sort) {
		XQSort::VectorOfSortSpec *specs = const_cast<XQSort::VectorOfSortSpec *>(sort->getSortSpecs());
		for(XQSort::VectorOfSortSpec::iterator j = specs->begin(); j != specs->end(); ++j) {
			(*j)->setExpression(optimize(const_cast<DataItem *>((*j)->getExpression())));
		}
	}

	// where and return
	if(item->getWhereExpr()) {
		item->setWhereExpr(optimize(const_cast<DataItem *>(item->getWhereExpr())));
	}
	item->setReturnExpr(optimize(const_cast<DataItem *>(item->getReturnExpr())));

	optimizePredicates(item);
	return item;
}

DataItem *NodeVisitingOptimizer::optimizeFLWORQuantified(XQQuantified *item)
{
	// bindings
	VectorOfVariableBinding *bindings = const_cast<VectorOfVariableBinding *>(item->getBindings());
	for(VectorOfVariableBinding::iterator i = bindings->begin(); i != bindings->end(); ++i) {
		(*i)->_allValues = optimize((*i)->_allValues);
	}

	// no where or sort specs

	// return
	item->setReturnExpr(optimize(const_cast<DataItem *>(item->getReturnExpr())));

	optimizePredicates(item);
	return item;
}

DataItem *NodeVisitingOptimizer::optimizeTypeswitch(XQTypeswitch *item)
{
	item->setExpression(optimize(const_cast<DataItem *>(item->getExpression())));

	XQTypeswitch::VectorOfClause *clauses = const_cast<XQTypeswitch::VectorOfClause *>(item->getClauses());
	for(XQTypeswitch::VectorOfClause::iterator i = clauses->begin(); i != clauses->end(); ++i) {
		(*i)->_expr = optimize((*i)->_expr);
	}

	const_cast<XQTypeswitch::Clause *>(item->getDefaultClause())->_expr =
		optimize(const_cast<DataItem *>(item->getDefaultClause()->_expr));

	optimizePredicates(item);
	return item;
}

DataItem *NodeVisitingOptimizer::optimizeValidate(XQValidate *item)
{
	item->setExpression(optimize(const_cast<DataItem *>(item->getExpression())));

	optimizePredicates(item);
	return item;
}

XQGlobalVariable *NodeVisitingOptimizer::optimizeGlobalVar(XQGlobalVariable *item)
{
	if(item->getVariableExpr()) {
		item->setVariableExpr(optimize(const_cast<DataItem *>(item->getVariableExpr())));
	}
	optimizePredicates(item);
	return item;
}

DataItem *NodeVisitingOptimizer::optimizeFunctionCall(XQFunctionCall *item)
{
	VectorOfDataItems &args = const_cast<VectorOfDataItems &>(item->getArguments());
	for(VectorOfDataItems::iterator i = args.begin(); i != args.end(); ++i) {
		*i = optimize(*i);
	}
	optimizePredicates(item);
	return item;
}

DataItem *NodeVisitingOptimizer::optimizeUserFunction(XQFunction::XQFunctionEvaluator *item)
{
	VectorOfDataItems &args = const_cast<VectorOfDataItems &>(item->getArguments());
	for(VectorOfDataItems::iterator i = args.begin(); i != args.end(); ++i) {
		*i = optimize(*i);
	}
	optimizePredicates(item);
	return item;
}

DataItem *NodeVisitingOptimizer::optimizeDOMConstructor(XQDOMConstructor *item)
{
	if(item->getName()) {
		item->setName(optimize(const_cast<DataItem *>(item->getName())));
	}
	VectorOfDataItems *attrs = const_cast<VectorOfDataItems *>(item->getAttributes());
	if(attrs) {
		for(VectorOfDataItems::iterator i = attrs->begin(); i != attrs->end(); ++i) {
			*i = optimize(*i);
		}
	}

	VectorOfDataItems *children = const_cast<VectorOfDataItems *>(item->getChildren());
	for(VectorOfDataItems::iterator j = children->begin(); j != children->end(); ++j) {
		*j = optimize(*j);
	}

	optimizePredicates(item);
	return item;
}

DataItem *NodeVisitingOptimizer::optimizeOrderingChange(XQOrderingChange *item)
{
	item->setExpr(optimize(item->getExpr()));
	optimizePredicates(item);
	return item;
}
