/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

/*

  Root function

*/

#ifndef _FUNCTIONROOT_HPP
#define _FUNCTIONROOT_HPP

#include <pathan/Pathan.hpp>

#include <pathan/dataItem/DataItemFunction.hpp>

class XPath2MemoryManager;

/** For function. */
class PATHAN_EXPORT FunctionRoot : public DataItemFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  FunctionRoot(const VectorOfDataItems &args, XPath2MemoryManager* memMgr);
  
  /**
   * Called during static analysis to determine if statically correct.
   * Performs constant folding if the function has an argument, and it
   * is constant.
   */
  virtual DataItem* staticResolution(StaticContext *context);

  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;

  static Node::Ptr root(const Node::Ptr &node, const DynamicContext *context);
};

#endif // _FUNCTIONROOT_HPP
