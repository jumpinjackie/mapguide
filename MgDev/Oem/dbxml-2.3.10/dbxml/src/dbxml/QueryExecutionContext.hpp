//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: QueryExecutionContext.hpp,v 1.11 2006/11/17 18:18:25 jsnelson Exp $
//

#ifndef __QUERYEXECUTIONCONTEXT_HPP
#define	__QUERYEXECUTIONCONTEXT_HPP

#include <string>

#include "IndexEntry.hpp"
#include "Statistics.hpp"

namespace DbXml
{

class Container;
class QueryContext;
class DbXmlContext;

/**
 * This function pointer declaration is used by the
 * QueryExecutionContext class to declare a callback
 * to a cost calculation function. There is a feedback
 * mechanism within the query plan execution engine
 * so that candidate set construction can be short
 * circuited once it becomes cheaper to filter.
 */
typedef double (*CostFunctionPointer)(long numDocuments);

/**
 * QueryExecutionContext
 *
 * Note that the costTofilter mechanism below is provided so that
 * during evaluation a query plan node can compare the cost of
 * refining the candidate set further, or giving up and allowing
 * the documents to be retrieved and filtered through a complete
 * XPath to DOM implementation.
 */
class QueryExecutionContext
{
public:
	QueryExecutionContext(QueryContext &context, bool debugging);
	~QueryExecutionContext();

	const Container &getContainer() const { return *container_; }
	void setContainer(const Container &container) { container_ = &container; }

	DbXmlContext *getDbXmlContext() { return dc_; }
	void setDbXmlContext(DbXmlContext *dc) { dc_ = dc; }

	StatisticsReadCache &getStatisticsReadCache() { return statsReadCache_; }
	IndexReadCache &getIndexReadCache() { return indexReadCache_; }

	void setCostToFilterFunction(CostFunctionPointer cfp);
	double costToFilter(const IDS::Ptr &ids) const;
	double costToFilter(const IndexData::Ptr &data) const;

	void addExecutionStep(const std::string &step);
	std::string getExecutionPath() const;

	QueryContext &getContext() const { return context; }

	std::string toString() const;
private:
	// no need for copy and assignment
	QueryExecutionContext(const QueryExecutionContext&);
	QueryExecutionContext &operator=(const QueryExecutionContext &);

	const Container *container_;
	StatisticsReadCache statsReadCache_;
	IndexReadCache indexReadCache_;

	QueryContext &context;
	DbXmlContext *dc_;

	CostFunctionPointer costToFilterPointer;

	// For debugging, and testing, keep track of the execution path.
	bool debugging_;
	std::string executionPath_;
};

}

#endif
