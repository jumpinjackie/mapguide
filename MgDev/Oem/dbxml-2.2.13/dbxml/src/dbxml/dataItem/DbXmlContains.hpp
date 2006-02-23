//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: DbXmlContains.hpp,v 1.3 2005/12/16 11:35:44 jsnelson Exp $
//

#ifndef __DBXMLCONTAINS_HPP
#define	__DBXMLCONTAINS_HPP

#include "QueryPlanHolder.hpp"

#include <pathan/dataItem/DataItemImpl.hpp>

namespace DbXml
{

class DbXmlContains : public DataItemImpl, public QueryPlanHolder
{
public:
	DbXmlContains(DataItem *arg, XPath2MemoryManager* memMgr);

	virtual DataItem* staticResolution(StaticContext *context);
	virtual Result createResult(DynamicContext* context, int flags=0) const;
	Result createResult(const Result &contextItems, unsigned int props, DynamicContext* context) const;

	const DataItem *getArgument() const { return arg_; }
	void setArgument(DataItem *a) { arg_ = a; }
	
private:
	class ContainsResult : public ResultImpl
	{
	public:
		ContainsResult(const Result &parent, const DbXmlContains *contains, DynamicContext *context);
		Item::Ptr next(DynamicContext *context);
		void skip();
		std::string asString(DynamicContext *context, int indent) const;
	private:
		Result parent_;
		const DbXmlContains *contains_;
	};

	DataItem *arg_;
};

}

#endif
