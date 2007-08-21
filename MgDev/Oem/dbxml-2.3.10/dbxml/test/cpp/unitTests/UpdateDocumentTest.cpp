//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: UpdateDocumentTest.cpp,v 1.19 2006/10/30 17:46:14 bostic Exp $
//

// tests the updating of documents (Xerces-C DOM methods)
//

// DB XML includes (external)
#include "dbxml/DbXml.hpp"

// System includes
#include <string>
#include <iostream>
#include <sstream>
#include <map>
#include <cassert>

// Xerces includes
#include <xercesc/util/XMLUniDefs.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/dom/DOM.hpp>

// XQilla includes
#include <xqilla/items/Node.hpp>

// test code includes
#include "../util/TestLogging.hpp"
#include "TestEnvironment.hpp"
#include "UpdateDocumentTest.hpp"

using namespace DbXml;
using namespace DbXmlTest;
using namespace std;
#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

// each update test needs to derive from Functor and register an entry in
// updateMethods_ (in UpdateDocumentTest::prepare)
class DbXmlTest::Functor
{
public:
  Functor(const std::string &name) : name_(name) {}
  virtual ~Functor() {}

  virtual void modify(TestLogger *log, XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node) const = 0;
  virtual void check(TestLogger *log, XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node) const = 0;

protected:
  std::string getName() const { return name_; }

private:
  std::string name_;
};

// utility methods for test functors
namespace {
  void header(TestLogger *log, const std::string &action, const std::string &method, XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node) {
    ostringstream os;
    os << "Starting " << action << " for method '" << method << "'";
    os << " (on node '" << XMLChToUTF8(node->getNodeName()).str() << "')";
    if(log) {
      TEST_MSG(*log, os.str());
    } else {
      cerr << os.str() << endl;
    }
  }

  void footer(TestLogger *log, const std::string &action, const std::string &method, XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node) {
    ostringstream os;
    os << "Finished " << action << " for method '" << method << "'";
    os << " (on node '" << XMLChToUTF8(node->getNodeName()).str() << "')";
    if(log) {
      TEST_MSG(*log, os.str());
    } else {
      cerr << os.str() << endl;
    }
  }

}

////////////////////////////////////////////////////////////////////////
// SetTextContent...
class SetTextContentFunctor : public Functor
{
public:
	SetTextContentFunctor() : Functor("setTextContent") {}

	//////
	// modify
	virtual void modify(TestLogger *log, XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node) const {
		header(log, "modify", getName(), node);

		if(node->getNodeType() != DOMNode::ELEMENT_NODE &&
		   node->getNodeType() != DOMNode::ATTRIBUTE_NODE) {
			// modify - prepend "Dr" to author name
			string newTextContent =  string("Dr ") + string(XMLChToUTF8(node->getTextContent()).str());
			node->setTextContent(UTF8ToXMLCh(newTextContent).str());
			if(log) TEST_MSG(*log, "setTextContent() called");
		}

		footer(log, "modify", getName(), node);
	}

	//////
	// check
	virtual void check(TestLogger *log, XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node) const {
		header(log, "check", getName(), node);

		if(node->getNodeType() != DOMNode::ELEMENT_NODE &&
		   node->getNodeType() != DOMNode::ATTRIBUTE_NODE) {
			// text content should contain "Dr"
			if(!XMLString::startsWith(node->getTextContent(), UTF8ToXMLCh("Dr").str())) {
				if(log) ERROR_MSG(*log, "Cannot find 'Dr' in text content '%1'") << XMLChToUTF8(node->getTextContent()).str();
				return;
			}
		}

		if(log) TEST_MSG(*log, "Text content OK");
		footer(log, "check", getName(), node);
	}
};

////////////////////////////////////////////////////////////////////////
// SetTextContent2...
static const XMLCh flibble[] = { 'f', 'l', 'i', 'b', 'b', 'l', 'e', 0 };

class SetTextContent2Functor : public Functor
{
public:
	SetTextContent2Functor() : Functor("setTextContent()[2]") {}

	//////
	// modify
	virtual void modify(TestLogger *log, XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node) const {
		header(log, "modify", getName(), node);

		if(node->getNodeType() != DOMNode::ELEMENT_NODE &&
		   node->getNodeType() != DOMNode::ATTRIBUTE_NODE) {
			node->setTextContent(flibble);
			if(log) TEST_MSG(*log, "setTextContent() called");
		}

		footer(log, "modify", getName(), node);
	}

	//////
	// check
	virtual void check(TestLogger *log, XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node) const {
		header(log, "check", getName(), node);

		if(node->getNodeType() != DOMNode::ELEMENT_NODE &&
		   node->getNodeType() != DOMNode::ATTRIBUTE_NODE) {
			if(!XMLString::equals(node->getTextContent(), flibble)) {
				if(log) ERROR_MSG(*log, "Text content equals '%1'") << XMLChToUTF8(node->getTextContent()).str();
				return;
			}

			XERCES_CPP_NAMESPACE_QUALIFIER DOMNodeList *children = node->getChildNodes();
			if(!children || children->getLength() != 1) {
				if(log) ERROR_MSG(*log, "Wrong number of children");
				return;
			}
			if(children->item(0)->getNodeType() != DOMNode::TEXT_NODE) {
				if(log) ERROR_MSG(*log, "Child of incorrect type '%1'") << children->item(0)->getNodeType();
				return;
			}
		}

		if(log) TEST_MSG(*log, "Text content OK");
		footer(log, "check", getName(), node);
	}
};

////////////////////////////////////////////////////////////////////////
// SetAttribute
static XMLCh depth[] = { 'd', 'e', 'p', 't', 'h', 0 };
static XMLCh ten[] = { '1', '0', 0 };

class SetAttributeFunctor : public Functor
{
public:
	SetAttributeFunctor() : Functor("setAttribute") {}

	//////
	// modify
	virtual void modify(TestLogger *log, XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node) const {
		header(log, "modify", getName(), node);

		if(node->getNodeType() == XERCES_CPP_NAMESPACE_QUALIFIER DOMNode::ELEMENT_NODE) {
			// modify - create a "depth" attribute
			DOMElement *ele = static_cast<DOMElement*>(node);
			ele->setAttribute(depth, ten);
		}

		footer(log, "modify", getName(), node);
	}

	//////
	// check
	virtual void check(TestLogger *log, XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node) const {
		header(log, "check", getName(), node);

		if(node->getNodeType() == XERCES_CPP_NAMESPACE_QUALIFIER DOMNode::ELEMENT_NODE) {
			DOMElement *ele = static_cast<DOMElement*>(node);
			DOMAttr *attr = ele->getAttributeNode(depth);
			if(!attr) {
				if(log) ERROR_MSG(*log, "Attribute does not exist");
				return;
			}
			else if(!XMLString::equals(attr->getNodeValue(), ten)) {
				if(log) ERROR_MSG(*log, "Attribute equals '%1'") << XMLChToUTF8(attr->getNodeValue()).str();
				return;
			}
		}
		else {
			if(node->getAttributes() != 0) {
				if(log) ERROR_MSG(*log, "Node type '%1' has attributes") << node->getNodeType();
			}
		}

		if(log) TEST_MSG(*log, "Attribute OK");
		footer(log, "check", getName(), node);
	}
};

////////////////////////////////////////////////////////////////////////
// SetAttributeNS

static XMLCh random_uri[] = { 'r', 'a', 'n', ':', 'd', 'o', 'm', 0 };

class SetAttributeNSFunctor : public Functor
{
public:
	SetAttributeNSFunctor() : Functor("setAttributeNS") {}

	//////
	// modify
	virtual void modify(TestLogger *log, XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node) const {
		header(log, "modify", getName(), node);

		if(node->getNodeType() == XERCES_CPP_NAMESPACE_QUALIFIER DOMNode::ELEMENT_NODE) {
			// modify - create a "depth" attribute
			DOMElement *ele = static_cast<DOMElement*>(node);
			ele->setAttributeNS(random_uri, depth, ten);
		}

		footer(log, "modify", getName(), node);
	}

	//////
	// check
	virtual void check(TestLogger *log, XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node) const {
		header(log, "check", getName(), node);

		if(node->getNodeType() == XERCES_CPP_NAMESPACE_QUALIFIER DOMNode::ELEMENT_NODE) {
			DOMElement *ele = static_cast<DOMElement*>(node);

			DOMAttr *attr = ele->getAttributeNodeNS(random_uri, depth);
			if(!attr) {
				if(log) ERROR_MSG(*log, "Attribute does not exist");
				return;
			}
			else if(!XMLString::equals(attr->getNodeValue(), ten)) {
				if(log) ERROR_MSG(*log, "Attribute equals '%1'") << XMLChToUTF8(attr->getNodeValue()).str();
				return;
			}

			if(string(XMLChToUTF8(node->getLocalName()).str()) == "TestDoc") {
				DOMAttr *attr2 = ele->getAttributeNodeNS(UTF8ToXMLCh("http://www.w3.org/2000/xmlns/").str(),
					UTF8ToXMLCh("t").str());
				if(!attr2) {
					if(log) ERROR_MSG(*log, "Namespace attribute does not exist");
					return;
				}
			}
		}
		else {
			if(node->getAttributes() != 0) {
				if(log) ERROR_MSG(*log, "Node type '%1' has attributes") << node->getNodeType();
			}
		}

		if(log) TEST_MSG(*log, "Attribute OK");
		footer(log, "check", getName(), node);
	}
};

// utilities for main part of program
namespace
{
  // converting (string) program arguments to enumerations
  UpdateDocumentTest::EvaluationType evaluationNameToType(const std::string name)
  {
    UpdateDocumentTest::EvaluationType type(UpdateDocumentTest::EVAL_NOT_SET);
    if(!name.compare("eager")) {
      type = UpdateDocumentTest::EAGER;
    } else if(!name.compare("lazy")) {
      type = UpdateDocumentTest::LAZY;
    }
    return type;
  }

  void usage(const string &progname, int exitCode)
  {
    cerr << "Usage: " << progname << " [INHERITED OPTIONS] [OPTIONS]" << endl;
    cerr << endl;
    cerr << "Options:" << endl;
    cerr << "\t--eval type\t\tevaluation type [lazy|eager]" << endl;
    cerr << endl;
    cerr << "Inherited options are those for the calling program." << endl;
    cerr << endl;

    exit(exitCode);
  }
}

// Execute a container query, modify the results and update the document
// Repeat the query, and check each node for the update
//
void UpdateDocumentTest::doTest(const std::string &method, const std::string &xpath)
{
  TestLogger *log = env_.log();
  if(log) TEST_MSG(*log, "Starting an UpdateDocument test");

  // sanity check - we have a test for this method
  UpdateMethodMap::const_iterator i = updateMethods_.find(method);
  if(i == updateMethods_.end()) {
    if(log) ERROR_MSG(*log, "No test code defined for update method '%1'") << method;
    throw std::exception();
  }
  Functor *testFunctor = i->second;

  if(log) TEST_MSG(*log, "Using the method '%1' and query '%2'") << method << xpath;

  XmlQueryContext *xqc = env_.context();
  xqc->setReturnType(XmlQueryContext::LiveValues);
  if(evalType_ == UpdateDocumentTest::EAGER) {
    xqc->setEvaluationType(XmlQueryContext::Eager);
  } else if(evalType_ == UpdateDocumentTest::LAZY) {
    xqc->setEvaluationType(XmlQueryContext::Lazy);
  }

  xqc->setNamespace("t", "test:namespace");

  // start a transaction
  XmlTransaction *xtxn = 0;
  bool transacted = env_.transacted();
  if(transacted) {
    xtxn = new XmlTransaction(env_.createTransaction());
  }

  // query the container - should return live nodes from single document
  XmlContainer *xc = env_.container();
  XmlManager *mgr = env_.db();
  XmlResults *xr = 0;
  std::string query = "collection('" + xc->getName() + "')";
  query += xpath;
  if(transacted) {
    xr = new XmlResults(mgr->query(*xtxn, query, *xqc));
  } else {
    xr = new XmlResults(mgr->query(query, *xqc));
  }

  // this is our copy of the original document - content to be set using DOM
  XmlDocument *xd = 0;

  // modify the nodes
  int originalCount = 0;
  XmlValue xv;
  while(xr->next(xv)) {
    if(!xv.isNode()) continue;

    ++originalCount;

    // update the node
    DOMNode *node = xv.asNode();
    testFunctor->modify(log, node);

    /// DEBUG
    if(false) {
      DOMWriter *writer = DOMImplementation::getImplementation()->createDOMWriter();
      cout << "DEBUG\nUpdated DOM node:\n" << ">>>>>>\n" << XMLChToUTF8(writer->writeToString(*node)).str() << "\n<<<<<<\n";
      delete writer;
    }
    ///

    // transient check - the DOM node should now be modified
    testFunctor->check(log, node);

    if(xd == 0) {
	    xd = new XmlDocument(xv.asDocument());
    }
    else if(*xd != xv.asDocument()) {
	    xd->setContentAsDOM(node->getOwnerDocument());

	    /// DEBUG
	    if(false) {
		    string buf;
		    cout << "DEBUG\nUpdated document has string content:\n" << ">>>>>>\n" << xd->getContent(buf) << "\n<<<<<<\n";
	    }
	    ///

	    XmlUpdateContext *xuc = env_.uc();
	    if(transacted) {
		    xc->updateDocument(*xtxn, *xd, *xuc);
	    } else {
		    xc->updateDocument(*xd, *xuc);
	    }
	    if(log) TEST_MSG(*log, "Document has been updated");

	    delete xd;
	    xd = new XmlDocument(xv.asDocument());
    }
  }

  if(xd != 0) {
	    xd->setContentAsDOM(xd->getContentAsDOM());

	    /// DEBUG
	    if(false) {
		    string buf;
		    cout << "DEBUG\nUpdated document has string content:\n" << ">>>>>>\n" << xd->getContent(buf) << "\n<<<<<<\n";
	    }
	    ///

	    XmlUpdateContext *xuc = env_.uc();
	    if(transacted) {
		    xc->updateDocument(*xtxn, *xd, *xuc);
	    } else {
		    xc->updateDocument(*xd, *xuc);
	    }
	    if(log) TEST_MSG(*log, "Document has been updated");

	    delete xd;
	    xd = 0;
  }

  // sanity checks
  if(!originalCount) {
    if(log) ERROR_MSG(*log, "Query did not hit - no nodes were modified!");
  }
  if(log) TEST_MSG(*log, "Completed node modifications");

  if(transacted) {
    env_.commitTransaction(*xtxn);
  }
  delete xtxn;

  delete xr;

  //////////////////////////////////////////////////////////////
  // repeat query (same nodes should be returned)

  // start a new transaction
  xtxn = 0;
  if(transacted) {
    xtxn = new XmlTransaction(env_.createTransaction());
  }

  // execute the same query and check the results for the updates
  xr = 0;
  if(transacted) {
    xr = new XmlResults(mgr->query(*xtxn, query, *xqc));
  } else {
    xr = new XmlResults(mgr->query(query, *xqc));
  }
  if(log) TEST_MSG(*log, "Repeated query, about to check results again");

  // check the nodes
  int updatedCount = 0;
  while(xr->next(xv)) {
    if(!xv.isNode()) continue;

    ++updatedCount;

    DOMNode *node = xv.asNode();
    testFunctor->check(log, node);
  }

  // possibility of a modify causing a node to be missed?
  if(originalCount != updatedCount) {
    if(log) ERROR_MSG(*log, "Found %1 nodes before update but %2 after") << originalCount << updatedCount;
  }

  if(transacted) {
    env_.commitTransaction(*xtxn);
  }

  delete xtxn;
  delete xr;

  if(log) TEST_MSG(*log, "Completed test");
}

UpdateDocumentTest::UpdateDocumentTest(TestEnvironment &env, const Iterator<std::string> &args)
  : UnitTest(env, "UpdateDocumentTest"),
    evalType_(UpdateDocumentTest::EVAL_NOT_SET)
{
  TestLogger *log = env_.log();

  // process arguments: evaluation type
  string evalStr, methodStr;
  string errMsg;
  bool ok(true);
  while(args.hasNext() && ok) {
    string arg(args.next());
    if(!arg.compare(0, 1, "-")) {
      if(!arg.compare(1, 5, "-eval")) {
        if(!args.hasNext()) {
          errMsg.append("No evaluation type argument specified!");
          ok = false;
        } else {
          evalStr.assign(args.next());
          evalType_ = evaluationNameToType(evalStr);
        }
      }
      else {
        // unknown switch - ignore
      }
    }
    else {
      // unknown argument - ignore
    }
  }

  if(!errMsg.empty()) {
    ERROR_MSG(*log, "%1") << errMsg;
  	usage(name_, -1);
    return;
  }

  if(evalType_ == UpdateDocumentTest::EVAL_NOT_SET) {
    ERROR_MSG(*log, "No recognised evaluation type specified!");
    return;
  }
  INFO_MSG(*log, "Evaluation type is %1") << evalStr;
}

UpdateDocumentTest::~UpdateDocumentTest()
{
  UpdateMethodMap::const_iterator i;
  for(i = updateMethods_.begin(); i != updateMethods_.end(); ++i) {
    delete i->second;
  }
}

// prepare
//
//   - hold a copy of the original
//   - get the name of the document (adapt to handle multiple docs if needed)
//   - set up the collection of update methods (Functors)
void UpdateDocumentTest::prepare()
{
  TestLogger *log = env_.log();

  //////////////
  // build associations between update method IDs and methods
  updateMethods_["setTextContent"] = new SetTextContentFunctor;
  updateMethods_["setTextContent2"] = new SetTextContent2Functor;
  updateMethods_["setAttribute"] = new SetAttributeFunctor;
  updateMethods_["setAttributeNS"] = new SetAttributeNSFunctor;

  //////////////
  // get the name of the document (needed for getting orginal document)
  bool transacted = env_.transacted();

  XmlTransaction *xtxn = 0;
  if(transacted) {
    xtxn = new XmlTransaction(env_.createTransaction());
  }

  // use db xml metadata query for document name
  XmlContainer *xc = env_.container();
  string xpath("dbxml:metadata('dbxml:name', collection('");
  xpath.append(xc->getName());
  xpath.append("'))");

  XmlManager *db = env_.db();
  XmlQueryContext *xqc = env_.context();

  XmlResults *xr = 0;
  if(transacted) {
    xr = new XmlResults(db->query(*xtxn, xpath, *xqc, DBXML_LAZY_DOCS));
  } else {
    xr = new XmlResults(db->query(xpath, *xqc, DBXML_LAZY_DOCS));
  }

  if(xr->size() != 1) {
    if(log) ERROR_MSG(*log, "Wrong number of document names found - expected 1 but got %1") << xr->size();
    throw std::exception();
  }
  XmlValue xv;
  xr->next(xv);
  docName_.assign(xv.asString());

  delete xr;

  if(transacted) {
    env_.commitTransaction(*xtxn);
  }
  delete xtxn;

  if(log) TEST_MSG(*log, "Control document is called '%1'") << docName_;
}

// execute
//
// for each test, reads an update method and an xpath query
void UpdateDocumentTest::execute()
{
	// iterate over individual tests (<base> elements in the index file)
	Sequence result = env_.query("/test/base");
	for (Sequence::iterator it = result.begin();
	     it != result.end(); it++) {
		if((*it)->isNode()) {
			const DOMNode *node = (const DOMNode*)(*it)->getInterface(Node::gXerces);
			assert(node != 0);

			// for each test, get the (update) method, query, ...
			string method;
			env_.getTextFromChildNode((DOMNode*)node,
						  "method", method);

			string xpath;
			env_.getTextFromChildNode((DOMNode*)node,
						  "query", xpath);

			doTest(method, xpath);

			resetContainer();
		}
	}
}

// Helper method - resets container by removing updated document and re-adding
// original document
void UpdateDocumentTest::resetContainer()
{
	TestLogger *log = env_.log();

	// sanity check - we assume a single document
	if(env_.documentCount() != 1) {
		if(log) ERROR_MSG(*log, "Expected 1 document but got %1") << env_.documentCount();
		throw std::exception();
	}

	bool transacted = env_.transacted();

	XmlTransaction *xtxn = 0;
	if(transacted) {
		xtxn = new XmlTransaction(env_.createTransaction());
	}

	env_.reloadContainer(xtxn);

	if(transacted) {
		xtxn->commit();
	}
	delete xtxn;
}

