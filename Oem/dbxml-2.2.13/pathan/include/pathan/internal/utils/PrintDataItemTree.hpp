//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2004
//	Sleepycat Software.  All rights reserved.
//
// $Id: PrintDataItemTree.hpp,v 1.3 2005/11/21 22:22:55 gmf Exp $
//

#ifndef __PRINTDATAITEMTREE_HPP
#define	__PRINTDATAITEMTREE_HPP

#include <string>

#include <pathan/Pathan.hpp>
#include <pathan/dataItem/DataItem.hpp>
#include <pathan/internal/navigation/NodeTest.hpp>
#include <pathan/SequenceType.hpp>
#include <pathan/operators/GeneralComp.hpp>
#include <pathan/dataItem/DataItemFor.hpp>

class PathanExpression;
class DataItemFunction;
class DataItemImpl;
class DataItemLiteral;
class DataItemNav;
class DataItemParenthesizedExpr;
class DataItemSequence;
class DataItemStep;
class DataItemVariable;
class DataItemIf;
class DataItemQuantifiedExpr;
class DataItemInstanceOf;
class DataItemCastableAs;
class DataItemCastAs;
class DataItemTreatAs;
class DataItemOperator;
class DataItemContextItem;
class DynamicContext;
class Item;

class PATHAN_EXPORT PrintDataItemTree
{
public:
  /** do-nothing virtual dtor */	
  virtual ~PrintDataItemTree() {}

  static std::string print(const PathanExpression *expr, const DynamicContext *context, int indent = 0);
  static std::string print(const DataItem *item, const DynamicContext *context, int indent = 0);

  virtual std::string printDataItem(const DataItem *item, const DynamicContext *context, int indent = 0);
  virtual std::string printFunction(const DataItemFunction *item, const DynamicContext *context, int indent);
  virtual std::string printLiteral(const DataItemLiteral *item, const DynamicContext *context, int indent);
  virtual std::string printNav(const DataItemNav *item, const DynamicContext *context, int indent);
  virtual std::string printParenthesized(const DataItemParenthesizedExpr *item, const DynamicContext *context, int indent);
  virtual std::string printSequence(const DataItemSequence *item, const DynamicContext *context, int indent);
  virtual std::string printStep(const DataItemStep *item, const DynamicContext *context, int indent);
  virtual std::string printVariable(const DataItemVariable *item, const DynamicContext *context, int indent);
  virtual std::string printIf(const DataItemIf *item, const DynamicContext *context, int indent);
  virtual std::string printFor(const DataItemFor *item, const DynamicContext *context, int indent);
  virtual std::string printQuantified(const DataItemQuantifiedExpr *item, const DynamicContext *context, int indent);
  virtual std::string printInstanceOf(const DataItemInstanceOf *item, const DynamicContext *context, int indent);
  virtual std::string printCastableAs(const DataItemCastableAs *item, const DynamicContext *context, int indent);
  virtual std::string printCastAs(const DataItemCastAs *item, const DynamicContext *context, int indent);
  virtual std::string printTreatAs(const DataItemTreatAs *item, const DynamicContext *context, int indent);
  virtual std::string printOperator(const DataItemOperator *item, const DynamicContext *context, int indent);
  virtual std::string printContextItem(const DataItemContextItem *item, const DynamicContext *context, int indent);

  virtual std::string printItem(const Item::Ptr item, const DynamicContext *context, int indent);
  virtual std::string printVariableBinding(const DataItemFor::VB *binding, const DynamicContext *context, int indent);
  virtual std::string printSequenceType(const SequenceType *type, const DynamicContext *context, int indent);
  virtual std::string printItemTypeAttrs(const SequenceType::ItemType *type, const DynamicContext *context);
  virtual std::string printPredicates(const DataItemImpl *item, const DynamicContext *context, int indent);

  static bool hasPredicates(const DataItemImpl *item);
  static std::string getIndent(int indent);
  static std::string getAxisName(DataItemStep::Axis axis);
  static std::string getItemTestTypeName(int type);
  static std::string getOccurrenceIndicatorName(SequenceType::OccurrenceIndicator oi);
  static std::string getComparisonOperationName(GeneralComp::ComparisonOperation co);
};

#endif
