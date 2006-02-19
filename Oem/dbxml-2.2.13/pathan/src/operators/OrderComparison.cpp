/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <assert.h>
#include <sstream>

#include <pathan/operators/OrderComparison.hpp>
#include <pathan/Node.hpp>
#include <pathan/ATBooleanOrDerived.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/internal/factory/DatatypeFactory.hpp>
#include <pathan/PathanFactory.hpp>

/*static*/ const XMLCh OrderComparison::name[]={ XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_d, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chDash, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_d, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r, XERCES_CPP_NAMESPACE_QUALIFIER chNull };

OrderComparison::OrderComparison(const VectorOfDataItems &args, bool testBefore, XPath2MemoryManager* memMgr)
  : DataItemOperator(name, args, memMgr)
{
	_testBefore = testBefore;
}

bool OrderComparison::getTestBefore() const
{
    return _testBefore;
}

Result OrderComparison::createResult(DynamicContext* context, int flags) const
{
  return new OrderComparisonResult(this, context);
}

OrderComparison::OrderComparisonResult::OrderComparisonResult(const OrderComparison *op, DynamicContext *context)
  : SingleResult(context),
    _op(op)
{
}

Item::Ptr OrderComparison::OrderComparisonResult::getSingleResult(DynamicContext *context) const
{
	const Node::Ptr arg1 = _op->getNodeParam(0,context);
  if(arg1 == NULLRCP) return 0;
	const Node::Ptr arg2 = _op->getNodeParam(1,context);
  if(arg2 == NULLRCP) return 0;

	if(_op->getTestBefore())
    return (const Item::Ptr)context->getPathanFactory()->createBoolean(arg1->lessThan(arg2, context), context);

	return (const Item::Ptr)context->getPathanFactory()->createBoolean(arg2->lessThan(arg1, context), context);
}

std::string OrderComparison::OrderComparisonResult::asString(DynamicContext *context, int indent) const
{
  std::ostringstream oss;
  std::string in(getIndent(indent));

  oss << in << "<ordercomparison/>" << std::endl;

  return oss.str();
}
