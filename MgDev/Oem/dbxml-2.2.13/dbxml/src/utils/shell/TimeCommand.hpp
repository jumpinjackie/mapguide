//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: TimeCommand.hpp,v 1.1 2005/12/06 00:01:48 gmf Exp $
//

#ifndef __TIMECOMMAND_HPP
#define	__TIMECOMMAND_HPP

#include "ShellCommand.hpp"

class TimeCommand : public ShellCommand
{
public:
	TimeCommand() {};
	virtual ~TimeCommand() {};

	virtual std::string getCommandName() const;
	virtual std::string getBriefHelp() const;
	virtual std::string getMoreHelp() const;
	
	virtual void execute(Args &args, Environment &env);
};

#endif
