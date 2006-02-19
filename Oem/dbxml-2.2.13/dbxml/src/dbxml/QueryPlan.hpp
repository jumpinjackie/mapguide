//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: QueryPlan.hpp,v 1.89 2005/11/28 13:13:20 jsnelson Exp $
//

#ifndef __QUERYPLAN_HPP
#define	__QUERYPLAN_HPP

#include <vector>
#include <string>
#include <pathan/XPath2MemoryManager.hpp>

#include "ImpliedSchemaNode.hpp"
#include "Key.hpp"
#include "IndexEntry.hpp"
#include "db_cxx.h"

namespace DbXml
{

class ValueQP;
class KeyStatistics;
class Log;
class QueryExecutionContext;

class QueryPlan
{
public:
	virtual ~QueryPlan() {}
	typedef enum {
		UNION,
		INTERSECT,
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
	};

	QueryPlan(Type t, XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *mm)
		: memMgr_(mm), type_(t) {}

	Type getType() const
	{
		return type_;
	}

	virtual IDS::SharedPtr execute(OperationContext &context, QueryExecutionContext &qec) const = 0;
	virtual IndexData::SharedPtr nodes_execute(OperationContext &context, QueryExecutionContext &qec) const = 0;
	virtual Cost cost(OperationContext &context, QueryExecutionContext &qec) const = 0;

	QueryPlan *finalizeGlobalRawQueryPlan();
	QueryPlan *createLocalRawQueryPlan(const ImpliedSchemaNode *root, const Log &log,
					   DynamicContext *context, bool partiallyOptimize) const;
	QueryPlan *createExecutableQueryPlan(Transaction *txn, const Container &container,
		DynamicContext *context, bool nodeQP, bool lookupValues, bool &success) const;

	/// Removes duplicate (by pointer comparison) and dissolves
	virtual QueryPlan *compress() = 0;
	/** returns 0 if it is a union or intersect, and it has no children.
	    returns the child if it has only one child. Otherwise returns this. */
	virtual QueryPlan *dissolve() = 0;
	/** Removes paths from the tree that do not come from the ImpliedSchemaNode
	    tree rooted at root. */
	virtual QueryPlan *filter(const ImpliedSchemaNode *root) = 0;
	/** Changes all PathsQP objects into StepQP objects. */
	virtual QueryPlan *resolvePaths() = 0;
	/** Uses isSubsetOf to compare arguments of union
	    and intersect operations, and remove unneeded ones, then dissolves.*/
	virtual QueryPlan *removeSubsets(const Log &log) = 0;
	/** Resolves all DataItem values of ValueQP to be the actual values. success should
	    be set to true before calling, and will be set to false if the method fails. The
	    method should only fail if lookupValues is false. */
	virtual QueryPlan *resolveValues(const Log &log, DynamicContext *context, bool lookupValues, bool &success) = 0;
	/** Resolves the index lookup information for all StepQP objects. */
	virtual QueryPlan *resolveSteps(const Container &container, const IndexSpecification &indexSpecification, bool nodeQP) = 0;

	/// Returns true if it's sure. Returns false if it doesn't know
	virtual bool isSubsetOf(const QueryPlan *o) const = 0;
	virtual QueryPlan *copy(XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *mm = 0) const = 0;
	virtual std::string toString(bool brief = true) const = 0;

protected:
	void logLegend(const Log &log) const;
	void logIDs(QueryExecutionContext &qec, const IDS::SharedPtr &ids) const;
	void logIndexData(QueryExecutionContext &qec, const IndexData::SharedPtr &data) const;
	void logCost(QueryExecutionContext &qec, Cost &cost, const KeyStatistics *stats) const;
	void logQP(const Log &log, const std::string &title, const QueryPlan *qp) const;

	XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *memMgr_;
	Type type_;
};

class OperationQP : public QueryPlan
{
public:
	typedef std::vector<QueryPlan*,PathanAllocator<QueryPlan*> > Vector;

	OperationQP(QueryPlan::Type type, XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *mm)
		: QueryPlan(type, mm), args_(PathanAllocator<QueryPlan*>(mm)) {}
	virtual ~OperationQP() {}

	const Vector &getArgs() const
	{
		return args_;
	}
	QueryPlan *addArg(QueryPlan *o);

	virtual QueryPlan *compress();
	virtual QueryPlan *dissolve();
	virtual QueryPlan *filter(const ImpliedSchemaNode *root);
	virtual QueryPlan *resolvePaths();
	virtual QueryPlan *resolveValues(const Log &log, DynamicContext *context, bool lookupValues, bool &success);
	virtual QueryPlan *resolveSteps(const Container &container, const IndexSpecification &indexSpecification, bool nodeQP);

	virtual bool isSupersetOf(const QueryPlan *o) const = 0;

protected:
	void logSubset(const Log &log, const QueryPlan *subset, const QueryPlan *superset) const;
	void logMerge(const Log &log, const QueryPlan *l, const QueryPlan *r, const QueryPlan *result) const;

	Vector args_;
};

class UnionQP : public OperationQP
{
public:
	UnionQP(XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *mm)
		: OperationQP(QueryPlan::UNION, mm) {}

	UnionQP(QueryPlan *l, QueryPlan *r, XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *mm);

	virtual IDS::SharedPtr execute(OperationContext &context, QueryExecutionContext &qec) const;
	virtual IndexData::SharedPtr nodes_execute(OperationContext &context, QueryExecutionContext &qec) const;
	virtual Cost cost(OperationContext &context, QueryExecutionContext &qec) const;

	virtual QueryPlan *removeSubsets(const Log &log);

	virtual bool isSubsetOf(const QueryPlan *o) const;
	virtual bool isSupersetOf(const QueryPlan *o) const;
	virtual QueryPlan *copy(XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *mm = 0) const;
	virtual std::string toString(bool brief = true) const;
};

class IntersectQP : public OperationQP
{
public:
	IntersectQP(XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *mm)
		: OperationQP(QueryPlan::INTERSECT, mm) {}

	IntersectQP(QueryPlan *l, QueryPlan *r, XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *mm);

	virtual IDS::SharedPtr execute(OperationContext &context, QueryExecutionContext &qec) const;
	virtual IndexData::SharedPtr nodes_execute(OperationContext &context, QueryExecutionContext &qec) const;
	virtual Cost cost(OperationContext &context, QueryExecutionContext &qec) const;

	virtual QueryPlan *removeSubsets(const Log &log);

	virtual bool isSubsetOf(const QueryPlan *o) const;
	virtual bool isSupersetOf(const QueryPlan *o) const;
	virtual QueryPlan *copy(XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *mm = 0) const;
	virtual std::string toString(bool brief = true) const;

private:
	QueryPlan *createRange(const ValueQP *l, const ValueQP *r);
};

class PathsQP : public QueryPlan
{
public:
	typedef std::vector<ImpliedSchemaNode*,PathanAllocator<ImpliedSchemaNode*> > Paths;

	PathsQP(XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *mm)
		: QueryPlan(QueryPlan::PATHS, mm), paths_(PathanAllocator<ImpliedSchemaNode*>(mm)) {}

	PathsQP(const ImpliedSchemaNode::Vector &paths, XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *mm)
		: QueryPlan(QueryPlan::PATHS, mm), paths_(PathanAllocator<ImpliedSchemaNode*>(mm))
	{
		paths_.insert(paths_.end(), paths.begin(), paths.end());
	}
	virtual ~PathsQP() {}

	virtual IDS::SharedPtr execute(OperationContext &context, QueryExecutionContext &qec) const { return IDS::SharedPtr(); }
	virtual IndexData::SharedPtr nodes_execute(OperationContext &context, QueryExecutionContext &qec) const { return IndexData::SharedPtr(); }
	virtual Cost cost(OperationContext &context, QueryExecutionContext &qec) const { return Cost(); }

	const Paths &getPaths() const { return paths_; }
	void addPaths(const Paths &o);

	/// Return true if all the paths come from the same tree
	bool sameTree() const;

	virtual QueryPlan *compress();
	virtual QueryPlan *dissolve();
	virtual QueryPlan *filter(const ImpliedSchemaNode *root);
	virtual QueryPlan *resolvePaths();
	virtual QueryPlan *removeSubsets(const Log &log) { return this; }
	virtual QueryPlan *resolveValues(const Log &log, DynamicContext *context, bool lookupValues, bool &success) { return this; }
	virtual QueryPlan *resolveSteps(const Container &container, const IndexSpecification &indexSpecification, bool nodeQP) { return this; }

	virtual bool isSubsetOf(const QueryPlan *o) const { return false; }
	virtual QueryPlan *copy(XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *mm = 0) const;
	virtual std::string toString(bool brief = true) const;

protected:
	Paths paths_;
};

class StepQP : public QueryPlan
{
public:
	StepQP(QueryPlan::Type type, XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *mm)
		: QueryPlan(type, mm) {}

	virtual QueryPlan *compress();
	virtual QueryPlan *dissolve();
	virtual QueryPlan *filter(const ImpliedSchemaNode *root);
	virtual QueryPlan *resolvePaths();
	virtual QueryPlan *removeSubsets(const Log &log);
	virtual QueryPlan *resolveValues(const Log &log, DynamicContext *context, bool lookupValues, bool &success);
	virtual QueryPlan *resolveSteps(const Container &container, const IndexSpecification &indexSpecification, bool nodeQP);

	static QueryPlan *createStep(const ImpliedSchemaNode *path, XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *mm);
	static bool getStepNames(const ImpliedSchemaNode *child, char *&parentName, char *&childName, XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *mm);

protected:
	void logIndexUse(const Log &log, const Index &index, const DbWrapper::Operation op) const;
	void logTransformation(const Log &log, const QueryPlan *transformed) const;
};

class QPValue {
public:
	QPValue(Syntax::Type syntax, const char *value, size_t vlen);
	QPValue(const DataItem *di);

	Syntax::Type getSyntax() const {
		return syntax_;
	}
	const char *getValue() const {
		return value_;
	}
	size_t getLength() const {
		return len_;
	}
	const DataItem *getDataItem() const {
		return di_;
	}
	std::string asString() const {
		if(di_ != 0) return "[to be calculated]";
		else return value_ == 0 ? "" : std::string(value_, len_);
	}
	bool equals(const QPValue &o) const;

private:
	Syntax::Type syntax_;
	const char *value_;
	size_t len_;
	const DataItem *di_;
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
	const ID &getID1() const {
		return id1_;
	}
	void setID1(const ID &id) {
		id1_ = id;
	}
	const ID &getID2() const {
		return id2_;
	}
	void setID2(const ID &id) {
		id2_ = id;
	}

	Key createKey() const;
	Key createKey(const QPValue &value) const;

private:
	bool nodeLookup_;
	Index index_;
	ID id1_;
	ID id2_;
};

class PresenceQP : public StepQP
{
public:
	PresenceQP(ImpliedSchemaNode::Type type,
		   const char *parent, const char *child,
		   XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *mm);

	const char *getParentName() const { return parentUriName_; }
	const char *getChildName() const { return childUriName_; }
	bool isParentSet() const { return parentUriName_ != 0; }
	ImpliedSchemaNode::Type getNodeType() const { return nodeType_; }
	DbWrapper::Operation getOperation() const { return operation_; }
	const QPKey &getKey() const { return key_; }

	virtual IDS::SharedPtr execute(OperationContext &context, QueryExecutionContext &qec) const;
	virtual IndexData::SharedPtr nodes_execute(OperationContext &context, QueryExecutionContext &qec) const;
	virtual Cost cost(OperationContext &context, QueryExecutionContext &qec) const;

	virtual QueryPlan *resolveSteps(const Container &container, const IndexSpecification &indexSpecification, bool nodeQP);

	virtual bool isSubsetOf(const QueryPlan *o) const;
	virtual QueryPlan *copy(XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *mm = 0) const;
	virtual std::string toString(bool brief = true) const;

protected:
	PresenceQP(QueryPlan::Type qpType, ImpliedSchemaNode::Type type,
		   const char *parent, const char *child,
		   const QPKey &key, DbWrapper::Operation operation,
		   XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *mm);
	QueryPlan *rootPresenceCheck(const Container &container, const IndexSpecification &is, bool nodeQP);

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
		DbWrapper::Operation operation, const DataItem *value,
		XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *mm);
	ValueQP(ImpliedSchemaNode::Type type,
		const char *parent, const char *child,
		Syntax::Type syntax, DbWrapper::Operation operation,
		const char *value, size_t vlen,
		XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *mm);

	virtual IDS::SharedPtr execute(OperationContext &context, QueryExecutionContext &qec) const;
	virtual IndexData::SharedPtr nodes_execute(OperationContext &context, QueryExecutionContext &qec) const;
	virtual Cost cost(OperationContext &context, QueryExecutionContext &qec) const;

	virtual QueryPlan *resolveValues(const Log &log, DynamicContext *context, bool lookupValues, bool &success);
	virtual QueryPlan *resolveSteps(const Container &container, const IndexSpecification &indexSpecification, bool nodeQP);

	virtual bool isSubsetOf(const QueryPlan *o) const;
	virtual QueryPlan *copy(XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *mm = 0) const;
	virtual std::string toString(bool brief = true) const;

	const QPValue &getValue() const {
		return value_;
	}

protected:
	ValueQP(QueryPlan::Type qpType, ImpliedSchemaNode::Type type,
		const char *parent, const char *child,
		const QPKey &key, DbWrapper::Operation operation,
		const QPValue &value,
		XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *mm);
	QueryPlan *splitSubstring(const Container &container, const IndexSpecification &is, bool nodeQP);
	bool isSubsetOfValue(const PresenceQP *step, DbWrapper::Operation myOp, DbWrapper::Operation hisOp) const;

	QPValue value_;
};

class RangeQP : public ValueQP
{
public:
	RangeQP(const ValueQP *gt, const ValueQP *lt, XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *mm);

	DbWrapper::Operation getOperation2() const { return operation2_; }

	virtual IDS::SharedPtr execute(OperationContext &context, QueryExecutionContext &qec) const;
	virtual IndexData::SharedPtr nodes_execute(OperationContext &context, QueryExecutionContext &qec) const;
	virtual Cost cost(OperationContext &context, QueryExecutionContext &qec) const;

	virtual QueryPlan *resolveSteps(const Container &container, const IndexSpecification &indexSpecification, bool nodeQP);

	virtual bool isSubsetOf(const QueryPlan *o) const;
	virtual QueryPlan *copy(XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *mm = 0) const;
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
		XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *mm);

	QPValue value2_;
	DbWrapper::Operation operation2_;
};

class DocumentQP : public ValueQP
{
public:
	DocumentQP(const std::string documentName, QueryPlan *qp, XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *mm);

	virtual IDS::SharedPtr execute(OperationContext &context, QueryExecutionContext &qec) const;
	virtual IndexData::SharedPtr nodes_execute(OperationContext &context, QueryExecutionContext &qec) const;
	virtual Cost cost(OperationContext &context, QueryExecutionContext &qec) const;

	virtual QueryPlan *compress();
	virtual QueryPlan *dissolve();
	virtual QueryPlan *filter(const ImpliedSchemaNode *root);
	virtual QueryPlan *resolvePaths();
	virtual QueryPlan *removeSubsets(const Log &log);
	virtual QueryPlan *resolveValues(const Log &log, DynamicContext *context, bool lookupValues, bool &success);
	virtual QueryPlan *resolveSteps(const Container &container, const IndexSpecification &indexSpecification, bool nodeQP);

	virtual bool isSubsetOf(const QueryPlan *o) const { return false; }
	virtual QueryPlan *copy(XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *mm = 0) const;
	virtual std::string toString(bool brief = true) const;

private:
	DocumentQP(const QPKey &key, const QPValue &value, QueryPlan *qp,
		   XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *mm);

	QueryPlan *qp_;
};

class UniverseQP : public PresenceQP
{
public:
	UniverseQP(XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *mm);

	virtual IndexData::SharedPtr nodes_execute(OperationContext &context, QueryExecutionContext &qec) const;

	virtual bool isSubsetOf(const QueryPlan *o) const;
	virtual QueryPlan *copy(XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *mm = 0) const;
	virtual std::string toString(bool brief = true) const;
};

class EmptyQP : public StepQP
{
public:
	EmptyQP(XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *mm)
		: StepQP(QueryPlan::EMPTY, mm) {}

	virtual IDS::SharedPtr execute(OperationContext &context, QueryExecutionContext &qec) const;
	virtual IndexData::SharedPtr nodes_execute(OperationContext &context, QueryExecutionContext &qec) const;
	virtual Cost cost(OperationContext &context, QueryExecutionContext &qec) const { return Cost(); }

	virtual bool isSubsetOf(const QueryPlan *o) const;
	virtual QueryPlan *copy(XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *mm = 0) const;
	virtual std::string toString(bool brief = true) const;
};

}

#endif
