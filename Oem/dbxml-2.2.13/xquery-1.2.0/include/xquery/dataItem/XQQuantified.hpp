//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2004-2005
//	Sleepycat Software.  All rights reserved.
// Copyright (c) 2004-2005
//	Progress Software Corporation.  All rights reserved.
//
// $Id: XQQuantified.hpp,v 1.9 2005/07/14 15:21:09 jsnelson Exp $
//

//////////////////////////////////////////////////////////////////////
// XQQuantified.hpp: interface for the XQQuantified class.
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XQQUANTIFIED_H__2B6902E9_A4FA_4AB3_9C69_08A8E77E70DA__INCLUDED_)
#define AFX_XQQUANTIFIED_H__2B6902E9_A4FA_4AB3_9C69_08A8E77E70DA__INCLUDED_

#include "../XQEngine.hpp"
#include "XQFLWOR.hpp"
#include <pathan/dataItem/SingleResult.hpp>

class XQENGINE_API XQQuantified : public XQFLWOR
{
public:
	typedef enum {some, every} QuantifierType;

	XQQuantified(QuantifierType qType, VectorOfVariableBinding* bindings, DataItem* returnExpr, XPath2MemoryManager *expr);

  virtual DataItem* staticResolution(StaticContext* context);

  QuantifierType getQuantifierType() const;

protected:
  virtual Result createResultImpl(VectorOfVariableBinding::const_iterator it, VectorOfVariableBinding::const_iterator end,
                                  DynamicContext* context, int flags = 0) const;

  class QuantifiedResult : public SingleResult
  {
  public:
    QuantifiedResult(VectorOfVariableBinding::const_iterator it, VectorOfVariableBinding::const_iterator end,
                     const XQQuantified *quantified, DynamicContext *context);

    Item::Ptr getSingleResult(DynamicContext *context) const;
    std::string asString(DynamicContext *context, int indent) const;

  private:
    const XQQuantified *_quantified;
    ExecutionBindings _ebs; // mutable
  };

	QuantifierType _qType;
};

#endif // !defined(AFX_XQQUANTIFIED_H__2B6902E9_A4FA_4AB3_9C69_08A8E77E70DA__INCLUDED_)
