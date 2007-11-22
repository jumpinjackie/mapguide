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
 * $Id: FunctionTokenize.hpp,v 1.8 2006/11/01 16:37:13 jpcs Exp $
 */

#ifndef _FUNCTIONTOKENIZE_HPP
#define _FUNCTIONTOKENIZE_HPP

#include <xqilla/ast/ConstantFoldingFunction.hpp>

/** 
 * Breaks the $input string into a sequence of strings, 
 * treating any substring that matches $pattern as a separator. 
 * The separators themselves are not returned. 
 *
 * tokenize(string? $input, string $pattern) => string*
 * tokenize(string? $input, string $pattern, string $flags) => string*
 */
class XQILLA_API FunctionTokenize : public ConstantFoldingFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  /**
   * Constructor.
   */
  FunctionTokenize(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr);
  
  /**
   * Returns $input broken into a sequence of strings 
   * using matches to $pattern as a separator. 
   */
  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;

};

#endif // _FUNCTIONTOKENIZE_HPP

