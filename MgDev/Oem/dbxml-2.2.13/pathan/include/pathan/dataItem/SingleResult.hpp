/*
 * Copyright (c) 2004, Parthenon Computing Limited, All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _SINGLERESULT_HPP
#define _SINGLERESULT_HPP

#include <pathan/dataItem/ResultImpl.hpp>

/** A reference counting wrapper for the lazily evaluated query result */
class PATHAN_EXPORT SingleResult : public ResultImpl
{
public:
  SingleResult(DynamicContext *context);

  virtual Item::Ptr next(DynamicContext *context);

  virtual Item::Ptr getSingleResult(DynamicContext *context) const = 0;

private:
  bool toDo_;
};

#endif
