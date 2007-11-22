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
 * $Id: Except.hpp,v 1.10 2006/11/01 16:37:14 jpcs Exp $
 */

/*

   Op Except 
The except operator takes two node sequences as operands and returns a sequence containing all the nodes that occur in the first operand
but not in the second operand
*/

#ifndef _EXCEPT_HPP
#define _EXCEPT_HPP

#include <xqilla/framework/XQillaExport.hpp>

#include <xqilla/ast/XQOperator.hpp>
#include <xqilla/ast/XQNav.hpp>
#include <xqilla/runtime/ResultBuffer.hpp>

class XQILLA_API Except : public XQOperator
{
public:
  static const XMLCh name[];

  Except(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr);
  
  ASTNode* staticResolution(StaticContext *context);
  virtual ASTNode *staticTyping(StaticContext *context);
  Result createResult(DynamicContext* context, int flags=0) const;

private:
  class ExceptResult : public ResultImpl
  {
  public:
    ExceptResult(const Except *op, int flags);

    Item::Ptr next(DynamicContext *context);
    std::string asString(DynamicContext *context, int indent) const;  
  private:
    const Except *_op;
    unsigned int _flags;
    bool _toDo;
    Result _result;
    ResultBuffer _excpt;
  };

  bool sortAdded_;
};

#endif 
