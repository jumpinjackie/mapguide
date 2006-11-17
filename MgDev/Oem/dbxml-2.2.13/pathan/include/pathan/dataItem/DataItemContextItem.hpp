/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _DATAITEMCONTEXTITEM_HPP
#define _DATAITEMCONTEXTITEM_HPP

#include <pathan/Pathan.hpp>

#include <pathan/dataItem/DataItemImpl.hpp>
#include <pathan/dataItem/SingleResult.hpp>

/** Returns the context item. Raises [err:XP0002] if the context item is null. */
class PATHAN_EXPORT DataItemContextItem : public DataItemImpl
{
public:
  DataItemContextItem(XPath2MemoryManager* memMgr);
  ~DataItemContextItem();

  /**
   * Called during static analysis of expression to determine is statically
   * correct.  If not an appropriate error is thrown.
   */
  virtual DataItem* staticResolution(StaticContext *context);

	virtual Result createResult(DynamicContext* context, int flags=0) const;

  class PATHAN_EXPORT ContextItemResult : public SingleResult
  {
  public:
		ContextItemResult(DynamicContext *context);
		Item::Ptr getSingleResult(DynamicContext *context) const;
		std::string asString(DynamicContext *context, int indent) const;    
  };
};

#endif

