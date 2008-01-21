//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: CommentCommand.hpp,v 1.5 2006/10/30 17:46:07 bostic Exp $
//

#ifndef __COMMENTCOMMAND_HPP
#define	__COMMENTCOMMAND_HPP

#include "dbxml/DbXml.hpp"
#include <vector>
#include <string>

#include "ShellCommand.hpp"

class CommentCommand : public ShellCommand
{
public:
	CommentCommand() {};
	virtual ~CommentCommand() {};

  virtual std::string getCommandName() const;
  virtual std::string getBriefHelp() const;
  virtual std::string getMoreHelp() const;

  virtual void execute(Args &args, Environment &env);
};

#endif
