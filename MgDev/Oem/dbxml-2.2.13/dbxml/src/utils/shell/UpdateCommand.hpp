//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: UpdateCommand.hpp,v 1.4 2005/04/05 16:44:29 bostic Exp $
//

#ifndef __UPDATECOMMAND_HPP
#define	__UPDATECOMMAND_HPP

#include "dbxml/DbXml.hpp"
#include <vector>
#include <string>

#include "ShellCommand.hpp"

class UpdateCommand : public ShellCommand
{
public:
	UpdateCommand() {};
	virtual ~UpdateCommand() {};

	virtual std::string getCommandName() const;
	virtual std::string getCommandNameCompat() const;
	virtual std::string getBriefHelp() const;
	virtual std::string getMoreHelp() const;
	
	virtual void execute(Args &args, Environment &env);
};

#endif
