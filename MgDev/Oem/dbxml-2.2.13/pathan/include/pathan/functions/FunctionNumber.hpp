/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

/*

  Number function

*/

#ifndef _FUNCTIONNUMBER_HPP
#define _FUNCTIONNUMBER_HPP

#include <pathan/Pathan.hpp>

#include <pathan/dataItem/DataItemFunction.hpp>

class AnyAtomicType;

/** Number operator function, and exists functions. */
class PATHAN_EXPORT FunctionNumber : public DataItemFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  FunctionNumber(const VectorOfDataItems &args, XPath2MemoryManager* memMgr);
  
  /**
   * Called during static analysis to determine if statically correct.
   * Performs constant folding if the function has an argument, and it
   * is constant.
   */
  virtual DataItem* staticResolution(StaticContext *context);

  /** Returns the argument as a number **/
  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;

  static Item::Ptr number(const AnyAtomicType::Ptr &item, DynamicContext *context);
};

#endif // _FUNCTIONNUMBER_HPP
