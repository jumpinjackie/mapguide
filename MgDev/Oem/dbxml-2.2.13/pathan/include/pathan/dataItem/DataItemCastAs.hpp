/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _DATAITEMCASTAS_HPP
#define _DATAITEMCASTAS_HPP

#include "../Pathan.hpp"

#include "DataItemImpl.hpp"
#include "SingleResult.hpp"

class SequenceType;

class PATHAN_EXPORT DataItemCastAs : public DataItemImpl
{
public:
	DataItemCastAs(DataItem* expr, SequenceType* exprType, XPath2MemoryManager* memMgr);

  virtual Result createResult(DynamicContext* context, int flags=0) const;
	virtual DataItem* staticResolution(StaticContext *context);

  const DataItem *getExpression() const;
  const SequenceType *getSequenceType() const;

  void setExpression(DataItem *item);

protected:
  class CastAsResult : public SingleResult
  {
  public:
    CastAsResult(const DataItemCastAs *di, DynamicContext *context);

    Item::Ptr getSingleResult(DynamicContext *context) const;
    std::string asString(DynamicContext *context, int indent) const;

  private:
    const DataItemCastAs *_di;
  };

	DataItem* _expr;
	SequenceType* _exprType;
};

#endif // _DATAITEMCASTAS_HPP
