//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: DbXmlFilter.hpp,v 1.13 2006/10/30 17:45:55 bostic Exp $
//

#ifndef __DBXMLFILTER_HPP
#define	__DBXMLFILTER_HPP

#include "DbXmlASTNode.hpp"

namespace DbXml
{

class DbXmlFilter : public DbXmlASTNode
{
public:
	DbXmlFilter(ASTNode *arg, XPath2MemoryManager* memMgr);

	virtual ASTNode *staticTyping(StaticContext *context);
	virtual DbXmlResult createResult(const DbXmlResult &contextItems, unsigned int props,
		DynamicContext* context) const;

	const ASTNode *getArgument() const { return arg_; }
	void setArgument(ASTNode *arg) { arg_ = arg; }

	class FilterResult : public DbXmlResultImpl
	{
	public:
		FilterResult(const DbXmlResult &parent, const DbXmlFilter *filter);
		Item::Ptr next(DynamicContext *context);
		Item::Ptr seek(const DocID &did, const NsNid &nid, DynamicContext *context);
	private:
		Item::Ptr filter(const Item::Ptr &item, DynamicContext *context);

		DbXmlResult parent_;
		const ASTNode *arg_;
		bool toDo_;
	};

private:
	ASTNode *arg_;
};

}

#endif
