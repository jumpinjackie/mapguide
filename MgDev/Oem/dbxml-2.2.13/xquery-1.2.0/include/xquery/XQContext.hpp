//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2004-2005
//	Sleepycat Software.  All rights reserved.
// Copyright (c) 2004-2005
//	Progress Software Corporation.  All rights reserved.
//
// $Id: XQContext.hpp,v 1.13 2005/04/05 16:44:43 bostic Exp $
//

//////////////////////////////////////////////////////////////////////
// XQContext.h: interface for the XQContext class.
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XQCONTEXT_H__D608B994_E090_4206_9473_81F3D7350410__INCLUDED_)
#define AFX_XQCONTEXT_H__D608B994_E090_4206_9473_81F3D7350410__INCLUDED_

#include "XQEngine.hpp"
#include "context/XQDynamicContext.hpp"
#include <pathan/DynamicContext.hpp>
#include <pathan/dataItem/DataItem.hpp>

#define CAST_TO_CONST_XQCONTEXT(x) (static_cast<const XQContext*>(x))
#define CAST_TO_CONST_DYNAMIC(x) (static_cast<const XQDynamicContext*>(CAST_TO_CONST_XQCONTEXT(x)))
#define CAST_TO_CONST_STATIC(x) (static_cast<const XQStaticContext*>(CAST_TO_CONST_XQCONTEXT(x)))

#define CAST_TO_XQCONTEXT(x) (static_cast<XQContext*>(x))
#define CAST_TO_DYNAMIC(x) (static_cast<XQDynamicContext*>(CAST_TO_XQCONTEXT(x)))
#define CAST_TO_STATIC(x) (static_cast<XQStaticContext*>(CAST_TO_XQCONTEXT(x)))

class XQENGINE_API XQContext : public XQDynamicContext, public DynamicContext
{
public:
  enum {
    DEBUG_HOOK = DataItem::PARENTHESIZED + 20,
    DOM_CONSTRUCTOR,
    FLWOR,
    FLWOR_QUANTIFIED,
    TYPESWITCH,
    VALIDATE,
    VARIABLE_DEFINITION,
    FUNCTION_CALL,
    USER_FUNCTION,
    ORDERING_CHANGE
  } whichType2;

  virtual ~XQContext() {};
};

#endif // !defined(AFX_XQCONTEXT_H__D608B994_E090_4206_9473_81F3D7350410__INCLUDED_)
