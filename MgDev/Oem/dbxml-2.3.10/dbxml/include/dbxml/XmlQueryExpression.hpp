//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: XmlQueryExpression.hpp,v 1.36 2006/10/30 17:45:48 bostic Exp $
//

#ifndef __XPATHEXPRESSION_HPP
#define __XPATHEXPRESSION_HPP

#include "DbXmlFwd.hpp"
#include "XmlPortability.hpp"
#include <db_cxx.h>
#include <string>

namespace DbXml
{

class QueryExpression;

class DBXML_EXPORT XmlQueryExpression
{
public:
	/** @name For Reference Counting */
	XmlQueryExpression();
	XmlQueryExpression(const XmlQueryExpression&);
	XmlQueryExpression &operator=(const XmlQueryExpression &);
	virtual ~XmlQueryExpression();
	bool isNull() const { return expression_ == 0; }

	/** @name Information Methods */
	const std::string &getQuery() const;
	std::string getQueryPlan() const;

	/** @name Execute Methods */

	/**
	 * Valid flags:
	 * DB_READ_UNCOMMITTED, DB_RMW, DB_READ_COMMITTED, DBXML_LAZY_DOCS,
	 * DB_TXN_SNAPSHOT
	 */
	XmlResults execute(XmlQueryContext &context, u_int32_t flags = 0) const;
	/**
	 * Valid flags:
	 * DB_READ_UNCOMMITTED, DB_RMW, DB_READ_COMMITTED, DBXML_LAZY_DOCS,
	 * DB_TXN_SNAPSHOT
	 */
	XmlResults execute(const XmlValue &contextItem, XmlQueryContext &context, u_int32_t flags = 0) const;

	/** @name Execute Methods (Transacted) */

	/**
	 * Valid flags:
	 * DB_READ_UNCOMMITTED, DB_RMW, DB_READ_COMMITTED, DBXML_LAZY_DOCS,
	 * DB_TXN_SNAPSHOT
	 */
	XmlResults execute(XmlTransaction &txn, XmlQueryContext &context, u_int32_t flags = 0) const;
	/**
	 * Valid flags:
	 * DB_READ_UNCOMMITTED, DB_RMW, DB_READ_COMMITTED, DBXML_LAZY_DOCS,
	 * DB_TXN_SNAPSHOT
	 */
	XmlResults execute(XmlTransaction &txn, const XmlValue &contextItem, XmlQueryContext &context, u_int32_t flags = 0) const;

	/** @name Private Methods (for internal use) */
	// @{

	XmlQueryExpression(QueryExpression *expression);
	operator QueryExpression &() const
	{
		return *expression_;
	}
	operator QueryExpression *() const
	{
		return expression_;
	}

	//@}

private:
	QueryExpression *expression_;
};

}

#endif

