//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: DbXmlNav.hpp,v 1.9 2005/12/16 11:35:44 jsnelson Exp $
//

#ifndef __DBXMLNAV_HPP
#define	__DBXMLNAV_HPP

#include <pathan/dataItem/DataItemNav.hpp>

#include "DbXmlNodeImpl.hpp"
#include "QueryPlanHolder.hpp"
#include "../IndexEntry.hpp"

namespace DbXml
{

class TransactedContainer;
class QueryPlan;

class DbXmlNav : public DataItemNav
{
public:
	DbXmlNav(XPath2MemoryManager* memMgr);
	DbXmlNav(DataItemNav *nav, XPath2MemoryManager* memMgr);
	DbXmlNav(DataItem *step, XPath2MemoryManager* memMgr);

	virtual Result createResult(DynamicContext* context, int flags=0) const;
	Result createResultForDbXmlFilter(const Result &contextItems, unsigned int properties, DynamicContext* context, int flags=0) const;

	void recalculateSRC();
	void setSteps(const Steps &steps) { _steps = steps; }

	static Result sortIfPossible(const Result &result, unsigned int &props, DynamicContext *context);

private:
	static Result makeStepResult(Result &parent, unsigned int &props, const Steps::const_iterator &it,
		const Steps::const_iterator &end, int flags, DynamicContext *context);

	class GroupedSort : public ResultImpl
	{
	public:
		GroupedSort(const Result &parent, DynamicContext *context);
		Item::Ptr next(DynamicContext *context);
		std::string asString(DynamicContext *context, int indent) const;
	private:
		Result parent_;
		bool toDo_;

		Item::Ptr next_;
		Result sorted_;
	};

	class DbXmlGotoRootResult : public ResultImpl
	{
	public:
		DbXmlGotoRootResult(const Result &parent, DynamicContext *context);
		Item::Ptr next(DynamicContext *context);
		std::string asString(DynamicContext *context, int indent) const;
	private:
		Result parent_;
	};

	class QueryPlanStepResult : public QueryPlanResultImpl
	{
	public:
		QueryPlanStepResult(const Result &parent, const DataItemStep *step,
			int flags, DynamicContext *context);
		void setResult(const DbXmlNodeImpl *node, DynamicContext *context);
		std::string asString(DynamicContext *context, int indent) const;
	private:
		const DataItemStep *step_;
		int flags_;
	};
};


}

#endif
