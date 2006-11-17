/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _DATAITEMCASTABLEAS_HPP
#define _DATAITEMCASTABLEAS_HPP

#include "../Pathan.hpp"

#include "DataItemImpl.hpp"
#include "SingleResult.hpp"

class SequenceType;

class PATHAN_EXPORT DataItemCastableAs : public DataItemImpl
{
public:
	DataItemCastableAs(DataItem* expr, SequenceType* exprType, XPath2MemoryManager* memMgr);

  virtual Result createResult(DynamicContext* context, int flags=0) const;
	virtual DataItem* staticResolution(StaticContext *context);

  const DataItem *getExpression() const;
  const SequenceType *getSequenceType() const;

  void setExpression(DataItem *item);

protected:
  class CastableAsResult : public SingleResult
  {
  public:
    CastableAsResult(const DataItemCastableAs *di, DynamicContext *context);

    Item::Ptr getSingleResult(DynamicContext *context) const;
    std::string asString(DynamicContext *context, int indent) const;

  private:
    const DataItemCastableAs *_di;
  };

	DataItem* _expr;
	SequenceType* _exprType;
};

#endif // _DATAITEMCASTABLEAS_HPP
