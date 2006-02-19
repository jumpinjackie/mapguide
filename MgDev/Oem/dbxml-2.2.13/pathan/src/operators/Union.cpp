/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <sstream>

#include <pathan/operators/Union.hpp>

#include <pathan/Sequence.hpp>
#include <pathan/Node.hpp>
#include <pathan/exceptions/XPath2ErrorException.hpp>
#include <pathan/DynamicContext.hpp>

/*static*/ const XMLCh Union::name[]={ XERCES_CPP_NAMESPACE_QUALIFIER chLatin_u, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, XERCES_CPP_NAMESPACE_QUALIFIER chNull };

Union::Union(const VectorOfDataItems &args, XPath2MemoryManager* memMgr)
  : DataItemOperator(name, args, memMgr)
{
  _src.setProperties(StaticResolutionContext::DOCORDER | StaticResolutionContext::GROUPED);
  _src.getStaticType().flags = StaticType::NODE_TYPE;
}

Result Union::createResult(DynamicContext* context, int flags) const
{
  Result result(new UnionResult(this, flags, context));

  if(context->getNodeSetOrdering()==StaticContext::ORDERING_UNORDERED || flags & DataItem::UNORDERED) {
    return result;
  }
  else {
    return result.sortIntoDocumentOrder(context);
  }
}

Union::UnionResult::UnionResult(const Union *op, int flags, DynamicContext *context)
  : ResultImpl(context),
    _op(op),
    _flags(flags),
    _index(0),
    _result(0)
{
}

Item::Ptr Union::UnionResult::next(DynamicContext *context)
{
  Item::Ptr item = 0;
  while(true) {
    item = _result.next(context);

    if(item == NULLRCP) {
      if(_index > 1) {
        _index = 0;
        return 0;
      }
      else {
        _result = _op->getArgument(_index++)->collapseTree(context, _flags);
      }
    }
    else {
      // Check it's a node
      if(!item->isNode()) {
        DSLthrow(XPath2ErrorException,X("Union::UnionResult::next"), X("A parameter of operator 'union' contains an item that is not a node [err:XP0006]"));
      }

      return item;
    }
  }

  return 0;
}

std::string Union::UnionResult::asString(DynamicContext *context, int indent) const
{
  std::ostringstream oss;
  std::string in(getIndent(indent));

  oss << in << "<union/>" << std::endl;

  return oss.str();
}
