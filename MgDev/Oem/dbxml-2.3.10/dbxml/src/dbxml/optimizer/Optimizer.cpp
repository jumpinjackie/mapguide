//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: Optimizer.cpp,v 1.12.2.1 2007/01/05 15:35:53 jsnelson Exp $
//

#include <xqilla/ast/ASTNode.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/exceptions/XQillaException.hpp>
#include <xqilla/exceptions/XQException.hpp>
#include <xqilla/simple-api/XQQuery.hpp>
#include <xqilla/exceptions/QueryInterruptedException.hpp>
#include <xqilla/exceptions/QueryTimeoutException.hpp>

#include "Optimizer.hpp"
#include "../UTF8.hpp"
#include "../dataItem/DbXmlContext.hpp"
#include "dbxml/XmlException.hpp"

using namespace DbXml;
using namespace std;

Optimizer::Optimizer(Optimizer *parent)
  : parent_(parent)
{}

Optimizer::~Optimizer()
{
	if(parent_ != 0) {
		delete parent_;
	}
}

void Optimizer::startOptimize(XQQuery *query)
{
	if(parent_ != 0) {
		parent_->startOptimize(query);
	}
	
	optimize(query);
}

ASTNode *Optimizer::startOptimize(ASTNode *item)
{
	if(parent_ != 0) {
		item = parent_->startOptimize(item);
	}
	return optimize(item);
}

void Optimizer::reset()
{
	if(parent_ != 0) {
		parent_->reset();
	}
	resetInternal();
}

void StaticResolver::optimize(XQQuery *query)
{
	try {
		query->staticResolution(CAST_TO_DBXMLCONTEXT(xpc_));
	}
	catch(QueryInterruptedException &ie) {
		throw XmlException(XmlException::OPERATION_INTERRUPTED,
			"Query was interrupted by the application");
	}
	catch(QueryTimeoutException &ie) {
		throw XmlException(XmlException::OPERATION_TIMEOUT,
			"Query timed out");
	}
	catch(const XQException &e) {
		throw XmlException(XmlException::XPATH_PARSER_ERROR, e);
	}
	catch(const XQillaException &e) {
		throw XmlException(
			XmlException::XPATH_PARSER_ERROR,
			XMLChToUTF8(e.getString()).str());
	}
}

ASTNode *StaticResolver::optimize(ASTNode *item)
{
	try {
		return item->staticResolution(xpc_);
	}
	catch(QueryInterruptedException &ie) {
		throw XmlException(XmlException::OPERATION_INTERRUPTED,
			"Query was interrupted by the application");
	}
	catch(QueryTimeoutException &ie) {
		throw XmlException(XmlException::OPERATION_TIMEOUT,
			"Query timed out");
	}
	catch(const XQException &e) {
		throw XmlException(XmlException::XPATH_PARSER_ERROR, e);
	}
	catch(const XQillaException &e) {
		throw XmlException(
			XmlException::XPATH_PARSER_ERROR,
			XMLChToUTF8(e.getString()).str());
	}
}

void StaticTyper::optimize(XQQuery *query)
{
	try {
		const ImportedModules &modules = query->getImportedModules();
		for(ImportedModules::const_iterator it = modules.begin();
		    it != modules.end(); ++it) {
			(*it)->staticTyping(const_cast<DynamicContext*>((*it)->getStaticContext()));
		}

		query->staticTyping(CAST_TO_DBXMLCONTEXT(xpc_));
	}
	catch(QueryInterruptedException &ie) {
		throw XmlException(XmlException::OPERATION_INTERRUPTED,
			"Query was interrupted by the application");
	}
	catch(QueryTimeoutException &ie) {
		throw XmlException(XmlException::OPERATION_TIMEOUT,
			"Query timed out");
	}
	catch(const XQException &e) {
		throw XmlException(XmlException::XPATH_PARSER_ERROR, e);
	}
	catch(const XQillaException &e) {
		throw XmlException(
			XmlException::XPATH_PARSER_ERROR,
			XMLChToUTF8(e.getString()).str());
	}
}

ASTNode *StaticTyper::optimize(ASTNode *item)
{
	try {
		return item->staticTyping(xpc_);
	}
	catch(QueryInterruptedException &ie) {
		throw XmlException(XmlException::OPERATION_INTERRUPTED,
			"Query was interrupted by the application");
	}
	catch(QueryTimeoutException &ie) {
		throw XmlException(XmlException::OPERATION_TIMEOUT,
			"Query timed out");
	}
	catch(const XQException &e) {
		throw XmlException(XmlException::XPATH_PARSER_ERROR, e);
	}
	catch(const XQillaException &e) {
		throw XmlException(
			XmlException::XPATH_PARSER_ERROR,
			XMLChToUTF8(e.getString()).str());
	}
}
