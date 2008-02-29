//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: DbXmlDocAvailable.hpp,v 1.5 2006/10/30 17:45:55 bostic Exp $
//

#ifndef __DBXMLDOCAVAILABLE_HPP
#define	__DBXMLDOCAVAILABLE_HPP

#include <xqilla/ast/XQFunction.hpp>

namespace DbXml
{

class TransactedContainer;
class QueryPlan;
class ReferenceMinder;
class XmlManager;
class Transaction;

class DbXmlDocAvailable : public XQFunction
{
public:
	static const XMLCh name[];

	DbXmlDocAvailable(ReferenceMinder &minder, const VectorOfASTNodes &args, XPath2MemoryManager* memMgr);

	virtual ASTNode* staticResolution(StaticContext* context);
	virtual ASTNode *staticTyping(StaticContext *context);
	virtual Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;

	void generateQueryPlan(XmlManager &mgr, Transaction *txn, DynamicContext *context);

	const TransactedContainer *getContainer() const { return container_; }

	const QueryPlan *getQueryPlan() const {
		return qp_;
	}

private:
	const XMLCh *getUriArg(DynamicContext *context) const;

	QueryPlan *qp_;
	bool invalid_;

	TransactedContainer *container_;
	ReferenceMinder &minder_;
};

}

#endif
