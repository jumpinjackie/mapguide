//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2004-2005
//	Sleepycat Software.  All rights reserved.
// Copyright (c) 2004-2005
//	Progress Software Corporation.  All rights reserved.
//
// $Id: XQStaticContext.hpp,v 1.9 2005/07/06 14:31:11 alby Exp $
//

//////////////////////////////////////////////////////////////////////
// XQStaticContext.h: interface for the XQuery static context.
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XQSTATICCONTEXT_H__D608B994_E090_4206_4973_81F3D7350410__INCLUDED_)
#define AFX_XQSTATICCONTEXT_H__D608B994_E090_4206_4973_81F3D7350410__INCLUDED_

#include "../XQEngine.hpp"
#include <pathan/DocumentCache.hpp>
#include <xercesc/dom/DOMDocument.hpp>

class XQENGINE_API XQStaticContext
{
public:
  virtual ~XQStaticContext() {};

  typedef enum {
    CONSTRUCTION_MODE_PRESERVE,
    CONSTRUCTION_MODE_STRIP
  } ConstructionMode;

  typedef enum {
    FLWOR_ORDER_EMPTY_GREATEST,
    FLWOR_ORDER_EMPTY_LEAST
  } FLWOROrderingMode;

  /** Binds a prefix to a namespace URI */
  virtual void setNamespaceBinding(const XMLCh* prefix, const XMLCh* uri) = 0;

  /** Return the construction mode */
  virtual ConstructionMode getConstructionMode() const = 0;
  /** Set the construction mode */
  virtual void setConstructionMode(ConstructionMode newMode) = 0;

  /** Set the policy for boundary space */
  virtual void setPreserveBoundarySpace(bool value) = 0;
  /** Get the policy for boundary  space */
  virtual bool getPreserveBoundarySpace() const = 0;

  /** Return the default ordering mode for FLWOR blocks */
  virtual FLWOROrderingMode getDefaultFLWOROrderingMode() const = 0;
  /** Set the default ordering mode for FLWOR blocks */
  virtual void setDefaultFLWOROrderingMode(FLWOROrderingMode newMode) = 0;

  /** Set the policy for namespace inheritance */
  virtual void setInheritNamespaces(bool value) = 0;
  /** Get the policy for namespace inheritance */
  virtual bool getInheritNamespaces() const = 0;

  /** Set the policy for namespace copy */
  virtual void setPreserveNamespaces(bool value) = 0;
  /** Get the policy for namespace copy */
  virtual bool getPreserveNamespaces() const = 0;

  /// The enableDebuging flag is considered to be in both the static and dynamic contexts
  virtual void enableDebugging(bool enable=true) = 0;
  /// The enableDebuging flag is considered to be in both the static and dynamic contexts
  virtual bool isDebuggingEnabled() const = 0;
};

#endif // !defined(AFX_XQSTATICCONTEXT_H__D608B994_E090_4206_4973_81F3D7350410__INCLUDED_)
