//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: NodeVisitingOptimizer.cpp,v 1.30.2.1 2007/01/03 19:21:13 jsnelson Exp $
//

#include "DbXmlInternal.hpp"
#include <iostream>
#include "UTF8.hpp"

#include "NodeVisitingOptimizer.hpp"
#include "../dataItem/DbXmlCompare.hpp"
#include "../dataItem/DbXmlFilter.hpp"
#include "../dataItem/DbXmlContext.hpp"
#include "../dataItem/DbXmlNav.hpp"
#include "../dataItem/DbXmlContains.hpp"
#include "../dataItem/DbXmlStep.hpp"
#include "../dataItem/Join.hpp"
#include "../dataItem/LookupIndexFunction.hpp"
#include "../dataItem/QueryPlanFunction.hpp"

#include <xqilla/context/DynamicContext.hpp>

using namespace DbXml;
using namespace std;

void NodeVisitingOptimizer::optimize(XQQuery *query)
{
	ImportedModules &modules = const_cast<ImportedModules&>(query->getImportedModules());
	for(ImportedModules::iterator it2 = modules.begin(); it2 != modules.end(); ++it2) {
		optimize(*it2);
	}

	vector<XQGlobalVariable*> newVars;
	GlobalVariables &vars = const_cast<GlobalVariables&>(query->getVariables());
	for(GlobalVariables::iterator it = vars.begin(); it != vars.end(); ++it) {
		XQGlobalVariable *newGV = optimizeGlobalVar(*it);
		if(newGV != 0) {
			newVars.push_back(newGV);
		}
	}
	vars.clear();
	for(vector<XQGlobalVariable*>::iterator i = newVars.begin();
	    i != newVars.end(); ++i) {
		vars.push_back(*i);
	}

	UserFunctions &funcs = const_cast<UserFunctions&>(query->getFunctions());
	for(UserFunctions::iterator i = funcs.begin(); i != funcs.end(); ++i) {
		(*i)->setFunctionBody(optimize(const_cast<ASTNode *>((*i)->getFunctionBody())));
	}

	if(query->getQueryBody() != 0) {
		query->setQueryBody(optimize(query->getQueryBody()));
	}
}

ASTNode *NodeVisitingOptimizer::optimize(ASTNode *item)
{
	ASTNode *result = item;
	switch(item->getType()) {
	case ASTNode::LITERAL: {
		result = optimizeLiteral((XQLiteral *)item);
		break;
	}
	case ASTNode::SEQUENCE: {
		result = optimizeSequence((XQSequence *)item);
		break;
	}
	case ASTNode::FUNCTION: {
		result = optimizeFunction((XQFunction *)item);
		break;
	}
	case ASTNode::NAVIGATION: {
		result = optimizeNav((XQNav *)item);
		break;
	}
	case ASTNode::VARIABLE: {
		result = optimizeVariable((XQVariable *)item);
		break;
	}
	case ASTNode::STEP: {
		result = optimizeStep((XQStep *)item);
		break;
	}
	case ASTNode::IF: {
		result = optimizeIf((XQIf *)item);
		break;
	}
	case ASTNode::INSTANCE_OF: {
		result = optimizeInstanceOf((XQInstanceOf *)item);
		break;
	}
	case ASTNode::CASTABLE_AS: {
		result = optimizeCastableAs((XQCastableAs *)item);
		break;
	}
	case ASTNode::CAST_AS: {
		result = optimizeCastAs((XQCastAs *)item);
		break;
	}
	case ASTNode::TREAT_AS: {
		result = optimizeTreatAs((XQTreatAs *)item);
		break;
	}
	case ASTNode::PARENTHESIZED: {
		result = optimizeParenthesizedExpr((XQParenthesizedExpr *)item);
		break;
	}
	case ASTNode::OPERATOR: {
		result = optimizeOperator((XQOperator *)item);
		break;
	}
	case ASTNode::CONTEXT_ITEM: {
		result = optimizeContextItem((XQContextItem *)item);
		break;
	}
	case ASTNode::DEBUG_HOOK: {
		result = optimizeDebugHook((XQDebugHook *)item);
		break;
	}
	case ASTNode::FLWOR: {
		result = optimizeFLWOR((XQFLWOR *)item);
		break;
	}
	case ASTNode::FLWOR_QUANTIFIED: {
		result = optimizeFLWORQuantified((XQQuantified *)item);
		break;
	}
	case ASTNode::TYPESWITCH: {
		result = optimizeTypeswitch((XQTypeswitch *)item);
		break;
	}
	case ASTNode::VALIDATE: {
		result = optimizeValidate((XQValidate *)item);
		break;
	}
	case ASTNode::FUNCTION_CALL: {
		result = optimizeFunctionCall((XQFunctionCall *)item);
		break;
	}
	case ASTNode::DOM_CONSTRUCTOR: {
		result = optimizeDOMConstructor((XQDOMConstructor *)item);
		break;
	}
	case ASTNode::ORDERING_CHANGE: {
		result = optimizeOrderingChange((XQOrderingChange *)item);
		break;
	}
	case ASTNode::ATOMIZE: {
		result = optimizeAtomize((XQAtomize *)item);
		break;
	}
	case ASTNode::XPATH1_CONVERT: {
		result = optimizeXPath1CompatConvertFunctionArg((XPath1CompatConvertFunctionArg *)item);
		break;
	}
	case ASTNode::PROMOTE_UNTYPED: {
		result = optimizePromoteUntyped((XQPromoteUntyped *)item);
		break;
	}
	case ASTNode::PROMOTE_NUMERIC: {
		result = optimizePromoteNumeric((XQPromoteNumeric *)item);
		break;
	}
	case ASTNode::PROMOTE_ANY_URI: {
		result = optimizePromoteAnyURI((XQPromoteAnyURI *)item);
		break;
	}
	case ASTNode::DOCUMENT_ORDER: {
		result = optimizeDocumentOrder((XQDocumentOrder *)item);
		break;
	}
	case ASTNode::PREDICATE: {
		result = optimizePredicate((XQPredicate *)item);
		break;
	}
	case ASTNode::USER_FUNCTION: {
		result = optimizeUserFunction((XQUserFunction::XQFunctionEvaluator *)item);
		break;
	}
	case ((ASTNode::whichType)DbXmlASTNode::DBXML_COMPARE): {
		result = optimizeDbXmlCompare((DbXmlCompare*)item);
		break;
	}
	case ((ASTNode::whichType)DbXmlASTNode::DBXML_FILTER): {
		result = optimizeDbXmlFilter((DbXmlFilter*)item);
		break;
	}
	case ((ASTNode::whichType)DbXmlASTNode::DBXML_CONTAINS): {
		result = optimizeDbXmlContains((DbXmlContains*)item);
		break;
	}
	case ((ASTNode::whichType)DbXmlASTNode::LOOKUP_INDEX): {
		result = optimizeLookupIndex((LookupIndex*)item);
		break;
	}
	case ((ASTNode::whichType)DbXmlASTNode::QUERY_PLAN_FUNCTION): {
		result = optimizeQueryPlanFunction((QueryPlanFunction*)item);
		break;
	}
	case ((ASTNode::whichType)DbXmlASTNode::DBXML_STEP): {
		result = optimizeDbXmlStep((DbXmlStep*)item);
		break;
	}
	case ((ASTNode::whichType)DbXmlASTNode::JOIN): {
		result = optimizeJoin((Join*)item);
		break;
	}
	case ((ASTNode::whichType)DbXmlASTNode::DBXML_NAV): {
		result = optimizeDbXmlNav((DbXmlNav*)item);
		break;
	}
	default: break;
	}
	return result;
}

ASTNode *NodeVisitingOptimizer::optimizeFunction(XQFunction *item)
{
	VectorOfASTNodes &args = const_cast<VectorOfASTNodes &>(item->getArguments());
	for(VectorOfASTNodes::iterator i = args.begin(); i != args.end(); ++i) {
		*i = optimize(*i);
	}
	return item;
}

ASTNode *NodeVisitingOptimizer::optimizeContextItem(XQContextItem *item)
{
	return item;
}

ASTNode *NodeVisitingOptimizer::optimizeLiteral(XQLiteral *item)
{
	return item;
}

ASTNode *NodeVisitingOptimizer::optimizeNav(XQNav *item)
{
	XQNav::Steps &args = const_cast<XQNav::Steps &>(item->getSteps());
	for(XQNav::Steps::iterator i = args.begin(); i != args.end(); ++i) {
		i->step = optimize(i->step);
	}
	return item;
}

ASTNode *NodeVisitingOptimizer::optimizeDbXmlNav(DbXmlNav *item)
{
	DbXmlNav::Steps &args = const_cast<DbXmlNav::Steps &>(item->getSteps());
	for(DbXmlNav::Steps::iterator i = args.begin(); i != args.end(); ++i) {
		*i = optimize(*i);
	}
	return item;
}

ASTNode *NodeVisitingOptimizer::optimizeParenthesizedExpr(XQParenthesizedExpr *item)
{
	VectorOfASTNodes &args = const_cast<VectorOfASTNodes &>(item->getChildren());
	for(VectorOfASTNodes::iterator i = args.begin(); i != args.end(); ++i) {
		*i = optimize(*i);
	}
	return item;
}

ASTNode *NodeVisitingOptimizer::optimizeSequence(XQSequence *item)
{
	return item;
}

ASTNode *NodeVisitingOptimizer::optimizeStep(XQStep *item)
{
	return item;
}

ASTNode *NodeVisitingOptimizer::optimizeDbXmlCompare(DbXmlCompare *item)
{
	item->setArgument(optimize(const_cast<ASTNode *>(item->getArgument())));
	return item;
}

ASTNode *NodeVisitingOptimizer::optimizeDbXmlFilter(DbXmlFilter *item)
{
	item->setArgument(optimize(const_cast<ASTNode *>(item->getArgument())));
	return item;
}

ASTNode *NodeVisitingOptimizer::optimizeDbXmlContains(DbXmlContains *item)
{
	item->setArgument(optimize(const_cast<ASTNode *>(item->getArgument())));
	return item;
}

ASTNode *NodeVisitingOptimizer::optimizeDbXmlStep(DbXmlStep *item)
{
	return item;
}

ASTNode *NodeVisitingOptimizer::optimizeLookupIndex(LookupIndex *item)
{
	return item;
}

ASTNode *NodeVisitingOptimizer::optimizeQueryPlanFunction(QueryPlanFunction *item)
{
	if(item->getArgument())
		item->setArgument(optimize(const_cast<ASTNode *>(item->getArgument())));
	return item;
}

ASTNode *NodeVisitingOptimizer::optimizeJoin(Join *item)
{
	item->setArgument(optimize(const_cast<ASTNode *>(item->getArgument())));
	return item;
}

ASTNode *NodeVisitingOptimizer::optimizeVariable(XQVariable *item)
{
	return item;
}

ASTNode *NodeVisitingOptimizer::optimizeIf(XQIf *item)
{
	item->setTest(optimize(const_cast<ASTNode *>(item->getTest())));
	item->setWhenTrue(optimize(const_cast<ASTNode *>(item->getWhenTrue())));
	item->setWhenFalse(optimize(const_cast<ASTNode *>(item->getWhenFalse())));
	return item;
}

ASTNode *NodeVisitingOptimizer::optimizeInstanceOf(XQInstanceOf *item)
{
	item->setExpression(optimize(const_cast<ASTNode *>(item->getExpression())));
	return item;
}

ASTNode *NodeVisitingOptimizer::optimizeCastableAs(XQCastableAs *item)
{
	item->setExpression(optimize(const_cast<ASTNode *>(item->getExpression())));
	return item;
}

ASTNode *NodeVisitingOptimizer::optimizeCastAs(XQCastAs *item)
{
	item->setExpression(optimize(const_cast<ASTNode *>(item->getExpression())));
	return item;
}

ASTNode *NodeVisitingOptimizer::optimizeTreatAs(XQTreatAs *item)
{
	item->setExpression(optimize(const_cast<ASTNode *>(item->getExpression())));
	return item;
}

ASTNode *NodeVisitingOptimizer::optimizeOperator(XQOperator *item)
{
	VectorOfASTNodes &args = const_cast<VectorOfASTNodes &>(item->getArguments());
	for(VectorOfASTNodes::iterator i = args.begin(); i != args.end(); ++i) {
		*i = optimize(*i);
	}
	return item;
}

ASTNode *NodeVisitingOptimizer::optimizeDebugHook(XQDebugHook *item)
{
	item->m_impl = optimize(item->m_impl);
	return item;
}

ASTNode *NodeVisitingOptimizer::optimizeFLWOR(XQFLWOR *item)
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
			(*j)->setExpression(optimize(const_cast<ASTNode *>((*j)->getExpression())));
		}
	}

	// where and return
	if(item->getWhereExpr()) {
		item->setWhereExpr(optimize(const_cast<ASTNode *>(item->getWhereExpr())));
	}
	item->setReturnExpr(optimize(const_cast<ASTNode *>(item->getReturnExpr())));

	return item;
}

ASTNode *NodeVisitingOptimizer::optimizeFLWORQuantified(XQQuantified *item)
{
	// bindings
	VectorOfVariableBinding *bindings = const_cast<VectorOfVariableBinding *>(item->getBindings());
	for(VectorOfVariableBinding::iterator i = bindings->begin(); i != bindings->end(); ++i) {
		(*i)->_allValues = optimize((*i)->_allValues);
	}

	// no where or sort specs

	// return
	item->setReturnExpr(optimize(const_cast<ASTNode *>(item->getReturnExpr())));

	return item;
}

ASTNode *NodeVisitingOptimizer::optimizeTypeswitch(XQTypeswitch *item)
{
	item->setExpression(optimize(const_cast<ASTNode *>(item->getExpression())));

	XQTypeswitch::VectorOfClause *clauses = const_cast<XQTypeswitch::VectorOfClause *>(item->getClauses());
	for(XQTypeswitch::VectorOfClause::iterator i = clauses->begin(); i != clauses->end(); ++i) {
		(*i)->_expr = optimize((*i)->_expr);
	}

	const_cast<XQTypeswitch::Clause *>(item->getDefaultClause())->_expr =
		optimize(const_cast<ASTNode *>(item->getDefaultClause()->_expr));

	return item;
}

ASTNode *NodeVisitingOptimizer::optimizeValidate(XQValidate *item)
{
	item->setExpression(optimize(const_cast<ASTNode *>(item->getExpression())));

	return item;
}

XQGlobalVariable *NodeVisitingOptimizer::optimizeGlobalVar(XQGlobalVariable *item)
{
	if(item->getVariableExpr()) {
		item->setVariableExpr(optimize(const_cast<ASTNode *>(item->getVariableExpr())));
	}
	return item;
}

ASTNode *NodeVisitingOptimizer::optimizeFunctionCall(XQFunctionCall *item)
{
	VectorOfASTNodes &args = const_cast<VectorOfASTNodes &>(item->getArguments());
	for(VectorOfASTNodes::iterator i = args.begin(); i != args.end(); ++i) {
		*i = optimize(*i);
	}
	return item;
}

ASTNode *NodeVisitingOptimizer::optimizeUserFunction(XQUserFunction::XQFunctionEvaluator *item)
{
	VectorOfASTNodes &args = const_cast<VectorOfASTNodes &>(item->getArguments());
	for(VectorOfASTNodes::iterator i = args.begin(); i != args.end(); ++i) {
		*i = optimize(*i);
	}
	return item;
}

ASTNode *NodeVisitingOptimizer::optimizeDOMConstructor(XQDOMConstructor *item)
{
	if(item->getName()) {
		item->setName(optimize(const_cast<ASTNode *>(item->getName())));
	}

	VectorOfASTNodes *attrs = const_cast<VectorOfASTNodes *>(item->getAttributes());
	if(attrs) {
		for(VectorOfASTNodes::iterator i = attrs->begin(); i != attrs->end(); ++i) {
			*i = optimize(*i);
		}
	}

	VectorOfASTNodes *children = const_cast<VectorOfASTNodes *>(item->getChildren());
	if(children) {
		for(VectorOfASTNodes::iterator j = children->begin(); j != children->end(); ++j) {
			*j = optimize(*j);
		}
	}

	if(item->getValue()) {
		item->setValue(optimize(const_cast<ASTNode *>(item->getValue())));
	}

	return item;
}

ASTNode *NodeVisitingOptimizer::optimizeOrderingChange(XQOrderingChange *item)
{
	item->setExpr(optimize(item->getExpr()));
	return item;
}

ASTNode *NodeVisitingOptimizer::optimizeAtomize(XQAtomize *item)
{
	item->setExpression(optimize(const_cast<ASTNode *>(item->getExpression())));
	return item;
}

ASTNode *NodeVisitingOptimizer::optimizeXPath1CompatConvertFunctionArg(XPath1CompatConvertFunctionArg *item)
{
	item->setExpression(optimize(const_cast<ASTNode *>(item->getExpression())));
	return item;
}

ASTNode *NodeVisitingOptimizer::optimizePromoteUntyped(XQPromoteUntyped *item)
{
	item->setExpression(optimize(const_cast<ASTNode *>(item->getExpression())));
	return item;
}

ASTNode *NodeVisitingOptimizer::optimizePromoteNumeric(XQPromoteNumeric *item)
{
	item->setExpression(optimize(const_cast<ASTNode *>(item->getExpression())));
	return item;
}

ASTNode *NodeVisitingOptimizer::optimizePromoteAnyURI(XQPromoteAnyURI *item)
{
	item->setExpression(optimize(const_cast<ASTNode *>(item->getExpression())));
	return item;
}

ASTNode *NodeVisitingOptimizer::optimizeDocumentOrder(XQDocumentOrder *item)
{
	item->setExpression(optimize(const_cast<ASTNode *>(item->getExpression())));
	return item;
}

ASTNode *NodeVisitingOptimizer::optimizePredicate(XQPredicate *item)
{
	item->setExpression(optimize(const_cast<ASTNode *>(item->getExpression())));
	item->setPredicate(optimize(const_cast<ASTNode *>(item->getPredicate())));
	return item;
}
