//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2004-2005
//	Sleepycat Software.  All rights reserved.
// Copyright (c) 2004-2005
//	Progress Software Corporation.  All rights reserved.
//
// $Id: XQException.hpp,v 1.5 2005/09/06 10:40:23 jsnelson Exp $
//

//////////////////////////////////////////////////////////////////////
// XQException.h: interface for the XQException class.
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XQEXCEPTION_H__446AD191_E9D0_4658_BD8C_032D29DA123E__INCLUDED_)
#define AFX_XQEXCEPTION_H__446AD191_E9D0_4658_BD8C_032D29DA123E__INCLUDED_

#include "XQEngine.hpp"
#include <pathan/exceptions/DSLException.hpp>
#include <pathan/internal/DOMutils/XStr.hpp>

class XQENGINE_API XQException : public DSLException
{
public:

  XQException(const XMLCh *reason, const XMLCh* file, unsigned int line, unsigned int column)
    : DSLException(X("XQException"), X("XQuery"), reason, __FILE__, __LINE__), m_nLine(line), m_nColumn(column), m_szFile(file) {};

  unsigned int m_nLine, m_nColumn;
  const XMLCh* m_szFile;
};

#endif // !defined(AFX_XQEXCEPTION_H__446AD191_E9D0_4658_BD8C_032D29DA123E__INCLUDED_)

