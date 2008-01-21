//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: IDS.hpp,v 1.23 2006/11/17 18:18:25 jsnelson Exp $
//

#ifndef __IDSET_HPP
#define	__IDSET_HPP

#include <algorithm>
#include <string>
#include <vector>
#include "DocID.hpp"
#include "SharedPtr.hpp"
#include "Key.hpp"

namespace DbXml
{

class IDS
{
public:
	typedef std::vector<DocID>::iterator iterator;
	typedef std::vector<DocID>::const_iterator const_iterator;
	typedef SharedPtr<IDS> Ptr;
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
	void push_back(DocID id)
	{
		ids.push_back(id);
	}
	Ptr set_union(const Ptr &o) const
	{
		IDS *result = new IDS(o->size() + size());
		std::set_union(begin(), end(), o->begin(), o->end(), std::back_inserter(result->ids));
		return Ptr(result);
	}
	Ptr set_intersection(const Ptr &o) const
	{
		IDS *result = new IDS(o->size() + size());
		std::set_intersection(begin(), end(), o->begin(), o->end(), std::back_inserter(result->ids));
		return Ptr(result);
	}
	bool contains(DocID id) const
	{
		return std::binary_search(begin(), end(), id);
	}
	void clear()
	{
		ids.clear();
	}
private:
	// no need for copy and assignment
	IDS(const IDS&);
	IDS &operator=(const IDS &);

	std::vector<DocID> ids;
};

}

#endif

