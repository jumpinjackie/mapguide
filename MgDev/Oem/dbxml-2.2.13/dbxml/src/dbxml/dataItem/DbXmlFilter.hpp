//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: DbXmlFilter.hpp,v 1.2 2005/08/11 17:07:33 jsnelson Exp $
//

#ifndef __DBXMLFILTER_HPP
#define	__DBXMLFILTER_HPP

#include <pathan/dataItem/DataItemImpl.hpp>

namespace DbXml
{

class DbXmlNav;

class DbXmlFilter : public DataItemImpl
{
public:
	DbXmlFilter(DbXmlNav *filterExpr, XPath2MemoryManager* memMgr);

	virtual DataItem* staticResolution(StaticContext *context);
	virtual Result createResult(DynamicContext* context, int flags=0) const;

	const DbXmlNav *getFilterExpr() const { return filterExpr_; }
	void setFilterExpr(DbXmlNav *filterExpr) { filterExpr_ = filterExpr; }

	class FilterResult : public ResultImpl
	{
	public:
		FilterResult(const Result &parent, unsigned int props,
			const DbXmlNav *filterExpr, DynamicContext *context);
		Item::Ptr next(DynamicContext *context);
		void skip();
		std::string asString(DynamicContext *context, int indent) const;
	private:
		class Stub : public ResultImpl
		{
		public:
			Stub(FilterResult *owner, DynamicContext *context);
			Item::Ptr next(DynamicContext *context);
			std::string asString(DynamicContext *context, int indent) const;
		private:
			FilterResult *owner_;
		};
		friend class Stub;

		Result parent_;
		unsigned int props_;
		const DbXmlNav *filterExpr_;
		bool toDo_;

		Item::Ptr item_;
		Result result_;
	};

private:
	DbXmlNav *filterExpr_;
};


}

#endif
