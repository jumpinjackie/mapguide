//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: QueryPlanFunction.hpp,v 1.30.2.1 2007/01/19 14:56:01 jsnelson Exp $
//

#ifndef __QUERYPLANFUNCTION_HPP
#define	__QUERYPLANFUNCTION_HPP

#include <xqilla/runtime/LazySequenceResult.hpp>

#include "../IDS.hpp"
#include "QueryPlanHolder.hpp"
#include "DbXmlASTNode.hpp"

namespace DbXml
{

class TransactedContainer;
class ImpliedSchemaNode;
class ReferenceMinder;
class XmlManager;
class Transaction;

class QueryPlanFunction : public DbXmlASTNode, public QueryPlanHolder
{
public:
	typedef enum {
		DOCUMENT,
		COLLECTION
	} Type;

	QueryPlanFunction(ReferenceMinder &minder, Type type, ASTNode *arg, XPath2MemoryManager* memMgr);

	virtual ASTNode* staticResolution(StaticContext* context);
	virtual ASTNode *staticTyping(StaticContext *context);
	virtual DbXmlResult createResult(const DbXmlResult &contextItems, unsigned int props,
		DynamicContext *context) const;

	virtual void generateQueryPlan(XmlManager &mgr, Transaction *txn, DbXmlContext *context);
	void removeNonConstant(XmlManager &mgr, Transaction *txn, DbXmlContext *context);

	Type getFunctionType() const
	{
		return type_;
	}
	const ASTNode *getArgument() const { return arg_; }
	void setArgument(ASTNode *arg) { arg_ = arg; }
	void setImpliedSchema(ImpliedSchemaNode *impliedSchema);
	const ImpliedSchemaNode *getImpliedSchema() const {
		return impliedSchema_;
	}

private:
	const XMLCh *getUriArg(DbXmlContext *context) const;

	class DefaultBehaviourResult : public LazySequenceResult
	{
	public:
		DefaultBehaviourResult(const QueryPlanFunction *func, DynamicContext *context);
		void getResult(Sequence &toFill, DynamicContext *context) const;
		std::string asString(DynamicContext *context, int indent) const;
	private:
		const QueryPlanFunction *func_;
	};

	class QueryPlanResult : public DbXmlResultImpl
	{
	public:
		QueryPlanResult(const QueryPlan *qp, const TransactedContainer *container,
			const XMLCh *documentUri, const LocationInfo *location);
		Item::Ptr next(DynamicContext *context);
		Item::Ptr seek(const DocID &did, const NsNid &nid, DynamicContext *context);
	private:
		void init(DynamicContext *context);

		const QueryPlan *qp_;
		const TransactedContainer *container_;
		const XMLCh *documentUri_;
		bool toDo_;
		IDS::Ptr ids_;
		IDS::iterator it_;
	};

	class JITCompileResult : public ResultImpl
	{
	public:
		JITCompileResult(const QueryPlanFunction *func, bool useQPs);
		~JITCompileResult();
		Item::Ptr next(DynamicContext *context);
		std::string asString(DynamicContext *context, int indent) const;
	private:
		bool useQPs_;
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
	ASTNode *arg_;

	ImpliedSchemaNode *impliedSchema_;

	ReferenceMinder &minder_;
};

}

#endif
