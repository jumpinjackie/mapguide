/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <sstream>

#include <pathan/internal/utils/PrintDataItemTree.hpp>
#include <pathan/dataItem/DataItemNav.hpp>
#include <pathan/dataItem/DataItemStep.hpp>
#include <pathan/Sequence.hpp>
#include <pathan/DynamicContext.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <pathan/exceptions/TypeErrorException.hpp>
#include <pathan/Node.hpp>
#include <pathan/functions/FunctionRoot.hpp>
#include <pathan/dataItem/StaticResolutionContext.hpp>
#include <pathan/dataItem/SequenceResult.hpp>
#include <pathan/internal/factory/DatatypeFactory.hpp>
#include <set>

DataItemNav::DataItemNav(XPath2MemoryManager* memMgr)
	: DataItemImpl(memMgr),
    _isSorted(-1),
    _properties(0),
	  _steps(PathanAllocator<StepInfo>(memMgr))
{
  setType(DataItem::NAVIGATION);
  _gotoRoot = false;
}


DataItemNav::~DataItemNav()
{
  //no-op
}

Result DataItemNav::createResult(DynamicContext* context, int flags) const
{
  Result result(0);
  flags &= ~(RETURN_ONE | RETURN_TWO);

  if(_gotoRoot) {
    result = new GotoRootResult(context);
  }

  Steps::const_iterator end = _steps.end();
  for(Steps::const_iterator it = _steps.begin(); it != end; ++it) {
    if(it->usesContextSize) {
      // We need the context size, so convert to a Sequence to work it out
      Sequence seq(result.toSequence(context));
      result = new StepResult(new SequenceResult(seq), it->step, seq.getLength(), flags, context);
    }
    else if(!it->step->getStaticResolutionContext().areContextFlagsUsed()) {
      result = it->step->collapseTree(context, flags);
    }
    else {
      result = new StepResult(result, it->step, 0, flags, context);
    }

    const StaticType &st = it->step->getStaticResolutionContext().getStaticType();
    if(it == (end-1)) {
      // the last step allows either nodes or atomic items
      if((st.flags & StaticType::NODE_TYPE) &&
         ((st.flags & StaticType::NUMERIC_TYPE) ||
          (st.flags & StaticType::OTHER_TYPE))) {
        result = new LastStepCheckResult(result, context);
      }
    }
    else {
      if((st.flags & StaticType::NUMERIC_TYPE) ||
         (st.flags & StaticType::OTHER_TYPE)) {
        result = new IntermediateStepCheckResult(result, context);
      }
    }
  }

  if(getIsSorted()) {
    return result;
  }
  else if(context->getNodeSetOrdering()==StaticContext::ORDERING_UNORDERED ||
     flags & DataItem::UNORDERED) {
    return new UniqueNodesResult(result, context);
  }
  else {
    return result.sortIntoDocumentOrder(context);
  }
}

void DataItemNav::addStep(DataItemStep::Axis axis, NodeTest* nodeTest)
{
  _steps.push_back(new (getMemoryManager()) DataItemStep(axis, nodeTest, getMemoryManager()));
}

void DataItemNav::addStep(const StepInfo &step)
{
  _steps.push_back(step);
}

void DataItemNav::addStepFront(DataItem* step)
{
  _steps.insert(_steps.begin(), step);
}

void DataItemNav::setGotoRootFirst(bool gotoRoot)
{
  _gotoRoot = gotoRoot;
}

DataItem* DataItemNav::staticResolution(StaticContext *context)
{
  Steps newSteps(PathanAllocator<StepInfo>(context->getMemoryManager()));

  StaticType oldContextItemType = context->getContextItemType();

  if(_gotoRoot) {
    _src.contextItemUsed(true);
    StaticType newContextItemType;
    newContextItemType.flags = StaticType::NODE_TYPE;
    context->setContextItemType(newContextItemType);
  }

  Steps::iterator begin = _steps.begin();
  Steps::iterator end = _steps.end();
  Steps::iterator it = begin;
  for(; it != end; ++it) {
    // Statically resolve our step
    DataItem *step = it->step->staticResolution(context);
    const StaticResolutionContext &stepSrc = step->getStaticResolutionContext();
    context->setContextItemType(stepSrc.getStaticType());

    if(stepSrc.areContextFlagsUsed() || _src.isNoFoldingForced()) {
      newSteps.push_back(StepInfo(step, stepSrc.isContextSizeUsed()));

      if(it != begin || _gotoRoot) {
        // Remove context item usage
        _src.addExceptContextFlags(stepSrc);
      }
      else {
        _src.add(stepSrc);
      }
    }
    else {
      // Constant fold, by clearing all our previous steps (and the root pseudo-step)
      // and just having our most recent step.
      // This is only possible because the result of steps has to be nodes, and
      // duplicates are removed, which means we aren't forced to execute a constant
      // step a set number of times.
      _gotoRoot = false;
      newSteps.clear();
      newSteps.push_back(StepInfo(step, stepSrc.isContextSizeUsed()));
      _src.add(stepSrc);
    }
  }

  _steps = newSteps;
  newSteps.clear();

  it = _steps.begin();
  end = _steps.end();
  for(; it != end; ++it) {
    // Special case, to optimise //foo or //descendant::foo
    if(it->step->getType() == DataItem::STEP) {
      DataItemStep *step = (DataItemStep*)it->step;
      NodeTest *nodetest = (NodeTest*)step->getNodeTest();

      // Check for a step with a type wildcard
      if(nodetest->getTypeWildcard() && (it + 1) != end) {
        const DataItem *peek = (it + 1)->step;
        if(peek->getType() == DataItem::STEP) {
          const DataItemStep *peekstep = (DataItemStep*)peek;
          // If the next node is CHILD or DESCENDANT axis, then
          // this step must have children
          if(peekstep->getAxis() == DataItemStep::CHILD || peekstep->getAxis() == DataItemStep::DESCENDANT) {

            // Check for a descendant-or-self axis
            if(step->getAxis() == DataItemStep::DESCENDANT_OR_SELF &&
               step->getPredicates().empty()) {

              bool usesContextPositionOrSize = false;
              Predicates::const_iterator it2 = peekstep->getPredicates().begin();
              Predicates::const_iterator end2 = peekstep->getPredicates().end();
              for(; it2 != end2; ++it2) {
                if(it2->pred->getStaticResolutionContext().isContextPositionUsed() ||
                   it2->pred->getStaticResolutionContext().isContextSizeUsed() ||
                   it2->pred->getStaticResolutionContext().getStaticType().flags & StaticType::NUMERIC_TYPE) {
                  usesContextPositionOrSize = true;
                  break;
                }
              }

              if(!usesContextPositionOrSize) {
                // This is a descendant-or-self::node()/child::foo that we can optimise into descendant::foo
                ++it;
                const_cast<DataItemStep*>(peekstep)->setAxis(DataItemStep::DESCENDANT);
              }
            }

            nodetest->setHasChildren();
          }
          // If the next node is ATTRIBUTE axis, then this step needs to be
          // an element
          else if(peekstep->getAxis() == DataItemStep::ATTRIBUTE) {
            nodetest->setTypeWildcard(false);
            nodetest->setNodeType(Node::element_string);
          }
        }
      }
    }

    newSteps.push_back(*it);
  }

  _steps = newSteps;

  if(!_steps.empty()) {
    _src.getStaticType() = _steps.back().step->getStaticResolutionContext().getStaticType();
  }
  else if(_gotoRoot) {
    _src.getStaticType().flags = StaticType::NODE_TYPE;
  }

  getIsSorted(); // Calculate the properties

  context->setContextItemType(oldContextItemType);

  if(_src.isUsed()) {
    return resolvePredicates(context);
  }
  else {
    return constantFold(context);
  }
}

bool DataItemNav::getGotoRootFirst() const {
  return _gotoRoot;
}

const DataItemNav::Steps &DataItemNav::getSteps() const {
  return _steps;
}

unsigned int DataItemNav::combineProperties(unsigned int prev_props, unsigned int step_props)
{
  unsigned int new_props = 0;
  if((prev_props & StaticResolutionContext::ONENODE) && (step_props & StaticResolutionContext::DOCORDER)) {
    // If there was only one input node, and the step is in document order
    // then we are still in document order
    new_props |= StaticResolutionContext::DOCORDER | StaticResolutionContext::GROUPED;
  }

  if((prev_props & StaticResolutionContext::DOCORDER) && (prev_props & StaticResolutionContext::PEER) &&
     (step_props & StaticResolutionContext::DOCORDER) && (step_props & StaticResolutionContext::SUBTREE)) {
    // We keep the DOCORDER property, along with a few others that come for free
    new_props |= StaticResolutionContext::DOCORDER | StaticResolutionContext::GROUPED | StaticResolutionContext::SUBTREE;
    if(step_props & StaticResolutionContext::PEER) {
      new_props |= StaticResolutionContext::PEER;
    }
  }

  if((prev_props & StaticResolutionContext::GROUPED) && (step_props & StaticResolutionContext::SAMEDOC)) {
    // If the step keeps in the SAMEDOC, then the GROUPED property is maintained
    new_props |= StaticResolutionContext::GROUPED;
  }

  if((prev_props & StaticResolutionContext::SUBTREE) && (step_props & StaticResolutionContext::SUBTREE)) {
    // If both are SUBTREE, then we still have SUBTREE
    new_props |= StaticResolutionContext::SUBTREE;
  }

  if((prev_props & StaticResolutionContext::SAMEDOC) && (step_props & StaticResolutionContext::SAMEDOC)) {
    // If both are SAMEDOC, then we still have SAMEDOC
    new_props |= StaticResolutionContext::SAMEDOC;
  }

  if((prev_props & StaticResolutionContext::ONENODE) && (step_props & StaticResolutionContext::ONENODE)) {
    // If both are ONENODE, then we still have ONENODE
    new_props |= StaticResolutionContext::ONENODE;
  }

  return new_props;
}

// If the results of this navigation are already sorted,
// return true.  _isSorted is a cache of the answer,
// which is obtained by walking the steps.
//  -1 - unknown
//  0 - not sorted
//  1 - sorted
bool DataItemNav::getIsSorted() const
{
  if (_isSorted == -1) {
    Steps::const_iterator end = _steps.end();
    Steps::const_iterator it = _steps.begin();

    if(_gotoRoot) {
      _properties = StaticResolutionContext::DOCORDER | StaticResolutionContext::GROUPED | StaticResolutionContext::PEER | StaticResolutionContext::ONENODE;
    }
    else {
      _properties = it->step->getStaticResolutionContext().getProperties();
      ++it;
    }

    for(; it != end; ++it) {
      _properties = combineProperties(_properties, it->step->getStaticResolutionContext().getProperties());
    }

    _isSorted = (_properties & StaticResolutionContext::DOCORDER) ? 1 : 0;
    const_cast<DataItemNav*>(this)->_src.setProperties(_properties | StaticResolutionContext::DOCORDER | StaticResolutionContext::GROUPED);
  }
  return (_isSorted > 0);
}

/////////////////////////////////////
// GotoRootResult
/////////////////////////////////////

DataItemNav::GotoRootResult::GotoRootResult(DynamicContext *context)
  : SingleResult(context)
{
}

Item::Ptr DataItemNav::GotoRootResult::getSingleResult(DynamicContext *context) const
{
  // Do equivalent of root()
  const Item::Ptr contextItem = context->getContextItem();
  if(contextItem != NULLRCP && contextItem->isNode()) {
    return FunctionRoot::root((const Node::Ptr)contextItem, context);
  } else {
    DSLthrow(TypeErrorException,X("DataItemNav::collapseTreeInternal"), X("An attempt to navigate when the Context Item was not a node was made [err:XPTY0020]"));
  }
  return 0;
}

std::string DataItemNav::GotoRootResult::asString(DynamicContext *context, int indent) const
{
  std::ostringstream oss;
  std::string in(getIndent(indent));

  oss << in << "<nav_goto_root/>" << std::endl;

  return oss.str();
}

/////////////////////////////////////
// StepResult
/////////////////////////////////////

DataItemNav::StepResult::StepResult(const Result &parent, DataItem *step, unsigned int contextSize, int flags, DynamicContext *context)
  : ResultImpl(context),
    initialised_(false),
    flags_(flags),
    parent_(parent),
    step_(step),
    stepResult_(0),
    contextPos_(0),
    contextSize_(contextSize),
    contextItem_(0)
{
}

Item::Ptr DataItemNav::StepResult::next(DynamicContext *context)
{
  unsigned int oldContextSize = context->getContextSize();
  unsigned int oldContextPosition = context->getContextPosition();
  const Item::Ptr oldContextItem = context->getContextItem();

  context->setContextSize(contextSize_);
  context->setContextPosition(contextPos_);
  context->setContextItem(contextItem_);

  Item::Ptr result = 0;
  while(true) {
    result = stepResult_.next(context);
    if(result == NULLRCP) {
      if(!initialised_ && parent_.isNull()) {
        initialised_ = true;
        // We have no parent step, so navigate from the context item
        contextItem_ = oldContextItem;
        contextPos_ = oldContextPosition;
        contextSize_ = oldContextSize;
      }
      else {
        context->setContextSize(oldContextSize);
        context->setContextPosition(oldContextPosition);
        context->setContextItem(oldContextItem);

        contextItem_ = parent_.next(context);
        if(contextItem_ == NULLRCP) {
          parent_ = 0;
          stepResult_ = 0;
          return 0;
        }
        ++contextPos_;
      }

      context->setContextSize(contextSize_);
      context->setContextPosition(contextPos_);
      context->setContextItem(contextItem_);

      stepResult_ = step_->collapseTree(context, flags_);
    }
    else {
      // no duplicate check
      context->setContextSize(oldContextSize);
      context->setContextPosition(oldContextPosition);
      context->setContextItem(oldContextItem);
      return result;
    }
  }

  return 0;
}

void DataItemNav::StepResult::skip()
{
	stepResult_ = 0;
	parent_.skip();
}

std::string DataItemNav::StepResult::asString(DynamicContext *context, int indent) const
{
  std::ostringstream oss;
  std::string in(getIndent(indent));

  oss << in << "<step contextSize=\"" << contextSize_ << "\">" << std::endl;
  if(!parent_.isNull()) {
    oss << in << "  <parent>" << std::endl;
    oss << parent_.asString(context, indent + 2);
    oss << in << "  </parent>" << std::endl;
  }
  oss << in << "  <dataitem>" << std::endl;
  oss << PrintDataItemTree::print(step_, context, indent + 2);
  oss << in << "  </dataitem>" << std::endl;
  oss << in << "</step>" << std::endl;

  return oss.str();
}

DataItemNav::IntermediateStepCheckResult::IntermediateStepCheckResult(const Result &parent, DynamicContext *context)
  : ResultImpl(context),
    parent_(parent)
{
}

Item::Ptr DataItemNav::IntermediateStepCheckResult::next(DynamicContext *context)
{
  Item::Ptr result = parent_.next(context);

  // Check it's a node
  if(result != NULLRCP && !result->isNode()) {
    DSLthrow(TypeErrorException,X("DataItemNav::StepResult::next"),
             X("The result of a step expression (StepExpr) is not a sequence of nodes [err:XPTY0019]"));
  }

  return result;
}

void DataItemNav::IntermediateStepCheckResult::skip()
{
	parent_.skip();
}

std::string DataItemNav::IntermediateStepCheckResult::asString(DynamicContext *context, int indent) const
{
  std::ostringstream oss;
  std::string in(getIndent(indent));

  oss << in << "<intermediatestepcheck>" << std::endl;
  if(!parent_.isNull()) {
    oss << in << "  <parent>" << std::endl;
    oss << parent_.asString(context, indent + 2);
    oss << in << "  </parent>" << std::endl;
  }
  oss << in << "</intermediatestepcheck>" << std::endl;

  return oss.str();
}

DataItemNav::LastStepCheckResult::LastStepCheckResult(const Result &parent, DynamicContext *context)
  : ResultImpl(context),
    parent_(parent),
    _nTypeOfItemsInLastStep(0)
{
}

Item::Ptr DataItemNav::LastStepCheckResult::next(DynamicContext *context)
{
  Item::Ptr result = parent_.next(context);

  if(result != NULLRCP) {
    // the last step allows either nodes or atomic items
    switch(_nTypeOfItemsInLastStep) {
    case 0: _nTypeOfItemsInLastStep=result->isNode()?1:2; break;
    case 1: if(!result->isNode()) 
      DSLthrow(TypeErrorException,X("DataItemNav::StepResult::next"),
               X("The result of the last step in a path expression contains both nodes and atomic values [err:XPTY0018]"));
      break;
    case 2: if(result->isNode()) 
      DSLthrow(TypeErrorException,X("DataItemNav::StepResult::next"),
               X("The result of the last step in a path expression contains both nodes and atomic values [err:XPTY0018]"));
      break;
    }
  }

  return result;
}

void DataItemNav::LastStepCheckResult::skip()
{
	parent_.skip();
}

std::string DataItemNav::LastStepCheckResult::asString(DynamicContext *context, int indent) const
{
  std::ostringstream oss;
  std::string in(getIndent(indent));

  oss << in << "<laststepcheck>" << std::endl;
  if(!parent_.isNull()) {
    oss << in << "  <parent>" << std::endl;
    oss << parent_.asString(context, indent + 2);
    oss << in << "  </parent>" << std::endl;
  }
  oss << in << "</laststepcheck>" << std::endl;

  return oss.str();
}

DataItemNav::UniqueNodesResult::UniqueNodesResult(const Result &parent, DynamicContext *context)
  : ResultImpl(context),
    parent_(parent),
    nTypeOfItemsInLastStep_(0),
    noDups_(uniqueLessThanCompareFn(context))
{
}

Item::Ptr DataItemNav::UniqueNodesResult::next(DynamicContext *context)
{
  Item::Ptr result = parent_.next(context);

  if(nTypeOfItemsInLastStep_ == 0 && result.notNull())
	  nTypeOfItemsInLastStep_ = result->isNode() ? 1 : 2;

  if(nTypeOfItemsInLastStep_ == 1) {
	  while(result.notNull()) {
		  if(noDups_.insert(result).second) break;
		  else result = parent_.next(context);
	  }
  }

  return result;
}

void DataItemNav::UniqueNodesResult::skip()
{
	parent_.skip();
}

std::string DataItemNav::UniqueNodesResult::asString(DynamicContext *context, int indent) const
{
  std::ostringstream oss;
  std::string in(getIndent(indent));

  oss << in << "<uniqueNodesResult>" << std::endl;
  if(!parent_.isNull()) {
    oss << in << "  <parent>" << std::endl;
    oss << parent_.asString(context, indent + 2);
    oss << in << "  </parent>" << std::endl;
  }
  oss << in << "</uniqueNodesResult>" << std::endl;

  return oss.str();
}

DataItemNav::UniqueNodesResult::uniqueLessThanCompareFn::
uniqueLessThanCompareFn(const DynamicContext *context)
  : context_(context)
{
}

bool DataItemNav::UniqueNodesResult::uniqueLessThanCompareFn::
operator()(const Node::Ptr &first, const Node::Ptr &second)
{
  return first->uniqueLessThan(second, context_);
}
