/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

/*

   default-collation function

*/


#ifndef _FUNCTIONDEFAULTCOLLATION_HPP
#define _FUNCTIONDEFAULTCOLLATION_HPP

#include <pathan/functionAPI/ConstantFoldingFunction.hpp>

/** default-collation function*/
class PATHAN_EXPORT FunctionDefaultCollation : public ConstantFoldingFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  FunctionDefaultCollation(const VectorOfDataItems &args, XPath2MemoryManager* memMgr);
  
  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;
};

#endif // _FUNCTIONDEFAULTCOLLATION_HPP
