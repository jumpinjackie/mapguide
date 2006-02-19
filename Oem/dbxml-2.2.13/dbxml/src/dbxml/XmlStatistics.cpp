//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: XmlStatistics.cpp,v 1.5 2005/08/26 23:29:58 gmf Exp $
//

#include "DbXmlInternal.hpp"
#include "dbxml/XmlStatistics.hpp"
#include "Statistics.hpp"

using namespace DbXml;

static const char *className = "XmlStatistics";
#define CHECK_POINTER checkNullPointer(statistics_,className)

XmlStatistics::XmlStatistics()
  : statistics_(0)
{
}

XmlStatistics::XmlStatistics(Statistics *statistics)
  : statistics_(statistics)
{
  statistics_->acquire();
}

XmlStatistics::~XmlStatistics()
{
	statistics_->release();
}

XmlStatistics::XmlStatistics(const XmlStatistics &o)
	: statistics_(o.statistics_)
{
	if (statistics_ != 0)
		statistics_->acquire();
}

XmlStatistics &XmlStatistics::operator=(const XmlStatistics &o)
{
	if (this != &o && statistics_ != o.statistics_) {
		if (statistics_ != 0)
			statistics_->release();
		statistics_ = o.statistics_;
		if (statistics_ != 0)
			statistics_->acquire();
	}
	return *this;
}

double XmlStatistics::getNumberOfIndexedKeys() const
{
	CHECK_POINTER;
	return statistics_->getNumberOfIndexedKeys();
}

double XmlStatistics::getNumberOfUniqueKeys() const
{
	CHECK_POINTER;
	return statistics_->getNumberOfUniqueKeys();
}

double XmlStatistics::getSumKeyValueSize() const
{
	CHECK_POINTER;
	return statistics_->getSumKeyValueSize();
}

