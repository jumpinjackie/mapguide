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
 * $Id: FunctionUpperCase.hpp,v 1.8 2006/11/01 16:37:13 jpcs Exp $
 */

#ifndef _FUNCTIONUPPERCASE_HPP
#define _FUNCTIONUPPERCASE_HPP

#include <xqilla/framework/XQillaExport.hpp>

#include <xqilla/ast/ConstantFoldingFunction.hpp>

/**
 * Returns the given value after translating every lower-case letter to its
 * upper-case correspondent.
 *
 * xf:upper-case(string? $srcval) =>  string?
 * 
 */
class XQILLA_API FunctionUpperCase : public ConstantFoldingFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  /**
   * Constructor
   */
  FunctionUpperCase(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr);

  /**
   * Returns the upper-case string.
   */
  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;
};

#endif // _FUNCTIONUPPERCASE_HPP



