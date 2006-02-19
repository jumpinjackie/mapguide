/*
 * Copyright (c) 2004, Parthenon Computing Limited, All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _ATOMIZERESULT_HPP
#define _ATOMIZERESULT_HPP

#include <pathan/dataItem/Result.hpp>
#include <pathan/dataItem/ResultImpl.hpp>

class PATHAN_EXPORT AtomizeResult : public ResultImpl
{
public:
  AtomizeResult(const Result &parent, DynamicContext *context);

  Item::Ptr next(DynamicContext *context);

  std::string asString(DynamicContext *context, int indent) const;

private:
  Result _parent;
  Result _sub;
};

#endif
