//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: SetBaseURICommand.hpp,v 1.6 2006/10/30 17:46:08 bostic Exp $
//

#ifndef __SETBASEURICOMMAND_HPP
#define	__SETBASEURICOMMAND_HPP

#include "ShellCommand.hpp"

class SetBaseURICommand : public ShellCommand
{
public:
	SetBaseURICommand() {};
	virtual ~SetBaseURICommand() {};
	
	virtual std::string getCommandName() const;
	virtual std::string getCommandNameCompat() const;
	virtual std::string getBriefHelp() const;
	virtual std::string getMoreHelp() const;
	
	virtual void execute(Args &args, Environment &env);
};

#endif
