//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: StartTransactionCommand.hpp,v 1.5 2006/10/30 17:46:08 bostic Exp $
//

#ifndef __STARTTRANSACTIONCOMMAND_HPP
#define	__STARTTRANSACTIONCOMMAND_HPP

#include "ShellCommand.hpp"

class StartTransactionCommand : public ShellCommand
{
public:
	StartTransactionCommand() {};
	virtual ~StartTransactionCommand() {};

	virtual std::string getCommandName() const;
	virtual std::string getCommandNameCompat() const;
	virtual std::string getBriefHelp() const;
	virtual std::string getMoreHelp() const;

	virtual void execute(Args &args, Environment &env);
};

#endif
