//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: DbXmlContext.hpp,v 1.7 2005/08/11 12:26:34 jsnelson Exp $
//

#ifndef __DBXMLCONTEXT_HPP
#define	__DBXMLCONTEXT_HPP

#include <xquery/XQContext.hpp>
#include "DbXmlURIResolver.hpp"
#include "../QueryContext.hpp"

#define CAST_TO_CONST_DBXMLCONTEXT(x) (static_cast<const DbXmlContext*>(x))
#define CAST_TO_DBXMLCONTEXT(x) (static_cast<DbXmlContext*>(x))

namespace DbXml
{

class DbXmlContext : public XQContext
{
public:
	enum whichType {
		DBXML_COMPARE = XQContext::ORDERING_CHANGE + 20,
		DBXML_FILTER,
		DBXML_CONTAINS
	};

	DbXmlContext(QueryContext &qc, Transaction *txn)
		: qc_(&qc), resolver_(qc.getManager(), txn) {}
	virtual ~DbXmlContext() {}

	QueryContext &getQueryContext() const { return *qc_; }
	void setQueryContext(QueryContext &qc) { qc_ = &qc; }
	void setTransaction(Transaction *txn) {
		resolver_.setTransaction(txn);
	}

	// these are required by DynamicContext (pathan)
	virtual void trace(const XMLCh* message1, const XMLCh* message2);

protected:
	QueryContext *qc_;
	DbXmlURIResolver resolver_;
};

}

#endif
