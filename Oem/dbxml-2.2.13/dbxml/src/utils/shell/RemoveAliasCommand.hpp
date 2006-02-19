//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: RemoveAliasCommand.hpp,v 1.1 2005/03/23 21:03:56 gmf Exp $
//

#ifndef __REMOVEALIASCOMMAND_HPP
#define	__REMOVEALIASCOMMAND_HPP

#include "ShellCommand.hpp"

class RemoveAliasCommand : public ShellCommand
{
public:
	RemoveAliasCommand() {};
	virtual ~RemoveAliasCommand() {};

	virtual std::string getCommandName() const;
	virtual std::string getBriefHelp() const;
	virtual std::string getMoreHelp() const;
	
	virtual void execute(Args &args, Environment &env);
};

#endif
