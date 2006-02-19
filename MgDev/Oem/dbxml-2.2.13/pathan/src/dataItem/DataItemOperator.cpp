/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <assert.h>
#include <xercesc/validators/schema/SchemaSymbols.hpp>
#include <pathan/dataItem/DataItemOperator.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/exceptions/XPath2TypeCastException.hpp>
#include <pathan/AnyAtomicType.hpp>
#include <pathan/ATBooleanOrDerived.hpp>
#include <pathan/Node.hpp>
#include <pathan/internal/factory/DatatypeFactory.hpp>
#include <pathan/SequenceType.hpp>

DataItemOperator::DataItemOperator(const XMLCh* opName, const VectorOfDataItems &args, XPath2MemoryManager* memMgr)
  : DataItemImpl(memMgr),
  _args(args)
{
  _opName=opName;
  setType(DataItem::OPERATOR);
}

void DataItemOperator::addArgument(DataItem* arg)
{
	_args.push_back(arg);
}

DataItem* DataItemOperator::getArgument(unsigned int index) const
{
	assert(index<_args.size());
	return _args[index];
}

void DataItemOperator::setArgument(unsigned int index, DataItem *arg)
{
	assert(index<_args.size());
	_args[index] = arg;
}

void DataItemOperator::removeArgument(unsigned int index)
{
	assert(index<_args.size());
	_args.erase(_args.begin() + index);
}

unsigned int DataItemOperator::getNumArgs() const
{
  return _args.size();
}

const XMLCh* DataItemOperator::getOperatorName() const
{
  return _opName;
}

Node::Ptr DataItemOperator::getNodeParam(unsigned int index, DynamicContext* context) const
{
	assert(_args.size() > index);
	// The result of a node comparison is defined by applying the following rules, in order:

	// 1. Each operand must be either a single node or an empty sequence; otherwise a type error is raised.
  Result arg_result(_args[index]->collapseTree(context, DataItem::UNORDERED|DataItem::RETURN_TWO));
  const Item::Ptr first = arg_result.next(context);

  if(first == NULLRCP) return 0;

  if(!first->isNode()) {
		DSLthrow(XPath2TypeCastException,X("DataItemOperator::getNodeParam"), X("A parameter of the operator is not a node or empty [err:XP0006]"));
  }

  const Item::Ptr second = arg_result.next(context);
  if(second != NULLRCP) {
		DSLthrow(XPath2TypeCastException,X("DataItemOperator::getNodeParam"), X("A parameter of the operator is not a single node or empty [err:XP0006]"));
  }

	return (const Node::Ptr )first;
}

bool DataItemOperator::checkSequenceIsNodes(const Sequence &s) {
	for(Sequence::const_iterator cur = s.begin(); cur != s.end(); ++cur) {
		if(!(*cur)->isNode()) {
			return false;
		}
	}
	return true;
}

DataItem* DataItemOperator::staticResolution(StaticContext *context) {
  return resolveDataItemsForDateOrTime(_args, context, true);
}

const VectorOfDataItems &DataItemOperator::getArguments() const {
  return _args;
}
