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
 * $Id: FunctionTranslate.hpp,v 1.8 2006/11/01 16:37:13 jpcs Exp $
 */

#ifndef _FUNCTIONTRANSLATE_HPP
#define _FUNCTIONTRANSLATE_HPP

#include <xqilla/ast/ConstantFoldingFunction.hpp>

/** Translate function. */
class XQILLA_API FunctionTranslate : public ConstantFoldingFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  FunctionTranslate(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr);
  
  /** XPath function. returns string1 with each character in string2 replaced with the corresponding character in string3 **/
  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;

};

#endif // _FUNCTIONTRANSLATE_HPP



