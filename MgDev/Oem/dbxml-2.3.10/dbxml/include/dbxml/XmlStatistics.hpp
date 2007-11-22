//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: XmlStatistics.hpp,v 1.5 2006/10/30 17:45:48 bostic Exp $
//

#ifndef __XMLSTATISTICS_HPP
#define __XMLSTATISTICS_HPP

#include "XmlPortability.hpp"

namespace DbXml
{

class Statistics;

class DBXML_EXPORT XmlStatistics
{
public:
	/** @name For Reference Counting */
	XmlStatistics();
	XmlStatistics(const XmlStatistics&);
	XmlStatistics &operator=(const XmlStatistics &);
	virtual ~XmlStatistics();
	bool isNull() const { return statistics_ == 0; }

	/** @name Statistics Methods */
	double getNumberOfIndexedKeys() const;
	double getNumberOfUniqueKeys() const;
	double getSumKeyValueSize() const;

	/** @name Private Methods (for internal use) */
	// @{

	XmlStatistics(Statistics *statistics);
	operator Statistics *()
	{
		return statistics_;
	}
	operator Statistics &()
	{
		return *statistics_;
	}

	//@}

private:
	Statistics *statistics_;
};

}

#endif

