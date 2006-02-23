//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: ResolverUnitTest.cpp,v 1.9 2005/06/27 19:56:00 gmf Exp $
//

// unit tests the Resolver

// System includes
#include <string>
#include <cassert>

#include <pathan/internal/items/impl/NodeImpl.hpp>

#include <xercesc/util/XMLString.hpp>

// DB XML includes (external)
#include "dbxml/DbXml.hpp"

// Local (test) includes
#include "../util/TestLogging.hpp"
#include "../util/TestResolver.hpp"
#include "TestEnvironment.hpp"
#include "ResolverUnitTest.hpp"

using namespace DbXml;
using namespace DbXmlTest;
using namespace std;
XERCES_CPP_NAMESPACE_USE

ResolverUnitTest::ResolverUnitTest(TestEnvironment &env, const Iterator<std::string> &args)
  : UnitTest(env, "ResolverUnitTest")
{
}

ResolverUnitTest::~ResolverUnitTest()
{
}

void ResolverUnitTest::execute()
{
	TestLogger *log = env_.log();

	// load the test resolver, specifying the directory for the XML files
	TestResolver resolver;
	resolver.setDir(env_.dataDir());
	env_.db()->registerResolver(resolver);
	if(log) TEST_MSG(*log, "Registered test resolver for local directory '%1'") << env_.dataDir();

	// outer loop is over individual tests
	Sequence bases = env_.query("/test/base");
	for (Sequence::iterator it = bases.begin();
	     it != bases.end(); it++) {
		if((*it)->isNode()) {
			const NodeImpl *nodeImpl = (const NodeImpl*)(*it)->getInterface(Item::gPathan);
			assert(nodeImpl != 0);

			const DOMNode *base = nodeImpl->getDOMNode();

			// inner loop is over inputs (documents, ...)
			//   create document from file, load document,
			//   delete document
			Sequence inputs = env_.query("document",
						     (DOMNode *)base);
			for (Sequence::iterator it1 = inputs.begin();
			     it1 != inputs.end(); it1++) {
				if((*it1)->isNode()) {
					const NodeImpl *nodeImpl1 =
						(const NodeImpl*)(*it1)->getInterface(Item::gPathan);
					assert(nodeImpl1 != 0);

					const DOMNode *input =
						nodeImpl1->
						getDOMNode();

					const XMLCh *xstr = input->
						getFirstChild()->getNodeValue();

					// load contents into document
					XmlDocument xd = env_.db()->
						createDocument();
					env_.putFileInDocument(XMLChToUTF8(xstr).
							       str(), xd);
					string name(xd.getName());
					if(log) TEST_MSG(*log, "Created document '%1'") << name;

					// load the document into the container
					XmlContainer *container = env_.container();
					if(env_.transacted()) {
						XmlTransaction txn(env_.createTransaction());
						container->putDocument(txn, xd, *env_.uc());
						env_.commitTransaction(txn);
					} else {
						container->putDocument(xd, *env_.uc());
					}
					if(log) TEST_MSG(*log, "Loaded '%1' into container") << name;

					// delete the document
					if(env_.transacted()) {
						XmlTransaction txn(env_.createTransaction());
						container->deleteDocument(txn, xd, *env_.uc());
						env_.commitTransaction(txn);
					} else {
						container->deleteDocument(xd, *env_.uc());
					}
					if(log) TEST_MSG(*log, "Deleted '%1' from container") << name;
				}
			}
		}
	}

	if(log) TEST_MSG(*log, "Completed the resolver unit tests");
}

