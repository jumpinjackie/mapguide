/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

/*
  Deep-equal funcion
*/

#ifndef _FUNCTIONDEEPEQUAL_HPP
#define _FUNCTIONDEEPEQUAL_HPP

#include <pathan/Pathan.hpp>

#include <pathan/functionAPI/ConstantFoldingFunction.hpp>

class Node;
class Collation;

class PATHAN_EXPORT FunctionDeepEqual : public ConstantFoldingFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  FunctionDeepEqual(const VectorOfDataItems &args, XPath2MemoryManager* memMgr);
  
  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;

  static bool deep_equal(Sequence seq1, Sequence seq2, Collation* collation, DynamicContext* context);
  static bool node_deep_equal(const Node::Ptr &node1, const Node::Ptr &node2, Collation* collation, DynamicContext* context);

};

#endif // _FUNCTIONDEEPEQUAL_HPP
