/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <assert.h>
#include <pathan/Sequence.hpp>
#include <pathan/DynamicContext.hpp>
#include "../exceptions/MiscException.hpp"
#include "../utils/NumUtils.hpp"
#include <pathan/ATDecimalOrDerived.hpp>
#include <pathan/Node.hpp>
#include <pathan/Collation.hpp>

Sequence::Sequence(XPath2MemoryManager* memMgr)
  : _itemList(PathanAllocator<Item::Ptr>(memMgr)),
    _memMgr(memMgr)
{
}

Sequence::~Sequence()
{
	//no-op
}

Sequence::Sequence(const Item::Ptr &item, XPath2MemoryManager* memMgr)
  : _itemList(1, item, PathanAllocator<Item::Ptr>(memMgr)),
    _memMgr(memMgr)
{
}

Sequence::Sequence(const Sequence & s)
  : _itemList(s._itemList),
    _memMgr(s._memMgr)
{
}

Sequence::Sequence(const Sequence &s, XPath2MemoryManager* memMgr)
  : _itemList(PathanAllocator<Item::Ptr>(memMgr)),
    _memMgr(memMgr)
{
  joinSequence(s);
}

Sequence::Sequence(unsigned int n, XPath2MemoryManager* memMgr)
  : _itemList(PathanAllocator<Item::Ptr>(memMgr)),
    _memMgr(memMgr)
{
    _itemList.reserve(n);
}

Sequence & Sequence::operator=(const Sequence & s) {
	if( this != &s ) { // no self-assignment
	  _itemList = s._itemList;
	}
	return *this;
}

void Sequence::addItemFront(const Item::Ptr &item)
{
  //This is a hack since the redhat 6.2 deque warns with push_front
  _itemList.insert(_itemList.begin(), item);
}

// returns the first item in the Sequence, or null if it is empty
const Item::Ptr &Sequence::first() const
{
  assert(!isEmpty());  // should not be asking for elements if the list is empty
  return _itemList[0];
}

// returns the second item in the Sequence, or null if it is empty
const Item::Ptr &Sequence::second() const
{
  assert(!isEmpty());  // should not be asking for elements if the list is empty
  return _itemList[1];
}

// might return NULL
const Item::Ptr &Sequence::item(unsigned int index) const
{
  return _itemList[index];
}

const Item::Ptr &Sequence::item(const ATDecimalOrDerived::Ptr &index) const
{
  if(index->isNegative()) {
    DSLthrow(MiscException,X("Sequence:item"),X("Index less than zero"));
  }
  return item(NumUtils::MAPMtoInt(((const ATDecimalOrDerived*)index)->asMAPM()));
}

void Sequence::clear(void) {
  _itemList.clear();
}

Sequence::iterator Sequence::begin(void)
{
  return _itemList.begin();
}

Sequence::iterator Sequence::end(void)
{
  return _itemList.end();
}

Sequence::const_iterator Sequence::begin(void) const
{
  return _itemList.begin();
}

Sequence::const_iterator Sequence::end(void) const
{
  return _itemList.end();
}

Sequence::reverse_iterator Sequence::rbegin(void)
{
  return _itemList.rbegin();
}

Sequence::reverse_iterator Sequence::rend(void)
{
  return _itemList.rend();
}

Sequence::const_reverse_iterator Sequence::rbegin(void) const
{
  return _itemList.rbegin();
}

Sequence::const_reverse_iterator Sequence::rend(void) const
{
  return _itemList.rend();
}

unsigned int Sequence::getLength(void) const
{
  return _itemList.size();
}

void Sequence::addItem(const Item::Ptr &item) {
	_itemList.push_back(item);
}

void Sequence::joinSequence(const Sequence & s) {
	_itemList.insert(_itemList.end(),s._itemList.begin(),s._itemList.end());
}

bool Sequence::isEmpty() const {
	return _itemList.empty();
}

class lessThanCompareFn {
public:
  lessThanCompareFn(const DynamicContext *context)
    : context_(context) {}

  bool operator()(const Item::Ptr &first, const Item::Ptr &second)
  {
    return ((const Node*)first.get())->lessThan((const Node::Ptr)second, context_);
  }

private:
  const DynamicContext *context_;
};

bool equalCompareFn(const Item::Ptr &first, const Item::Ptr &second)
{
	return ((Node*)(const Item*)first)->equals((const Node::Ptr )second);
}

void Sequence::sortIntoDocumentOrder(const DynamicContext *context)
{
    // REVISIT: we should ensure that the sequence is made only of nodes, but for performance reason
    // we just check the first item.
    // Note that it is OK that we are asked to sort a sequence made of atomic values, as a path expression
    // can now return it, so we should not complain
    if(isEmpty() || !first()->isNode())
        return;
  // Sort
  std::sort(_itemList.begin(), _itemList.end(), lessThanCompareFn(context));

  // Remove the duplicates
  _itemList.erase(std::unique(_itemList.begin(), _itemList.end(), equalCompareFn), _itemList.end());
}

struct CollationCompare {
  CollationCompare(const Collation *collation, const DynamicContext *context)
    : _collation(collation), _context(context) {}

  bool operator()(const Item::Ptr arg1, const Item::Ptr arg2) const {
    const XMLCh* s1 = arg1->asString(_context);
    const XMLCh* s2 = arg2->asString(_context);
    return (_collation->compare(s1,s2) == -1);
  }

  const Collation *_collation;
  const DynamicContext *_context;
};

void Sequence::sortWithCollation(const Collation *collation, const DynamicContext *context)
{
  std::sort(_itemList.begin(), _itemList.end(), CollationCompare(collation, context));
}
