//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: ID.hpp,v 1.29 2005/08/25 14:39:36 jsnelson Exp $
//

#ifndef __ID_HPP
#define	__ID_HPP

#include <iosfwd>
#include <db_cxx.h>

#include "nodeStore/NsUtil.hpp"

class Dbt;

namespace DbXml
{

class DbtOut;

class ID
{
public:
	ID() : id_(0)
	{}
	ID(u_int32_t id) : id_(id)
	{ }
	void reset()
	{
		id_ = 0;
	}
	u_int32_t raw() const
	{
		return id_;
	}
	const void *rawPtr() const
	{
		return &id_;
	}
	void setThisFromDbt(const Dbt &dbt);
	void setDbtFromThis(DbtOut &dbt) const;
	void setThisFromDbtAsId(const Dbt &dbt);
	void setDbtFromThisAsId(DbtOut &dbt) const;
	u_int32_t unmarshal(const void *buf);
	u_int32_t marshal(void *buf) const;
	u_int32_t size() const { return sizeof(u_int32_t); }
	u_int32_t marshalSize() const;
	int operator==(const ID &o) const
	{
		return id_ == o.id_;
	}
	int operator==(u_int32_t id) const
	{
		return id_ == id;
	}
	int operator!=(const ID &o) const
	{
		return id_ != o.id_;
	}
	int operator!=(u_int32_t id) const
	{
		return id_ != id;
	}
	bool operator<(const ID &o) const
	{
		return id_ < o.id_;
	}
	bool operator>(const ID &o) const
	{
		return id_ > o.id_;
	}

	static int compareMarshalled(const xmlbyte_t *&p1, const xmlbyte_t *&p2);

	// default implementation
	// ID(const ID&);
	// void operator=(const ID &);
private:
	u_int32_t id_;
};

std::ostream& operator<<(std::ostream& s, ID id);

inline int ID::compareMarshalled(const xmlbyte_t *&p1, const xmlbyte_t *&p2)
{
	// Sort based on marshalled integer length first
	int count = NsUtil::nsCountMarshalledInt(p1);
	int res = count - NsUtil::nsCountMarshalledInt(p2);

	if(res == 0) {
		// Marshalled integers can be sorted using normal
		// lexicographical comparison if they are the same
		// length.
		while(count-- && (res = (int)*p1 - (int)*p2) == 0) {
			++p1; ++p2;
		}
	}
	return res;
}

}

#endif

