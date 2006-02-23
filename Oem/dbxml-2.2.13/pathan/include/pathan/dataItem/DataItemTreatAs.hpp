/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _DATAITEMTREATAS_HPP
#define _DATAITEMTREATAS_HPP

#include "../Pathan.hpp"

#include "DataItemImpl.hpp"
#include "LazySequenceResult.hpp"

class SequenceType;

class PATHAN_EXPORT DataItemTreatAs : public DataItemImpl
{
public:
	DataItemTreatAs(DataItem* expr, SequenceType* exprType, XPath2MemoryManager* memMgr);

	virtual Result createResult(DynamicContext* context, int flags=0) const;
	virtual DataItem* staticResolution(StaticContext *context);

  const DataItem *getExpression() const;
  const SequenceType *getSequenceType() const;

  void setExpression(DataItem *item);

protected:
	DataItem* _expr;
	SequenceType* _exprType;
};

#endif // DATAITEMTREATAS_HPP
