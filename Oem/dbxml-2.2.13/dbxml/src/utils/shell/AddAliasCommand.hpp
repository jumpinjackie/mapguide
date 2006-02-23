//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: AddAliasCommand.hpp,v 1.1 2005/03/23 21:03:55 gmf Exp $
//

#ifndef __ADDALIASCOMMAND_HPP
#define	__ADDALIASCOMMAND_HPP

#include "ShellCommand.hpp"

class AddAliasCommand : public ShellCommand
{
public:
	AddAliasCommand() {};
	virtual ~AddAliasCommand() {};

	virtual std::string getCommandName() const;
	virtual std::string getBriefHelp() const;
	virtual std::string getMoreHelp() const;
	
	virtual void execute(Args &args, Environment &env);
};

#endif
