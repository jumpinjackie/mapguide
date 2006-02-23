//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: QueryPlanCommand.hpp,v 1.4 2005/04/05 16:44:29 bostic Exp $
//

#ifndef __QUERYPLANCOMMAND_HPP
#define	__QUERYPLANCOMMAND_HPP

#include "ShellCommand.hpp"

class QueryPlanCommand : public ShellCommand
{
public:
	QueryPlanCommand() {};
	virtual ~QueryPlanCommand() {};
	
	virtual std::string getCommandName() const;
	virtual std::string getCommandNameCompat() const;
	virtual std::string getBriefHelp() const;
	virtual std::string getMoreHelp() const;
	
	virtual void execute(Args &args, Environment &env);
};

#endif
