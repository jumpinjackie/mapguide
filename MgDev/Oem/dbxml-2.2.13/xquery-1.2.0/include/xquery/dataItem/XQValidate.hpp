//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2004-2005
//	Sleepycat Software.  All rights reserved.
// Copyright (c) 2004-2005
//	Progress Software Corporation.  All rights reserved.
//
// $Id: XQValidate.hpp,v 1.11 2005/07/14 15:21:10 jsnelson Exp $
//

//////////////////////////////////////////////////////////////////////
// XQValidate.hpp: interface for the XQValidate class.
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XQVALIDATE_H__2B6902E9_A4FA_4AB3_9C69_08A8E77E70DA__INCLUDED_)
#define AFX_XQVALIDATE_H__2B6902E9_A4FA_4AB3_9C69_08A8E77E70DA__INCLUDED_

#include "../XQEngine.hpp"
#include <pathan/dataItem/DataItemImpl.hpp>
#include <pathan/DocumentCache.hpp>

/** Validate function. */
class XQENGINE_API XQValidate : public DataItemImpl
{
public:
  XQValidate(DataItem* valExpr, DocumentCache::ValidationMode valMode, XPath2MemoryManager* expr);

  /** evaluate the expression, and check if it's valid according to the schemas **/
  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;

  /**
   *  Called during static analysis to determine if statically correct.  If
   *  not an appropriate error is thrown.
   */
  virtual DataItem* staticResolution(StaticContext *context);

  const DataItem *getExpression() const;
  DocumentCache::ValidationMode getValidationMode() const;

  void setExpression(DataItem *expr);

protected:
  DataItem* _expr;
  DocumentCache::ValidationMode _validationMode;
};

#endif // !defined(AFX_XQVALIDATE_H__2B6902E9_A4FA_4AB3_9C69_08A8E77E70DA__INCLUDED_)
