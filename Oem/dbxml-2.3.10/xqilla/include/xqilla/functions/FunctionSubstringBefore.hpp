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
 * $Id: FunctionSubstringBefore.hpp,v 1.8 2006/11/01 16:37:13 jpcs Exp $
 */

/*

  Substring-before function

*/

#ifndef _FUNCTIONSUBSTRINGBEFORE_HPP
#define _FUNCTIONSUBSTRINGBEFORE_HPP

#include <xqilla/framework/XQillaExport.hpp>

#include <xqilla/ast/ConstantFoldingFunction.hpp>

/** Substring-before function. */
class XQILLA_API FunctionSubstringBefore : public ConstantFoldingFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  FunctionSubstringBefore(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr);
  
  /** XPath function. returns the substring of the first string preceding the first occurrence of the second string within that string **/
  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;

};

#endif // _FUNCTIONSUBSTRINGBEFORE_HPP
