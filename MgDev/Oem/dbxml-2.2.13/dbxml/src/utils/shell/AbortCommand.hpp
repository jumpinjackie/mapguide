//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: AbortCommand.hpp,v 1.3 2005/04/05 16:44:26 bostic Exp $
//

#ifndef __ABORTCOMMAND_HPP
#define	__ABORTCOMMAND_HPP

#include "dbxml/DbXml.hpp"
#include <vector>
#include <string>

#include "ShellCommand.hpp"

class AbortCommand : public ShellCommand
{
public:
	AbortCommand() {};
	virtual ~AbortCommand() {};

  virtual std::string getCommandName() const;
  virtual std::string getBriefHelp() const;
  virtual std::string getMoreHelp() const;

  virtual void execute(Args &args, Environment &env);
};

#endif
