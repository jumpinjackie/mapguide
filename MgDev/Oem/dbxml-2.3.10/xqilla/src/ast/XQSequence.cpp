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
 * $Id: XQSequence.cpp,v 1.17 2006/11/17 18:08:07 jpcs Exp $
 */

#include <sstream>

#include "../config/xqilla_config.h"
#include <assert.h>
#include <xqilla/ast/XQSequence.hpp>
#include <xqilla/ast/ASTNode.hpp>

#if defined(WIN32) && !defined(__CYGWIN__)
#define snprintf _snprintf
#endif

#include <xqilla/ast/StaticResolutionContext.hpp>
#include <xqilla/runtime/Sequence.hpp>
#include <xqilla/runtime/SequenceResult.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/items/Item.hpp>
#include <xqilla/items/AnyAtomicTypeConstructor.hpp>
#include <xqilla/items/NumericTypeConstructor.hpp>
#include <xqilla/items/ATQNameConstructor.hpp>
#include <xqilla/items/ATQNameOrDerived.hpp>
#include <xqilla/items/DateOrTimeType.hpp>
#include <xqilla/utils/XPath2Utils.hpp>
#include <xqilla/exceptions/IllegalArgumentException.hpp>

inline ItemConstructor *itemToItemConstructor(const Item::Ptr &item, DynamicContext *context, XPath2MemoryManager *memMgr)
{
  if(item->isNode()) {
    XQThrow2(IllegalArgumentException, X("itemToItemConstructor"), X("Cannot create an ItemConstructor for a Node"));
  }
  else {
    const AnyAtomicType *atom = (const AnyAtomicType*)item.get();
    switch(atom->getPrimitiveTypeIndex())
    {
    case AnyAtomicType::QNAME: 
        {
          const ATQNameOrDerived *qname = (const ATQNameOrDerived*)atom;
          return new (memMgr) ATQNameConstructor(atom->getTypeURI(),
                                                 atom->getTypeName(),
                                                 qname->getURI(),
                                                 qname->getPrefix(),
                                                 qname->getName());
        }
    case AnyAtomicType::DECIMAL:
    case AnyAtomicType::DOUBLE:
    case AnyAtomicType::FLOAT:
        {
          const Numeric *number = (const Numeric*)atom;
          if((number->getState()==Numeric::NUM || number->getState()==Numeric::NEG_NUM) && !number->isZero())
            return new (memMgr) NumericTypeConstructor(number->getTypeURI(),
                                                       number->getTypeName(),
                                                       number->asMAPM(),
                                                       number->getPrimitiveTypeIndex(),
                                                       memMgr);
          else
            return new (memMgr) AnyAtomicTypeConstructor(number->getTypeURI(),
                                                         number->getTypeName(),
                                                         number->asString(context),
                                                         number->getPrimitiveTypeIndex());
        }
    default:
          return new (memMgr) AnyAtomicTypeConstructor(atom->getTypeURI(),
                                                       atom->getTypeName(),
                                                       atom->asString(context),
                                                       atom->getPrimitiveTypeIndex());
    }
  }
}

static const unsigned int CONSTANT_FOLD_LIMIT = 30;

XQSequence *XQSequence::constantFold(Result &result, DynamicContext *context, XPath2MemoryManager* memMgr,
	const LocationInfo *location)
{
  XQSequence *seq = new (memMgr) XQSequence(memMgr);
  seq->setLocationInfo(location);

  Item::Ptr item;
  while((item = result->next(context)).notNull()) {
    if(seq->_itemConstructors.size() > CONSTANT_FOLD_LIMIT) return 0;
    seq->_itemConstructors.push_back(itemToItemConstructor(item, context, memMgr));
  }

  return seq;
}

XQSequence::XQSequence(const Item::Ptr &item, DynamicContext *context, XPath2MemoryManager* memMgr)
  : ASTNodeImpl(memMgr), _itemConstructors(XQillaAllocator<ItemConstructor*>(memMgr))
{
  setType(ASTNode::SEQUENCE);
  _itemConstructors.push_back(itemToItemConstructor(item, context, memMgr));
}

XQSequence::XQSequence(ItemConstructor *ic, XPath2MemoryManager* memMgr)
  : ASTNodeImpl(memMgr), _itemConstructors(XQillaAllocator<ItemConstructor*>(memMgr))
{
  setType(ASTNode::SEQUENCE);
  _itemConstructors.push_back(ic);
}

XQSequence::XQSequence(XPath2MemoryManager* memMgr)
  : ASTNodeImpl(memMgr), _itemConstructors(XQillaAllocator<ItemConstructor*>(memMgr))
{
  setType(ASTNode::SEQUENCE);
}

XQSequence::~XQSequence()
{
  //no-op
}

ASTNode* XQSequence::staticResolution(StaticContext *context)
{
  return this;
}

ASTNode *XQSequence::staticTyping(StaticContext *context)
{
  _src.clear();

  _src.getStaticType().flags = 0;

  ItemConstructor::Vector::iterator it = _itemConstructors.begin();
  for(; it != _itemConstructors.end(); ++it) {
    _src.getStaticType().typeUnion((*it)->getStaticType());
  }

  return this;
}

Result XQSequence::createResult(DynamicContext* context, int flags) const
{
  return new SequenceResult(this);
}

/** Returns true if this XQ has no predicates, and is an instance of
    XQSequence or XQLiteral. If the literal value of this XQ
    is a single DateOrTimeType, then !hasTimezone() on it must return true,
    otherwise this method will return false. */
bool XQSequence::isDateOrTimeAndHasNoTimezone(StaticContext *context) const
{
  AutoDelete<DynamicContext> dContext(context->createDynamicContext());
  dContext->setMemoryManager(context->getMemoryManager());

  ItemConstructor::Vector::const_iterator it = _itemConstructors.begin();
  for(; it != _itemConstructors.end(); ++it) {
    Item::Ptr item = (*it)->createItem(dContext);
    if(item->isAtomicValue() &&
       ((const AnyAtomicType::Ptr)item)->isDateOrTimeTypeValue() &&
       !((const DateOrTimeType::Ptr)item)->hasTimezone()) {
      return true;
    }
  }
  return false;
}

bool XQSequence::isSingleNumericConstant(StaticContext *context) const
{
  if(_itemConstructors.size() == 1) {
    AutoDelete<DynamicContext> dContext(context->createDynamicContext());
    dContext->setMemoryManager(context->getMemoryManager());

    Item::Ptr item = _itemConstructors[0]->createItem(dContext);
    if(item->isAtomicValue() &&
       ((const AnyAtomicType::Ptr)item)->isNumericValue()) {
      return true;
    }
  }
  return false;
}

XQSequence::SequenceResult::SequenceResult(const XQSequence *seq)
  : ResultImpl(seq),
    _seq(seq),
    _it(seq->getItemConstructors().begin())
{
}

Item::Ptr XQSequence::SequenceResult::next(DynamicContext *context)
{
  Item::Ptr item;
  if(_it != _seq->getItemConstructors().end()) {
    item = (*_it)->createItem(context);
    ++_it;
  }
  return item;
}

std::string XQSequence::SequenceResult::asString(DynamicContext *context, int indent) const
{
  std::ostringstream oss;
  std::string in(getIndent(indent));

  oss << in << "<sequence/>" << std::endl;

  return oss.str();
}
