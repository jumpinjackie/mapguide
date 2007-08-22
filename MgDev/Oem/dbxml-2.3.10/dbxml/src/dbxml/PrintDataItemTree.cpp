//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: PrintDataItemTree.cpp,v 1.41.2.2 2007/01/05 15:35:52 jsnelson Exp $
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
#include "dataItem/DbXmlFilter.hpp"
#include "dataItem/DbXmlContains.hpp"
#include "dataItem/DbXmlNav.hpp"
#include "dataItem/DbXmlStep.hpp"
#include "dataItem/LookupIndexFunction.hpp"

#include <xqilla/utils/UTF8Str.hpp>
#include <xqilla/simple-api/XQQuery.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/context/Collation.hpp>
#include <xqilla/ast/XQGlobalVariable.hpp>
#include <xqilla/ast/XQAtomize.hpp>
#include <xqilla/ast/XPath1Compat.hpp>
#include <xqilla/ast/ConvertFunctionArg.hpp>
#include <xqilla/ast/XQDocumentOrder.hpp>
#include <xqilla/ast/XQPredicate.hpp>

#include <xqilla/ast/ASTNode.hpp>
#include <xqilla/ast/XQFunction.hpp>
#include <xqilla/context/DynamicContext.hpp>

using namespace DbXml;
using namespace std;

static const int INDENT = 1;

string DbXmlPrintXQTree::print(const XQillaExpression *expr, const DynamicContext *context, int indent)
{
	return "Cannot print XQillaExpression in BDB XML";
}

string DbXmlPrintXQTree::print(const XQQuery *query, const DynamicContext *context, int indent)
{
	ostringstream s;

	string in(getIndent(indent));

	if(query->getIsLibraryModule()) {
		s << in << "<Module";
	} else {
		s << in << "<XQuery";
	}

	if(query->getModuleTargetNamespace()) {
		s << " targetNamespace=\"" << XMLChToUTF8(query->getModuleTargetNamespace()).str() << "\"";
	}
	s << ">" << endl;

	const ImportedModules &modules = query->getImportedModules();
	for(ImportedModules::const_iterator it = modules.begin();
	    it != modules.end(); ++it) {
		s << print(*it, context, indent + INDENT);
	}

	DbXmlPrintXQTree p;

	for(UserFunctions::const_iterator i = query->getFunctions().begin();
	    i != query->getFunctions().end(); ++i) {
		XQUserFunction *f = *i;

		const XMLCh *funUri = f->getURI();
		const XMLCh *funName = f->getName();

		string name("{");
		name += XMLChToUTF8(funUri).str();
		name += "}:";
		name += XMLChToUTF8(funName).str();

		s << in << "  <FunctionDefinition name=\"" << name << "\">" << endl;
		s << p.printASTNode(f->getFunctionBody(), context, indent + INDENT + INDENT);
		s << in << "  </FunctionDefinition>" << endl;
	  }
	for(GlobalVariables::const_iterator it = query->getVariables().begin();
	    it != query->getVariables().end(); ++it) {
		s << p.printGlobal(*it, context, indent + INDENT);
	}

	if(query->getQueryBody() != 0)
		s << in << p.printASTNode(query->getQueryBody(), context, indent + INDENT);

	if(query->getIsLibraryModule()) {
		s << in << "</Module>" << endl;
	} else {
		s << in << "</XQuery>" << endl;
	}

	return s.str();
}

string DbXmlPrintXQTree::print(const ASTNode *item, const DynamicContext *context, int indent)
{
	DbXmlPrintXQTree p;
	return p.printASTNode(item, context, indent);
}

string DbXmlPrintXQTree::printASTNode(const ASTNode *item, const DynamicContext *context, int indent)
{
	switch(item->getType()) {
	case ASTNode::LITERAL: {
		return printLiteral((XQLiteral *)item, context, indent);
		break;
	}
	case ASTNode::SEQUENCE: {
		return printSequence((XQSequence *)item, context, indent);
		break;
	}
	case ASTNode::FUNCTION: {
		return printFunction((XQFunction *)item, context, indent);
		break;
	}
	case ASTNode::NAVIGATION: {
		return printNav((XQNav *)item, context, indent);
		break;
	}
	case ASTNode::VARIABLE: {
		return printVariable((XQVariable *)item, context, indent);
		break;
	}
	case ASTNode::STEP: {
		return printStep((XQStep *)item, context, indent);
		break;
	}
	case ASTNode::IF: {
		return printIf((XQIf *)item, context, indent);
		break;
	}
	case ASTNode::INSTANCE_OF: {
		return printInstanceOf((XQInstanceOf *)item, context, indent);
		break;
	}
	case ASTNode::CASTABLE_AS: {
		return printCastableAs((XQCastableAs *)item, context, indent);
		break;
	}
	case ASTNode::CAST_AS: {
		return printCastAs((XQCastAs *)item, context, indent);
		break;
	}
	case ASTNode::TREAT_AS: {
		return printTreatAs((XQTreatAs *)item, context, indent);
		break;
	}
	case ASTNode::PARENTHESIZED: {
		return printParenthesized((XQParenthesizedExpr *)item, context, indent);
		break;
	}
	case ASTNode::OPERATOR: {
		return printOperator((XQOperator *)item, context, indent);
		break;
	}
	case ASTNode::CONTEXT_ITEM: {
		return printContextItem((XQContextItem *)item, context, indent);
		break;
	}
	case ASTNode::DEBUG_HOOK: {
		return printDebugHook((XQDebugHook *)item, context, indent);
		break;
	}
	case ASTNode::FLWOR: {
		return printFLWOR((XQFLWOR *)item, context, indent);
		break;
	}
	case ASTNode::FLWOR_QUANTIFIED: {
		return printFLWORQuantified((XQQuantified *)item, context, indent);
		break;
	}
	case ASTNode::TYPESWITCH: {
		return printTypeswitch((XQTypeswitch *)item, context, indent);
		break;
	}
	case ASTNode::VALIDATE: {
		return printValidate((XQValidate *)item, context, indent);
		break;
	}
	case ASTNode::FUNCTION_CALL: {
		return printFunctionCall((XQFunctionCall *)item, context, indent);
		break;
	}
	case ASTNode::DOM_CONSTRUCTOR: {
		return printDOMConstructor((XQDOMConstructor *)item, context, indent);
		break;
	}
	case ASTNode::ORDERING_CHANGE: {
		return printOrderingChange((XQOrderingChange *)item, context, indent);
		break;
	}
	case ASTNode::ATOMIZE: {
		return printAtomize((XQAtomize *)item, context, indent);
		break;
	}
	case ASTNode::DOCUMENT_ORDER: {
		return printDocumentOrder((XQDocumentOrder *)item, context, indent);
		break;
	}
	case ASTNode::XPATH1_CONVERT: {
		return printXPath1CompatConvertFunctionArg((XPath1CompatConvertFunctionArg *)item, context, indent);
		break;
	}
	case ASTNode::PROMOTE_UNTYPED: {
		return printPromoteUntyped((XQPromoteUntyped *)item, context, indent);
		break;
	}
	case ASTNode::PROMOTE_NUMERIC: {
		return printPromoteNumeric((XQPromoteNumeric *)item, context, indent);
		break;
	}
	case ASTNode::PROMOTE_ANY_URI: {
		return printPromoteAnyURI((XQPromoteAnyURI *)item, context, indent);
		break;
	}
	case ASTNode::PREDICATE: {
		return printPredicate((XQPredicate *)item, context, indent);
		break;
	}
	case ASTNode::USER_FUNCTION: {
		return printUserFunction((XQUserFunction::XQFunctionEvaluator *)item, context, indent);
		break;
	}
	case ((ASTNode::whichType)DbXmlASTNode::DBXML_STEP): {
		return printDbXmlStep((DbXmlStep*)item, context, indent);
		break;
	}
	case ((ASTNode::whichType)DbXmlASTNode::DBXML_COMPARE): {
		return printDbXmlCompare((DbXmlCompare*)item, context, indent);
		break;
	}
	case ((ASTNode::whichType)DbXmlASTNode::DBXML_FILTER): {
		return printDbXmlFilter((DbXmlFilter*)item, context, indent);
		break;
	}
	case ((ASTNode::whichType)DbXmlASTNode::DBXML_CONTAINS): {
		return printDbXmlContains((DbXmlContains*)item, context, indent);
		break;
	}
        case ((ASTNode::whichType)DbXmlASTNode::LOOKUP_INDEX): {
                return printLookupIndex((LookupIndex*)item, context, indent);
                break;
        }
        case ((ASTNode::whichType)DbXmlASTNode::QUERY_PLAN_FUNCTION): {
                return printQueryPlanFunction((QueryPlanFunction*)item, context, indent);
                break;
        }
        case ((ASTNode::whichType)DbXmlASTNode::JOIN): {
                return printJoin((Join*)item, context, indent);
                break;
        }
	case ((ASTNode::whichType)DbXmlASTNode::DBXML_NAV): {
		return printDbXmlNav((DbXmlNav*)item, context, indent);
		break;
	}
	default: break;
	}
	return getIndent(indent) + "<Unknown/>\n";
}

string DbXmlPrintXQTree::printFunction(const XQFunction *item, const DynamicContext *context, int indent)
{
	const XMLCh *funUri = item->getFunctionURI();
	const XMLCh *funName = item->getFunctionName();

	if(funUri == XQFunction::XMLChFunctionURI) {
		if(funName == DbXmlDocAvailable::name) {
			return printDbXmlDocAvailable((DbXmlDocAvailable*)item, context, indent);
		}
	}

	ostringstream s;

	string in(getIndent(indent));

	string name("{");
	name += XMLChToUTF8(funUri).str();
	name += "}:";
	name += XMLChToUTF8(funName).str();

	const VectorOfASTNodes &args = item->getArguments();
	if(args.empty()) {
		s << in << "<Function name=\"" << name << "\"/>" << endl;
	} else {
		s << in << "<Function name=\"" << name << "\">" << endl;
		for(VectorOfASTNodes::const_iterator i = args.begin();
		    i != args.end(); ++i) {
			s << printASTNode(*i, context, indent + INDENT);
		}
		s << in << "</Function>" << endl;
	}

	return s.str();
}

string DbXmlPrintXQTree::printQueryPlanFunction(
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

	if(item->getContainer() == 0 && item->getArgument() != 0) {
		s << printASTNode(item->getArgument(), context, indent + INDENT);
	}

	s << in << "</QueryPlanFunction>" << endl;

	return s.str();
}

string DbXmlPrintXQTree::printDbXmlDocAvailable(
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
		const VectorOfASTNodes &args = item->getArguments();
		for(VectorOfASTNodes::const_iterator i = args.begin();
		    i != args.end(); ++i) {
			s << printASTNode(*i, context, indent + INDENT);
		}
	}

	s << in << "</DbXmlDocAvailable>" << endl;

	return s.str();
}

string DbXmlPrintXQTree::printQueryPlan(const QueryPlan *qp, bool isExecutable, int indent)
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

string DbXmlPrintXQTree::printNodeTestAttrs(const NodeTest *step)
{
	ostringstream s;

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

	return s.str();
}

static string getJoinTypeName(Join::Type join)
{
  switch(join) {
  case Join::ANCESTOR: {
    return "ancestor";
  }
  case Join::ANCESTOR_OR_SELF: {
    return "ancestor-or-self";
  }
  case Join::ATTRIBUTE: {
    return "attribute";
  }
  case Join::CHILD: {
    return "child";
  }
  case Join::DESCENDANT: {
    return "descendant";
  }
  case Join::DESCENDANT_OR_SELF: {
    return "descendant-or-self";
  }
  case Join::FOLLOWING: {
    return "following";
  }
  case Join::FOLLOWING_SIBLING: {
    return "following-sibling";
  }
  case Join::NAMESPACE: {
    return "namespace";
  }
  case Join::PARENT: {
    return "parent";
  }
  case Join::PRECEDING: {
    return "preceding";
  }
  case Join::PRECEDING_SIBLING: {
    return "preceding-sibling";
  }
  case Join::SELF: {
    return "self";
  }
  case Join::PARENT_A: {
	  return "parent-of-attribute";
  }
  case Join::PARENT_C: {
	  return "parent-of-child";
  }
  case Join::ATTRIBUTE_OR_CHILD: {
	  return "attribute-or-child";
  }
  default: {
    return "unknown";
  }
  }
}

string DbXmlPrintXQTree::printDbXmlStep(const DbXmlStep *item, const DynamicContext *context, int indent)
{
	ostringstream s;

	string in(getIndent(indent));
	const DbXmlNodeTest *nt = item->getNodeTest();

	s << in << "<DbXmlStep";
	s << " axis=\"" << getJoinTypeName(item->getJoinType()) << "\"";
  
	SequenceType::ItemType *type = 0;
	if(nt != 0) {
		type = nt->getItemType();
		if(type == 0) {
			s << printNodeTestAttrs(nt);
		}
	}

	if(type != 0 || item->getQueryPlan() != 0)
	{
		s << ">" << endl;
		if(item->getQueryPlan() != 0) {
			s  << printQueryPlan(item->getQueryPlan(), item->isQueryPlanExecutable(), indent + INDENT);
		}
		if(type != 0) {
			s << in << "  <ItemType";
			s << printItemTypeAttrs(type, context);
			s << "/>" << endl;
		}
		s << in << "</DbXmlStep>" << endl;
	}
	else {
		s << "/>" << endl;
	}

	return s.str();
}

string DbXmlPrintXQTree::printDbXmlCompare(const DbXmlCompare *item, const DynamicContext *context, int indent)
{
	ostringstream s;

	string in(getIndent(indent));
	string name = getComparisonOperationName(item->getOperation());

	const DbXmlNodeTest *nt = item->getNodeTest();

	s << in << "<DbXmlCompare name=\"" << name << "\"";
	s << " join=\"" << getJoinTypeName(item->getJoinType()) << "\"";
	if(nt != 0 && nt->getItemType() == 0) {
		s << printNodeTestAttrs(nt);
	}
	s << ">" << endl;

	if(item->getQueryPlan() != 0) {
		s  << printQueryPlan(item->getQueryPlan(), item->isQueryPlanExecutable(), indent + INDENT);
	}
	if(nt != 0 && nt->getItemType() != 0) {
		s << in << "  <ItemType";
		s << printItemTypeAttrs(nt->getItemType(), context);
		s << "/>" << endl;
	}
	s << printASTNode(item->getArgument(), context, indent + INDENT);
	s << in << "</DbXmlCompare>" << endl;

	return s.str();
}

string DbXmlPrintXQTree::printDbXmlFilter(const DbXmlFilter *item, const DynamicContext *context, int indent)
{
	ostringstream s;

	string in(getIndent(indent));

	s << in << "<DbXmlFilter>" << endl;
	s << printASTNode(item->getArgument(), context, indent + INDENT);
	s << in << "</DbXmlFilter>" << endl;

	return s.str();
}

string DbXmlPrintXQTree::printDbXmlContains(const DbXmlContains *item, const DynamicContext *context, int indent)
{
	ostringstream s;

	string in(getIndent(indent));

	const DbXmlNodeTest *nt = item->getNodeTest();

	s << in << "<DbXmlContains";
	s << " join=\"" << getJoinTypeName(item->getJoinType()) << "\"";
	if(nt != 0 && nt->getItemType() == 0) {
		s << printNodeTestAttrs(nt);
	}
	s << ">" << endl;

	if(item->getQueryPlan() != 0) {
		s  << printQueryPlan(item->getQueryPlan(), item->isQueryPlanExecutable(), indent + INDENT);
	}
	if(nt != 0 && nt->getItemType() != 0) {
		s << in << "  <ItemType";
		s << printItemTypeAttrs(nt->getItemType(), context);
		s << "/>" << endl;
	}
	s << printASTNode(item->getArgument(), context, indent + INDENT);
	s << in << "</DbXmlContains>" << endl;

	return s.str();
}

string DbXmlPrintXQTree::printLookupIndex(const LookupIndex *item, const DynamicContext *context, int indent)
{
        ostringstream s;

        string in(getIndent(indent));

        s << in << "<LookupIndex";
        if(item->getContainer() != 0) {
                s << " container=\"" << item->getContainer()->getName() << "\"";
        }
        s << ">" << endl;
        if(item->getQueryPlan() != 0) {
		s << item->getQueryPlan()->printQueryPlan(context, indent + INDENT);
        }
        s << in << "</LookupIndex>" << endl;

        return s.str();
}

string DbXmlPrintXQTree::printJoin(const Join *item, const DynamicContext *context, int indent)
{
        ostringstream s;

        string in(getIndent(indent));

        s << in << "<Join";
	s << " type=\"" << getJoinTypeName(item->getJoinType()) << "\"";
	s << " return=\"" << (item->getReturnArg() ? "argument" : "context_item") << "\"";
	s << ">" << endl;
	s << printASTNode(item->getArgument(), context, indent + INDENT);
        s << in << "</Join>" << endl;

        return s.str();
}

string DbXmlPrintXQTree::printDbXmlNav(const DbXmlNav *item, const DynamicContext *context, int indent)
{
	ostringstream s;

	string in(getIndent(indent));

	const DbXmlNav::Steps &steps = item->getSteps();

	s << in << "<DbXmlNav>" << endl;
	for(DbXmlNav::Steps::const_iterator i = steps.begin(); i != steps.end(); ++i) {
		s << printASTNode(*i, context, indent + INDENT);
	}
	s << in << "</DbXmlNav>" << endl;

	return s.str();
}
