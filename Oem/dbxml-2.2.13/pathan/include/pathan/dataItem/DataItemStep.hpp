/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _DATAITEMSTEP_HPP
#define _DATAITEMSTEP_HPP

#include <pathan/Pathan.hpp>

#include <pathan/dataItem/DataItemImpl.hpp>

class NodeTest;

///this class calculates a node list based on a context
class PATHAN_EXPORT DataItemStep : public DataItemImpl
{
public:
  enum Axis {
    ANCESTOR,
    ANCESTOR_OR_SELF,
    ATTRIBUTE,
    CHILD,
    DESCENDANT,
    DESCENDANT_OR_SELF,
    FOLLOWING,
    FOLLOWING_SIBLING,
    NAMESPACE,
    PARENT,
    PRECEDING,
    PRECEDING_SIBLING,
    SELF
  };

  //constuctor that takes the step this DataItem is refuring to
  DataItemStep(Axis axis, NodeTest* nodeTest, XPath2MemoryManager* memMgr);
  ~DataItemStep();

  virtual DataItem* staticResolution(StaticContext *context);
  virtual Result createResult(DynamicContext* context, int flags=0) const;
  virtual Result postPredicateResultHook(Result &result, DynamicContext* context, int flags=0) const;

  const NodeTest *getNodeTest() const;
  void setNodeTest(NodeTest *nodeTest);

  Axis getAxis() const;
  void setAxis(Axis axis);

  static bool isForwardAxis(Axis axis);

private:
  /**
   * Lazily apply an axis and NodeTest filter to the context node
   */
  class StepResult : public ResultImpl
  {
  public:
    StepResult(const DataItemStep *step, DynamicContext *context);
    virtual ~StepResult() {}

    Item::Ptr next(DynamicContext *context);
    std::string asString(DynamicContext *context, int indent) const;

  protected:
    bool toDo_;
    Result result_;
    const DataItemStep *step_;
  };

  NodeTest* nodeTest_;
  Axis axis_;
};

#endif
