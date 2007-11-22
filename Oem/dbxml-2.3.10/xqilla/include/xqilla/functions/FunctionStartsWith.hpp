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
 * $Id: FunctionStartsWith.hpp,v 1.8 2006/11/01 16:37:13 jpcs Exp $
 */

/*

  StartsWith function

*/

#ifndef _FUNCTIONSTARTSWITH_HPP
#define _FUNCTIONSTARTSWITH_HPP

#include <xqilla/ast/ConstantFoldingFunction.hpp>

/** StartsWith function. */
class XQILLA_API FunctionStartsWith : public ConstantFoldingFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  FunctionStartsWith(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr);
  
  /** XPath function. returns true if string1 starts with string2 **/
  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;

};

#endif // _FUNCTIONSTARTSWITH_HPP
