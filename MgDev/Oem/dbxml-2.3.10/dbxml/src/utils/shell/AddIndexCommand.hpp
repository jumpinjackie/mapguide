//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: AddIndexCommand.hpp,v 1.6 2006/10/30 17:46:07 bostic Exp $
//

#ifndef __ADDINDEXCOMMAND_HPP
#define	__ADDINDEXCOMMAND_HPP

#include "dbxml/DbXml.hpp"
#include <vector>
#include <string>

#include "ShellCommand.hpp"

class AddIndexCommand : public ShellCommand
{
public:
	AddIndexCommand() {};
	virtual ~AddIndexCommand() {};

	virtual std::string getCommandName() const;
	virtual std::string getCommandNameCompat() const;
	virtual std::string getBriefHelp() const;
	virtual std::string getMoreHelp() const;
	
	virtual void execute(Args &args, Environment &env);
};

#endif
