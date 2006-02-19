//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: CollectionAndDocOptimizer.hpp,v 1.15 2005/09/22 10:41:51 jsnelson Exp $
//

#ifndef __COLLECTIONANDDOCOPTIMIZER_HPP
#define	__COLLECTIONANDDOCOPTIMIZER_HPP

#include "NodeVisitingOptimizer.hpp"
#include "../ReferenceMinder.hpp"

#include <pathan/XPath2MemoryManager.hpp>
#include <pathan/operators/GeneralComp.hpp>

class DynamicContext;

namespace DbXml
{

class DbXmlNav;

class CollectionAndDocOptimizer : public NodeVisitingOptimizer
{
public:
	CollectionAndDocOptimizer(ReferenceMinder &minder, DynamicContext *xpc, Optimizer *parent = 0)
		: NodeVisitingOptimizer(parent),
		  xpc_(xpc), minder_(minder)
	{}
	virtual ~CollectionAndDocOptimizer() {}

protected:
	virtual void resetInternal()
	{
		minder_.resetMinder();
	}

	virtual DataItem *optimize(DataItem *item);
	virtual DataItem *optimizePredicate(DataItem *item);
	virtual DataItem *optimizeOperator(DataItemOperator *item);
	virtual DataItem *optimizeNav(DataItemNav *item);
	virtual DataItem *optimizeDbXmlNav(DbXmlNav *item);
	virtual DataItem *optimizeFLWOR(XQFLWOR *item);
	virtual DataItem *optimizeFLWORQuantified(XQQuantified *item);
	virtual DataItem *optimizeFunction(DataItemFunction *item);
	virtual DataItem *optimizeStep(DataItemStep *item);

private:
	DataItem *createDbXmlCompare(DataItemOperator *item, GeneralComp::ComparisonOperation op, bool generalComp);
	DataItem *createDbXmlContains(DataItemFunction *item);
	DataItem *optimizeFLOWRToNav(VectorOfVariableBinding *bindings, XQFLWOR *item, bool quantified);
	DataItem *optimizeNodeReturningDI(DataItemImpl *item);

	typedef std::pair<DataItem*, bool> ParentInfo;
	typedef std::vector<ParentInfo> AncestorStack;

	AncestorStack ancestors_;

	DynamicContext *xpc_;
	ReferenceMinder &minder_;
};

}

#endif
