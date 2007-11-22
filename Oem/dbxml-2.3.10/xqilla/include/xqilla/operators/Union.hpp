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
 * $Id: Union.hpp,v 1.10 2006/11/01 16:37:14 jpcs Exp $
 */

/*
   Union function -- takes two sequences, and merge them
*/

#ifndef _UNION_HPP
#define _UNION_HPP

#include <xqilla/framework/XQillaExport.hpp>

#include <xqilla/ast/XQNav.hpp>
#include <xqilla/ast/XQOperator.hpp>
#include <xqilla/runtime/ResultImpl.hpp>

/** Union operator function*/
class XQILLA_API Union : public XQOperator
{
public:
  static const XMLCh name[];

  Union(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr);
  
  ASTNode* staticResolution(StaticContext *context);
  virtual ASTNode *staticTyping(StaticContext *context);
  Result createResult(DynamicContext* context, int flags=0) const;

private:
  class UnionResult : public ResultImpl
  {
  public:
    UnionResult(const Union *op, int flags);

    Item::Ptr next(DynamicContext *context);
    std::string asString(DynamicContext *context, int indent) const;  
  private:
    const Union *_op;
    unsigned int _flags;
    unsigned int _index;
    Result _result;
  };

  bool sortAdded_;
};

#endif

