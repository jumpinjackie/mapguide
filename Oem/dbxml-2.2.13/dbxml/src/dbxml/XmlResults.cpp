//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: XmlResults.cpp,v 1.37 2005/08/26 23:29:58 gmf Exp $
//

#include "DbXmlInternal.hpp"
#include "dbxml/XmlException.hpp"
#include "dbxml/XmlResults.hpp"
#include "dbxml/XmlDocument.hpp"
#include "Results.hpp"

using namespace DbXml;

static const char *className = "XmlResults";
#define CHECK_POINTER checkNullPointer(results_,className)

XmlResults::XmlResults()
  : results_(0)
{
}

XmlResults::XmlResults(Results *results)
	: results_(results)
{
	if (results_ != 0)
		results_->acquire();
}

XmlResults::~XmlResults()
{
	if (results_ != 0)
		results_->release();
}

XmlResults::XmlResults(const XmlResults &o)
	: results_(o.results_)
{
	if (results_ != 0)
		results_->acquire();
}

XmlResults &XmlResults::operator=(const XmlResults &o)
{
	if (this != &o && results_ != o.results_) {
		if (results_ != 0)
			results_->release();
		results_ = o.results_;
		if (results_ != 0)
			results_->acquire();
	}
	return *this;
}

void XmlResults::reset()
{
	CHECK_POINTER;
	results_->reset();
}

size_t XmlResults::size() const
{
	CHECK_POINTER;
	return results_->size();
}

bool XmlResults::hasNext()
{
	CHECK_POINTER;
	return results_->hasNext();
}

bool XmlResults::hasPrevious()
{
	CHECK_POINTER;
	return results_->hasPrevious();
}

bool XmlResults::next(XmlValue &value)
{
	CHECK_POINTER;
	int err = 0;
	try {
		err = results_->next(value);
	} catch (DbException &e) {
		throw XmlException(e);
	}
	if (err != 0)
		throw XmlException(err);

	return (!value.isNull());
}

bool XmlResults::peek(XmlValue &value)
{
	CHECK_POINTER;
	int err = 0;
	try {
		err = results_->peek(value);
	} catch (DbException &e) {
		throw XmlException(e);
	}
	if (err != 0)
		throw XmlException(err);

	return (!value.isNull());
}

bool XmlResults::previous(XmlValue &value)
{
	CHECK_POINTER;
	int err = 0;
	try {
		err = results_->previous(value);
	} catch (DbException &e) {
		throw XmlException(e);
	}
	if (err != 0)
		throw XmlException(err);

	return (!value.isNull());
}

bool XmlResults::next(XmlDocument &document)
{
	CHECK_POINTER;
	XmlValue value;
	if(next(value)) {
		document = value.asDocument();
		return true;
	}
	return false;
}

bool XmlResults::previous(XmlDocument &document)
{
	CHECK_POINTER;
	XmlValue value;
	if(previous(value)) {
		document = value.asDocument();
		return true;
	}
	return false;
}

bool XmlResults::peek(XmlDocument &document)
{
	CHECK_POINTER;
	XmlValue value;
	if(peek(value)) {
		document = value.asDocument();
		return true;
	}
	return false;
}

void XmlResults::add(const XmlValue &value)
{
	CHECK_POINTER;
	results_->add(value);
}

