//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
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
