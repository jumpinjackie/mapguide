//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: ASTReplaceOptimizer.cpp,v 1.4 2006/10/30 17:45:58 bostic Exp $
//

#include "../DbXmlInternal.hpp"
#include "ASTReplaceOptimizer.hpp"
#include "../dataItem/QueryPlanFunction.hpp"
#include "../dataItem/DbXmlDocAvailable.hpp"
#include "../dataItem/DbXmlAtomize.hpp"
#include "../dataItem/DbXmlCompare.hpp"
#include "../dataItem/DbXmlContains.hpp"
#include "../dataItem/DbXmlStep.hpp"

#include <xqilla/functions/FunctionDoc.hpp>
#include <xqilla/functions/FunctionDocAvailable.hpp>
#include <xqilla/functions/FunctionCollection.hpp>
#include <xqilla/functions/FunctionContains.hpp>
#include <xqilla/operators/GeneralComp.hpp>
#include <xqilla/operators/Equals.hpp>
#include <xqilla/operators/NotEquals.hpp>
#include <xqilla/operators/LessThan.hpp>
#include <xqilla/operators/LessThanEqual.hpp>
#include <xqilla/operators/GreaterThan.hpp>
#include <xqilla/operators/GreaterThanEqual.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/exceptions/XMLParseException.hpp>

using namespace DbXml;
using namespace std;

ASTNode *ASTReplaceOptimizer::optimizeOperator(XQOperator *item)
{
	XPath2MemoryManager *mm = xpc_->getMemoryManager();

	NodeVisitingOptimizer::optimizeOperator(item);

	if(item->getOperatorName() == GeneralComp::name) {
		DbXmlGeneralComp *result = new (mm) DbXmlGeneralComp((GeneralComp*)item, mm);
		result->setLocationInfo(item);
		return result;
	}

	else if(item->getOperatorName() == Equals::name) {
		DbXmlEquals *result = new (mm) DbXmlEquals((Equals*)item, mm);
		result->setLocationInfo(item);
		return result;
	}

	else if(item->getOperatorName() == NotEquals::name) {
		DbXmlNotEquals *result = new (mm) DbXmlNotEquals((NotEquals*)item, mm);
		result->setLocationInfo(item);
		return result;
	}

	else if(item->getOperatorName() == LessThan::name) {
		DbXmlLessThan *result = new (mm) DbXmlLessThan((LessThan*)item, mm);
		result->setLocationInfo(item);
		return result;
	}

	else if(item->getOperatorName() == LessThanEqual::name) {
		DbXmlLessThanEqual *result = new (mm) DbXmlLessThanEqual((LessThanEqual*)item, mm);
		result->setLocationInfo(item);
		return result;
	}

	else if(item->getOperatorName() == GreaterThan::name) {
		DbXmlGreaterThan *result = new (mm) DbXmlGreaterThan((GreaterThan*)item, mm);
		result->setLocationInfo(item);
		return result;
	}

	else if(item->getOperatorName() == GreaterThanEqual::name) {
		DbXmlGreaterThanEqual *result = new (mm) DbXmlGreaterThanEqual((GreaterThanEqual*)item, mm);
		result->setLocationInfo(item);
		return result;
	}

	return item;
}

ASTNode *ASTReplaceOptimizer::optimizeFunction(XQFunction *item)
{
	XPath2MemoryManager *mm = xpc_->getMemoryManager();

	const XMLCh *uri = item->getFunctionURI();
	const XMLCh *name = item->getFunctionName();

	NodeVisitingOptimizer::optimizeFunction(item);

	// Replace fn:doc() or fn:collection() with our fn:QueryPlanFunction()
	if(uri == XQFunction::XMLChFunctionURI) {
		if(name == FunctionDoc::name || name == FunctionCollection::name) {
			QueryPlanFunction::Type type = 
				(name == FunctionDoc::name ? QueryPlanFunction::DOCUMENT : QueryPlanFunction::COLLECTION);
			ASTNode *arg = item->getArguments().size() == 0 ? 0 : item->getArguments()[0];
			QueryPlanFunction *result = new (mm)
				QueryPlanFunction(minder_, type, arg, mm);
			result->setLocationInfo(item);

			return result;
		}

		else if(name == FunctionDocAvailable::name) {
			DbXmlDocAvailable *result = new (mm)
				DbXmlDocAvailable(minder_, item->getArguments(), mm);
			result->setLocationInfo(item);
			return result;
		}

		else if(name == FunctionContains::name) {
			DbXmlFunContains *result = new (mm) DbXmlFunContains((FunctionContains*)item, mm);
			result->setLocationInfo(item);
			return result;
		}
	}

	return item;
}

ASTNode *ASTReplaceOptimizer::optimizeAtomize(XQAtomize *item)
{
	XPath2MemoryManager *mm = xpc_->getMemoryManager();

	DbXmlAtomize *result = new (mm) DbXmlAtomize(item, mm);
	result->setLocationInfo(item);

	return NodeVisitingOptimizer::optimizeAtomize(result);
}

ASTNode *ASTReplaceOptimizer::optimizeStep(XQStep *item)
{
	XPath2MemoryManager *mm = xpc_->getMemoryManager();

	// Replace the XQStep with our own DbXmlStep
	DbXmlStep *result = new (mm) DbXmlStep(item, mm);
	result->setLocationInfo(item);

	return result;
}

ASTNode *ASTReplaceOptimizer::optimizeDocumentOrder(XQDocumentOrder *item)
{
	// Remove XQDocumentOrder from around XQNav - we'll add them back in again
	if(item->getExpression()->getType() == ASTNode::NAVIGATION ||
		item->getExpression()->getType() == (ASTNode::whichType)DbXmlASTNode::DBXML_NAV) {
		return optimize(const_cast<ASTNode *>(item->getExpression()));
	}

	return NodeVisitingOptimizer::optimizeDocumentOrder(item);
}

ASTNode *ASTReplaceOptimizer::optimizeValidate(XQValidate *item)
{
	// We don't support "validate" expressions, so we need to check
	// for them and raise an error if we find them.
	// NB We throw an XQilla exception to get the file and line info
	XQThrow3(XMLParseException,X("ASTReplaceOptimizer::optimizeValidate"),
		X("The validation feature is not supported. You cannot use the \"validate\" keyword. [err:XQST0075]"),
		item);
}
