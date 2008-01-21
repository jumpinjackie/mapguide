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
 * $Id: AggregateFunction.hpp,v 1.8 2006/11/01 16:37:11 jpcs Exp $
 */

#ifndef _AGGREGATEFUNCTION_HPP
#define _AGGREGATEFUNCTION_HPP

#include <xqilla/ast/ConstantFoldingFunction.hpp>

/** Virtual interface class for functions */
class XQILLA_API AggregateFunction : public ConstantFoldingFunction
{
public:

  /// constructor, checks for the correct number of arguments.
  AggregateFunction(const XMLCh* name,unsigned int argsFrom, unsigned int argsTo, const char* paramDecl, const VectorOfASTNodes &args, XPath2MemoryManager* memMgr);

protected:
  /* 
   * returns a sequence with untypedAtomic values casted to the type of the other values in the sequence
   * or if all values are untypedAtomic they are cast to the convertType. 
   * If the items aren't all the same type (or untypedAtomic) an exception is thrown.   
   */
  virtual Sequence validateSequence(Sequence sequence, DynamicContext* context) const;

  // is the type specified by the given URI/Name pair a numeric one?
  bool isNumericType(const XMLCh* typeURI, const XMLCh* typeName, const DynamicContext* context) const;  

  // is the item a NaN?
  bool isNumericNaN(const Item::Ptr &item) const;

};
#endif // _AGGREGATEFUNCTION_HPP
