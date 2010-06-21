/*
 * Copyright (c) 2001-2006
 *     DecisionSoft Limited. All rights reserved.
 * Copyright (c) 2004-2006
 *     Progress Software Corporation. All rights reserved.
 * Copyright (c) 2004-2006
 *     Oracle. All rights reserved.
 *
 * See the file LICENSE for redistribution information.
 *
 * $Id: PrintAST.cpp,v 1.15.2.1 2007/01/03 19:19:51 jpcs Exp $
 */

#include "../config/xqilla_config.h"
#include <iostream>
#include <sstream>

#include <xqilla/utils/PrintAST.hpp>
#include <xqilla/utils/UTF8Str.hpp>

#include "../dom-api/impl/XQillaExpressionImpl.hpp"
#include <xqilla/ast/ASTNode.hpp>
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
#include <xqilla/schema/SequenceType.hpp>
#include <xqilla/parser/QName.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/items/Item.hpp>
#include <xqilla/items/AnyAtomicType.hpp>
#include <xqilla/utils/XStr.hpp>
#include <xqilla/exceptions/NamespaceLookupException.hpp>
#include <xqilla/operators/GeneralComp.hpp>
#include <xqilla/simple-api/XQQuery.hpp>
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
#include <xqilla/ast/XQFunction.hpp>
#include <xqilla/ast/XQAtomize.hpp>
#include <xqilla/ast/XPath1Compat.hpp>
#include <xqilla/ast/ConvertFunctionArg.hpp>
#include <xqilla/ast/XQDocumentOrder.hpp>
#include <xqilla/ast/XQPredicate.hpp>
#include <xqilla/fulltext/FTContains.hpp>
#include <xqilla/fulltext/FTSelection.hpp>
#include <xqilla/fulltext/FTWords.hpp>
#include <xqilla/fulltext/FTOr.hpp>
#include <xqilla/fulltext/FTAnd.hpp>
#include <xqilla/fulltext/FTMildnot.hpp>
#include <xqilla/fulltext/FTUnaryNot.hpp>
#include <xqilla/fulltext/FTOrder.hpp>
#include <xqilla/fulltext/FTDistance.hpp>
#include <xqilla/fulltext/FTRange.hpp>
#include <xqilla/fulltext/FTScope.hpp>
#include <xqilla/fulltext/FTContent.hpp>
#include <xqilla/fulltext/FTWindow.hpp>

#include <xercesc/dom/DOMNode.hpp>
#if defined(XERCES_HAS_CPP_NAMESPACE)
  XERCES_CPP_NAMESPACE_USE
#endif

using namespace std;

static const int INDENT = 1;

string PrintAST::print(const XQillaExpression *expr, const DynamicContext *context, int indent)
{
  return print(((const XQillaExpressionImpl *)expr)->getCompiledExpression(), context, indent);
}

string PrintAST::print(const XQQuery *query, const DynamicContext *context, int indent)
{
  ostringstream s;

	string in(getIndent(indent));

  if(query->getIsLibraryModule()) {
    s << in << "<Module";
  }
  else {
    s << in << "<XQuery";
  }
  if(query->getModuleTargetNamespace()) {
    s << " targetNamespace=\"" << UTF8(query->getModuleTargetNamespace()) << "\"";
  }
  s << ">" << endl;

  const ImportedModules &modules = query->getImportedModules();
  for(ImportedModules::const_iterator it = modules.begin();
      it != modules.end(); ++it) {
    s << print(*it, context, indent + INDENT);
  }

  PrintAST p;

  for(std::vector<XQUserFunction*, XQillaAllocator<XQUserFunction*> >::const_iterator i = query->getFunctions().begin();
      i != query->getFunctions().end(); ++i) {
    XQUserFunction *f = *i;

    const XMLCh *funUri = f->getURI();
    const XMLCh *funName = f->getName();

    string name("{");
    name += UTF8(funUri);
    name += "}:";
    name += UTF8(funName);

    s << in << "  <FunctionDefinition name=\"" << name << "\">" << endl;
    s << p.printASTNode(f->getFunctionBody(), context, indent + INDENT + INDENT);
    s << in << "  </FunctionDefinition>" << endl;
  }
  for(vector<XQGlobalVariable*, XQillaAllocator<XQGlobalVariable*> >::const_iterator it = query->getVariables().begin();
      it != query->getVariables().end(); ++it) {
    s << p.printGlobal(*it, context, indent + INDENT);
  }

  if(query->getQueryBody() != 0)
	  s << p.printASTNode(query->getQueryBody(), context, indent + INDENT);

  if(query->getIsLibraryModule()) {
    s << in << "</Module>";
  }
  else {
    s << in << "</XQuery>";
  }

  return s.str();
}

string PrintAST::print(const ASTNode *item, const DynamicContext *context, int indent)
{
  PrintAST p;
  return p.printASTNode(item, context, indent);
}

string PrintAST::printASTNode(const ASTNode *item, const DynamicContext *context, int indent)
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
  case ASTNode::FTCONTAINS: {
    return printFTContains((FTContains *)item, context, indent);
    break;
  }
  default:
    break;
  }
  return getIndent(indent) + "<Unknown/>\n";
}

string PrintAST::printFunction(const XQFunction *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  const XMLCh *funUri = item->getFunctionURI();
  const XMLCh *funName = item->getFunctionName();

  string name("{");
  name += UTF8(funUri);
  name += "}:";
  name += UTF8(funName);

  const VectorOfASTNodes &args = item->getArguments();
  if(args.empty()) {
    s << in << "<Function name=\"" << name << "\"/>" << endl;
  }
  else {
    s << in << "<Function name=\"" << name << "\">" << endl;
    for(VectorOfASTNodes::const_iterator i = args.begin(); i != args.end(); ++i) {
      s << printASTNode(*i, context, indent + INDENT);
    }
    s << in << "</Function>" << endl;
  }

  return s.str();
}

string PrintAST::printOperator(const XQOperator *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));
  string name(UTF8(item->getOperatorName()));
  if(name == "comp") {
    name = getComparisonOperationName(((const GeneralComp *)item)->getOperation());
  }

  const VectorOfASTNodes &args = item->getArguments();
  if(args.empty()) {
    s << in << "<Operator name=\"" << name << "\"/>" << endl;
  }
  else {
    s << in << "<Operator name=\"" << name << "\">" << endl;
    for(VectorOfASTNodes::const_iterator i = args.begin(); i != args.end(); ++i) {
      s << printASTNode(*i, context, indent + INDENT);
    }
    s << in << "</Operator>" << endl;
  }

  return s.str();
}

string PrintAST::printContextItem(const XQContextItem *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<ContextItem/>" << endl;

  return s.str();
}

string PrintAST::printLiteral(const XQLiteral *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<Literal>" << endl;
  s << in << "  " << item->getItemConstructor()->asString(context) << endl;
  s << in << "</Literal>" << endl;

  return s.str();
}

string PrintAST::printNav(const XQNav *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  const XQNav::Steps &steps = item->getSteps();

  s << in << "<Navigation>" << endl;
  for(XQNav::Steps::const_iterator i = steps.begin(); i != steps.end(); ++i) {
    s << printASTNode(i->step, context, indent + INDENT);
  }
  s << in << "</Navigation>" << endl;

  return s.str();
}

string PrintAST::printStep(const XQStep *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));
  const NodeTest *step = item->getNodeTest();

  s << in << "<Step";
  s << " axis=\"" << getAxisName(item->getAxis()) << "\"";
  
  SequenceType::ItemType *type = step->getItemType();
  if(type == 0) {
    if(step->getNamespaceWildcard()) {
      s << " uri=\"*\"";
    }
    else {
      if(step->getNodePrefix() != 0) {
        s << " prefix=\"" << UTF8(step->getNodePrefix()) << "\"";
      }
      if(step->getNodeUri() != 0) {
        s << " uri=\"" << UTF8(step->getNodeUri()) << "\"";
      }
    }
    if(step->getNameWildcard()) {
      s << " name=\"*\"";
    }
    else if(step->getNodeName() != 0) {
      s << " name=\"" << UTF8(step->getNodeName()) << "\"";
    }
    
    if(step->getTypeWildcard()) {
      s << " nodeType=\"*\"";
    }
    else if(step->isNodeTypeSet()) {
      s << " nodeType=\"" << UTF8(step->getNodeType()) << "\"";
    }
  }

  if(type != 0) {
    s << ">" << endl;
    if(type != 0) {
      s << in << "  <ItemType";
      s << printItemTypeAttrs(type, context);
      s << "/>" << endl;
    }
    s << in << "</Step>" << endl;
  }
  else {
    s << "/>" << endl;
  }

  return s.str();
}

string PrintAST::printParenthesized(const XQParenthesizedExpr *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));
  const VectorOfASTNodes &children = item->getChildren();

  if(children.empty()) {
    s << in << "<Parenthesized/>" << endl;
  }
  else {
    s << in << "<Parenthesized>" << endl;
    for(VectorOfASTNodes::const_iterator i = children.begin(); i != children.end(); ++i) {
      s << printASTNode(*i, context, indent + INDENT);
    }
    s << in << "</Parenthesized>" << endl;
  }

  return s.str();
}

string PrintAST::printSequence(const XQSequence *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));
  const ItemConstructor::Vector &values = item->getItemConstructors();

  if(values.empty()) {
    s << in << "<Sequence/>" << endl;
  }
  else {
    s << in << "<Sequence>" << endl;
    for(ItemConstructor::Vector::const_iterator i = values.begin(); i != values.end(); ++i) {
      s << in << "  " << (*i)->asString(context) << endl;
    }
    s << in << "</Sequence>" << endl;
  }

  return s.str();
}

string PrintAST::printVariable(const XQVariable *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));
  string prefix(UTF8(item->getPrefix()));
  string name(UTF8(item->getName()));
  if(prefix != "") {
    name = prefix + ":" + name;
  }

  s << in << "<Variable name=\"" << name << "\"/>" << endl;

  return s.str();
}

string PrintAST::printIf(const XQIf *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<If>" << endl;
  s << in << "  <Test>" << endl;
  s << printASTNode(item->getTest(), context, indent + INDENT + INDENT);
  s << in << "  </Test>" << endl;
  s << in << "  <Then>" << endl;
  s << printASTNode(item->getWhenTrue(), context, indent + INDENT + INDENT);
  s << in << "  </Then>" << endl;
  s << in << "  <Else>" << endl;
  s << printASTNode(item->getWhenFalse(), context, indent + INDENT + INDENT);
  s << in << "  </Else>" << endl;
  s << in << "</If>" << endl;

  return s.str();
}

string PrintAST::printInstanceOf(const XQInstanceOf *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<InstanceOf>" << endl;
  s << printASTNode(item->getExpression(), context, indent + INDENT);
  s << printSequenceType(item->getSequenceType(), context, indent + INDENT);
  s << in << "</InstanceOf>" << endl;

  return s.str();
}

string PrintAST::printCastableAs(const XQCastableAs *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<CastableAs>" << endl;
  s << printASTNode(item->getExpression(), context, indent + INDENT);
  s << printSequenceType(item->getSequenceType(), context, indent + INDENT);
  s << in << "</CastableAs>" << endl;

  return s.str();
}

string PrintAST::printCastAs(const XQCastAs *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<CastAs>" << endl;
  s << printASTNode(item->getExpression(), context, indent + INDENT);
  s << printSequenceType(item->getSequenceType(), context, indent + INDENT);
  s << in << "</CastAs>" << endl;

  return s.str();
}

string PrintAST::printTreatAs(const XQTreatAs *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<TreatAs>" << endl;
  s << printASTNode(item->getExpression(), context, indent + INDENT);
  s << printSequenceType(item->getSequenceType(), context, indent + INDENT);
  s << in << "</TreatAs>" << endl;

  return s.str();
}

string PrintAST::printUserFunction(const XQUserFunction::XQFunctionEvaluator *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  const XMLCh *funUri = item->getFunctionURI();
  const XMLCh *funName = item->getFunctionName();

  string name("{");
  name += UTF8(funUri);
  name += "}:";
  name += UTF8(funName);

  const VectorOfASTNodes &args = item->getArguments();
  s << in << "<UserFunction name=\"" << name << "\">" << endl;

  if(item->getFunctionDefinition()->getParams()) {
    XQUserFunction::VectorOfFunctionParameters::const_iterator binding = item->getFunctionDefinition()->getParams()->begin();
    for(VectorOfASTNodes::const_iterator arg = args.begin(); arg != args.end() && binding != item->getFunctionDefinition()->getParams()->end(); ++arg, ++binding) {
      if((*binding)->_qname) {
        s << in << "  <Binding name=\"{" << UTF8((*binding)->_uri) << "}:" << UTF8((*binding)->_name) << "\">" << endl;
        s << printASTNode(*arg, context, indent + INDENT + INDENT);
        s << in << "  </Binding>" << endl;
      }
    }
  }

  // We don't output the body, as it may result in an infinite loop
  // for recursive user functions - jpcs
//   s << in << "  <Body>" << endl;
//   s << printASTNode(item->getFunctionDefinition()->getFunctionBody(), context, indent + INDENT + INDENT);
//   s << in << "  </Body>" << endl;
  s << in << "</UserFunction>" << endl;

  return s.str();
}

string PrintAST::printDebugHook(const XQDebugHook *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));
  string functionName(UTF8(item->getFunctionName()));
  string file(UTF8(item->getFile()));

  s << in << "<DebugHook function=\"" << functionName
    << "\" file=\"" << file << ":" << item->getLine() << ":"
    << item->getColumn() << "\">" << endl;
  s << printASTNode(item->m_impl, context, indent + INDENT);
  s << in << "</DebugHook>" << endl;

  return s.str();
}

string PrintAST::printFLWOR(const XQFLWOR *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));
  const VectorOfVariableBinding *bindings = item->getBindings();
  const ASTNode *where = item->getWhereExpr();
  const XQSort *sort = item->getSort();

  s << in << "<FLWOR>" << endl;
  for(VectorOfVariableBinding::const_iterator i = bindings->begin(); i != bindings->end(); ++i) {
    s << printXQVariableBinding(*i, context, indent + INDENT);
  }
  if(where) {
    s << in << "  <Where>" << endl;
    s << printASTNode(where, context, indent + INDENT + INDENT);
    s << in << "  </Where>" << endl;
  }
  if(sort) s << printSort(sort, context, indent + INDENT);
  s << printASTNode(item->getReturnExpr(), context, indent + INDENT);
  s << in << "</FLWOR>" << endl;

  return s.str();
}

string PrintAST::printXQVariableBinding(const XQVariableBinding *binding, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));
  string type;
  if(binding->_bindingType == XQVariableBinding::letBinding) {
    type = "LetBinding";
  }
  else {
    type = "ForBinding";
  }

  s << in << "<" << type << " name=\"" << UTF8(binding->_variable);
  if(binding->_positionalVariable) {
    s << "\" at=\"" << UTF8(binding->_positionalVariable);
  }
  s << "\">" << endl;
  s << printASTNode(binding->_allValues, context, indent + INDENT);
  if(binding->_where) {
    s << in << "  <Where>" << endl;
    s << printASTNode(binding->_where, context, indent + INDENT + INDENT);
    s << in << "  </Where>" << endl;
  }
  s << in << "</" << type << ">" << endl;

  return s.str();
}

string PrintAST::printSort(const XQSort *sort, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));
  string type;
  if(sort->getSortType() == XQSort::stable) {
    type = "StableSort";
  }
  else {
    type = "Sort";
  }

  s << in << "<" << type << ">" << endl;
  for(XQSort::VectorOfSortSpec::const_iterator it = sort->getSortSpecs()->begin();
      it != sort->getSortSpecs()->end(); ++it) {
    s << in << "  <Specification";
    if((*it)->getModifier()) {
      s << " modifier=\"";
      bool addBar = false;
      if((*it)->getModifier() & XQSort::SortSpec::ascending) {
        s << "ascending";
        addBar = true;
      }
      if((*it)->getModifier() & XQSort::SortSpec::descending) {
        if(addBar) s << "|";
        s << "descending";
        addBar = true;
      }
      if((*it)->getModifier() & XQSort::SortSpec::empty_greatest) {
        if(addBar) s << "|";
        s << "empty_greatest";
        addBar = true;
      }
      if((*it)->getModifier() & XQSort::SortSpec::empty_least) {
        if(addBar) s << "|";
        s << "empty_least";
      }
      s << "\"";
    }
    if((*it)->getCollation() != 0 && *((*it)->getCollation()) != 0) {
      s << " collation=\"" << UTF8((*it)->getCollation()) << "\"";
    }
    s << ">" << endl;
    s << printASTNode((*it)->getExpression(), context, indent + INDENT + INDENT);
    s << in << "  </Specification>" << endl;
  }
  s << in << "</" << type << ">" << endl;

  return s.str();
}

string PrintAST::printFLWORQuantified(const XQQuantified *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));
  XQQuantified::QuantifierType type = item->getQuantifierType();
  string name(type == XQQuantified::some ? "SomeFLWOR" : "EveryFLWOR");

  const VectorOfVariableBinding *bindings = item->getBindings();

  s << in << "<" << name << ">" << endl;
  for(VectorOfVariableBinding::const_iterator i = bindings->begin(); i != bindings->end(); ++i) {
    s << printXQVariableBinding(*i, context, indent + INDENT);
  }
  s << printASTNode(item->getReturnExpr(), context, indent + INDENT);
  s << in << "</" << name << ">" << endl;

  return s.str();
}

string PrintAST::printTypeswitch(const XQTypeswitch *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  const XQTypeswitch::VectorOfClause *clauses = item->getClauses();

  s << in << "<TypeSwitch>" << endl;
  s << printASTNode(item->getExpression(), context, indent + INDENT);
  for(XQTypeswitch::VectorOfClause::const_iterator i = clauses->begin(); i != clauses->end(); ++i) {
    s << printClause(*i, context, indent + INDENT);
  }
  s << printClause(item->getDefaultClause(), context, indent + INDENT);
  s << in << "</TypeSwitch>" << endl;

  return s.str();
}

string PrintAST::printClause(const XQTypeswitch::Clause *clause, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  if(clause->_type) {
    s << in << "<Clause";
    if(clause->_variable) {
      s << " name=\"" << UTF8(clause->_variable) << "\"";
    }
    s << ">" << endl;
    s << printSequenceType(clause->_type, context, indent + INDENT);
    s << printASTNode(clause->_expr, context, indent + INDENT);
    s << in << "</Clause>" << endl;
  }
  else {
    s << in << "<Default";
    if(clause->_variable) {
      s << " name=\"" << UTF8(clause->_variable) << "\"";
    }
    s << ">" << endl;
    s << printASTNode(clause->_expr, context, indent + INDENT);
    s << in << "</Default>" << endl;
  }

  return s.str();
}

string PrintAST::printValidate(const XQValidate *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<Validate mode=\"";
  switch(item->getValidationMode()) {
  case DocumentCache::VALIDATION_STRICT: {
    s << "strict";
    break;
  }
  case DocumentCache::VALIDATION_LAX: {
    s << "lax";
    break;
  }
  }
  s << "\">" << endl;
  s << printASTNode(item->getExpression(), context, indent + INDENT);
  s << in << "</Validate>" << endl;

  return s.str();
}

string PrintAST::printGlobal(const XQGlobalVariable *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<GlobalVar name=\"" << UTF8(item->getVariableName());
  if(item->isExternal()) {
    s << " external=\"true\"";
  }
  if(item->getSequenceType() || item->getVariableExpr()) {
    s << ">" << endl;
    if(item->getSequenceType()) {
      s << printSequenceType(item->getSequenceType(), context, indent + INDENT);
    }
    if(item->getVariableExpr()) {
      s << printASTNode(item->getVariableExpr(), context, indent + INDENT);
    }
    s << in << "</GlobalVar>" << endl;
  }
  else {
    s << "/>" << endl;
  }

  return s.str();
}

string PrintAST::printFunctionCall(const XQFunctionCall *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  const XMLCh *funPre = item->getName()->getPrefix();
  const XMLCh *funName = item->getName()->getName();

  string name;
  name += UTF8(funPre);
  name += ":";
  name += UTF8(funName);

  const VectorOfASTNodes &args = item->getArguments();
  if(args.empty()) {
    s << in << "<FunctionCall name=\"" << name << "\"/>" << endl;
  }
  else {
    s << in << "<FunctionCall name=\"" << name << "\">" << endl;
    for(VectorOfASTNodes::const_iterator i = args.begin(); i != args.end(); ++i) {
      s << printASTNode(*i, context, indent + INDENT);
    }
    s << in << "</FunctionCall>" << endl;
  }

  return s.str();
}

string PrintAST::printDOMConstructor(const XQDOMConstructor *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<DOMConstructor type=\"" << UTF8(item->getNodeType());
  if(item->getName() ||
     (item->getAttributes() != 0 && !item->getAttributes()->empty()) ||
     (item->getChildren() != 0 && !item->getChildren()->empty())) {
    s << "\">" << endl;
    if(item->getName()) {
      s << in << "  <Name>" << endl;
      s << printASTNode(item->getName(), context, indent + INDENT + INDENT);
      s << in << "  </Name>" << endl;
    }
    if(item->getAttributes() != 0 && !item->getAttributes()->empty()) {
      s << in << "  <Attributes>" << endl;
      for(VectorOfASTNodes::const_iterator i = item->getAttributes()->begin();
          i != item->getAttributes()->end(); ++i) {
        s << printASTNode(*i, context, indent + INDENT + INDENT);
      }
      s << in << "  </Attributes>" << endl;
    }
    if(item->getChildren() != 0 && !item->getChildren()->empty()) {
      s << in << "  <Children>" << endl;
      for(VectorOfASTNodes::const_iterator i = item->getChildren()->begin();
          i != item->getChildren()->end(); ++i) {
        s << printASTNode(*i, context, indent + INDENT + INDENT);
      }
      s << in << "  </Children>" << endl;
    }
    if(item->getValue() != 0) {
      s << in << "  <Children>" << endl;
      s << printASTNode(item->getValue(), context, indent + INDENT + INDENT);
      s << in << "  </Children>" << endl;
    }
    s << in << "</DOMConstructor>" << endl;
  }
  else {
    s << "\"/>" << endl;
  }

  return s.str();
}

string PrintAST::printOrderingChange(const XQOrderingChange *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<OrderingChange value=\"";
  if(item->getOrderingValue() == StaticContext::ORDERING_ORDERED)
    s << "ordered";
  else s << "unordered";
  s << "\">" << endl;
  s << printASTNode(item->getExpr(), context, indent + INDENT);
  s << in << "</OrderingChange>" << endl;

  return s.str();
}

string PrintAST::printFTContains(const FTContains *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<FTContains>" << endl;
  s << printASTNode(item->getArgument(), context, indent + INDENT);
  s << printFTSelection(item->getSelection(), context, indent + INDENT);
  if(item->getIgnore() != NULL) {
    s << in << "  <ignore>" << endl;
    s << printASTNode(item->getIgnore(), context, indent + INDENT + INDENT);
    s << in << "  </ignore>" << endl;
  }
  s << in << "</FTContains>" << endl;

  return s.str();
}

string PrintAST::printFTSelection(const FTSelection *selection, const DynamicContext *context, int indent)
{
  switch(selection->getType()) {
  case FTSelection::OR: {
    return printFTOr((FTOr*)selection, context, indent);
    break;
  }
  case FTSelection::AND: {
    return printFTAnd((FTAnd*)selection, context, indent);
    break;
  }
  case FTSelection::MILD_NOT: {
    return printFTMildnot((FTMildnot*)selection, context, indent);
    break;
  }
  case FTSelection::UNARY_NOT: {
    return printFTUnaryNot((FTUnaryNot*)selection, context, indent);
    break;
  }
  case FTSelection::WORDS: {
    return printFTWords((FTWords*)selection, context, indent);
    break;
  }
  case FTSelection::WORD: {
    return printFTWord((FTWord*)selection, context, indent);
    break;
  }
  case FTSelection::ORDER: {
    return printFTOrder((FTOrder*)selection, context, indent);
    break;
  }
  case FTSelection::DISTANCE: {
    return printFTDistance((FTDistance*)selection, context, indent);
    break;
  }
  case FTSelection::DISTANCE_LITERAL: {
    return printFTDistanceLiteral((FTDistanceLiteral*)selection, context, indent);
    break;
  }
  case FTSelection::SCOPE: {
    return printFTScope((FTScope*)selection, context, indent);
    break;
  }
  case FTSelection::CONTENT: {
    return printFTContent((FTContent*)selection, context, indent);
    break;
  }
  case FTSelection::WINDOW: {
    return printFTWindow((FTWindow*)selection, context, indent);
    break;
  }
  case FTSelection::WINDOW_LITERAL: {
    return printFTWindowLiteral((FTWindowLiteral*)selection, context, indent);
    break;
  }
  default:
    break;
  }
  return getIndent(indent) + "<Unknown/>\n";
}

string PrintAST::printFTWords(const FTWords *selection, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<FTWords option=\"";
  switch(selection->getOption()) {
  case FTWords::ANY: s << "any"; break;
  case FTWords::ANY_WORD: s << "any word"; break;
  case FTWords::ALL: s << "all"; break;
  case FTWords::ALL_WORDS: s << "all words"; break;
  case FTWords::PHRASE: s << "phrase"; break;
  }
  s << "\">" << endl;
  s << printASTNode(selection->getExpr(), context, indent + INDENT);
  s << in << "</FTWords>" << endl;

  return s.str();
}

string PrintAST::printFTWord(const FTWord *selection, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<FTWord>" << UTF8(selection->getQueryString()) << "</FTWord>" << endl;

  return s.str();
}

string PrintAST::printFTOr(const FTOr *selection, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<FTOr>" << endl;
  for(VectorOfFTSelections::const_iterator i = selection->getArguments().begin();
      i != selection->getArguments().end(); ++i) {
    s << printFTSelection(*i, context, indent + INDENT);
  }
  s << in << "</FTOr>" << endl;

  return s.str();
}

string PrintAST::printFTAnd(const FTAnd *selection, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<FTAnd>" << endl;
  for(VectorOfFTSelections::const_iterator i = selection->getArguments().begin();
      i != selection->getArguments().end(); ++i) {
    s << printFTSelection(*i, context, indent + INDENT);
  }
  s << in << "</FTAnd>" << endl;

  return s.str();
}

string PrintAST::printFTMildnot(const FTMildnot *selection, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<FTMildnot>" << endl;
  s << printFTSelection(selection->getLeft(), context, indent + INDENT);
  s << printFTSelection(selection->getRight(), context, indent + INDENT);
  s << in << "</FTMildnot>" << endl;

  return s.str();
}

string PrintAST::printFTUnaryNot(const FTUnaryNot *selection, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<FTUnaryNot>" << endl;
  s << printFTSelection(selection->getArgument(), context, indent + INDENT);
  s << in << "</FTUnaryNot>" << endl;

  return s.str();
}

string PrintAST::printFTOrder(const FTOrder *selection, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<FTOrder>" << endl;
  s << printFTSelection(selection->getArgument(), context, indent + INDENT);
  s << in << "</FTOrder>" << endl;

  return s.str();
}

string PrintAST::printFTDistance(const FTDistance *selection, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<FTDistance units=\"" << getFTUnitName(selection->getUnit()) << "\">" << endl;
  s << printFTSelection(selection->getArgument(), context, indent + INDENT);
  s << printFTRange(selection->getRange(), context, indent + INDENT);
  s << in << "</FTDistance>" << endl;

  return s.str();
}

string PrintAST::printFTDistanceLiteral(const FTDistanceLiteral *selection, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<FTDistanceLiteral type=\"" << getFTRangeTypeName(selection->getType())
    << "\" distance=\"" << selection->getDistance();
  if(selection->getType() == FTRange::FROM_TO) {
    s << "\" distance2=\"" << selection->getDistance2();
  }
  s << "\" units=\"" << getFTUnitName(selection->getUnit()) << "\">" << endl;
  s << printFTSelection(selection->getArgument(), context, indent + INDENT);
  s << in << "</FTDistanceLiteral>" << endl;

  return s.str();
}

string PrintAST::printFTScope(const FTScope *selection, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<FTScope type=\"";
  switch(selection->getType()) {
  case FTScope::SAME: s << "same"; break;
  case FTScope::DIFFERENT: s << "different"; break;
  }
  s << "\" units=\"" << getFTUnitName(selection->getUnit()) << "\">" << endl;
  s << printFTSelection(selection->getArgument(), context, indent + INDENT);
  s << in << "</FTScope>" << endl;

  return s.str();
}

string PrintAST::printFTContent(const FTContent *selection, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<FTContent type=\"";
  switch(selection->getType()) {
  case FTContent::AT_START: s << "at start"; break;
  case FTContent::AT_END: s << "at end"; break;
  case FTContent::ENTIRE_CONTENT: s << "entire content"; break;
  }
  s << "\">" << endl;
  s << printFTSelection(selection->getArgument(), context, indent + INDENT);
  s << in << "</FTContent>" << endl;

  return s.str();
}

string PrintAST::printFTWindow(const FTWindow *selection, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<FTWindow units=\"" << getFTUnitName(selection->getUnit()) << "\">" << endl;
  s << printFTSelection(selection->getArgument(), context, indent + INDENT);
  s << printASTNode(selection->getExpr(), context, indent + INDENT);
  s << in << "</FTWindow>" << endl;

  return s.str();
}

string PrintAST::printFTWindowLiteral(const FTWindowLiteral *selection, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<FTWindowLiteral distance=\"" << selection->getDistance();
  s << "\" units=\"" << getFTUnitName(selection->getUnit()) << "\">" << endl;
  s << printFTSelection(selection->getArgument(), context, indent + INDENT);
  s << in << "</FTWindowLiteral>" << endl;

  return s.str();
}

string PrintAST::printFTRange(const FTRange &range, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<FTRange type=\"" << getFTRangeTypeName(range.type) << "\">" << endl;
  s << printASTNode(range.arg1, context, indent + INDENT);
  if(range.arg2 != 0)
    s << printASTNode(range.arg2, context, indent + INDENT);
  s << in << "</FTRange>" << endl;

  return s.str();
}

string PrintAST::getFTRangeTypeName(FTRange::Type type)
{
  switch(type) {
  case FTRange::EXACTLY: return "exactly";
  case FTRange::AT_LEAST: return "at least";
  case FTRange::AT_MOST: return "at most";
  case FTRange::FROM_TO: return "from to";
  }
  return "unknown";
}

string PrintAST::getFTUnitName(FTOption::FTUnit unit)
{
  switch(unit) {
  case FTOption::WORDS: return "words";
  case FTOption::SENTENCES: return "sentences";
  case FTOption::PARAGRAPHS: return "paragraphs";
  }
  return "unknown";
}

string PrintAST::printAtomize(const XQAtomize *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<Atomize>" << endl;
  s << printASTNode(item->getExpression(), context, indent + INDENT);
  s << in << "</Atomize>" << endl;

  return s.str();
}

string PrintAST::printDocumentOrder(const XQDocumentOrder *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));
  string name(item->getUnordered() ? "UniqueNodes" : "DocumentOrder");

  s << in << "<" << name << ">" << endl;
  s << printASTNode(item->getExpression(), context, indent + INDENT);
  s << in << "</" << name << ">" << endl;

  return s.str();
}

string PrintAST::printXPath1CompatConvertFunctionArg(const XPath1CompatConvertFunctionArg *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<XPath1CompatConvertFunctionArg>" << endl;
  s << printASTNode(item->getExpression(), context, indent + INDENT);
  s << printSequenceType(item->getSequenceType(), context, indent + INDENT);
  s << in << "</XPath1CompatConvertFunctionArg>" << endl;

  return s.str();
}

string PrintAST::printPromoteUntyped(const XQPromoteUntyped *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<PromoteUntyped uri=\"" << UTF8(item->getTypeURI())
    << "\" name=\"" << UTF8(item->getTypeName())
    << "\">" << endl;
  s << printASTNode(item->getExpression(), context, indent + INDENT);
  s << in << "</PromoteUntyped>" << endl;

  return s.str();
}

string PrintAST::printPromoteNumeric(const XQPromoteNumeric *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<PromoteNumeric uri=\"" << UTF8(item->getTypeURI())
    << "\" name=\"" << UTF8(item->getTypeName())
    << "\">" << endl;
  s << printASTNode(item->getExpression(), context, indent + INDENT);
  s << in << "</PromoteNumeric>" << endl;

  return s.str();
}

string PrintAST::printPromoteAnyURI(const XQPromoteAnyURI *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<PromoteAnyURI uri=\"" << UTF8(item->getTypeURI())
    << "\" name=\"" << UTF8(item->getTypeName())
    << "\">" << endl;
  s << printASTNode(item->getExpression(), context, indent + INDENT);
  s << in << "</PromoteAnyURI>" << endl;

  return s.str();
}

string PrintAST::printPredicate(const XQPredicate *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<Predicate>" << endl; 
  s << printASTNode(item->getExpression(), context, indent + INDENT);
  s << printASTNode(item->getPredicate(), context, indent + INDENT);
  s << in << "</Predicate>" << endl;

  return s.str();
}

string PrintAST::printSequenceType(const SequenceType *type, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<SequenceType";
  s << " occurrence=\"" << getOccurrenceIndicatorName(type->getOccurrenceIndicator()) << "\"";
  s << printItemTypeAttrs(type->getItemType(), context);
  s << "/>" << endl;

  return s.str();
}

string PrintAST::printItemTypeAttrs(const SequenceType::ItemType *type, const DynamicContext *context)
{
  if(type == 0) return " testType=\"empty\"";

  ostringstream s;

  s << " testType=\"" << getItemTestTypeName(type->getItemTestType()) << "\"";
  
  if(type->getAllowNilled()) {
    s << " nil=\"true\"";
  }

  try {
    const XMLCh *nameURI = type->getNameURI((DynamicContext *)context, 0);
    if(type->getName() != 0) {
      s << " name=\"{" << UTF8(nameURI)
        << "}:" << UTF8(type->getName()->getName()) << "\"";
    }
    else if(nameURI != 0) {
      s << " nameURI=\"" << UTF8(nameURI) << "\"";
    }
  }
  catch(const NamespaceLookupException &e) {
    if(type->getName() != 0) {
      s << " name=\"" << UTF8(type->getName()->getPrefix())
        << ":" << UTF8(type->getName()->getName()) << "\"";
    }
  }

  try {
    const XMLCh *typeURI = type->getTypeURI((DynamicContext *)context, 0);
    if(type->getType() != 0) {
      s << " type=\"" << UTF8(typeURI)
        << ":" << UTF8(type->getType()->getName()) << "\"";
    }
    else if(typeURI != 0) {
      s << " typeURI=\"" << UTF8(typeURI) << "\"";
    }
  }
  catch(const NamespaceLookupException &e) {
    if(type->getType() != 0) {
      s << " type=\"" << UTF8(type->getType()->getPrefix())
        << ":" << UTF8(type->getType()->getName()) << "\"";
    }
  }

  return s.str();
}

string PrintAST::printItem(const Item::Ptr item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));
  string value(UTF8(item->asString(context)));
  s << in << "<Item value=\"" << value << "\" type=\"";
  if(item->isNode()) {
    s << "node";
  }
  else {
    const AnyAtomicType::Ptr att = (const AnyAtomicType::Ptr )item;
    s << "{" << UTF8(att->getTypeURI()) << "}:"
      << UTF8(att->getTypeName());
  }
  s << "\"/>" << endl;

  return s.str();
}

string PrintAST::getIndent(int indent)
{
  ostringstream s;

  for(int i = indent; i != 0; --i) {
    s << "  ";
  }

  return s.str();
}

string PrintAST::getAxisName(XQStep::Axis axis)
{
  switch(axis) {
  case XQStep::ANCESTOR: {
    return "ancestor";
  }
  case XQStep::ANCESTOR_OR_SELF: {
    return "ancestor-or-self";
  }
  case XQStep::ATTRIBUTE: {
    return "attribute";
  }
  case XQStep::CHILD: {
    return "child";
  }
  case XQStep::DESCENDANT: {
    return "descendant";
  }
  case XQStep::DESCENDANT_OR_SELF: {
    return "descendant-or-self";
  }
  case XQStep::FOLLOWING: {
    return "following";
  }
  case XQStep::FOLLOWING_SIBLING: {
    return "following-sibling";
  }
  case XQStep::NAMESPACE: {
    return "namespace";
  }
  case XQStep::PARENT: {
    return "parent";
  }
  case XQStep::PRECEDING: {
    return "preceding";
  }
  case XQStep::PRECEDING_SIBLING: {
    return "preceding-sibling";
  }
  case XQStep::SELF: {
    return "self";
  }
  default: {
    return "unknown";
  }
  }
}

string PrintAST::getItemTestTypeName(int type)
{
  switch(type) {
  case SequenceType::ItemType::TEST_ELEMENT: {
    return "element";
  }
  case SequenceType::ItemType::TEST_ATTRIBUTE: {
    return "attribute";
  }
  case SequenceType::ItemType::TEST_SCHEMA_ELEMENT: {
    return "schema-element";
  }
  case SequenceType::ItemType::TEST_SCHEMA_ATTRIBUTE: {
    return "schema-attribute";
  }
  case SequenceType::ItemType::TEST_NODE: {
    return "node";
  }
  case SequenceType::ItemType::TEST_PI: {
    return "processing-instruction";
  }
  case SequenceType::ItemType::TEST_COMMENT: {
    return "comment";
  }
  case SequenceType::ItemType::TEST_TEXT: {
    return "text";
  }
  case SequenceType::ItemType::TEST_DOCUMENT: {
    return "document";
  }
  case SequenceType::ItemType::TEST_ANYTHING: {
    return "anything";
  }
  case SequenceType::ItemType::TEST_ATOMIC_TYPE: {
    return "atomic-type";
  }
  default: {
    return "unknown";
  }
  }
}

string PrintAST::getOccurrenceIndicatorName(SequenceType::OccurrenceIndicator oi)
{
  switch(oi) {
  case SequenceType::EXACTLY_ONE: {
    return "exactly_one";
  }
  case SequenceType::STAR: {
    return "zero_to_many";
  }
  case SequenceType::PLUS: {
    return "one_to_many";
  }
  case SequenceType::QUESTION_MARK: {
    return "zero_or_one";
  }
  default: {
    return "unknown";
  }
  }
}

string PrintAST::getComparisonOperationName(GeneralComp::ComparisonOperation co)
{
  switch(co) {
  case GeneralComp::EQUAL: {
    return "equal";
  }
  case GeneralComp::NOT_EQUAL: {
    return "not_equal";
  }
  case GeneralComp::LESS_THAN: {
    return "less_than";
  }
  case GeneralComp::LESS_THAN_EQUAL: {
    return "less_than_equal";
  }
  case GeneralComp::GREATER_THAN: {
    return "greater_than";
  }
  case GeneralComp::GREATER_THAN_EQUAL: {
    return "greater_than_equal";
  }
  default: {
    return "unknown";
  }
  }
}
