/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _DATAITEMLITERAL_HPP
#define _DATAITEMLITERAL_HPP

#include <pathan/Pathan.hpp>

#include <pathan/dataItem/DataItemImpl.hpp>

class ItemConstructor;

/** defines the behaviour for the literal  types*/
class PATHAN_EXPORT DataItemLiteral : public DataItemImpl
{
public:
  DataItemLiteral(ItemConstructor *ic, XPath2MemoryManager* memMgr);

  virtual bool isConstantAndHasTimezone(StaticContext* context) const;
  virtual bool isSingleNumericConstant(StaticContext *context) const;

  virtual DataItem* staticResolution(StaticContext *context);
  virtual Result createResult(DynamicContext* context, int flags=0) const;

  const ItemConstructor *getItemConstructor() const
  {
    return _itemConstructor;
  }

  void setItemConstructor(ItemConstructor *i)
  {
    _itemConstructor = i;
  }

private:
  ItemConstructor *_itemConstructor;
};

#endif

