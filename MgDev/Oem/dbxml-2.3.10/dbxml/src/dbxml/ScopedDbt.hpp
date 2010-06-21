//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: ScopedDbt.hpp,v 1.31 2006/10/30 17:45:53 bostic Exp $
//

#ifndef __SCOPEDDBT_HPP
#define	__SCOPEDDBT_HPP

#include "DbXmlInternal.hpp"
#include <cstdlib>
#include <cstring>
#include "db_cxx.h"
#include "SharedPtr.hpp"

namespace DbXml
{

// A Dbt for passing data into a DB method.
class DbtIn : public Dbt
{
public:
	DbtIn()
	{
		Dbt::set_flags(DB_DBT_USERMEM);
	}
	DbtIn(void *data, size_t size)
	{
		Dbt::set_flags(DB_DBT_USERMEM);
		set(data, size);
	}
	DbtIn(const DbtIn &dbt)
	{
		Dbt::set_flags(DB_DBT_USERMEM);
		set(dbt.get_data(), dbt.get_size());
	}
	~DbtIn()
	{}
	void set_flags(u_int32_t flags)
	{
		DBXML_ASSERT(!(flags&(DB_DBT_MALLOC | DB_DBT_REALLOC | DB_DBT_USERMEM)));
		Dbt::set_flags(DB_DBT_USERMEM | flags);
	}
	void set(void *data, size_t size)
	{
		set_data(data);
		set_size((u_int32_t)size);
		set_ulen((u_int32_t)size);
	}
	bool operator<(const DbtIn &o) const
	{
		size_t s1 = get_size();
		size_t s2 = o.get_size();
		return (s1 == s2 ? memcmp(get_data(), o.get_data(), s1) < 0 : s1 < s2);
	}
private:
	DbtIn &operator= (const DbtIn &o);
};

// A Dbt for passing into and getting data back from a DB method.
class DbtOut : public Dbt
{
public:
	typedef DbXml::SharedPtr<DbXml::DbtOut> Ptr;
	DbtOut()
	{
		Dbt::set_flags(DB_DBT_REALLOC);
	}
	DbtOut(const void *data, size_t size)
	{
		Dbt::set_flags(DB_DBT_REALLOC);
		set(data, size);
	}
	DbtOut(const Dbt &dbt)
	{
		Dbt::set_flags(DB_DBT_REALLOC);
		set(dbt.get_data(), dbt.get_size());
	}
	~DbtOut()
	{
		if (get_data()) {
			::free(get_data());
		}
	}
	void set_flags(u_int32_t flags)
	{
		DBXML_ASSERT(!(flags&(DB_DBT_MALLOC | DB_DBT_REALLOC | DB_DBT_USERMEM)));
		Dbt::set_flags(DB_DBT_REALLOC | flags);
	}
	void set(const void *data, size_t size)
	{
		if (size > get_size()) {
			Dbt::set_data(::realloc(get_data(), size));
		}
		Dbt::set_size((u_int32_t)size);
		if (data != 0) {
			::memcpy(get_data(), data, size);
		}
	}
	void setNoCopy(void *data, size_t size)
	{
		if (get_data()) {
			::free(get_data());
		}
		Dbt::set_data(data);
		Dbt::set_size((u_int32_t)size);
	}
	void set_data(void *)
	{
		DBXML_ASSERT(0);
	}
	void set_size(size_t size)
	{
		set(0, size);
	}
};

}

#endif
