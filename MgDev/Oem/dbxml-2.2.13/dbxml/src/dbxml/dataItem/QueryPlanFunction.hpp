//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: QueryPlanFunction.hpp,v 1.16 2005/08/04 16:32:55 jsnelson Exp $
//

#ifndef __QUERYPLANFUNCTION_HPP
#define	__QUERYPLANFUNCTION_HPP

#include <pathan/dataItem/DataItemFunction.hpp>
#include <pathan/dataItem/LazySequenceResult.hpp>

#include "../IDS.hpp"

namespace DbXml
{

class TransactedContainer;
class ImpliedSchemaNode;
class QueryPlan;
class ReferenceMinder;
class XmlManager;
class Transaction;
class XmlQueryContext;

class QueryPlanFunction : public DataItemFunction
{
public:
	static const XMLCh name[];

	typedef enum {
		DOCUMENT,
		COLLECTION
	} Type;

	QueryPlanFunction(ReferenceMinder &minder, Type type, const VectorOfDataItems &args, XPath2MemoryManager* memMgr);

	virtual DataItem* staticResolution(StaticContext* context);
	virtual Result createResult(DynamicContext* context, int flags=0) const;

	void generateQueryPlan(XmlManager &db, Transaction *txn, QueryPlan *qp,
			       DynamicContext *context);

	const TransactedContainer *getContainer() const { return container_; }

	Type getFunctionType() const
	{
		return type_;
	}
	void setImpliedSchema(ImpliedSchemaNode *impliedSchema);
	const ImpliedSchemaNode *getImpliedSchema() const {
		return impliedSchema_;
	}
	const QueryPlan *getQueryPlan() const {
		return qp_;
	}
	bool isQueryPlanExecutable() const {
		return qpIsExecutable_;
	}

private:
	const XMLCh *getUriArg(DynamicContext *context) const;

	class DefaultBehaviourResult : public LazySequenceResult
	{
	public:
		DefaultBehaviourResult(const QueryPlanFunction *func, int flags, DynamicContext *context);
		void getResult(Sequence &toFill, DynamicContext *context) const;
		std::string asString(DynamicContext *context, int indent) const;
	private:
		int flags_;
		const QueryPlanFunction *func_;
	};

	class QueryPlanResult : public ResultImpl
	{
	public:
		QueryPlanResult(const QueryPlan *qp, const TransactedContainer *container, int flags, DynamicContext *context);
		Item::Ptr next(DynamicContext *context);
		std::string asString(DynamicContext *context, int indent) const;
	private:
		int flags_;
		const QueryPlan *qp_;
		const TransactedContainer *container_;
		bool toDo_;
		IDS::SharedPtr ids_;
		IDS::iterator it_;
	};

	class JITCompileResult : public ResultImpl
	{
	public:
		JITCompileResult(const QueryPlanFunction *func, int flags, DynamicContext *context);
		~JITCompileResult();
		Item::Ptr next(DynamicContext *context);
		std::string asString(DynamicContext *context, int indent) const;
	private:
		int flags_;
		const QueryPlanFunction *func_;
		XmlContainer *container_;
		bool toDo_;
		Result results_;
	};

	// Required for MSVC6
	friend class DefaultBehaviourResult;
	friend class QueryPlanResult;
	friend class JITCompileResult;

	const Type type_;

	ImpliedSchemaNode *impliedSchema_;
	bool qpIsExecutable_;
	QueryPlan *qp_;

	TransactedContainer *container_;
	ReferenceMinder &minder_;
};

}

#endif
