//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: SetTypedVariableCommand.hpp,v 1.4 2005/04/05 16:44:29 bostic Exp $
//

#ifndef __SETTYPEDVARIABLECOMMAND_HPP
#define	__SETTYPEDVARIABLECOMMAND_HPP

#include "dbxml/DbXml.hpp"
#include <vector>
#include <string>

#include "ShellCommand.hpp"

class SetTypedVariableCommand : public ShellCommand
{
public:
	SetTypedVariableCommand() {};
	virtual ~SetTypedVariableCommand() {};

	virtual std::string getCommandName() const;
	virtual std::string getCommandNameCompat() const;
	virtual std::string getBriefHelp() const;
	virtual std::string getMoreHelp() const;
	
	virtual void execute(Args &args, Environment &env);
	
private:
	static DbXml::XmlValue::Type getValueType(std::string type);
};

#endif
