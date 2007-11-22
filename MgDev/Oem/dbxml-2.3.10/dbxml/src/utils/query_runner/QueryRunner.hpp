//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: QueryRunner.hpp,v 1.10 2006/10/30 17:46:06 bostic Exp $
//

#ifndef QUERYRUNNER_HPP
#define QUERYRUNNER_HPP

#include <string>
#include <vector>
#include <xercesc/util/XercesDefs.hpp>
#include <xercesc/dom/DOMErrorHandler.hpp>

XERCES_CPP_NAMESPACE_BEGIN
class DOMImplementation;
class DOMDocument;
class DOMNodeList;
class DOMWriter;
class DOMElement;
class DOMNode;
class XMLException;
class DOMBuilder;
XERCES_CPP_NAMESPACE_END

namespace DbXml {
class XmlManager;
class XmlContainer;
class XmlQueryContext;
}

class QueryRunner : public XERCES_CPP_NAMESPACE_QUALIFIER DOMErrorHandler {

public:

    /**
     * @param configFile the name of the configuration file
     */
    QueryRunner(const std::string &configFile);

    ~QueryRunner();

    /**
    * Runs the tests found in the configuraiton file against the containers specified there
    */
    void runTests() const;

    /**
    * @param the filename to output the xml to
    */
    void setOutputFilename(const std::string &filename);

    /**
     * @param write if true then include the results of the queries in the output XML
     */
    void setWriteResults(bool write);

    /**
     * @param debug if true then dump debug diagnostics to error stream
     */
    void setDebug(bool debug);

private:
    void writeNode(const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *n) const;
    void initialise();
    void parseConfigFile(const std::string &configFile);
    double doQuery(DbXml::XmlManager &db, 
                   const DbXml::XmlContainer &container, 
                   const std::string &query, 
                   XERCES_CPP_NAMESPACE_QUALIFIER DOMElement* result) const;

    XERCES_CPP_NAMESPACE_QUALIFIER DOMElement* getElementChildByName(const XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *parent,
                                                                 const XMLCh *name) const;
 

    XERCES_CPP_NAMESPACE_QUALIFIER DOMImplementation *impl;
    XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc;
    XERCES_CPP_NAMESPACE_QUALIFIER DOMWriter *writer;
    XERCES_CPP_NAMESPACE_QUALIFIER DOMBuilder *parser;

    bool    fSawErrors;

    class QueryGroup {
    public:
        QueryGroup(const XMLCh *n): name(n){};
        XERCES_CPP_NAMESPACE_QUALIFIER DOMNodeList *containers;
        XERCES_CPP_NAMESPACE_QUALIFIER DOMNodeList *queries;
        const XMLCh *name;
    };

    std::vector<QueryGroup > queryGroups;
    const XMLCh *databaseName;
    const XMLCh *outputFileName;
    bool writeResults;
    bool debug;

public:
    // -----------------------------------------------------------------------
    //  Getter methods
    // -----------------------------------------------------------------------
    bool getSawErrors() const;

    // -----------------------------------------------------------------------
    //  Implementation of the DOM ErrorHandler interface
    // -----------------------------------------------------------------------
    bool handleError(const XERCES_CPP_NAMESPACE_QUALIFIER DOMError& domError);
    void resetErrors();
};

#endif
