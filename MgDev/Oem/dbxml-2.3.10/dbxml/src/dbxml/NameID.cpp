//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: NameID.cpp,v 1.2 2006/10/30 17:45:53 bostic Exp $
//

#include <sstream>

#include "DbXmlInternal.hpp"
#include "NameID.hpp"
#include "ScopedDbt.hpp"
#include "Manager.hpp"
#include "nodeStore/NsFormat.hpp"

using namespace DbXml;
using namespace std;

void NameID::setThisFromDbt(const Dbt &dbt)
{
	unmarshal(dbt.get_data());
}

void NameID::setDbtFromThis(DbtOut &dbt) const
{
	char buf[sizeof(nameId_t)+1];
	u_int32_t len = marshal(buf);
	dbt.set(buf, len); // copies the data
}

void NameID::setThisFromDbtAsId(const Dbt &dbt)
{
	id_ = *(reinterpret_cast<nameId_t *>(dbt.get_data()));
}

void NameID::setDbtFromThisAsId(DbtOut &dbt) const
{
	dbt.set(&id_, size());
}

u_int32_t NameID::unmarshal(const void *buf)
{
	return NsFormat::unmarshalInt((const xmlbyte_t *) buf, &id_);
}

u_int32_t NameID::marshal(void *buf) const
{
	return NsFormat::marshalInt((xmlbyte_t *)buf, id_);
}

u_int32_t NameID::marshalSize() const
{
	return NsFormat::countInt(id_);
}

// Is this used????
int NameID::compareMarshaled(const unsigned char *&p1,
			     const unsigned char *&p2)
{
	// Sort based on marshaled integer length first
	int count = NsFormat::countMarshaledInt(p1);
	int res = count - NsFormat::countMarshaledInt(p2);
	if(res != 0) return res;

	// Marshaled integers can be sorted using normal
	// lexicographical comparison if they are the same
	// length.
	while(count && (res = (int)*p1 - (int)*p2) == 0) {
		++p1; ++p2; --count;
	}
	return res;
}

std::ostream& DbXml::operator<<(std::ostream& s, NameID id)
{
	return s << id.raw();
}

