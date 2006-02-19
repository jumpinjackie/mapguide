//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: DbXmlUri.hpp,v 1.3 2005/04/05 16:44:04 bostic Exp $
//

#ifndef __DBXMLURI_HPP
#define	__DBXMLURI_HPP

#include <string>
#include <xercesc/util/XMLUni.hpp>

namespace DbXml
{

class XmlManager;
class Transaction;
class XmlContainer;
class XmlDocument;

/**
 * A class that encapsulates the parsing and understanding
 * of a DbXml scheme uri.
 */
class DbXmlUri
{
public:
	DbXmlUri(const std::string &uri, bool documentUri);
	DbXmlUri(const std::string &baseUri, const std::string &relativeUri, bool documentUri);
	DbXmlUri(const XMLCh *uri, bool documentUri);
	DbXmlUri(const XMLCh *baseUri, const XMLCh *relativeUri, bool documentUri);

	bool isValid() const { return valid_; }

	const std::string &getResolvedUri() const { return resolvedUri_; }
	const std::string &getContainerName() const { return containerName_; }
	const std::string &getDocumentName() const { return documentName_; }

	XmlContainer openContainer(XmlManager &mgr, Transaction *txn) const;
	XmlDocument openDocument(XmlManager &mgr, Transaction *txn) const;

	// validate a Base URI
	static bool isValidBase(const std::string &baseUri);

	// globals
	static const char *dbxmlScheme;
	static int dbxmlSchemeLen;
private:
	void parseUri(const XMLCh *baseUri, const XMLCh *relativeUri, bool documentUri);

	bool valid_;
	std::string resolvedUri_;
	std::string containerName_;
	std::string documentName_;
};

}

#endif

