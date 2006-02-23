//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: QueryPlanGenerator.hpp,v 1.20 2005/08/23 12:46:02 jsnelson Exp $
//

#ifndef __QUERYPLANGENERATOR_HPP
#define	__QUERYPLANGENERATOR_HPP

#include <dbxml/XmlQueryContext.hpp>
#include "NodeVisitingOptimizer.hpp"
#include "../ImpliedSchemaNode.hpp"
#include "../QueryPlan.hpp"

#include <pathan/simpleVariables/VariableStoreTemplate.hpp>
#include <pathan/internal/XPath2MemoryManagerImpl.hpp>

namespace DbXml
{

class QueryPlanFunction;
class DbXmlDocAvailable;
class Transaction;
class DbXmlNodeTest;
class DbXmlCompare;
class DbXmlFilter;
class DbXmlNav;
class DbXmlContains;
class QueryPlanHolder;

/**
 * Generates two trees - an implied "schema" of the paths
 * in the tree that will be navigated, and an operations
 * tree the links the navigation steps with set operations
 * based on document selection (not NLS).
 *
 * Should be run after static resolution.
 */
class QueryPlanGenerator : public Optimizer
{
public:
	QueryPlanGenerator(XmlManager &mgr, Transaction *txn, DynamicContext *xpc,
			   Optimizer *parent = 0);
	virtual ~QueryPlanGenerator() { varStore_.clear(); }

	const QueryPlan *getQueryPlan() const
	{
		return unionOp_;
	}

private:
	typedef ImpliedSchemaNode::Vector Paths;
	typedef std::set<unsigned int> VariableIDs;

	class PathResult {
	public:
		Paths returnPaths;
		QueryPlan *operation;
		PathResult() : operation(0) {}

		void join(const PathResult &o);
		void join(ImpliedSchemaNode *o);

		void markSubtree() const;
		void markRoot() const;
	};

	class VarValue : public PathResult {
	public:
		unsigned int id;

		VarValue(unsigned int i) : id(i) {}
		VarValue(const PathResult &p, unsigned int i)
			: PathResult(p), id(i) {}
	};

	typedef VarHashEntry<VarValue> VarStoreRef;
	typedef VariableStoreTemplate<VarValue> VarStore;

	class ArgHolder {
	public:
		ArgHolder(const XMLCh *u, const XMLCh *n, const PathResult &v)
			: uri(u), name(n), value(v) {}

		const XMLCh *uri, *name;
		PathResult value;
	};

	virtual void resetInternal();

	virtual void optimize(XQQuery *query);
	virtual DataItem *optimize(DataItem *item);

	PathResult generate(DataItem *item, VariableIDs *ids);
	PathResult generateParenthesizedExpr(DataItemParenthesizedExpr *item, VariableIDs *ids);
	PathResult generateFunction(DataItemFunction *item, VariableIDs *ids);
	PathResult generateOperator(DataItemOperator *item, VariableIDs *ids);
	PathResult generateNav(DataItemNav *item, VariableIDs *ids);
	PathResult generateStep(DataItemStep *item, VariableIDs *ids);
	PathResult generateVariable(DataItemVariable *item, VariableIDs *ids);
	PathResult generateIf(DataItemIf *item, VariableIDs *ids);
	PathResult generateInstanceOf(DataItemInstanceOf *item, VariableIDs *ids);
	PathResult generateCastableAs(DataItemCastableAs *item, VariableIDs *ids);
	PathResult generateFLWOR(XQFLWOR *item, VariableIDs *ids);
	PathResult generateFLWORQuantified(XQQuantified *item, VariableIDs *ids);
	PathResult generateTypeswitch(XQTypeswitch *item, VariableIDs *ids);
	PathResult generateGlobalVar(XQGlobalVariable *item, VariableIDs *ids);
	PathResult generateDOMConstructor(XQDOMConstructor *item, VariableIDs *ids);
	PathResult generateUserFunction(XQFunction::XQFunctionEvaluator *item, VariableIDs *ids);
	PathResult generateContextItem(DataItemContextItem *item, VariableIDs *ids);
	PathResult generateLiteral(DataItemLiteral *item, VariableIDs *ids);
	PathResult generateSequence(DataItemSequence *item, VariableIDs *ids);
	PathResult generateCastAs(DataItemCastAs *item, VariableIDs *ids);
	PathResult generateTreatAs(DataItemTreatAs *item, VariableIDs *ids);
	PathResult generateDebugHook(XQDebugHook *item, VariableIDs *ids);
	PathResult generateOrderingChange(XQOrderingChange *item, VariableIDs *ids);
	PathResult generateValidate(XQValidate *item, VariableIDs *ids);
	PathResult generateDbXmlCompare(DbXmlCompare *item, VariableIDs *ids);
	PathResult generateDbXmlFilter(DbXmlFilter *item, VariableIDs *ids);
	PathResult generateDbXmlContains(DbXmlContains *item, VariableIDs *ids);

	//   PathResult generateFor(DataItemFor *item, VariableIDs *ids);
	//   PathResult generateQuantifiedExpr(DataItemQuantifiedExpr *item, VariableIDs *ids);
	//   PathResult generateFunctionCall(XQFunctionCall *item, VariableIDs *ids);

	void generatePredicates(DataItemImpl *item, PathResult &result, VariableIDs *ids);

	void generateBuiltInStep(ImpliedSchemaNode *target, ImpliedSchemaNode &node,
				 PathResult &result);
	void generateParentStep(ImpliedSchemaNode *target, ImpliedSchemaNode &node,
				PathResult &result);
	void generateSelfStep(ImpliedSchemaNode *target, ImpliedSchemaNode &node,
			      PathResult &result);
	void generateDescendantOrSelfStep(ImpliedSchemaNode *target,
					  ImpliedSchemaNode &node,
					  PathResult &result);
	void generateAncestorStep(ImpliedSchemaNode *target,
				  ImpliedSchemaNode &node, PathResult &result);
	void generateAncestorOrSelfStep(ImpliedSchemaNode *target,
					ImpliedSchemaNode &node,
					PathResult &result);
	void generateFollowingStep(ImpliedSchemaNode *target,
				   ImpliedSchemaNode &node, PathResult &result);
	void generatePrecedingStep(ImpliedSchemaNode *target,
				   ImpliedSchemaNode &node, PathResult &result);
	void generateSiblingStep(ImpliedSchemaNode *target, ImpliedSchemaNode &node,
				 PathResult &result);

	void generateComparison(ImpliedSchemaNode::Type type, VectorOfDataItems &args,
				PathResult &result, VariableIDs *ids);
	void generateSubstring(ImpliedSchemaNode::Type type, VectorOfDataItems &args,
			       PathResult &result, VariableIDs *ids);

	void generateRawQueryPlans(QueryPlan *qp);

	const VarValue &getCurrentContext() const;
	unsigned int setCurrentContext(const PathResult &value);
	unsigned int setVariable(const XMLCh *uri, const XMLCh *name,
				 const PathResult &value);

	void storeInScopeVars(ImpliedSchemaNode *root);
	bool varsInScope(ImpliedSchemaNode *node, const VariableIDs &argIDs) const;

	void addSecondaryOp(const PathResult &result);
	void addSecondaryOpAndMark(const PathResult &result);

	XmlManager &mgr_;
	Transaction *txn_;
	DynamicContext *xpc_;

	bool predicateContext_;
	std::set<const DataItem*> userFunctionStack_;
	std::vector<QueryPlanFunction*> qpfs_;
	std::vector<DbXmlDocAvailable*> docAvails_;
	std::vector<QueryPlanHolder*> qpHolders_;
	std::vector<DbXmlNav*> navs_;
	std::map<ImpliedSchemaNode*, VariableIDs> inScopeVars_;

	XPath2MemoryManagerImpl memMgr_;
	UnionQP *unionOp_; ///< Memory owned by memMgr_
	VarStore varStore_; ///< Memory owned by memMgr_
	unsigned int varId_;
};

}

#endif
