//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: QueryExpression.hpp,v 1.29 2006/10/30 17:45:53 bostic Exp $
//

#ifndef __EXPRESSION_HPP
#define	__EXPRESSION_HPP

#include <string>
#include "dbxml/XmlQueryContext.hpp"
#include "ReferenceCounted.hpp"
#include "ReferenceMinder.hpp"
#include "QueryExecutionContext.hpp"

#include <xqilla/framework/XPath2MemoryManager.hpp>

class XPath2MemoryManager;
class ASTNode;

class DynamicContext;
class XQQuery;

namespace DbXml
{

class Results;
class Optimizer;
class Value;
class Transaction;
class DbXmlContext;

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

	DbXmlContext *getDynamicContext() const;
	XQQuery *getCompiledExpression() const;
	
protected:
	virtual Optimizer *createOptimizer(Transaction *txn);
	
	ReferenceMinder minder_;
	
private:
	QueryExpression(const QueryExpression&);
	QueryExpression &operator=(const QueryExpression &);
	
	std::string query_;
	XmlQueryContext context_;
	QueryExecutionContext qec_;
	
	AutoDelete<DbXmlContext> xqContext_;
	XQQuery *expr_;
};
	
}

#endif
