//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: InsertAfterCommand.hpp,v 1.5 2006/10/30 17:46:08 bostic Exp $
//

#ifndef __INSERTAFTERCOMMAND_HPP
#define	__INSERTAFTERCOMMAND_HPP

#include "AddContentCommand.hpp"

class InsertAfterCommand : public AddContentCommand
{
public:
  InsertAfterCommand() {};
	virtual ~InsertAfterCommand() {};

  virtual std::string getCommandName() const;
  virtual std::string getBriefHelp() const;
  virtual std::string getMoreHelp() const;

  virtual void execute(Args &args, Environment &env);
};

#endif
