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
 * $Id: Intersect.hpp,v 1.10 2006/11/01 16:37:14 jpcs Exp $
 */

/*

   Op Intersect 
The intersect operator takes two node sequences as operands and returns a sequence containing all the nodes that occur in both operands.
*/

#ifndef _INTERSECT_HPP
#define _INTERSECT_HPP

#include <xqilla/framework/XQillaExport.hpp>

#include <xqilla/ast/XQOperator.hpp>

class XQILLA_API Intersect : public XQOperator
{
public:
  static const XMLCh name[];

  Intersect(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr);
  
  ASTNode* staticResolution(StaticContext *context);
  virtual ASTNode *staticTyping(StaticContext *context);
  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;

private:
  bool sortAdded_;
};

#endif 
