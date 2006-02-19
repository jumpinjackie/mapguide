/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

/*

 * xf:namespace-uri-from-QName function

*/

#ifndef _FUNCTIONGETNAMESPACEURIFROMQNAME_HPP
#define _FUNCTIONGETNAMESPACEURIFROMQNAME_HPP

#include <pathan/Pathan.hpp>

#include <pathan/functionAPI/ConstantFoldingFunction.hpp>

class PATHAN_EXPORT FunctionNamespaceURIFromQName : public ConstantFoldingFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  FunctionNamespaceURIFromQName(const VectorOfDataItems &args, XPath2MemoryManager* memMgr);
  
  /** Returns the namespace part of a QName*/
  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;

};

#endif // _FUNCTIONGETNAMESPACEURIFROMQNAME_HPP
