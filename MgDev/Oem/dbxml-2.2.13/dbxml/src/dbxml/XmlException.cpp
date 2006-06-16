//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: XmlException.cpp,v 1.27 2005/08/16 13:29:32 jsnelson Exp $
//

#include "DbXmlInternal.hpp"
#include "dbxml/XmlException.hpp"

#include <db_cxx.h>

#include <string>
#include <sstream>

using namespace DbXml;

//
// jcm - Note that the member variable description is not a std::string
// because in some cases when the XmlException object is deleted an exception
// is thrown by the MSVC6 debug heap. Using strdup and free seems to work ok.
//

XmlException::XmlException(ExceptionCode ec, const std::string &description, const char *file, int line)
	: exceptionCode_(ec),
	dberr_(0),
	description_(::strdup(description.c_str())),
	file_(file),
	line_(line),
	text_(0)
{
	describe();
}

XmlException::XmlException(ExceptionCode ec, const char *description, const char *file, int line)
	: exceptionCode_(ec),
	dberr_(0),
	description_(::strdup(description)),
	file_(file),
	line_(line),
	text_(0)
{
	describe();
}

XmlException::XmlException(const DbException &e, const char *file, int line)
	: exceptionCode_(DATABASE_ERROR),
	dberr_(e.get_errno()),
	description_(::strdup(e.what())),
	file_(file),
	line_(line),
	text_(0)
{
	describe();
}

XmlException::XmlException(int dberr, const char *file, int line)
	: exceptionCode_(DATABASE_ERROR),
	dberr_(dberr),
	description_(::strdup(db_strerror(dberr))),
	file_(file),
	line_(line),
	text_(0)
{
	describe();
}

XmlException::XmlException(const XmlException &that)
	: std::exception(that),
	exceptionCode_(that.exceptionCode_),
	dberr_(that.dberr_),
	description_(::strdup(that.description_)),
	file_(that.file_),
	line_(that.line_),
	text_(::strdup(that.text_))
{}

XmlException::~XmlException() throw()
{
	file_ = 0;
	::free(description_);
	if (text_ != 0)
		::free(text_);
}

void XmlException::describe()
{
	std::ostringstream s;
	s << "Error: ";
	s << description_;
	if (file_ != 0) {
		s << " File: ";
		s << file_;

		if (line_ != 0) {
			s << " Line: ";
			s << line_;
		}
	}
	text_ = ::strdup(s.str().c_str());
}

const char *XmlException::what() const throw()
{
	return text_;
}
