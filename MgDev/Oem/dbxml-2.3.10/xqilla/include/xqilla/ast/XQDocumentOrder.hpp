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
 * $Id: XQDocumentOrder.hpp,v 1.6 2006/11/01 16:37:11 jpcs Exp $
 */

#ifndef _XQDOCUMENTORDER_HPP
#define _XQDOCUMENTORDER_HPP

#include <xqilla/framework/XQillaExport.hpp>
#include <xqilla/ast/ASTNodeImpl.hpp>
#include <xqilla/runtime/LazySequenceResult.hpp>
#include <xqilla/items/Node.hpp>

#include <set>

class XQILLA_API XQDocumentOrder : public ASTNodeImpl
{
public:
  XQDocumentOrder(ASTNode* expr, XPath2MemoryManager* memMgr);

  virtual Result createResult(DynamicContext* context, int flags=0) const;
  virtual ASTNode* staticResolution(StaticContext *context);
  virtual ASTNode *staticTyping(StaticContext *context);

  const ASTNode *getExpression() const { return expr_; }
  void setExpression(ASTNode *expr) { expr_ = expr; }
  bool getUnordered() const { return unordered_; }

protected:
  ASTNode* expr_;
  bool unordered_;
};

class XQILLA_API DocumentOrderResult : public LazySequenceResult
{
public:
  DocumentOrderResult(const LocationInfo *location, const Result &parent, DynamicContext *context)
    : LazySequenceResult(location, context), parent_(parent) {}
  void getResult(Sequence &toFill, DynamicContext *context) const;
  std::string asString(DynamicContext *context, int indent) const { return "documentorderresult"; }
private:
  mutable Result parent_;
};

class XQILLA_API UniqueNodesResult : public ResultImpl
{
public:
  UniqueNodesResult(const XQDocumentOrder *doc, const Result &parent, const DynamicContext *context)
    : ResultImpl(doc), parent_(parent), nTypeOfItemsInLastStep_(0),
      noDups_(uniqueLessThanCompareFn(context)) {}
  Item::Ptr next(DynamicContext *context);
  void skip() { parent_->skip(); }
  std::string asString(DynamicContext *context, int indent) const { return "uniquenodesresult"; }

private:
  class uniqueLessThanCompareFn {
  public:
    uniqueLessThanCompareFn(const DynamicContext *context)
      : context_(context) {}
    bool operator()(const Node::Ptr &first, const Node::Ptr &second)
    {
      return first->uniqueLessThan(second, context_);
    }
  private:
    const DynamicContext *context_;
  };
  typedef std::set<Node::Ptr, uniqueLessThanCompareFn> NoDuplicatesSet;

  Result parent_;
  int nTypeOfItemsInLastStep_;
  NoDuplicatesSet noDups_;
};

#endif
