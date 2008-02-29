//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: QueryRunner.cpp,v 1.19 2006/10/30 17:46:05 bostic Exp $
//

#include "dbxml/DbXml.hpp"

#include <fstream>
#include <cstdio>
#include <ctime>
#include <string>
#include <ctime>
#include <map>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMWriter.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/dom/DOMImplementationLS.hpp>
#include <xercesc/framework/StdOutFormatTarget.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/framework/MemBufFormatTarget.hpp>
#include <xercesc/util/XMLUni.hpp>
#include <xercesc/util/XMLUniDefs.hpp>
#include <xercesc/dom/DOMBuilder.hpp>
#include <xercesc/dom/DOMNodeList.hpp>

#include <xercesc/util/PlatformUtils.hpp>

#include "QueryRunner.hpp"
#include "Transcoding.h"

static XMLCh database_str[] = { XERCES_CPP_NAMESPACE_QUALIFIER chLatin_d,
XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t,
XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_b,
XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_s,
XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chNull };

static XMLCh group_str[] = { XERCES_CPP_NAMESPACE_QUALIFIER chLatin_g,
XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o,
XERCES_CPP_NAMESPACE_QUALIFIER chLatin_u, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_p,
XERCES_CPP_NAMESPACE_QUALIFIER chNull };

static XMLCh name_str[] = { XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n,
XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_m,
XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chNull };

static XMLCh container_str[] = { XERCES_CPP_NAMESPACE_QUALIFIER chLatin_c,
XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n,
XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a,
XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n,
XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r,
XERCES_CPP_NAMESPACE_QUALIFIER chNull };

static XMLCh query_str[] = { XERCES_CPP_NAMESPACE_QUALIFIER chLatin_q,
XERCES_CPP_NAMESPACE_QUALIFIER chLatin_u, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e,
XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_y,
XERCES_CPP_NAMESPACE_QUALIFIER chNull };

static XMLCh size_str[] = { XERCES_CPP_NAMESPACE_QUALIFIER chLatin_s,
XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_z, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chNull };

static XMLCh type_str[] = { XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t,
XERCES_CPP_NAMESPACE_QUALIFIER chLatin_y, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_p, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chNull };

static XMLCh LS_str[] = { XERCES_CPP_NAMESPACE_QUALIFIER chLatin_L,
XERCES_CPP_NAMESPACE_QUALIFIER chLatin_S, XERCES_CPP_NAMESPACE_QUALIFIER chNull };

static XMLCh XQueryRunner_str[] = { XERCES_CPP_NAMESPACE_QUALIFIER chLatin_x,
XERCES_CPP_NAMESPACE_QUALIFIER chLatin_q, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_u,
XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r,
XERCES_CPP_NAMESPACE_QUALIFIER chLatin_y, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r,
XERCES_CPP_NAMESPACE_QUALIFIER chLatin_u, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n,
XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e,
XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r, XERCES_CPP_NAMESPACE_QUALIFIER chNull }
;
static XMLCh action_str[] = { XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a,
XERCES_CPP_NAMESPACE_QUALIFIER chLatin_c, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t,
XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o,
XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, XERCES_CPP_NAMESPACE_QUALIFIER chNull };

static XMLCh description_str[] = { XERCES_CPP_NAMESPACE_QUALIFIER chLatin_d,
XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_s,
XERCES_CPP_NAMESPACE_QUALIFIER chLatin_c, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r,
XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_p,
XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i,
XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n,
XERCES_CPP_NAMESPACE_QUALIFIER chNull };

static XMLCh time_str[] = { XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t,
XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_m,
XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chNull };

static XMLCh count_str[] = { XERCES_CPP_NAMESPACE_QUALIFIER chLatin_c,
XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_u, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, XERCES_CPP_NAMESPACE_QUALIFIER chNull };

static XMLCh result_str[] = { XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r,
XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_s,
XERCES_CPP_NAMESPACE_QUALIFIER chLatin_u, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_l,
XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, XERCES_CPP_NAMESPACE_QUALIFIER chNull };

static XMLCh error_str[] = { XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e,
XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r,
XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r,
XERCES_CPP_NAMESPACE_QUALIFIER chNull };

static XMLCh resultitem_str[] = { XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r,
XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_s,
XERCES_CPP_NAMESPACE_QUALIFIER chLatin_u, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_l,
XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i,
XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e,
XERCES_CPP_NAMESPACE_QUALIFIER chLatin_m, XERCES_CPP_NAMESPACE_QUALIFIER chNull };

namespace {
  std::string typeToString(DbXml::XmlContainer::ContainerType type) {
    std::string str("?");
    switch(type) {
    case DbXml::XmlContainer::WholedocContainer:
      str.assign("WDS");
      break;
    case DbXml::XmlContainer::NodeContainer:
      str.assign("NLS");
      break;
    default:
      break;
    }
    return str;
  }

  // try to guess size from name, e.g. "TC-MD-tiny.dbxml"
  std::string getSizeFromName(const std::string &name) {
    static std::map<std::string, std::string> sizes;
    sizes["miniscule"] = "Miniscule";
    sizes["tiny"] = "Tiny";
    sizes["small"] = "Small";
    sizes["medium"] = "Medium";
    sizes["normal"] = "Normal";
    sizes["large"] = "Large";
    sizes["huge"] = "Huge";
    typedef std::map<std::string,std::string>::const_iterator CI;

    std::string str("?");
    for(CI it = sizes.begin(); it != sizes.end(); ++it) {
      if(name.find(it->first) != std::string::npos) {
        str.assign(it->second);
        break;
      }
    }

    return str;
  }
}

QueryRunner::QueryRunner(const std::string &configFile) : outputFileName(0), writeResults(false), debug(false)  {

    initialise();
    parseConfigFile(configFile);
}

QueryRunner::~QueryRunner(){
    delete parser;
    delete writer;
    delete (XMLCh *)outputFileName;
}

void QueryRunner::setWriteResults(bool write) {
  writeResults = write;
}

void QueryRunner::setDebug(bool debug_param) {
  debug = debug_param;
}

void QueryRunner::parseConfigFile(const std::string &configFile) {
    XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc = parser->parseURI(CXXToXMLCh(configFile).unicodeForm());
    databaseName = doc->getDocumentElement()->getAttributeNS(0, database_str);
    XERCES_CPP_NAMESPACE_QUALIFIER DOMNodeList *groupList = doc->getElementsByTagName(group_str);

    int length = groupList->getLength();
    for(int i = 0; i < length; i++) {
       XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *group = static_cast<XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *>(groupList->item(i));
       QueryGroup qp(group->getAttributeNS(0, name_str));
       qp.containers = group->getElementsByTagName(container_str);
       qp.queries = group->getElementsByTagName(query_str);
       queryGroups.push_back(qp);
    }
}

void QueryRunner::initialise() {
    XERCES_CPP_NAMESPACE_QUALIFIER XMLPlatformUtils::Initialize();
    impl = XERCES_CPP_NAMESPACE_QUALIFIER DOMImplementationRegistry::getDOMImplementation(LS_str);
    doc = impl->createDocument(0, XQueryRunner_str, 0);
    writer = impl->createDOMWriter();
    writer->setErrorHandler(this);
    writer->setFeature(XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgDOMWRTFormatPrettyPrint, true);
    parser = impl->createDOMBuilder(XERCES_CPP_NAMESPACE_QUALIFIER DOMImplementationLS::MODE_SYNCHRONOUS, 0);
    parser->setFeature(XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgDOMNamespaces, true);
    parser->setErrorHandler(this);
}

void QueryRunner::writeNode(const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *n) const {

    XERCES_CPP_NAMESPACE_QUALIFIER XMLFormatTarget *myFormTarget;

   if(outputFileName == 0) {
       myFormTarget = new XERCES_CPP_NAMESPACE_QUALIFIER StdOutFormatTarget();
   }
   else {
       myFormTarget = new XERCES_CPP_NAMESPACE_QUALIFIER LocalFileFormatTarget(outputFileName);
   }
   writer->writeNode(myFormTarget, *n);
   delete myFormTarget;
}

void QueryRunner::runTests() const {

  double totalTime = 0;

  //open a container in the db environment
  DbEnv env(0);
  env.set_cachesize(0, 64 * 1024 * 1024, 1);
  env.open(XMLChToCXX(databaseName).localForm().c_str(), DB_INIT_MPOOL|DB_CREATE, 0);
  DbXml::XmlManager mgr(&env);

  std::vector<QueryGroup >::const_iterator vecIt = queryGroups.begin();
  std::vector<QueryGroup >::const_iterator end = queryGroups.end();

  for(;vecIt != end; vecIt++) {
    double groupTime = 0;
    QueryGroup qg = *vecIt;
    XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *groupEle = doc->createElementNS(0, group_str);
    groupEle->setAttributeNS(0, name_str, qg.name);
    doc->getDocumentElement()->appendChild(groupEle);

    if(debug) {
      std::cerr << "Group \"" << XMLChToCXX(qg.name).localForm() << "\"" << std::endl;
    }

    int containerLen = qg.containers->getLength();
    std::string previousContainerName;
    for(int conIndex = 0; conIndex < containerLen; conIndex++) {
      double containerTime = 0;
      const XMLCh* containerName = qg.containers->item(conIndex)->getTextContent();

      if(debug) {
        std::cerr << "Container \"" << XMLChToCXX(containerName).localForm() << "\"" << std::endl;
      }

      XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *containerEle = doc->createElementNS(0, container_str);
      containerEle->setAttributeNS(0, name_str, containerName);
      groupEle->appendChild(containerEle);
      DbXml::XmlContainer container = mgr.openContainer(XMLChToCXX(containerName).localForm());

      std::string currentContainerName(container.getName());

      int queryLen = qg.queries->getLength();
      for(int queryIndex = 0; queryIndex < queryLen; queryIndex++) {

        XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *query = static_cast<XERCES_CPP_NAMESPACE_QUALIFIER DOMElement* >(qg.queries->item(queryIndex));

        XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *description = getElementChildByName(query, description_str);
        XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *action = getElementChildByName(query, action_str);

        if(debug) {
          // display query ID, extracted from description
          std::string descriptonStr(XMLChToCXX(description->getFirstChild()->getNodeValue()).localForm());
          std::string id("?");
          if(descriptonStr.find("_Q") != std::string::npos) {
            size_t idx1 = descriptonStr.find("_Q");
            if(descriptonStr.find_first_of(' ', idx1) != std::string::npos) {
              size_t idx2 = descriptonStr.find_first_of(' ', idx1);
              id = descriptonStr.substr(0, idx2);
            }
          }
          std::cerr << "\t" << "Query \"" << id  << "\"" << std::endl;
        }

        //replace this with stuff from XMLString
        std::string queryString = XMLChToCXX(action->getFirstChild()->getNodeValue()).localForm();

        // 2004-10-13 ARW the DOM has been modified with the name of the
        // first collection, so we need to look for either "input()" [first
        // pass] or the collection(<previous container>) [subsequent passes]
        std::string inputString;
        std::string replacement;

        if(conIndex == 0) {
          inputString.assign("input()");
          replacement = "collection('" + currentContainerName + "')";
        } else {
          inputString = previousContainerName;
          replacement = currentContainerName;
        }
        unsigned int inputIndex = queryString.find(inputString, 0);
        while(inputIndex != std::string::npos) {
          queryString.replace(inputIndex, inputString.length(), replacement);
          inputIndex = queryString.find(inputString, 0);
        }

        action->setTextContent(CXXToXMLCh(queryString).unicodeForm());

        XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *result = doc->createElementNS(0, result_str);
        containerTime += doQuery(mgr, container, queryString, result);
        result->appendChild(doc->importNode(description, true));
        result->appendChild(doc->importNode(action, true));
        containerEle->appendChild(result);
      }

      std::string storageTypeStr(typeToString(container.getContainerType()));
      containerEle->setAttributeNS(0, type_str, CXXToXMLCh(storageTypeStr).unicodeForm());
      std::string sizeStr(getSizeFromName(container.getName()));
      containerEle->setAttributeNS(0, size_str, CXXToXMLCh(sizeStr).unicodeForm());

      containerEle->setAttributeNS(0, time_str, CXXToXMLCh(containerTime).unicodeForm());
      groupTime += containerTime;
      containerTime = 0;
      previousContainerName = currentContainerName;
    }
    groupEle->setAttributeNS(0, time_str, CXXToXMLCh(groupTime).unicodeForm());
    totalTime += groupTime;
    groupTime= 0;
  }
  doc->getDocumentElement()->setAttributeNS(0, time_str, CXXToXMLCh(totalTime).unicodeForm());
  writeNode(doc);
}

XERCES_CPP_NAMESPACE_QUALIFIER DOMElement* QueryRunner::getElementChildByName(const XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *parent,
                                                                 const XMLCh *name) const {

    XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *n = parent->getFirstChild();

    while(n != 0) {
        if(n->getNodeType() == XERCES_CPP_NAMESPACE_QUALIFIER DOMNode::ELEMENT_NODE &&
            XERCES_CPP_NAMESPACE_QUALIFIER XMLString::equals(n->getNodeName(), name)) {
            return static_cast<XERCES_CPP_NAMESPACE_QUALIFIER DOMElement* >(n);
        }
        n = n->getNextSibling();
    }

    return 0;
}

bool QueryRunner::handleError(const XERCES_CPP_NAMESPACE_QUALIFIER DOMError& domError)
{
    fSawErrors = true;
    if (domError.getSeverity() == XERCES_CPP_NAMESPACE_QUALIFIER DOMError::DOM_SEVERITY_WARNING)
	std::cerr << "\nWarning at file ";
    else if (domError.getSeverity() == XERCES_CPP_NAMESPACE_QUALIFIER DOMError::DOM_SEVERITY_ERROR)
        std::cerr << "\nError at file ";
    else
        std::cerr << "\nFatal Error at file ";

    std::cerr << XMLChToCXX(domError.getLocation()->getURI()).localForm()
         << ", line " << domError.getLocation()->getLineNumber()
         << ", char " << domError.getLocation()->getColumnNumber()
         << "\n  Message: " << XMLChToCXX(domError.getMessage()).localForm() << std::endl;

    return true;
}

void QueryRunner::resetErrors()
{
    fSawErrors = false;
}

double QueryRunner::doQuery(DbXml::XmlManager &mgr,
                          const DbXml::XmlContainer &container,
                          const std::string &query,
                          XERCES_CPP_NAMESPACE_QUALIFIER DOMElement* result) const
{
  double time = 0;
  try {
    DbXml::XmlQueryContext context = mgr.createQueryContext();
    context.setReturnType(DbXml::XmlQueryContext::DeadValues);
    context.setEvaluationType(DbXml::XmlQueryContext::Eager);
    clock_t start, finish;
    std::string fullQuery = "collection('" + container.getName() + "')" + query;
    start = clock();
    DbXml::XmlResults results(mgr.query(fullQuery, context));
    finish = clock();
    time = ((double) (finish-start) / CLOCKS_PER_SEC);
    result->setAttributeNS(0, time_str, CXXToXMLCh(time).unicodeForm());

    int count = results.size();
    result->setAttributeNS(0, count_str, CXXToXMLCh(count).unicodeForm());

    if(writeResults) {
      DbXml::XmlValue value;
      while(results.next(value)) {
        XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *resultItem = doc->createElementNS(0, resultitem_str);
        resultItem->setTextContent(CXXToXMLCh(value.asString()).unicodeForm());
        result->appendChild(resultItem);
      }
    }
  }
  catch(DbXml::XmlException &e)
    {
      XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *error = doc->createElementNS(0, error_str);
      error->setTextContent(CXXToXMLCh(e.what()).unicodeForm());
      result->appendChild(error);

    }
  catch(std::exception &e)
    {
      XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *error = doc->createElementNS(0, error_str);
      error->setTextContent(CXXToXMLCh(e.what()).unicodeForm());
      result->appendChild(error);
    }

  return time;
}

void QueryRunner::setOutputFilename(const std::string &filename) {
   outputFileName = CXXToXMLCh(filename, true).unicodeForm();
}

void usage()
{
        std::cerr <<   "This program performs various XPath queries against DBXML containers\n\n"

                  << "\t -h help\n"
                  << "\t -c config file\n"
                  << "\t -f output file (default is std out)\n"
                  << "\t -o include results in the XML output\n"
                  << "\t -d debug (written to stderr)\n"
                  << std::endl;
        exit( -1 );
}

int main(int argc, char **argv)
{
    std::string configFileName, outputFileName;
    bool writeResults = false;
    bool debug = false;

    for (int i=1; i<argc; i++) {
      if ( argv[i][0] == '-' ) {
          switch(argv[i][1]) {
            case 'c':
                if((i + 1)  < argc) {
                    configFileName = argv[++i];
                }
              break;
              case 'f':
                if((i + 1) < argc) {
                  outputFileName = argv[++i];
                }
              break;
            case 'o':
              writeResults = true;
              break;
            case 'd':
              debug = true;
              break;
            default:
              usage();
          }
       }
    }

    if(configFileName.empty()) {
        usage();
    }

    try {
      QueryRunner xqe(configFileName);
      xqe.setWriteResults(writeResults);
      xqe.setDebug(debug);
      if(!outputFileName.empty()) {
        xqe.setOutputFilename(outputFileName);
      }
      xqe.runTests();
    }
    catch(const XERCES_CPP_NAMESPACE_QUALIFIER XMLException &toCatch)
      {
        std::cerr << "Error while running QueryRunner.\n"
                                  << "  Xerces exception message: "
                  << XMLChToCXX(toCatch.getMessage()).localForm() << std::endl;
        return 1;
      }
    catch(const DbXml::XmlException &toCatch)
      {
        std::cerr << "Error while running QueryRunner.\n"
                  << " DB XML exception message: "
                  << toCatch.getExceptionCode()
                  << ":"
                  << (char*)toCatch.what()
                  << std::endl;
        return 1;
      }
    catch(...)
      {
        std::cerr << "Error while running QueryRunner.\n"
                  << "Unknown exception caught."
                  << std::endl;
        return 1;
      }

    return 0;
}

