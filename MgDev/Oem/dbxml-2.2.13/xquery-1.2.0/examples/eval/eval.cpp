//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2004-2005
//	Sleepycat Software.  All rights reserved.
// Copyright (c) 2004-2005
//	Progress Software Corporation.  All rights reserved.
//
// $Id: eval.cpp,v 1.21 2005/11/21 23:16:52 gmf Exp $
//
#include <xquery/XQEvaluator.hpp>
#include <xquery/XQEvaluator.hpp>
#include <xquery/context/XQContextImpl.hpp>
#include <xquery/XQQuery.hpp>
#include <xquery/XQRemoteDebugger.hpp>
#include <xquery/XQException.hpp>

#include <iostream>
#include <vector>
#include <stdlib.h>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/XMLUniDefs.hpp>
#include <xercesc/util/XMLUni.hpp>
#include <xercesc/framework/XMLBuffer.hpp>
#include <xercesc/framework/StdOutFormatTarget.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>

//Pathan includes
#include <pathan/PathanException.hpp>
#include <pathan/PathanPlatformUtils.hpp>
#include <pathan/Node.hpp>
#include <pathan/AnyAtomicType.hpp>
#include <pathan/Sequence.hpp>
#include <pathan/exceptions/DSLException.hpp>
#include <pathan/internal/factory/DatatypeFactory.hpp>
#include <pathan/internal/XPath2MemoryManagerImpl.hpp>
#include <pathan/internal/items/impl/NodeImpl.hpp>


#define XERCES XERCES_CPP_NAMESPACE_QUALIFIER

#include <xercesc/util/XercesDefs.hpp>

////////////////////////////
// function declarations  //
////////////////////////////

/** Print usage */
void usage();

const char* inputFile=NULL, *outputFile=NULL, *host=NULL, *baseURIDir=NULL;
bool bRemoteDebug=false;

int main(int argc, char *argv[])
{
  // First we parse the command line arguments
  std::vector<char *> queries;
  int numberOfTimes = 1;
  bool quiet = false;
  for(int i = 1; i < argc; ++i) {
    if(*argv[i] == '-' && argv[i][2] == '\0' ){

      // -h option, print usage
      if(argv[i][1] == 'h') {
        usage();
        return 0;
      }
      else if(argv[i][1] == 'i') {
        i++;
        if(i==argc)
        {
          std::cerr << "Missing argument to option 'i'" << std::endl;
          return 1;
        }
        inputFile=argv[i];
      }
      else if(argv[i][1] == 'b') {
        i++;
        if(i==argc)
        {
          std::cerr << "Missing argument to option 'b'" << std::endl;
          return 1;
        }
        baseURIDir=argv[i];
      }
      else if(argv[i][1] == 'o') {
        i++;
        if(i==argc)
        {
          std::cerr << "Missing argument to option 'o'" << std::endl;
          return 1;
        }
        outputFile=argv[i];
      }
      else if(argv[i][1] == 'd') {
        bRemoteDebug=true;
        i++;
        if(i==argc)
        {
          std::cerr << "Missing argument to option 'd'" << std::endl;
          return 1;
        }
        host=argv[i];
      }
      else if(argv[i][1] == 'n') {
        i++;
        if(i==argc)
        {
          std::cerr << "Missing argument to option 'n'" << std::endl;
          return 1;
        }
        numberOfTimes=atoi(argv[i]);
      }
      else if(argv[i][1] == 'q') {
        quiet = true;
      }
      else {
        usage();
        return 1;
      }
    }
    else {
      queries.push_back(argv[i]);
    }
  }

  // Check for bad command line arguments
  if(queries.empty()) {
    usage();
    return 1;
  }

  // initialisation of Pathan
  try {
    PathanPlatformUtils::initialize(NULL);
  } catch (const XERCES XMLException& eXerces) {
    char *pMsg = XERCES XMLString::transcode(eXerces.getMessage());
    std::cerr << "Error during Xerces-C initialisation.\n"
              << "Xerces exception message: "
              << pMsg << std::endl;
    delete pMsg;
    return 1;
  }

  XPath2MemoryManager* memMgr = new XPath2MemoryManagerImpl;
  XQRemoteDebugger* remoteDebugger=NULL;

  XMLCh wszQuery[8000];

  int executionCount = 0;
  try
  {
    std::vector<std::pair<XQContext*, XQQuery*> > parsedQueries;
    for(std::vector<char*>::iterator it1 = queries.begin();
        it1 != queries.end(); ++it1) {
      XQContext *context = new (memMgr) XQContextImpl(memMgr);

      XERCES XMLString::transcode(*it1, wszQuery, 8000);

      // the XQContext has set the baseURI to the current file
      // we override to a user-specified value, or to the same directory as the
      // query (current file)
      if(baseURIDir != NULL) {
        XMLCh wszBaseURIDir[1024];
        XERCES XMLString::transcode(baseURIDir, wszBaseURIDir, 1024);
        context->setBaseURI(wszBaseURIDir);
      } else {
        // FIXME assumes UTF8, Windows portability issues?
        char *pwd = ::getenv("PWD");
        if(pwd != NULL) {
          std::string queryPath(*it1);
          size_t idx = queryPath.rfind('/');
          if(idx != std::string::npos) {
            std::string baseURI = std::string("file:");
            baseURI += std::string(pwd);
            baseURI += std::string(1, '/');
            baseURI += queryPath.substr(0, idx);
            baseURI += std::string(1, '/');
            context->setBaseURI(XERCES XMLString::transcode(baseURI.c_str()));
          }
        }
      }

      if(bRemoteDebug)
        {
          remoteDebugger=new XQRemoteDebugger(X(host),memMgr);
          context->setDebugCallback(remoteDebugger);
          context->enableDebugging(true);
        }
      XQQuery* pQuery=XQEvaluator::parseFromURI(wszQuery, context);
      parsedQueries.push_back(std::pair<XQContext*, XQQuery*>(context, pQuery));
    }

    for(int count = numberOfTimes; count > 0; --count) {

      for(std::vector<std::pair<XQContext*, XQQuery*> >::iterator it2 = parsedQueries.begin();
          it2 != parsedQueries.end(); ++it2) {

        AutoRelease<XQContext> dynamic_context(static_cast<XQContext*>(it2->first->createDynamicContext()));
        if(inputFile != NULL) {
          // if an XML file was specified
          Sequence seq=dynamic_context->resolveDocument(X(inputFile));
          if(!seq.isEmpty() && seq.first()->isNode()) {
            dynamic_context->setContextItem(seq.first());
            dynamic_context->setContextPosition(1);
            dynamic_context->setContextSize(1);
          }
        }
        time_t now;
        dynamic_context->setCurrentTime(time(&now));

        Sequence result = it2->second->evaluate(dynamic_context).toSequence(dynamic_context);
        ++executionCount;

        if(!quiet) {
          // serializer used for nodes only
          XERCES DOMWriter* serializer = XERCES DOMImplementation::getImplementation()->createDOMWriter();
          XERCES XMLFormatTarget* target = NULL;

          // use STDOUT if a file was not specified
          bool toFile(false);
          if(outputFile != NULL) {
            target = new XERCES LocalFileFormatTarget(outputFile);
            toFile = true;
          } else {
            target = new XERCES StdOutFormatTarget();
          }

          // assume UTF8
          XERCES XMLFormatter *formatter = new XERCES XMLFormatter(XERCES XMLUni::fgUTF8EncodingString, target);

          for(Sequence::iterator it=result.begin();it!=result.end();it++)
            {
              if((*it)->isNode() && (*it)->getInterface(Item::gPathan) != 0) {
                const NodeImpl *nodeImpl = (const NodeImpl*)(*it)->getInterface(Item::gPathan);
                serializer->writeNode(target, *(nodeImpl->getDOMNode()));
                // separate nodes with \n
                target->writeChars((const XMLByte*)"\n", 1, formatter);
              } else {
                char* str = XERCES XMLString::transcode((*it)->asString(dynamic_context));
                target->writeChars((const XMLByte*)str, strlen(str), formatter);
                XERCES XMLString::release(&str);
              }
            }

          serializer->release();
          delete formatter;
          delete target;
        }

      }
    }

    for(std::vector<std::pair<XQContext*, XQQuery*> >::iterator it2 = parsedQueries.begin();
        it2 != parsedQueries.end(); ++it2) {
      it2->first->release();
    }
  }
  catch(XQException &e)
  {
    std::cerr << "Caught XQException:" << std::endl << XERCES XMLString::transcode(e.getError()) << std::endl;
    std::cerr << "at " << XERCES XMLString::transcode(e.m_szFile) << ":" << e.m_nLine << ":" << e.m_nColumn << std::endl;
    return 1;
  }
  catch(DSLException &e)
  {
    std::cerr << "Caught DSLException:" << std::endl << XERCES XMLString::transcode(e.getError()) << std::endl;
    return 1;
  }
  catch(PathanException &e)
  {
    std::cerr << "Caught Pathan Exception:" << std::endl << XERCES XMLString::transcode(e.getString()) << std::endl;
    return 1;
  }
  catch(XERCES DOMException &e)
  {
    std::cerr << "Caught Xerces DOM Exception:" << std::endl << XERCES XMLString::transcode(e.msg) << std::endl;
    return 1;
  }
  catch(...)
  {
    std::cerr << "Caught unknown exception" << std::endl;
    return 1;
  }

  if(quiet) std::cout << "Executions: " << executionCount << std::endl;

  // clean up and exit
  if(remoteDebugger)
    delete remoteDebugger;
  delete memMgr;
  PathanPlatformUtils::terminate();

  return 0;
}

// print the usage message
void usage()
{
  std::cerr << "Usage: eval [options] <XQuery file>..." << std::endl << std::endl;
  std::cerr << "-q             : Quiet mode - no output" << std::endl;
  std::cerr << "-n <number>    : Run the queries a number of times" << std::endl;
  std::cerr << "-i <file>      : Load XML document and bind it as the context item" << std::endl;
  std::cerr << "-b <baseURI>   : Set the base URI for the XPath2 context" << std::endl;
  std::cerr << "-o <file>      : Write the result to the specified file" << std::endl;
  std::cerr << "-h             : Show this display" << std::endl;
  std::cerr << "-d <host:port> : Enable remote debugging" << std::endl;
}
