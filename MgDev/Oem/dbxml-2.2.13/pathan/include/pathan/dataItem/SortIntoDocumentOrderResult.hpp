/*
 * Copyright (c) 2004, Parthenon Computing Limited, All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _SORTINTODOCUMENTORDERRESULT_HPP
#define _SORTINTODOCUMENTORDERRESULT_HPP

#include <pathan/dataItem/Result.hpp>
#include <pathan/dataItem/LazySequenceResult.hpp>

class PATHAN_EXPORT SortIntoDocumentOrderResult : public LazySequenceResult
{
public:
  SortIntoDocumentOrderResult(Result &parent, DynamicContext *context);

  void getResult(Sequence &toFill, DynamicContext *context) const;

  std::string asString(DynamicContext *context, int indent) const;

private:
  Result parent_;
};

#endif
