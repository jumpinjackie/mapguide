//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: DbXmlASTNode.hpp,v 1.3 2006/10/30 17:45:54 bostic Exp $
//

#ifndef __DBXMLASTNODE_HPP
#define	__DBXMLASTNODE_HPP

#include "DbXmlResult.hpp"

#include <xqilla/ast/ASTNodeImpl.hpp>

namespace DbXml
{

class DbXmlASTNode : public ASTNodeImpl
{
public:
	enum whichType {
		DBXML_COMPARE = ASTNode::FTCONTAINS + 20,
		DBXML_FILTER,
		DBXML_CONTAINS,
		LOOKUP_INDEX,
		QUERY_PLAN_FUNCTION,
		DBXML_STEP,
		JOIN,
		DBXML_NAV
	};

	static DbXmlASTNode *getDbXmlASTNode(ASTNode *ast)
	{
		switch(ast->getType()) {
		case ((ASTNode::whichType)DBXML_COMPARE):
		case ((ASTNode::whichType)DBXML_FILTER):
		case ((ASTNode::whichType)DBXML_CONTAINS):
		case ((ASTNode::whichType)LOOKUP_INDEX):
		case ((ASTNode::whichType)QUERY_PLAN_FUNCTION):
		case ((ASTNode::whichType)DBXML_STEP):
		case ((ASTNode::whichType)JOIN):
		case ((ASTNode::whichType)DBXML_NAV):
			return (DbXmlASTNode*)ast;
		default: break;
		}
		return 0;
	}

	DbXmlASTNode(whichType type, XPath2MemoryManager* memMgr)
		: ASTNodeImpl(memMgr)
	{
		setType((ASTNode::whichType)type);
	}

	virtual ~DbXmlASTNode() {}

	/// This has been called by the time any DbXmlASTNode
	/// objects are created, and won't be called again.
	virtual ASTNode *staticResolution(StaticContext *context)
	{
		return this;
	}

	virtual Result createResult(DynamicContext *context, int flags=0) const
	{
		static const unsigned int props =
			StaticResolutionContext::DOCORDER | StaticResolutionContext::GROUPED |
			StaticResolutionContext::PEER | StaticResolutionContext::SUBTREE |
			StaticResolutionContext::SAMEDOC | StaticResolutionContext::ONENODE;
		return createResult(0, props, context);
	}

	virtual DbXmlResult createDbXmlResult(DynamicContext* context) const
	{
		static const unsigned int props =
			StaticResolutionContext::DOCORDER | StaticResolutionContext::GROUPED |
			StaticResolutionContext::PEER | StaticResolutionContext::SUBTREE |
			StaticResolutionContext::SAMEDOC | StaticResolutionContext::ONENODE;
		return createResult(0, props, context);
	}

	virtual DbXmlResult createResult(const DbXmlResult &contextItems, unsigned int props,
		DynamicContext *context) const = 0;

	static DbXmlResult createDbXmlResult(ASTNode *ast, DynamicContext *context)
	{
		DbXmlASTNode *da = getDbXmlASTNode(ast);
		if(da != 0) return da->createDbXmlResult(context);
		return ast->collapseTree(context).get();
	}
};

}

#endif
