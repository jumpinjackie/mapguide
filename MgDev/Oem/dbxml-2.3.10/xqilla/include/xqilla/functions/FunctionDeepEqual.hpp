/*
 * Copyright (c) 2001-2006
 *     DecisionSoft Limited. All rights reserved.
 * Copyright (c) 2004-2006
 *     Progress Software Corporation. All rights reserved.
 * Copyright (c) 2004-2006
 *     Oracle. All rights reserved.
 *
 * See the file LICENSE for redistribution information.
 *
 * $Id: FunctionDeepEqual.hpp,v 1.9 2006/11/01 16:37:13 jpcs Exp $
 */

/*
  Deep-equal funcion
*/

#ifndef _FUNCTIONDEEPEQUAL_HPP
#define _FUNCTIONDEEPEQUAL_HPP

#include <xqilla/framework/XQillaExport.hpp>

#include <xqilla/ast/ConstantFoldingFunction.hpp>

class Node;
class Collation;

class XQILLA_API FunctionDeepEqual : public ConstantFoldingFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  FunctionDeepEqual(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr);
  
  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;

  static bool deep_equal(Sequence seq1, Sequence seq2, Collation* collation, DynamicContext* context, const LocationInfo *info);
  static bool node_deep_equal(const Node::Ptr &node1, const Node::Ptr &node2, Collation* collation, DynamicContext* context, const LocationInfo *info);

};

#endif // _FUNCTIONDEEPEQUAL_HPP
