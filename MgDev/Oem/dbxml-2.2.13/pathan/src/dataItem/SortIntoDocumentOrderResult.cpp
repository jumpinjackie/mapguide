/*
 * Copyright (c) 2004, Parthenon Computing Limited, All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <sstream>
#include <pathan/dataItem/SortIntoDocumentOrderResult.hpp>
#include <pathan/Sequence.hpp>

SortIntoDocumentOrderResult::SortIntoDocumentOrderResult(Result &parent, DynamicContext *context)
  : LazySequenceResult(context),
    parent_(parent)
{
}

void SortIntoDocumentOrderResult::getResult(Sequence &toFill, DynamicContext *context) const
{
  toFill = const_cast<Result&>(parent_).toSequence(context); // parent_ is mutable
  const_cast<Result&>(parent_) = 0;
  toFill.sortIntoDocumentOrder(context);
}

std::string SortIntoDocumentOrderResult::asString(DynamicContext *context, int indent) const
{
  std::ostringstream oss;
  std::string in(getIndent(indent));

  oss << in << "<documentorder>" << std::endl;
  oss << parent_.asString(context, indent + 1);
  oss << in << "</documentorder>" << std::endl;

  return oss.str();
}
