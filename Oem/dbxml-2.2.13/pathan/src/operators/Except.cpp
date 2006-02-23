/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */
  
#include "../config/pathan_config.h"
#include <assert.h>
#include <sstream>

#include <pathan/operators/Except.hpp>
#include <pathan/exceptions/XPath2ErrorException.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/Node.hpp>

/*static*/ const XMLCh Except::name[]={ XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_x, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_c, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_p, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, XERCES_CPP_NAMESPACE_QUALIFIER chNull };

Except::Except(const VectorOfDataItems &args, XPath2MemoryManager* memMgr)
  : DataItemOperator(name, args, memMgr)
{
  _src.setProperties(StaticResolutionContext::DOCORDER | StaticResolutionContext::GROUPED);
  _src.getStaticType().flags = StaticType::NODE_TYPE;
}

Result Except::createResult(DynamicContext* context, int flags) const
{
  Result result(new ExceptResult(this, flags, context));

  if(context->getNodeSetOrdering()==StaticContext::ORDERING_UNORDERED || flags & DataItem::UNORDERED) {
    return result;
  }
  else {
    return result.sortIntoDocumentOrder(context);
  }
}

Except::ExceptResult::ExceptResult(const Except *op, int flags, DynamicContext *context)
  : ResultImpl(context),
    _op(op),
    _flags(flags),
    _toDo(true),
    _result(0),
    _excpt(0)
{
}

Item::Ptr Except::ExceptResult::next(DynamicContext *context)
{
  if(_toDo) {
    _toDo = false;
    _result = _op->getArgument(0)->collapseTree(context, _flags);
    _excpt = _op->getArgument(1)->collapseTree(context, DataItem::UNORDERED);
  }

  Item::Ptr item = _result.next(context);
  while(item != NULLRCP) {    

    bool found = false;
    Result except_result(_excpt.createResult(context));
    Item::Ptr except_item;
    while((except_item = except_result.next(context)) != NULLRCP) {
      // Check it's a node
      if(!except_item->isNode()) {
        DSLthrow(XPath2ErrorException,X("Except::ExceptResult::next"), X("A parameter of operator 'except' contains an item that is not a node [err:XP0006]"));
      }
      if(((Node*)(const Item*)item)->equals((const Node::Ptr )except_item)) {
        found = true;
        break;
      }
    }

    if(!found) {
      // Check it's a node
      if(!item->isNode()) {
        DSLthrow(XPath2ErrorException,X("Except::ExceptResult::next"), X("A parameter of operator 'except' contains an item that is not a node [err:XP0006]"));
      }
      break;
    }

    item = _result.next(context);
  }

  if(item == NULLRCP) {
    _result = 0;
    _excpt = 0;
  }

  return item;
}

std::string Except::ExceptResult::asString(DynamicContext *context, int indent) const
{
  std::ostringstream oss;
  std::string in(getIndent(indent));

  oss << in << "<except/>" << std::endl;

  return oss.str();
}
