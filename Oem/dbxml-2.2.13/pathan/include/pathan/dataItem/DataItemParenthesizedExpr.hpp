/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _DATAITEMPARENTHESIZEDEXPR_HPP
#define _DATAITEMPARENTHESIZEDEXPR_HPP

#include <pathan/Pathan.hpp>

#include <pathan/dataItem/DataItemImpl.hpp>

class PATHAN_EXPORT DataItemParenthesizedExpr : public DataItemImpl
{

public:
  DataItemParenthesizedExpr(XPath2MemoryManager* memMgr);	

  ///adds a dataitem to this DataItemParenthesizedExpr
  void addItem(DataItem* di);

  virtual Result createResult(DynamicContext* context, int flags=0) const;
  virtual DataItem* staticResolution(StaticContext *context);

  const VectorOfDataItems &getChildren() const;

private:
  class ParenthesizedResult : public ResultImpl
  {
  public:
    ParenthesizedResult(const DataItemParenthesizedExpr *di, int flags, DynamicContext *context);

    Item::Ptr next(DynamicContext *context);
    std::string asString(DynamicContext *context, int indent) const;

  private:
    int _flags;
    const DataItemParenthesizedExpr *_di;
    VectorOfDataItems::const_iterator _i;
    Result _result;
  };

	VectorOfDataItems _dataItems;
};

#endif
