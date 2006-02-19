/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

/*
 * resolve-QName function
 */

#ifndef _FUNCTIONRESOLVEQNAME_HPP
#define _FUNCTIONRESOLVEQNAME_HPP

#include <pathan/Pathan.hpp>

#include <pathan/functionAPI/ConstantFoldingFunction.hpp>

/** resolve-QName function. */
class PATHAN_EXPORT FunctionResolveQName : public ConstantFoldingFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  FunctionResolveQName(const VectorOfDataItems &args, XPath2MemoryManager* memMgr);
  
  /** Returns an QName with the lexical form given in the first argument. The 
   *  prefix is resolved using the in-scope namespaces for a given element. **/
  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;

};

#endif // _FUNCTIONRESOLVEQNAME_HPP
