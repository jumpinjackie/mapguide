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
 * $Id: FunctionSubstringAfter.hpp,v 1.8 2006/11/01 16:37:13 jpcs Exp $
 */

/*

  Substring-after function

*/

#ifndef _FUNCTIONSUBSTRINGAFTER_HPP
#define _FUNCTIONSUBSTRINGAFTER_HPP

#include <xqilla/ast/ConstantFoldingFunction.hpp>

/** Substring-after function. */
class XQILLA_API FunctionSubstringAfter : public ConstantFoldingFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  FunctionSubstringAfter(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr);
  
  /** XPath function. returns the substring of the first string following the first occurrence of the second string within that string **/
  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;

};

#endif // _FUNCTIONSUBSTRINGAFTER_HPP
