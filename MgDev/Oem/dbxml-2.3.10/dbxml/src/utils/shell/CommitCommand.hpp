//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: CommitCommand.hpp,v 1.6 2006/10/30 17:46:07 bostic Exp $
//

#ifndef __COMMITCOMMAND_HPP
#define	__COMMITCOMMAND_HPP

#include "ShellCommand.hpp"

class CommitCommand : public ShellCommand
{
public:
	CommitCommand() {};
	virtual ~CommitCommand() {};

	virtual std::string getCommandName() const;
	virtual std::string getBriefHelp() const;
	virtual std::string getMoreHelp() const;

	virtual void execute(Args &args, Environment &env);
};

#endif
