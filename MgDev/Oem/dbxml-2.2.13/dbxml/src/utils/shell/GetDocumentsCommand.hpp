//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: GetDocumentsCommand.hpp,v 1.2 2005/04/05 16:44:28 bostic Exp $
//

#ifndef __GETDOCUMENTSCOMMAND_HPP
#define	__GETDOCUMENTSCOMMAND_HPP

#include "ShellCommand.hpp"

class GetDocumentsCommand : public ShellCommand
{
public:
	GetDocumentsCommand() {};
	virtual ~GetDocumentsCommand() {};

	virtual std::string getCommandName() const;
	virtual std::string getBriefHelp() const;
	virtual std::string getMoreHelp() const;

	virtual void execute(Args &args, Environment &env);
};

#endif
