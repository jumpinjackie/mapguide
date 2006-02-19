/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <sstream>

#include <pathan/internal/utils/PrintDataItemTree.hpp>
#include <pathan/dataItem/DataItemImpl.hpp>
#include <pathan/XPath2Utils.hpp>
#include "../utils/NumUtils.hpp"
#include <pathan/dataItem/DataItem.hpp>
#include <pathan/Sequence.hpp>
#include <pathan/exceptions/XPath2TypeCastException.hpp>
#include <pathan/Numeric.hpp>
#include <pathan/Node.hpp>
#include <pathan/ATBooleanOrDerived.hpp>
#include <pathan/ATDecimalOrDerived.hpp>
#include <pathan/DateOrTimeType.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/internal/context/DynamicContextImpl.hpp>
#include <pathan/internal/factory/DatatypeFactory.hpp>
#include <pathan/dataItem/DataItemSequence.hpp>
#include <xercesc/validators/schema/SchemaSymbols.hpp>
#include <pathan/dataItem/SequenceResult.hpp>
#include <pathan/PathanFactory.hpp>

DataItemImpl::DataItemImpl(XPath2MemoryManager* memMgr)
  : _predList(PathanAllocator<PredInfo>(memMgr)),
    _src(memMgr),
    _memMgr(memMgr)
{
  // Nothing to do
}

DataItemImpl::~DataItemImpl(void)
{
  //no-op
}

void DataItemImpl::addPredicates(const VectorOfDataItems &steps)
{
    for(VectorOfDataItems::const_iterator i=steps.begin();i!=steps.end();i++)
        _predList.push_back(PredInfo(*i));
}

void DataItemImpl::addPredicates(const Predicates &steps)
{
    _predList.insert(_predList.end(),steps.begin(),steps.end());
}

const DataItemImpl::Predicates& DataItemImpl::getPredicates(void) const
{
  return _predList;
}


/** Returns true if this DataItem has no predicates, and is an instance of
    DataItemSequence or DataItemLiteral */
bool DataItemImpl::isConstant() const
{
  return (_type == DataItem::SEQUENCE || _type == DataItem::LITERAL) && _predList.empty();
}

/** Overridden in DataItemSequence and DataItemLiteral */
bool DataItemImpl::isConstantAndHasTimezone(StaticContext *context) const
{
  return false;
}

DataItem::whichType DataItemImpl::getType(void) const
{
  return _type;
}

void DataItemImpl::setType(DataItem::whichType t)
{
  _type = t;
}

bool DataItemImpl::isSingleNumericConstant(StaticContext *context) const
{
  return false;
}

Result DataItemImpl::createPredicateResult(Result &parent, const PredInfo &p, DynamicContext *context) const
{
  const StaticResolutionContext &src = p.pred->getStaticResolutionContext();

  unsigned int contextSize = 0;
  if(src.isContextSizeUsed()) {
    // We need the context size, so convert to a Sequence to work it out
    Sequence seq(parent.toSequence(context));
    contextSize = seq.getLength();
    parent = new SequenceResult(seq);
  }
  if(src.getStaticType().flags == StaticType::NUMERIC_TYPE &&
     !src.isContextItemUsed() && !src.isContextPositionUsed()) {
    // It only contains numeric type results, and doesn't use the context
    // item or position
    return new NumericPredicateFilterResult(parent, p, contextSize, context);
  }
  else if((src.getStaticType().flags & StaticType::NUMERIC_TYPE) == 0) {
    // It only contains non-numeric results
    return new NonNumericPredicateFilterResult(parent, p, contextSize, context);
  }
  else {
    return new PredicateFilterResult(parent, p, contextSize, context);
  }
}

Result DataItemImpl::collapseTree(DynamicContext* context, int flags) const
{
  Result result = createResult(context, flags);

  for(Predicates::const_iterator it = _predList.begin(); it != _predList.end(); ++it) {
    result = createPredicateResult(result, *it, context);
  }

  return postPredicateResultHook(result, context, flags);
}

Result DataItemImpl::postPredicateResultHook(Result &result, DynamicContext* context, int flags) const
{
  return result;
}

Result DataItemImpl::createResult(DynamicContext* context, int flags) const
{
  return new CollapseTreeInternalResult(this, flags, context);
}

Sequence DataItemImpl::collapseTreeInternal(DynamicContext* context, int flags) const
{
  return Sequence(context->getMemoryManager());
}

/** Calls staticResolution on the DataItem, then if isConstant() is true for
    it, and constantFold is true, returns the result of the
    constantFold() method, otherwise returns the result of the resolvePredicates()
    method. */
DataItem *DataItemImpl::resolveDataItem(DataItem *&di, StaticContext *context, bool cf)
{
  di = di->staticResolution(context);
  _src.getStaticType() = di->getStaticResolutionContext().getStaticType();
  _src.add(di->getStaticResolutionContext());
  if(di->isConstant() && cf) {
    return constantFold(context);
  }
  else {
    return resolvePredicates(context);
  }
}

/** Calls staticResolution on the DataItems, then if isConstant() is true for
    all of them, and constantFold is true, returns the result of the
    constantFold() method, otherwise returns the result of the resolvePredicates()
    method. */
DataItem *DataItemImpl::resolveDataItems(VectorOfDataItems &dis, StaticContext *context, bool cf)
{
  bool allConstant = true;
  for(VectorOfDataItems::iterator i = dis.begin(); i != dis.end(); ++i) {
    *i = (*i)->staticResolution(context);
    _src.add((*i)->getStaticResolutionContext());
    if(!(*i)->isConstant()) {
      allConstant = false;
    }
  }

  if(allConstant && cf) {
    return constantFold(context);
  }
  else {
    return resolvePredicates(context);
  }
}

/** Calls staticResolution on the DataItems, then if isConstantAndHasTimezone() is true for
    all of them, and constantFold is true, returns the result of the
    constantFold() method, otherwise returns the result of the resolvePredicates()
    method. */
DataItem *DataItemImpl::resolveDataItemsForDateOrTime(VectorOfDataItems &dis, StaticContext *context, bool cf)
{
  bool allConstant = true;
  for(VectorOfDataItems::iterator i = dis.begin(); i != dis.end(); ++i) {
    *i = (*i)->staticResolution(context);
    _src.add((*i)->getStaticResolutionContext());
    if(!(*i)->isConstantAndHasTimezone(context)) {
      allConstant = false;
      if((*i)->isConstant()) {
        _src.implicitTimezoneUsed(true);
      }
    }
  }

  if(allConstant && cf) {
    return constantFold(context);
  }
  else {
    return resolvePredicates(context);
  }
}

/** Calls staticResolution on the predicates of this DataItem, constant folding
    if possible */
DataItem *DataItemImpl::resolvePredicates(StaticContext *context)
{
  StaticType oldContextItemType = context->getContextItemType();
  context->setContextItemType(_src.getStaticType());

  Predicates newPreds(PathanAllocator<PredInfo>(context->getMemoryManager()));
  DataItemImpl *result = resolvePredicate(_predList.rbegin(), newPreds, context);
  result->_predList = newPreds;

  context->setContextItemType(oldContextItemType);

  return result;
}

/** Calls staticResolution on the predicates of this DataItem, constant folding
    if possible */
DataItemImpl *DataItemImpl::resolvePredicate(Predicates::reverse_iterator it, Predicates &newPreds, StaticContext *context)
{
  if(it == _predList.rend()) {
    return this;
  }
  else {
    DataItemImpl *newDI = resolvePredicate(it + 1, newPreds, context);

    // A predicate before us has short circuited the predicate resolving,
    // so we'll just pass back it's result.
    if(newDI != this) {
      return newDI;
    }

    DataItem *pred = it->pred->staticResolution(context);
    const StaticResolutionContext &newSrc = pred->getStaticResolutionContext();

    if(!newSrc.isUsed() && !pred->isSingleNumericConstant(context)) {
      // It's not a numeric constant
      AutoRelease<DynamicContext> dContext(context->createDynamicContext());
      dContext->setMemoryManager(context->getMemoryManager());
      if(pred->collapseTree(dContext).getEffectiveBooleanValue(dContext)) {
        // We have a true predicate, so don't add it to the new predicate list
        return this;
      }
      else {
        // We have a false predicate, which is constant folded to an empty sequence,
        // short circuiting the subsequent predicate resolutions
        newPreds.clear();
        return new (getMemoryManager()) DataItemSequence(getMemoryManager());
      }
    }
    else {
      // It's not constant, or it's numeric, so add it to the new predicate list, and return
      newPreds.push_back(pred);

      // Remove context item usage
      _src.addExceptContextFlags(newSrc);

      return this;
    }
  }
  return this;  // not reached; shut up compiler
}

/** Performs constant folding on this DataItem, transfering any predicates to
    the returned DataItem */
DataItem *DataItemImpl::constantFold(StaticContext *context) const
{
  AutoRelease<DynamicContext> dContext(context->createDynamicContext());
  dContext->setMemoryManager(context->getMemoryManager());
  Result result = createResult(dContext);
  DataItem* newBlock = new (getMemoryManager()) DataItemSequence(result, dContext, getMemoryManager());
  newBlock->addPredicates(_predList);
  return newBlock->staticResolution(context);
}

XPath2MemoryManager* DataItemImpl::getMemoryManager(void) const {

  return _memMgr;
}

const StaticResolutionContext &DataItemImpl::getStaticResolutionContext() const
{
  return _src;
}

/////////////////////////////////////
// CollapseTreeInternalResult
/////////////////////////////////////

DataItemImpl::CollapseTreeInternalResult::CollapseTreeInternalResult(const DataItemImpl *di, int flags, DynamicContext *context)
  : LazySequenceResult(context),
    _flags(flags),
    _di(di)
{
}

void DataItemImpl::CollapseTreeInternalResult::getResult(Sequence &toFill, DynamicContext *context) const
{
  const Sequence &tarts = _di->collapseTreeInternal(context, _flags);
  toFill = tarts;
}

std::string DataItemImpl::CollapseTreeInternalResult::asString(DynamicContext *context, int indent) const
{
  std::ostringstream oss;
  std::string in(getIndent(indent));

  oss << in << "<collapsetreeinternal/>" << std::endl;

  return oss.str();
}

/////////////////////////////////////
// PredicateFilterResult
/////////////////////////////////////

class AutoContextInfoReset
{
public:
  AutoContextInfoReset(DynamicContext* context)
    : oldContextItem(context->getContextItem()),
      oldContextPosition(context->getContextPosition()),
      oldContextSize(context->getContextSize()),
      context_(context)
  {
  }

  ~AutoContextInfoReset()
  {
    resetContextInfo();
  }

  void resetContextInfo()
  {
    context_->setContextItem(oldContextItem);
    context_->setContextPosition(oldContextPosition);
    context_->setContextSize(oldContextSize);
  }

  Item::Ptr oldContextItem;
  unsigned int oldContextPosition;
  unsigned int oldContextSize;

private:
  DynamicContext* context_;
};

DataItemImpl::PredicateFilterResult::PredicateFilterResult(const Result &parent, const PredInfo &pred, unsigned int contextSize, DynamicContext *context)
  : ResultImpl(context),
    todo_(true),
    parent_(parent),
    pred_(pred),
    contextPos_(0),
    contextSize_(contextSize)
{
}

Item::Ptr DataItemImpl::PredicateFilterResult::next(DynamicContext *context)
{
  AutoContextInfoReset autoReset(context);

  bool contextUsed = pred_.pred->getStaticResolutionContext().isContextItemUsed() ||
    pred_.pred->getStaticResolutionContext().isContextPositionUsed();

  Item::Ptr result = 0;
  while(result == NULLRCP) {
    result = parent_.next(context);
    if(result == NULLRCP) {
      parent_ = 0;
      return 0;
    }

    ++contextPos_;

    if(todo_ || contextUsed) {
      todo_ = false;

      context->setContextSize(contextSize_);
      context->setContextPosition(contextPos_);
      context->setContextItem(result);

      Result pred_result = pred_.pred->collapseTree(context, DataItem::UNORDERED);
      first_ = pred_result.next(context);
      if(first_ != NULLRCP) {
        second_ = pred_result.next(context);
      }

      autoReset.resetContextInfo();
    }

    // 3.2.2 ...
    // The predicate truth value is derived by applying the following rules, in order:
    // 1) If the value of the predicate expression is an atomic value of a numeric type, the predicate truth
    // value is true if and only if the value of the predicate expression is equal to the context position.
    if(first_ != NULLRCP && second_ == NULLRCP && first_->isAtomicValue() &&
       ((const AnyAtomicType::Ptr)first_)->isNumericValue()) {
      const Numeric::Ptr num = (const Numeric::Ptr)first_;
      if(!num->equals((const AnyAtomicType::Ptr)context->getPathanFactory()->createInteger((long)contextPos_, context), context)) {
        result = 0;
      }
      else if(!contextUsed) {
        parent_ = 0;
      }
    }
    else {
      // 2) Otherwise, the predicate truth value is the effective boolean value of the predicate expression
      if(!ResultImpl::getEffectiveBooleanValue(first_, second_, context)) {
        result = 0;
      }
    }
  }

  return result;
}

void DataItemImpl::PredicateFilterResult::skip()
{
  parent_.skip();
}

std::string DataItemImpl::PredicateFilterResult::asString(DynamicContext *context, int indent) const
{
  std::ostringstream oss;
  std::string in(getIndent(indent));

  oss << in << "<predicate contextSize=\"" << contextSize_ << "\">" << std::endl;
  if(!parent_.isNull()) {
    oss << in << "  <parent>" << std::endl;
    oss << parent_.asString(context, indent + 2);
    oss << in << "  </parent>" << std::endl;
  }
  oss << in << "  <dataitem>" << std::endl;
  oss << PrintDataItemTree::print(pred_.pred, context, indent + 2);
  oss << in << "  </dataitem>" << std::endl;
  oss << in << "</predicate>" << std::endl;

  return oss.str();
}

/////////////////////////////////////
// NonNumericPredicateFilterResult
/////////////////////////////////////

DataItemImpl::NonNumericPredicateFilterResult::NonNumericPredicateFilterResult(const Result &parent, const PredInfo &pred, unsigned int contextSize, DynamicContext *context)
  : ResultImpl(context),
    todo_(true),
    parent_(parent),
    pred_(pred),
    contextPos_(0),
    contextSize_(contextSize)
{
}

Item::Ptr DataItemImpl::NonNumericPredicateFilterResult::next(DynamicContext *context)
{
  AutoContextInfoReset autoReset(context);

  bool contextUsed = pred_.pred->getStaticResolutionContext().isContextItemUsed() ||
    pred_.pred->getStaticResolutionContext().isContextPositionUsed();

  Item::Ptr result = 0;
  while(result == NULLRCP) {
    result = parent_.next(context);
    if(result == NULLRCP) {
      parent_ = 0;
      return 0;
    }

    ++contextPos_;

    if(todo_ || contextUsed) {
      todo_ = false;

      context->setContextSize(contextSize_);
      context->setContextPosition(contextPos_);
      context->setContextItem(result);

      // 2) Otherwise, the predicate truth value is the effective boolean value of the predicate expression
      if(!pred_.pred->collapseTree(context, DataItem::UNORDERED).getEffectiveBooleanValue(context)) {
        result = 0;
        if(!contextUsed) {
          parent_ = 0;
        }
      }

      autoReset.resetContextInfo();
    }
  }

  return result;
}

void DataItemImpl::NonNumericPredicateFilterResult::skip()
{
  parent_.skip();
}

std::string DataItemImpl::NonNumericPredicateFilterResult::asString(DynamicContext *context, int indent) const
{
  std::ostringstream oss;
  std::string in(getIndent(indent));

  oss << in << "<nonnumeric_predicate contextSize=\"" << contextSize_ << "\">" << std::endl;
  if(!parent_.isNull()) {
    oss << in << "  <parent>" << std::endl;
    oss << parent_.asString(context, indent + 2);
    oss << in << "  </parent>" << std::endl;
  }
  oss << in << "  <dataitem>" << std::endl;
  oss << PrintDataItemTree::print(pred_.pred, context, indent + 2);
  oss << in << "  </dataitem>" << std::endl;
  oss << in << "</nonnumeric_predicate>" << std::endl;

  return oss.str();
}

/////////////////////////////////////
// NumericPredicateFilterResult
/////////////////////////////////////

DataItemImpl::NumericPredicateFilterResult::NumericPredicateFilterResult(const Result &parent, const PredInfo &pred, unsigned int contextSize, DynamicContext *context)
  : ResultImpl(context),
    todo_(true),
    parent_(parent),
    pred_(pred),
    contextSize_(contextSize)
{
}

Item::Ptr DataItemImpl::NumericPredicateFilterResult::next(DynamicContext *context)
{
  if(todo_) {
    todo_ = false;

    Numeric::Ptr one = context->getPathanFactory()->createInteger(1, context);

    AutoContextInfoReset autoReset(context);
    context->setContextSize(contextSize_);

    // Set the context item to something other than null,
    // since fn:last() checks to see that there is actually
    // a context item
    context->setContextItem(one);

    Result pred_result = pred_.pred->collapseTree(context);
    Numeric::Ptr first = (Numeric::Ptr)pred_result.next(context);
    if(first == NULLRCP) {
      // The effective boolean value is therefore false
      parent_ = 0;
      return 0;
    }

    Item::Ptr second = pred_result.next(context);
    if(second != NULLRCP) {
      // The effective boolean value causes an error -
      // so call it to get the correct error
      parent_ = 0;
      ResultImpl::getEffectiveBooleanValue(first, second, context);
      return 0;
    }

    autoReset.resetContextInfo();

    Numeric::Ptr pos = one;
    while(pos->lessThan(first, context) && parent_.next(context) != NULLRCP) {
      pos = pos->add(one, context);
    }

    if(pos->equals(first, context)) {
      Item::Ptr result = parent_.next(context);
      parent_ = 0;
      return result;
    }

    parent_ = 0;
  }

  return 0;
}

void DataItemImpl::NumericPredicateFilterResult::skip()
{
  parent_.skip();
}

std::string DataItemImpl::NumericPredicateFilterResult::asString(DynamicContext *context, int indent) const
{
  std::ostringstream oss;
  std::string in(getIndent(indent));

  oss << in << "<numeric_predicate>" << std::endl;
  if(!parent_.isNull()) {
    oss << in << "  <parent>" << std::endl;
    oss << parent_.asString(context, indent + 2);
    oss << in << "  </parent>" << std::endl;
  }
  oss << in << "  <dataitem>" << std::endl;
  oss << PrintDataItemTree::print(pred_.pred, context, indent + 2);
  oss << in << "  </dataitem>" << std::endl;
  oss << in << "</numeric_predicate>" << std::endl;

  return oss.str();
}

