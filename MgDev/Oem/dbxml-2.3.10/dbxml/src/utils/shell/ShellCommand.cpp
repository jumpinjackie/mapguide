//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: ShellCommand.cpp,v 1.9 2006/10/30 17:46:08 bostic Exp $
//

#include "ShellCommand.hpp"
#include <cctype>

using namespace DbXml;
using namespace std;

static void lowerCase(string &s)
{
	for (unsigned int i = 0; i < s.length(); ++i)
		s[i] = tolower(s[i]);
}

bool ShellCommand::matches(const string &command, const string &toMatch)
{
	// Be case-insensitive, for convenience
	if (command.length() == 0)
		return false;
	string l_command = command;
	string l_match = toMatch;
	lowerCase(l_command);
	lowerCase(l_match);
	
	return l_match.substr(0, l_command.length()) == l_command;
}

bool ShellCommand::matches(const string &command) const
{
	return (matches(command, getCommandName()) ||
		matches(command, getCommandNameCompat()));
}

bool ShellCommand::operator <(const ShellCommand *other) const
{
	return (getCommandName() < other->getCommandName());
}

bool ShellCommand::operator ==(const ShellCommand *other) const
{
	return (getCommandName() == other->getCommandName());
}
