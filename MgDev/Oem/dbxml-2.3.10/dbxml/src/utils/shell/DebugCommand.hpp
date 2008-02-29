//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: DebugCommand.hpp,v 1.5 2006/10/30 17:46:07 bostic Exp $
//

#ifndef __DEBUGCOMMAND_HPP
#define	__DEBUGCOMMAND_HPP

#include "ShellCommand.hpp"

class DebugCommand : public ShellCommand
{
public:
  DebugCommand() {};
	virtual ~DebugCommand() {};

  virtual std::string getCommandName() const;
  virtual std::string getBriefHelp() const;
  virtual std::string getMoreHelp() const;

  virtual void execute(Args &args, Environment &env);
};

#endif
