//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: ReindexCommand.hpp,v 1.1 2005/08/31 17:14:33 gmf Exp $
//

#ifndef __REINDEXCOMMAND_HPP
#define	__REINDEXCOMMAND_HPP

#include "ShellCommand.hpp"

class ReindexCommand : public ShellCommand
{
public:
	ReindexCommand() {};
	virtual ~ReindexCommand() {};

	virtual std::string getCommandName() const;
	virtual std::string getBriefHelp() const;
	virtual std::string getMoreHelp() const;
	
	virtual void execute(Args &args, Environment &env);
};

#endif
