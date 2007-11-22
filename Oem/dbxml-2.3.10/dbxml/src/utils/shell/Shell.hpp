//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: Shell.hpp,v 1.9 2006/10/30 17:46:08 bostic Exp $
//

#ifndef __SHELL_HPP
#define	__SHELL_HPP

#include "dbxml/DbXml.hpp"
#include <vector>
#include <string>

#include "InputParser.hpp"

class ShellCommand;
class Environment;

class Shell
{
public:
	Shell();
	virtual ~Shell();
	
	/// Returns false if there were errors
	virtual bool mainLoop(std::istream &in, Environment &env) const;
	
	void addCommand(ShellCommand *command);
	void sortCommands();

	ShellCommand *findCommand(std::string &command) const;
private:
	Shell(const Shell &);
	Shell &operator=(const Shell &);

	std::vector<ShellCommand*> commands_;
};

#endif
