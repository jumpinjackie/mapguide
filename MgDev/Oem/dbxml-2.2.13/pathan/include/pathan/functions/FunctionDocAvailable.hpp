/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

/*
  doc-available function
*/

#ifndef _FUNCTIONDOCAVAILABLE_HPP
#define _FUNCTIONDOCAVAILABLE_HPP

#include <pathan/Pathan.hpp>

#include <pathan/dataItem/DataItemFunction.hpp>

class PATHAN_EXPORT FunctionDocAvailable : public DataItemFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  FunctionDocAvailable(const VectorOfDataItems &args, XPath2MemoryManager* memMgr);
  
  virtual DataItem* staticResolution(StaticContext *context);

  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;
};

#endif // _FUNCTIONDOCAVAILABLE_HPP
