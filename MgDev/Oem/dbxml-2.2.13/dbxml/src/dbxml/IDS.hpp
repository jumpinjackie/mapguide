//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: IDS.hpp,v 1.17 2005/07/25 11:45:10 jsnelson Exp $
//

#ifndef __IDSET_HPP
#define	__IDSET_HPP

#include <algorithm>
#include <string>
#include <vector>
#include "ID.hpp"
#include "shared_ptr.hpp"
#include "Key.hpp"

namespace DbXml
{

class IDS
{
public:
	typedef std::vector<ID>::iterator iterator;
	typedef std::vector<ID>::const_iterator const_iterator;
	typedef shared_ptr<IDS> SharedPtr;
	IDS(size_t reserve = 64)
	{
		ids.reserve(reserve);
	}
	// ~IDS(); default is ok
	std::string toString() const;
	iterator begin()
	{
		return ids.begin();
	}
	iterator end()
	{
		return ids.end();
	}
	const_iterator begin() const
	{
		return ids.begin();
	}
	const_iterator end() const
	{
		return ids.end();
	}
	long size() const
	{
		return (long) ids.size();
	}
	bool empty() const
	{
		return ids.empty();
	}
	void push_back(ID id)
	{
		ids.push_back(id);
	}
	SharedPtr set_union(const SharedPtr &o) const
	{
		IDS *result = new IDS(o->size() + size());
		std::set_union(begin(), end(), o->begin(), o->end(), std::back_inserter(result->ids));
		return SharedPtr(result);
	}
	SharedPtr set_intersection(const SharedPtr &o) const
	{
		IDS *result = new IDS(o->size() + size());
		std::set_intersection(begin(), end(), o->begin(), o->end(), std::back_inserter(result->ids));
		return SharedPtr(result);
	}
	void clear()
	{
		ids.clear();
	}
private:
	// no need for copy and assignment
	IDS(const IDS&);
	IDS &operator=(const IDS &);

	std::vector<ID> ids;
};

}

#endif

