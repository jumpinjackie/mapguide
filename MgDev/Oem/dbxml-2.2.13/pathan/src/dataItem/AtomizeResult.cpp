/*
 * Copyright (c) 2004, Parthenon Computing Limited, All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <sstream>
#include <pathan/dataItem/AtomizeResult.hpp>
#include <pathan/Node.hpp>
#include <pathan/Sequence.hpp>

AtomizeResult::AtomizeResult(const Result &parent, DynamicContext *context)
  : ResultImpl(context),
    _parent(parent),
    _sub(0)
{
}

Item::Ptr AtomizeResult::next(DynamicContext *context)
{
  // for $item in (Expr) return
  //   typeswitch ($item)
  //     case $value as atomic value return $value
  //     default $node return fn:data($node)

  Item::Ptr result = _sub.next(context);
  while(result == NULLRCP) {
    _sub = 0;
    result = _parent.next(context);
    if(result == NULLRCP) {
      _parent = 0;
      return 0;
    }
    if(result->isNode()) {
      _sub = ((Node*)(const Item*)result)->dmTypedValue(context);
      result = _sub.next(context);
    }
  }
  return result;
}

std::string AtomizeResult::asString(DynamicContext *context, int indent) const
{
  std::ostringstream oss;
  std::string in(getIndent(indent));

  oss << in << "<atomize>" << std::endl;
  oss << _parent.asString(context, indent + 1);
  oss << in << "</atomize>" << std::endl;

  return oss.str();
}
