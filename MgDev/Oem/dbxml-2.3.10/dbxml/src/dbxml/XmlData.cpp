//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: XmlData.cpp,v 1.5 2006/10/30 17:45:54 bostic Exp $
//

#include <db_cxx.h>

#include "dbxml/XmlData.hpp"

using namespace DbXml;

XmlData::XmlData()
  : dbt_(new Dbt())
{
}

XmlData::XmlData(const XmlData &o)
  : dbt_(new Dbt(*o.dbt_))
{
}

XmlData::XmlData(const Dbt &dbt)
  : dbt_(new Dbt(dbt))
{
}

XmlData::XmlData(void *data, u_int32_t size)
  : dbt_(new Dbt(data, size))
{
}

XmlData &XmlData::operator=(const XmlData &o)
{
  if(&o != this) {
    delete dbt_;
    dbt_ = new Dbt(*o.dbt_);
  }
  return *this;
}

XmlData::~XmlData()
{
  delete dbt_;
}

void *XmlData::get_data() const
{
  return dbt_->get_data();
}

void XmlData::set_data(void *value)
{
  dbt_->set_data(value);
}

u_int32_t XmlData::get_size() const
{
  return dbt_->get_size();
}

void XmlData::set_size(u_int32_t size)
{
  dbt_->set_size(size);
}

Dbt &XmlData::getDbt()
{
  return *dbt_;
}

const Dbt &XmlData::getDbt() const
{
  return *dbt_;
}

