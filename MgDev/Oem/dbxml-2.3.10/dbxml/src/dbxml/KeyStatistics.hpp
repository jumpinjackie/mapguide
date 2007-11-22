//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: KeyStatistics.hpp,v 1.32 2006/10/30 17:45:52 bostic Exp $
//

#ifndef __KEYSTATISTICS_HPP
#define	__KEYSTATISTICS_HPP

#include <string>

#include "nodeStore/NsTypes.hpp"

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

	int marshal(xmlbyte_t *ptr, bool count) const;
	void unmarshal(const xmlbyte_t *ptr);

	std::string asString() const;

	int32_t numIndexedKeys_;
	int32_t numUniqueKeys_;
	int32_t sumKeyValueSize_;
};

std::ostream& operator<<(std::ostream& s, const KeyStatistics &ks);

}

#endif

