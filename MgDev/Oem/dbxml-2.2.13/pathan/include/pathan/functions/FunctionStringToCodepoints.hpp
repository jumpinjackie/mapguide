/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

/*

   StringToCodepoints function
*/

#ifndef _FUNCTIONSTRINGTOCODEPOINTS_HPP
#define _FUNCTIONSTRINGTOCODEPOINTS_HPP

#include <pathan/Pathan.hpp>


#include <pathan/functionAPI/ConstantFoldingFunction.hpp>

/** StringToCodepoints function */
class PATHAN_EXPORT FunctionStringToCodepoints : public ConstantFoldingFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  FunctionStringToCodepoints(const VectorOfDataItems &args, XPath2MemoryManager* memMgr);
  
  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;
};

#endif // _FUNCTIONCONTAINS_HPP

