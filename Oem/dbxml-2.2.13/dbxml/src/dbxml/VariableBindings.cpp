//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: VariableBindings.cpp,v 1.21 2005/08/26 11:50:07 jsnelson Exp $
//

#include "DbXmlInternal.hpp"
#include "dbxml/XmlException.hpp"
#include "VariableBindings.hpp"

using namespace DbXml;

// VariableBindings

VariableBindings::VariableBindings(const VariableBindings &o)
{
	*this = o;
}

VariableBindings &VariableBindings::operator = (const VariableBindings &o)
{
	if (this != &o) {
		values_ = o.values_;
	}
	return *this;
}

bool VariableBindings::getVariableValue(const std::string &name,
					XmlResults &value) const
{
	Values::const_iterator i = values_.find(name);
	if (i != values_.end()) {
		value = i->second;
		value.reset();
	} else {
		value = XmlResults();
	}
	return !value.isNull();
}

void VariableBindings::setVariableValue(const std::string &name,
					const XmlResults &value)
{
	values_[name] = value;
}
