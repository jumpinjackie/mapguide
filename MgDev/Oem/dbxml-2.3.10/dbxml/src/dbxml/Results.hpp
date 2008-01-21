//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: Results.hpp,v 1.54 2006/10/30 17:45:53 bostic Exp $
//

#ifndef __RESULTS_HPP
#define	__RESULTS_HPP

#include <string>
#include "dbxml/XmlValue.hpp"
#include "dbxml/XmlQueryContext.hpp"
#include "dbxml/XmlQueryExpression.hpp"
#include "ReferenceCounted.hpp"
#include "Value.hpp"
#include "HighResTimer.hpp"
#include "ReferenceMinder.hpp"
#include "IndexEntry.hpp"
#include "Name.hpp"
#include "IndexSpecification.hpp"
#include "Cursor.hpp"
#include "QueryExecutionContext.hpp"

class DynamicContext;

namespace DbXml
{

class Container;

/**
 * Results super class
 */
class Results : public ReferenceCounted
{
public:
	Results() {}
	virtual ~Results() {}

	virtual int next(XmlValue &value) = 0;
	virtual bool hasNext() = 0;
	virtual int peek(XmlValue &value) = 0;
	virtual int previous(XmlValue &value) = 0;
	virtual bool hasPrevious() = 0;
	virtual int reset() = 0;
	virtual void add(const XmlValue &value) = 0;
	virtual size_t size() const = 0;
	virtual bool isLazy() const { return false; }
private:
	// no need for copy and assignment
	Results(const Results&);
	Results &operator=(const Results &);
};

/**
 * Lazily evaluated results
 */
class LazyResults : public Results
{
public:
	virtual void add(const XmlValue &value);
	virtual void add(const XmlValueVector &vv);
	virtual size_t size() const;
	virtual bool isLazy() const { return true; }
};

/**
 * Results from a lazily evaluated expression
 */
class LazyDIResults : public LazyResults
{
public:
	LazyDIResults(QueryContext *context, Value *contextItem, QueryExpression &expr, Transaction *txn, u_int32_t flags);
	~LazyDIResults();

	virtual int next(XmlValue &value);
	virtual bool hasNext();
	virtual int peek(XmlValue &value);
	virtual int previous(XmlValue &value);
	virtual bool hasPrevious();
	virtual int reset();

protected:
	XmlQueryContext context_;
	XmlQueryExpression expr_;
	XmlValue contextItem_;
	bool lazyDocs_;

	HighResTimer timer_;
	ReferenceMinder evaluationMinder_;
	QueryExecutionContext qec_;

	DynamicContext *xqc_;
	Result result_;
	Item::Ptr nextItem_;
};

/**
 * Results from a lazily manifested IndexData
 */
class LazyIndexResults : public LazyResults
{
public:
	LazyIndexResults(Container &container,
			 QueryContext *context,
			 Transaction *txn, const Index &index,
			 const IndexLookup &il,
			 u_int32_t flags);

	virtual int next(XmlValue &value);
	virtual bool hasNext();
	virtual int peek(XmlValue &value);
	virtual int previous(XmlValue &value);
	virtual bool hasPrevious();
	virtual int reset();
private:
	virtual int doNext(XmlValue &value, bool isPeek);

protected:
	XmlQueryContext context_;
	XmlContainer container_;
	
	DbWrapper::Operation lowOp_;
	Key lowKey_;
	DbWrapper::Operation highOp_;
	Key highKey_;
	bool reverse_;
	bool docOnly_;
	bool cacheDocuments_;

	ScopedPtr<IndexCursor> cursor_;
	IndexEntry ie_;
	ReferenceMinder evaluationMinder_;
};

/**
 * Results to hold an eagerly evaluated set of values
 */
class ValueResults : public Results
{
public:
	ValueResults();
	ValueResults(const XmlValue &value);
	ValueResults(Results *resultsToAdopt);
	virtual ~ValueResults();

	virtual int next(XmlValue &value);
	virtual bool hasNext();
	virtual int peek(XmlValue &value);
	virtual int previous(XmlValue &value);
	virtual bool hasPrevious();
	virtual int reset();
	virtual void add(const XmlValue &value);
	virtual void add(const XmlValueVector &vv);
	virtual size_t size() const;
private:
	virtual int doNext(XmlValue &value, bool isPeek);

protected:
	// For Eager evaluation we copy the values into a vector.
	DbXml::XmlValueVector vv_;
	DbXml::XmlValueVector::iterator *vvi_;
};

}

#endif

