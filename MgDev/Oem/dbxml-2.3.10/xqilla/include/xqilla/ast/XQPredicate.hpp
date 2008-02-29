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
 * $Id: XQPredicate.hpp,v 1.3 2006/11/01 16:37:11 jpcs Exp $
 */

#ifndef _XQPREDICATE_HPP
#define _XQPREDICATE_HPP

#include <xqilla/ast/ASTNodeImpl.hpp>

class XQPredicate;

class VectorOfPredicates : public std::vector<XQPredicate*,XQillaAllocator<XQPredicate*> >
{
public:
  VectorOfPredicates(XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *mm)
    : std::vector<XQPredicate*,XQillaAllocator<XQPredicate*> >(XQillaAllocator<XQPredicate*>(mm)) {}
};


class XQILLA_API XQPredicate : public ASTNodeImpl
{
public:
  XQPredicate(ASTNode *predicate, XPath2MemoryManager* memMgr);
  XQPredicate(ASTNode *expr, ASTNode *predicate, XPath2MemoryManager* memMgr);

  virtual Result createResult(DynamicContext* context, int flags=0) const;
  virtual ASTNode* staticResolution(StaticContext *context);
  virtual ASTNode *staticTyping(StaticContext *context);

  const ASTNode *getExpression() const { return expr_; }
  void setExpression(ASTNode *expr) { expr_ = expr; }

  const ASTNode *getPredicate() const { return predicate_; }
  void setPredicate(ASTNode *pred) { predicate_ = pred; }

  static ASTNode *addPredicates(ASTNode *expr, VectorOfPredicates *preds);

protected:
  class PredicateFilterResult : public ResultImpl
  {
  public:
    PredicateFilterResult(const Result &parent, const ASTNode *pred, unsigned int contextSize);
    Item::Ptr next(DynamicContext *context);
    void skip();
    std::string asString(DynamicContext *context, int indent) const;
  private:
    bool todo_;
    Result parent_;
    const ASTNode *pred_;
    unsigned int contextPos_;
    unsigned int contextSize_;
    Item::Ptr first_;
    Item::Ptr second_;
  };

  class NonNumericPredicateFilterResult : public ResultImpl
  {
  public:
    NonNumericPredicateFilterResult(const Result &parent, const ASTNode *pred, unsigned int contextSize);
    Item::Ptr next(DynamicContext *context);
    void skip();
    std::string asString(DynamicContext *context, int indent) const;
  private:
    bool todo_;
    Result parent_;
    const ASTNode *pred_;
    unsigned int contextPos_;
    unsigned int contextSize_;
  };

  class NumericPredicateFilterResult : public ResultImpl
  {
  public:
    NumericPredicateFilterResult(const Result &parent, const ASTNode *pred, unsigned int contextSize);
    Item::Ptr next(DynamicContext *context);
    void skip();
    std::string asString(DynamicContext *context, int indent) const;
  private:
    bool todo_;
    Result parent_;
    const ASTNode *pred_;
    unsigned int contextSize_;
  };

  ASTNode *expr_;
  ASTNode *predicate_;
};

#endif
