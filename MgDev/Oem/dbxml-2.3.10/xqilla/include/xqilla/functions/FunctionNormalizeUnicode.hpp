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
 * $Id: FunctionNormalizeUnicode.hpp,v 1.8 2006/11/01 16:37:13 jpcs Exp $
 */

/*

  normalize-unicode function

*/

#ifndef _FUNCTIONNORMALIZEUNICODE_HPP
#define _FUNCTIONNORMALIZEUNICODE_HPP

#include <xqilla/framework/XQillaExport.hpp>

#include <xqilla/ast/ConstantFoldingFunction.hpp>

/** normalize-unicode function. */
class XQILLA_API FunctionNormalizeUnicode : public ConstantFoldingFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  FunctionNormalizeUnicode(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr);
  
  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;
};

#endif // _FUNCTIONNORMALIZEUNICODE_HPP

