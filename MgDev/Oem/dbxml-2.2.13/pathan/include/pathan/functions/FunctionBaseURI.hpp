/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

/*

   base-uri function

*/

#ifndef _FUNCTIONBASEURI_HPP
#define _FUNCTIONBASEURI_HPP

#include <pathan/Pathan.hpp>

#include <pathan/functionAPI/ConstantFoldingFunction.hpp>
#include <pathan/Sequence.hpp>

/** base-uri function */
class PATHAN_EXPORT FunctionBaseURI : public ConstantFoldingFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  FunctionBaseURI(const VectorOfDataItems &args, XPath2MemoryManager* memMgr);
  
  virtual DataItem* staticResolution(StaticContext *context);

	/* returns the base-URI property of document or element nodes,
	 * and the empty sequence for all other kinds of nodes */
  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;
};

#endif
