//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: SetMetaDataCommand.hpp,v 1.3 2006/10/30 17:46:08 bostic Exp $
//

#ifndef __SETMETADATACOMMAND_HPP
#define __SETMETADATACOMMAND_HPP

#include "ShellCommand.hpp"

class SetMetaDataCommand : public ShellCommand
{
public:
	SetMetaDataCommand() {};
	virtual ~SetMetaDataCommand() {};

	virtual std::string getCommandName() const;
	virtual std::string getBriefHelp() const;
	virtual std::string getMoreHelp() const;
	
	virtual void execute(Args &args, Environment &env);
};

#endif
