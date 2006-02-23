//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: ID.cpp,v 1.24 2005/08/19 18:20:42 gmf Exp $
//

#include "DbXmlInternal.hpp"
#include "ID.hpp"
#include "ScopedDbt.hpp"
#include "Manager.hpp"
#include "nodeStore/NsUtil.hpp"

#include "db_utils.h"

using namespace DbXml;

void ID::setThisFromDbt(const Dbt &dbt)
{
	unmarshal(dbt.get_data());
}

void ID::setDbtFromThis(DbtOut &dbt) const
{
	char buf[5];
	u_int32_t len = marshal(buf);
	dbt.set(buf, len); // copies the data
}

void ID::setThisFromDbtAsId(const Dbt &dbt)
{
	id_ = *(reinterpret_cast<u_int32_t *>(dbt.get_data()));
}

void ID::setDbtFromThisAsId(DbtOut &dbt) const
{
	dbt.set(&id_, size());
}

u_int32_t ID::unmarshal(const void *buf)
{
	return NsUtil::nsUnmarshInt((const xmlbyte_t *) buf, &id_);
}

u_int32_t ID::marshal(void *buf) const
{
	return NsUtil::nsMarshInt((xmlbyte_t *)buf, id_);
}

u_int32_t ID::marshalSize() const
{
	return NsUtil::nsCountInt(id_);
}

std::ostream& DbXml::operator<<(std::ostream& s, ID id)
{
	return s << id.raw();
}
