/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

/*

  QName function

*/

#ifndef _FUNCTIONQNAME_HPP
#define _FUNCTIONQNAME_HPP

#include <pathan/Pathan.hpp>

#include <pathan/functionAPI/ConstantFoldingFunction.hpp>

/** QName constructor function. */
class PATHAN_EXPORT FunctionQName : public ConstantFoldingFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  FunctionQName(const VectorOfDataItems &args, XPath2MemoryManager*  memMgr);
  
  /** Given a uri and a name, it returns an expanded QName **/
  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;

};

#endif // _FUNCTIONQNAME_HPP

