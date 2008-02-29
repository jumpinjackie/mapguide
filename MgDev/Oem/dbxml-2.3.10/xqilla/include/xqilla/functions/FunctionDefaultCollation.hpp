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
 * $Id: FunctionDefaultCollation.hpp,v 1.8 2006/11/01 16:37:13 jpcs Exp $
 */

/*

   default-collation function

*/


#ifndef _FUNCTIONDEFAULTCOLLATION_HPP
#define _FUNCTIONDEFAULTCOLLATION_HPP

#include <xqilla/ast/ConstantFoldingFunction.hpp>

/** default-collation function*/
class XQILLA_API FunctionDefaultCollation : public ConstantFoldingFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  FunctionDefaultCollation(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr);
  
  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;
};

#endif // _FUNCTIONDEFAULTCOLLATION_HPP
