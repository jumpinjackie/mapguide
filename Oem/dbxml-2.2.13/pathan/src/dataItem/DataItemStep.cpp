/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include <sstream>

#include "../config/pathan_config.h"
#include <pathan/dataItem/DataItemStep.hpp>
#include <pathan/internal/navigation/NodeTest.hpp>
#include <pathan/Item.hpp>
#include <pathan/ATDecimalOrDerived.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/internal/factory/DatatypeFactory.hpp>
#include <pathan/dataItem/StaticResolutionContext.hpp>
#include <pathan/exceptions/TypeErrorException.hpp>
#include <pathan/internal/utils/PrintDataItemTree.hpp>

DataItemStep::DataItemStep(Axis axis, NodeTest* nodeTest, XPath2MemoryManager* memMgr)
	: DataItemImpl(memMgr),
    nodeTest_(nodeTest),
    axis_(axis)
{
  setType(DataItem::STEP);
}

DataItemStep::~DataItemStep()
{
	//no-op
}


DataItem* DataItemStep::staticResolution(StaticContext *context)
{
  unsigned int properties = 0;
  // properties depend on the axis of the step
  switch (axis_) {
  case SELF:
    properties |= StaticResolutionContext::ONENODE;
    // Fall through
  case CHILD:
  case ATTRIBUTE:
  case NAMESPACE:
    properties |= StaticResolutionContext::SUBTREE | StaticResolutionContext::PEER;
    break;
  case DESCENDANT:
  case DESCENDANT_OR_SELF:
    properties |= StaticResolutionContext::SUBTREE;
    break;
  case FOLLOWING_SIBLING:
  case PRECEDING_SIBLING:
    properties |= StaticResolutionContext::PEER;
    break;
  case PARENT:
    properties |= StaticResolutionContext::PEER | StaticResolutionContext::ONENODE;
    break;
  default:
    break;
  }
  properties |= StaticResolutionContext::DOCORDER | StaticResolutionContext::GROUPED | StaticResolutionContext::SAMEDOC;

  _src.setProperties(properties);
  _src.getStaticType().flags = StaticType::NODE_TYPE;
  _src.contextItemUsed(true);
  nodeTest_->staticResolution(context);
  return resolvePredicates(context);
}

Result DataItemStep::createResult(DynamicContext* context, int flags) const 
{
  return new StepResult(this, context);
}

bool DataItemStep::isForwardAxis(Axis axis)
{
  switch(axis) {
  case ANCESTOR:
  case ANCESTOR_OR_SELF:
  case PARENT:
  case PRECEDING:
  case PRECEDING_SIBLING:
    return false;

  case ATTRIBUTE:
  case CHILD:
  case DESCENDANT:
  case DESCENDANT_OR_SELF:
  case FOLLOWING:
  case FOLLOWING_SIBLING:
  case NAMESPACE:
  case SELF:
    return true;
  }
  return false;
}

Result DataItemStep::postPredicateResultHook(Result &result, DynamicContext* context, int flags) const
{
  // parent axis doesn't need reordering
  if(!(context->getNodeSetOrdering()==StaticContext::ORDERING_UNORDERED || flags & DataItem::UNORDERED) &&
     !isForwardAxis(axis_) && axis_ != PARENT) {
    // Reorder reverse axis results
    return result.sortIntoDocumentOrder(context);
  }
  else {
    return result;
  }
}

const NodeTest *DataItemStep::getNodeTest() const {
  return nodeTest_;
}

void DataItemStep::setNodeTest(NodeTest *nt) {
  nodeTest_ = nt;
}

DataItemStep::Axis DataItemStep::getAxis() const {
  return axis_;
}

void DataItemStep::setAxis(DataItemStep::Axis a) {
  axis_ = a;
}

DataItemStep::StepResult::StepResult(const DataItemStep *step, DynamicContext *context)
  : ResultImpl(context),
    toDo_(true),
    result_(0),
    step_(step)
{
}

Item::Ptr DataItemStep::StepResult::next(DynamicContext *context)
{
  if(toDo_) {
    toDo_ = false;

    Item::Ptr item = context->getContextItem();

    if(item == NULLRCP || !item->isNode()) {
      DSLthrow(TypeErrorException,X("DataItemStep::StepResult::next"), X("An attempt was made to perform an axis step when the Context Item was not a node [err:XPTY0020]"));
    }

    result_ = ((Node*)item.get())->getAxisResult(step_->getAxis(), step_->getNodeTest(), context);
  }

  return result_.next(context);
}

std::string DataItemStep::StepResult::asString(DynamicContext *context, int indent) const
{
  std::ostringstream oss;
  std::string in(getIndent(indent));

  oss << in << "<dataitemstep name=\"" << PrintDataItemTree::getAxisName(step_->getAxis()) << "\"/>" << std::endl;

  return oss.str();
}
