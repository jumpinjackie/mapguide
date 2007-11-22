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
 * $Id: FunctionDistinctValues.hpp,v 1.11 2006/11/01 16:37:13 jpcs Exp $
 */

/*

  Distinct-values function
  
*/

#ifndef _FUNCTIONDISTINCTVALUES_HPP
#define _FUNCTIONDISTINCTVALUES_HPP

#include <xqilla/framework/XQillaExport.hpp>

#include <xqilla/ast/XQFunction.hpp>

// Distinct values is not really an aggregate function, but since its behaviour is so
// similar (in terms of sequence validation) we will extend from it.
class XQILLA_API FunctionDistinctValues : public XQFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  FunctionDistinctValues(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr);
  
  virtual ASTNode* staticResolution(StaticContext *context);
  virtual ASTNode *staticTyping(StaticContext *context);
  virtual Result createResult(DynamicContext* context, int flags=0) const;
};

#endif // _FUNCTIONDISTINCTVALUES_HPP
