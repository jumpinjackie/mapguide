//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: SetVerboseCommand.hpp,v 1.2 2005/04/05 16:44:29 bostic Exp $
//

#ifndef __SETVERBOSECOMMAND_HPP
#define	__SETVERBOSECOMMAND_HPP

#include "ShellCommand.hpp"

class SetVerboseCommand : public ShellCommand
{
public:
	SetVerboseCommand() {};
	virtual ~SetVerboseCommand() {};
	
	virtual std::string getCommandName() const;
	virtual std::string getBriefHelp() const;
	virtual std::string getMoreHelp() const;
	
	virtual void execute(Args &args, Environment &env);
};

#endif
