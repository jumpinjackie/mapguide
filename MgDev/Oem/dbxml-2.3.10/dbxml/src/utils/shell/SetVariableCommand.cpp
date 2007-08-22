//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: SetVariableCommand.cpp,v 1.7 2006/10/30 17:46:08 bostic Exp $
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
		string("Usage: setVariable <varName> <value> (<value>...)\n") +
		string("This command uses XmlQueryContext::setVariableValue()\n") +
		string("to set a variable value (untyped).  Use setTypedVariable\n") +
		string("to set a typed variable.");
}

void SetVariableCommand::execute(Args &args, Environment &env)
{
	if(args.size() < 3) {
		throw CommandException("Wrong number of arguments");
	}
	
	if(env.verbose()) cout << "Setting $" << args[1] << " =";

	XmlResults results = env.db().createResults();

	for(unsigned int arg = 2; arg < args.size(); ++arg) {
		if(env.verbose()) cout << " " << args[arg];
		results.add(args[arg]);
	}

	if(env.verbose()) cout << endl;

	env.context().setVariableValue(args[1], results);
}

