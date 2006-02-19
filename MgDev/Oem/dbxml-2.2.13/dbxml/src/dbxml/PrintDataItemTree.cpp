//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: PrintDataItemTree.cpp,v 1.24 2005/09/06 10:42:07 jsnelson Exp $
//

#include <iostream>
#include <sstream>

#include "PrintDataItemTree.hpp"
#include "UTF8.hpp"
#include "dataItem/QueryPlanFunction.hpp"
#include "dataItem/DbXmlDocAvailable.hpp"
#include "QueryPlan.hpp"
#include "TransactedContainer.hpp"
#include "dataItem/DbXmlNodeTest.hpp"
#include "dataItem/DbXmlCompare.hpp"
#include "dataItem/DbXmlContext.hpp"
#include "dataItem/DbXmlFilter.hpp"
#include "dataItem/DbXmlContains.hpp"
#include "dataItem/DbXmlNav.hpp"

#include <xquery/XQQuery.hpp>
#include <xquery/XQContext.hpp>
#include <xquery/dataItem/XQGlobalVariable.hpp>

#include <pathan/dataItem/DataItem.hpp>
#include <pathan/dataItem/DataItemFunction.hpp>
#include <pathan/DynamicContext.hpp>

using namespace DbXml;
using namespace std;

static const int INDENT = 1;

string DbXmlPrintDataItemTree::print(const PathanExpression *expr, const DynamicContext *context, int indent)
{
	return "Cannot print PathanExpression in BDB XML";
}

string DbXmlPrintDataItemTree::print(const XQQuery *query, const DynamicContext *context, int indent)
{
	ostringstream s;

	s << getIndent(indent) << "<XQuery";
	if(query->getIsLibraryModule()) {
		s << " module=\"true\"";
	}
	if(query->getModuleTargetNamespace()) {
		s << " targetNamespace=\"" << XMLChToUTF8(query->getModuleTargetNamespace()).str() << "\"";
	}
	s << ">" << endl;
	indent += INDENT;

	DbXmlPrintDataItemTree p;

	for(std::vector<XQFunction*, PathanAllocator<XQFunction*> >::const_iterator i = query->m_userDefFns.begin();
	    i != query->m_userDefFns.end(); ++i) {
		XQFunction *f = *i;

		const XMLCh *funUri = f->getURI();
		const XMLCh *funName = f->getName();

		string name("{");
		name += XMLChToUTF8(funUri).str();
		name += "}:";
		name += XMLChToUTF8(funName).str();

		s << "  <FunctionDefinition name=\"" << name << "\">" << endl;
		s << p.printDataItem(f->getFunctionBody(), context, 2);
		s << "  </FunctionDefinition>" << endl;
	  }
	for(vector<XQGlobalVariable*, PathanAllocator<XQGlobalVariable*> >::const_iterator it = query->m_userDefVars.begin();
	    it != query->m_userDefVars.end(); ++it) {
		s << p.printDataItem(*it, context, indent);
	}
	s << p.printDataItem(query->getQueryBody(), context, indent);

	indent -= INDENT;
	s << getIndent(indent) << "</XQuery>" << endl;

	return s.str();
}

string DbXmlPrintDataItemTree::print(const DataItem *item, const DynamicContext *context, int indent)
{
	DbXmlPrintDataItemTree p;
	return p.printDataItem(item, context, indent);
}

string DbXmlPrintDataItemTree::printDataItem(const DataItem *item, const DynamicContext *context, int indent)
{
	switch(item->getType()) {
	case DataItem::LITERAL: {
		return printLiteral((DataItemLiteral *)item, context, indent);
		break;
	}
	case DataItem::SEQUENCE: {
		return printSequence((DataItemSequence *)item, context, indent);
		break;
	}
	case DataItem::FUNCTION: {
		return printFunction((DataItemFunction *)item, context, indent);
		break;
	}
	case DataItem::NAVIGATION: {
		return printNav((DataItemNav *)item, context, indent);
		break;
	}
	case DataItem::VARIABLE: {
		return printVariable((DataItemVariable *)item, context, indent);
		break;
	}
	case DataItem::STEP: {
		return printStep((DataItemStep *)item, context, indent);
		break;
	}
	case DataItem::IF: {
		return printIf((DataItemIf *)item, context, indent);
		break;
	}
	case DataItem::INSTANCE_OF: {
		return printInstanceOf((DataItemInstanceOf *)item, context, indent);
		break;
	}
	case DataItem::CASTABLE_AS: {
		return printCastableAs((DataItemCastableAs *)item, context, indent);
		break;
	}
	case DataItem::CAST_AS: {
		return printCastAs((DataItemCastAs *)item, context, indent);
		break;
	}
	case DataItem::TREAT_AS: {
		return printTreatAs((DataItemTreatAs *)item, context, indent);
		break;
	}
	case DataItem::PARENTHESIZED: {
		return printParenthesized((DataItemParenthesizedExpr *)item, context, indent);
		break;
	}
	case DataItem::FOR: {
		return printFor((DataItemFor *)item, context, indent);
		break;
	}
	case DataItem::QUANTIFIED: {
		return printQuantified((DataItemQuantifiedExpr *)item, context, indent);
		break;
	}
	case DataItem::OPERATOR: {
		return printOperator((DataItemOperator *)item, context, indent);
		break;
	}
	case DataItem::CONTEXT_ITEM: {
		return printContextItem((DataItemContextItem *)item, context, indent);
		break;
	}
	case ((DataItem::whichType)XQContext::DEBUG_HOOK): {
		return printDebugHook((XQDebugHook *)item, context, indent);
		break;
	}
	case ((DataItem::whichType)XQContext::FLWOR): {
		return printFLWOR((XQFLWOR *)item, context, indent);
		break;
	}
	case ((DataItem::whichType)XQContext::FLWOR_QUANTIFIED): {
		return printFLWORQuantified((XQQuantified *)item, context, indent);
		break;
	}
	case ((DataItem::whichType)XQContext::TYPESWITCH): {
		return printTypeswitch((XQTypeswitch *)item, context, indent);
		break;
	}
	case ((DataItem::whichType)XQContext::VALIDATE): {
		return printValidate((XQValidate *)item, context, indent);
		break;
	}
	case ((DataItem::whichType)XQContext::VARIABLE_DEFINITION): {
		return printGlobal((XQGlobalVariable *)item, context, indent);
		break;
	}
	case ((DataItem::whichType)XQContext::FUNCTION_CALL): {
		return printFunctionCall((XQFunctionCall *)item, context, indent);
		break;
	}
	case ((DataItem::whichType)XQContext::DOM_CONSTRUCTOR): {
		return printDOMConstructor((XQDOMConstructor *)item, context, indent);
		break;
	}
	case ((DataItem::whichType)XQContext::ORDERING_CHANGE): {
		return printOrderingChange((XQOrderingChange *)item, context, indent);
		break;
	}
	case ((DataItem::whichType)XQContext::USER_FUNCTION): {
		return printUserFunction((XQFunction::XQFunctionEvaluator *)item, context, indent);
		break;
	}
	case ((DataItem::whichType)DbXmlContext::DBXML_COMPARE): {
		return printDbXmlCompare((DbXmlCompare*)item, context, indent);
		break;
	}
	case ((DataItem::whichType)DbXmlContext::DBXML_FILTER): {
		return printDbXmlFilter((DbXmlFilter*)item, context, indent);
		break;
	}
	case ((DataItem::whichType)DbXmlContext::DBXML_CONTAINS): {
		return printDbXmlContains((DbXmlContains*)item, context, indent);
		break;
	}
	default: break;
	}
	return getIndent(indent) + "<Unknown/>\n";
}

string DbXmlPrintDataItemTree::printFunction(const DataItemFunction *item, const DynamicContext *context, int indent)
{
	const XMLCh *funUri = item->getFunctionURI();
	const XMLCh *funName = item->getFunctionName();

	if(funUri == DataItemFunction::XMLChFunctionURI) {
		if(funName == QueryPlanFunction::name) {
			return printQueryPlanFunction((QueryPlanFunction*)item, context, indent);
		} else if(funName == DbXmlDocAvailable::name) {
			return printDbXmlDocAvailable((DbXmlDocAvailable*)item, context, indent);
		}
	}

	ostringstream s;

	string in(getIndent(indent));

	string name("{");
	name += XMLChToUTF8(funUri).str();
	name += "}:";
	name += XMLChToUTF8(funName).str();

	const VectorOfDataItems &args = item->getArguments();
	if(args.empty() && !hasPredicates(item)) {
		s << in << "<Function name=\"" << name << "\"/>" << endl;
	} else {
		s << in << "<Function name=\"" << name << "\">" << endl;
		for(VectorOfDataItems::const_iterator i = args.begin();
		    i != args.end(); ++i) {
			s << printDataItem(*i, context, indent + INDENT);
		}
		s << printPredicates(item, context, indent + INDENT);
		s << in << "</Function>" << endl;
	}

	return s.str();
}

string DbXmlPrintDataItemTree::printQueryPlanFunction(
	const QueryPlanFunction *item, const DynamicContext *context, int indent)
{
	ostringstream s;

	string in(getIndent(indent));

	string result(item->getFunctionType() == QueryPlanFunction::DOCUMENT ?
		      "document" : "collection");

	s << in << "<QueryPlanFunction result=\"" << result << "\"";
	if(item->getContainer() != 0) {
		s << " container=\"" << item->getContainer()->getName() << "\"";
	}
	s << ">" << endl;

	if(item->getQueryPlan()) {
		s << printQueryPlan(item->getQueryPlan(), item->isQueryPlanExecutable(), indent + INDENT);
	}

#if 0
#ifdef DEBUG
	// For now, don't print implied schema; it's not being
	// used, and makes query plan output larger
	if(item->getImpliedSchema() != 0) {
		s << in << "  <ImpliedSchema>" << endl;
		s << item->getImpliedSchema()->toString(
			indent + INDENT + INDENT);
		s << in << "  </ImpliedSchema>" << endl;
	} else {
		s << in << "  <ImpliedSchema/>" << endl;
	}
#endif
#endif

	if(item->getContainer() == 0) {
		const VectorOfDataItems &args = item->getArguments();
		for(VectorOfDataItems::const_iterator i = args.begin();
		    i != args.end(); ++i) {
			s << printDataItem(*i, context, indent + INDENT);
		}
	}

	s << printPredicates(item, context, indent + INDENT);
	s << in << "</QueryPlanFunction>" << endl;

	return s.str();
}

string DbXmlPrintDataItemTree::printDbXmlDocAvailable(
	const DbXmlDocAvailable *item, const DynamicContext *context, int indent)
{
	ostringstream s;

	string in(getIndent(indent));

	s << in << "<DbXmlDocAvailable";
	if(item->getContainer() != 0) {
		s << " container=\"" << item->getContainer()->getName() << "\"";
	}
	s << ">" << endl;

	if(item->getQueryPlan()) {
		s << printQueryPlan(item->getQueryPlan(), /*isExecutable*/true, indent + INDENT);
	}

	if(item->getContainer() == 0) {
		const VectorOfDataItems &args = item->getArguments();
		for(VectorOfDataItems::const_iterator i = args.begin();
		    i != args.end(); ++i) {
			s << printDataItem(*i, context, indent + INDENT);
		}
	}

	s << printPredicates(item, context, indent + INDENT);
	s << in << "</DbXmlDocAvailable>" << endl;

	return s.str();
}

string DbXmlPrintDataItemTree::printQueryPlan(const QueryPlan *qp, bool isExecutable, int indent)
{
	ostringstream s;

	string in(getIndent(indent));

	string planString = isExecutable ? "OQPlan" : "RQPlan";
 	if(qp != 0) {
		s << in << "<" << planString << ">" <<
			qp->toString(false) << "</" <<
			planString << ">" << endl;
	} else {
		s << in << "<" << planString << "/>" << endl;
	}

	return s.str();
}

string DbXmlPrintDataItemTree::printStep(const DataItemStep *item, const DynamicContext *context, int indent)
{
	ostringstream s;

	string in(getIndent(indent));
	const NodeTest *step = item->getNodeTest();
	const DbXmlNodeTest *dbxmlnt = (const DbXmlNodeTest*)step->getInterface(DbXmlNodeTest::gDbXml);

	s << in << "<Step";
	s << " axis=\"" << getAxisName(item->getAxis()) << "\"";
  
	SequenceType::ItemType *type = step->getItemType();
	if(type == 0) {
		if(step->getNamespaceWildcard()) {
			s << " uri=\"*\"";
		}
		else {
			if(step->getNodePrefix() != 0) {
				s << " prefix=\"" << XMLChToUTF8(step->getNodePrefix()).str() << "\"";
			}
			if(step->getNodeUri() != 0) {
				s << " uri=\"" << XMLChToUTF8(step->getNodeUri()).str() << "\"";
			}
		}
		if(step->getNameWildcard()) {
			s << " name=\"*\"";
		}
		else if(step->getNodeName() != 0) {
			s << " name=\"" << XMLChToUTF8(step->getNodeName()).str() << "\"";
		}
		
		if(step->getTypeWildcard()) {
			s << " nodeType=\"*\"";
		}
		else if(step->isNodeTypeSet()) {
			s << " nodeType=\"" << XMLChToUTF8(step->getNodeType()).str() << "\"";
		}
	}

	if(hasPredicates(item) || type != 0 ||
		(dbxmlnt != 0 && dbxmlnt->getQueryPlan() != 0))
	{
		s << ">" << endl;
		if(dbxmlnt != 0 && dbxmlnt->getQueryPlan() != 0) {
			s  << printQueryPlan(dbxmlnt->getQueryPlan(), dbxmlnt->isQueryPlanExecutable(), indent + INDENT);
		}
		if(type != 0) {
			s << in << "  <ItemType";
			s << printItemTypeAttrs(type, context);
			s << "/>" << endl;
		}
		s << printPredicates(item, context, indent + INDENT);
		s << in << "</Step>" << endl;
	}
	else {
		s << "/>" << endl;
	}

	return s.str();
}

string DbXmlPrintDataItemTree::printDbXmlCompare(const DbXmlCompare *item, const DynamicContext *context, int indent)
{
	ostringstream s;

	string in(getIndent(indent));
	string name = getComparisonOperationName(item->getOperation());

	s << in << "<DbXmlCompare name=\"" << name << "\">" << endl;
	if(item->getQueryPlan() != 0) {
		s  << printQueryPlan(item->getQueryPlan(), item->isQueryPlanExecutable(), indent + INDENT);
	}
	s << printDataItem(item->getArgument(), context, indent + INDENT);
	s << printPredicates(item, context, indent + INDENT);
	s << in << "</DbXmlCompare>" << endl;

	return s.str();
}

string DbXmlPrintDataItemTree::printDbXmlFilter(const DbXmlFilter *item, const DynamicContext *context, int indent)
{
	ostringstream s;

	string in(getIndent(indent));

	s << in << "<DbXmlFilter>" << endl;
	s << printDataItem(item->getFilterExpr(), context, indent + INDENT);
	s << printPredicates(item, context, indent + INDENT);
	s << in << "</DbXmlFilter>" << endl;

	return s.str();
}

string DbXmlPrintDataItemTree::printDbXmlContains(const DbXmlContains *item, const DynamicContext *context, int indent)
{
	ostringstream s;

	string in(getIndent(indent));

	s << in << "<DbXmlContains>" << endl;
	if(item->getQueryPlan() != 0) {
		s  << printQueryPlan(item->getQueryPlan(), item->isQueryPlanExecutable(), indent + INDENT);
	}
	s << printDataItem(item->getArgument(), context, indent + INDENT);
	s << printPredicates(item, context, indent + INDENT);
	s << in << "</DbXmlContains>" << endl;

	return s.str();
}
