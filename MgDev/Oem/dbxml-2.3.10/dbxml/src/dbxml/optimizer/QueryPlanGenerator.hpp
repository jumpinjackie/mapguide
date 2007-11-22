//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: QueryPlanGenerator.hpp,v 1.37 2006/10/30 17:45:59 bostic Exp $
//

#ifndef __QUERYPLANGENERATOR_HPP
#define	__QUERYPLANGENERATOR_HPP

#include <dbxml/XmlQueryContext.hpp>
#include "NodeVisitingOptimizer.hpp"
#include "../ImpliedSchemaNode.hpp"
#include "../QueryPlan.hpp"
#include "../dataItem/Join.hpp"

#include <xqilla/context/impl/VariableStoreTemplate.hpp>
#include <xqilla/framework/XPath2MemoryManagerImpl.hpp>

namespace DbXml
{

class DbXmlContext;

class QueryPlanFunction;
class DbXmlDocAvailable;
class DbXmlNodeTest;
class DbXmlCompare;
class DbXmlFilter;
class DbXmlNav;
class DbXmlContains;
class DbXmlStep;
class LookupIndex;
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
	QueryPlanGenerator(DbXmlContext *xpc, Optimizer *parent = 0);
	virtual ~QueryPlanGenerator() { varStore_.clear(); }

	const QueryPlan *getQueryPlan() const
	{
		return unionOp_;
	}

	typedef ImpliedSchemaNode::Vector Paths;

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
private:
	typedef std::set<unsigned int> VariableIDs;

	class VarValue : public PathResult {
	public:
		unsigned int id;

		VarValue(unsigned int i) : id(i) {}
		VarValue(const PathResult &p, unsigned int i)
			: PathResult(p), id(i) {}
	};

	typedef VarHashEntry<VarValue> VarStoreRef;
	typedef VariableStoreTemplate<VarValue> VarStore;

	virtual void resetInternal();

	virtual void optimize(XQQuery *query);
	virtual ASTNode *optimize(ASTNode *item);

	PathResult generate(ASTNode *item, VariableIDs *ids);
	PathResult generateParenthesizedExpr(XQParenthesizedExpr *item, VariableIDs *ids);
	PathResult generateFunction(XQFunction *item, VariableIDs *ids);
	PathResult generateOperator(XQOperator *item, VariableIDs *ids);
	PathResult generateNav(XQNav *item, VariableIDs *ids);
	PathResult generateStep(XQStep *item, VariableIDs *ids);
	PathResult generateVariable(XQVariable *item, VariableIDs *ids);
	PathResult generateIf(XQIf *item, VariableIDs *ids);
	PathResult generateInstanceOf(XQInstanceOf *item, VariableIDs *ids);
	PathResult generateCastableAs(XQCastableAs *item, VariableIDs *ids);
	PathResult generateFLWOR(XQFLWOR *item, VariableIDs *ids);
	PathResult generateFLWORBinding(XQFLWOR *item, VectorOfVariableBinding::iterator i,
		VectorOfVariableBinding::iterator end, VariableIDs *ids);
	PathResult generateFLWORQuantified(XQQuantified *item, VariableIDs *ids);
	PathResult generateTypeswitch(XQTypeswitch *item, VariableIDs *ids);
	PathResult generateGlobalVar(XQGlobalVariable *item, VariableIDs *ids);
	PathResult generateDOMConstructor(XQDOMConstructor *item, VariableIDs *ids);
	PathResult generateUserFunction(XQUserFunction::XQFunctionEvaluator *item, VariableIDs *ids);
	PathResult generateContextItem(XQContextItem *item, VariableIDs *ids);
	PathResult generateLiteral(XQLiteral *item, VariableIDs *ids);
	PathResult generateSequence(XQSequence *item, VariableIDs *ids);
	PathResult generateCastAs(XQCastAs *item, VariableIDs *ids);
	PathResult generateTreatAs(XQTreatAs *item, VariableIDs *ids);
	PathResult generateDebugHook(XQDebugHook *item, VariableIDs *ids);
	PathResult generateOrderingChange(XQOrderingChange *item, VariableIDs *ids);
	PathResult generateAtomize(XQAtomize *item, VariableIDs *ids);
	PathResult generatePromoteUntyped(XQPromoteUntyped *item, VariableIDs *ids);
	PathResult generatePromoteNumeric(XQPromoteNumeric *item, VariableIDs *ids);
	PathResult generatePromoteAnyURI(XQPromoteAnyURI *item, VariableIDs *ids);
	PathResult generateDocumentOrder(XQDocumentOrder *item, VariableIDs *ids);
	PathResult generatePredicate(XQPredicate *item, VariableIDs *ids);
	PathResult generateValidate(XQValidate *item, VariableIDs *ids);
	PathResult generateDbXmlCompare(DbXmlCompare *item, VariableIDs *ids);
	PathResult generateDbXmlFilter(DbXmlFilter *item, VariableIDs *ids);
	PathResult generateDbXmlContains(DbXmlContains *item, VariableIDs *ids);
	PathResult generateLookupIndex(LookupIndex *item, VariableIDs *ids);
	PathResult generateQueryPlanFunction(QueryPlanFunction *item, VariableIDs *ids);
	PathResult generateDbXmlStep(DbXmlStep *item, VariableIDs *ids);
	PathResult generateDbXmlNav(DbXmlNav *item, VariableIDs *ids);

	PathResult generateStep(Join::Type join, const NodeTest *step,
		QueryPlanHolder *qph, VariableIDs *ids);
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

	void generateComparison(ImpliedSchemaNode::Type type, bool generalComp,
		VectorOfASTNodes &args, PathResult &result, VariableIDs *ids,
		QueryPlanHolder &arg0qph, QueryPlanHolder &arg1qph);
	void generateSubstring(ImpliedSchemaNode::Type type, VectorOfASTNodes &args,
		PathResult &result, VariableIDs *ids, QueryPlanHolder *qph);
	void generateLookup(QueryPlanHolder *qph, ImpliedSchemaNode::Type type,
		const char *child, const char *parent, PathResult &result);

	void setQueryPlans();

	const VarValue &getCurrentContext() const;
	unsigned int setCurrentContext(const PathResult &value);
	unsigned int setVariable(const XMLCh *uri, const XMLCh *name,
				 const PathResult &value);

	void storeInScopeVars(ImpliedSchemaNode *root);
	bool varsInScope(ImpliedSchemaNode *node, const VariableIDs &argIDs) const;

	void addSecondaryOp(const PathResult &result);
	void addSecondaryOpAndMark(const PathResult &result);

	DbXmlContext *xpc_;

	std::set<const ASTNode*> userFunctionStack_;
	std::vector<QueryPlanFunction*> qpfs_;
	std::map<ImpliedSchemaNode*, VariableIDs> inScopeVars_;

	XPath2MemoryManagerImpl memMgr_;
	UnionQP *unionOp_; ///< Memory owned by memMgr_
	VarStore varStore_; ///< Memory owned by memMgr_
	unsigned int varId_;
};

}

#endif
