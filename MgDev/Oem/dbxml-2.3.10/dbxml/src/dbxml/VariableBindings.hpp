//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: VariableBindings.hpp,v 1.19 2006/10/30 17:45:53 bostic Exp $
//

#ifndef __VARIABLEBINDINGS_HPP
#define	__VARIABLEBINDINGS_HPP

#include <string>
#include <map>
#include <dbxml/XmlResults.hpp>

namespace DbXml
{

class VariableBindings
{
public:
	typedef std::map<std::string, XmlResults> Values;
	VariableBindings()
	{}
	;
	~VariableBindings()
	{}
	;
	VariableBindings(const VariableBindings &);
	VariableBindings & operator = (const VariableBindings &);

	bool getVariableValue(const std::string &name, XmlResults &value) const;
	void setVariableValue(const std::string &name, const XmlResults &value);
	const Values &getValues() const
	{
		return values_;
	}
private:
	Values values_;
};

}

#endif

