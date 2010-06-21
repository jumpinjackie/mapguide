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
 * $Id: XQNav.cpp,v 1.22 2006/11/01 16:37:18 jpcs Exp $
 */

#include "../config/xqilla_config.h"
#include <sstream>

#include <xqilla/utils/PrintAST.hpp>
#include <xqilla/ast/XQNav.hpp>
#include <xqilla/ast/XQStep.hpp>
#include <xqilla/runtime/Sequence.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <xqilla/exceptions/TypeErrorException.hpp>
#include <xqilla/items/Node.hpp>
#include <xqilla/ast/StaticResolutionContext.hpp>
#include <xqilla/ast/XQTreatAs.hpp>
#include <xqilla/ast/XQDocumentOrder.hpp>
#include <xqilla/ast/XQPredicate.hpp>
#include <xqilla/runtime/SequenceResult.hpp>
#include <xqilla/items/DatatypeFactory.hpp>
#include <xqilla/context/ContextHelpers.hpp>

XQNav::XQNav(XPath2MemoryManager* memMgr)
  : ASTNodeImpl(memMgr),
    _steps(XQillaAllocator<StepInfo>(memMgr)),
    _sortAdded(false)
{
  setType(NAVIGATION);
}


XQNav::~XQNav()
{
  //no-op
}

Result XQNav::createResult(DynamicContext* context, int flags) const
{
  Steps::const_iterator end = _steps.end();
  Steps::const_iterator it = _steps.begin();

  Result result(0);
  if(it != end) {
    ASTNode *step = it->step;

    result = step->collapseTree(context, flags);
    StaticType st = step->getStaticResolutionContext().getStaticType();

    for(++it; it != end; ++it) {
      if(st.containsType(StaticType::ANY_ATOMIC_TYPE)) {
        result = new IntermediateStepCheckResult(step, result);
      }

      step = it->step;

      if(step->getStaticResolutionContext().isContextSizeUsed()) {
        // We need the context size, so convert to a Sequence to work it out
        Sequence seq(result->toSequence(context));
        result = new NavStepResult(new SequenceResult(step, seq), step, seq.getLength());
      }
      else {
        result = new NavStepResult(result, step, 0);
      }

      st = step->getStaticResolutionContext().getStaticType();
    }

    // the last step allows either nodes or atomic items
    if(st.containsType(StaticType::NODE_TYPE) &&
       st.containsType(StaticType::ANY_ATOMIC_TYPE)) {
      result = new LastStepCheckResult(step, result);
    }
  }

  return result;
}

void XQNav::addStep(const StepInfo &step)
{
  if(step.step->getType() == NAVIGATION) {
    Steps &navSteps = ((XQNav*)step.step)->_steps;
    for(Steps::iterator it2 = navSteps.begin();
        it2 != navSteps.end(); ++it2) {
      _steps.push_back(*it2);
    }
  }
  else {
    _steps.push_back(step);
  }
}

void XQNav::addStepFront(ASTNode* step)
{
  if(step->getType() == NAVIGATION) {
    Steps &navSteps = ((XQNav*)step)->_steps;
    for(Steps::reverse_iterator it2 = navSteps.rbegin();
        it2 != navSteps.rend(); ++it2) {
      _steps.insert(_steps.begin(), *it2);
    }
  }
  else {
    _steps.insert(_steps.begin(), step);
  }
}

ASTNode* XQNav::staticResolution(StaticContext *context)
{
  if(!_sortAdded) {
    _sortAdded = true;
    // Wrap ourselves in a document order sort
    XPath2MemoryManager *mm = context->getMemoryManager();

    ASTNode *result = new (mm) XQDocumentOrder(this, mm);
    result->setLocationInfo(_steps.back().step);
    
    return result->staticResolution(context);
  }

  Steps::iterator begin = _steps.begin();
  Steps::iterator end = _steps.end();
  Steps::iterator it = begin;
  for(; it != end; ++it) {
    // Statically resolve our step
    it->step = it->step->staticResolution(context);
  }

  return this;
}

ASTNode* XQNav::staticTyping(StaticContext *context)
{
  _src.clear();

  AutoContextItemTypeReset contextTypeReset(context, context->getContextItemType());

  unsigned int props = StaticResolutionContext::DOCORDER | StaticResolutionContext::GROUPED |
    StaticResolutionContext::PEER | StaticResolutionContext::SUBTREE | StaticResolutionContext::SAMEDOC |
    StaticResolutionContext::ONENODE;

  Steps newSteps(XQillaAllocator<StepInfo>(context->getMemoryManager()));

  Steps::iterator begin = _steps.begin();
  Steps::iterator end = _steps.end();
  Steps::iterator it = begin;
  for(; it != end; ++it) {
    // Statically resolve our step
    ASTNode *step = it->step->staticTyping(context);
    const StaticResolutionContext &stepSrc = step->getStaticResolutionContext();
    context->setContextItemType(stepSrc.getStaticType());

    props = combineProperties(props, stepSrc.getProperties());

    if(stepSrc.areContextFlagsUsed() || _src.isNoFoldingForced() ||
       stepSrc.getStaticType().containsType(StaticType::ANY_ATOMIC_TYPE) ||
       !stepSrc.isCreative()) {
      if(it != begin) {
        // Remove context item usage
        _src.addExceptContextFlags(stepSrc);
      }
      else {
        _src.add(stepSrc);
      }
    }
    else {
      // Constant fold, by clearing all our previous steps and just having our most recent step.
      // This is only possible because the result of steps has to be nodes, and
      // duplicates are removed, which means we aren't forced to execute a constant
      // step a set number of times.
      newSteps.clear();
      _src.add(stepSrc);
    }

    if(step->getType() == NAVIGATION) {
      Steps &navSteps = ((XQNav*)step)->_steps;
      for(Steps::iterator it2 = navSteps.begin();
          it2 != navSteps.end(); ++it2) {
        newSteps.push_back(it2->step);
      }
    }
    else {
	    newSteps.push_back(step);
    }
  }

  _steps = newSteps;
  newSteps.clear();

  it = _steps.begin();
  end = _steps.end();
  for(; it != end; ++it) {
    // Special case, to optimise //foo or //descendant::foo
    if(it->step->getType() == STEP) {
      XQStep *step = (XQStep*)it->step;
      NodeTest *nodetest = (NodeTest*)step->getNodeTest();

      // Check for a step with a type wildcard
      if(nodetest->getTypeWildcard() && (it + 1) != end) {

        bool usesContextPositionOrSize = false;
        const ASTNode *peek = (it + 1)->step;
        while(peek->getType() == PREDICATE) {
          const XQPredicate *pred = (const XQPredicate*)peek;
          if(pred->getPredicate()->getStaticResolutionContext().isContextPositionUsed() ||
             pred->getPredicate()->getStaticResolutionContext().isContextSizeUsed() ||
             pred->getPredicate()->getStaticResolutionContext().getStaticType().containsType(StaticType::NUMERIC_TYPE)) {
            usesContextPositionOrSize = true;
          }
          peek = pred->getExpression();
        }

        if(peek->getType() == STEP) {
          const XQStep *peekstep = (XQStep*)peek;
          // If the next node is CHILD or DESCENDANT axis, then
          // this step must have children
          if(peekstep->getAxis() == XQStep::CHILD || peekstep->getAxis() == XQStep::DESCENDANT) {

            // Check for a descendant-or-self axis
            if(step->getAxis() == XQStep::DESCENDANT_OR_SELF) {
              if(!usesContextPositionOrSize) {
                // This is a descendant-or-self::node()/child::foo that we can optimise into descendant::foo
                ++it;
                const_cast<XQStep*>(peekstep)->setAxis(XQStep::DESCENDANT);
                // Set the properties to those for descendant axis
                const_cast<StaticResolutionContext&>(peekstep->getStaticResolutionContext()).
                  setProperties(StaticResolutionContext::SUBTREE | StaticResolutionContext::DOCORDER |
                                StaticResolutionContext::GROUPED | StaticResolutionContext::SAMEDOC);
              }
            }

            nodetest->setHasChildren();
          }
          // If the next node is ATTRIBUTE axis, then this step needs to be
          // an element
          else if(peekstep->getAxis() == XQStep::ATTRIBUTE) {
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

  _src.setProperties(props);

  if(!_src.isUsed()) {
    return constantFold(context);
  }
  return this;
}

const XQNav::Steps &XQNav::getSteps() const {
  return _steps;
}

unsigned int XQNav::combineProperties(unsigned int prev_props, unsigned int step_props)
{
  unsigned int new_props = 0;

  if((step_props & StaticResolutionContext::SELF)) {
    new_props |= prev_props;
  }

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

/////////////////////////////////////
// NavStepResult
/////////////////////////////////////

NavStepResult::NavStepResult(const Result &parent, ASTNode *step, size_t contextSize)
  : ResultImpl(step),
    initialised_(false),
    parent_(parent),
    step_(step),
    stepResult_(0),
    contextPos_(0),
    contextSize_(contextSize),
    contextItem_(0)
{
}

Item::Ptr NavStepResult::next(DynamicContext *context)
{
  AutoContextInfoReset autoReset(context);

  context->setContextSize(contextSize_);
  context->setContextPosition(contextPos_);
  context->setContextItem(contextItem_);

  Item::Ptr result = stepResult_->next(context);
  while(result.isNull()) {
    context->testInterrupt();

    autoReset.resetContextInfo();

    contextItem_ = parent_->next(context);
    if(contextItem_.isNull()) {
      parent_ = 0;
      return 0;
    }
    ++contextPos_;

    context->setContextSize(contextSize_);
    context->setContextPosition(contextPos_);
    context->setContextItem(contextItem_);

    stepResult_ = step_->collapseTree(context);
    result = stepResult_->next(context);
  }

  return result;
}

std::string NavStepResult::asString(DynamicContext *context, int indent) const
{
  std::ostringstream oss;
  std::string in(getIndent(indent));

  oss << in << "<step contextSize=\"" << (unsigned long)contextSize_ << "\">" << std::endl;
  if(!parent_.isNull()) {
    oss << in << "  <parent>" << std::endl;
    oss << parent_->asString(context, indent + 2);
    oss << in << "  </parent>" << std::endl;
  }
  oss << PrintAST::print(step_, context, indent + 1);
  oss << in << "</step>" << std::endl;

  return oss.str();
}

IntermediateStepCheckResult::IntermediateStepCheckResult(const LocationInfo *o, const Result &parent)
  : ResultImpl(o),
    parent_(parent)
{
}

Item::Ptr IntermediateStepCheckResult::next(DynamicContext *context)
{
  Item::Ptr result = parent_->next(context);

  // Check it's a node
  if(result != NULLRCP && !result->isNode()) {
    XQThrow(TypeErrorException,X("NavStepResult::next"),
             X("The result of a step expression (StepExpr) is not a sequence of nodes [err:XPTY0019]"));
  }

  return result;
}

std::string IntermediateStepCheckResult::asString(DynamicContext *context, int indent) const
{
  std::ostringstream oss;
  std::string in(getIndent(indent));

  oss << in << "<intermediatestepcheck>" << std::endl;
  if(!parent_.isNull()) {
    oss << in << "  <parent>" << std::endl;
    oss << parent_->asString(context, indent + 2);
    oss << in << "  </parent>" << std::endl;
  }
  oss << in << "</intermediatestepcheck>" << std::endl;

  return oss.str();
}

LastStepCheckResult::LastStepCheckResult(const LocationInfo *o, const Result &parent)
  : ResultImpl(o),
    parent_(parent),
    _nTypeOfItemsInLastStep(0)
{
}

Item::Ptr LastStepCheckResult::next(DynamicContext *context)
{
  Item::Ptr result = parent_->next(context);

  if(result != NULLRCP) {
    // the last step allows either nodes or atomic items
    switch(_nTypeOfItemsInLastStep) {
    case 0: _nTypeOfItemsInLastStep=result->isNode()?1:2; break;
    case 1: if(!result->isNode()) 
      XQThrow(TypeErrorException,X("LastStepCheckResult::next"),
               X("The result of the last step in a path expression contains both nodes and atomic values [err:XPTY0018]"));
      break;
    case 2: if(result->isNode()) 
      XQThrow(TypeErrorException,X("LastStepCheckResult::next"),
               X("The result of the last step in a path expression contains both nodes and atomic values [err:XPTY0018]"));
      break;
    }
  }

  return result;
}

std::string LastStepCheckResult::asString(DynamicContext *context, int indent) const
{
  std::ostringstream oss;
  std::string in(getIndent(indent));

  oss << in << "<laststepcheck>" << std::endl;
  if(!parent_.isNull()) {
    oss << in << "  <parent>" << std::endl;
    oss << parent_->asString(context, indent + 2);
    oss << in << "  </parent>" << std::endl;
  }
  oss << in << "</laststepcheck>" << std::endl;

  return oss.str();
}
