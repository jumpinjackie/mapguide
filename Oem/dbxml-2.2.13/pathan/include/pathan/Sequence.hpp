/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _SEQUENCE_HPP
#define _SEQUENCE_HPP

#include <vector>
#include "Pathan.hpp"
#include "ATDecimalOrDerived.hpp"

// needed for the definition of PathanAllocator
#include "XPath2MemoryManager.hpp"

class Item;
class SequenceType;
class DynamicContext;

typedef std::vector<Item::Ptr, PathanAllocator<Item::Ptr> > VectorOfItems;

/** defines the behaviour for the string data type*/
class PATHAN_EXPORT Sequence 
{

public:
  typedef VectorOfItems::iterator iterator;
  typedef VectorOfItems::const_iterator const_iterator;
  typedef VectorOfItems::reverse_iterator reverse_iterator;
  typedef VectorOfItems::const_reverse_iterator const_reverse_iterator;

  // constructor that takes one Item
  Sequence(const Item::Ptr &item, XPath2MemoryManager* memMgr);
  // constructor that creates a empty sequence
  Sequence(XPath2MemoryManager* memMgr);
  /// construct and reserve space for n elements
  Sequence(unsigned int n, XPath2MemoryManager* memMgr);
  // copy constructor
  Sequence(const Sequence&, XPath2MemoryManager* memMgr);
  // copy constructor
  Sequence(const Sequence&);
  Sequence & operator=(const Sequence &);

  // no-op
  ~Sequence();

  void clear();

  const Item::Ptr &first() const;
  const Item::Ptr &second() const;

  ///Begin an iterator
  Sequence::iterator begin(void);
  Sequence::const_iterator begin(void) const;

  ///Find the end of the iterator
  Sequence::iterator end(void);
  Sequence::const_iterator end(void) const;

  ///Reverse iterator methods
  Sequence::reverse_iterator rbegin(void);
  Sequence::const_reverse_iterator rbegin(void) const;

  Sequence::reverse_iterator rend(void);
  Sequence::const_reverse_iterator rend(void) const;

  ///Returns the number of nodes in the list                        
  unsigned int getLength(void) const;

  ///Return the indexth item in the collection 
  const Item::Ptr &item(unsigned int index) const;
  ///Return the indexth item in the collection - takes ATDecimalOrDerived
  const Item::Ptr &item(const ATDecimalOrDerived::Ptr &index) const;

  /// add an item to this sequence
  void addItem(const Item::Ptr &item);

  /// add an item to this sequence
  void addItemFront(const Item::Ptr &item);

  /// combine another sequence onto this sequence
  void joinSequence(const Sequence & s);

  ///Returns true if the list is empty
  bool isEmpty() const;

  /// sort into document order (only works for Sequences containing only Nodes)
  void sortIntoDocumentOrder(const DynamicContext *context);

  /// sort as strings, using the given collation
  void sortWithCollation(const Collation *collation, const DynamicContext *context);

  XPath2MemoryManager *getMemoryManager() const { return _memMgr; }

private:
  VectorOfItems _itemList;
  XPath2MemoryManager *_memMgr;
};

#endif
