//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: SetBaseURICommand.cpp,v 1.6 2006/10/30 17:46:08 bostic Exp $
//

#include "SetBaseURICommand.hpp"

using namespace DbXml;
using namespace std;

string SetBaseURICommand::getCommandName() const
{
	return "setBaseUri";
}

string SetBaseURICommand::getCommandNameCompat() const
{
	return "baseuriset";
}

string SetBaseURICommand::getBriefHelp() const
{
	return "Set the base uri in the default context";
}

string SetBaseURICommand::getMoreHelp() const
{
	return
		string("Usage: setBaseUri <uri>\n") +
		string("The base uri must be of the form, \"scheme:path\".\n") +
		string("This command calls the XmlQueryContext::setBaseURI() method.");
}

void SetBaseURICommand::execute(Args &args, Environment &env)
{
	if(args.size() != 2) {
		throw CommandException("Wrong number of arguments");
	}
	
	if(env.verbose()) cout << "Base URI = " << args[1] << endl;
	env.context().setBaseURI(args[1]);
}

