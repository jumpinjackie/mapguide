//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2004
//	Sleepycat Software.  All rights reserved.
//
// $Id: PrintDataItemTree.cpp,v 1.13 2005/07/19 16:48:33 jsnelson Exp $
//

#include "../config/pathan_config.h"
#include <iostream>
#include <sstream>

#include <pathan/internal/utils/PrintDataItemTree.hpp>
#include <pathan/internal/utils/UTF8Str.hpp>

#include <pathan/internal/dom-extensions/PathanExpressionImpl.hpp>
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
#include <pathan/dataItem/DataItemQuantifiedExpr.hpp>
#include <pathan/dataItem/DataItemInstanceOf.hpp>
#include <pathan/dataItem/DataItemCastableAs.hpp>
#include <pathan/dataItem/DataItemCastAs.hpp>
#include <pathan/dataItem/DataItemTreatAs.hpp>
#include <pathan/dataItem/DataItemOperator.hpp>
#include <pathan/dataItem/DataItemContextItem.hpp>
#include <pathan/SequenceType.hpp>
#include <pathan/internal/parser/QName.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/Item.hpp>
#include <pathan/AnyAtomicType.hpp>
#include <pathan/internal/DOMutils/XStr.hpp>
#include <pathan/exceptions/NamespaceLookupException.hpp>
#include <pathan/operators/GeneralComp.hpp>

#include <xercesc/dom/DOMNode.hpp>
#if defined(XERCES_HAS_CPP_NAMESPACE)
  XERCES_CPP_NAMESPACE_USE
#endif

using namespace std;

static const int INDENT = 1;

string PrintDataItemTree::print(const PathanExpression *expr, const DynamicContext *context, int indent)
{
  return print(((const PathanExpressionImpl *)expr)->getCompiledExpression(), context, indent);
}

string PrintDataItemTree::print(const DataItem *item, const DynamicContext *context, int indent)
{
  PrintDataItemTree p;
  return p.printDataItem(item, context, indent);
}

string PrintDataItemTree::printDataItem(const DataItem *item, const DynamicContext *context, int indent)
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
  default: {
    return getIndent(indent) + "<Unknown/>\n";
    break;
  }
  }
}

string PrintDataItemTree::printFunction(const DataItemFunction *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  const XMLCh *funUri = item->getFunctionURI();
  const XMLCh *funName = item->getFunctionName();

  string name("{");
  name += UTF8(funUri);
  name += "}:";
  name += UTF8(funName);

  const VectorOfDataItems &args = item->getArguments();
  if(args.empty() && !hasPredicates(item)) {
    s << in << "<Function name=\"" << name << "\"/>" << endl;
  }
  else {
    s << in << "<Function name=\"" << name << "\">" << endl;
    for(VectorOfDataItems::const_iterator i = args.begin(); i != args.end(); ++i) {
      s << printDataItem(*i, context, indent + INDENT);
    }
    s << printPredicates(item, context, indent + INDENT);
    s << in << "</Function>" << endl;
  }

  return s.str();
}

string PrintDataItemTree::printOperator(const DataItemOperator *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));
  string name(UTF8(item->getOperatorName()));
  if(name == "comp") {
    name = getComparisonOperationName(((const GeneralComp *)item)->getOperation());
  }

  const VectorOfDataItems &args = item->getArguments();
  if(args.empty() && !hasPredicates(item)) {
    s << in << "<Operator name=\"" << name << "\"/>" << endl;
  }
  else {
    s << in << "<Operator name=\"" << name << "\">" << endl;
    for(VectorOfDataItems::const_iterator i = args.begin(); i != args.end(); ++i) {
      s << printDataItem(*i, context, indent + INDENT);
    }
    s << printPredicates(item, context, indent + INDENT);
    s << in << "</Operator>" << endl;
  }

  return s.str();
}

string PrintDataItemTree::printContextItem(const DataItemContextItem *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  if(hasPredicates(item)) {    
    s << in << "<ContextItem>" << endl;
    s << printPredicates(item, context, indent + INDENT);
    s << in << "</ContextItem>" << endl;
  }
  else {
    s << in << "<ContextItem/>" << endl;
  }

  return s.str();
}

string PrintDataItemTree::printLiteral(const DataItemLiteral *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<Literal>" << endl;
  s << in << "  " << item->getItemConstructor()->asString(context) << endl;
  s << printPredicates(item, context, indent + INDENT);
  s << in << "</Literal>" << endl;

  return s.str();
}

string PrintDataItemTree::printNav(const DataItemNav *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));
  string gotoRoot(item->getGotoRootFirst() ? " gotoRoot=\"true\"" : "");

  const DataItemNav::Steps &steps = item->getSteps();

  if(steps.empty() && !hasPredicates(item)) {
    s << in << "<Navigation" << gotoRoot << "/>" << endl;
  }
  else {
    s << in << "<Navigation" << gotoRoot << ">" << endl;
    for(DataItemNav::Steps::const_iterator i = steps.begin(); i != steps.end(); ++i) {
      s << printDataItem(i->step, context, indent + INDENT);
    }
    s << printPredicates(item, context, indent + INDENT);
    s << in << "</Navigation>" << endl;
  }

  return s.str();
}

string PrintDataItemTree::printStep(const DataItemStep *item, const DynamicContext *context, int indent)
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

  if(hasPredicates(item) || type != 0) {
    s << ">" << endl;
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

string PrintDataItemTree::printParenthesized(const DataItemParenthesizedExpr *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));
  const VectorOfDataItems &children = item->getChildren();

  if(children.empty() && !hasPredicates(item)) {
    s << in << "<Parenthesized/>" << endl;
  }
  else {
    s << in << "<Parenthesized>" << endl;
    for(VectorOfDataItems::const_iterator i = children.begin(); i != children.end(); ++i) {
      s << printDataItem(*i, context, indent + INDENT);
    }
    s << printPredicates(item, context, indent + INDENT);
    s << in << "</Parenthesized>" << endl;
  }

  return s.str();
}

string PrintDataItemTree::printSequence(const DataItemSequence *item, const DynamicContext *context, int indent)
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
    s << printPredicates(item, context, indent + INDENT);
    s << in << "</Sequence>" << endl;
  }

  return s.str();
}

string PrintDataItemTree::printVariable(const DataItemVariable *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));
  string prefix(UTF8(item->getPrefix()));
  string name(UTF8(item->getName()));
  if(prefix != "") {
    name = prefix + ":" + name;
  }

  if(hasPredicates(item)) {    
    s << in << "<Variable name=\"" << name << "\">" << endl;
    s << printPredicates(item, context, indent + INDENT);
    s << in << "</Variable>" << endl;
  }
  else {
    s << in << "<Variable name=\"" << name << "\"/>" << endl;
  }

  return s.str();
}

string PrintDataItemTree::printIf(const DataItemIf *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<If>" << endl;
  s << in << "  <Test>" << endl;
  s << printDataItem(item->getTest(), context, indent + INDENT + INDENT);
  s << in << "  </Test>" << endl;
  s << in << "  <Then>" << endl;
  s << printDataItem(item->getWhenTrue(), context, indent + INDENT + INDENT);
  s << in << "  </Then>" << endl;
  s << in << "  <Else>" << endl;
  s << printDataItem(item->getWhenFalse(), context, indent + INDENT + INDENT);
  s << in << "  </Else>" << endl;
  s << printPredicates(item, context, indent + INDENT);
  s << in << "</If>" << endl;

  return s.str();
}

string PrintDataItemTree::printFor(const DataItemFor *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));
  const DataItemFor::Bindings &bindings = item->getBindings();

  s << in << "<For>" << endl;
  for(DataItemFor::Bindings::const_iterator i = bindings.begin(); i != bindings.end(); ++i) {
    s << printVariableBinding(*i, context, indent + INDENT);
  }
  s << printDataItem(item->getReturnExpr(), context, indent + INDENT);
  s << printPredicates(item, context, indent + INDENT);
  s << in << "</For>" << endl;

  return s.str();
}

string PrintDataItemTree::printQuantified(const DataItemQuantifiedExpr *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));
  DataItemQuantifiedExpr::QuantifierType type = item->getQuantifierType();
  string name(type == DataItemQuantifiedExpr::some ? "Some" : "Every");

  const DataItemFor::Bindings &bindings = item->getBindings();

  s << in << "<" << name << ">" << endl;
  for(DataItemFor::Bindings::const_iterator i = bindings.begin(); i != bindings.end(); ++i) {
    s << printVariableBinding(*i, context, indent + INDENT);
  }
  s << printDataItem(item->getReturnExpr(), context, indent + INDENT);
  s << printPredicates(item, context, indent + INDENT);
  s << in << "</" << name << ">" << endl;

  return s.str();
}

string PrintDataItemTree::printInstanceOf(const DataItemInstanceOf *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<InstanceOf>" << endl;
  s << printDataItem(item->getExpression(), context, indent + INDENT);
  s << printSequenceType(item->getSequenceType(), context, indent + INDENT);
  s << printPredicates(item, context, indent + INDENT);
  s << in << "</InstanceOf>" << endl;

  return s.str();
}

string PrintDataItemTree::printCastableAs(const DataItemCastableAs *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<CastableAs>" << endl;
  s << printDataItem(item->getExpression(), context, indent + INDENT);
  s << printSequenceType(item->getSequenceType(), context, indent + INDENT);
  s << printPredicates(item, context, indent + INDENT);
  s << in << "</CastableAs>" << endl;

  return s.str();
}

string PrintDataItemTree::printCastAs(const DataItemCastAs *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<CastAs>" << endl;
  s << printDataItem(item->getExpression(), context, indent + INDENT);
  s << printSequenceType(item->getSequenceType(), context, indent + INDENT);
  s << in << "</CastAs>" << endl;

  return s.str();
}

string PrintDataItemTree::printTreatAs(const DataItemTreatAs *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<TreatAs>" << endl;
  s << printDataItem(item->getExpression(), context, indent + INDENT);
  s << printSequenceType(item->getSequenceType(), context, indent + INDENT);
  s << printPredicates(item, context, indent + INDENT);
  s << in << "</TreatAs>" << endl;

  return s.str();
}

string PrintDataItemTree::printSequenceType(const SequenceType *type, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<SequenceType";
  s << " occurrence=\"" << getOccurrenceIndicatorName(type->getOccurrenceIndicator()) << "\"";
  s << printItemTypeAttrs(type->getItemType(), context);
  s << "/>" << endl;

  return s.str();
}

string PrintDataItemTree::printItemTypeAttrs(const SequenceType::ItemType *type, const DynamicContext *context)
{
  ostringstream s;

  s << " testType=\"" << getItemTestTypeName(type->getItemTestType()) << "\"";
  
  if(type->getAllowNilled()) {
    s << " nil=\"true\"";
  }

  try {
    const XMLCh *nameURI = type->getNameURI((DynamicContext *)context);
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
    const XMLCh *typeURI = type->getTypeURI((DynamicContext *)context);
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

string PrintDataItemTree::printVariableBinding(const DataItemFor::VB *binding, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<Binding name=\"{" << UTF8(binding->_uri) << "}:" << UTF8(binding->_name) << "\">" << endl;
  s << printDataItem(binding->_allValues, context, indent + INDENT);
  s << in << "</Binding>" << endl;

  return s.str();
}

string PrintDataItemTree::printItem(const Item::Ptr item, const DynamicContext *context, int indent)
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

bool PrintDataItemTree::hasPredicates(const DataItemImpl *item)
{
  return !item->getPredicates().empty();
}

string PrintDataItemTree::printPredicates(const DataItemImpl *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  const DataItem::Predicates &preds = item->getPredicates();

  if(!preds.empty()) {
    string in(getIndent(indent));
    s << in << "<Predicates>" << endl;
    
    for(DataItem::Predicates::const_iterator i = preds.begin(); i != preds.end(); ++i) {
      s << printDataItem(i->pred, context, indent + INDENT);
    }
    
    s << in << "</Predicates>" << endl;
  }

  return s.str();
}

string PrintDataItemTree::getIndent(int indent)
{
  ostringstream s;

  for(int i = indent; i != 0; --i) {
    s << "  ";
  }

  return s.str();
}

string PrintDataItemTree::getAxisName(DataItemStep::Axis axis)
{
  switch(axis) {
  case DataItemStep::ANCESTOR: {
    return "ancestor";
  }
  case DataItemStep::ANCESTOR_OR_SELF: {
    return "ancestor-or-self";
  }
  case DataItemStep::ATTRIBUTE: {
    return "attribute";
  }
  case DataItemStep::CHILD: {
    return "child";
  }
  case DataItemStep::DESCENDANT: {
    return "descendant";
  }
  case DataItemStep::DESCENDANT_OR_SELF: {
    return "descendant-or-self";
  }
  case DataItemStep::FOLLOWING: {
    return "following";
  }
  case DataItemStep::FOLLOWING_SIBLING: {
    return "following-sibling";
  }
  case DataItemStep::NAMESPACE: {
    return "namespace";
  }
  case DataItemStep::PARENT: {
    return "parent";
  }
  case DataItemStep::PRECEDING: {
    return "preceding";
  }
  case DataItemStep::PRECEDING_SIBLING: {
    return "preceding-sibling";
  }
  case DataItemStep::SELF: {
    return "self";
  }
  default: {
    return "unknown";
  }
  }
}

string PrintDataItemTree::getItemTestTypeName(int type)
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

string PrintDataItemTree::getOccurrenceIndicatorName(SequenceType::OccurrenceIndicator oi)
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

string PrintDataItemTree::getComparisonOperationName(GeneralComp::ComparisonOperation co)
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
