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
 * $Id: LocationInfo.hpp,v 1.2 2006/11/01 16:37:11 jpcs Exp $
 */

#ifndef _LOCATIONINFO_HPP
#define _LOCATIONINFO_HPP


#include <xqilla/framework/XQillaExport.hpp>

class XQILLA_API LocationInfo
{
public:
  LocationInfo()
    : file_(0), line_(0), column_(0) {}
  LocationInfo(const XMLCh *file, unsigned int line, unsigned int column)
    : file_(file), line_(line), column_(column) {}

  const XMLCh *getFile() const { return file_; }
  unsigned int getLine() const { return line_; }
  unsigned int getColumn() const { return column_; }
  void setLocationInfo(const XMLCh *file, unsigned int line, unsigned int column)
  {
    file_ = file;
    line_ = line;
    column_ = column;
  }
  void setLocationInfo(const LocationInfo *o)
  {
    if(o != 0) {
      file_ = o->file_;
      line_ = o->line_;
      column_ = o->column_;
    }
  }

private:
  const XMLCh *file_;
  unsigned int line_, column_;
};

#endif
