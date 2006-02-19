//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: AppendCommand.hpp,v 1.3 2005/04/05 16:44:27 bostic Exp $
//

#ifndef __APPENDCOMMAND_HPP
#define	__APPENDCOMMAND_HPP

#include "AddContentCommand.hpp"

class AppendCommand : public AddContentCommand
{
public:
  AppendCommand() {};
	virtual ~AppendCommand() {};

  virtual std::string getCommandName() const;
  virtual std::string getBriefHelp() const;
  virtual std::string getMoreHelp() const;

  virtual void execute(Args &args, Environment &env);
};

#endif
