//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: ReturnTypeCommand.cpp,v 1.8 2006/10/30 17:46:08 bostic Exp $
//

#include "ReturnTypeCommand.hpp"

using namespace DbXml;
using namespace std;

string ReturnTypeCommand::getCommandName() const
{
	return "setReturnType";
}

string ReturnTypeCommand::getCommandNameCompat() const
{
	return "returntype";
}

string ReturnTypeCommand::getBriefHelp() const
{
	return "Sets the return type on the default context";
}

string ReturnTypeCommand::getMoreHelp() const
{
	return
		string("Usage: setReturnType <dv|lv>\n") +
		string("This command calls XmlQueryContext::setReturnType() to set\n") +
		string("the ReturnType to DeadValues (dv) or LiveValues(lv).\n") +
		string("The system default is LiveValues");
}

void ReturnTypeCommand::execute(Args &args, Environment &env)
{
	if(args.size() != 2) {
		throw CommandException("Wrong number of arguments");
	}

	string type;
	XmlQueryContext::ReturnType t = (XmlQueryContext::ReturnType)-1;

	if(args[1] == "dv") {
		t = XmlQueryContext::DeadValues;
		type = "DeadValues";
	} else if(args[1] == "lv") {
		t = XmlQueryContext::LiveValues;
		type = "LiveValues";
	}

	if(t == (XmlQueryContext::ReturnType)-1) {
		throw CommandException("Unknown return type: " + args[1]);
	}

	if(env.verbose()) cout << "Setting return type to: " << type << endl;
	env.context().setReturnType(t);
}

