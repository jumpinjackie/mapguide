/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _DATAITEMIF_HPP
#define _DATAITEMIF_HPP

#include <pathan/Pathan.hpp>

#include <pathan/dataItem/DataItemImpl.hpp>

/** If function. */
class PATHAN_EXPORT DataItemIf : public DataItemImpl
{
public:
  DataItemIf(DataItem* test, DataItem* whenTrue, DataItem* whenFalse, XPath2MemoryManager* memMgr);
  
  virtual Result createResult(DynamicContext* context, int flags=0) const;
  virtual DataItem* staticResolution(StaticContext *context);

  const DataItem *getTest() const;
  const DataItem *getWhenTrue() const;
  const DataItem *getWhenFalse() const;

  void setTest(DataItem *item);
  void setWhenTrue(DataItem *item);
  void setWhenFalse(DataItem *item);

protected:
  class IfResult : public ResultImpl
  {
  public:
    IfResult(const DataItemIf *di, int flags, DynamicContext *context);

    Item::Ptr next(DynamicContext *context);
    std::string asString(DynamicContext *context, int indent) const;

  private:
    int _flags;
    const DataItemIf *_di;
    Result _results;
  };

  DataItem* _test,* _whenTrue,* _whenFalse;
};

#endif // _DATAITEMIF_HPP


