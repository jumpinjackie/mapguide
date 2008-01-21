//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: NodeHandleFunction.hpp,v 1.2 2006/10/30 17:45:55 bostic Exp $
//

#ifndef __NODEHANDLEFUNCTION_HPP
#define	__NODEHANDLEFUNCTION_HPP

#include "DbXmlFunction.hpp"

#include <xqilla/runtime/SingleResult.hpp>

namespace DbXml
{

class TransactedContainer;
class DbXmlContext;

class NodeToHandleFunction : public DbXmlFunction
{
public:
	static const XMLCh name[];
	static const unsigned int minArgs, maxArgs;

	NodeToHandleFunction(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr);

	virtual ASTNode* staticResolution(StaticContext* context);
	virtual ASTNode *staticTyping(StaticContext *context);
	virtual Result createResult(DynamicContext* context, int flags=0) const;

private:
	class NodeToHandleResult : public SingleResult
	{
	public:
		NodeToHandleResult(const NodeToHandleFunction *func);
		Item::Ptr getSingleResult(DynamicContext *context) const;
		std::string asString(DynamicContext *context, int indent) const;
	private:
		const NodeToHandleFunction *func_;
	};
};

class HandleToNodeFunction : public DbXmlFunction
{
public:
	static const XMLCh name[];
	static const unsigned int minArgs, maxArgs;

	HandleToNodeFunction(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr);

	virtual ASTNode* staticResolution(StaticContext* context);
	virtual ASTNode *staticTyping(StaticContext *context);
	virtual Result createResult(DynamicContext* context, int flags=0) const;

	TransactedContainer *getContainerArg(DbXmlContext *context, bool lookup) const;

private:
	class HandleToNodeResult : public SingleResult
	{
	public:
		HandleToNodeResult(const HandleToNodeFunction *func);
		Item::Ptr getSingleResult(DynamicContext *context) const;
		std::string asString(DynamicContext *context, int indent) const;
	private:
		const HandleToNodeFunction *func_;
	};

	TransactedContainer *container_;
};

}

#endif
