/*
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 1998,2006 Oracle.  All rights reserved.
 *
 * $Id: xqts_runner.cpp,v 1.20.2.1 2006/12/08 16:42:43 gmf Exp $
 */

#include <iostream>
#include <sstream>
#include <cstdio>
#include <map>
#include <dbxml/DbXml.hpp>
#ifdef HAVE_GETOPT
#include <unistd.h>
#else
extern "C" int getopt(int argc, char * const argv[], const char *optstring);
extern "C" char *optarg;
extern "C" int optind;
#endif

#include <xqilla/xqts/TestSuiteParser.hpp>
#include <xqilla/xqts/TestSuiteResultListener.hpp>
#include <xqilla/xqts/TestSuiteRunner.hpp>
#include <xqilla/utils/XQillaPlatformUtils.hpp>
#include <xqilla/utils/XStr.hpp>

#include <xercesc/util/Janitor.hpp>
#include <xercesc/framework/URLInputSource.hpp>
#include <xercesc/util/BinFileInputStream.hpp>

using namespace DbXml;
using namespace std;

#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

static const string TEST_URI_SCHEME = "foo://";

class DbXmlTestSuiteRunner : public TestSuiteRunner, private XmlResolver
{
public:
	DbXmlTestSuiteRunner(const XmlContainer &container, bool verbose,
		const string &singleTest, TestSuiteResultListener *results);

	virtual void addSource(const string &id, const string &filename, const string &schema);
	virtual void addSchema(const string &id, const string &filename, const string &uri);
	virtual void addModule(const string &id, const string &filename);
	virtual void addCollection(const string &id, const string &filename);

	virtual void startTestGroup(const string &name);
	virtual void endTestGroup();

	virtual void runTestCase(const TestCase &testCase);

private:
	virtual XmlInputStream *resolveSchema(XmlTransaction *txn, XmlManager &mgr,
		const std::string &schemaLocation, const std::string &nameSpace) const;
	virtual bool resolveModuleLocation(XmlTransaction *txn, XmlManager &mgr,
		const std::string &nameSpace, XmlResults &result) const;
	virtual XmlInputStream *resolveModule(XmlTransaction *txn, XmlManager &mgr,
		const std::string &moduleLocation, const std::string &nameSpace) const;
	virtual bool resolveCollection(XmlTransaction *txn, XmlManager &mgr,
		const std::string &uri, XmlResults &result) const;
	virtual bool resolveDocument(XmlTransaction *txn, XmlManager &mgr,
		const std::string &uri, XmlValue &result) const;

private:
	mutable DbXml::XmlContainer container_;
	bool verbose_;

	string singleTest_;
	string fullTestName_;
	const TestCase* testCase_;

	// id -> filename
	map<string, string> moduleFiles_;
	// id -> list of inputFiles ID
	map<string, list<string> > collections_;
};

static void usage(const string &progname, int exitCode)
{
	string::size_type pos = progname.rfind('/');
	if(pos == string::npos) {
		pos = progname.rfind('\\');
	}
	if(pos == string::npos) {
		pos = 0;
	}
	else {
		++pos;
	}

	cerr << "Usage: " << progname.substr(pos) << " [OPTIONS] <path_to_test_suite> (<specific test>)?" << endl;
	cerr << endl;
	cerr << "Options:" << endl;
	cerr << "-C <cachesize>\tset the cache size to the specified value, in Mb. (default: 64)" << endl;
	cerr << "-c <container>\tuse the specified container name. (default: xqts.dbxml)" << endl;
	cerr << "-e <file>\tuse the given file as a known error file" << endl;
	cerr << "-E <file>\toutput an error file" << endl;
	cerr << "-h <home>\tuse specified directory as a Berkeley DB environment. (default: .)" << endl;
	cerr << "-i\t\tuse indexing" << endl;
	cerr << "-s <storage>\tuse the specified storage model, including DLS, DLS+, NLS and NLS+. (default: NLS)" << endl;
	cerr << "-V\t\tprint software version" << endl;
	cerr << "-v\t\tverbose. Using the switch a second time increases verbosity" << endl;
	cerr << "-x\t\toutput results as XML" << endl;
	cerr << "-?\t\thelp -- print this message" << endl;
	cerr << endl;
	exit(exitCode);
}

int
main(int argc, char **argv)
{
	const char *progName = argv[0];

	int verbose = 0;
	int cacheSize = 64 * 1024 * 1024;
	bool nodeStorage = true;
	bool nodeIndexes = false;
	string envDir = ".";
	string containerName = "xqts.dbxml";
	string errorFile;
	string outputErrorFile;
	bool xmlResults = false;
	bool useIndexing = false;

	int ch;
	while ((ch = getopt(argc, argv, "?vVh:s:C:c:E:e:xi")) != -1) {
		switch (ch) {
		case 'C': {
			cacheSize = atoi(optarg) * 1024 * 1024;
			break;
		}
		case 'v': {
			++verbose;
			break;
		}
		case 'V': {
			printf("%s\n", DbXml::dbxml_version(NULL, NULL, NULL));
			printf("%s\n", DbEnv::version(NULL, NULL, NULL));
			exit(0);
		}
		case 's': {
			string storage = optarg;
			if(storage == "DLS") {
				nodeStorage = false;
				nodeIndexes = false;
			}
			else if(storage == "DLS+") {
				nodeStorage = false;
				nodeIndexes = true;
			}
			else if(storage == "NLS") {
				nodeStorage = true;
				nodeIndexes = false;
			}
			else if(storage == "NLS+") {
				nodeStorage = true;
				nodeIndexes = true;
			}
			else {
				printf("Unknown storage type: %s\n", storage.c_str());
				usage(progName, 1);
			}
			break;
		}
		case 'h': {
			envDir = optarg;
			break;
		}
		case 'c': {
			containerName = optarg;
			break;
		}
		case 'e': {
			errorFile = optarg;
			break;
		}
		case 'E': {
			outputErrorFile = optarg;
			break;
		}
		case 'x': {
			xmlResults = true;
			break;
		}
		case 'i': {
			useIndexing = true;
			break;
		}
		case '?': {
			usage(progName, 0);
			break;
		}
		default: {
			usage(progName, 1);
			break;
		}
		}
	}

	if(argc - optind > 2) {
		printf("Too many arguments\n");
		usage(progName, 1);
	}

	if(argc == optind) {
		printf("Path to the XQuery Test Suite not specified\n");
		usage(progName, 1);
	}

	string testSuiteHome = string(argv[optind]) + "/";
	string singleTest;
	if(argc - optind > 1)
		singleTest = argv[optind + 1];

	// Turn on logging if extra verbose is specified
	if(verbose > 1) {
		setLogLevel(LEVEL_ALL, true);
		setLogCategory(CATEGORY_ALL, true);
		setLogCategory(CATEGORY_NODESTORE, verbose > 2);
	}

	bool passed = true;
	try {
		DbEnv *dbenv = new DbEnv(0);
		dbenv->set_error_stream(&cerr);
		dbenv->set_cachesize(0, cacheSize, 1);
		dbenv->set_lk_max_lockers(50000);
		dbenv->set_lk_max_locks(50000);
		dbenv->set_lk_max_objects(50000);
		dbenv->open(envDir.c_str(), DB_CREATE|DB_INIT_MPOOL, 0);

		// Set the numeric precision to the specified minimum -
		// this will make more tests pass unaltered, since the
		// results are formatted using the minimum precision.
		XQillaPlatformUtils::enableExtendedPrecision(false);

		XmlManager mgr(dbenv, DBXML_ALLOW_EXTERNAL_ACCESS|DBXML_ADOPT_DBENV);

		int cflags = DB_CREATE;
		if(nodeIndexes) cflags |= DBXML_INDEX_NODES;
		XmlContainer cont = mgr.openContainer(containerName,
			cflags, (nodeStorage ? XmlContainer::NodeContainer :
				XmlContainer::WholedocContainer), 0);
		cont.addAlias("xqts.dbxml");

		if(useIndexing) {
			XmlUpdateContext uc = mgr.createUpdateContext();
			cont.addDefaultIndex("node-element-equality-string", uc);
			cont.addDefaultIndex("node-attribute-equality-string", uc);
			cont.addDefaultIndex("node-element-equality-double", uc);
			cont.addDefaultIndex("node-attribute-equality-double", uc);
			cont.addDefaultIndex("edge-element-presence", uc);
			cont.addDefaultIndex("edge-attribute-presence", uc);
			cont.addIndex("", "description", "node-element-substring-string", uc);
			cont.addIndex("", "title", "node-element-substring-string", uc);
		}

		Janitor<TestSuiteResultListener> results(0);
		if(xmlResults) {
			results.reset(new XMLReportResultListener());
			XMLReportResultListener *xmlreport = (XMLReportResultListener*)results.get();
			xmlreport->setImplementation("Berkeley DB XML", DbXml::dbxml_version(NULL, NULL, NULL));
			xmlreport->setOrganization("Oracle", "http://www.sleepycat.com");
			xmlreport->setSubmittor("John Snelson", "john.snelson@oracle.com");
			xmlreport->setAnonymous(true);

			xmlreport->addImplementationDefinedItem("expressionUnicode", "UTF-8");
			xmlreport->addImplementationDefinedItem("implicitTimezone", "Defined by the system clock");
			xmlreport->addImplementationDefinedItem("XMLVersion", "1.0");
			xmlreport->addImplementationDefinedItem("axes", "Full axis support");
			xmlreport->addImplementationDefinedItem("defaultOrderEmpty", "empty least");
			xmlreport->addImplementationDefinedItem("normalizationForms", "NFC, NFD, NFKC, NFKD");
			xmlreport->addImplementationDefinedItem("docProcessing", "dtd validation");

			xmlreport->addFeature("Minimal Conformance", true);
			xmlreport->addFeature("Schema Import", true);
			xmlreport->addFeature("Schema Validation", false);
			xmlreport->addFeature("Static Typing", false);
			xmlreport->addFeature("Static Typing Extensions", false);
			xmlreport->addFeature("Full Axis", true);
			xmlreport->addFeature("Module", true);
			xmlreport->addFeature("Serialization", false);
			xmlreport->addFeature("Trivial XML Embedding", false);
		}
		else {
			results.reset(new ConsoleResultListener());
		}

		KnownErrorChecker knownErrors(results.get());
		if(errorFile != "" && !knownErrors.loadErrors(errorFile)) {
			return 1;
		}

		DbXmlTestSuiteRunner runner(cont, verbose, singleTest, &knownErrors);
		TestSuiteParser parser(testSuiteHome, &runner);

		parser.run();

		if(xmlResults) {
			((XMLReportResultListener*)results.get())->printReport();
		}
		else {
			passed = ((ConsoleResultListener*)results.get())->printReport();
		}

		if(errorFile != "") {
			passed = knownErrors.printReport();
		}

		if(outputErrorFile != "" && !knownErrors.saveErrors(outputErrorFile)) {
			cout << "Unable to open error file: " << outputErrorFile << endl;
			return 1;
		}

	} catch (XmlException &xe) {
		cout << "XmlException: " << xe.what() << endl;
		return 1;
	} catch (DbException &de) {
		cout << "DbException: " << de.what() << endl;
		return 1;
	} catch(...) {
		cout << "Unknown exception" << endl;
		return 1;
	}

	return passed ? 0 : 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

DbXmlTestSuiteRunner::DbXmlTestSuiteRunner(const XmlContainer &container, bool verbose,
	const string &singleTest, TestSuiteResultListener *results)
	: TestSuiteRunner(results),
	  container_(container),
	  verbose_(verbose),
	  singleTest_(singleTest),
	  testCase_(0)
{
	container_.getManager().registerResolver(*this);
}

void DbXmlTestSuiteRunner::startTestGroup(const string &name)
{
	if(fullTestName_ != "")
		fullTestName_ += ":";
	fullTestName_ += name;

	m_results->startTestGroup(name);
}

void DbXmlTestSuiteRunner::endTestGroup()
{
	int nColonPos = fullTestName_.find_last_of(":");
	if(nColonPos != -1)
		fullTestName_ = string(fullTestName_.c_str(), nColonPos);
	else fullTestName_ = "";

	m_results->endTestGroup();
}

void DbXmlTestSuiteRunner::addSource(const string &id, const string &filename, const string &schema)
{
	XmlManager mgr = container_.getManager();
	XmlDocument doc = mgr.createDocument();
	doc.setName(id);
	doc.setContentAsXmlInputStream(mgr.createURLInputStream("", filename));
	doc.setMetaData("", "schema", XmlValue(schema));

	if(verbose_) {
		cout << "Adding source file: " << id << endl;
	}
	XmlUpdateContext uc = mgr.createUpdateContext();
	try {
		container_.putDocument(doc, uc);
	}
	catch(XmlException &xe) {
		if(xe.getExceptionCode() == XmlException::UNIQUE_ERROR) {
			// It's already in there
		}
		else throw;
	}
}

void DbXmlTestSuiteRunner::addSchema(const string &id, const string &filename, const string &uri)
{
	XmlManager mgr = container_.getManager();
	XmlDocument doc = mgr.createDocument();
	doc.setName(id);
	doc.setContentAsXmlInputStream(mgr.createURLInputStream("", filename));
	doc.setMetaData("", "uri", XmlValue(uri));

	if(verbose_) {
		cout << "Adding schema file: " << id << endl;
	}
	XmlUpdateContext uc = mgr.createUpdateContext();
	try {
		container_.putDocument(doc, uc);
	}
	catch(XmlException &xe) {
		if(xe.getExceptionCode() == XmlException::UNIQUE_ERROR) {
			// It's already in there
		}
		else throw;
	}
}

void DbXmlTestSuiteRunner::addModule(const string &id, const string &filename)
{
  moduleFiles_[id] = filename;
}

void DbXmlTestSuiteRunner::addCollection(const string &id, const string &filename)
{
  collections_[TEST_URI_SCHEME + id].push_back(filename);
}

class DocumentInputStream : public XmlInputStream
{
public:
	DocumentInputStream(const XmlDocument &doc)
		: doc_(doc), stream_(doc.getContentAsXmlInputStream()) {}
	virtual ~DocumentInputStream() { delete stream_; }

	virtual unsigned int curPos () const { return stream_->curPos(); }
	virtual unsigned int readBytes (char *toFill, 
		const unsigned int maxToRead) { return stream_->readBytes(toFill, maxToRead); }
private:
	XmlDocument doc_;
	XmlInputStream *stream_;
};

XmlInputStream *DbXmlTestSuiteRunner::resolveSchema(XmlTransaction *txn, XmlManager &mgr,
	const std::string &schemaLocation, const std::string &nameSpace) const
{
	// Resolve the schema to the one stored in our container
	XmlQueryContext qc = mgr.createQueryContext();
	XmlResults results = mgr.query(string("collection(\"xqts.dbxml\")[dbxml:metadata(\"uri\") = \"") +
		nameSpace + string("\"]"), qc);

	XmlValue val;
	if(results.next(val)) {
		return new DocumentInputStream(val.asDocument());
	}

	return 0;
}

bool DbXmlTestSuiteRunner::resolveModuleLocation(XmlTransaction *txn, XmlManager &mgr,
	const std::string &nameSpace, XmlResults &result) const
{
	bool ret = false;
	list<std::pair<string, string> >::const_iterator i;
	for(i = testCase_->moduleFiles.begin(); i != testCase_->moduleFiles.end(); ++i) {
		if(i->first == nameSpace) {
			result.add(i->second);
			ret = true;
		}
	}
	return ret;
}

XmlInputStream *DbXmlTestSuiteRunner::resolveModule(XmlTransaction *txn, XmlManager &mgr,
	const std::string &moduleLocation, const std::string &nameSpace) const
{
	std::list<std::pair<std::string, std::string> >::const_iterator i;
	for(i = testCase_->moduleFiles.begin();
	    i != testCase_->moduleFiles.end(); ++i) {
		if(i->first == nameSpace && i->second == moduleLocation) {
			map<string, string>::const_iterator i2 =
				moduleFiles_.find(i->second);
			if(i2 != moduleFiles_.end()) {
				return mgr.createURLInputStream("", i2->second + ".xq");
			}
		}
	}

	return 0;
}

bool DbXmlTestSuiteRunner::resolveCollection(XmlTransaction *txn, XmlManager &mgr,
	const std::string &uri, XmlResults &result) const
{
	std::map<std::string, std::list<std::string> >::const_iterator it = collections_.find(uri);
	if(it != collections_.end()) {
		for(std::list<std::string>::const_iterator s = it->second.begin();
		    s != it->second.end(); ++s) {
			if(txn == 0)
				result.add(container_.getDocument(*s, DBXML_LAZY_DOCS));
			else result.add(container_.getDocument(*txn, *s, DBXML_LAZY_DOCS));
		}
		return true;
	}
	return false;
}

bool DbXmlTestSuiteRunner::resolveDocument(XmlTransaction *txn, XmlManager &mgr,
	const std::string &uri, XmlValue &result) const
{
	if(uri.find(TEST_URI_SCHEME) == 0) {
		string docname = uri.substr(TEST_URI_SCHEME.length());
		if(txn == 0)
			result = container_.getDocument(docname, DBXML_LAZY_DOCS);
		else result = container_.getDocument(*txn, docname, DBXML_LAZY_DOCS);
		return true;
	}
	return false;
}

void characterEscape(const string &input, ostringstream &dest)
{
	for(string::const_iterator i = input.begin();
	    i != input.end(); ++i) {
		if(*i == '>') {
			dest << "&gt;";
		}
		else if(*i == '<') {
			dest << "&lt;";
		}
		else if(*i == '&') {
			dest << "&amp;";
		}
		else {
			dest << *i;
		}
	}
}

string findAndReplace(const string &input, const string &find, const string &replace)
{
	string result = input;

	string::size_type search_from = 0;
	string::size_type pos = 0;
	while((pos = result.find(find, search_from)) != string::npos) {
		// Only perform the substitution if this is the complete lexical token
		// (checked in a hacky, but adequate way).
		string::size_type pos_end = pos + find.length();
		if(result.size() > pos_end) {
			if((result[pos_end] >= '0' && result[pos_end] <= '9') ||
				(result[pos_end] >= 'A' && result[pos_end] <= 'Z') ||
				(result[pos_end] >= 'a' && result[pos_end] <= 'z')) {
				search_from = pos + 1;
				continue;
			}
		}
				
		result.replace(pos, find.size(), replace);
	}

	return result;
}

void DbXmlTestSuiteRunner::runTestCase(const TestCase &testCase)
{
	if(singleTest_ != "" &&
		testCase.name.find(singleTest_) == string::npos &&
		fullTestName_.find(singleTest_) == string::npos) {
		m_results->reportSkip(testCase, "Not run");
		return;
	}

	if(fullTestName_.find("Optional:StaticTyping") != string::npos) {
		m_results->reportSkip(testCase, "Static typing not supported");
		return;
	}
	if(fullTestName_.find("Optional:SchemaValidation") != string::npos) {
		m_results->reportSkip(testCase, "Schema validation not supported");
		return;
	}
	if(fullTestName_.find("Optional:TrivialEmbedding") != string::npos) {
		m_results->reportSkip(testCase, "Trivial embedding not supported");
		return;
	}

	testCase_ = &testCase;

	try {
		XmlManager mgr = container_.getManager();

		string &query = const_cast<string&>(testCase.query);

		XmlQueryContext qc = mgr.createQueryContext();
		qc.setBaseURI("");

		// Remove the variable declarations
		map<string, string>::const_iterator v;
		for(v = testCase.inputVars.begin(); v != testCase.inputVars.end(); ++v) {
			if(v->first != "local:var") {
				query = findAndReplace(query,
					string("declare variable $") + v->first + " external;",
					"");
			}
		}
		// Replace the variables with the fn:doc() function
		for(v = testCase.inputVars.begin(); v != testCase.inputVars.end(); ++v) {
			if(v->first == "local:var") {
				// Do local:var as a variable
				qc.setVariableValue(v->first, container_.getDocument(v->second, DBXML_LAZY_DOCS));
			}
			else {
				query = findAndReplace(query,
					string("$") + v->first,
					string("fn:doc(\"dbxml:/xqts.dbxml/") + v->second + "\")");
			}
		}

		// Execute the auxilliary queries, and set up the external variable values
		for(v = testCase.extraVars.begin(); v != testCase.extraVars.end(); ++v) {
			Janitor<BinFileInputStream> stream((BinFileInputStream*)URLInputSource(0, X(v->second.c_str())).makeStream());
			unsigned int dwSize = stream->getSize();
			string extraQuery;
			extraQuery.resize(dwSize);
			stream->readBytes((XMLByte*)extraQuery.c_str(), dwSize);

			XmlResults value = mgr.query(extraQuery, qc);
			qc.setVariableValue(v->first, value);
		}

		// Set the URI variables
		for(v = testCase.inputURIVars.begin(); v != testCase.inputURIVars.end(); ++v) {
			qc.setVariableValue(v->first, TEST_URI_SCHEME + v->second);
		}

		// Set up the context item
		XmlDocument contextDocument;
		if(!testCase.contextItem.empty()) {
			contextDocument =
				container_.getDocument(testCase.contextItem, DBXML_LAZY_DOCS);
		}

		// Set up the default collection
		if(!testCase.defaultCollection.empty()) {
			qc.setDefaultCollection(TEST_URI_SCHEME + testCase.defaultCollection);
		}

		XmlQueryExpression expr = mgr.prepare(query, qc);
		XmlResults results;
		if(contextDocument.isNull())
			results = expr.execute(qc);
		else results = expr.execute(contextDocument, qc);

		ostringstream output;
		bool previousWasNode = true;
		XmlValue val;
		while(results.next(val)) {
			if(!previousWasNode && !val.isNode())
				output << " ";
			if(val.isNode()) {
				string nodeValue = val.asString();

				// Remove the xml decl from a document node
				string::size_type start = nodeValue.find("<?xml");
				if(start == 0) {
					string::size_type end = nodeValue.find("?>\n");
					if(end == string::npos) {
						end = nodeValue.find("?>\r\n");
						if(end == string::npos) {
							end = nodeValue.find("?>\r");
							if(end == string::npos) {
								end = nodeValue.find("?>");
								if(end != string::npos)
									end += 2;
							}
							else end += 3;
						} else end += 4;
					} else end += 3;
					if(end != string::npos) {
						nodeValue.erase(start, end - start);
					}
				}

				output << nodeValue;
			}
			else characterEscape(val.asString(), output);

			previousWasNode = val.isNode();
		}

		testResults(testCase, output.str());
	}
	catch(XmlException &xe) {
		testErrors(testCase, xe.what());
	}
	catch(...) {
		testErrors(testCase, "[Unknown exception]");
	}

	testCase_ = 0;
}
