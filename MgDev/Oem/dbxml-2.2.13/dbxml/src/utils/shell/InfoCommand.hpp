//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: InfoCommand.hpp,v 1.1 2005/03/23 21:03:56 gmf Exp $
//

#ifndef __INFOCOMMAND_HPP
#define	__INFOCOMMAND_HPP

#include "ShellCommand.hpp"

class InfoCommand : public ShellCommand
{
public:
	InfoCommand() {};
	virtual ~InfoCommand() {};

	virtual std::string getCommandName() const;
	virtual std::string getBriefHelp() const;
	virtual std::string getMoreHelp() const;
	
	virtual void execute(Args &args, Environment &env);
};

#endif
