/*
 * Copyright (c) 2001-2006
 *     DecisionSoft Limited. All rights reserved.
 * Copyright (c) 2004-2006
 *     Progress Software Corporation. All rights reserved.
 * Copyright (c) 2004-2006
 *     Oracle. All rights reserved.
 *
 * See the file LICENSE for redistribution information.
 *
 * $Id: XQException.hpp,v 1.10 2006/11/01 16:37:12 jpcs Exp $
 */

//////////////////////////////////////////////////////////////////////
// XQException.h: interface for the XQException class.
//////////////////////////////////////////////////////////////////////

#if !defined(AFXQ_XQEXCEPTION_H__446AD191_E9D0_4658_BD8C_032D29DA123E__INCLUDED_)
#define AFXQ_XQEXCEPTION_H__446AD191_E9D0_4658_BD8C_032D29DA123E__INCLUDED_

#include <xqilla/framework/XQillaExport.hpp>
#include <xqilla/utils/XStr.hpp>

#define XQThrow(type, function, reason) throw type(function, reason, this, __FILE__, __LINE__)
#define XQThrow2(type, function, reason) throw type(function, reason, 0, __FILE__, __LINE__)
#define XQThrow3(type, function, reason, location) throw type(function, reason, location, __FILE__, __LINE__)
#define XQSimpleThrow(reason, xqfile, xqline, xqcolumn) throw XQException(reason, xqfile, xqline, (unsigned int)xqcolumn, __FILE__, __LINE__)

class LocationInfo;

class XQILLA_API XQException
{
public:
  XQException(const XMLCh *reason, const XMLCh* file, unsigned int line, unsigned int column, const char *cppFile, unsigned int cppLine);
  XQException(const XQException &);
  virtual ~XQException();

  const XMLCh *getType() const { return m_type; }
  const XMLCh *getError() const { return m_error; }

  const XMLCh *getXQueryFile() const { return m_xqFile; }
  unsigned int getXQueryLine() const { return m_xqLine; }
  unsigned int getXQueryColumn() const { return m_xqColumn; }

  void setXQueryPosition(const XMLCh *file, unsigned int line, unsigned int column);
  void setXQueryPosition(const LocationInfo *info);

  const XMLCh *getCppFunction() const { return m_cppFunction; }
  const char *getCppFile() const { return m_cppFile; }
  unsigned int getCppLine() const { return m_cppLine; }

  /// For debugger
  bool isErrorReported() const { return m_errorReported; }
  void setErrorReported(bool value = true) { m_errorReported = value; }

  void printDebug(const XMLCh* const context) const;

protected:
  XQException(const XMLCh* const type, const XMLCh* const functionName, const XMLCh* const reason, const LocationInfo *info, const char *cppFile, unsigned int cppLine);

private:
  XQException &operator=(const XQException &);
  
  XMLCh* m_type;
  XMLCh* m_error;

  XMLCh* m_cppFunction;
  const char *m_cppFile;
  unsigned int m_cppLine;

  unsigned int m_xqLine, m_xqColumn;
  XMLCh* m_xqFile;

  bool m_errorReported;
};

#endif // !defined(AFXQ_XQEXCEPTION_H__446AD191_E9D0_4658_BD8C_032D29DA123E__INCLUDED_)

