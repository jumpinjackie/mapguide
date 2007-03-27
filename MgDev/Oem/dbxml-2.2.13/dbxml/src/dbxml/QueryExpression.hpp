//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: QueryExpression.hpp,v 1.24 2005/10/17 10:08:51 jsnelson Exp $
//

#ifndef __EXPRESSION_HPP
#define	__EXPRESSION_HPP

#include <string>
#include "dbxml/XmlQueryContext.hpp"
#include "ReferenceCounted.hpp"
#include "ReferenceMinder.hpp"

#include <pathan/XPath2MemoryManager.hpp>

class XPath2MemoryManager;
class DataItem;

class XQContext;
class XQQuery;

namespace DbXml
{

class Results;
class Optimizer;
class Value;
class Transaction;

class QueryExpression : public ReferenceCountedProtected
{
public:
	QueryExpression(const std::string &query,
		XmlQueryContext &context, Transaction *txn);
	virtual ~QueryExpression();
	
	const std::string &getQuery() const {
		return query_;
	}
	std::string getQueryPlan() const;
	
	XmlQueryContext &getContext() {
		return context_;
	}
	Results *execute(Transaction *txn, Value *contextItem,
			 XmlQueryContext &context, u_int32_t flags = 0);

	XQContext *getXQContext() const;
	XQQuery *getCompiledExpression() const;
	
protected:
	virtual Optimizer *createOptimizer(Transaction *txn);
	
	ReferenceMinder minder_;
	
private:
	QueryExpression(const QueryExpression&);
	QueryExpression &operator=(const QueryExpression &);
	
	std::string query_;
	XmlQueryContext context_;
	
	AutoRelease<XQContext> xqContext_;
	XQQuery *expr_;
};
	
}

#endif
