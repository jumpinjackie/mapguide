/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

/*
  namespace-uri-for-prefix function
*/

#ifndef _FUNCTIONGETNAMESPACEURIFORPREFIX_HPP
#define _FUNCTIONGETNAMESPACEURIFORPREFIX_HPP

#include <pathan/Pathan.hpp>

#include <pathan/functionAPI/ConstantFoldingFunction.hpp>

class PATHAN_EXPORT FunctionNamespaceURIForPrefix : public ConstantFoldingFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  FunctionNamespaceURIForPrefix(const VectorOfDataItems &args, XPath2MemoryManager* memMgr);
  
  /** Given a uri and a name, it returns an expanded QName **/
  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;

};

#endif // _FUNCTIONGETNAMESPACEURIFORPREFIX_HPP

