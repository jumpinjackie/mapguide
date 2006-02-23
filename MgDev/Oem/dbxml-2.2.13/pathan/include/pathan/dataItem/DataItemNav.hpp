/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _DataItemNav_HPP
#define _DataItemNav_HPP

#include <set>
#include <vector>
#include "../Pathan.hpp"

#include "DataItemImpl.hpp"
#include "../Sequence.hpp"

#include "SingleResult.hpp"
#include "../Node.hpp"
#include "DataItemStep.hpp"
#include <xercesc/dom/DOMNode.hpp>

///this class calculates a node list based on a context
class PATHAN_EXPORT DataItemNav : public DataItemImpl
{
public:
  class PATHAN_EXPORT StepInfo {
  public:
    StepInfo() : usesContextSize(false), step(0) {}
    StepInfo(DataItem *s) : usesContextSize(false), step(s) {}
    StepInfo(DataItem *s, bool cs) : usesContextSize(cs), step(s) {}
    bool usesContextSize;
    DataItem *step;
  };
  typedef std::vector<StepInfo,PathanAllocator<StepInfo> > Steps;

  DataItemNav(XPath2MemoryManager* memMgr);
  virtual ~DataItemNav();
	

  /** Add a new NavigationStep to the sequence of navigation steps to
      be performed. This operation is used by parse tree classes to
      turn themselves into an optimised, reusable navigation. **/
  void addStep(const StepInfo &step);

  /** Add a new NavigationStep to the sequence of navigation steps to
      be performed. This operation is used by parse tree classes to
      turn themselves into an optimised, reusable navigation. **/
  void addStep(DataItemStep::Axis axis, NodeTest* nodeTest);

  /** Add a new NavigationStep to the start of the sequence of
      navigation steps to be performed.**/
  void addStepFront(DataItem* step);

  ///set the "go to root of tree first" flag.
  void setGotoRootFirst(bool gotoRoot);

  virtual Result createResult(DynamicContext* context, int flags=0) const;

  virtual DataItem* staticResolution(StaticContext *context);

  bool getGotoRootFirst() const;
  const Steps &getSteps() const;
  bool getIsSorted() const;

protected:
  static unsigned int combineProperties(unsigned int prev_props, unsigned int step_props);

  class PATHAN_EXPORT GotoRootResult : public SingleResult
  {
  public:
    GotoRootResult(DynamicContext *context);
    Item::Ptr getSingleResult(DynamicContext *context) const;
    std::string asString(DynamicContext *context, int indent) const;
  };

  class PATHAN_EXPORT StepResult : public ResultImpl
  {
  public:
    StepResult(const Result &parent, DataItem *step, unsigned int contextSize, int flags, DynamicContext *context);

    Item::Ptr next(DynamicContext *context);
    void skip();
    std::string asString(DynamicContext *context, int indent) const;

  private:
    bool initialised_;
    int flags_;
    Result parent_;
    const DataItem *step_;
    Result stepResult_;
    unsigned int contextPos_;
    unsigned int contextSize_;
    Item::Ptr contextItem_;
  };

  class PATHAN_EXPORT IntermediateStepCheckResult : public ResultImpl
  {
  public:
    IntermediateStepCheckResult(const Result &parent, DynamicContext *context);

    Item::Ptr next(DynamicContext *context);
    void skip();
    std::string asString(DynamicContext *context, int indent) const;

  private:
    Result parent_;
  };

  class PATHAN_EXPORT LastStepCheckResult : public ResultImpl
  {
  public:
    LastStepCheckResult(const Result &parent, DynamicContext *context);

    Item::Ptr next(DynamicContext *context);
    void skip();
    std::string asString(DynamicContext *context, int indent) const;

  private:
    Result parent_;
    int _nTypeOfItemsInLastStep;
  };

  class PATHAN_EXPORT UniqueNodesResult : public ResultImpl
  {
  public:
    UniqueNodesResult(const Result &parent, DynamicContext *context);

    Item::Ptr next(DynamicContext *context);
    void skip();
    std::string asString(DynamicContext *context, int indent) const;

  private:
    class uniqueLessThanCompareFn {
    public:
      uniqueLessThanCompareFn(const DynamicContext *context);
      bool operator()(const Node::Ptr &first, const Node::Ptr &second);

    private:
      const DynamicContext *context_;
    };
    typedef std::set<Node::Ptr, uniqueLessThanCompareFn> NoDuplicatesSet;

    Result parent_;
    int nTypeOfItemsInLastStep_;
    NoDuplicatesSet noDups_;
  };

  bool _gotoRoot;
  mutable int _isSorted;
  mutable unsigned int _properties;
  //list of steps to be performed.
  Steps _steps;
};

#endif


