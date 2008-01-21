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
 * $Id: FunctionCodepointEqual.hpp,v 1.10 2006/11/01 16:37:12 jpcs Exp $
 */

/*
  codepoint-equal function
*/

#ifndef _FUNCTION_CODEPOINT_EQUAL_HPP
#define _FUNCTION_CODEPOINT_EQUAL_HPP

#include <xqilla/framework/XQillaExport.hpp>

#include <xqilla/ast/ConstantFoldingFunction.hpp>

class XQILLA_API FunctionCodepointEqual : public XQFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  FunctionCodepointEqual(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr);

  ASTNode* staticResolution(StaticContext *context);  
  virtual ASTNode *staticTyping(StaticContext *context);
  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;

private:
  Collation *collation_;
};

#endif // _FUNCTION_CODEPOINT_EQUAL_HPP
