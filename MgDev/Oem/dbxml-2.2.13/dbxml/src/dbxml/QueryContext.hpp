//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: QueryContext.hpp,v 1.47 2005/10/12 12:20:16 jsnelson Exp $
//

#ifndef __QUERYCONTEXT_HPP
#define	__QUERYCONTEXT_HPP

#include <string>
#include <map>
#include "NamespaceMap.hpp"
#include "VariableBindings.hpp"
#include "dbxml/XmlQueryContext.hpp"
#include "ReferenceCounted.hpp"
#include "dbxml/XmlManager.hpp"

#include <xercesc/dom/DOMBuilder.hpp>

class XPath2MemoryManager;
class XPathSelectionTreeParser;
class XPathNSResolver;
class XQContext;

namespace DbXml
{

class XmlValue;
class OperationContext;
class QueryExecutionContext;
class ReferenceMinder;
class Transaction;

class QueryContext : public ReferenceCounted
{
public:
	/// Constructor.
	QueryContext(XmlManager &mgr, XmlQueryContext::ReturnType rt, XmlQueryContext::EvaluationType et);
	virtual ~QueryContext();

	void setNamespace( const std::string &prefix, const std::string &uri );
	std::string getNamespace( const std::string &prefix );
	void removeNamespace( const std::string &prefix );
	void clearNamespaces();

	void setVariableValue(const std::string &name, const XmlResults &value);
	bool getVariableValue(const std::string &name, XmlResults &value) const;

	void setBaseURI(const std::string &baseURI);
	std::string getBaseURI() const { return baseURI_; }

	void setReturnType(XmlQueryContext::ReturnType rt = XmlQueryContext::LiveValues)
	{
		returnType_ = rt;
	}
	XmlQueryContext::ReturnType getReturnType() const
	{
		return returnType_;
	}
	void setEvaluationType(XmlQueryContext::EvaluationType et = XmlQueryContext::Eager)
	{
		evaluationType_ = et;
	}
	XmlQueryContext::EvaluationType getEvaluationType() const
	{
		return evaluationType_;
	}
	void setDefaultCollection(const std::string &uri)
	{
		defaultCollection_ = uri;
	}
	std::string getDefaultCollection() const
	{
		return defaultCollection_;
	}
	XmlManager &getManager()
	{
		return mgr_;
	}

	ReferenceMinder *getMinder() { return minder_; }
	void setMinder(ReferenceMinder *minder) { minder_ = minder; }
	u_int32_t getFlags() const { return db_flags_; }
	void setFlags(u_int32_t flags) { db_flags_ = flags; }

	QueryContext *copy() const
	{
		return new QueryContext(*this);
	}
	QueryContext *mergeIntoDynamicContext(const QueryContext &staticContext) const;

	// Private stuff.
	XQContext *createXQContext(Transaction *txn);
	XQContext *createDynamicXQContext(Transaction *txn, XQContext *staticContext);
	OperationContext &getOperationContext();
	void setTransaction(Transaction *txn);
	QueryExecutionContext &getQueryExecutionContext();

private:
	QueryContext(const QueryContext&);
	// no need for assignment
	QueryContext &operator=(const QueryContext &);

	NamespaceMap namespaces_;
	VariableBindings variables_;
	std::string baseURI_;
	XmlQueryContext::ReturnType returnType_;
	XmlQueryContext::EvaluationType evaluationType_;
	std::string defaultCollection_;
	XmlManager mgr_;

	// Evalution context
	OperationContext *operationContext_;
	QueryExecutionContext *qec_;
	ReferenceMinder *minder_;
	u_int32_t db_flags_;
};

}

#endif

