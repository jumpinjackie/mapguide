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
 * $Id: FunctionEndsWith.hpp,v 1.8 2006/11/01 16:37:13 jpcs Exp $
 */

/*
  
  xf:ends-with(string? $operand1, string? $operand2) => boolean?
  xf:ends-with(string? $operand1, string? $operand2, anyURI $collationLiteral) => boolean?
  
*/

#ifndef _FUNCTIONENDSWITH_HPP
#define _FUNCTIONENDSWITH_HPP

#include <xqilla/framework/XQillaExport.hpp>

#include <xqilla/ast/ConstantFoldingFunction.hpp>

class XQILLA_API FunctionEndsWith : public ConstantFoldingFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  FunctionEndsWith(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr);

  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;
};

#endif // _FUNCTIONENDSWITH_HPP



