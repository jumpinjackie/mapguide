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
 * $Id: FunctionBaseURI.hpp,v 1.9 2006/11/01 16:37:12 jpcs Exp $
 */

/*

   base-uri function

*/

#ifndef _FUNCTIONBASEURI_HPP
#define _FUNCTIONBASEURI_HPP

#include <xqilla/framework/XQillaExport.hpp>

#include <xqilla/ast/ConstantFoldingFunction.hpp>
#include <xqilla/runtime/Sequence.hpp>

/** base-uri function */
class XQILLA_API FunctionBaseURI : public ConstantFoldingFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  FunctionBaseURI(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr);
  
  virtual ASTNode* staticResolution(StaticContext *context);
  virtual ASTNode *staticTyping(StaticContext *context);

	/* returns the base-URI property of document or element nodes,
	 * and the empty sequence for all other kinds of nodes */
  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;
};

#endif
