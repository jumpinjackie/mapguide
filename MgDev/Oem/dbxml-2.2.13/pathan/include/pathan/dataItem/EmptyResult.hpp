/*
 * Copyright (c) 2004, Parthenon Computing Limited, All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _EMPTYRESULT_HPP
#define _EMPTYRESULT_HPP

#include <pathan/dataItem/ResultImpl.hpp>

class PATHAN_EXPORT EmptyResult : public ResultImpl
{
public:
  EmptyResult(DynamicContext *context);

  Item::Ptr next(DynamicContext *context);

  std::string asString(DynamicContext *context, int indent) const;
};

#endif
