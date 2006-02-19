//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: KeyStatistics.cpp,v 1.38 2005/08/16 13:29:30 jsnelson Exp $
//

#include "DbXmlInternal.hpp"
#include "ScopedDbt.hpp"
#include "db_utils.h"
#include "Buffer.hpp"
#include "KeyStatistics.hpp"
#include "Manager.hpp"

#include <sstream>

using namespace DbXml;

KeyStatistics::KeyStatistics()
{
	zero();
}

void KeyStatistics::zero()
{
	numIndexedKeys_ = 0;
	numUniqueKeys_ = 0;
	sumKeyValueSize_ = 0;
}

void KeyStatistics::add(const KeyStatistics& es)
{
	numIndexedKeys_ += es.numIndexedKeys_;
	numUniqueKeys_ += es.numUniqueKeys_;
	sumKeyValueSize_ += es.sumKeyValueSize_;
}

double KeyStatistics::averageKeyValueSize() const
{
	return (numIndexedKeys_ == 0 ? 0 : sumKeyValueSize_ / numIndexedKeys_);
}

void KeyStatistics::setThisFromDbt(const DbtOut &dbt)
{
	Buffer buffer(dbt.get_data(), dbt.get_size(), /*wrapper*/true); // buffer doesn't make a copy.

	buffer.read(&numIndexedKeys_, sizeof(numIndexedKeys_));
	buffer.read(&numUniqueKeys_, sizeof(numUniqueKeys_));
	buffer.read(&sumKeyValueSize_, sizeof(sumKeyValueSize_));

	if (Manager::isBigendian()) {
		M_32_SWAP(numIndexedKeys_);
		M_32_SWAP(numUniqueKeys_);
		M_32_SWAP(sumKeyValueSize_);
	}
}

void KeyStatistics::setDbtFromThis(DbtOut &dbt) const
{
	Buffer buffer(0, 12);

	buffer.write(&numIndexedKeys_, sizeof(numIndexedKeys_));
	buffer.write(&numUniqueKeys_, sizeof(numUniqueKeys_));
	buffer.write(&sumKeyValueSize_, sizeof(sumKeyValueSize_));

	if(Manager::isBigendian()) {
		int32_t *p = reinterpret_cast<int32_t*>(buffer.getBuffer());
		M_32_SWAP(*(p + 0));
		M_32_SWAP(*(p + 1));
		M_32_SWAP(*(p + 2));
	}

	dbt.set(buffer.getBuffer(), buffer.getOccupancy());
}

std::string KeyStatistics::asString() const
{
	std::ostringstream s;
	s << "indexed=" << numIndexedKeys_;
	s << " unique=" << numUniqueKeys_;
	s << " size=" << sumKeyValueSize_;
	return s.str();
}

std::ostream& DbXml::operator<<(std::ostream& s, const KeyStatistics &ks)
{
	s << ks.asString();
	return s;
}
