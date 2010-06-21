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
 * $Id: FunctionLocalNameFromQName.hpp,v 1.8 2006/11/01 16:37:13 jpcs Exp $
 */

/*
 * fn:local-name-from-QName function
 */

#ifndef _FUNCTIONGETLOCALNAMEFROMQNAME_HPP
#define _FUNCTIONGETLOCALNAMEFROMQNAME_HPP

#include <xqilla/framework/XQillaExport.hpp>

#include <xqilla/ast/ConstantFoldingFunction.hpp>
#include <xqilla/runtime/Sequence.hpp>

/** Name function */
class XQILLA_API FunctionLocalNameFromQName : public ConstantFoldingFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  FunctionLocalNameFromQName(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr);
  
  /** Returns the local-name part of a QName, eg for foo:bar it returns bar */
  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;
};

#endif // _FUNCTIONGETLOCALNAMEFROMQNAME_HPP
