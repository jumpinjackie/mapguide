//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2004-2005
//	Sleepycat Software.  All rights reserved.
// Copyright (c) 2004-2005
//	Progress Software Corporation.  All rights reserved.
//
// $Id: XQOrderingChange.hpp,v 1.5 2005/07/14 15:21:09 jsnelson Exp $
//

//////////////////////////////////////////////////////////////////////
// XQOrderingChange.hpp: interface for the XQOrderingChange class.
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XQORDERINGCHANGE_H__A97FA5AE_5355_45CD_9092_ED94F5CD035B__INCLUDED_)
#define AFX_XQORDERINGCHANGE_H__A97FA5AE_5355_45CD_9092_ED94F5CD035B__INCLUDED_

#include <pathan/dataItem/DataItemImpl.hpp>
#include <pathan/StaticContext.hpp>

class XQENGINE_API XQOrderingChange : public DataItemImpl
{
public:
  XQOrderingChange(StaticContext::NodeSetOrdering ordering, DataItem* dataItem, XPath2MemoryManager* expr);

	virtual Result createResult(DynamicContext *context, int flags=0) const;
	virtual DataItem* staticResolution(StaticContext *context);

	StaticContext::NodeSetOrdering getOrderingValue() const;
  DataItem* getExpr() const;
  void setExpr(DataItem *expr);

protected:
  class OrderingChangeResult : public ResultImpl
  {
  public:
    OrderingChangeResult(const XQOrderingChange* orderChange, int flags, DynamicContext *context);

    Item::Ptr next(DynamicContext *context);
    std::string asString(DynamicContext *context, int indent) const;

  private:
    const XQOrderingChange* _oc;
    int _flags;
    Result _innerExpr;
  };

  StaticContext::NodeSetOrdering m_nOrdering;
  DataItem* m_pExpr;
};

#endif // !defined(AFX_XQORDERINGCHANGE_H__A97FA5AE_5355_45CD_9092_ED94F5CD035B__INCLUDED_)
