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
 * $Id: FTContains.hpp,v 1.3 2006/11/01 16:37:12 jpcs Exp $
 */

#ifndef _FTCONTAINS_HPP
#define _FTCONTAINS_HPP

#include <xqilla/framework/XQillaExport.hpp>

#include <xqilla/ast/ASTNodeImpl.hpp>
#include <xqilla/runtime/SingleResult.hpp>

class FTSelection;

class XQILLA_API FTContains : public ASTNodeImpl
{
public:
  FTContains(ASTNode *argument, FTSelection *selection, ASTNode *ignore, XPath2MemoryManager *memMgr);
  ~FTContains();

  virtual ASTNode *staticResolution(StaticContext *context);
  virtual ASTNode *staticTyping(StaticContext *context);
  virtual Result createResult(DynamicContext *context, int flags=0) const;

  const ASTNode *getArgument() const { return argument_; }
  const FTSelection *getSelection() const { return selection_; }
  const ASTNode *getIgnore() const { return ignore_; }

private:
  class XQILLA_API FTContainsResult : public SingleResult
  {
  public:
    FTContainsResult(const FTContains *parent);
    Item::Ptr getSingleResult(DynamicContext *context) const;
    std::string asString(DynamicContext *context, int indent) const;

  private:
    const FTContains *parent_;
  };

private:
  ASTNode *argument_;
  FTSelection *selection_;
  ASTNode *ignore_;
};

#endif

