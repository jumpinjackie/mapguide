/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _DATAITEMQUANTIFIEDEXPR_HPP
#define _DATAITEMQUANTIFIEDEXPR_HPP

#include "../Pathan.hpp"
#include "DataItemFor.hpp"
#include "SingleResult.hpp"

/** Some/Every function. */
class PATHAN_EXPORT DataItemQuantifiedExpr : public DataItemFor
{
public:
  typedef enum {some, every} QuantifierType;

  DataItemQuantifiedExpr(QuantifierType qType, VectorOfBindings* bindings, DataItem* returnExpr, XPath2MemoryManager* memMgr);
  
  virtual DataItem* staticResolution(StaticContext *context);

  QuantifierType getQuantifierType() const;

protected:
  virtual Result createResultImpl(Bindings::const_iterator it, Bindings::const_iterator end, DynamicContext* context, int flags = 0) const;

  class QuantifiedResult : public SingleResult
  {
  public:
    QuantifiedResult(QuantifierType type, Bindings::const_iterator it, Bindings::const_iterator end, const DataItem *returnExpr, DynamicContext *context);

    Item::Ptr getSingleResult(DynamicContext *context) const;
    std::string asString(DynamicContext *context, int indent) const;

  private:
    QuantifierType _type;
    ExecutionBindings _ebs;
    const DataItem *_returnExpr;
  };

  QuantifierType _qType;
};

#endif // _DATAITEMQUANTIFIEDEXPR_HPP

