//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: DbXmlCompare.hpp,v 1.6 2005/12/16 11:35:44 jsnelson Exp $
//

#ifndef __DBXMLCOMPARE_HPP
#define	__DBXMLCOMPARE_HPP

#include "QueryPlanHolder.hpp"

#include <pathan/dataItem/DataItemImpl.hpp>
#include <pathan/operators/GeneralComp.hpp>

namespace DbXml
{

class DbXmlCompare : public DataItemImpl, public QueryPlanHolder
{
public:
	DbXmlCompare(GeneralComp::ComparisonOperation op, bool generalComp, DataItem *arg, XPath2MemoryManager* memMgr);

	virtual DataItem* staticResolution(StaticContext *context);
	virtual Result createResult(DynamicContext* context, int flags=0) const;
	Result createResult(const Result &contextItems, unsigned int props, DynamicContext* context) const;

	GeneralComp::ComparisonOperation getOperation() const { return operation_; }
	const DataItem *getArgument() const { return arg_; }
	void setArgument(DataItem *a) { arg_ = a; }
	
private:
	class ValueCompareResult : public ResultImpl
	{
	public:
		ValueCompareResult(const Result &parent, const DbXmlCompare *comp, DynamicContext *context);
		Item::Ptr next(DynamicContext *context);
		void skip();
		std::string asString(DynamicContext *context, int indent) const;
	private:
		Result parent_;
		const DbXmlCompare *compare_;
	};

	class GeneralCompareResult : public ResultImpl
	{
	public:
		GeneralCompareResult(const Result &parent, const DbXmlCompare *comp, DynamicContext *context);
		Item::Ptr next(DynamicContext *context);
		void skip();
		std::string asString(DynamicContext *context, int indent) const;
	private:
		Result parent_;
		const DbXmlCompare *compare_;
	};

	GeneralComp::ComparisonOperation operation_;
	bool generalComp_;
	DataItem *arg_;
};

}

#endif
