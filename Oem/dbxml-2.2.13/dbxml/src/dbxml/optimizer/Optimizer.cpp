//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: Optimizer.cpp,v 1.6 2005/08/12 14:54:36 jsnelson Exp $
//

#include <pathan/dataItem/DataItem.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/PathanException.hpp>
#include <pathan/exceptions/DSLException.hpp>

#include <xquery/XQQuery.hpp>

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

DataItem *Optimizer::startOptimize(DataItem *item)
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
	catch(const DSLException &e) {
		throw XmlException(
			XmlException::XPATH_PARSER_ERROR,
			XMLChToUTF8(e.getError()).str());
	}
	catch(const PathanException &e) {
		throw XmlException(
			XmlException::XPATH_PARSER_ERROR,
			XMLChToUTF8(e.getString()).str());
	}
}

DataItem *StaticResolver::optimize(DataItem *item)
{
	try {
		return item->staticResolution(xpc_);
	}
	catch(const DSLException &e) {
		throw XmlException(
			XmlException::XPATH_PARSER_ERROR,
			XMLChToUTF8(e.getError()).str());
	}
	catch(const PathanException &e) {
		throw XmlException(
			XmlException::XPATH_PARSER_ERROR,
			XMLChToUTF8(e.getString()).str());
	}
}
