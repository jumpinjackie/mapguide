//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: XmlResults.hpp,v 1.36 2005/04/05 16:43:56 bostic Exp $
//

#ifndef __XMLRESULTS_HPP
#define __XMLRESULTS_HPP

#include "DbXmlFwd.hpp"
#include "XmlPortability.hpp"
#include <string>

namespace DbXml
{

class Results;

class DBXML_EXPORT XmlResults
{
public:
	/** @name For Reference Counting */
	XmlResults();
	XmlResults(const XmlResults&);
	XmlResults &operator=(const XmlResults &);
	virtual ~XmlResults();
	bool isNull() const { return results_ == 0; }

	/** @name Iteration Methods */
	bool next(XmlValue &value);
	bool next(XmlDocument &document);

	bool previous(XmlValue &value);
	bool previous(XmlDocument &document);

	bool peek(XmlValue &value);
	bool peek(XmlDocument &document);

	bool hasNext();
	bool hasPrevious();
	void reset();

	/** @name Eager Results Methods */
	size_t size() const;
	void add(const XmlValue &value);

	/** @name Private Methods (for internal use) */
	// @{

	XmlResults(Results *results);
	operator Results *()
	{
		return results_;
	}
	operator Results &()
	{
		return *results_;
	}

	//@}

private:
	Results *results_;
};

}

#endif

