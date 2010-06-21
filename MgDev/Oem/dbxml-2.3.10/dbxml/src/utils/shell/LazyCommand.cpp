//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: LazyCommand.cpp,v 1.6 2006/10/30 17:46:08 bostic Exp $
//

#include "LazyCommand.hpp"

using namespace DbXml;
using namespace std;

string LazyCommand::getCommandName() const
{
	return "setLazy";
}

string LazyCommand::getCommandNameCompat() const
{
	return "lazy";
}

string LazyCommand::getBriefHelp() const
{
	return "Sets lazy evaluation on or off in the default context";
}

string LazyCommand::getMoreHelp() const
{
	return
		string("Usage: setLazy <on | off>\n") +
		string("This command calls the XmlQueryContext::setEvaluationType() method.");
}

void LazyCommand::execute(Args &args, Environment &env)
{
	if(args.size() != 2) {
		throw CommandException("Wrong number of arguments");
	}
	
	if(args[1] == "on") {
		env.context().setEvaluationType(XmlQueryContext::Lazy);
		if(env.verbose()) cout << "Lazy evaluation on" << endl;
	} else {
		env.context().setEvaluationType(XmlQueryContext::Eager);
		if(env.verbose()) cout << "Lazy evaluation off" << endl;
	}
}

