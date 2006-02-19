/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

/*
  codepoints-to-string function
*/

#ifndef _FUNCTIONCODEPOINTSTOSTRING_HPP
#define _FUNCTIONCODEPOINTSTOSTRING_HPP

#include <pathan/Pathan.hpp>

#include <pathan/functionAPI/ConstantFoldingFunction.hpp>

/** codepoints-to-string function */
class PATHAN_EXPORT FunctionCodepointsToString : public ConstantFoldingFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  ///Constructor.
  FunctionCodepointsToString(const VectorOfDataItems &args, XPath2MemoryManager* memMgr);
  
protected:
  /** takes a series of integers (codepoints) and turns them into a unicode string */
  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;
};
#endif // _FUNCTIONCODEPOINTSTOSTRING_HPP

