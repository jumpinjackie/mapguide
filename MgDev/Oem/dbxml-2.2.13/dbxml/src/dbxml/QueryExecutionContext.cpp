//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: QueryExecutionContext.cpp,v 1.7 2005/09/21 15:34:30 gmf Exp $
//

#include "DbXmlInternal.hpp"
#include "QueryExecutionContext.hpp"
#include <float.h>

using namespace DbXml;
using namespace std;

QueryExecutionContext::QueryExecutionContext(QueryContext &context, bool debugging)
	: container_(0),
	  context(context),
	  costToFilterPointer(0),
	  debugging_(debugging)
{}

QueryExecutionContext::~QueryExecutionContext()
{}

void QueryExecutionContext::setCostToFilterFunction(CostFunctionPointer cfp)
{
	costToFilterPointer = cfp;
}

double QueryExecutionContext::costToFilter(const IDS::SharedPtr &ids) const
{
	double r = DBL_MAX; // Very costly... keep refining candidate set.
	if (costToFilterPointer != 0) {
		r = (*costToFilterPointer)((!ids ? 0 : ids->size()));
	}
	return r;
}

double QueryExecutionContext::costToFilter(const IndexData::SharedPtr &data) const
{
	double r = DBL_MAX; // Very costly... keep refining candidate set.
	if (costToFilterPointer != 0) {
		r = (*costToFilterPointer)((!data ? 0 : data->size()));
	}
	return r;
}

string QueryExecutionContext::toString() const
{
	return "QueryExecutionContext"; // jcm - could provide a description for debugging
}

void QueryExecutionContext::addExecutionStep(const string &step)
{
  if(debugging_) {
    executionPath_ += step;
    executionPath_ += ",";
  }
}

string QueryExecutionContext::getExecutionPath() const
{
	return executionPath_;
}

