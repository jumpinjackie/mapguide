//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: EdgeIndexLookupCommand.hpp,v 1.3 2006/10/30 17:46:07 bostic Exp $
//

#ifndef __EDGEINDEXLOOKUPCOMMAND_HPP
#define	__EDGEINDEXLOOKUPCOMMAND_HPP

#include "ShellCommand.hpp"

class EdgeIndexLookupCommand : public ShellCommand
{
public:
	EdgeIndexLookupCommand() {};
	virtual ~EdgeIndexLookupCommand() {};

	virtual std::string getCommandName() const;
	virtual std::string getBriefHelp() const;
	virtual std::string getMoreHelp() const;

	virtual void execute(Args &args, Environment &env);
};

#endif
