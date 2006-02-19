/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

/*

  Substring function

*/

#ifndef _FUNCTIONSUBSTRING_HPP
#define _FUNCTIONSUBSTRING_HPP

#include <pathan/Pathan.hpp>

#include <pathan/functionAPI/ConstantFoldingFunction.hpp>

/** Substring function. */
class PATHAN_EXPORT FunctionSubstring : public ConstantFoldingFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  FunctionSubstring(const VectorOfDataItems &args, XPath2MemoryManager* memMgr);
  
  /** Returns the substring to the right of parm2 until optional parm3 from string parm1 **/
  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;

};

#endif // _FUNCTIONSUBSTRING_HPP





