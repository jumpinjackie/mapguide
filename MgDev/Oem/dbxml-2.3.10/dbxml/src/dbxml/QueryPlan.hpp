//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: QueryPlan.hpp,v 1.107 2006/11/17 18:18:27 jsnelson Exp $
//

#ifndef __QUERYPLAN_HPP
#define	__QUERYPLAN_HPP

#include <vector>
#include <string>
#include <xqilla/framework/XPath2MemoryManager.hpp>
#include <xqilla/ast/StaticResolutionContext.hpp>

#include "ImpliedSchemaNode.hpp"
#include "Key.hpp"
#include "IndexEntry.hpp"
#include "db_cxx.h"

class Result;
class ASTNode;
class LocationInfo;

namespace DbXml
{

class ValueQP;
class KeyStatistics;
class Log;
class QueryExecutionContext;
class DbXmlContext;
class QPValue;
class IndexLookups;
class QueryPlanHolder;

typedef std::vector<const QPValue*> ValueDefs;
typedef std::set<const QueryPlanHolder*> QPHSet;

class QueryPlan
{
public:
	virtual ~QueryPlan() {}
	typedef enum {
		UNION,
		INTERSECT,
		INTERSECT_DOCS,
		PATHS,
		PRESENCE,
		VALUE,
		RANGE,
		UNIVERSE,
		EMPTY,
		DOCUMENT
	} Type;

	class Cost {
	public:
		Cost() : keys(0), pages(0) {}
		Cost(double k, double p) : keys(k), pages(p) {}

		double keys; ///< Max estimated keys returned
		double pages; ///< Max estimated pages required to fulfill

		void intersectOp(const Cost &cost);
		void unionOp(const Cost &cost);
		int compare(const Cost &cost);
	};

	QueryPlan(Type t, XPath2MemoryManager *mm)
		: memMgr_(mm), type_(t), _src(mm) {}

	Type getType() const
	{
		return type_;
	}

	virtual QueryPlan *staticTyping(StaticContext *context) = 0;
	virtual const StaticResolutionContext &getStaticResolutionContext() const { return _src; }

	virtual IDS::Ptr execute(OperationContext &context, QueryExecutionContext &qec) const = 0;
	virtual IndexData::Ptr nodes_execute(OperationContext &context, QueryExecutionContext &qec) const = 0;
	virtual Cost cost(OperationContext &context, QueryExecutionContext &qec) const = 0;

	QueryPlan *createRawQueryPlan(const Log &log, DbXmlContext *context) const;
	QueryPlan *createPartiallyOptimisedQueryPlan(Transaction *txn, const Container &container,
		DbXmlContext *context, bool nodeQP, bool &fullyOptimised, bool &exact) const;
	QueryPlan *createExecutableQueryPlan(Transaction *txn, const Container &container, DbXmlContext *context,
		bool nodeQP, bool &exact) const;
	static QueryPlan *createUniverseQueryPlan(Transaction *txn, const Container &container, DbXmlContext *context);

	/// Removes duplicate (by pointer comparison) and dissolves
	virtual QueryPlan *compress() = 0;
	/** returns 0 if it is a union or intersect, and it has no children.
	    returns the child if it has only one child. Otherwise returns this. */
	virtual QueryPlan *dissolve() = 0;
	/** Removes paths from the tree that do not come from the ImpliedSchemaNode
	    tree rooted at the document root. */
	virtual QueryPlan *rootFilter(const ImpliedSchemaNode *root) = 0;
	/** Removes paths from the tree that are not based on the ImpliedSchemaNodes
	    given. */
	virtual QueryPlan *nodeFilter(const ImpliedSchemaNode::Vector &nodes) = 0;
	/** Returns the QueryPlanHolder objects from the PathsQP in this QueryPlan */
	virtual void findQueryPlanHolders(QPHSet &qphset) const = 0;
	/** Changes all PathsQP objects into StepQP objects. */
	virtual QueryPlan *resolvePaths() = 0;
	/** Uses isSubsetOf to compare arguments of union
	    and intersect operations, and remove unneeded ones, then dissolves.*/
	virtual QueryPlan *removeSubsets(const Log &log) = 0;
	/** Removes non-constant value lookups from the plan.*/
	virtual QueryPlan *removeNonConstant() = 0;
	/** Resolves constant values (and missing syntax types) for ValueQP, by evaluating constant sub-expressions. */
	virtual QueryPlan *resolveValues(const Log &log, DbXmlContext *context) = 0;
	/** Resolves missing syntax types for ValueQP, by looking up the actual tuple item. */
	virtual QueryPlan *resolveSyntax(const Log &log, DbXmlContext *context) = 0;
	/** Resolves the index lookup information for all StepQP objects. success should
	    be set to true before calling, and will be set to false if the method fails. */
	virtual QueryPlan *resolveIndexes(const Container &container, const IndexSpecification &indexSpecification, bool nodeQP, bool &success, bool &exact) = 0;

	/// Returns true if it's sure. Returns false if it doesn't know
	virtual bool isSubsetOf(const QueryPlan *o) const = 0;
	virtual QueryPlan *copy(XPath2MemoryManager *mm = 0) const = 0;
	virtual std::string printQueryPlan(const DynamicContext *context, int indent) const = 0;
	virtual std::string toString(bool brief = true) const = 0;

	void logQP(const Log &log, const std::string &title, const QueryPlan *qp) const;

protected:
	void logLegend(const Log &log) const;
	void logIDs(QueryExecutionContext &qec, const IDS::Ptr &ids) const;
	void logIndexData(QueryExecutionContext &qec, const IndexData::Ptr &data) const;
	void logCost(QueryExecutionContext &qec, Cost &cost, const KeyStatistics *stats) const;

	XPath2MemoryManager *memMgr_;
	Type type_;
	StaticResolutionContext _src;
};

class OperationQP : public QueryPlan
{
public:
	typedef std::vector<QueryPlan*,XQillaAllocator<QueryPlan*> > Vector;

	OperationQP(QueryPlan::Type type, XPath2MemoryManager *mm)
		: QueryPlan(type, mm), args_(XQillaAllocator<QueryPlan*>(mm)) {}
	virtual ~OperationQP() {}

	const Vector &getArgs() const
	{
		return args_;
	}
	virtual QueryPlan *addArg(QueryPlan *o);

	virtual QueryPlan *compress();
	virtual QueryPlan *dissolve();
	virtual QueryPlan *rootFilter(const ImpliedSchemaNode *root);
	virtual QueryPlan *nodeFilter(const ImpliedSchemaNode::Vector &nodes);
	virtual void findQueryPlanHolders(QPHSet &qphset) const;
	virtual QueryPlan *resolvePaths();
	virtual QueryPlan *resolveValues(const Log &log, DbXmlContext *context);
	virtual QueryPlan *resolveSyntax(const Log &log, DbXmlContext *context);
	virtual QueryPlan *resolveIndexes(const Container &container, const IndexSpecification &indexSpecification, bool nodeQP, bool &success, bool &exact);

	virtual bool isSupersetOf(const QueryPlan *o) const = 0;

protected:
	void logSubset(const Log &log, const QueryPlan *subset, const QueryPlan *superset) const;
	void logMerge(const Log &log, const QueryPlan *l, const QueryPlan *r, const QueryPlan *result) const;

	Vector args_;
};

class UnionQP : public OperationQP
{
public:
	UnionQP(XPath2MemoryManager *mm)
		: OperationQP(QueryPlan::UNION, mm) {}

	UnionQP(QueryPlan *l, QueryPlan *r, XPath2MemoryManager *mm);

	virtual QueryPlan *staticTyping(StaticContext *context);

	virtual IDS::Ptr execute(OperationContext &context, QueryExecutionContext &qec) const;
	virtual IndexData::Ptr nodes_execute(OperationContext &context, QueryExecutionContext &qec) const;
	virtual Cost cost(OperationContext &context, QueryExecutionContext &qec) const;

	virtual QueryPlan *removeSubsets(const Log &log);
	virtual QueryPlan *removeNonConstant();

	virtual bool isSubsetOf(const QueryPlan *o) const;
	virtual bool isSupersetOf(const QueryPlan *o) const;
	virtual QueryPlan *copy(XPath2MemoryManager *mm = 0) const;
	virtual std::string printQueryPlan(const DynamicContext *context, int indent) const;
	virtual std::string toString(bool brief = true) const;
};

class IntersectQP : public OperationQP
{
public:
	IntersectQP(XPath2MemoryManager *mm)
		: OperationQP(QueryPlan::INTERSECT, mm) {}

	IntersectQP(QueryPlan *l, QueryPlan *r, XPath2MemoryManager *mm);

	virtual QueryPlan *staticTyping(StaticContext *context);

	virtual IDS::Ptr execute(OperationContext &context, QueryExecutionContext &qec) const;
	virtual IndexData::Ptr nodes_execute(OperationContext &context, QueryExecutionContext &qec) const;
	virtual Cost cost(OperationContext &context, QueryExecutionContext &qec) const;

	virtual QueryPlan *removeSubsets(const Log &log);
	virtual QueryPlan *removeNonConstant();
	QueryPlan *removeSupersetsOf(const QueryPlan *o, const Log &log);

	virtual bool isSubsetOf(const QueryPlan *o) const;
	virtual bool isSupersetOf(const QueryPlan *o) const;
	virtual QueryPlan *copy(XPath2MemoryManager *mm = 0) const;
	virtual std::string printQueryPlan(const DynamicContext *context, int indent) const;
	virtual std::string toString(bool brief = true) const;

private:
	QueryPlan *createRange(const ValueQP *l, const ValueQP *r);
};

class IntersectDocsQP : public OperationQP
{
public:
	IntersectDocsQP(QueryPlan *l, QueryPlan *r, XPath2MemoryManager *mm);

	virtual QueryPlan *addArg(QueryPlan *o);

	virtual QueryPlan *staticTyping(StaticContext *context);

	virtual IDS::Ptr execute(OperationContext &context, QueryExecutionContext &qec) const;
	virtual IndexData::Ptr nodes_execute(OperationContext &context, QueryExecutionContext &qec) const;
	virtual Cost cost(OperationContext &context, QueryExecutionContext &qec) const;

	virtual QueryPlan *removeSubsets(const Log &log);
	virtual QueryPlan *removeNonConstant();

	virtual bool isSubsetOf(const QueryPlan *o) const;
	virtual bool isSupersetOf(const QueryPlan *o) const;
	virtual QueryPlan *copy(XPath2MemoryManager *mm = 0) const;
	virtual std::string printQueryPlan(const DynamicContext *context, int indent) const;
	virtual std::string toString(bool brief = true) const;
};

class PathsQP : public QueryPlan
{
public:
	typedef std::vector<ImpliedSchemaNode*,XQillaAllocator<ImpliedSchemaNode*> > Paths;

	PathsQP(XPath2MemoryManager *mm)
		: QueryPlan(QueryPlan::PATHS, mm), paths_(XQillaAllocator<ImpliedSchemaNode*>(mm)) {}

	PathsQP(const ImpliedSchemaNode::Vector &paths, XPath2MemoryManager *mm)
		: QueryPlan(QueryPlan::PATHS, mm), paths_(XQillaAllocator<ImpliedSchemaNode*>(mm))
	{
		paths_.insert(paths_.end(), paths.begin(), paths.end());
	}
	virtual ~PathsQP() {}

	virtual QueryPlan *staticTyping(StaticContext *context) { return this; }

	virtual IDS::Ptr execute(OperationContext &context, QueryExecutionContext &qec) const { return IDS::Ptr(); }
	virtual IndexData::Ptr nodes_execute(OperationContext &context, QueryExecutionContext &qec) const { return IndexData::Ptr(); }
	virtual Cost cost(OperationContext &context, QueryExecutionContext &qec) const { return Cost(); }

	const Paths &getPaths() const { return paths_; }
	void addPaths(const Paths &o);

	/// Return true if all the paths come from the same tree
	bool sameTree() const;

	virtual QueryPlan *compress();
	virtual QueryPlan *dissolve();
	virtual QueryPlan *rootFilter(const ImpliedSchemaNode *root);
	virtual QueryPlan *nodeFilter(const ImpliedSchemaNode::Vector &nodes);
	virtual void findQueryPlanHolders(QPHSet &qphset) const;
	virtual QueryPlan *resolvePaths();
	virtual QueryPlan *removeSubsets(const Log &log) { return this; }
	virtual QueryPlan *removeNonConstant() { return this; }
	virtual QueryPlan *resolveValues(const Log &log, DbXmlContext *context) { return this; }
	virtual QueryPlan *resolveSyntax(const Log &log, DbXmlContext *context) { return this; }
	virtual QueryPlan *resolveIndexes(const Container &container, const IndexSpecification &indexSpecification,
		bool nodeQP, bool &success, bool &exact) { return this; }

	virtual bool isSubsetOf(const QueryPlan *o) const { return false; }
	virtual QueryPlan *copy(XPath2MemoryManager *mm = 0) const;
	virtual std::string printQueryPlan(const DynamicContext *context, int indent) const;
	virtual std::string toString(bool brief = true) const;

protected:
	Paths paths_;
};

class StepQP : public QueryPlan
{
public:
	StepQP(QueryPlan::Type type, XPath2MemoryManager *mm)
		: QueryPlan(type, mm) {}

	virtual QueryPlan *compress();
	virtual QueryPlan *dissolve();
	virtual QueryPlan *rootFilter(const ImpliedSchemaNode *root);
	virtual QueryPlan *nodeFilter(const ImpliedSchemaNode::Vector &nodes);
	virtual void findQueryPlanHolders(QPHSet &qphset) const;
	virtual QueryPlan *resolvePaths();
	virtual QueryPlan *removeSubsets(const Log &log);
	virtual QueryPlan *removeNonConstant();
	virtual QueryPlan *resolveValues(const Log &log, DbXmlContext *context);
	virtual QueryPlan *resolveSyntax(const Log &log, DbXmlContext *context);
	virtual QueryPlan *resolveIndexes(const Container &container, const IndexSpecification &indexSpecification, bool nodeQP, bool &success, bool &exact);

	static QueryPlan *createStep(const ImpliedSchemaNode *path, XPath2MemoryManager *mm);
	static bool getStepNames(const ImpliedSchemaNode *child, char *&parentName, char *&childName, XPath2MemoryManager *mm);

protected:
	void logIndexUse(const Log &log, const Index &index, const DbWrapper::Operation op) const;
	void logTransformation(const Log &log, const QueryPlan *transformed) const;
};

class QPValue {
public:
 	QPValue(Syntax::Type syntax, const char *value, size_t vlen, bool generalComp);
	QPValue(Syntax::Type syntax, const ASTNode *di, bool generalComp);

	Syntax::Type getSyntax() const {
		return syntax_;
	}
	void setSyntax(Syntax::Type s) {
		syntax_ = s;
	}
	const char *getValue() const {
		return value_;
	}
	size_t getLength() const {
		return len_;
	}
	const ASTNode *getASTNode() const {
		return di_;
	}
	void setASTNode(const ASTNode *di) {
		di_ = di;
	}
	bool isGeneralComp() const {
		return generalComp_;
	}
	std::string asString() const;
	bool equals(const QPValue &o) const;

private:
	Syntax::Type syntax_;
	const char *value_;
	size_t len_;
	const ASTNode *di_;
	bool generalComp_;
};

/// Needed because Key can't be used inside a memory managed object
class QPKey {
public:
	QPKey();
	QPKey(Syntax::Type syntax);

	void setNodeLookup(bool n) {
		nodeLookup_ = n;
	}
	const Index &getIndex() const {
		return index_;
	}
	Index &getIndex() {
		return index_;
	}
	const NameID &getID1() const {
		return id1_;
	}
	NameID &getID1() {
		return id1_;
	}
	void setID1(const NameID &id) {
		id1_ = id;
	}
	const NameID &getID2() const {
		return id2_;
	}
	NameID &getID2() {
		return id2_;
	}
	void setID2(const NameID &id) {
		id2_ = id;
	}

	Key createKey(int timezone) const;
	Key createKey(const QPValue &value, int timezone) const;
	Key createKey(const char *value, size_t length, int timezone) const;

private:
	bool nodeLookup_;
	Index index_;
	NameID id1_;
	NameID id2_;
};

class PresenceQP : public StepQP
{
public:
	PresenceQP(ImpliedSchemaNode::Type type,
		   const char *parent, const char *child,
		   XPath2MemoryManager *mm);

	const char *getParentName() const { return parentUriName_; }
	const char *getChildName() const { return childUriName_; }
	bool isParentSet() const { return parentUriName_ != 0; }
	ImpliedSchemaNode::Type getNodeType() const { return nodeType_; }
	DbWrapper::Operation getOperation() const { return operation_; }
	const QPKey &getKey() const { return key_; }

	void lookupIndex(DbWrapper::Operation operation, Key &key, IDS::Ptr &ids,
		OperationContext &context, QueryExecutionContext &qec) const;
	void lookupIndexNodes(DbWrapper::Operation operation, Key &key, IndexData::Ptr &data,
		OperationContext &context, QueryExecutionContext &qec) const;
	Cost lookupCost(DbWrapper::Operation operation, Key &key,
		OperationContext &context, QueryExecutionContext &qec) const;

	virtual void getKeys(IndexLookups &keys, DbXmlContext *context) const;
	virtual void getKeysForCost(IndexLookups &keys, DbXmlContext *context) const;

	virtual QueryPlan *staticTyping(StaticContext *context);

	virtual IDS::Ptr execute(OperationContext &context, QueryExecutionContext &qec) const;
	virtual IndexData::Ptr nodes_execute(OperationContext &context, QueryExecutionContext &qec) const;
	virtual Cost cost(OperationContext &context, QueryExecutionContext &qec) const;

	virtual QueryPlan *resolveIndexes(const Container &container, const IndexSpecification &indexSpecification,
		bool nodeQP, bool &success, bool &exact);

	virtual bool isSubsetOf(const QueryPlan *o) const;
	virtual QueryPlan *copy(XPath2MemoryManager *mm = 0) const;
	virtual std::string printQueryPlan(const DynamicContext *context, int indent) const;
	virtual std::string toString(bool brief = true) const;

protected:
	PresenceQP(QueryPlan::Type qpType, ImpliedSchemaNode::Type type,
		   const char *parent, const char *child,
		   const QPKey &key, DbWrapper::Operation operation,
		   XPath2MemoryManager *mm);
	QueryPlan *rootPresenceCheck(const Container &container, const IndexSpecification &is,
		bool nodeQP, bool &success, bool &exact);

	ImpliedSchemaNode::Type nodeType_;
	const char *parentUriName_;
	const char *childUriName_;

	QPKey key_;
	DbWrapper::Operation operation_;
};

class ValueQP : public PresenceQP
{
public:
	ValueQP(ImpliedSchemaNode::Type type,
		const char *parent, const char *child,
		DbWrapper::Operation operation, bool generalComp,
		Syntax::Type syntax, const ASTNode *value,
		XPath2MemoryManager *mm);
	ValueQP(ImpliedSchemaNode::Type type,
		const char *parent, const char *child,
		Syntax::Type syntax, DbWrapper::Operation operation,
		bool generalComp, const char *value, size_t vlen,
		XPath2MemoryManager *mm);
	ValueQP(QueryPlan::Type qpType, ImpliedSchemaNode::Type type,
		const char *parent, const char *child,
		const QPKey &key, DbWrapper::Operation operation,
		const QPValue &value,
		XPath2MemoryManager *mm);

	virtual QueryPlan *staticTyping(StaticContext *context);

	virtual void getKeys(IndexLookups &keys, DbXmlContext *context) const;
	virtual void getKeysForCost(IndexLookups &keys, DbXmlContext *context) const;

	virtual QueryPlan *resolveValues(const Log &log, DbXmlContext *context);
	virtual QueryPlan *resolveSyntax(const Log &log, DbXmlContext *context);
	virtual QueryPlan *resolveIndexes(const Container &container, const IndexSpecification &indexSpecification,
		bool nodeQP, bool &success, bool &exact);

	virtual QueryPlan *removeNonConstant();

	virtual bool isSubsetOf(const QueryPlan *o) const;
	virtual QueryPlan *copy(XPath2MemoryManager *mm = 0) const;
	virtual std::string printQueryPlan(const DynamicContext *context, int indent) const;
	virtual std::string toString(bool brief = true) const;

	const QPValue &getValue() const {
		return value_;
	}

protected:
	bool isSubsetOfValue(const PresenceQP *step, DbWrapper::Operation myOp, DbWrapper::Operation hisOp) const;
	void getKeysImpl(IndexLookups &keys, const char *value, size_t length, int timezone) const;
	QueryPlan *resolveFromResult(Result &result, const Log &log, DbXmlContext *context, const LocationInfo *location);

	QPValue value_;
};

class RangeQP : public ValueQP
{
public:
	RangeQP(const ValueQP *gt, const ValueQP *lt, XPath2MemoryManager *mm);

	DbWrapper::Operation getOperation2() const { return operation2_; }

	virtual QueryPlan *staticTyping(StaticContext *context);

	virtual IDS::Ptr execute(OperationContext &context, QueryExecutionContext &qec) const;
	virtual IndexData::Ptr nodes_execute(OperationContext &context, QueryExecutionContext &qec) const;
	virtual Cost cost(OperationContext &context, QueryExecutionContext &qec) const;

	virtual QueryPlan *resolveIndexes(const Container &container, const IndexSpecification &indexSpecification,
		bool nodeQP, bool &success, bool &exact);

	virtual bool isSubsetOf(const QueryPlan *o) const;
	virtual QueryPlan *copy(XPath2MemoryManager *mm = 0) const;
	virtual std::string printQueryPlan(const DynamicContext *context, int indent) const;
	virtual std::string toString(bool brief = true) const;

	const QPValue &getValue2() const {
		return value2_;
	}

protected:
	RangeQP(ImpliedSchemaNode::Type type,
		const char *parent, const char *child,
		const QPKey &key1, DbWrapper::Operation operation1,
		const QPValue &value1, DbWrapper::Operation operation2,
		const QPValue &value2,
		XPath2MemoryManager *mm);

	QPValue value2_;
	DbWrapper::Operation operation2_;
};

class DocumentQP : public ValueQP
{
public:
	DocumentQP(const std::string &documentName, QueryPlan *qp, XPath2MemoryManager *mm);

	virtual QueryPlan *staticTyping(StaticContext *context);

	virtual IDS::Ptr execute(OperationContext &context, QueryExecutionContext &qec) const;
	virtual IndexData::Ptr nodes_execute(OperationContext &context, QueryExecutionContext &qec) const;
	virtual Cost cost(OperationContext &context, QueryExecutionContext &qec) const;

	virtual QueryPlan *compress();
	virtual QueryPlan *dissolve();
	virtual QueryPlan *rootFilter(const ImpliedSchemaNode *root);
	virtual QueryPlan *nodeFilter(const ImpliedSchemaNode::Vector &nodes);
	virtual void findQueryPlanHolders(QPHSet &qphset) const;
	virtual QueryPlan *resolvePaths();
	virtual QueryPlan *removeSubsets(const Log &log);
	virtual QueryPlan *removeNonConstant();
	virtual QueryPlan *resolveValues(const Log &log, DbXmlContext *context);
	virtual QueryPlan *resolveSyntax(const Log &log, DbXmlContext *context);
	virtual QueryPlan *resolveIndexes(const Container &container, const IndexSpecification &indexSpecification,
		bool nodeQP, bool &success, bool &exact);

	virtual bool isSubsetOf(const QueryPlan *o) const { return false; }
	virtual QueryPlan *copy(XPath2MemoryManager *mm = 0) const;
	virtual std::string printQueryPlan(const DynamicContext *context, int indent) const;
	virtual std::string toString(bool brief = true) const;

	QueryPlan *getArgument() { return qp_; }
	void setArgument(QueryPlan *arg) { qp_ = arg; }

private:
	DocumentQP(const QPKey &key, const QPValue &value, QueryPlan *qp,
		   XPath2MemoryManager *mm);

	QueryPlan *qp_;
};

class UniverseQP : public PresenceQP
{
public:
	UniverseQP(XPath2MemoryManager *mm);

	virtual IndexData::Ptr nodes_execute(OperationContext &context, QueryExecutionContext &qec) const;

	virtual bool isSubsetOf(const QueryPlan *o) const;
	virtual QueryPlan *copy(XPath2MemoryManager *mm = 0) const;
	virtual std::string printQueryPlan(const DynamicContext *context, int indent) const;
	virtual std::string toString(bool brief = true) const;
};

class EmptyQP : public StepQP
{
public:
	EmptyQP(XPath2MemoryManager *mm)
		: StepQP(QueryPlan::EMPTY, mm) {}

	virtual QueryPlan *staticTyping(StaticContext *context) { return this; }

	virtual IDS::Ptr execute(OperationContext &context, QueryExecutionContext &qec) const;
	virtual IndexData::Ptr nodes_execute(OperationContext &context, QueryExecutionContext &qec) const;
	virtual Cost cost(OperationContext &context, QueryExecutionContext &qec) const { return Cost(); }

	virtual bool isSubsetOf(const QueryPlan *o) const;
	virtual QueryPlan *copy(XPath2MemoryManager *mm = 0) const;
	virtual std::string printQueryPlan(const DynamicContext *context, int indent) const;
	virtual std::string toString(bool brief = true) const;
};

}

#endif
