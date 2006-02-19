//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: GetDocumentsCommand.cpp,v 1.5 2005/10/13 11:09:55 jsnelson Exp $
//

#include "GetDocumentsCommand.hpp"

using namespace DbXml;
using namespace std;

string GetDocumentsCommand::getCommandName() const
{
	return "getDocuments";
}

string GetDocumentsCommand::getBriefHelp() const
{
	return "Gets document(s) by name from default container";
}

string GetDocumentsCommand::getMoreHelp() const
{
	return
		string("Usage: getDocuments [<docName>]\n") +
		string("If docName is set, it is looked up in the default container.\n") +
		string("If no arguments are used, all documents in the container are\n") +
		string("looked up, and placed in the results.\n") +
		string("The result document names and/or content can be displayed using\n") +
		string("the 'print' command.");
}

void GetDocumentsCommand::execute(Args &args, Environment &env)
{
	if(args.size() != 1 && args.size() != 2) {
		throw CommandException("Wrong number of arguments");
	}
	env.testContainer();
	env.deleteResults();

	XmlValue value;
	if(args.size() == 2) {
		value = args[1];
	}

	if(env.txn()) {
		env.results() = new XmlResults(
			env.container()->lookupIndex(
				*env.txn(),
				env.context(),
				DbXml::metaDataNamespace_uri,
				DbXml::metaDataName_name,
				"node-metadata-equality-string",
				value, DBXML_LAZY_DOCS));
	} else {
		env.results() = new XmlResults(
			env.container()->lookupIndex(
				env.context(),
				DbXml::metaDataNamespace_uri,
				DbXml::metaDataName_name,
				"node-metadata-equality-string",
				value, DBXML_LAZY_DOCS));
	}

	if(env.context().getEvaluationType() == XmlQueryContext::Eager) {
		if(env.verbose()) cout << env.results()->size() << " documents found"
				       << endl << endl;
	} else {
		if(env.verbose()) cout << "Lazy getDocuments completed" << endl << endl;
	}
}

