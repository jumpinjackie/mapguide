/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <sstream>

#include <pathan/operators/NodeComparison.hpp>
#include <pathan/Node.hpp>
#include <pathan/ATBooleanOrDerived.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/internal/factory/DatatypeFactory.hpp>
#include <xercesc/dom/DOMNode.hpp>
#include <pathan/PathanFactory.hpp>

/*static*/ const XMLCh NodeComparison::name[]={ XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_s, XERCES_CPP_NAMESPACE_QUALIFIER chNull };

NodeComparison::NodeComparison(const VectorOfDataItems &args, XPath2MemoryManager* memMgr)
  : DataItemOperator(name, args, memMgr)
{
}

Result NodeComparison::createResult(DynamicContext* context, int flags) const
{
  return new NodeComparisonResult(this, context);
}

NodeComparison::NodeComparisonResult::NodeComparisonResult(const NodeComparison *op, DynamicContext *context)
  : SingleResult(context),
    _op(op)
{
}

Item::Ptr NodeComparison::NodeComparisonResult::getSingleResult(DynamicContext *context) const
{
	const Node::Ptr arg1 = _op->getNodeParam(0,context);
  if(arg1 == NULLRCP) return 0;
	const Node::Ptr arg2 = _op->getNodeParam(1,context);
  if(arg2 == NULLRCP) return 0;
  
	return (const Item::Ptr)context->getPathanFactory()->createBoolean(arg1->equals(arg2), context);
}

std::string NodeComparison::NodeComparisonResult::asString(DynamicContext *context, int indent) const
{
  std::ostringstream oss;
  std::string in(getIndent(indent));

  oss << in << "<nodecomparison/>" << std::endl;

  return oss.str();
}
