//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: QueryCommand.hpp,v 1.5 2006/10/30 17:46:08 bostic Exp $
//

#ifndef __QUERYCOMMAND_HPP
#define	__QUERYCOMMAND_HPP

#include "dbxml/DbXml.hpp"
#include <vector>
#include <string>

#include "ShellCommand.hpp"

class QueryCommand : public ShellCommand
{
public:
	QueryCommand() {};
	virtual ~QueryCommand() {};

  virtual std::string getCommandName() const;
  virtual std::string getBriefHelp() const;
  virtual std::string getMoreHelp() const;

  virtual void execute(Args &args, Environment &env);
};

#endif
