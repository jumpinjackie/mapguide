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
 * $Id: FunctionMatches.hpp,v 1.8 2006/11/01 16:37:13 jpcs Exp $
 */

/*

  matches function

*/

#ifndef _FUNCTIONMATCHES_HPP
#define _FUNCTIONMATCHES_HPP

#include <xqilla/ast/ConstantFoldingFunction.hpp>

/** Matches function. */
class XQILLA_API FunctionMatches : public ConstantFoldingFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  FunctionMatches(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr);
  
  /** XPath function: returns true if string1 matches the regExp supplied as string2, otherwise returns false. **/
  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;

};

#endif // _FUNCTIONMATCHES_HPP
