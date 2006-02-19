/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _DATAITEMIMPL_HPP
#define _DATAITEMIMPL_HPP

class DynamicContext;
class Numeric;

#include "DataItem.hpp"
#include "Result.hpp"
#include "LazySequenceResult.hpp"
#include "StaticResolutionContext.hpp"

/** class which is used as base for all the different data types */
class PATHAN_EXPORT DataItemImpl : public DataItem
{
public:
  DataItemImpl(XPath2MemoryManager* memMgr);
  virtual ~DataItemImpl();

  DataItem::whichType getType(void) const;
	
  ///returns the predicate list to be applied to each node we select
  const Predicates& getPredicates(void) const;
  //these can create a step if there are none!!
  virtual void addPredicates(const VectorOfDataItems &steps);
  virtual void addPredicates(const Predicates &steps);

  /** Returns true if this DataItem has no predicates, and is an instance of
      DataItemSequence or DataItemLiteral */
  virtual bool isConstant() const;
  /** Returns true if this DataItem has no predicates, and is an instance of
      DataItemSequence or DataItemLiteral. If the literal value of this DataItem
      is a single DateOrTimeType, then hasTimezone() on it must return true,
      otherwise this method will return false. */
  virtual bool isConstantAndHasTimezone(StaticContext* context) const;

  ///calls createResult and then checks predicates
  virtual Result collapseTree(DynamicContext* context, int flags=0) const;

  /**
   * Can be overridden by derived classes. Default implementation returns a
   * SequenceResult made from the Sequence returned by collapseTreeInternal.
   */
  virtual Result createResult(DynamicContext* context, int flags=0) const;

  /**
   * Can be overridden by derived classes to add an execution stage after
   * predicate evaluation. This is mainly used by reverse axes to reorder
   * their steps.
   */
  virtual Result postPredicateResultHook(Result &result, DynamicContext* context, int flags=0) const;

  /**
   * Collapse the compiled expression based on context. Default implementation
   * returns an empty sequence.
   */
  virtual Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;

  Result createPredicateResult(Result &parent, const PredInfo &p, DynamicContext *context) const;
  virtual bool isSingleNumericConstant(StaticContext *context) const;

  /** Calls staticResolution on the DataItems, then if isConstant() is true for
      all of them, and constantFold is true, returns the result of the
      constantFold() method, otherwise returns the result of the resolvePredicates()
      method. */
  DataItem *resolveDataItems(VectorOfDataItems &dis, StaticContext *context, bool constantFold);
  /** Calls staticResolution on the DataItems, then if isConstantAndHasTimezone() is true for
      all of them, and constantFold is true, returns the result of the
      constantFold() method, otherwise returns the result of the resolvePredicates()
      method. */
  DataItem *resolveDataItemsForDateOrTime(VectorOfDataItems &dis, StaticContext *context, bool constantFold);
  /** Calls staticResolution on the DataItem, then if isConstant() is true for
      it, and constantFold is true, returns the result of the
      constantFold() method, otherwise returns the result of the resolvePredicates()
      method. */
  DataItem *resolveDataItem(DataItem *&di, StaticContext *context, bool constantFold);
  /** Calls staticResolution on the predicates of this DataItem, constant folding
      if possible */
  DataItem *resolvePredicates(StaticContext *context);
  virtual DataItemImpl *resolvePredicate(Predicates::reverse_iterator it, Predicates &newPreds, StaticContext *context);
  /** Performs constant folding on this DataItem, transfering any predicates to
      the returned DataItem */
  DataItem *constantFold(StaticContext *context) const;

  virtual const StaticResolutionContext &getStaticResolutionContext() const;

protected:
  void setType(DataItem::whichType t);
  XPath2MemoryManager* getMemoryManager(void) const;

  class CollapseTreeInternalResult : public LazySequenceResult
  {
  public:
    CollapseTreeInternalResult(const DataItemImpl *di, int flags, DynamicContext *context);

    void getResult(Sequence &toFill, DynamicContext *context) const;
    std::string asString(DynamicContext *context, int indent) const;

  private:
    int _flags;
    const DataItemImpl *_di;
  };

  class PredicateFilterResult : public ResultImpl
  {
  public:
    PredicateFilterResult(const Result &parent, const PredInfo &pred, unsigned int contextSize, DynamicContext *context);

    Item::Ptr next(DynamicContext *context);
    void skip();
    std::string asString(DynamicContext *context, int indent) const;

  private:
    bool todo_;
    Result parent_;
    const PredInfo &pred_;
    unsigned int contextPos_;
    unsigned int contextSize_;
    Item::Ptr first_;
    Item::Ptr second_;
  };

  class NonNumericPredicateFilterResult : public ResultImpl
  {
  public:
    NonNumericPredicateFilterResult(const Result &parent, const PredInfo &pred, unsigned int contextSize, DynamicContext *context);

    Item::Ptr next(DynamicContext *context);
    void skip();
    std::string asString(DynamicContext *context, int indent) const;

  private:
    bool todo_;
    Result parent_;
    const PredInfo &pred_;
    unsigned int contextPos_;
    unsigned int contextSize_;
  };

  class NumericPredicateFilterResult : public ResultImpl
  {
  public:
    NumericPredicateFilterResult(const Result &parent, const PredInfo &pred, unsigned int contextSize, DynamicContext *context);

    Item::Ptr next(DynamicContext *context);
    void skip();
    std::string asString(DynamicContext *context, int indent) const;

  private:
    bool todo_;
    Result parent_;
    const PredInfo &pred_;
    unsigned int contextSize_;
  };

  Predicates _predList;

  StaticResolutionContext _src;

private:
  whichType _type;
  XPath2MemoryManager* _memMgr;
};

#endif
