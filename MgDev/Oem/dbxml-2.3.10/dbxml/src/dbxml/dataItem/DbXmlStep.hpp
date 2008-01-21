//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: DbXmlStep.hpp,v 1.3 2006/10/30 17:45:55 bostic Exp $
//

#ifndef __DBXMLSTEP_HPP_7892340
#define	__DBXMLSTEP_HPP_7892340

#include "QueryPlanHolder.hpp"
#include "DbXmlASTNode.hpp"
#include "Join.hpp"

#include <xqilla/ast/ASTNodeImpl.hpp>

class XQStep;

namespace DbXml
{

class DbXmlNodeTest;

class DbXmlStep : public DbXmlASTNode, public QueryPlanHolder
{
public:
	DbXmlStep(const XQStep *step, XPath2MemoryManager* memMgr);
	DbXmlStep(Join::Type join, XPath2MemoryManager* memMgr);
	DbXmlStep(Join::Type join, const DbXmlStep *step, XPath2MemoryManager* memMgr);
	DbXmlStep(Join::Type join, DbXmlNodeTest *nodeTest, XPath2MemoryManager* memMgr);

	virtual ASTNode* staticResolution(StaticContext *context);
	virtual ASTNode *staticTyping(StaticContext *context);
	virtual DbXmlResult createResult(const DbXmlResult &contextItems, unsigned int props,
		DynamicContext* context) const;

	Join::Type getJoinType() const { return joinType_; }
	const DbXmlNodeTest *getNodeTest() const { return nodeTest_; }

private:
	Join::Type joinType_;
	DbXmlNodeTest *nodeTest_;
};

}

#endif
