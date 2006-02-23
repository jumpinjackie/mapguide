//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: KeyStatistics.hpp,v 1.29 2005/04/05 16:44:02 bostic Exp $
//

#ifndef __KEYSTATISTICS_HPP
#define	__KEYSTATISTICS_HPP

#include <string>

namespace DbXml
{
class DbtOut;

class KeyStatistics
{
public:
	KeyStatistics();

	void add(const KeyStatistics& es);
	double averageKeyValueSize() const;
	void zero();

	void setThisFromDbt(const DbtOut &dbt);
	void setDbtFromThis(DbtOut &dbt) const;

	std::string asString() const;

	int32_t numIndexedKeys_;
	int32_t numUniqueKeys_;
	int32_t sumKeyValueSize_;
};

std::ostream& operator<<(std::ostream& s, const KeyStatistics &ks);

}

#endif

