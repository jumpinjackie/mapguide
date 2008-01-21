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
 * $Id: FunctionCeiling.hpp,v 1.8 2006/11/01 16:37:12 jpcs Exp $
 */

/*

  xf:ceiling(numeric? $srcval) => numeric?

*/

#ifndef _FUNCTIONCEILING_HPP
#define _FUNCTIONCEILING_HPP

#include <xqilla/framework/XQillaExport.hpp>

#include <xqilla/ast/NumericFunction.hpp>

class XQILLA_API FunctionCeiling : public NumericFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  FunctionCeiling(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr);

  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;

};

#endif // _FUNCTIONCEILING_HPP

