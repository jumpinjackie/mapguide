//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: DbXmlNav.hpp,v 1.19 2006/10/30 17:45:55 bostic Exp $
//

#ifndef __DBXMLNAV_HPP
#define	__DBXMLNAV_HPP

#include "DbXmlNodeImpl.hpp"
#include "QueryPlanHolder.hpp"
#include "DbXmlASTNode.hpp"
#include "../IndexEntry.hpp"

class XQNav;

namespace DbXml
{

class TransactedContainer;
class QueryPlan;

class DbXmlNav : public DbXmlASTNode
{
public:
	typedef std::vector<ASTNode*,XQillaAllocator<ASTNode*> > Steps;

	DbXmlNav(XPath2MemoryManager* memMgr);
	DbXmlNav(XQNav *nav, XPath2MemoryManager* memMgr);
	DbXmlNav(ASTNode *step, XPath2MemoryManager* memMgr);

	virtual ASTNode *staticTyping(StaticContext *context);
	virtual DbXmlResult createResult(const DbXmlResult &contextItems, unsigned int props,
		DynamicContext *context) const;

	void recalculateSRC();

	void addStep(ASTNode *step);
	void addStepFront(ASTNode *step);
	void setSteps(const Steps &steps) { steps_ = steps; }
	const Steps &getSteps() const { return steps_; }

	bool getSortAdded() const { return sortAdded_; }
	void setSortAdded(bool value) { sortAdded_ = value; }

	static DbXmlResult sort(const DbXmlResult &result, unsigned int &props, const LocationInfo *location,
		DynamicContext *context);
	static DbXmlResult sortIfPossible(const DbXmlResult &result, unsigned int &props, const LocationInfo *location,
		DynamicContext *context);

private:
	static DbXmlResult makeStepResult(DbXmlResult &parent, unsigned int &props, const Steps::const_iterator &it,
		const Steps::const_iterator &end, DynamicContext *context, const LocationInfo *location);

	class GroupedSort : public ResultImpl
	{
	public:
		GroupedSort(const Result &parent, const LocationInfo *location);
		Item::Ptr next(DynamicContext *context);
		std::string asString(DynamicContext *context, int indent) const;
	private:
		Result parent_;
		bool toDo_;

		Item::Ptr next_;
		Result sorted_;
	};

	Steps steps_;
	bool sortAdded_;
};


}

#endif
