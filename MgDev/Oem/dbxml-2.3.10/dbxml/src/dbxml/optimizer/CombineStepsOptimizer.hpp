//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: CombineStepsOptimizer.hpp,v 1.2 2006/10/30 17:45:59 bostic Exp $
//

#ifndef __COMBINESTEPSOPTIMIZER_HPP
#define	__COMBINESTEPSOPTIMIZER_HPP

#include "NodeVisitingOptimizer.hpp"

namespace DbXml
{

class DbXmlContext;

class CombineStepsOptimizer : public NodeVisitingOptimizer
{
public:
	CombineStepsOptimizer(DbXmlContext *xpc, Optimizer *parent = 0)
		: NodeVisitingOptimizer(parent), xpc_(xpc) {}

	static ASTNode *dissolveFilter(DbXmlFilter *item, XPath2MemoryManager *mm);

protected:
	virtual void resetInternal() {}

	virtual ASTNode *optimizeDbXmlNav(DbXmlNav *item);
	virtual ASTNode *optimizeDbXmlFilter(DbXmlFilter *item);

private:
	void combine(ASTNode *&prev, ASTNode *current, DbXmlNav *newNav);
	void combineStep(DbXmlStep *step, ASTNode *&prev, ASTNode *current, DbXmlNav *newNav);
	void combineLookup(LookupIndex *index, ASTNode *&prev, ASTNode *current, DbXmlNav *newNav);
	void combineCompare(DbXmlCompare *compare, ASTNode *&prev, ASTNode *current, DbXmlNav *newNav);
	void combineContains(DbXmlContains *contains, ASTNode *&prev, ASTNode *current, DbXmlNav *newNav);

	void checkForExactCompare(DbXmlCompare *compare, ASTNode *&prev, DbXmlNav *newNav);
	void checkForExactContains(DbXmlContains *contains, ASTNode *&prev, DbXmlNav *newNav);

	DbXmlContext *xpc_;
};

}

#endif
