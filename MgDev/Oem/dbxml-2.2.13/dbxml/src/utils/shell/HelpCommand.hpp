//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: HelpCommand.hpp,v 1.4 2005/09/26 15:04:42 gmf Exp $
//

#ifndef __HELPCOMMAND_HPP
#define	__HELPCOMMAND_HPP

#include "dbxml/DbXml.hpp"
#include <vector>
#include <string>

#include "ShellCommand.hpp"

class HelpCommand : public ShellCommand
{
public:
	HelpCommand(std::vector<ShellCommand*> &commands) : commands_(commands) {};
	virtual ~HelpCommand() {};
	
	virtual std::string getCommandName() const;
	virtual std::string getCommandNameCompat() const;
	virtual std::string getBriefHelp() const;
	virtual std::string getMoreHelp() const;
	
	virtual void execute(Args &args, Environment &env);
	
private:
	std::vector<ShellCommand*> &commands_;
};

#endif
