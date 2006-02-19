/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _DATAITEMS_HPP
#define _DATAITEMS_HPP


#include <pathan/Pathan.hpp>
#include <vector>
#include <pathan/XPath2MemoryManager.hpp>

class DynamicContext;
class StaticContext;
class Result;
class DataItem;
class StaticResolutionContext;
class SequenceType;

typedef std::vector<DataItem*,PathanAllocator<DataItem*> > VectorOfDataItems;

class PATHAN_EXPORT DataItem
{
public:
  ///enum for data types
  typedef enum {
    LITERAL,
    SEQUENCE,
    FUNCTION,
    NAVIGATION,
    VARIABLE,
    STEP,
    FOR,
    QUANTIFIED,
    IF,
    INSTANCE_OF,
    CASTABLE_AS,
    CAST_AS,
    TREAT_AS,
    OPERATOR,
    CONTEXT_ITEM,
    PARENTHESIZED,
    UNUSED_VALUE = 140 // force compiler to use more than 4 bits for this enum
  } whichType;

  class PredInfo {
  public:
    PredInfo() : pred(0) {}
    PredInfo(DataItem *s) : pred(s) {}
    DataItem *pred;
  };
  typedef std::vector<PredInfo,PathanAllocator<PredInfo> > Predicates;

  enum executionFlags {
    UNORDERED =0x01,
    RETURN_ONE=0x02,
    RETURN_TWO=0x04
  };

  /** do-nothing virtual dtor */	
  virtual ~DataItem() {}
	
  virtual Result collapseTree(DynamicContext* context, int flags=0) const = 0;

  /** Returns true if this DataItem has no predicates, and is an instance of
      DataItemSequence or DataItemLiteral */
  virtual bool isConstant() const = 0;
  /** Returns true if this DataItem has no predicates, and is an instance of
      DataItemSequence or DataItemLiteral. If the literal value of this DataItem
      is a single DateOrTimeType, then hasTimezone() on it must return true,
      otherwise this method will return false. */
  virtual bool isConstantAndHasTimezone(StaticContext* context) const = 0;
  virtual bool isSingleNumericConstant(StaticContext *context) const = 0;

  virtual void addPredicates(const VectorOfDataItems &steps) = 0;
  virtual void addPredicates(const Predicates &steps) = 0;

  /// Returns the type of the DataItemImpl pointed to.
  virtual whichType getType(void) const = 0;

  virtual DataItem* staticResolution(StaticContext *context) = 0;
  /// Returns the StaticResolutionContext for this DataItem
  virtual const StaticResolutionContext &getStaticResolutionContext() const = 0;
};

#endif

