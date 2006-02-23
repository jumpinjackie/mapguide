/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

/*
   index-of function: returns the location(s) of the specified item in a sequence
*/

#ifndef _FUNCTIONINDEXOF_HPP
#define _FUNCTIONINDEXOF_HPP

#include <pathan/functionAPI/ConstantFoldingFunction.hpp>
#include <pathan/Sequence.hpp>

class XPath2MemoryManager;
class Collation;

/** index-of function*/
class PATHAN_EXPORT FunctionIndexOf : public ConstantFoldingFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  FunctionIndexOf(const VectorOfDataItems &args, XPath2MemoryManager* memMgr);
  
  /** Returns a sequence of indices where the item occurs in the sequence */
  Sequence indexOf(Sequence &list, const Item::Ptr &item, Collation* collation, DynamicContext* context) const;
  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;
};

#endif // _FUNCTIONINDEXOF_HPP
