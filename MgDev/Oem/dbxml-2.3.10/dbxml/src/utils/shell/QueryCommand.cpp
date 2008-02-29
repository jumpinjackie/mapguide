//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: QueryCommand.cpp,v 1.12 2006/10/30 17:46:08 bostic Exp $
//

#include "QueryCommand.hpp"

#include <sstream>

using namespace DbXml;
using namespace std;

string QueryCommand::getCommandName() const
{
	return "query";
}

string QueryCommand::getBriefHelp() const
{
	return "Execute an expression in the context of the XmlManager";
}

string QueryCommand::getMoreHelp() const
{
	return
		string("Usage: query <queryExpression>\n") +
		string("This command uses the XmlManager::query() method.");
}

static string showErrorContext(const XmlException &e, const char *query)
{
	ostringstream msg;
	msg << e.what();
	if(e.getQueryLine() != 0 && e.getQueryFile() == 0) {
		msg << endl;
		int line = 1;
		while(line != e.getQueryLine() && *query != 0) {
			if(*query == '\n' ||
				(*query == '\r' && *(query + 1) != '\n'))
				++line;
			++query;
		}
		if(line == e.getQueryLine()) {
			while(*query != 0) {
				msg << (*query);
				if(*query == '\n' ||
					(*query == '\r' && *(query + 1) != '\n'))
					break;
				++query;
			}
			if(e.getQueryColumn() != 0) {
				if(*query == 0) msg << endl;
				msg << string(e.getQueryColumn() - 1, ' ') << "^";
			}
		}
	}
	return msg.str();
}

void QueryCommand::execute(Args &args, Environment &env)
{
	if(args.size() != 2) {
		throw CommandException("Wrong number of arguments");
	}
	XmlResults *newResults = 0;
	try {
		if(env.txn()) {
			newResults = new XmlResults(env.db().query(*env.txn(), args[1], env.context(), DBXML_LAZY_DOCS));
		} else {
			newResults = new XmlResults(env.db().query(args[1], env.context(), DBXML_LAZY_DOCS));
		}
	}
	catch(XmlException &e) {
		throw CommandException(showErrorContext(e, args[1].c_str()));
	}
	env.deleteResults();
	env.results() = newResults;
	if(env.context().getEvaluationType() == XmlQueryContext::Eager) {
		if(env.verbose())
			cout << env.results()->size()
			     << " objects returned for eager expression '"
			     << args[1] << "'" << endl << endl;
	} else {
		if(env.verbose())
			cout << "Lazy expression '" << args[1]
			     << "' completed" << endl << endl;
	}
}

