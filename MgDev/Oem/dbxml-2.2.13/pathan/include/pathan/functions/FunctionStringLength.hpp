/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

/*

  String-length function

*/

#ifndef _FUNCTIONSTRINGLENGTH_HPP
#define _FUNCTIONSTRINGLENGTH_HPP


#include <pathan/dataItem/DataItemFunction.hpp>

/** String-length function. */
class PATHAN_EXPORT FunctionStringLength : public DataItemFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  FunctionStringLength(const VectorOfDataItems &args, XPath2MemoryManager* memMgr);
  
  /**
   * Called during static analysis to determine if statically correct.
   * Performs constant folding if the function has an argument, and it
   * is constant.
   */
  virtual DataItem* staticResolution(StaticContext *context);

  /** Returns the readable length of the string argument. **/
  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;

private:
  const XMLCh* getString(DynamicContext* context) const;
};

#endif // _FUNCTIONSTRINGLENGTH_HPP
