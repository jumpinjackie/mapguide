/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

/*

  matches function

*/

#ifndef _FUNCTIONMATCHES_HPP
#define _FUNCTIONMATCHES_HPP

#include <pathan/functionAPI/ConstantFoldingFunction.hpp>

/** Matches function. */
class PATHAN_EXPORT FunctionMatches : public ConstantFoldingFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  FunctionMatches(const VectorOfDataItems &args, XPath2MemoryManager* memMgr);
  
  /** XPath function: returns true if string1 matches the regExp supplied as string2, otherwise returns false. **/
  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;

};

#endif // _FUNCTIONMATCHES_HPP
