/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

/*
  Concat operator function
*/

#ifndef _FUNCTIONCONCAT_HPP
#define _FUNCTIONCONCAT_HPP

#include <pathan/Pathan.hpp>

#include <pathan/functionAPI/ConstantFoldingFunction.hpp>

/** Concat operator function*/
class PATHAN_EXPORT FunctionConcat : public ConstantFoldingFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  ///Constructor.
  FunctionConcat(const VectorOfDataItems &args, XPath2MemoryManager* memMgr);
  
protected:
  /** concatenates two strings. Takes two strings, and gives a string */
  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;
};

#endif // _FUNCTIONCONCAT_HPP
