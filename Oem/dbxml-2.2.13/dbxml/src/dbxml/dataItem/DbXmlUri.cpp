//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: DbXmlUri.cpp,v 1.12 2005/09/22 17:26:50 jsnelson Exp $
//

#include "DbXmlInternal.hpp"
#include "DbXmlUri.hpp"
#include "../UTF8.hpp"
#include "../Globals.hpp"
#include "../Manager.hpp"
#include "../OperationContext.hpp"
#include "../scoped_ptr.hpp"
#include <dbxml/XmlContainer.hpp>
#include <dbxml/XmlTransaction.hpp>
#include <dbxml/XmlException.hpp>

#include <xercesc/util/XMLUri.hpp>

#include <pathan/XPath2Utils.hpp>
#include <pathan/exceptions/FunctionException.hpp>

#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

using namespace DbXml;
using namespace std;

const char *DbXmlUri::dbxmlScheme = "dbxml:";
int DbXmlUri::dbxmlSchemeLen = 6;

DbXmlUri::DbXmlUri(const std::string &uri, bool documentUri)
	: valid_(false)
{
	parseUri(0, UTF8ToXMLCh(uri).str(), documentUri);
}

DbXmlUri::DbXmlUri(const XMLCh *uri, bool documentUri)
	: valid_(false)
{
	parseUri(0, uri, documentUri);
}

DbXmlUri::DbXmlUri(const std::string &baseUri, const std::string &relativeUri,
		   bool documentUri)
	: valid_(false)
{
	parseUri(UTF8ToXMLCh(baseUri).str(), UTF8ToXMLCh(relativeUri).str(),
		 documentUri);
}

DbXmlUri::DbXmlUri(const XMLCh *baseUri, const XMLCh *relativeUri,
		   bool documentUri)
	: valid_(false)
{
	parseUri(baseUri, relativeUri, documentUri);
}

// throws if it doesn't exist
XmlContainer DbXmlUri::openContainer(XmlManager &mgr, Transaction *txn) const
{
	Manager &mgrImpl = (Manager &)mgr;

	if (mgrImpl.allowAutoOpen()) {
		if(txn) {
			XmlTransaction childTxn = txn->createChild();
			XmlContainer container(
				mgrImpl.openContainer(
					containerName_, childTxn,
					mgrImpl.getDefaultContainerFlags(),
					mgrImpl.getDefaultContainerType(), 0));
			childTxn.commit();
			return container;
		} else {
			return mgrImpl.openContainer(
				containerName_, 0,
				mgrImpl.getDefaultContainerFlags(),
				mgrImpl.getDefaultContainerType(), 0);
		}
	} else {
		// container must be open
		TransactedContainer *cont =
			mgrImpl.getOpenContainer(containerName_);
		if (cont)
			return cont;
		std::string msg = "Cannot query in container dbxml:";
		msg += containerName_;
		msg += ".  Container not open and auto-open is not enabled";
		throw XmlException(
			XmlException::CONTAINER_CLOSED, msg);
	}
}

XmlDocument DbXmlUri::openDocument(XmlManager &mgr, Transaction *txn) const
{
	XmlContainer cont(openContainer(mgr, txn));
	XmlDocument doc;

	OperationContext oc(txn);
	((Container&)cont).getDocument(oc, documentName_, doc, DBXML_LAZY_DOCS);

	return doc;
}

static bool isAbsoluteURI(const std::string &uri)
{
	// return true if the relativeUri starts with "xxxx:"
	// NOTE: no validation of this scheme string is done
	// If the scheme is one-character (e.g. C:), return false,
	// and treat it as as "relative" URI, which results in
	// putting "dbxml:" in front of it.
	int colonPos = uri.find(":");
	if (colonPos >= 2) {
		int sep1Pos = uri.find("/");
		int sep2Pos = uri.find("\\");
		if ((sep1Pos == -1 && sep2Pos == -1) ||
		    ((colonPos < sep1Pos) ||
		     (colonPos < sep2Pos)))
			return true;
	}
	return false;
}

// For DbXml uri's, not much validation -- just recognize
// the dbxml: scheme.
// If the dbxml: is followed immediately by something like http:
// implying that there's an absolute URI passed, make it invalid.
// If it's followed by dbxml: (e.g. dbxml:dbxml:...), just
// skip one of them.
void DbXmlUri::parseUri(const XMLCh *baseUri, const XMLCh *relativeUri,
			bool documentUri)
{
	try {
		std::string relative = XMLChToUTF8(relativeUri).str();
		std::string uri;
		if (isAbsoluteURI(relative)) {
			uri = relative;
		} else {
			if(baseUri != 0)
				uri = XMLChToUTF8(baseUri).str();
			uri += relative;
		}

		resolvedUri_ = uri;

		/*
		 * dbxml: URI is assumed to be local.  If we
		 * ever want to handle "dbxml://remote_host/path syntax,
		 * something more needs to be done.
		 */
		if (uri.find(dbxmlScheme) == 0) {
			valid_ = true;
			// If the first character is slash, treat
			// path as absolute; otherwise, it is relative.
			// I.e. slash is significant
			// If this is a document URI, the doc name
			// is the final component of the path.
			// This means that document names cannot
			// include '/' or '\' separators.
			int pos = -1;
			if(documentUri) {
				pos = uri.rfind('/');
				if (pos == -1)
					pos = uri.rfind('\\');
			}

			if(pos != -1) {
				containerName_.assign(uri, dbxmlSchemeLen,
						      pos - dbxmlSchemeLen);
				documentName_.assign(uri, pos + 1,
						     uri.size() - pos);
			} else {
				containerName_.assign(uri, dbxmlSchemeLen,
						      uri.size() -
						      dbxmlSchemeLen);
			}
		}
	}
	catch(...)  {
		// don't throw; it's just that the URI isn't valid.
		// But...it may be the only thing that can throw now is
		// a bad character in the string.  TBD.
	}
}

// validate a Base URI.  Just needs to be:
//   scheme:
// Special URI characters, such as '/' '?' and '#'
// cannot precede the colon.  Scheme must be
// more than 1 character.
// static
bool DbXmlUri::isValidBase(const std::string &base)
{
	std::string copy = base;
	int colIndex = copy.find(':');
	int sIndex = copy.find('/');
	int qIndex = copy.find('?');
	int pIndex = copy.find('#');
	if ((colIndex < 2) ||
	    (colIndex > sIndex && sIndex != -1) ||
	    (colIndex > qIndex && qIndex != -1) ||
	    (colIndex > pIndex && pIndex != -1))
		return false;
	return true;
}
