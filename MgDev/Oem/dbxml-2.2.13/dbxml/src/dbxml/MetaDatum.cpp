//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: MetaDatum.cpp,v 1.13 2005/08/12 00:06:45 gmf Exp $
//

#include <dbxml/XmlData.hpp>
#include "MetaDatum.hpp"
#include "Container.hpp"
#include "ID.hpp"
#include "OperationContext.hpp"
#include "ScopedDbt.hpp"
#include "Manager.hpp"

#include "db_utils.h"

using namespace DbXml;

MetaDatum::MetaDatum(const Name &name, XmlValue::Type type)
	: name_(name),
	  type_(type),
	  dbt_(0),
	  modified_(false),
	  removed_(false)
{}

MetaDatum::MetaDatum(const Name &name, XmlValue::Type type, DbtOut **dbt, bool modified)
	: name_(name),
	  type_(type),
	  dbt_(*dbt),
	  modified_(modified),
	  removed_(false)
{
	*dbt = 0;
}

MetaDatum::~MetaDatum()
{
	delete dbt_;
}

void MetaDatum::setDbt(MetaDatum *md) // Note: Consumes md
{
	setDbt(&md->dbt_);
	delete md;
}

const DbtOut *MetaDatum::getDbt() const
{
	return dbt_;
}

void MetaDatum::setDbt(DbtOut **dbt) // Note: Consumes the Dbt
{
	delete dbt_;
	dbt_ = *dbt;
	modified_ = true;
	*dbt = 0;
}

const char *MetaDatum::getValue() const
{
	const char *r = 0;
	if(dbt_ && dbt_->get_size() > 0) {
		r = (const char *)dbt_->get_data();
	}
	return r;
}

XmlValue::Type MetaDatum::getType() const
{
	return type_;
}

const Name &MetaDatum::getName() const
{
	return name_;
}

void MetaDatum::decodeKeyDbt(const DbtOut &key, ID &did, ID &nid,
			     XmlValue::Type &type)
{
	// Key
	//  did nid type
	const char *p = (const char *)key.get_data();
	u_int32_t len = did.unmarshal(p);
	len += nid.unmarshal(p+len);
	if (len <= key.get_size())
		type = (XmlValue::Type) * ((unsigned char*)(p+len));
	else
		type = XmlValue::NONE;
}

void MetaDatum::setKeyDbt(const ID &did, const ID &nid,
			  XmlValue::Type type, DbtOut &key)
{
	char dbuf[10], nbuf[10]; // large enough for any ID
	// Key
	u_int32_t dlen = did.marshal(dbuf);
	u_int32_t nlen = nid.marshal(nbuf);
	size_t l = dlen + nlen + (type == XmlValue::NONE ? 0 : 1);
	key.set(0, l);
	Buffer b(key.get_data(), l, /*wrapper=*/true);
	b.write(dbuf, dlen);
	b.write(nbuf, nlen);
	if (type != XmlValue::NONE) {
		unsigned char t = type;
		b.write(&t, sizeof(t));
	}
}

void MetaDatum::setValueDbtFromThis(DbtIn &value)
{

	value.set_data(dbt_->get_data());
	value.set_size(dbt_->get_size());
}

bool MetaDatum::canBeIndexed() const
{
  return  dbt_ != 0 &&
	  type_ >= XmlValue::ANY_SIMPLE_TYPE &&
	  !removed_;
}

void MetaDatum::asValue(XmlValue &value) const
{
	value = XmlValue(type_, *dbt_);
}
