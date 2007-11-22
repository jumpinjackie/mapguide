//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: ShellCommand.hpp,v 1.8 2006/10/30 17:46:08 bostic Exp $
//

#ifndef __SHELLCOMMAND_HPP
#define	__SHELLCOMMAND_HPP

#include "dbxml/DbXml.hpp"
#include <vector>
#include <string>

#include "Environment.hpp"

class CommandException : public std::exception
{
public:
	CommandException(const std::string &problem) : what_(problem) {}
	virtual ~CommandException() throw() {}
	virtual const char* what() const throw() { return what_.c_str(); }
private:
	std::string what_;
};

class ShellCommand
{
public:
	typedef std::vector<std::string> Args;

	virtual ~ShellCommand() {};

	virtual std::string getCommandName() const = 0;
	virtual std::string getCommandNameCompat() const {
		return ""; //backward compat
	}
	virtual std::string getBriefHelp() const = 0;
	virtual std::string getMoreHelp() const = 0;

	static bool matches(const std::string &command,
			    const std::string &toMatch);
	virtual bool matches(const std::string &command) const;

	virtual void execute(Args &args, Environment &env) = 0;

	bool operator <(const ShellCommand *other) const;
	bool operator ==(const ShellCommand *other) const;

protected:
	ShellCommand() {};
};

#endif
