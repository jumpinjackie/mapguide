/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include <sstream>

#include "../config/pathan_config.h"
#include <assert.h>
#include <pathan/dataItem/DataItemSequence.hpp>
#include <pathan/dataItem/DataItem.hpp>

#if defined(WIN32) && !defined(__CYGWIN__)
#define snprintf _snprintf
#endif

#include <pathan/Sequence.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/Item.hpp>
#include <pathan/dataItem/StaticResolutionContext.hpp>
#include <pathan/dataItem/SequenceResult.hpp>
#include <pathan/AnyAtomicTypeConstructor.hpp>
#include <pathan/ATQNameConstructor.hpp>
#include <pathan/XPath2Utils.hpp>
#include <pathan/ATQNameOrDerived.hpp>
#include <pathan/DateOrTimeType.hpp>
#include <pathan/exceptions/IllegalArgumentException.hpp>

#include <xercesc/validators/schema/SchemaSymbols.hpp>

inline ItemConstructor *itemToItemConstructor(const Item::Ptr &item, DynamicContext *context, XPath2MemoryManager *memMgr)
{
  if(item->isNode()) {
    DSLthrow(IllegalArgumentException, X("itemToItemConstructor"), X("Cannot create an ItemConstructor for a Node"));
  }
  else {
    const AnyAtomicType *atom = (const AnyAtomicType*)(const Item*)item;

    if(XPath2Utils::equals(atom->getPrimitiveTypeName(),
                           XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgDT_QNAME) &&
       XPath2Utils::equals(atom->getPrimitiveTypeURI(),
                           XERCES_CPP_NAMESPACE_QUALIFIER SchemaSymbols::fgURI_SCHEMAFORSCHEMA)) {
      const ATQNameOrDerived *qname = (const ATQNameOrDerived*)atom;
      return new (memMgr) ATQNameConstructor(atom->getTypeURI(),
                                             atom->getTypeName(),
                                             qname->getURI(),
                                             qname->getPrefix(),
                                             qname->getName());
    }
    else {
      bool numeric = false;
      switch(atom->getPrimitiveTypeIndex()) {
      case AnyAtomicType::DECIMAL:
      case AnyAtomicType::DOUBLE:
      case AnyAtomicType::FLOAT: {
        numeric = true;
        break;
      }
      default: break;
      }      

      return new (memMgr) AnyAtomicTypeConstructor(atom->getTypeURI(),
                                                   atom->getTypeName(),
                                                   atom->asString(context),
                                                   numeric);
    }
  }
}

inline void resultToItemConstructors(Result &result, DynamicContext *context, ItemConstructor::Vector &ics, XPath2MemoryManager *memMgr)
{
  Item::Ptr item;
  while((item = result.next(context)) != NULLRCP) {
    ics.push_back(itemToItemConstructor(item, context, memMgr));
  }
}

DataItemSequence::DataItemSequence(Result &result, DynamicContext *context, XPath2MemoryManager* memMgr)
  : DataItemImpl(memMgr), _itemConstructors(PathanAllocator<ItemConstructor*>(memMgr))
{
  setType(DataItem::SEQUENCE);
  resultToItemConstructors(result, context, _itemConstructors, memMgr);
}

DataItemSequence::DataItemSequence(const Item::Ptr &item, DynamicContext *context, XPath2MemoryManager* memMgr)
  : DataItemImpl(memMgr), _itemConstructors(PathanAllocator<ItemConstructor*>(memMgr))
{
  setType(DataItem::SEQUENCE);
  _itemConstructors.push_back(itemToItemConstructor(item, context, memMgr));
}

DataItemSequence::DataItemSequence(ItemConstructor *ic, XPath2MemoryManager* memMgr)
  : DataItemImpl(memMgr), _itemConstructors(PathanAllocator<ItemConstructor*>(memMgr))
{
  setType(DataItem::SEQUENCE);
  _itemConstructors.push_back(ic);
}

DataItemSequence::DataItemSequence(XPath2MemoryManager* memMgr)
  : DataItemImpl(memMgr), _itemConstructors(PathanAllocator<ItemConstructor*>(memMgr))
{
  setType(DataItem::SEQUENCE);
}

DataItemSequence::~DataItemSequence()
{
  //no-op
}

DataItemImpl *DataItemSequence::resolvePredicate(Predicates::reverse_iterator it, Predicates &newPreds, DynamicContext *context)
{
  if(it != _predList.rend()) {
    resolvePredicate(it + 1, newPreds, context);

    if(_itemConstructors.empty()) {
      newPreds.clear();
    }
    else {
      DataItem *pred = it->pred->staticResolution(context);
      const StaticResolutionContext &newSrc = pred->getStaticResolutionContext();
      PredInfo p_info(pred);

      if(newSrc.isUsedExceptContextFlags() || !newPreds.empty()) {
        // We can't constant fold, so just return
        newPreds.push_back(p_info);
        _src.addExceptContextFlags(newSrc);
      }
      else {
        // Constant fold
        Result parent(createResult(context));
        Result result = createPredicateResult(parent, p_info, context).toSequence(context);
        _itemConstructors.clear();
        resultToItemConstructors(result, context, _itemConstructors, getMemoryManager());
      }
    }
  }

  return this;
}

DataItem* DataItemSequence::staticResolution(StaticContext *context)
{
  _src.getStaticType().flags = 0;

  ItemConstructor::Vector::iterator it = _itemConstructors.begin();
  for(; it != _itemConstructors.end(); ++it) {
    if((*it)->isNumeric()) {
      _src.getStaticType().flags |= StaticType::NUMERIC_TYPE;
    } else {
      _src.getStaticType().flags |= StaticType::OTHER_TYPE;
    }
  }

  return resolvePredicates(context);
}

Result DataItemSequence::createResult(DynamicContext* context, int flags) const
{
  return new SequenceResult(this, context);
}

/** Returns true if this DataItem has no predicates, and is an instance of
    DataItemSequence or DataItemLiteral. If the literal value of this DataItem
    is a single DateOrTimeType, then hasTimezone() on it must return true,
    otherwise this method will return false. */
bool DataItemSequence::isConstantAndHasTimezone(StaticContext *context) const
{
  if(isConstant() && _itemConstructors.size() == 1) {
    AutoRelease<DynamicContext> dContext(context->createDynamicContext());
    dContext->setMemoryManager(context->getMemoryManager());

    Item::Ptr item = _itemConstructors[0]->createItem(dContext);
    if(item->isAtomicValue() &&
       ((const AnyAtomicType::Ptr)item)->isDateOrTimeTypeValue() &&
       ((const DateOrTimeType::Ptr)item)->hasTimezone()) {
      return true;
    }
  }
  return false;
}

bool DataItemSequence::isSingleNumericConstant(StaticContext *context) const
{
  if(isConstant() && _itemConstructors.size() == 1) {
    AutoRelease<DynamicContext> dContext(context->createDynamicContext());
    dContext->setMemoryManager(context->getMemoryManager());

    Item::Ptr item = _itemConstructors[0]->createItem(dContext);
    if(item->isAtomicValue() &&
       ((const AnyAtomicType::Ptr)item)->isNumericValue()) {
      return true;
    }
  }
  return false;
}

DataItemSequence::SequenceResult::SequenceResult(const DataItemSequence *seq, DynamicContext *context)
  : ResultImpl(context),
    _seq(seq),
    _it(seq->getItemConstructors().begin())
{
}

Item::Ptr DataItemSequence::SequenceResult::next(DynamicContext *context)
{
  Item::Ptr item;
  if(_it != _seq->getItemConstructors().end()) {
    item = (*_it)->createItem(context);
    ++_it;
  }
  return item;
}

std::string DataItemSequence::SequenceResult::asString(DynamicContext *context, int indent) const
{
  std::ostringstream oss;
  std::string in(getIndent(indent));

  oss << in << "<sequence/>" << std::endl;

  return oss.str();
}
