//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: XQueryTest.cpp,v 1.25 2005/09/06 10:42:24 jsnelson Exp $
//

// tests the execution of XQuery 1.0 expressions
//
//

// DB XML includes (external)
#include "dbxml/DbXml.hpp"

// System includes
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <map>
#include <vector>
#include <set>
#include <cassert>

// Xerces includes

// Pathan includes
#include <pathan/XPath2Result.hpp>
#include <pathan/internal/items/impl/NodeImpl.hpp>
#include <xquery/XQContext.hpp>

// test code includes
#include "../util/TestLogging.hpp"
#include "../util/Transcoding.hpp"
#include "TestEnvironment.hpp"
#include "XQueryTest.hpp"

using namespace DbXml;
using namespace DbXmlTest;
using namespace std;
XERCES_CPP_NAMESPACE_USE

XQueryTest::XQueryTest(TestEnvironment &env, const Iterator<std::string> &args)
	: UnitTest(env, "XQueryTest")
{
}

XQueryTest::~XQueryTest()
{
}

void XQueryTest::execute()
{
	TestLogger *log = env_.log();

	// outer loop is over individual tests
	Sequence result = env_.query("/test/base");
	for (Sequence::iterator it = result.begin();
	     it != result.end(); it++) {
		if((*it)->isNode()) {
			// get query (name of file, in the data directory)
			const NodeImpl *nodeImpl = (const NodeImpl*)(*it)->getInterface(Item::gPathan);
			assert(nodeImpl != 0);

			Sequence result2 =
				env_.query("query", (DOMNode*)
					   nodeImpl->getDOMNode());
			for (Sequence::iterator it2 = result2.begin();
			     it2 != result2.end(); it2++) {

				if((*it2)->isNode()) {
					const NodeImpl *nodeImpl2 = (const NodeImpl*)(*it2)->getInterface(Item::gPathan);
					assert(nodeImpl2 != 0);

					string file, xquery;
					DOMNode *dn = (DOMNode *)nodeImpl2->getDOMNode();
					env_.getQuery(dn, file, xquery);

					Sequence result3 = env_.query(
						"for $a in @pending return string($a)", dn);
					if(result3.getLength() != 0) {
						const XMLCh *s = ((const Node::Ptr)result3.first())->
							asString((const DynamicContext*)env_.xqContext());
						if(log) PENDING_MSG(*log, "%1 for file: '%2'") << XMLChToUTF8(s).str() << file;
					} else {
						doTest(file, xquery);
					}
				}
			}
		}
	}
}

struct QPItem {
	bool inserted;
	string name;
	// Note: VC6 can't cope with this recursive definition.
	//  set<QPItem> items;
	// Fake it...
	void *items;

	QPItem() : inserted(false), items(new set<QPItem>) {
	}

	~QPItem() {
		if (!inserted)
			delete get_items();
	}

	set <QPItem> *get_items() const {
		return (set <QPItem> *)items;
	}

	void insert(QPItem &toInsert) {
		get_items()->insert(toInsert);
		toInsert.inserted = true;
	}

	bool operator==(const QPItem &o) const {
		return name == o.name &&
			*get_items() == *o.get_items();
	}

	bool operator<(const QPItem &o) const {
		if(name < o.name)
			return true;
		else if(name == o.name)
			return *get_items() < *o.get_items();
		return false;
	}

	string toString() const {
		string result = name;
		if(!get_items()->empty()) {
			result += "(";
			bool first = true;
			for(set<QPItem>::const_iterator i = get_items()->begin();
			    i != get_items()->end(); ++i) {
				if(first) first = false;
				else result += ",";
				result += i->toString();
			}
			result += ")";
		}
		return result;
	}
};

static size_t parseQP(const std::string &qp, QPItem &result, size_t pos = 0) {
	bool argMode = false;
	size_t start = pos;
	while(pos < qp.size()) {
		char c = qp[pos++];
		switch(c) {
		case '(': {
			result.name = qp.substr(start, pos - start - 1);

			QPItem arg;
			pos = parseQP(qp, arg, pos);
			result.insert(arg);

			argMode = true;
			break;
		}
		case ')': {
			if(!argMode) {
				result.name = qp.substr(start, pos - start - 1);
				return pos - 1;
			}
			else return pos;
		}
		case ',': {
			if(argMode) {
				QPItem arg;
				pos = parseQP(qp, arg, pos);
				result.insert(arg);
			}
			else {
				result.name = qp.substr(start, pos - start - 1);
				return pos - 1;
			}
			break;
		}
		default: {
			// do nothing
			break;
		}
		}
	}

	result.name = qp.substr(start, pos - start - 1);
	return pos;
}

static std::string makeQueryPlanComparable(const std::string &qp)
{
	string result;
	int qp_end = 0;
	int qp_start = qp.find("QPlan>");
	while(qp_start != -1) {
		result += qp.substr(qp_end, qp_start + 6 - qp_end);
		QPItem qpi;
		qp_end = parseQP(qp, qpi, qp_start + 6);
		result += qpi.toString();
		qp_start = qp.find("QPlan>", qp_end + 9);
	}
	result += qp.substr(qp_end, qp.size() - qp_end);

	return result;
}

#define UPDATE_QUERY_RESULTS 0
#define UPDATE_QUERY_PLANS 0

// Prepare and execute the specified XQuery - use both doc() and collection()
// expressions as the input source (although only doc() results are verified).
// Queries are run for for both document and value return types, using eager
// evaluation
void XQueryTest::doTest(const std::string &file, const std::string &xquery)
{
	TestLogger *log = env_.log();
	if(log) TEST_MSG(*log, "Executing the XQuery 1.0 expression in the file '%1'") << file;

	// build queries using different input sources
	map<string, string> inputSources;
	typedef map<string, string>::const_iterator CI;

	string container_name = env_.id() + ".dbxml";

	// write query using a doc() input source
	//  - convert existing string by adding "dbxml:<container name>/" prefix
	static const string doc_function("doc(\"");
	string doc_prefix = "dbxml:" + container_name + "/";

	string tmp(xquery);
	u_int32_t pos(0);
	size_t idx = tmp.find(doc_function, pos);
	while( idx != string::npos) {
		size_t pos_function = idx + doc_function.length();
		tmp.replace(pos_function, 0, doc_prefix);
		pos = pos_function + doc_prefix.length();
		idx = tmp.find(doc_function, pos);
	}
	inputSources["doc()"] = tmp;

	// build map of return types
	map<XmlQueryContext::ReturnType, string> returnTypes;
	typedef map<XmlQueryContext::ReturnType, string>::const_iterator CI2;
	returnTypes[XmlQueryContext::DeadValues] = "Dead Values";
	returnTypes[XmlQueryContext::LiveValues] = "Live Values";

	XmlQueryContext *xqc = env_.context();

	// iterate over input source type
	for(CI it = inputSources.begin(); it != inputSources.end(); ++ it) {
		string desc(it->first);
		string query_expr(it->second);

		if(log) TEST_MSG(*log, "Using input source %1; query is:\n\n%2\n") << desc << query_expr;

		// iterate over return types
		for(CI2 it2 = returnTypes.begin(); it2 != returnTypes.end(); ++ it2) {
			XmlQueryContext::ReturnType type = it2->first;
			if(log) TEST_MSG(*log, "Using %1") << it2->second;

			xqc->setReturnType(type);

			XmlTransaction *txn = 0;
			if(env_.transacted()) {
				txn = new XmlTransaction(env_.createTransaction());
			}

			XmlQueryExpression *expr = 0;
			if(env_.transacted()) {
				expr = new XmlQueryExpression(env_.db()->prepare(*txn, query_expr, *xqc));
			} else {
				expr = new XmlQueryExpression(env_.db()->prepare(query_expr, *xqc));
			}
			if(log) TEST_MSG(*log, "Prepared expression - about to execute");

			XmlResults *results = 0;
			if(env_.transacted()) {
				results = new XmlResults(expr->execute(*txn, *xqc));
			} else {
				results = new XmlResults(expr->execute(*xqc));
			}
			if(log) TEST_MSG(*log, "Executed expression; %1 hits found") << results->size();

			// string together individual values to get actual result
			static const string expected_extension(".out");

			string actual;
			XmlValue value;
			while(results->next(value)) {
				actual.append(value.asString());
			}

			string expected;
			if(getResult(file, expected_extension, expected)) {
				if(!actual.compare(expected)) {
					if(log) TEST_MSG(*log, "Results matched - successful test");
				} else {
#if UPDATE_QUERY_RESULTS
					setResult(file, expected_extension, actual);
#else
					ostringstream os;
					os << "Results differ - test failed:\n";
					os << "Got:\n***" << actual << "***\n";
					os << "Expected:\n***" << expected << "***\n";
					if(log) ERROR_MSG(*log, os.str());
#endif
				}
			} else {
				setResult(file, expected_extension, actual);
				if(log) ERROR_MSG(*log, "No expected result found");
			}

			string qp_extension;
			if(env_.flags() & DBXML_INDEX_NODES) {
				// The query plan is different if have enabled
				// the indexing of nodes
				qp_extension += ".index_nodes";
			}
			qp_extension += ".qplan";

			string actual_qp = expr->getQueryPlan();

			string expected_qp;
			if(getResult(file, qp_extension, expected_qp)) {
				string aqp = makeQueryPlanComparable(actual_qp);
				string eqp = makeQueryPlanComparable(expected_qp);

				if(aqp != eqp) {
#if UPDATE_QUERY_PLANS
					setResult(file, qp_extension, actual_qp);
#else
					ostringstream os;
					os << "Query plans differ - test failed:\n";
					os << "Got:\n***" << aqp << "***\n";
					os << "Expected:\n***" << eqp << "***\n";
					if(log) ERROR_MSG(*log, os.str());
					break;
#endif
				} else {
					if(log) TEST_MSG(*log, "Query plan matched - successful test");
				}
			} else {
				setResult(file, qp_extension, actual_qp);
				if(log) ERROR_MSG(*log, "No query plan result found");
			}

			if(env_.transacted()) {
				env_.commitTransaction(*txn);
			}
			delete txn;

			delete results;
			delete expr;
		}
	}

	if(log) TEST_MSG(*log, "Completed test");
}

bool XQueryTest::getResult(const std::string &file, const std::string &new_extension, std::string &expected)
{
	bool ret(false);
	// ".xquery"  -> ".out" for file name
	static const string old_extension(".xquery");
	string tmp(file);
	size_t idx = tmp.rfind(old_extension);
	if(idx != string::npos) {
		tmp.replace(idx, old_extension.length(), new_extension);
	}
	string path = env_.dataDir() + tmp;
	ostringstream oss;
	FILE *fd = fopen(path.c_str(), "rb");
	if(fd != 0) {
		static const int buffer_size = 2048;
		unsigned char buffer[buffer_size];
		do {
			size_t len = fread(buffer, sizeof(unsigned char), buffer_size, fd);
			oss.write((char*)buffer, len);
		} while(!feof(fd));
		fclose(fd);
		expected.assign(oss.str());
		ret = true;
	}

	return ret;
}

bool XQueryTest::setResult(const std::string &file, const std::string &new_extension, const std::string &expected)
{
	bool ret(false);
	// ".xquery"  -> ".out" for file name
	static const string old_extension(".xquery");
	string tmp(file);
	size_t idx = tmp.rfind(old_extension);
	if(idx != string::npos) {
		tmp.replace(idx, old_extension.length(), new_extension);
	}
	string path = env_.dataDir() + tmp;

	FILE *fd = fopen(path.c_str(), "wb");
	if(fd != 0) {
		fwrite(expected.c_str(), sizeof(char), expected.length(), fd);
		fclose(fd);
		ret = true;
	}

	return ret;
}
