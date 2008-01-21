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
 * $Id: FunctionStringJoin.hpp,v 1.8 2006/11/01 16:37:13 jpcs Exp $
 */

/*
  string-join function
*/

#ifndef _FUNCTIONSTRINGJOIN_HPP
#define _FUNCTIONSTRINGJOIN_HPP

#include <xqilla/framework/XQillaExport.hpp>

#include <xqilla/ast/ConstantFoldingFunction.hpp>

/* string-join function */
class XQILLA_API FunctionStringJoin : public ConstantFoldingFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  //Constructor.
  FunctionStringJoin(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr);
  
protected:
  /* Accepts a sequence of strings and returns the strings concatenated together with an optional separator. */
  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;

};

#endif // _FUNCTIONSTRINGJOIN_HPP
