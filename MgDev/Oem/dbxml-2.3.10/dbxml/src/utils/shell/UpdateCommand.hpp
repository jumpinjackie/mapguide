//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: UpdateCommand.hpp,v 1.6 2006/10/30 17:46:09 bostic Exp $
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
