//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: LookupIndexFunction.hpp,v 1.10 2006/11/17 18:18:28 jsnelson Exp $
//

#ifndef __LOOKUPINDEXFUNCTION_HPP
#define	__LOOKUPINDEXFUNCTION_HPP

#include "DbXmlFunction.hpp"
#include "QueryPlanHolder.hpp"
#include "DbXmlASTNode.hpp"

namespace DbXml
{

class LookupIndexFunction : public DbXmlFunction, public QueryPlanHolder
{
public:
	static const XMLCh name[];
	static const unsigned int minArgs, maxArgs;

	LookupIndexFunction(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr);

	virtual ASTNode* staticResolution(StaticContext* context);
	virtual ASTNode *staticTyping(StaticContext *context);
	virtual Result createResult(DynamicContext* context, int flags=0) const;

	TransactedContainer *getContainerArg(DbXmlContext *context, bool lookup) const;
	const char *getURINameArg(unsigned int argNum, DbXmlContext *context, bool lookup) const;
	virtual QueryPlan *createQueryPlan(DbXmlContext *context, bool lookup) const;

	const char *getChildURIName() const { return childURIName_; }
	const char *getParentURIName() const { return parentURIName_; }

protected:
	LookupIndexFunction(const XMLCh* name, unsigned int argsFrom, unsigned int argsTo,
		const char* paramDecl, const VectorOfASTNodes &args, XPath2MemoryManager* memMgr);

	class LookupIndexFunctionResult : public ResultImpl, public QueryPlanHolder
	{
	public:
		LookupIndexFunctionResult(const LookupIndexFunction *func)
			: ResultImpl(func), func_(func), result_(0) {}
		Item::Ptr next(DynamicContext *context);
		std::string asString(DynamicContext *context, int indent) const
		{
			return "lookupindexfunctionresult";
		}
	private:
		const LookupIndexFunction *func_;
		Result result_;
	};

	const char *childURIName_, *parentURIName_;
};

class LookupAttributeIndexFunction : public LookupIndexFunction
{
public:
	static const XMLCh name[];
	static const unsigned int minArgs, maxArgs;

	LookupAttributeIndexFunction(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr);

	virtual ASTNode *staticTyping(StaticContext *context);
	virtual QueryPlan *createQueryPlan(DbXmlContext *context, bool lookup) const;
};

class LookupMetaDataIndexFunction : public LookupIndexFunction
{
public:
	static const XMLCh name[];
	static const unsigned int minArgs, maxArgs;

	LookupMetaDataIndexFunction(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr);

	virtual ASTNode *staticTyping(StaticContext *context);
	virtual QueryPlan *createQueryPlan(DbXmlContext *context, bool lookup) const;
};

class LookupIndex : public DbXmlASTNode, public QueryPlanHolder
{
public:
	LookupIndex(const QueryPlanHolder *qph, XPath2MemoryManager *mm);

	virtual ASTNode *staticTyping(StaticContext *context);
	virtual DbXmlResult createResult(const DbXmlResult &contextItems, unsigned int props,
		DynamicContext *context) const;
};

class LookupIndexResult : public DbXmlResultImpl
{
public:
	LookupIndexResult(const QueryPlanHolder *qpHolder, const LocationInfo *location)
		: DbXmlResultImpl(location), holder_(qpHolder), toDo_(true) {}
	Item::Ptr next(DynamicContext *context);
	Item::Ptr seek(const DocID &did, const NsNid &nid, DynamicContext *context);
private:
	void init(DynamicContext *context);

	const QueryPlanHolder *holder_;
	bool toDo_;

	IndexData::Ptr data_;
	IndexData::const_iterator it_;
};

}

#endif
