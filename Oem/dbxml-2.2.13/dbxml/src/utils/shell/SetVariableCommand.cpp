//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: SetVariableCommand.cpp,v 1.4 2005/04/05 16:44:29 bostic Exp $
//

#include "SetVariableCommand.hpp"

using namespace DbXml;
using namespace std;

string SetVariableCommand::getCommandName() const
{
	return "setVariable";
}

string SetVariableCommand::getCommandNameCompat() const
{
	return "varset";
}

string SetVariableCommand::getBriefHelp() const
{
	return "Set a variable in the default context";
}

string SetVariableCommand::getMoreHelp() const
{
	return
		string("Usage: setVariable <varName> <value>\n") +
		string("This command uses XmlQueryContext::setVariableValue()\n") +
		string("to set a variable value (untyped).  Use setTypedVariable\n") +
		string("to set a typed variable.");
}

void SetVariableCommand::execute(Args &args, Environment &env)
{
	if(args.size() != 3) {
		throw CommandException("Wrong number of arguments");
	}
	
	if(env.verbose()) cout << "Setting $" << args[1] << " = " <<
		args[2] << endl;
	env.context().setVariableValue(args[1], args[2]);
}

