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
 * $Id: FunctionTrue.hpp,v 1.8 2006/11/01 16:37:13 jpcs Exp $
 */

#ifndef _FUNCTIONTRUE_HPP
#define _FUNCTIONTRUE_HPP

#include <xqilla/framework/XQillaExport.hpp>

#include <xqilla/ast/ConstantFoldingFunction.hpp>

/**
 * Returns the boolean value true.
 *
 * xf:true() => boolean
 */
class XQILLA_API FunctionTrue : public ConstantFoldingFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;
  
  /**
   * Constructor.
   */
  FunctionTrue(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr);

  /**
   * Returns the boolean value true.
   */
  virtual Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;
};

#endif // _FUNCTIONTRUE_HPP

